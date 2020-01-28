#include "plips.h"
#include "encodings/utf8.h"
#include "linenoise.h"
#include "plips_reader.h"
#include <czmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// TODO:
// M-x c-guess-no-install
// M-x c-guess-view


plips_val plips_nil = {PLIPS_NIL, NULL, {0}, 0, 0};
plips_val plips_true = {PLIPS_TRUE, NULL, {0}, 0, 0};
plips_val plips_false = {PLIPS_FALSE, NULL, {0}, 0, 0};

plips_val *plips_error = NULL;
zhashx_t *repl_env;
typedef struct _plips_fn_t {
  // function pointer
  char *name;
  plips_val *(*fn);
  // number of arguments accepted
  int num_args;
} plips_fn_t;

plips_val *eval_ast(plips_val *ast, zhashx_t *env) {
  plips_val *ret;
  zlistx_t *new_list;
  switch (plips_type_get(ast)) {
  case PLIPS_ATOM:
    ret = zhashx_lookup(env, ast->val.str);
    if (ret == NULL) {
      plips_error = malloc(sizeof(plips_val));
      plips_error->type = PLIPS_STRING;
      sprintf(plips_error->val.str, "Error: symbol %s not found", ast->val.str);
    }
    return ret;
    break;
  case PLIPS_LIST:
    new_list = zlistx_new();

    break;
  default:
    return ast;
    break;
  }
}

plips_val *EVAL(plips_val *ast, zhashx_t *env) {
  if (!ast || plips_error)
    return NULL;
  if (ast->type != PLIPS_LIST)
    return eval_ast(ast, env);

  if (ast->type == PLIPS_LIST) {
    if (zlistx_size(ast->val.list) == 0)
      return ast;
    plips_val *new_list = eval_ast(ast, env);
    // todo, call new_list[0] with [1,2,3,4...] as arguments
  }

  return NULL;
}

char *PRINT(plips_val *ast) {
  if (plips_error)
    return NULL;
  return type_print(ast, 0);
}

plips_val *READ(char *command, int verbose) {
  plips_val *ast = reader_str(command, verbose);
  return ast;
}

plips_val *RE(zhashx_t *env, char *command, int verbose) {
  plips_val *ast = READ(command, verbose);
  if (ast == NULL || plips_error)
    return NULL;
  plips_val *exp = EVAL(ast, env);
  if (ast != exp) {
    // TODO: free *ast here
  }
  return exp;
}
void completion(const char *buf, linenoiseCompletions *lc) {
  if (buf[0] == '(') {
    linenoiseAddCompletion(lc, "(define symbol form)");
    linenoiseAddCompletion(lc, "(define symbol '(1 2))");
    linenoiseAddCompletion(lc, "hello こんにちは 👨‍💻");
  }
}

int64_t _plips_add(int64_t a, int64_t b) { return a + b; }
int64_t _plips_sub(int64_t a, int64_t b) { return a - b; }
int64_t _plips_mul(int64_t a, int64_t b) { return a * b; }
int64_t _plips_div(int64_t a, int64_t b) { return a / b; }



int insert_env_fn(char *name, int num_args, plips_val *(*fn), zhashx_t *env) {



  plips_fn_t *fun = malloc(sizeof(plips_fn_t));
  if (fun == NULL)
    return -1;
  fun->num_args = num_args;
  fun->fn = fn;
  fun->name = strdup(name);
  int ret = zhashx_insert(env, name, fun);
  return ret;
}

void setup_repl_env() {
  repl_env = zhashx_new();
  insert_env_fn("+", 2, &_plips_add, repl_env);
  insert_env_fn("-", 2, &_plips_sub, repl_env);
  insert_env_fn("/", 2, &_plips_mul, repl_env);
  insert_env_fn("*", 2, &_plips_div, repl_env);
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
    if (!strcmp(*argv, "--multiline")) {
      linenoiseSetMultiLine(1);
      printf("Multi-line mode enabled.\n");
    } else if (!strcmp(*argv, "--keycodes")) {
      linenoisePrintKeyCodes();
      exit(0);
    } else if (!strcmp(*argv, "--verbose")) {
      verbose = 1;
    } else {
      fprintf(stderr, "Usage: %s [--multiline] [--keycodes]\n", prgname);
      exit(1);
    }
  }

  linenoiseSetEncodingFunctions(linenoiseUtf8PrevCharLen,
                                linenoiseUtf8NextCharLen,
                                linenoiseUtf8ReadCode);

  /* Set the completion callback. This will be called every time the
   * user uses the <tab> key. */
  linenoiseSetCompletionCallback(completion);
  linenoiseSetHintsCallback(hints);

  /* Load history from file. The history file is just a plain text file
   * where entries are separated by newlines. */
  linenoiseHistoryLoad("history.txt"); /* Load the history at startup */

  /* Now this is the main loop of the typical linenoise-based application.
   * The call to linenoise() will block as long as the user types something
   * and presses enter.
   *
   * The typed string is returned as a malloc() allocated string by
   * linenoise, so the user needs to free() it. */
  char *output;
  setup_repl_env();

  while ((line = linenoise("\033[32mPLIPS\x1b[0m> ")) != NULL) {
    /* Do something with the string. */
    if (line[0] != '\0' && line[0] != '/') {
      // printf("echo: '%s'\n", line);
      plips_val *exp = RE(line, verbose);
      if (plips_error && strcmp("EOF", plips_error->val.str) == 0) {
        return 0;
      }
      output = PRINT(exp);
      if (plips_error) {
        fprintf(stderr, "Error: %s\n", type_print(plips_error, 1));
        // malval_free(mal_error);
        plips_error = NULL;
      } else if (output) {
        puts(output);
        free(output); // Free output string
      }
      linenoiseHistoryAdd(line);           /* Add to the history. */
      linenoiseHistorySave("history.txt"); /* Save the history on disk. */
    } else if (!strncmp(line, "/historylen", 11)) {
      /* The "/historylen" command will change the history len. */
      int len = atoi(line + 11);
      linenoiseHistorySetMaxLen(len);
    } else if (line[0] == '/') {
      printf("Unreconized command: %s\n", line);
    }
    free(line);
  }
  return 0;
}

void plips_val_destroy(plips_val **s) {
  plips_val **self = (plips_val **)s;
  assert(self);

  if ((*self)->type == PLIPS_LIST)
    zlistx_destroy(&(*self)->val.list);
  free(*self);
  *self = NULL;
}

// Allocate a malval and set its type and value
plips_val *plips_val_new(plips_type type, plips_val *metadata) {
  plips_val *pv = (plips_val *)malloc(sizeof(plips_val));
  pv->type = type;
  pv->metadata = metadata;
  return pv;
}

plips_val *plips_val_int_new(int64_t val) {
  plips_val *mv = plips_val_new(PLIPS_INT, NULL);
  mv->val.i64 = val;
  return mv;
}

plips_val *plips_val_float_new(double val) {
  plips_val *mv = plips_val_new(PLIPS_FLOAT, NULL);
  mv->val.f64 = val;
  return mv;
}

plips_val *plips_val_string_new(char *val) {
  plips_val *mv = plips_val_new(PLIPS_STRING, NULL);
  mv->val.str = val;
  return mv;
}

plips_val *plips_val_symbol_new(char *val) {
  plips_val *mv = plips_val_new(PLIPS_SYMBOL, NULL);
  mv->val.str = val;
  return mv;
}
plips_val *plips_list_new() {
  plips_val *newlist = malloc(sizeof(plips_val));
  newlist->type = PLIPS_LIST;
  newlist->val.list = zlistx_new();
  zlistx_set_destructor(newlist->val.list, plips_val_destroy);
  return newlist;
}

plips_val *plips_val_keyword_new(char *val) {
  plips_val *mv = plips_val_new(PLIPS_STRING, NULL);
  asprintf(&mv->val.str, "\x7f%s", val);
  return mv;
}

plips_val *plips_val_hash_new(zhashx_t *val) {
  plips_val *mv = plips_val_new(PLIPS_HASH, NULL);
  mv->val.hash = val;
  return mv;
}

plips_val *plips_val_new_atom(plips_val *val) {
  plips_val *mv = plips_val_new(PLIPS_ATOM, NULL);
  mv->val.atom_val = val;
  return mv;
}

plips_val *plips_val_new_function(void *(*func)(void *), int arg_cnt) {
  plips_val *mv = plisp_val_new(PLIPS_FN_C, NULL);
  mv->func_args = arg_cnt;
  mv->ismacro = 0;
  switch (arg_cnt) {
  case -1:
    mv->val.f1 = (void *(*)(void *))func;
    break;
  case 0:
    mv->val.f0 = (void *(*)())func;
    break;
  case 1:
    mv->val.f1 = (void *(*)(void *))func;
    break;
  case 2:
    mv->val.f2 = (void *(*)(void *, void *))func;
    break;
  case 3:
    mv->val.f3 = (void *(*)(void *, void *, void *))func;
    break;
  case 4:
    mv->val.f4 = (void *(*)(void *, void *, void *, void *))func;
    break;
  case 5:
    mv->val.f5 = (void *(*)(void *, void *, void *, void *, void *))func;
    break;
  case 6:
    mv->val.f6 =
        (void *(*)(void *, void *, void *, void *, void *, void *))func;
    break;
  case 7:
    mv->val.f7 =
        (void *(*)(void *, void *, void *, void *, void *, void *, void *))func;
    break;
  case 8:
    mv->val.f8 = (void *(*)(void *, void *, void *, void *, void *, void *,
                            void *, void *))func;
    break;
  case 9:
    mv->val.f9 = (void *(*)(void *, void *, void *, void *, void *, void *,
                            void *, void *, void *))func;
    break;
  case 10:
    mv->val.f10 = (void *(*)(void *, void *, void *, void *, void *, void *,
                             void *, void *, void *, void *))func;
    break;
  case 11:
    mv->val.f11 = (void *(*)(void *, void *, void *, void *, void *, void *,
                             void *, void *, void *, void *, void *))func;
    break;
  case 12:
    mv->val.f12 =
        (void *(*)(void *, void *, void *, void *, void *, void *, void *,
                   void *, void *, void *, void *, void *))func;
    break;
  case 13:
    mv->val.f13 =
        (void *(*)(void *, void *, void *, void *, void *, void *, void *,
                   void *, void *, void *, void *, void *, void *))func;
    break;
  case 14:
    mv->val.f14 =
        (void *(*)(void *, void *, void *, void *, void *, void *, void *,
                   void *, void *, void *, void *, void *, void *, void *))func;
    break;
  case 15:
    mv->val.f15 = (void *(*)(void *, void *, void *, void *, void *, void *,
                             void *, void *, void *, void *, void *, void *,
                             void *, void *, void *))func;
    break;
  case 16:
    mv->val.f16 = (void *(*)(void *, void *, void *, void *, void *, void *,
                             void *, void *, void *, void *, void *, void *,
                             void *, void *, void *, void *))func;
    break;
  case 17:
    mv->val.f17 = (void *(*)(void *, void *, void *, void *, void *, void *,
                             void *, void *, void *, void *, void *, void *,
                             void *, void *, void *, void *, void *))func;
    break;
  case 18:
    mv->val.f18 =
        (void *(*)(void *, void *, void *, void *, void *, void *, void *,
                   void *, void *, void *, void *, void *, void *, void *,
                   void *, void *, void *, void *))func;
    break;
  case 19:
    mv->val.f19 =
        (void *(*)(void *, void *, void *, void *, void *, void *, void *,
                   void *, void *, void *, void *, void *, void *, void *,
                   void *, void *, void *, void *, void *))func;
    break;
  case 20:
    mv->val.f20 =
        (void *(*)(void *, void *, void *, void *, void *, void *, void *,
                   void *, void *, void *, void *, void *, void *, void *,
                   void *, void *, void *, void *, void *, void *))func;
    break;
  }
  return mv;
}
