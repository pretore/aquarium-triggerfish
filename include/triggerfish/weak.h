#ifndef _TRIGGERFISH_WEAK_H_
#define _TRIGGERFISH_WEAK_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define TRIGGERFISH_WEAK_ERROR_OBJECT_IS_NULL                       1
#define TRIGGERFISH_WEAK_ERROR_STRONG_IS_NULL                       2
#define TRIGGERFISH_WEAK_ERROR_STRONG_IS_INVALID                    3
#define TRIGGERFISH_WEAK_ERROR_MEMORY_ALLOCATION_FAILED             4
#define TRIGGERFISH_WEAK_ERROR_OUT_IS_NULL                          5

struct triggerfish_strong;
struct triggerfish_weak;

/**
 * @brief Create new weak reference.
 * @param [in] strong from which a weak reference is to be created.
 * @param [out] out receive the newly created weak reference.
 * @return On success true, otherwise false if an error has occurred.
 * @throws TRIGGERFISH_WEAK_ERROR_STRONG_IS_NULL if strong is <i>NULL</i>.
 * @throws TRIGGERFISH_WEAK_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws TRIGGERFISH_WEAK_ERROR_MEMORY_ALLOCATION_FAILED if there is
 * insufficient memory to create the weak reference.
 * @throws TRIGGERFISH_WEAK_ERROR_STRONG_IS_INVALID if the strong reference
 * instance has been invalidated.
 */
bool triggerfish_weak_of(struct triggerfish_strong *strong,
                         struct triggerfish_weak **out);

/**
 * @brief Destroy a weak reference.
 * @param [in] object weak reference instance.
 * @return On success true, otherwise false if an error has occurred.
 * @throws TRIGGERFISH_WEAK_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 */
bool triggerfish_weak_destroy(struct triggerfish_weak *object);

/**
 * @brief Receive strong reference for the weak reference.
 * @param [in] object weak reference instance.
 * @param [out] out receive the strong reference.
 * @return On success true, otherwise false if an error has occurred.
 * @throws TRIGGERFISH_WEAK_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws TRIGGERFISH_WEAK_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws TRIGGERFISH_WEAK_ERROR_STRONG_IS_INVALID if the strong reference was
 * invalidated.
 * @note <b>out</b> must be released once done with it.
 */
bool triggerfish_weak_strong(const struct triggerfish_weak *object,
                             struct triggerfish_strong **out);

#endif /* _TRIGGERFISH_WEAK_H_ */
