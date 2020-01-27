#ifndef _PLISP_READER_H
#define _PLISP_READER_H
#define PLIPS_LIST 1
#define PLIPS_ATOM 2
#define PLIPS_INT 3
#define PLIPS_FLOAT 4

typedef struct _plips_reader_t plips_reader_t;
typedef struct _plips_type_t plips_type_t;
plips_reader_t *reader_new(char *input);
void reader_destroy(plips_reader_t *self);
char* reader_next(plips_reader_t *reader);
char* reader_peek(plips_reader_t *reader);
void token_destructor (void **string_p);
void reader_print(plips_reader_t *reader);
plips_type_t * reader_str(char *command);
#endif
