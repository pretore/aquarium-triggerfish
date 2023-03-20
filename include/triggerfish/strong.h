#ifndef _TRIGGERFISH_STRONG_H_
#define _TRIGGERFISH_STRONG_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sea-urchin.h>

#define TRIGGERFISH_STRONG_ERROR_OBJECT_IS_NULL \
    SEA_URCHIN_ERROR_OBJECT_IS_NULL
#define TRIGGERFISH_STRONG_ERROR_INSTANCE_IS_NULL \
    SEA_URCHIN_ERROR_VALUE_IS_NULL
#define TRIGGERFISH_STRONG_ERROR_ON_DESTROY_IS_NULL \
    SEA_URCHIN_ERROR_FUNCTION_IS_NULL
#define TRIGGERFISH_STRONG_ERROR_MEMORY_ALLOCATION_FAILED \
    SEA_URCHIN_ERROR_MEMORY_ALLOCATION_FAILED
#define TRIGGERFISH_STRONG_ERROR_OBJECT_IS_INVALID \
    SEA_URCHIN_ERROR_VALUE_IS_INVALID
#define TRIGGERFISH_STRONG_ERROR_OUT_IS_NULL \
    SEA_URCHIN_ERROR_OUT_IS_NULL

struct triggerfish_strong;

/**
 * @brief Create new strong reference.
 * @param [in] instance whose lifetime will be managed by the strong reference.
 * @param [in] on_destroy which will be invoked with the reference is being
 * destroyed.
 * @param [out] out receive the newly created strong reference.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws TRIGGERFISH_STRONG_ERROR_INSTANCE_IS_NULL if instance is <i>NULL</i>.
 * @throws TRIGGERFISH_STRONG_ERROR_ON_DESTROY_IS_NULL if on_destroy is
 * <i>NULL</i>.
 * @throws TRIGGERFISH_STRONG_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws TRIGGERFISH_STRONG_ERROR_MEMORY_ALLOCATION_FAILED if there is not
 * enough memory to create the strong reference.
 * @note <b>out</b> must be released once done with it.
 */
int triggerfish_strong_of(void *instance,
                          void (*on_destroy)(void *instance),
                          struct triggerfish_strong **out);

/**
 * @brief Retrieve the reference count.
 * @param [in] object strong reference.
 * @param [out] out receive the reference count.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws TRIGGERFISH_STRONG_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws TRIGGERFISH_STRONG_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 */
int triggerfish_strong_count(struct triggerfish_strong *object,
                             uintmax_t *out);

/**
 * @brief Increase the reference count.
 * @param [in] object strong reference.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws TRIGGERFISH_STRONG_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws TRIGGERFISH_STRONG_ERROR_OBJECT_IS_INVALID if the strong reference
 * has been invalidated.
 */
int triggerfish_strong_retain(struct triggerfish_strong *object);

/**
 * @brief Decrease the reference count.
 * @param [in] object strong reference.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws TRIGGERFISH_STRONG_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 */
int triggerfish_strong_release(struct triggerfish_strong *object);

/**
 * @brief Retrieve referenced object instance.
 * @param [in] object strong reference.
 * @param [out] out receive referenced object instance.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws TRIGGERFISH_STRONG_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws TRIGGERFISH_STRONG_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws TRIGGERFISH_STRONG_ERROR_OBJECT_IS_INVALID if the strong reference
 * instance has been invalidated.
 */
int triggerfish_strong_instance(const struct triggerfish_strong *object,
                                void **out);

#endif /* _TRIGGERFISH_STRONG_H_ */
