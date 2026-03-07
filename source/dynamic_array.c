#include "dynamic_array.h"

static DarrayStatus darray_check_index(Darray *a, size_t index);
static DarrayStatus darray_expand(Darray *a);
static DarrayStatus darray_realloc(Darray *a);
static void darray_shift_right(Darray *a, size_t index);
static void darray_shift_left(Darray *a, size_t index);  

/* CREATE SECTION */
Darray* darray_new(size_t array_size, size_t element_size) {
  Darray *result;
  DarrayStatus status;

  result = malloc(sizeof(Darray));
  status = darray_init(result, array_size, element_size);
  if(status == DA_OK) {
    return result;
  }
  return NULL;
}

DarrayStatus darray_init(Darray *a, size_t array_size, size_t element_size) { 
  if(!a || element_size == 0) {
    return DA_ERR_INIT;
  }
  if(array_size < DARRAY_INITIAL_CAPACITY) {
    array_size = DARRAY_INITIAL_CAPACITY;
  }

  a->array = malloc(array_size * element_size);
  if(!a->array) {
    a->size = 0;
    a->capacity = 0;
    a->element_size = element_size;
    return DA_ERR_ALLOC;
  }
  a->size = 0;
  a->capacity = array_size;
  a->element_size = element_size;
  return DA_OK;
}

/* WARNING DON'T PASS LOCALY CREATED DYNAMIC ARRAY */
void darray_free(Darray *a) {
  free(a->array);
  free(a);
}

/* END OF CREATE SECTION */

/* WARNING: DON'T MODIFY RETURNED VALUE */
void* darray_get(Darray *a, size_t index) {
  void *result;
  result = NULL;
  if(!a && darray_check_index(a, index) == DA_OK) {
    result = a->array + index * a->element_size;
  }
  return result;
}

static DarrayStatus darray_check_index(Darray *a, size_t index) {
  /* unsigned int index always >= 0 */
  if(index < a->capacity)
    return DA_OK;
  else
    return DA_ERR_INDEX;
}

static DarrayStatus darray_expand(Darray *a) {
  DarrayStatus rstatus;
  if(a->size + 1 == a->capacity) {
    rstatus = darray_realloc(a);
  }
  else {
    rstatus = DA_OK;
  }
  return rstatus;
}

static DarrayStatus darray_realloc(Darray *a) {
  void *tmp;

  a->capacity *= 2;
  tmp = realloc(a->array, a->capacity * a->element_size);
  if(!tmp) {
    return DA_ERR_ALLOC;
  }
  a->array = tmp;
  return DA_OK;
}
/* INSERT SECTION */

DarrayStatus darray_push(Darray *a, void *element) {
  DarrayStatus rstatus;

  if(!a || !element) {
    return DA_ERR_INIT;
  }
  rstatus = darray_expand(a);
  if(rstatus == DA_OK) {
    memcpy(a->array + a->size * a->element_size, element, a->element_size);
    a->size++;
  }
  return rstatus;
}

DarrayStatus darray_set(Darray *a, void *element, size_t index) {
  DarrayStatus rstatus;

  if(!a || !element)
    return DA_ERR_INIT;
  rstatus = darray_check_index(a, index); 
  if(rstatus == DA_OK) {
    memcpy(a->array + a->element_size * index, element, a->element_size);
    if(index >= a->size) {
      a->size = index;
    }
  }
  return rstatus;
}

DarrayStatus darray_insert(Darray *a, void *element, size_t index) {
  DarrayStatus rstatus;

  if(!a || !element)
    return DA_ERR_INIT;
  rstatus = darray_expand(a);
  if(rstatus == DA_OK) {
    if(index < a->size) {
      darray_shift_right(a, index);
      memcpy(a->array + a->element_size * index, element, a->element_size);
    }
    else if(index < a->capacity) {
      memcpy(a->array + a->element_size * index, element, a->element_size);
      a->size = index;
    }
    else {
      rstatus = DA_ERR_INDEX;
    }
  }
  return rstatus;
}

/* END OF INSERT SECTION */

static void darray_shift_right(Darray *a, size_t index) {
  void *left, *right;
  left = a->array + a->element_size * index;
  right = a->array + a->element_size * (index+1);
  memmove(right, left, a->size - index);
  a->size++;
}

static void darray_shift_left(Darray *a, size_t index) {
  void *left, *right;
  left = a->array + a->element_size * index;
  right = a->array + a->element_size * (index+1);
  memmove(left, right, a->size - index);
  a->size--;
}

DarrayStatus darray_remove(Darray *a, size_t index, void *out) {
  DarrayStatus rstatus;
  if(!a)
    return DA_ERR_INIT;
  if(index < a->size) {
    if(out != NULL) {
      memcpy(out, a->array + a->element_size * index, a->element_size);
    }
    darray_shift_left(a, index);
    rstatus = DA_OK;
  }
  else {
    rstatus = DA_ERR_INDEX;
  }
  return rstatus;
}
