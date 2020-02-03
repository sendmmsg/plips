#include "plips.h"
#include <czmq.h>

plips_env *plips_env_new() {
    plips_env *self;
    self = malloc(sizeof(plips_env));
    self->outer = NULL;
    self->table = zhashx_new();
    zhashx_set_destructor(self->table, (void (*)(void **)) & plips_val_destroy);
    return self;
}

void plips_env_destroy(plips_env **s) {
    plips_env **self = (plips_env **) s;
    assert(self);

    if ((*self)->table)
        zhashx_destroy(&(*self)->table);
    free(*self);
    *self = NULL;
}

plips_val *plips_env_find(plips_env *self, char *key) {
    plips_val *item = (plips_val *) zhashx_lookup(self->table, key);
    if (item == NULL && self->outer != NULL)
        return plips_env_find((plips_env *) self->outer, key);
    return item;
}

void plips_env_set_outer(plips_env *self, plips_env *outer) {
    self->outer = (struct _plips_env *) outer;
}

plips_val *plips_env_get(plips_env *self, char *key) {
    return plips_env_find(self, key);
}

void plips_env_del(plips_env *self, char *key) {
    zhashx_delete(self->table, key);
}

// size of this enviornment
size_t plips_env_size(plips_env *self) {
    zhashx_size(self->table);
}
// size of this environment + all outer
size_t plips_env_total_size(plips_env *self) {
    if (self->outer = NULL)
        return zhashx_size(self->table);

    return zhashx_size(self->table) + plips_env_total_size((plips_env *) self->outer);
}

int plips_env_set(plips_env *self, char *key, plips_val *val){
    assert(self != NULL);
    if(key == NULL){
        printf("Error: plips_env_set, attempt to insert with NULL key!\n");
        return -1;
    }
    if(val == NULL){
        printf("Error: plips_env_set, attempt to insert with NULL val!\n");
        return -1;
    }
    if(zhashx_insert(self->table, key, val) == -1){
        printf("%s already in env, removing and adding again\n", key);
        // try to delete existing item and try again
        zhashx_delete(self->table, key);
    }
    return zhashx_insert(self->table, key, val);
}
