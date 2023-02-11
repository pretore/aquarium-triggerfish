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
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    assert_false(triggerfish_weak_destroy(NULL));
    assert_int_equal(TRIGGERFISH_WEAK_ERROR_OBJECT_IS_NULL, triggerfish_error);
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_destroy(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    struct triggerfish_weak *object = calloc(1, sizeof(*object));
    assert_true(triggerfish_weak_destroy(object));
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void on_destroy(void *instance) {
    assert_non_null(instance);
    function_called();
}

static void check_destroy_error_on_low_memory_situation(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    struct triggerfish_strong *strong;
    assert_true(triggerfish_strong_of(malloc(1), on_destroy, &strong));
    struct triggerfish_weak *object;
    assert_true(triggerfish_weak_of(strong, &object));
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = true;
    assert_true(triggerfish_weak_destroy(object));
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = false;
    expect_function_call(on_destroy);
    assert_true(triggerfish_strong_release(strong));
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_of_error_on_strong_is_null(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    assert_false(triggerfish_weak_of(NULL, (void *)1));
    assert_ptr_equal(TRIGGERFISH_WEAK_ERROR_STRONG_IS_NULL, triggerfish_error);
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_of_error_on_out_is_null(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    assert_false(triggerfish_weak_of((void *)1, NULL));
    assert_ptr_equal(TRIGGERFISH_WEAK_ERROR_OUT_IS_NULL, triggerfish_error);
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_of_error_on_memory_allocation_failed(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden = true;
    assert_false(triggerfish_weak_of((void *)1, (void *)1));
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden = false;
    assert_int_equal(TRIGGERFISH_WEAK_ERROR_MEMORY_ALLOCATION_FAILED,
                     triggerfish_error);
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_of_error_on_strong_is_invalid(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    struct triggerfish_strong strong = {};
    struct triggerfish_weak *out;
    pthread_mutex_lock_is_overridden = true;
    will_return(cmocka_test_pthread_mutex_lock, EINVAL);
    assert_false(triggerfish_weak_of(&strong, &out));
    pthread_mutex_lock_is_overridden = false;
    assert_int_equal(TRIGGERFISH_WEAK_ERROR_STRONG_IS_INVALID,
                     triggerfish_error);
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_of(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    void *instance = malloc(1);
    struct triggerfish_strong *strong;
    assert_true(triggerfish_strong_of(instance, on_destroy, &strong));
    struct triggerfish_weak *out;
    assert_true(triggerfish_weak_of(strong, &out));
    assert_non_null(out);
    assert_ptr_equal(atomic_load(&out->strong), strong);
    expect_function_call(on_destroy);
    assert_true(triggerfish_strong_release(strong));
    assert_ptr_equal(atomic_load(&out->strong), 0);
    assert_true(triggerfish_weak_destroy(out));
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_copy_of_error_on_other_is_null(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    assert_false(triggerfish_weak_copy_of(NULL, (void *) 1));
    assert_int_equal(TRIGGERFISH_WEAK_ERROR_OTHER_IS_NULL, triggerfish_error);
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_copy_of_error_on_out_is_null(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    assert_false(triggerfish_weak_copy_of((void *) 1, NULL));
    assert_int_equal(TRIGGERFISH_WEAK_ERROR_OUT_IS_NULL, triggerfish_error);
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_copy_of_error_on_memory_allocation_failed(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = true;
    assert_false(triggerfish_weak_copy_of((void *) 1, (void *) 1));
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = false;
    assert_int_equal(TRIGGERFISH_WEAK_ERROR_MEMORY_ALLOCATION_FAILED,
                     triggerfish_error);
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_copy_of(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    void *instance = malloc(1);
    struct triggerfish_strong *strong;
    assert_true(triggerfish_strong_of(instance, on_destroy, &strong));
    struct triggerfish_weak *out;
    assert_true(triggerfish_weak_of(strong, &out));
    struct triggerfish_weak *copy;
    assert_true(triggerfish_weak_copy_of(out, &copy));
    assert_int_equal(atomic_load(&copy->strong), atomic_load(&out->strong));
    assert_ptr_equal(atomic_load(&copy->strong), strong);
    expect_function_call(on_destroy);
    assert_true(triggerfish_strong_release(strong));
    assert_ptr_equal(atomic_load(&out->strong), 0);
    assert_ptr_equal(atomic_load(&copy->strong), 0);
    assert_true(triggerfish_weak_destroy(out));
    assert_true(triggerfish_weak_destroy(copy));
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_strong_error_on_object_is_null(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    assert_false(triggerfish_weak_strong(NULL, (void *)1));
    assert_int_equal(TRIGGERFISH_WEAK_ERROR_OBJECT_IS_NULL, triggerfish_error);
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_strong_error_on_out_is_null(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    assert_false(triggerfish_weak_strong((void *)1, NULL));
    assert_int_equal(TRIGGERFISH_WEAK_ERROR_OUT_IS_NULL, triggerfish_error);
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_strong_error_strong_is_invalid(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    struct triggerfish_weak object = {};
    assert_false(triggerfish_weak_strong(&object, (void *)1));
    assert_int_equal(TRIGGERFISH_WEAK_ERROR_STRONG_IS_INVALID,
                     triggerfish_error);
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_strong(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    void *instance = malloc(1);
    struct triggerfish_strong *strong;
    assert_true(triggerfish_strong_of(instance, on_destroy, &strong));
    struct triggerfish_weak *object;
    assert_true(triggerfish_weak_of(strong, &object));
    assert_non_null(object);
    struct triggerfish_strong *out;
    assert_ptr_equal(atomic_load(&strong->counter), 1);
    assert_true(triggerfish_weak_strong(object, &out));
    assert_ptr_equal(atomic_load(&strong->counter), 2);
    assert_ptr_equal(strong, out);
    assert_true(triggerfish_strong_release(strong));
    expect_function_call(on_destroy);
    assert_true(triggerfish_strong_release(strong));
    assert_ptr_equal(atomic_load(&object->strong), 0);
    assert_true(triggerfish_weak_destroy(object));
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
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
