#define _GNU_SOURCE
#include "plips.h"

plips_val plips_nil = {PLIPS_NIL, NULL, {0}, 0, 0};
plips_val plips_true = {PLIPS_TRUE, NULL, {0}, 0, 0};
plips_val plips_false = {PLIPS_FALSE, NULL, {0}, 0, 0};

void plips_val_destroy(plips_val **s) {
    plips_val **self = (plips_val **) s;
    assert(self);

    if ((*self)->type == PLIPS_LIST)
        zlistx_destroy(&(*self)->val.list);
    free(*self);
    *self = NULL;
}

// Allocate a malval and set its type and value
plips_val *plips_val_new(plips_type type, plips_val *metadata) {
    plips_val *pv = (plips_val *) malloc(sizeof(plips_val));
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
plips_val *plips_val_list_new() {
    plips_val *newlist = malloc(sizeof(plips_val));
    newlist->type = PLIPS_LIST;
    newlist->val.list = zlistx_new();
    zlistx_set_destructor(newlist->val.list, (void (*)(void **)) plips_val_destroy);
    return newlist;
}

void *plips_val_list_append(plips_val *list, void *item) {
    assert(list->type == PLIPS_LIST);
    return zlistx_add_end(list->val.list, item);
}
void *plips_val_list_first(plips_val *list) {
    assert(list->type == PLIPS_LIST);
    return zlistx_first(list->val.list);
}
void *plips_val_list_next(plips_val *list) {
    assert(list->type == PLIPS_LIST);
    return zlistx_next(list->val.list);
}
 size_t plips_val_list_len(plips_val *list) {
     assert(list->type == PLIPS_LIST);
     return zlistx_size(list->val.list);
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

plips_val *plips_val_atom_new(plips_val *val) {
    plips_val *mv = plips_val_new(PLIPS_ATOM, NULL);
    mv->val.atom_val = val;
    return mv;
}

void plips_val_print(plips_val *pt, int verbose) {
    char *str = plips_val_tostr(pt, verbose);
    printf("%s\n", str);
    free(str);
}

char *plips_val_tostr(plips_val *pt, int verbose) {
    size_t size;
    char *ptr;
    FILE *ss;
    ss = open_memstream(&ptr, &size);
    if (ss == NULL) {
        perror("open_memstream: ");
        exit(EXIT_FAILURE);
    }

    if (pt->type == PLIPS_LIST) {
        if (verbose)
            fprintf(ss, "L-%ld: (", zlistx_size(pt->val.list));
        else
            fprintf(ss, "(");

        plips_val *i = zlistx_first(pt->val.list);
        while (i) {
            char *s = plips_val_tostr(i, verbose);
            fprintf(ss, "%s ", s);
            free(s);
            i = zlistx_next(pt->val.list);
        }
        fprintf(ss, ") ");
    } else if (pt->type == PLIPS_ATOM) {
        if (verbose)
            fprintf(ss, "A:");
        fprintf(ss, "%s ", pt->val.str);
    } else if (pt->type == PLIPS_SYMBOL) {
        if (verbose)
            fprintf(ss, "S:");
        fprintf(ss, "%s ", pt->val.str);
    } else if (pt->type == PLIPS_INT) {
        if (verbose)
            fprintf(ss, "I:");
        fprintf(ss, "%ld ", pt->val.i64);
    } else if (pt->type == PLIPS_FLOAT) {
        if (verbose)
            fprintf(ss, "F:");
        fprintf(ss, "%f ", pt->val.f64);
    } else if (pt->type == PLIPS_FALSE) {
        if (verbose)
            fprintf(ss, "A: ");
        fprintf(ss, "false ");
    } else if (pt->type == PLIPS_TRUE) {
        if (verbose)
            fprintf(ss, "A: ");
        fprintf(ss, "true ");
    } else if (pt->type == PLIPS_NIL) {
        if (verbose)
            fprintf(ss, "A: ");
        fprintf(ss, "nil ");
    } else if (pt->type == PLIPS_STRING) {
        if (verbose)
            fprintf(ss, "S:");
        fprintf(ss, "%s ", pt->val.str);
    } else if (pt->type == PLIPS_FN_C) {
        if (verbose)
            fprintf(ss, "FN_C(%d):", pt->func_args);
        void *p;
        switch (pt->func_args) {
            case 0:
                p = (void *) pt->val.f0;
                break;
            case 1:
                p = (void *) pt->val.f1;
                break;
            case 2:
                p = (void *) pt->val.f2;
                break;
            case 3:
                p = (void *) pt->val.f3;
                break;
            case 4:
                p = (void *) pt->val.f4;
                break;
            case 5:
                p = (void *) pt->val.f5;
                break;
            case 6:
                p = (void *) pt->val.f6;
                break;
            case 7:
                p = (void *) pt->val.f7;
                break;
            case 8:
                p = (void *) pt->val.f8;
                break;
            case 9:
                p = (void *) pt->val.f9;
                break;
            case 10:
                p = (void *) pt->val.f10;
                break;
            case 11:
                p = (void *) pt->val.f11;
                break;
            case 12:
                p = (void *) pt->val.f12;
                break;
            case 13:
                p = (void *) pt->val.f13;
                break;
            case 14:
                p = (void *) pt->val.f14;
                break;
            case 15:
                p = (void *) pt->val.f15;
                break;
            case 16:
                p = (void *) pt->val.f16;
                break;
            case 17:
                p = (void *) pt->val.f17;
                break;
            case 18:
                p = (void *) pt->val.f18;
                break;
            case 19:
                p = (void *) pt->val.f19;
                break;
            case 20:
                p = (void *) pt->val.f20;
                break;
        }
        fprintf(ss, "%p ", p);
    }

    else {
        fprintf(ss, "unknown pt type %d", pt->type);
    }

    fclose(ss);
    return ptr;
}

plips_val *plips_val_function_new(void *(*func)(void *), int arg_cnt) {
    plips_val *mv = plips_val_new(PLIPS_FN_C, NULL);
    mv->func_args = arg_cnt;
    mv->ismacro = 0;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
    switch (arg_cnt) {
        case -1:
            mv->val.f1 = (void *(*) (void *) ) func;
            break;
        case 0:
            mv->val.f0 = (void *(*) ()) func;
            break;
        case 1:
            mv->val.f1 = (void *(*) (void *) ) func;
            break;
        case 2:
            mv->val.f2 = (void *(*) (void *, void *) ) func;
            break;
        case 3:
            mv->val.f3 = (void *(*) (void *, void *, void *) ) func;
            break;
        case 4:
            mv->val.f4 = (void *(*) (void *, void *, void *, void *) ) func;
            break;
        case 5:
            mv->val.f5 = (void *(*) (void *, void *, void *, void *, void *) ) func;
            break;
        case 6:
            mv->val.f6 = (void *(*) (void *, void *, void *, void *, void *, void *) ) func;
            break;
        case 7:
            mv->val.f7 = (void *(*) (void *, void *, void *, void *, void *, void *, void *) ) func;
            break;
        case 8:
            mv->val.f8 = (void *(*) (void *, void *, void *, void *, void *, void *, void *, void *) ) func;
            break;
        case 9:
            mv->val.f9 =
              (void *(*) (void *, void *, void *, void *, void *, void *, void *, void *, void *) ) func;
            break;
        case 10:
            mv->val.f10 = (void *(*) (void *, void *, void *, void *, void *, void *, void *, void *, void *,
                                      void *) ) func;
            break;
        case 11:
            mv->val.f11 = (void *(*) (void *, void *, void *, void *, void *, void *, void *, void *, void *,
                                      void *, void *) ) func;
            break;
        case 12:
            mv->val.f12 = (void *(*) (void *, void *, void *, void *, void *, void *, void *, void *, void *,
                                      void *, void *, void *) ) func;
            break;
        case 13:
            mv->val.f13 = (void *(*) (void *, void *, void *, void *, void *, void *, void *, void *, void *,
                                      void *, void *, void *, void *) ) func;
            break;
        case 14:
            mv->val.f14 = (void *(*) (void *, void *, void *, void *, void *, void *, void *, void *, void *,
                                      void *, void *, void *, void *, void *) ) func;
            break;
        case 15:
            mv->val.f15 = (void *(*) (void *, void *, void *, void *, void *, void *, void *, void *, void *,
                                      void *, void *, void *, void *, void *, void *) ) func;
            break;
        case 16:
            mv->val.f16 = (void *(*) (void *, void *, void *, void *, void *, void *, void *, void *, void *,
                                      void *, void *, void *, void *, void *, void *, void *) ) func;
            break;
        case 17:
            mv->val.f17 = (void *(*) (void *, void *, void *, void *, void *, void *, void *, void *, void *,
                                      void *, void *, void *, void *, void *, void *, void *, void *) ) func;
            break;
        case 18:
            mv->val.f18 =
              (void *(*) (void *, void *, void *, void *, void *, void *, void *, void *, void *, void *,
                          void *, void *, void *, void *, void *, void *, void *, void *) ) func;
            break;
        case 19:
            mv->val.f19 =
              (void *(*) (void *, void *, void *, void *, void *, void *, void *, void *, void *, void *,
                          void *, void *, void *, void *, void *, void *, void *, void *, void *) ) func;
            break;
        case 20:
            mv->val.f20 = (void *(*) (void *, void *, void *, void *, void *, void *, void *, void *, void *,
                                      void *, void *, void *, void *, void *, void *, void *, void *, void *,
                                      void *, void *) ) func;
            break;
    }
#pragma GCC diagnostic pop
    return mv;
}
