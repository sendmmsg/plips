#ifndef _PLISP_VAL_H
#define _PLISP_VAL_H
#include "plips.h"

extern plips_val plips_nil;
extern plips_val plips_true;
extern plips_val plips_false;

plips_val *plips_val_new(plips_type, plips_val *metadata);
void plips_val_destroy(plips_val **mv);
void plips_val_print(plips_val *val, int verbose);
char *plips_val_tostr(plips_val *val, int verbose);

plips_val *plips_val_int_new(int64_t val);
plips_val *plips_val_float_new(double val);
plips_val *plips_val_string_new(char *val);
plips_val *plips_val_symbol_new(char *val);
plips_val *plips_val_keyword_new(char *val);
plips_val *plips_val_list_new();
plips_val *plips_val_hash_new(zhashx_t *val);
plips_val *plips_val_atom_new(plips_val *val);
plips_val *plips_val_fun_c_new(void *(*func)(void *), int arg_cnt);
plips_val *plips_val_fun_plips_new(plips_val *args, plips_val * body, plips_env *env,
                                   plips_val *(*eval_fn)(plips_val*, plips_env*));

void *plips_val_list_append(plips_val *list, void *item);
void *plips_val_list_first(plips_val *list);
void *plips_val_list_next(plips_val *list);
size_t plips_val_list_len(plips_val *list);
plips_val *plips_val_list_nth(plips_val *list, int n);
#endif
