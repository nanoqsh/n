# n
The nano "template" free data structure library.

## Examples
Data structures don't know the size of the stored data, so in most cases you need to explicitly pass `sizeof(type)`.
For example, consider the vector of an allocated array called `vec`.

```c
// Creation of a new vec
vec v = vec_new();

// .. with initial capacity
vec v = vec_with_cap(6, sizeof(u32));

// Push data in vec
u32 n = 12;
vec_push(&v, &i, sizeof(u32));

// Pop data from vec
u32 *val = vec_pop(&v, sizeof(u32));

// Drop the vec
vec_drop(&v);
```

There are also more additional structures such as `slice`, `lmap`, `box`, etc.
