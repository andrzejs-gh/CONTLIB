# CONTLIB Documentation

## Table of contents

- [Overview](#overview)
- [Full method list](#full-method-list)
- [Error codes and displaying container information](#error-codes-and-displaying-container-information)

---

## Overview
CONTILB provides **cont** objects - generic dynamic containers for storing elements of arbitrary size.

**cont** struct:
```c
typedef struct
{
    size_t unit;          // size of the element 
    size_t count;         // number of elements
    size_t capacity;      // capacity (>= 1)
    size_t max_capacity;  // max capacity (0 for unlimited or any value >= 1)
    double growth_factor; // growth factor in the range (1.0 ; 10.0]
    unsigned char* addr;  // pointer to allocated memory
    cont_methods* m;      // pointer to method vtable
} cont;
```
**INVALID_CONT** constant:
```c
const cont INVALID_CONT = {
    .unit = 0,
    .count = 0,
	.capacity = 0,
	.max_capacity = 0,
	.growth_factor = 0.0,
	.addr = NULL,
	.m = &invalid_cont_methods
};
```
**cont** methods (see: [full method list](#full-method-list)) can be called directly as specified in the prototypes below, or via the vtable. Direct calls involve full function names with the prefix *'cont_...'*, while indirect calls omit that prefix. Example:
```c
cont_set(&cnt, i, &item);
```
vs
```c
cnt.m->set(&cnt, i, &item);
```

Properly freed **cont** becomes **INVALID_CONT**. While it is safe to call methods via indirect calls on **INVALID_CONT** (the return values are either CONT_IS_INVALID, CONT_ALREADY_FREED error codes, NULL pointer, or INVALID_CONT - because the vtable is swaped), safety is not guaranteed in the case of direct calls and it is generaly recomended to check if a **cont** is valid via:
```c
cont_is_valid(&cnt);
```
```c
cnt.m->is_valid(&cnt);
```
1 - valid, 0 - invalid.
It detects not only if a **cont** is the INVALID_CONT, but also if **cont**'s fields have invalid values.

A **cont** has the capacity of at least 1 unit. Initial value is specified upon instantiation. 

Maximum capacity can be set to any value equal to or greater than 1, or to 0 which defines ***unlimited***. The default value is unlimited capacity.

Growth factor can be set to any value greater than 1.0 and less than or equal to 10.0. The default value is 2.0. If the capacity is full and new element(s) need to be added, the **cont** grows geometricaly acording to the formula: capacity *= growth_factor until sufficient capacity is reached or maximum capacity is reached.

<p align="right">
<a href="#table-of-contents">GO TO TOP ^</a>
</p>

---

## Full method list

### Creation and destruction

- [cont_new](#-cont_new-)
- [cont_free](#-cont_free-)

### Validation

- [cont_is_valid](#-cont_is_valid-)

### Setting container attributes

- [cont_set_count](#-cont_set_count-)
- [cont_set_capacity](#-cont_set_capacity-)
- [cont_set_max_capacity](#-cont_set_capacity-)
- [cont_set_growth_factor](#-cont_set_capacity-)

### Getting elements

- [cont_get](#-cont_get-)
- [cont_pop](#-cont_pop-)
- [cont_cv](#-cont_cv-)
- [cont_split](#-cont_split-)
- [cont_sub](#-cont_sub-)
- [cont_clone](#-cont_clone-)

### Adding and modifying elements

- [cont_set](#-cont_set-)
- [cont_push](#-cont_push-)
- [cont_push_front](#-cont_push_front-)
- [cont_append](#-cont_append-)
- [cont_prepend](#-cont_prepend-)
- [cont_extend](#-cont_extend-)
- [cont_write](#-cont_write-)
- [cont_insert](#-cont_insert-)
- [cont_insert_range](#-cont_insert_range-)

### Resizing container and removing elements

- [cont_set_space](#-cont_set_space-)
- [cont_grow](#-cont_grow-)
- [cont_shrink](#-cont_shrink-)
- [cont_remove](#-cont_remove-)
- [cont_cut](#-cont_cut-)
- [cont_clear](#-cont_clear-)
- [cont_collapse](#-cont_collapse-)

### Special

- [cont_reverse](#-cont_reverse-)
- [cont_set_blank](#-cont_set_blank-)

<p align="right">
<a href="#table-of-contents">GO TO TOP ^</a>
</p>

---

## Creation and destruction

### ** **cont_new** **

```c
cont cont_new(size_t capacity, size_t unit);
```

*Indirect call:*

```c
// no indirect call
```

Creates new container instance with the given capacity and element size.
Both `capacity` and `unit` must be greater than 0.

* **Return (success):** 

  * New container instance
  
* **Return (failure):** 

  * `INVALID_CONT` - `(!capacity || !unit || capacity > SIZE_MAX / unit)` or malloc failure

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_free** **

```c
int cont_free(cont* cont_);
```

*Indirect call:*

```c
cnt.m->free(&cnt);
```

Frees memory allocated by the container and turns it into INVALID_CONT.

* **Return (success):** 

  * `0`
  
* **Return (failure):** 
  
  * `CONT_ALREADY_FREED` - `(!.addr)`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

## Validation

### ** **cont_is_valid** **

```c
int cont_is_valid(cont* cont_);
```

*Indirect call:*

```c
cnt.m->is_valid(&cnt);
```

Validates the container. It detects not only if the container is INVALID_CONT, but also if its fields have valid values.

* **Return (success):** 

  * `1`

* **Return (failure):** 

  * `0`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

## Setting Container Attributes

### ** **cont_set_count** **

```c
int cont_set_count(cont* cont_, size_t count);
```

*Indirect call:*

```c
cnt.m->set_count(&cnt, count);
```

Sets the number of elements (`.count`).
Only decreasing the count i allowed.

* **Return (success):** 

  * `0`

* **Return (failure):** 
  
  * `COUNT_LARGER_THAN_CURRENT_COUNT` - `(count > .count)`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_set_capacity** **

```c
int cont_set_capacity(cont* cont_, size_t capacity);
```

*Indirect call:*

```c
cnt.m->set_capacity(&cnt, capacity);
```

Sets container capacity (`.capacity`).
Shrinking will truncate elements that stick out.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `NULL_CAPACITY` - `(!capacity)`
  * `SIZE_OVERFLOW` - `(capacity > SIZE_MAX / .unit)`
  * `MAX_CAPACITY_EXCEEDED` - `(.max_capacity && capacity > .max_capacity)`
  * `REALLOC_FAILURE`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_set_max_capacity** **

```c
int cont_set_max_capacity(cont* cont_, size_t max_size);
```

*Indirect call:*

```c
cnt.m->set_max_capacity(&cnt, max_capacity);
```

Sets `.max_capacity`.
`0` = unlimited (default value).

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `SIZE_OVERFLOW` - `(max_size > SIZE_MAX / .unit)`
  * `REALLOC_FAILURE`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_set_growth_factor** **

```c
int cont_set_growth_factor(cont* cont_, double growth_factor);`
```

*Indirect call:*

```c
cnt.m->set_growth_factor(&cnt, factor_value);
```

Sets growth factor. Valid range 1.0 < factor ≤ 10.0.

* **Return (success):** 

* `0`

* **Return (failure):** 

  * `INVALID_GROWTH_FACTOR_VALUE` - `(!(growth_factor > 1.0) || !(growth_factor <= 10.0) || !isfinite(growth_factor))`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

## Getting Elements

### ** **cont_get** **

```c
void* cont_get(cont* cont_, size_t index);
```

*Indirect call:*

```c
cnt.m->get(&cnt, i);
```

Returns pointer to the element at a given index. If index is invalid, it returns NULL.

* **Return (success):** 

  * Pointer to element

* **Return (failure):** 
  
  * `NULL` - `(index >= .count)`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_pop** **

```c
void* cont_pop(cont* cont_);
```

*Indirect call:*

```c
cnt.m->pop(&cnt);
```

Returns pointer to the last element and decreases count by 1. If count == 0, it returns NULL and exits.

* **Return (success):** 

  * Pointer to element

* **Return (failure):**
  
  * `NULL` - `(!.count)`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_cv** **

```c
int cont_cv(cont* cont_, size_t index, void* buffer, size_t n);
```

*Indirect call:*

```c
cnt.m->cv(&cnt, i, buffer, n);
```

Copies n elements starting at the specified index into a specified buffer. 
Passing n == 0 results in taking all elements from the index to the end.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `NULL_ARRAY_POINTER` - `(!buffer)`
  * `INVALID_INDEX` - `(index >= .count)`
  * `INVALID_RANGE` - `(n > .count - index)`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_split** **

```c
cont cont_split(cont* cont_, size_t index);
```

*Indirect call:*

```c
cnt.m->split(&cnt, i);
```

Splits container in two at a given index and returns the one starting at the index.
If the index is invalid or if memory allocation/reallocation fails, it makes sure there are no memory leaks and returns INVALID_CONT.

* **Return (success):** 
  
  * Subcontainer

* **Return (failure):** 
  
  * `INVALID_CONT` - `(!index || index >= .count)`, malloc/realloc failure

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_sub** **

```c
cont cont_sub(cont* cont_, size_t index, size_t n);
```

*Indirect call:*

```c
cnt.m->sub(&cnt, i, n);
```

Returns a subcontainer with n elements starting at a given index without modifying the original.
Passing n == 0 results in taking all elements from the index to the end.

* **Return (success):** 

  * Subcontainer

* **Return (failure):** 
  
  * `INVALID_CONT` - `(index >= .count)`, `(n > .count - index)`, malloc failure

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_clone** **

```c
cont cont_clone(cont* cont_);
```

*Indirect call:*

```c
cnt.m->clone(&cnt);
```

Returns an exact copy of the container. If malloc fails, it returns INVALID_CONT.

* **Return (success):** 

  * Cloned container

* **Return (failure):** 

  * `INVALID_CONT` - malloc failure

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

## Adding elements / setting values

### ** **cont_set** **

```c
int cont_set(cont* cont_, size_t index, void* item);
```

*Indirect call:*

```c
cnt.m->set(&cnt, i, &item);
```

Sets the value of an element at a given index. 

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `INVALID_INDEX` - `(index >= .count)`
  * `NULL_ITEM_POINTER` - `(!item)`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_push** **

```c
int cont_push(cont* cont_, void* item);
```

*Indirect call:*

```c
cnt.m->push(&cnt, &item);
```

Adds an element to the end of the container.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `NULL_ITEM_POINTER` - `(!item)`
  * `MAX_CAPACITY_EXCEEDED` - `(.max_capacity && .count == .max_capacity)`
  * `cont_grow` errors

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_push_front** **

```c
int cont_push_front(cont* cont_, void* item);
```

*Indirect call:*

```c
cnt.m->push_front(&cnt, &item);
```

Adds an element at the first index. If the container is not empty, it shifts all existing elements to the right.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `NULL_ITEM_POINTER` - `(!item)`
  * `MAX_CAPACITY_EXCEEDED` - `(.max_capacity && .count == .max_capacity)`
  * `cont_grow` errors

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_append** **

```c
int cont_append(cont* cont_, void* arr, size_t n);
```

*Indirect call:*

```c
cnt.m->append(&cnt, arr, n);
```

Appends n elements from a given array to the container. 

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `NULL_ARRAY_POINTER` - `(!arr)`
  * `NULL_ELEMENT_COUNT` - `(!n)`
  * `SIZE_OVERFLOW` - `(.count > SIZE_MAX - n)`
  * `MAX_CAPACITY_EXCEEDED` - `(.max_capacity && .count + n > .max_capacity)`
  * `cont_grow` errors

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_prepend** **

```c
int cont_prepend(cont* cont_, void* arr, size_t n);
```

*Indirect call:*

```c
cnt.m->prepend(&cnt, arr, n);
```

Prepends n elements from a given array to the container, shifting any existing elements to the right.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `NULL_ARRAY_POINTER` - `(!arr)`
  * `NULL_ELEMENT_COUNT` - `(!n)`
  * `SIZE_OVERFLOW` - `(.count > SIZE_MAX - n)`
  * `MAX_CAPACITY_EXCEEDED` - `(.max_capacity && .count + n > .max_capacity)`
  * `cont_grow` errors

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_extend** **

```c
int cont_extend(cont* cont_, cont* cont_2);
```

*Indirect call:*

```c
cnt.m->extend(&cnt, &cnt2);
```

Extends the container by appending all elements from another container. The pointers must point to different containers and their units must match.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `SAME_CONT` - `(cont_ == cont_2)`
  * `UNIT_MISMATCH` - `(cont_.unit != cont_2.unit)`
  * `cont_grow` errors

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_write** **

```c
int cont_write(cont* cont_, size_t index, void* arr, size_t num);
```

*Indirect call:*

```c
cnt.m->write(&cnt, index, arr, num);
```

Writes n elements from a given array to the container starting at the given index (can also start at last index + 1, appending elements). Overwrites existing elements if there is a collision.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `NULL_ARRAY_POINTER` - `(!arr)`
  * `NULL_ELEMENT_COUNT` - `(!num)`
  * `INVALID_INDEX` - `(index > .count)`
  * `SIZE_OVERFLOW` - `(index > SIZE_MAX - num)`, `(num > SIZE_MAX / .unit)`
  * `MAX_CAPACITY_EXCEEDED` - adding elements past .max_capacity (if set)
  * `cont_grow` errors

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_insert** **

```c
int cont_insert(cont* cont_, size_t index, void* item);
```

*Indirect call:*

```c
cnt.m->insert(&cnt, index, &item);
```

Inserts an element at the specified index, shifting any existing elements to the right.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `NULL_ITEM_POINTER` - `(!item)`
  * `INVALID_INDEX` - `(index > .count)`
  * `MAX_CAPACITY_EXCEEDED` - `(.max_capacity && .count == .max_capacity)`
  * `cont_grow` errors

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_insert_range** **

```c
int cont_insert_range(cont* cont_, size_t index, void* arr, size_t num);
```

*Indirect call:*

```c
cnt.m->insert_range(&cnt, index, arr, num);
```

Inserts n elements from the given array from the specified index onward, shifting existing elements to the right.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `NULL_ARRAY_POINTER` - `(!arr)`
  * `NULL_ELEMENT_COUNT` - `(!num)`
  * `INVALID_INDEX` - `(index > .count)`
  * `SIZE_OVERFLOW` - `(.count > SIZE_MAX - num)`, `(num > SIZE_MAX / .unit)`
  * `MAX_CAPACITY_EXCEEDED` - `(.max_capacity && .count + num > .max_capacity)`
  * `cont_grow` errors

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

## Resizing / deleting elements

### ** **cont_set_space** **

```c
int cont_set_space(cont* cont_, size_t n);
```

*Indirect call:*

```c
cnt.m->set_space(&cnt, n);
```

Sets the free space between the last element and the end of the container determined by its current capacity.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `EMPTY_CONT` - `(!.count)`
  * `SIZE_OVERFLOW` - `(.count > SIZE_MAX - n)`, `(new_capacity > SIZE_MAX / .unit)`
  * `MAX_CAPACITY_EXCEEDED` - `(.max_capacity && .count + n > .max_capacity)`
  * `REALLOC_FAILURE`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_grow** **

```c
int cont_grow(cont* cont_, size_t required_capacity);
```

*Indirect call:*

```c
cnt.m->grow(&cnt, required_capacity);
```

Increases capacity according to the formula: capacity *= growth_factor until the value is greater or equal to required_capacity. This function is used internally by methods that increase the container's capacity, manual use is not recommended.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `SIZE_OVERFLOW` - `(final_capacity_ > SIZE_MAX / unit)`
  * `REALLOC_FAILURE`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_shrink** **

```c
int cont_shrink(cont* cont_);
```

*Indirect call:*

```c
cnt.m->shrink(&cnt);
```

Reduces capacity to fit the count. Equivalent to `cont_set_space(&cnt, 0)`.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `EMPTY_CONT` - `(!.count)`
  * `REALLOC_FAILURE`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_remove** **

```c
int cont_remove(cont* cont_, size_t index);
```

*Indirect call:*

```c
cnt.m->remove(&cnt, index);
```

Removes the element at the specified index, shifting any following elements to the left.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `INVALID_INDEX` - `(index >= .count)`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_cut** **

```c
int cont_cut(cont* cont_, size_t index, size_t n);
```

*Indirect call:*

```c
cnt.m->cut(&cnt, index, n);
```

Removes n elements starting at the specified index, shifting any following elements to the left.
If n == 0, all elements from the specified index to the end of the container are removed.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `INVALID_INDEX` - `(index >= .count)`
  * `INVALID_RANGE` - `(n > .count - index)`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_clear** **

```c
int cont_clear(cont* cont_);
```

*Indirect call:*

```c
cnt.m->clear(&cnt);
```

Sets count to 0 making the container empty. Does not affect capacity.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * --

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **cont_collapse** **

```c
int cont_collapse(cont* cont_);
```

*Indirect call:*

```c
cnt.m->collapse(&cnt);
```

Sets capacity to 1 and count to 0.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `REALLOC_FAILURE`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

## Special

### ** **cont_reverse** **

```c
int cont_reverse(cont* cont_);
```

*Indirect call:*

```c
cnt.m->reverse(&cnt);
```

Reverses the order of elements in the container.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `EMPTY_CONT` - `(!.count)`
  * `MALLOC_FAILURE`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_set_blank** **

```c
int cont_set_blank(cont* cont_, size_t position, size_t n);
```

*Indirect call:*

```c
cnt.m->set_blank(&cnt, position, n);
```

Sets n bytes to '\0' starting at the specified position. The range: `position + n` must be within the container’s capacity.
If n == 0, all bytes from the specified position to the end of the capacity are '\0'ed.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `INVALID_INDEX` - `(position >= .capacity)`
  * `INVALID_RANGE` - `(n > .capacity - position)`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

## Error codes and displaying container information

Full list of error codes from cont.h:

```c
enum error_codes
{
	INVALID_INDEX = 1,
	SIZE_OVERFLOW,
	NULL_ARRAY_POINTER,
	REALLOC_FAILURE,
	MALLOC_FAILURE,
	NULL_ITEM_POINTER,
	NULL_ELEMENT_COUNT,
	NULL_CAPACITY,
	MAX_CAPACITY_EXCEEDED,
	COUNT_LARGER_THAN_CURRENT_COUNT,
	INVALID_GROWTH_FACTOR_VALUE,
	CONT_ALREADY_FREED,
	EMPTY_CONT,
	UNIT_MISMATCH,
	SAME_CONT,
	INVALID_RANGE,
	CONT_IS_INVALID
};
```

To display the error name or cont information, use the functions from cont_info.c (make sure to include cont_info.h):

```c
void print_cont_err(int code)
```

```c
void cont_info(cont* cont_)
```

<p align="right">
<a href="#table-of-contents">GO TO TOP ^</a>
</p>
