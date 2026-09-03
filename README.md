# CONTLIB Documentation

## Table of contents

- [Overview](#overview)
- [Full method list](#full-method-list)
- [Error codes](#error-codes)

---

## Overview
**CONTILB** provides **cont** objects - generic and dynamic data containers for storing elements of arbitrary size and alignment.

**cont** struct (non-opaque):
```c
typedef struct
{
    size_t unit;          // size of the item 
    size_t alignment      // item alignment
    size_t count;         // number of items
    size_t capacity;      // capacity (>= 1)
    size_t max_capacity;  // max capacity (0 for unlimited or any value >= 1)
    double growth_factor; // growth factor in the range (1.0 ; 10.0]
    unsigned char* addr;  // pointer to allocated memory
} cont;
```
The struct is intentionaly made non-opaque so the fields can be manually manipulated to bypass API overhead if needed.

When creating a new **cont** (see [Creation and destruction](#creation-and-destruction), [cont_NEW](#-cont_new-)), you specify the type and the initial capacity - which defines the memory that the **cont** will allocate: 
```c
capacity * sizeof(type)
```

Maximum capacity can be set to any value equal to or greater than `1`, or to `0`, which defines ***unlimited**. The default value is **unlimited**. You can use the macro:
```c
// cont.h

#define cont_NO_LIMIT 0
```
for better readability.

Growth factor can be set to any value between:
```c
// cont.h

#define cont_GF_LBOUND 1.0
#define cont_GF_UBOUND 10.0
```
the default value is set to `2.0`:
```c
// cont.h

#define cont_GF 2.0
```

When the capacity is full and new element(s) need to be added, the **cont** grows geometricaly acording to the formula: 
```c
capacity *= growth_factor
``` 
untill sufficient capacity is reached or **maximum capacity** is reached.

Freeing a **cont** releases its buffer and invalidates it, making it an **INVALID_CONT**, which is defined as:
```c
// cont.c

const cont INVALID_CONT = (cont){0};
```

<p align="right">
<a href="#table-of-contents">GO TO TOP ^</a>
</p>

---

## Full method list

### Creation and destruction

- [cont_NEW](#-cont_new-)
- [cont_new](#-cont_new--1)
- [cont_free](#-cont_free-)

### Validation

- [cont_is_valid](#-cont_is_valid-)

### Setting container attributes

- [cont_set_count](#-cont_set_count-)
- [cont_set_capacity](#-cont_set_capacity-)
- [cont_set_max_capacity](#-cont_set_capacity-)
- [cont_set_growth_factor](#-cont_set_capacity-)

### Getting elements

- [cot_ITEM](#-cont_item-)
- [cont_get](#-cont_get-)
- [cont_pop](#-cont_pop-)
- [cont_cv](#-cont_cv-)
- [cont_split](#-cont_split-)
- [cont_sub](#-cont_sub-)
- [cont_clone](#-cont_clone-)

### Adding and modifying elements

- [cont_ITEM](#-cont_item--1)
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

### ** **cont_NEW** **

```c
cont_NEW(type, capacity);
```

Macro wrapper around [cont_new](#-cont_new-) for the ease of use.
Creates a new **cont** instance and returns it by value.
The `capacity` must be greater than `0`.

* **Return (success):** 

  * New container instance
  
* **Return (failure):** 

  * `INVALID_CONT` - `(!capacity || capacity > SIZE_MAX / unit)` or malloc failure

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_new** **

```c
cont cont_new(size_t capacity, size_t unit, size_t alignment);
```

Creates a new **cont** instance and returns it by value.
- `capacity` argument must be greater than `0`. 
- `unit` argument should be `sizeof(type)`
- `alignment` argument should be `_Alignof(type)`

It's easier to use [cont_NEW](#-cont_new-) and leave this function for internal usage.

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
int cont_free(cont* cnt);
```

Frees the buffer allocated by a container and turns the container into an `INVALID_CONT`.

* **Return (success):** 

  * `0`
  
* **Return (failure):** 
  
  * `CONT_IS_NULL` - `(cnt == NULL)`
  * `CONT_ALREADY_FREED` - `(!.addr)`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

## Validation

### ** **cont_is_valid** **

```c
int cont_is_valid(cont* cnt);
```

Validates container fields.

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
int cont_set_count(cont* cnt, size_t count);
```

Sets container count. If the passed number exceeds `.capacity`, [cont_grow](#-cont_grow-) is called.

* **Return (success):** 

  * `0`

* **Return (failure):** 
  
  * `CONT_IS_NULL` - `(cnt == NULL)`
  * [cont_grow](#-cont_grow-) error codes

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_set_capacity** **

```c
int cont_set_capacity(cont* cnt, size_t capacity);
```

Sets container capacity (`.capacity`).
Shrinking below the current value will truncate elements that stick out. Reallocs the buffer freeing or increasing allocated memory (which is always equal to `.capacity * .unit`).

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `CONT_IS_NULL` (`cnt == NULL`)
  * `NULL_CAPACITY` - `(capacity == 0)`
  * `SIZE_OVERFLOW` - `(capacity > SIZE_MAX / .unit)`
  * `MAX_CAPACITY_EXCEEDED`
  * `REALLOC_FAILURE`
  * `ALIGNED_ALLOC_FAILURE`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_set_max_capacity** **

```c
int cont_set_max_capacity(cont* cnt, size_t max_size);
```

Sets container's `.max_capacity`. If the passed argument is smaller than current `.capacity`, the buffer is truncated and `.capacity` (possibly also the `.count`) is set equal to the passed argument.
Passing `0` / `cont_NO_LIMIT` sets **unlimited** max capacity.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `CONT_IS_NULL` (`cnt == NULL`)
  * `SIZE_OVERFLOW` - `(max_size > SIZE_MAX / .unit)`
  * `REALLOC_FAILURE`
  * `ALIGNED_ALLOC_FAILURE`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_set_growth_factor** **

```c
int cont_set_growth_factor(cont* cnt, double growth_factor);`
```

Sets container growth factor. Valid range is by default defined by the values:
```c
// cont.h

#define cont_GF_LBOUND 1.0
#define cont_GF_UBOUND 10.0
```

* **Return (success):** 

* `0`

* **Return (failure):** 

  * `CONT_IS_NULL` (`cnt == NULL`)
  * `INVALID_GROWTH_FACTOR_VALUE`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

## Getting Elements

### ** **cont_ITEM** **

```c
cont_ITEM(cont_ptr, index, type);
```

Special macro for bypassing API and getting straight to the element at a given index:
```c
// cont.h

#define cont_ITEM(cnt, index, type) 					  \
                ((type*)cnt->addr)[index]
```
No safety mechanisms, use with caution.

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_get** **

```c
void* cont_get(cont* cnt, size_t index);
```

Returns pointer to the element at a given index. If index is invalid, it returns `NULL`.

* **Return (success):** 

  * Pointer to element

* **Return (failure):** 
  
  * `NULL` - `(index >= .count)`, `(cnt == NULL)`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_pop** **

```c
void* cont_pop(cont* cnt);
```

Returns pointer to the last element and decreases container count by 1. If `.count == 0`, it returns `NULL` and exits.

* **Return (success):** 

  * Pointer to element

* **Return (failure):**
  
  * `NULL` - `(.count == 0)`, `(cnt == NULL)`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_cv** **

```c
int cont_cv(cont* cnt, size_t index, void* buffer, size_t n);
```

Copies `n` elements into a given `buffer` starting at `index`.
Passing `n == 0` / `n == cont_ALL` results in copying all elements from `index` to the end of a container. Passed `buffer` cannot overlap with container's buffer.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `CONT_IS_NULL` `(cnt == NULL)`
  * `NULL_ARRAY_POINTER` - `(buffer == NULL)`
  * `INVALID_INDEX` - `(index >= .count)`
  * `INVALID_RANGE` - `(n > .count - index)`
  * `BUFFER_OVERLAP`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_split** **

```c
cont cont_split(cont* cnt, size_t index);
```

Splits a container in two at a given index and returns the one starting at that index.
If the index is invalid or if memory allocation/reallocation fails, it returns `INVALID_CONT`.

* **Return (success):** 
  
  * Subcontainer

* **Return (failure):** 
  
  * `INVALID_CONT` - `(cnt == NULL)`, `(index == 0 || index >= .count)`, malloc/realloc/aligned alloc failure

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_sub** **

```c
cont cont_sub(cont* cnt, size_t index, size_t n);
```

Returns a subcontainer with n elements starting at a given index without modifying the original.
Passing `n == 0` / `n == cont_ALL` results in taking all elements from the index to the end.

* **Return (success):** 

  * Subcontainer

* **Return (failure):** 
  
  * `INVALID_CONT` - `(cnt == NULL)`, `(index >= .count)`, `(n > .count - index)`, malloc/aligned alloc failure

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_clone** **

```c
cont cont_clone(cont* cnt);
```

Returns an exact copy of the container. On failure, it returns `INVALID_CONT`.

* **Return (success):** 

  * Cloned container

* **Return (failure):** 

  * `INVALID_CONT` - `(cnt == NULL)`, malloc/aligned alloc failure

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

## Adding elements / setting values

### ** **cont_ITEM** **

```c
cont_ITEM(cont_ptr, index, type);
```

Special macro for bypassing API and getting straight to the element at a given index:
```c
// cont.h

#define cont_ITEM(cnt, index, type) 					  \
                ((type*)cnt->addr)[index]
```
No safety mechanisms, use with caution.

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_set** **

```c
int cont_set(cont* cnt, size_t index, void* item);
```

Sets the value of an element at a given index. 

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `CONT_IS_NULL` - `(cnt == NULL)`
  * `INVALID_INDEX` - `(index >= .count)`
  * `NULL_ITEM_POINTER` - `(item == NULL)`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_push** **

```c
int cont_push(cont* cnt, void* item);
```

Adds an element at the end of a container, `item` cannot point to the inside of the container.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `CONT_IS_NULL` - `(cnt == NULL)`
  * `NULL_ITEM_POINTER` - `(item ==  NULL)`
  * `MAX_CAPACITY_EXCEEDED`
  * `BUFFER_OVERLAP` - if `item` points to the inside of the container
  * [cont_grow](#-cont_grow-) error codes

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_push_front** **

```c
int cont_push_front(cont* cnt, void* item);
```

Adds an element at the first index. If a container is not empty, it shifts all existing elements to the right. The `item` cannot point to the inside of the container.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `CONT_IS_NULL` - `(cnt == NULL)`
  * `NULL_ITEM_POINTER` - `(!item)`
  * `MAX_CAPACITY_EXCEEDED`
  * `BUFFER_OVERLAP` - if `item` points to the inside of the container
  * [cont_grow](#-cont_grow-) error codes

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_append** **

```c
int cont_append(cont* cnt, void* arr, size_t n);
```

Appends `n` elements from a given array to the container. The array cannot overlap with the container.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `CONT_IS_NULL` - `(cnt == NULL)`
  * `NULL_ARRAY_POINTER` - `(arr == NULL)`
  * `NULL_ELEMENT_COUNT` - `(n == 0)`
  * `SIZE_OVERFLOW` - `(.count > SIZE_MAX - n)`
  * `BUFFER_OVERLAP`
  * `MAX_CAPACITY_EXCEEDED`
  * [cont_grow](#-cont_grow-) error codes

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_prepend** **

```c
int cont_prepend(cont* cnt, void* arr, size_t n);
```

Prepends `n` elements from a given array to the container, shifting any existing elements to the right. The array cannot overlap with the container.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `CONT_IS_NULL` - `(cnt == NULL)`
  * `NULL_ARRAY_POINTER` - `(arr == NULL)`
  * `NULL_ELEMENT_COUNT` - `(n == 0)`
  * `SIZE_OVERFLOW` - `(.count > SIZE_MAX - n)`
  * `MAX_CAPACITY_EXCEEDED`
  * `BUFFER_OVERLAP`
  * [cont_grow](#-cont_grow-) error codes

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_extend** **

```c
int cont_extend(cont* cnt, cont* cnt2);
```

Extends a container by appending all elements from another container. The pointers must point to different containers and their `.unit`'s must match.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `CONT_IS_NULL` - `(cnt == NULL)`
  * `SAME_CONT` - `(cont_ == cont_2)`
  * `UNIT_MISMATCH` - `(cont_.unit != cont_2.unit)`
  * [cont_grow](#-cont_grow-) error codes

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_write** **

```c
int cont_write(cont* cnt, size_t index, void* arr, size_t num_of_items)
```

Writes `num_of_items` elements from a given array to a container, starting at a given index inside the container (can also start at last index + 1, appending elements). Overwrites existing elements. The array cannot overlap with the container.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `CONT_IS_NULL` - `(cnt == NULL)`
  * `NULL_ARRAY_POINTER` - `(arr == NULL)`
  * `NULL_ELEMENT_COUNT` - `(num == 0)`
  * `INVALID_INDEX` - `(index > .count)`
  * `SIZE_OVERFLOW` - `(index > SIZE_MAX - num)`, `(num > SIZE_MAX / .unit)`
  * `MAX_CAPACITY_EXCEEDED` 
  * `BUFFER_OVERLAP`
  * [cont_grow](#-cont_grow-) error codes

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_insert** **

```c
int cont_insert(cont* cnt, size_t index, void* item);
```

Inserts an element at a specified index, shifting any existing elements to the right. The item cannot point to the inside of the container.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `CONT_IS_NULL` - `(cnt == NULL)`
  * `NULL_ITEM_POINTER` - `(!item)`
  * `INVALID_INDEX` - `(index > .count)`
  * `MAX_CAPACITY_EXCEEDED`
  * `BUFFER_OVERLAP`
  * [cont_grow](#-cont_grow-) errors

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_insert_range** **

```c
int cont_insert_range(cont* cnt, size_t index, void* arr, size_t num_of_items)
```

Inserts `num_of_items` elements from a given array at a specified index shifting existing elements to the right. The array cannot overlap with the container.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `CONT_IS_NULL` - `(cnt == NULL)`
  * `NULL_ARRAY_POINTER` - `(!arr)`
  * `NULL_ELEMENT_COUNT` - `(!num)`
  * `INVALID_INDEX` - `(index > .count)`
  * `SIZE_OVERFLOW` - `(.count > SIZE_MAX - num)`, `(num > SIZE_MAX / .unit)`
  * `MAX_CAPACITY_EXCEEDED`
  * `BUFFER_OVERLAP`
  * [cont_grow](#-cont_grow-) error codes

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

## Resizing / deleting elements

### ** **cont_set_space** **

```c
int cont_set_space(cont* cnt, size_t n);
```

Sets the free space between the last element and the end of a container capacity: `.capacity - .count`.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `CONT_IS_NULL` - `(cnt == NULL)`
  * `EMPTY_CONT` - `(.count == 0)`
  * `SIZE_OVERFLOW` - `(.count > SIZE_MAX - n)`, `(new_capacity > SIZE_MAX / .unit)`
  * `MAX_CAPACITY_EXCEEDED`
  * `REALLOC_FAILURE`
  * `ALIGNED_ALLOC_FAILURE`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_grow** **

```c
int cont_grow(cont* cnt, size_t required_capacity);
```

Increases `.capacity` according to the formula: 
```c
capacity *= growth_factor 
```
until the value is greater or equal to `required_capacity`. This function is used internally by methods that increase a container's `.capacity`.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `CONT_IS_NULL` - `(cnt == NULL)`
  * `SIZE_OVERFLOW` - `(final_capacity_ > SIZE_MAX / unit)`
  * `REALLOC_FAILURE`
  * `ALIGNED_ALLOC_FAILURE`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_shrink** **

```c
int cont_shrink(cont* cnt);
```

Sets `.capacity` equal to `.count`. Equivalent to calling: `cont_set_space(&cnt, 0)`. Reallocs the buffer freeing unneeded memory.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `CONT_IS_NULL` - `(cnt == NULL)`
  * `EMPTY_CONT` - `(!.count)`
  * `REALLOC_FAILURE`
  * `ALIGNED_ALLOC_FAILURE`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_remove** **

```c
int cont_remove(cont* cnt, size_t index);
```

Removes an element at a specified index shifting any following elements to the left.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `CONT_IS_NULL` - `(cnt == NULL)`
  * `INVALID_INDEX` - `(index >= .count)`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_remove_range** **

```c
int cont_remove_range(cont* cnt, size_t index, size_t n);
```

Removes `n` elements starting at specified index shifting any following elements to the left.
If `n == 0` / `n == cont_ALL`, all elements from the specified index onwards are removed.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `CONT_IS_NULL` - `(cnt == NULL)`
  * `INVALID_INDEX` - `(index >= .count)`
  * `INVALID_RANGE` - `(n > .count - index)`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_clear** **

```c
int cont_clear(cont* cnt);
```

Sets `.count` to 0 making a container empty. Does not affect `.capacity`.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `CONT_IS_NULL` - `(cnt == NULL)`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>

---

### ** **cont_collapse** **

```c
int cont_collapse(cont* cnt);
```

Sets `.capacity` to 1 and `.count` to 0. Reallocs the buffer freeing unneeded memory.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `CONT_IS_NULL` - `(cnt == NULL)`
  * `REALLOC_FAILURE`
  * `ALIGNED_ALLOC_FAILURE`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

## Special

### ** **cont_reverse** **

```c
int cont_reverse(cont* cnt);
```

Reverses the order of elements in a container.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `CONT_IS_NULL` - `(cnt == NULL)`
  * `EMPTY_CONT` - `(!.count)`
  * `MALLOC_FAILURE`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

### ** **cont_set_blank** **

```c
int cont_set_blank(cont* cnt, size_t position, size_t n);
```

Sets `n` bytes to 0 starting at a specified position. The range: `position + n` must be within the container’s buffer.
If `n == 0`, all bytes from the specified position to the end of the buffer are 0'ed.

* **Return (success):** 

  * `0`

* **Return (failure):**

  * `CONT_IS_NULL` - `(cnt == NULL)`
  * `INVALID_INDEX` - `(position >= .capacity)`
  * `INVALID_RANGE` - `(n > .capacity - position)`

<p align="right">
<a href="#full-method-list">GO TO METHOD LIST ^</a>
</p>
  
---

## Error codes

Full list of error codes from cont.h:

```c
// cont.h

enum error_codes
{
	INVALID_INDEX = 1,
	SIZE_OVERFLOW,
	NULL_ARRAY_POINTER,
	REALLOC_FAILURE,
	MALLOC_FAILURE,
	ALIGNED_ALLOC_FAILURE,
	NULL_ITEM_POINTER,
	NULL_ELEMENT_COUNT,
	NULL_CAPACITY,
	MAX_CAPACITY_EXCEEDED,
	INVALID_GROWTH_FACTOR_VALUE,
	CONT_ALREADY_FREED,
	EMPTY_CONT,
	UNIT_MISMATCH,
	SAME_CONT,
	INVALID_RANGE,
	CONT_IS_INVALID,
	CONT_IS_NULL,
	BUFFER_OVERLAP
};
```

<p align="right">
<a href="#table-of-contents">GO TO TOP ^</a>
</p>
