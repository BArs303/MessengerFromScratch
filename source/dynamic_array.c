#include "dynamic_array.h"

static DarrayStatus darray_check_index(Darray *a, size_t index);
static DarrayStatus darray_realloc(Darray *a, size_t new_size);
static void darray_shift_right(Darray *a, size_t index);
static void darray_shift_left(Darray *a, size_t index);  

/* CREATE SECTION */
Darray* darray_new(size_t array_size, size_t element_size) {
  Darray *result;
  DarrayStatus status;

  result = (Darray*)malloc(sizeof(Darray));
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
  if(a != NULL && darray_check_index(a, index) == DA_OK) {
    result = (char*)a->array + index * a->element_size;
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

DarrayStatus darray_expand(Darray *a, size_t new_size) {
  DarrayStatus rstatus;
  bool realloc_required;

  realloc_required = false;
  while(new_size >= a->capacity * a->element_size) {
    a->capacity *= 2;
    realloc_required = true;
  }

  if(realloc_required) {
    rstatus = darray_realloc(a, a->capacity * a->element_size);
  }
  else {
    rstatus = DA_OK;
  }
  return rstatus;
}

static DarrayStatus darray_realloc(Darray *a, size_t new_size) {
  void *tmp;

  tmp = realloc(a->array, new_size);
  if(!tmp) {
    return DA_ERR_ALLOC;
  }
  a->array = tmp;
  return DA_OK;
}
/* INSERT SECTION */

DarrayStatus darray_push(Darray *a, void *element) {
  DarrayStatus rstatus;
  void *position;

  if(!a || !element) {
    return DA_ERR_INIT;
  }
  rstatus = darray_expand(a, (a->size + 1) * a->element_size);
  if(rstatus == DA_OK) {
    position = (char*)a->array + a->size * a->element_size;
    memcpy(position, element, a->element_size);
    a->size++;
  }
  return rstatus;
}

DarrayStatus darray_set(Darray *a, void *element, size_t index) {
  DarrayStatus rstatus;
  void *position;

  if(!a || !element)
    return DA_ERR_INIT;
  rstatus = darray_check_index(a, index); 
  if(rstatus == DA_OK) {
    position = (char*)a->array + a->element_size * index;
    memcpy(position, element, a->element_size);
    if(index >= a->size) {
      a->size = index;
    }
  }
  return rstatus;
}

DarrayStatus darray_insert(Darray *a, void *element, size_t index) {
  DarrayStatus rstatus;
  void *position;

  if(!a || !element)
    return DA_ERR_INIT;
  rstatus = darray_expand(a, (a->size + 1) * a->element_size);
  if(rstatus == DA_OK) {
    position = (char*)a->array + a->element_size * index; 
    if(index < a->size) {
      darray_shift_right(a, index);
      memcpy(position, element, a->element_size);
    }
    else if(index < a->capacity) {
      memcpy(position, element, a->element_size);
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

  left = (char*)a->array + a->element_size * index;
  right = (char*)a->array + a->element_size * (index+1);
  memmove(right, left, a->size - index);
  a->size++;
}

static void darray_shift_left(Darray *a, size_t index) {
  void *left, *right;
  left = (char*)a->array + a->element_size * index;
  right = (char*)a->array + a->element_size * (index+1);
  memmove(left, right, a->size - index);
  a->size--;
}

static DarrayStatus darray_shrink(Darray *a) {
  DarrayStatus rstatus;
  rstatus = DA_OK;
  if(a->size < a->capacity / 4) {
    a->capacity /= 2;
    rstatus = darray_realloc(a, a->capacity * a->element_size);
  }
  return rstatus;
}

DarrayStatus darray_remove(Darray *a, size_t index, void *out) {
  DarrayStatus rstatus;
  if(!a)
    return DA_ERR_INIT;
  if(index < a->size) {
    if(out != NULL) {
      memcpy(out, (char*)a->array + a->element_size * index, a->element_size);
    }
    darray_shift_left(a, index);
    rstatus = darray_shrink(a);
  }
  else {
    rstatus = DA_ERR_INDEX;
  }
  return rstatus;
}

DarrayStatus darray_merge(Darray *dst, void *src, size_t array_size) {
  DarrayStatus rstatus;
  void *position;

  if(!dst || !src) 
    return DA_ERR_INIT;
  rstatus = darray_expand(dst, dst->size * dst->element_size + array_size);
  if(rstatus == DA_OK) {
    position = (char*)dst->array + dst->size * dst->element_size;
    memcpy(position, src, array_size);
    dst->size += array_size;
  }

  return DA_OK;
}
