#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <errno.h>
#include <string.h>
#include <triggerfish.h>

#include "private/strong.h"
#include "private/weak.h"

#include <test/cmocka.h>

static void check_destroy_error_on_object_is_null(void **state) {
    assert_int_equal(
            triggerfish_weak_destroy(NULL),
            TRIGGERFISH_WEAK_ERROR_OBJECT_IS_NULL);
}

static void check_destroy(void **state) {
    struct triggerfish_weak *object = calloc(1, sizeof(*object));
    assert_int_equal(triggerfish_weak_destroy(object), 0);
}

static void on_destroy(void *instance) {
    assert_non_null(instance);
    function_called();
}

static void check_destroy_error_on_low_memory_situation(void **state) {
    struct triggerfish_strong *strong;
    assert_int_equal(triggerfish_strong_of(malloc(1), on_destroy, &strong), 0);
    struct triggerfish_weak *object;
    assert_int_equal(triggerfish_weak_of(strong, &object), 0);
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = true;
    assert_int_equal(triggerfish_weak_destroy(object), 0);
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = false;
    expect_function_call(on_destroy);
    assert_int_equal(triggerfish_strong_release(strong), 0);
}

static void check_of_error_on_strong_is_null(void **state) {
    assert_int_equal(
            triggerfish_weak_of(NULL, (void *) 1),
            TRIGGERFISH_WEAK_ERROR_STRONG_IS_NULL);
}

static void check_of_error_on_out_is_null(void **state) {
    assert_int_equal(
            triggerfish_weak_of((void *) 1, NULL),
            TRIGGERFISH_WEAK_ERROR_OUT_IS_NULL);
}

static void check_of_error_on_memory_allocation_failed(void **state) {
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = true;
    assert_int_equal(
            triggerfish_weak_of((void *) 1, (void *) 1),
            TRIGGERFISH_WEAK_ERROR_MEMORY_ALLOCATION_FAILED);
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = false;
}

static void check_of_error_on_strong_is_invalid(void **state) {
    struct triggerfish_strong strong = {};
    struct triggerfish_weak *out;
    pthread_mutex_lock_is_overridden = true;
    will_return(cmocka_test_pthread_mutex_lock, EINVAL);
    assert_int_equal(
            triggerfish_weak_of(&strong, &out),
            TRIGGERFISH_WEAK_ERROR_STRONG_IS_INVALID);
    pthread_mutex_lock_is_overridden = false;
}

static void check_of(void **state) {
    void *instance = malloc(1);
    struct triggerfish_strong *strong;
    assert_int_equal(triggerfish_strong_of(instance, on_destroy, &strong), 0);
    struct triggerfish_weak *out;
    assert_int_equal(triggerfish_weak_of(strong, &out), 0);
    assert_non_null(out);
    assert_ptr_equal(atomic_load(&out->strong), strong);
    expect_function_call(on_destroy);
    assert_int_equal(triggerfish_strong_release(strong), 0);
    assert_ptr_equal(atomic_load(&out->strong), 0);
    assert_int_equal(triggerfish_weak_destroy(out), 0);
}

static void check_copy_of_error_on_other_is_null(void **state) {
    assert_int_equal(
            triggerfish_weak_copy_of(NULL, (void *) 1),
            TRIGGERFISH_WEAK_ERROR_OTHER_IS_NULL);
}

static void check_copy_of_error_on_out_is_null(void **state) {
    assert_int_equal(
            triggerfish_weak_copy_of((void *) 1, NULL),
            TRIGGERFISH_WEAK_ERROR_OUT_IS_NULL);
}

static void check_copy_of_error_on_memory_allocation_failed(void **state) {
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = true;
    assert_int_equal(
            triggerfish_weak_copy_of((void *) 1, (void *) 1),
            TRIGGERFISH_WEAK_ERROR_MEMORY_ALLOCATION_FAILED);
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = false;
}

static void check_copy_of(void **state) {
    void *instance = malloc(1);
    struct triggerfish_strong *strong;
    assert_int_equal(triggerfish_strong_of(instance, on_destroy, &strong), 0);
    struct triggerfish_weak *out;
    assert_int_equal(triggerfish_weak_of(strong, &out), 0);
    struct triggerfish_weak *copy;
    assert_int_equal(triggerfish_weak_copy_of(out, &copy), 0);
    assert_int_equal(atomic_load(&copy->strong), atomic_load(&out->strong));
    assert_ptr_equal(atomic_load(&copy->strong), strong);
    expect_function_call(on_destroy);
    assert_int_equal(triggerfish_strong_release(strong), 0);
    assert_ptr_equal(atomic_load(&out->strong), 0);
    assert_ptr_equal(atomic_load(&copy->strong), 0);
    assert_int_equal(triggerfish_weak_destroy(out), 0);
    assert_int_equal(triggerfish_weak_destroy(copy), 0);
}

static void check_strong_error_on_object_is_null(void **state) {
    assert_int_equal(
            triggerfish_weak_strong(NULL, (void *) 1),
            TRIGGERFISH_WEAK_ERROR_OBJECT_IS_NULL);
}

static void check_strong_error_on_out_is_null(void **state) {
    assert_int_equal(
            triggerfish_weak_strong((void *) 1, NULL),
            TRIGGERFISH_WEAK_ERROR_OUT_IS_NULL);
}

static void check_strong_error_strong_is_invalid(void **state) {
    struct triggerfish_weak object = {};
    assert_int_equal(
            triggerfish_weak_strong(&object, (void *) 1),
            TRIGGERFISH_WEAK_ERROR_STRONG_IS_INVALID);
}

static void check_strong(void **state) {
    void *instance = malloc(1);
    struct triggerfish_strong *strong;
    assert_int_equal(triggerfish_strong_of(instance, on_destroy, &strong), 0);
    struct triggerfish_weak *object;
    assert_int_equal(triggerfish_weak_of(strong, &object), 0);
    assert_non_null(object);
    struct triggerfish_strong *out;
    assert_ptr_equal(atomic_load(&strong->counter), 1);
    assert_int_equal(triggerfish_weak_strong(object, &out), 0);
    assert_ptr_equal(atomic_load(&strong->counter), 2);
    assert_ptr_equal(strong, out);
    assert_int_equal(triggerfish_strong_release(strong), 0);
    expect_function_call(on_destroy);
    assert_int_equal(triggerfish_strong_release(strong), 0);
    assert_ptr_equal(atomic_load(&object->strong), 0);
    assert_int_equal(triggerfish_weak_destroy(object), 0);
}

int main(int argc, char *argv[]) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(check_destroy_error_on_object_is_null),
            cmocka_unit_test(check_destroy),
            cmocka_unit_test(check_destroy_error_on_low_memory_situation),
            cmocka_unit_test(check_of_error_on_strong_is_null),
            cmocka_unit_test(check_of_error_on_out_is_null),
            cmocka_unit_test(check_of_error_on_memory_allocation_failed),
            cmocka_unit_test(check_of_error_on_strong_is_invalid),
            cmocka_unit_test(check_of),
            cmocka_unit_test(check_copy_of_error_on_other_is_null),
            cmocka_unit_test(check_copy_of_error_on_out_is_null),
            cmocka_unit_test(check_copy_of_error_on_memory_allocation_failed),
            cmocka_unit_test(check_copy_of),
            cmocka_unit_test(check_strong_error_on_object_is_null),
            cmocka_unit_test(check_strong_error_on_out_is_null),
            cmocka_unit_test(check_strong_error_strong_is_invalid),
            cmocka_unit_test(check_strong),
    };
    //cmocka_set_message_output(CM_OUTPUT_XML);
    return cmocka_run_group_tests(tests, NULL, NULL);
}
