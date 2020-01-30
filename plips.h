#ifndef _PLISP_H
#define _PLISP_H
#include <czmq.h>
typedef struct _plisp_env
{
    struct _plips_env *outer;
    zhashx_t *table;
} plips_env;
typedef enum
{
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

typedef struct _plips_val
{
    plips_type type;
    struct _plips_val *metadata;
    union ptypes
    {
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
        void *(*f9)(void *, void *, void *, void *, void *, void *, void *, void *, void *);
        void *(*f10)(void *, void *, void *, void *, void *, void *, void *, void *, void *, void *);
        void *(*f11)(void *, void *, void *, void *, void *, void *, void *, void *, void *, void *, void *);
        void *(*f12)(
          void *, void *, void *, void *, void *, void *, void *, void *, void *, void *, void *, void *);
        void *(*f13)(void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *);
        void *(*f14)(void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *);
        void *(*f15)(void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *);
        void *(*f16)(void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *);
        void *(*f17)(void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *);
        void *(*f18)(void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *);
        void *(*f19)(void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *);
        void *(*f20)(void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *,
                     void *);
        struct
        {
            struct _plips_val *(*evaluator)(struct _plips_val *, struct _plips_env *);
            struct _plips_val *args;
            struct _plips_val *body;
            struct _plips_env *env;
        } func;
    } val;
    int func_args;
    int ismacro;
} plips_val;

typedef struct _plips_reader_t plips_reader_t;

#include "plips_env.h"
#include "plips_val.h"
#include "plips_reader.h"
//#define streq(s1, s2) (!strcmp((s1), (s2)))

#endif
