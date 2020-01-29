#include "plips_reader.h"
#include <czmq.h>
#include <errno.h>
#include <pcre.h>

pcre *reCompiled;
pcre_extra *pcreExtra;
int pcreExecRet;
int subStrVec[30];
const char *pcreErrorStr;
int pcreErrorOffset;
const char *psubStrMatchStr;
int _plips_reader_init = 0;

struct _plips_reader_t
{
    zlistx_t *tokens;
    int num_tokens;
    int pos;
};

static void reader_init() {
    _plips_reader_init = 1;
    char *plRe = "[\\s ,]*(~@|[\\[\\]{}()'`~@]|\"(?:[\\\\].|[^\\\\\"])*\"?|;.*|[^\\s "
                 "\\[\\]{}()'\"`~@,;]*)";
    reCompiled = pcre_compile(plRe, PCRE_UTF8, &pcreErrorStr, &pcreErrorOffset, NULL);
    if (reCompiled == NULL) {
        printf("ERROR: Could not compile '%s': %s\n", plRe, pcreErrorStr);
        exit(1);
    }
    pcreExtra = pcre_study(reCompiled, 0, &pcreErrorStr);
    if (pcreErrorStr != NULL) {
        printf("ERROR: Could not study '%s': %s\n", plRe, pcreErrorStr);
        exit(1);
    }
}

void reader_destroy(plips_reader_t *self) {
    zlistx_destroy(&(self->tokens));
    free(self);
    self = NULL;
}

void token_destructor(void **string_p) {
    assert((char **) string_p);
    pcre_free_substring(*((char **) string_p));
    *string_p = NULL;
}

plips_reader_t *reader_new(char *input) {
    if (_plips_reader_init == 0)
        reader_init();

    plips_reader_t *self = malloc(sizeof(plips_reader_t));
    self->tokens = zlistx_new();
    zlistx_set_destructor(self->tokens, token_destructor);

    self->pos = 0;
    int len = strlen(input);
    int pos = 0;

    while (pos < len) {
        int ret = pcre_exec(reCompiled, pcreExtra, input, len, pos, 0, subStrVec, 30);
        if (ret < 0) {
            switch (ret) {
                case PCRE_ERROR_NOMATCH:
                    printf("String did not match the pattern\n");
                    break;
                case PCRE_ERROR_NULL:
                    printf("Something was null\n");
                    break;
                case PCRE_ERROR_BADOPTION:
                    printf("A bad option was passed\n");
                    break;
                case PCRE_ERROR_BADMAGIC:
                    printf("Magic number bad (compiled re corrupt?)\n");
                    break;
                case PCRE_ERROR_UNKNOWN_NODE:
                    printf("Something kooky in the compiled re\n");
                    break;
                case PCRE_ERROR_NOMEMORY:
                    printf("Ran out of memory\n");
                    break;
                default:
                    printf("Unknown error\n");
                    break;
            } /* end switch */
            fprintf(stderr, "parse error at pos: %d\nparsed: ", pos);
            reader_print(self);
            reader_destroy(self);
            return NULL;
        } else {
            // At this point, rc contains the number of substring matches found...
            if (ret == 0) {
                printf("But too many substrings were found to fit in subStrVec!\n");
                // Set rc to the max number of substring matches possible.
                ret = 30 / 3;
            }
            pcre_get_substring(input, subStrVec, ret, 1, &(psubStrMatchStr));
            // printf("substring \"%s\"\n",psubStrMatchStr);

            zlistx_add_end(self->tokens, (void *) psubStrMatchStr);
            pos = subStrVec[ret - 1];
            self->num_tokens++;
        }
    }
    zlistx_first(self->tokens);
    return self;
}

void reader_print(plips_reader_t *reader) {
    size_t size;
    char *ptr;
    FILE *ss;

    ss = open_memstream(&ptr, &size);
    if (ss == NULL) {
        perror("open_memstream: ");
        exit(EXIT_FAILURE);
    }

    char *item = zlistx_first(reader->tokens);
    fprintf(ss, "[");
    while (item) {
        fprintf(ss, " \"%s\" ", item);
        item = zlistx_next(reader->tokens);
    }
    fprintf(ss, "]\n");
    fclose(ss);
    printf("%s", ptr);
    free(ptr);
}


char *reader_next(plips_reader_t *reader) {
    char *item;
    if (reader->pos == 0) {
        item = zlistx_first(reader->tokens);
    } else {
        item = zlistx_next(reader->tokens);
    }
    reader->pos += 1;

    return item;
}

char *reader_peek(plips_reader_t *reader) {
    char *item;
    if (reader->pos == 0) {
        item = zlistx_first(reader->tokens);
    } else {
        item = zlistx_next(reader->tokens);
        zlistx_prev(reader->tokens);
    }
    return item;
}

plips_val *reader_form(plips_reader_t *r);


plips_val *read_list(plips_reader_t *r) {
    //    printf("read_list called\n");
    plips_val *newlist = plips_val_list_new();
    char *tok = reader_next(r);
    //    printf("Eating left par %s\n",tok);
    do {
        //        printf("read_list-%d\n", j++);
        plips_val_list_append(newlist, reader_form(r));
        tok = reader_peek(r);
        //        printf("read_list next \"%s\"", tok);
    } while (tok != NULL && tok[0] != ')');

    if (tok == NULL) {
        printf("SYNTAX ERROR: unbalanced parathesis\n");
    } else if (tok[0] == ')') {
        //        printf("Eating right par %s\n", reader_next(r));
    }
    return newlist;
}
// returns 0 on fail, 1 on success
int convert_to_integer(char *str, int64_t *dst) {
    char *p = str;
    errno = 0;
    *dst = 0;
    int64_t val = strtoul(str, &p, 10);
    if (errno != 0) {
        // conversion failed (EINVAL, ERANGE)
        goto hex;
    }
    if (str == p) {
        // conversion failed (no characters consumed)
        goto hex;
    }
    if (*p != 0) {
        // conversion failed (trailing data)
        goto hex;
    }
    *dst = val;
    return 1;
hex:
    if (strlen(str) < 3)
        return 0;
    if (str[0] != '0' || str[1] != 'x')
        return 0;
    errno = 0;
    val = strtoul(str, &p, 16);
    if (errno != 0) {
        // conversion failed (EINVAL, ERANGE)
        return 0;
    }
    if (str == p) {
        // conversion failed (no characters consumed)
        return 0;
    }
    if (*p != 0) {
        // conversion failed (trailing data)
        return 0;
    }
    *dst = val;
    return 1;
}

int convert_to_double(char *str, double *dst) {
    char *p = str;
    errno = 0;
    *dst = 0;
    double val = strtod(str, &p);
    if (errno != 0) {
        // conversion failed (EINVAL, ERANGE)
        return 0;
    }
    if (str == p) {
        // conversion failed (no characters consumed)
        return 0;
    }
    if (*p != 0) {
        // conversion failed (trailing data)
        return 0;
    }
    *dst = val;
    return 1;
}
int convert_to_string(char *str, char **dst) {
    size_t size;
    char *ptr;
    FILE *ss;
    int len = strlen(str) - 1;
    //    printf("c2s: start:%c end: %c\n", str[0], str[len]);
    if (str[0] != '"' || str[len] != '"')
        return 0;

    ss = open_memstream(&ptr, &size);
    if (ss == NULL) {
        perror("open_memstream: ");
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < len; i++) {
        if (str[i] == '\\' && str[i + 1] == '"') {
            fprintf(ss, "\"");
            i++;
        } else if (str[i] == '\\' && str[i + 1] == '\\') {
            fprintf(ss, "\\");
            i++;
        } else if (str[i] == '\\' && str[i + 1] == 'n') {
            fprintf(ss, "\n");
            i++;
        } else {
            fprintf(ss, "%c", str[i]);
        }
    }

    fclose(ss);
    //    printf("parsed string: '%s'\n", ptr);
    *dst = ptr;
    return 1;
}

plips_val *read_atom(plips_reader_t *r) {
    //    printf("read_atom called\n");
    plips_val *atom = malloc(sizeof(plips_val));

    char *item = reader_next(r);
    if (strlen(item) == 5 && strcmp("false", item) == 0) {
        atom->type = PLIPS_FALSE;
        return atom;
    }
    if (strlen(item) == 4 && strcmp("true", item) == 0) {
        atom->type = PLIPS_TRUE;
        return atom;
    }
    if (strlen(item) == 3 && strcmp("nil", item) == 0) {
        atom->type = PLIPS_NIL;
        return atom;
    }
    if (convert_to_integer(item, &atom->val.i64)) {
        atom->type = PLIPS_INT;
        return atom;
    } else if (convert_to_double(item, &atom->val.f64)) {
        atom->type = PLIPS_FLOAT;
        return atom;
    } else if (convert_to_string(item, &atom->val.str)) {
        atom->type = PLIPS_STRING;
        return atom;
    }

    // doesnt match anything else,  make it an atom
    atom->type = PLIPS_SYMBOL;
    atom->val.str = strdup(item);
    return atom;
}

plips_val *reader_form(plips_reader_t *r) {
    char *tok;
    do {
        tok = reader_peek(r);
    } while (tok != NULL && tok[0] == ';');

    if (tok == NULL) {
        printf("out of tokens\n");
        return NULL;
    } else if (tok[0] == '(') {
        return read_list(r);
    } else {
        return read_atom(r);
    }
}
plips_val *reader_str(char *command, int verbose) {
    plips_reader_t *r = reader_new(command);
    // reader_print(r);
    plips_val *t = reader_form(r);
    // type_print(t, verbose);
    //    printf("\n");
    reader_destroy(r);
    return t;
}
