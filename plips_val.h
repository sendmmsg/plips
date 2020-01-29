#ifndef _PLISP_VAL_H
#define _PLISP_VAL_H
#include <czmq.h>
typedef enum {
              PLIPS_LIST = 1,
              PLIPS_ATOM = 2,
              PLIPS_INT = 3,
              PLIPS_FLOAT = 4,
              PLIPS_FALSE = 5,
              PLIPS_TRUE = 6,
              PLIPS_NIL = 7,
              PLIPS_STRING = 8,
              PLIPS_SYMBOL = 9,
              PLIPS_HASH = 10,
              PLIPS_FN_C = 11,
              PLIPS_FN = 12,
} plips_type;
typedef struct _plips_val plips_val;
struct _plips_val {
  plips_type type;
  struct _plips_val *metadata;
  union ptypes {
    zlistx_t *list;
    char *str;
    int64_t i64;
    double f64;
    zhashx_t *hash;
    struct _plips_val *atom_val;
    void *(*f0)();
    void *(*f1)(void *);
    void *(*f2)(void *, void *);
    void *(*f3)(void *, void *, void *);
    void *(*f4)(void *, void *, void *, void *);
    void *(*f5)(void *, void *, void *, void *, void *);
    void *(*f6)(void *, void *, void *, void *, void *, void *);
    void *(*f7)(void *, void *, void *, void *, void *, void *, void *);
    void *(*f8)(void *, void *, void *, void *, void *, void *, void *, void *);
    void *(*f9)(void *, void *, void *, void *, void *, void *, void *, void *,
                void *);
    void *(*f10)(void *, void *, void *, void *, void *, void *, void *, void *,
                 void *, void *);
    void *(*f11)(void *, void *, void *, void *, void *, void *, void *, void *,
                 void *, void *, void *);
    void *(*f12)(void *, void *, void *, void *, void *, void *, void *, void *,
                 void *, void *, void *, void *);
    void *(*f13)(void *, void *, void *, void *, void *, void *, void *, void *,
                 void *, void *, void *, void *, void *);
    void *(*f14)(void *, void *, void *, void *, void *, void *, void *, void *,
                 void *, void *, void *, void *, void *, void *);
    void *(*f15)(void *, void *, void *, void *, void *, void *, void *, void *,
                 void *, void *, void *, void *, void *, void *, void *);
    void *(*f16)(void *, void *, void *, void *, void *, void *, void *, void *,
                 void *, void *, void *, void *, void *, void *, void *,
                 void *);
    void *(*f17)(void *, void *, void *, void *, void *, void *, void *, void *,
                 void *, void *, void *, void *, void *, void *, void *, void *,
                 void *);
    void *(*f18)(void *, void *, void *, void *, void *, void *, void *, void *,
                 void *, void *, void *, void *, void *, void *, void *, void *,
                 void *, void *);
    void *(*f19)(void *, void *, void *, void *, void *, void *, void *, void *,
                 void *, void *, void *, void *, void *, void *, void *, void *,
                 void *, void *, void *);
    void *(*f20)(void *, void *, void *, void *, void *, void *, void *, void *,
                 void *, void *, void *, void *, void *, void *, void *, void *,
                 void *, void *, void *, void *);
    struct {
      plips_val *(*evaluator)(plips_val *, struct _plips_env *);
      plips_val *args;
      plips_val *body;
        struct _plips_env *env;
    } func;
  } val;
  int func_args;
  int ismacro;
};

extern plips_val mal_nil;
extern plips_val mal_true;
extern plips_val mal_false;

plips_val *plips_val_new(plips_type, plips_val *metadata);
void plips_val_destroy(plips_val **mv);
void plips_val_print(plips_val *val, int verbose);
char* plips_val_tostr(plips_val *val, int verbose);
plips_val *plips_val_int_new(int64_t val);
plips_val *plips_val_float_new(double val);
plips_val *plips_val_string_new(char *val);
plips_val *plips_val_symbol_new(char *val);
plips_val *plips_val_keyword_new(char *val);
plips_val *plips_val_list_new();
plips_val *plips_val_hash_new(zhashx_t *val);
plips_val *plips_val_atom_new(plips_val *val);
plips_val *plips_val_function_new(void *(*func)(void *), int arg_cnt);
void* plips_val_list_append(plips_val *list, void *item);
void* plips_val_list_first(plips_val *list);
void* plips_val_list_next(plips_val *list);
#endif
