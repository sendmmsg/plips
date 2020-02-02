#define _GNU_SOURCE
#include "plips.h"
#include "encodings/utf8.h"
#include "linenoise.h"
#include "plips_reader.h"
#include "plips_core.h"
#include <czmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// TODO:
// M-x c-guess-no-install
// M-x c-guess-view
plips_val *plips_error = NULL;
plips_env *repl_env;

typedef struct _plips_fn_t
{
    // function pointer
    char *name;
    plips_val *(*fn);
    // number of arguments accepted
    int num_args;
} plips_fn_t;

plips_val *APPLY(plips_val *ast) {
    plips_val *fn;
    plips_val *a[20];
    memset(a, 0, 20 * sizeof(plips_val *));
    fn = plips_val_list_first(ast);
    int num_args = fn->func_args;
    int got_args = zlistx_size(ast->val.list) - 1;
    if (num_args != got_args && num_args != -1) {
        plips_error = malloc(sizeof(plips_val));
        plips_error->type = PLIPS_STRING;
        asprintf(&plips_error->val.str, "Error: procedure expected %d arguments, got %d", num_args, got_args);
        return NULL;
    }

    if(num_args == -1){
        return fn->val.f1(ast);
    }

    for (int i = 0; i < num_args; i++) {
        a[i] = plips_val_list_next(ast);
        //        printf("APPLY: a[%d] = %s\n", i, plips_val_tostr(a[i], 1));
    }

    switch (num_args) {
        case 0:
            return fn->val.f0();
        case 1:
            return fn->val.f1(a[0]);
        case 2:
            return fn->val.f2(a[0], a[1]);
        case 3:
            return fn->val.f3(a[0], a[1], a[2]);
        case 4:
            return fn->val.f4(a[0], a[1], a[2], a[3]);
        case 5:
            return fn->val.f5(a[0], a[1], a[2], a[3], a[4]);
        case 6:
            return fn->val.f6(a[0], a[1], a[2], a[3], a[4], a[5]);
        case 7:
            return fn->val.f7(a[0], a[1], a[2], a[3], a[4], a[5], a[6]);
        case 8:
            return fn->val.f8(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
        case 9:
            return fn->val.f9(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8]);
        case 10:
            return fn->val.f10(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9]);
        case 11:
            return fn->val.f11(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10]);
        case 12:
            return fn->val.f12(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11]);
        case 13:
            return fn->val.f13(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11],
                               a[12]);
        case 14:
            return fn->val.f14(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11],
                               a[12], a[13]);
        case 15:
            return fn->val.f15(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11],
                               a[12], a[13], a[14]);
        case 16:
            return fn->val.f16(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11],
                               a[12], a[13], a[14], a[15]);
        case 17:
            return fn->val.f17(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11],
                               a[12], a[13], a[14], a[15], a[16]);
        case 18:
            return fn->val.f18(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11],
                               a[12], a[13], a[14], a[15], a[16], a[17]);
        case 19:
            return fn->val.f19(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11],
                               a[12], a[13], a[14], a[15], a[16], a[17], a[18]);
        case 20:
            return fn->val.f20(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11],
                               a[12], a[13], a[14], a[15], a[16], a[17], a[18], a[19]);
    }

    return ast;
}

plips_val *EVAL(plips_val *ast, plips_env *env);

plips_env *parse_let_2(plips_env *env, plips_val *item) {
    if (plips_val_list_len(item) == 2) {
        plips_val *symb = plips_val_list_first(item);
        plips_val *val = plips_val_list_next(item);
        if (symb == NULL || val == NULL) {
            plips_error = plips_val_string_new("Error: let* syntax error in binding");
            return NULL;
        }
        if (symb->type != PLIPS_SYMBOL) {
            plips_error = malloc(sizeof(plips_val));
            plips_error->type = PLIPS_STRING;
            asprintf(&plips_error->val.str, "Error: let*, %s is not a symbol", symb->val.str);
            return NULL;
        }
        plips_val *ret = EVAL(val, env);
        plips_env_set(env, symb->val.str, ret);
        return env;
    }
    plips_error = plips_val_string_new("Error: let* syntax error");
    return NULL;
}

// (a 1)
// ((a 1) (b 2))
// ((a 1) (b 2) (c 3))
//
plips_env *parse_let(plips_env *env, plips_val *item) {
    if (env == NULL) {
        env = plips_env_new();
    }
    plips_val *symb = plips_val_list_first(item);
    if (symb->type == PLIPS_LIST) {
        while (symb) {
            if (symb->type == PLIPS_LIST) {
                env = parse_let_2(env, symb);
                symb = plips_val_list_next(item);
            } else {
                plips_error = plips_val_string_new("Error: let* syntax error");
                plips_env_destroy(&env);
                return NULL;
            }
        }
        return env;
        // iterate and call parse_let_2 on all items
    } else if (symb->type == PLIPS_SYMBOL) {
        env = parse_let_2(env, item);
        return env;
    }
    plips_error = plips_val_string_new("Error: let* syntax error");
    return NULL;
}


plips_val *eval_ast(plips_val *ast, plips_env *env) {
    plips_val *ret;
    plips_val *item;
    plips_env *new_env;
    switch (ast->type) {
        case PLIPS_ATOM:
            //    printf("looking up atom %s\n", ast->val.str);
            ret = plips_env_get(env, ast->val.str);
            if (ret == NULL) {
                plips_error = malloc(sizeof(plips_val));
                plips_error->type = PLIPS_STRING;
                asprintf(&plips_error->val.str, "Error: atom %s not found", ast->val.str);
            }

            return ret;
            break;
        case PLIPS_SYMBOL:
            //    printf("looking up symbol %s\n", ast->val.str);
            ret = plips_env_get(env, ast->val.str);
            if (ret == NULL) {
                plips_error = malloc(sizeof(plips_val));
                plips_error->type = PLIPS_STRING;
                asprintf(&plips_error->val.str, "Error: symbol %s not found", ast->val.str);
            }
            return ret;
        case PLIPS_LIST:
            item = plips_val_list_first(ast);
            if (item->type == PLIPS_SYMBOL) {
                if (streq(item->val.str, "def!") || streq(item->val.str, "define")) {
                    if (plips_val_list_len(ast) != 3) {
                        plips_error = plips_val_string_new("Error: def! requires 2 arguments");
                        return NULL;
                    }
                    item = plips_val_list_next(ast);
                    if (item->type != PLIPS_SYMBOL) {
                        plips_error = plips_val_string_new("Error: def! 1st argument must be a symbol!");
                        return NULL;
                    }
                    ret = EVAL(plips_val_list_next(ast), env);
                    plips_env_set(env, item->val.str, ret);
                    return ret;
                } else if (streq(item->val.str, "let*")) {
                    if (plips_val_list_len(ast) != 3) {
                        plips_error = plips_val_string_new("Error: let* requires an argument");
                        return NULL;
                    }
                    item = plips_val_list_first(ast);
                    item = plips_val_list_next(ast);
                    if (item == NULL) {
                        plips_error = plips_val_string_new("Error: let* requires an argument");
                        return NULL;
                    }
                    if (item->type != PLIPS_LIST) {
                        plips_val_print(item, 1);
                        plips_error = plips_val_string_new("Error: let* requires a list as first argument");
                        return NULL;
                    }
                    new_env = parse_let(NULL, item);

                    plips_env_set_outer(new_env, env);
                    ret = EVAL(plips_val_list_next(ast), new_env);
                    plips_env_set(new_env, item->val.str, ret);
                    return ret;
                } else if (streq(item->val.str, "fn*") || streq(item->val.str, "lambda")) {
                    printf("not implemented yet\n");
                    return ast;
                } else if (streq(item->val.str, "if")) {
                    if(plips_val_list_len(ast) != 4){
                        plips_error = plips_val_string_new("Error: if requires three arguments");
                        return NULL;
                    }
                    plips_val *cond = plips_val_list_nth(ast,1);
                    ret = EVAL(cond, env);
                    if(ret == NULL){
                        plips_error = plips_val_string_new("Error: if conditional returned NULL");
                        return NULL;
                    }
                    if(ret == &plips_false || ret == &plips_nil){
                        return EVAL(plips_val_list_nth(ast,3), env);
                    }
                    return EVAL(plips_val_list_nth(ast,2), env);
                }
            }
            ret = plips_val_list_new();
            while (item != NULL) {
                plips_val *new_item = EVAL(item, env);
                if (new_item == NULL)
                    return NULL;
                plips_val_list_append(ret, new_item);
                item = plips_val_list_next(ast);
            }


            item = plips_val_list_first(ret);
            if (item->type != PLIPS_FN_C) {
                plips_error = malloc(sizeof(plips_val));
                plips_error->type = PLIPS_STRING;
                char *type = plips_val_tostr(item, 1);
                asprintf(&plips_error->val.str, "Error: syntax error, cannot execute: %s", type);
                free(type);
                return NULL;
            }
            return APPLY(ret);

            return ret;
            break;
        default:
            //    printf("eval_ast: type %d not handled, return ast\n", ast->type);
            return ast;
            break;
    }
}

plips_val *EVAL(plips_val *ast, plips_env *env) {
    /* char *ast_str = plips_val_tostr(ast, 1); */
    /* printf("EVAL %s\n", ast_str); */
    /* free(ast_str); */
    if (!ast || plips_error)
        return NULL;
    if (ast->type != PLIPS_LIST)
        return eval_ast(ast, env);

    if (ast->type == PLIPS_LIST) {
        if (zlistx_size(ast->val.list) == 0)
            return ast;
        plips_val *new_list = eval_ast(ast, env);
        return new_list;
    }

    return NULL;
}

char *PRINT(plips_val *ast) {
    if (plips_error)
        return NULL;
    return plips_val_tostr(ast, 0);
}

plips_val *READ(char *command, int verbose) {
    //    printf("READ: %s\n", command);
    plips_val *ast = reader_str(command, verbose);
    //    plips_val_print(ast, 1);
    return ast;
}

plips_val *RE(plips_env *env, char *command, int verbose) {
    plips_val *ast = READ(command, verbose);
    if (ast == NULL || plips_error)
        return NULL;
    plips_val *exp = EVAL(ast, env);
    //  printf("EVAL returned %p, plips_error = %p\n", exp,plips_error);

    if (ast != exp) {
        // TODO: free *ast here
    }
    return exp;
}



void completion(const char *buf, linenoiseCompletions *lc) {
    char *lparen = strrchr(buf, '(');
    printf("lparen = %s\n", lparen);
    if (lparen == buf || lparen == NULL) {
        if (streq(buf, "(")) {
            linenoiseAddCompletion(lc, "(+ a b)");
            linenoiseAddCompletion(lc, "(- a b)");
            linenoiseAddCompletion(lc, "(* a b)");
            linenoiseAddCompletion(lc, "(/ a b)");
        }
    } else if (lparen) {
        if (streq(buf, "(+")) {
            linenoiseAddCompletion(lc, "(+ a b)");
        }
        if (streq(buf, "(-")) {
            linenoiseAddCompletion(lc, "(- a b)");
        }
        if (streq(buf, "(*")) {
            linenoiseAddCompletion(lc, "(* a b)");
        }
        if (streq(buf, "(/")) {
            linenoiseAddCompletion(lc, "(/ a b)");
        }
    }
}

char *hints(const char *buf, int *color, int *bold) {
    if (!strcasecmp(buf, "(define ")) {
        *color = 35;
        *bold = 0;
        return " symbol form)";
    }
    if (!strcasecmp(buf, "(display")) {
        *color = 35;
        *bold = 0;
        return " fmt &data)";
    }
    return NULL;
}
int main(int argc, char **argv) {
    char *line;
    char *prgname = argv[0];
    int verbose = 0;
    while (argc > 1) {
        argc--;
        argv++;
        if (streq(*argv, "--multiline")) {
            linenoiseSetMultiLine(1);
            printf("Multi-line mode enabled.\n");
        } else if (streq(*argv, "--keycodes")) {
            linenoisePrintKeyCodes();
            exit(0);
        } else if (streq(*argv, "--verbose")) {
            verbose = 1;
        } else {
            fprintf(stderr, "Usage: %s [--multiline] [--keycodes]\n", prgname);
            exit(1);
        }
    }

    linenoiseSetEncodingFunctions(linenoiseUtf8PrevCharLen, linenoiseUtf8NextCharLen, linenoiseUtf8ReadCode);

    linenoiseSetCompletionCallback(completion);
    linenoiseSetHintsCallback(hints);

    linenoiseHistoryLoad("history.txt");
    char *output;
    repl_env = plips_setup_repl_env();

    while ((line = linenoise("😡\033[32mPLIPS\x1b[0m> ")) != NULL) {
        if (line[0] != '\0' && line[0] != '/') {
            linenoiseHistoryAdd(line);
            linenoiseHistorySave("history.txt");

            plips_val *exp = RE(repl_env, line, verbose);
            if (exp == NULL && plips_error) {
                plips_val_print(plips_error, 0);
                plips_val_destroy(&plips_error);
                continue;
            }

            // plips_val_tostr(exp, 1);
            if (plips_error && strcmp("EOF", plips_error->val.str) == 0) {
                plips_val_print(plips_error, 0);
                return 0;
            }
            output = PRINT(exp);
            if (plips_error) {
                fprintf(stderr, "Error: %s\n", plips_val_tostr(plips_error, 1));
                plips_error = NULL;
            } else if (output) {
                puts(output);
                free(output);
            }

        } else if (!strncmp(line, "/historylen", 11)) {
            int len = atoi(line + 11);
            linenoiseHistorySetMaxLen(len);
        } else if (line[0] == '/') {
            printf("Unreconized command: %s\n", line);
        }
        free(line);
    }
    return 0;
}
