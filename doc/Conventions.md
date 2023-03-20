## Functions

### Name

All functions in this library will start with ``triggerfish_``. 

Functions that get and set values must contain ``get`` or ``set`` in 
their name unless you can only get a value then the prefix is omitted.

### Return Types

If there is no possibility of failure then the function must have a
``void`` return type.

```c
void triggerfish_required(const void *object);
```

If the function can fail then the return type must be an ``int`` where
``0`` means that the function completed without error.

```c
int triggerfish_add_size_t(const size_t a, const size_t b, size_t *out)
```

There are a few exceptions to this rule where already established
conventions makes more sense for example when an ``int`` return type is
used for comparison functions.

```c
int memcmp(const void *s1, const void *s2, size_t n);
```

### Errors

Errors are reported by returning the appropriate error code.

```c
int triggerfish_add_size_t(const size_t a, const size_t b, size_t *out) {
    if (!out) {
        return TRIGGERFISH_ERROR_OUT_IS_NULL;
    }
    ...
```

It is required to remap error codes from ``aquarium-sea-urchin`` to domain
specific errors.

```c
#define TRIGGERFISH_ERROR_OUT_IS_NULL SEA_URCHIN_ERROR_OUT_IS_NULL
```
