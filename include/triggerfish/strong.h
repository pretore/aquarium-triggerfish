#ifndef _TRIGGERFISH_STRONG_H_
#define _TRIGGERFISH_STRONG_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define TRIGGERFISH_STRONG_ERROR_OBJECT_IS_NULL                     1
#define TRIGGERFISH_STRONG_ERROR_INSTANCE_IS_NULL                   2
#define TRIGGERFISH_STRONG_ERROR_ON_DESTROY_IS_NULL                 3
#define TRIGGERFISH_STRONG_ERROR_MEMORY_ALLOCATION_FAILED           4
#define TRIGGERFISH_STRONG_ERROR_OBJECT_IS_INVALID                  5
#define TRIGGERFISH_STRONG_ERROR_OUT_IS_NULL                        6

struct triggerfish_strong;

/**
 * @brief Create new strong reference.
 * @param [in] instance whose lifetime will be managed by the strong reference.
 * @param [in] on_destroy which will be invoked with the reference is being
 * destroyed.
 * @param [out] out receive the newly created strong reference.
 * @return On success true, otherwise false if an error has occurred.
 * @throws TRIGGERFISH_STRONG_ERROR_INSTANCE_IS_NULL if instance is <i>NULL</i>.
 * @throws TRIGGERFISH_STRONG_ERROR_ON_DESTROY_IS_NULL if on_destroy is
 * <i>NULL</i>.
 * @throws TRIGGERFISH_STRONG_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws TRIGGERFISH_STRONG_ERROR_MEMORY_ALLOCATION_FAILED if there is not
 * enough memory to create the strong reference.
 */
bool triggerfish_strong_of(void *instance,
                           void (*on_destroy)(void *instance),
                           struct triggerfish_strong **out);

/**
 * @brief Retrieve the reference count.
 * @param [in] object strong reference.
 * @param [out] out receive the reference count.
 * @return On success true, otherwise false if an error has occurred.
 * @throws TRIGGERFISH_STRONG_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws TRIGGERFISH_STRONG_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 */
bool triggerfish_strong_count(struct triggerfish_strong *object,
                              uintmax_t *out);

/**
 * @brief Increase the reference count.
 * @param [in] object strong reference.
 * @return On success true, otherwise false if an error has occurred.
 * @throws TRIGGERFISH_STRONG_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws TRIGGERFISH_STRONG_ERROR_OBJECT_IS_INVALID if the strong reference
 * has been invalidated.
 */
bool triggerfish_strong_retain(struct triggerfish_strong *object);

/**
 * @brief Decrease the reference count.
 * @param [in] object strong reference.
 * @return On success true, otherwise false if an error has occurred.
 * @throws TRIGGERFISH_STRONG_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 */
bool triggerfish_strong_release(struct triggerfish_strong *object);

/**
 * @brief Retrieve referenced object instance.
 * @param [in] object strong reference.
 * @param [out] out receive referenced object instance.
 * @return On success true, otherwise false if an error has occurred.
 * @throws TRIGGERFISH_STRONG_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws TRIGGERFISH_STRONG_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws TRIGGERFISH_STRONG_ERROR_OBJECT_IS_INVALID if the strong reference
 * instance has been invalidated.
 */
bool triggerfish_strong_instance(const struct triggerfish_strong *object,
                                 void **out);

#endif /* _TRIGGERFISH_STRONG_H_ */
