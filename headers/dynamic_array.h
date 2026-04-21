#ifndef DARRAY_H
#define DARRAY_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define DARRAY_INITIAL_CAPACITY 16

enum darray_status {
  DA_ERR_ALLOC, //error during alocation
  DA_ERR_INIT,  //error in the passed parameters
  DA_ERR_INDEX, //index out of range
  DA_OK,
};


struct dynamic_array {
  void *array;
  size_t size;
  size_t capacity;
  size_t element_size;
};

typedef struct dynamic_array Darray;
typedef enum darray_status DarrayStatus;

Darray* darray_new(size_t array_size, size_t element_size);
DarrayStatus darray_init(Darray *a, size_t array_size, size_t element_size);
void darray_free(Darray *a);

DarrayStatus darray_push(Darray *a, void *element);
DarrayStatus darray_set(Darray *a, void *element, size_t index);
DarrayStatus darray_insert(Darray *a, void *element, size_t index);

void* darray_get(Darray *a, size_t index);

/* out argument can be NULL */
DarrayStatus darray_remove(Darray *a, size_t index, void *out);
DarrayStatus darray_merge(Darray *a, void *array, size_t array_size);
DarrayStatus darray_expand(Darray *a, size_t new_size);

#endif
