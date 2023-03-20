#ifndef _TRIGGERFISH_WEAK_H_
#define _TRIGGERFISH_WEAK_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sea-urchin.h>

#define TRIGGERFISH_WEAK_ERROR_OBJECT_IS_NULL \
    SEA_URCHIN_ERROR_OBJECT_IS_NULL
#define TRIGGERFISH_WEAK_ERROR_STRONG_IS_NULL \
    SEA_URCHIN_ERROR_VALUE_IS_NULL
#define TRIGGERFISH_WEAK_ERROR_STRONG_IS_INVALID \
    SEA_URCHIN_ERROR_VALUE_IS_INVALID
#define TRIGGERFISH_WEAK_ERROR_MEMORY_ALLOCATION_FAILED \
    SEA_URCHIN_ERROR_MEMORY_ALLOCATION_FAILED
#define TRIGGERFISH_WEAK_ERROR_OUT_IS_NULL \
    SEA_URCHIN_ERROR_OUT_IS_NULL
#define TRIGGERFISH_WEAK_ERROR_OTHER_IS_NULL \
    SEA_URCHIN_ERROR_OTHER_IS_NULL

struct triggerfish_strong;
struct triggerfish_weak;

/**
 * @brief Create new weak reference.
 * @param [in] strong from which a weak reference is to be created.
 * @param [out] out receive the newly created weak reference.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws TRIGGERFISH_WEAK_ERROR_STRONG_IS_NULL if strong is <i>NULL</i>.
 * @throws TRIGGERFISH_WEAK_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws TRIGGERFISH_WEAK_ERROR_MEMORY_ALLOCATION_FAILED if there is
 * insufficient memory to create the weak reference.
 * @throws TRIGGERFISH_WEAK_ERROR_STRONG_IS_INVALID if the strong reference
 * was invalidated.
 */
int triggerfish_weak_of(struct triggerfish_strong *strong,
                        struct triggerfish_weak **out);

/**
 * @brief Create copy of weak reference.
 * @param [in] other from which a copy is to be be created.
 * @param [out] out receive the copy.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws TRIGGERFISH_WEAK_ERROR_OTHER_IS_NULL if other is <i>NULL</i>.
 * @throws TRIGGERFISH_WEAK_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws TRIGGERFISH_WEAK_ERROR_MEMORY_ALLOCATION_FAILED if there is
 * insufficient memory to copy the weak reference.
 */
int triggerfish_weak_copy_of(const struct triggerfish_weak *other,
                             struct triggerfish_weak **out);

/**
 * @brief Destroy a weak reference.
 * @param [in] object weak reference instance.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws TRIGGERFISH_WEAK_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 */
int triggerfish_weak_destroy(struct triggerfish_weak *object);

/**
 * @brief Receive strong reference for the weak reference.
 * @param [in] object weak reference instance.
 * @param [out] out receive the strong reference.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws TRIGGERFISH_WEAK_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws TRIGGERFISH_WEAK_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws TRIGGERFISH_WEAK_ERROR_STRONG_IS_INVALID if the strong reference was
 * invalidated.
 * @note <b>out</b> must be released once done with it.
 */
int triggerfish_weak_strong(const struct triggerfish_weak *object,
                            struct triggerfish_strong **out);

#endif /* _TRIGGERFISH_WEAK_H_ */
