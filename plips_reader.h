#ifndef _PLISP_READER_H
#define _PLISP_READER_H
#include "plips.h"

plips_reader_t *reader_new(char *input);
void reader_destroy(plips_reader_t *self);
char *reader_next(plips_reader_t *reader);
char *reader_peek(plips_reader_t *reader);
void token_destructor(void **string_p);
void reader_print(plips_reader_t *reader);
plips_val *reader_str(char *command, int verbose);
char *type_print(plips_val *pt, int verbose);
int plips_type_get(plips_val *pt);
#endif
