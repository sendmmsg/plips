#ifndef _PLIPS_ENV_H
#define _PLIPS_ENV_H
#include <czmq.h>
#include "plips.h"

plips_env *plips_env_new();
void plips_env_destroy(plips_env **self);
plips_val *plips_env_find(plips_env *self, char *key);
void plips_env_set_outer(plips_env *self, plips_env *outer);
plips_val *plips_env_get(plips_env *self, char *key);
void plips_env_del(plips_env *self, char *key);
size_t plips_env_size(plips_env *self);
size_t plips_env_total_size(plips_env *self);
int plips_env_set(plips_env *self, char *key, plips_val *val);
#endif
