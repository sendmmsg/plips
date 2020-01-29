#ifndef _PLISP_H
#define _PLISP_H
#include "plips_val.h"
#include <czmq.h>

typedef struct _plisp_env
{
    struct _plips_env *outer;
    zhashx_t *table;
} plips_env;

typedef struct _plips_reader_t plips_reader_t;

#endif
