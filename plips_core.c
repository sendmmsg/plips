#include "plips.h"

typedef enum _OpType {
             ADD=1,
             SUB,
             MUL,
             DIV,
             EQ,
             GT,
             GTE,
             LT,
             LTE,
} OpType;
plips_val *__plips_arit(plips_val *a, plips_val *b, OpType type){
    if (a == NULL || b == NULL) {
        plips_error = plips_val_string_new("Error: in procedure _arit, one of the arguments is nil");
        return NULL;
    }
    if (a->type != PLIPS_INT && a->type != PLIPS_FLOAT) {
        printf("type %d\n", a->type);
        plips_error = plips_val_string_new("Error: in procedure _arit, first parameter is of wrong type");
        return NULL;
    }
    if (b->type != PLIPS_INT && b->type != PLIPS_FLOAT) {
        plips_error = plips_val_string_new("Error: in procedure _arit, second parameter is of wrong type");
        return NULL;
    }
    if (a->type == PLIPS_INT && b->type == PLIPS_INT) {
        switch(type){
        case ADD:
            return plips_val_int_new(a->val.i64 + b->val.i64);
        case SUB:
            return plips_val_int_new(a->val.i64 - b->val.i64);
        case MUL:
            return plips_val_int_new(a->val.i64 * b->val.i64);
        case DIV:
            return plips_val_int_new(a->val.i64 / b->val.i64);
        case EQ:
            if (a->val.i64 == b->val.i64)
                return &plips_true;
            return &plips_false;

        case GT:
            if (a->val.i64 > b->val.i64)
                return &plips_true;
            return &plips_false;
        case GTE:
            if (a->val.i64 >= b->val.i64)
                return &plips_true;
            return &plips_false;
        case LT:
            if (a->val.i64 < b->val.i64)
                return &plips_true;
            return &plips_false;
        case LTE:
            if (a->val.i64 <= b->val.i64)
                return &plips_true;
            return &plips_false;
        }
    }
    if (a->type == PLIPS_FLOAT && b->type == PLIPS_INT) {
        switch(type){
        case ADD:
            return plips_val_float_new(a->val.f64 + b->val.i64);
        case SUB:
            return plips_val_float_new(a->val.f64 - b->val.i64);
        case MUL:
            return plips_val_float_new(a->val.f64 * b->val.i64);
        case DIV:
            return plips_val_float_new(a->val.f64 / b->val.i64);
        case EQ:
            if (a->val.f64 == b->val.i64)
                return &plips_true;
            return &plips_false;

        case GT:
            if (a->val.f64 > b->val.i64)
                return &plips_true;
            return &plips_false;
        case GTE:
            if (a->val.f64 >= b->val.i64)
                return &plips_true;
            return &plips_false;
        case LT:
            if (a->val.f64 < b->val.i64)
                return &plips_true;
            return &plips_false;
        case LTE:
            if (a->val.f64 <= b->val.i64)
                return &plips_true;
            return &plips_false;
        }
    }
    if (a->type == PLIPS_INT && b->type == PLIPS_FLOAT) {
        switch(type){
        case ADD:
            return plips_val_float_new(a->val.i64 + b->val.f64);
        case SUB:
            return plips_val_float_new(a->val.i64 - b->val.f64);
        case MUL:
            return plips_val_float_new(a->val.i64 * b->val.f64);
        case DIV:
            return plips_val_float_new(a->val.i64 / b->val.f64);
        case EQ:
            if (a->val.i64 == b->val.f64)
                return &plips_true;
            return &plips_false;

        case GT:
            if (a->val.i64 > b->val.f64)
                return &plips_true;
            return &plips_false;
        case GTE:
            if (a->val.i64 >= b->val.f64)
                return &plips_true;
            return &plips_false;
        case LT:
            if (a->val.i64 < b->val.f64)
                return &plips_true;
            return &plips_false;
        case LTE:
            if (a->val.i64 <= b->val.f64)
                return &plips_true;
            return &plips_false;

        }
    }
    if (a->type == PLIPS_FLOAT && b->type == PLIPS_FLOAT) {
        switch(type){
        case ADD:
            return plips_val_float_new(a->val.f64 + b->val.f64);
        case SUB:
            return plips_val_float_new(a->val.f64 - b->val.f64);
        case MUL:
            return plips_val_float_new(a->val.f64 * b->val.f64);
        case DIV:
            return plips_val_float_new(a->val.f64 / b->val.f64);
                    case EQ:
            if (a->val.f64 == b->val.f64)
                return &plips_true;
            return &plips_false;

        case GT:
            if (a->val.f64 > b->val.f64)
                return &plips_true;
            return &plips_false;
        case GTE:
            if (a->val.f64 >= b->val.f64)
                return &plips_true;
            return &plips_false;
        case LT:
            if (a->val.f64 < b->val.f64)
                return &plips_true;
            return &plips_false;
        case LTE:
            if (a->val.f64 <= b->val.f64)
                return &plips_true;
            return &plips_false;

        }
    }
    plips_error = plips_val_string_new("Error: in procedure +, some impossible error");
    return NULL;
}


/* TODO: make a generic function here, getting rid of all the duplication */
plips_val *_plips_add(plips_val *a, plips_val *b) {
    return __plips_arit(a,b,ADD);
}
plips_val *_plips_sub(plips_val *a, plips_val *b) {
    return __plips_arit(a,b,SUB);
}
plips_val *_plips_mul(plips_val *a, plips_val *b) {
    return __plips_arit(a,b,MUL);
}
plips_val *_plips_div(plips_val *a, plips_val *b) {
    return __plips_arit(a,b,DIV);
}
plips_val *_plips_eq(plips_val *a, plips_val *b) {
    return __plips_arit(a,b,EQ);
}
plips_val *_plips_gt(plips_val *a, plips_val *b) {
    return __plips_arit(a,b,GT);
}
plips_val *_plips_gte(plips_val *a, plips_val *b) {
    return __plips_arit(a,b,GTE);
}
plips_val *_plips_lt(plips_val *a, plips_val *b) {
    return __plips_arit(a,b,LT);
}

plips_val *_plips_lte(plips_val *a, plips_val *b) {
    return __plips_arit(a,b,LTE);
}


plips_val *_plips_is_list(plips_val *a) {
    if(a->type == PLIPS_LIST)
        return &plips_true;

    return &plips_false;
}
plips_val *_plips_is_symbol(plips_val *a) {
    if(a->type == PLIPS_SYMBOL)
        return &plips_true;

    return &plips_false;
}

plips_val *_plips_is_string(plips_val *a) {
    if(a->type == PLIPS_STRING)
        return &plips_true;

    return &plips_false;
}

plips_val *_plips_is_int(plips_val *a) {
    if(a->type == PLIPS_INT)
        return &plips_true;

    return &plips_false;
}

plips_val *_plips_is_float(plips_val *a) {
    if(a->type == PLIPS_FLOAT)
        return &plips_true;

    return &plips_false;
}
plips_val *_plips_list(plips_val *ast) {
    plips_val *newlist = plips_val_list_new();
    plips_val *a;
    if(plips_val_list_len(ast) == 1)
        return newlist;

    a = plips_val_list_first(ast);
    a = plips_val_list_next(ast);
    while(a != NULL){
        plips_val_list_append(newlist, a);
        a = plips_val_list_next(ast);
    }

    return newlist;
}

plips_val *_plips_list_nth(plips_val *list, plips_val *num) {
    assert(list != NULL);
    assert(num != NULL);

    if(list->type != PLIPS_LIST){
        plips_error = plips_val_string_new("Error: nth first argument is not a list");
        return NULL;
    }

    if(num->type != PLIPS_INT){
        plips_error = plips_val_string_new("Error: nth second argument is not an integer");
        return NULL;
    }
    int n = num->val.i64;
    if(n < 0)
        return &plips_nil;
    return plips_val_list_nth(list, n);
}


plips_env * plips_setup_repl_env() {
    plips_env *repl_env = plips_env_new();
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
    plips_val *f = plips_val_function_new((void *(*) (void *) ) _plips_add, 2);
    plips_env_set(repl_env, "+", f);
    f = plips_val_function_new((void *(*) (void *) ) _plips_sub, 2);
    plips_env_set(repl_env, "-", f);
    f = plips_val_function_new((void *(*) (void *) ) _plips_mul, 2);
    plips_env_set(repl_env, "*", f);
    f = plips_val_function_new((void *(*) (void *) ) _plips_div, 2);
    plips_env_set(repl_env, "/", f);

    f = plips_val_function_new((void *(*) (void *) ) _plips_eq, 2);
    plips_env_set(repl_env, "=", f);
    f = plips_val_function_new((void *(*) (void *) ) _plips_gt, 2);
    plips_env_set(repl_env, ">", f);
    f = plips_val_function_new((void *(*) (void *) ) _plips_gte, 2);
    plips_env_set(repl_env, ">=", f);
    f = plips_val_function_new((void *(*) (void *) ) _plips_lt, 2);
    plips_env_set(repl_env, "<", f);
    f = plips_val_function_new((void *(*) (void *) ) _plips_lte, 2);
    plips_env_set(repl_env, "<=", f);



    f = plips_val_function_new((void *(*) (void *) ) _plips_is_list, 1);
    plips_env_set(repl_env, "list?", f);
    f = plips_val_function_new((void *(*) (void *) ) _plips_is_symbol, 1);
    plips_env_set(repl_env, "symb?", f);
    f = plips_val_function_new((void *(*) (void *) ) _plips_is_string, 1);
    plips_env_set(repl_env, "string?", f);
    f = plips_val_function_new((void *(*) (void *) ) _plips_is_int, 1);
    plips_env_set(repl_env, "int?", f);
    f = plips_val_function_new((void *(*) (void *) ) _plips_is_float, 1);
    plips_env_set(repl_env, "int?", f);

    f = plips_val_function_new((void *(*) (void *) ) _plips_list, -1);
    plips_env_set(repl_env, "list",f );
    f = plips_val_function_new((void *(*) (void *) ) _plips_list_nth, 2);
    plips_env_set(repl_env, "nth", f );


#pragma GCC diagnostic pop
    return repl_env;
}
