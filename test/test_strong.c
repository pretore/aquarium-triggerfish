#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <errno.h>
#include <triggerfish.h>

#include "private/strong.h"

#include <test/cmocka.h>

static void check_of_error_on_instance_is_null(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    assert_false(triggerfish_strong_of(NULL, (void *) 1, (void *) 1));
    assert_int_equal(TRIGGERFISH_STRONG_ERROR_INSTANCE_IS_NULL,
                     triggerfish_error);
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_of_error_on_on_destroy_is_null(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    assert_false(triggerfish_strong_of((void *) 1, NULL, (void *) 1));
    assert_int_equal(TRIGGERFISH_STRONG_ERROR_ON_DESTROY_IS_NULL,
                     triggerfish_error);
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_of_error_on_out_is_null(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    assert_false(triggerfish_strong_of((void *) 1, (void *) 1, NULL));
    assert_int_equal(TRIGGERFISH_STRONG_ERROR_OUT_IS_NULL, triggerfish_error);
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_of_error_on_memory_allocation_failed(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    struct triggerfish_strong *out;
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden = true;
    assert_false(triggerfish_strong_of((void *) 1, (void *) 1, &out));
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden = false;
    assert_int_equal(TRIGGERFISH_STRONG_ERROR_MEMORY_ALLOCATION_FAILED,
                     triggerfish_error);
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    pthread_mutex_init_is_overridden = true;
    will_return(cmocka_test_pthread_mutex_init, ENOMEM);
    assert_false(triggerfish_strong_of((void *) 1, (void *) 1, &out));
    pthread_mutex_init_is_overridden = false;
    assert_int_equal(TRIGGERFISH_STRONG_ERROR_MEMORY_ALLOCATION_FAILED,
                     triggerfish_error);
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void on_destroy(void *instance) {
    assert_non_null(instance);
    function_called();
}

static void check_of(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    void *instance = malloc(1);
    struct triggerfish_strong *object;
    assert_true(triggerfish_strong_of(instance, on_destroy, &object));
    assert_non_null(object);
    assert_ptr_equal(object->instance, instance);
    assert_ptr_equal(object->on_destroy, on_destroy);
    assert_int_equal(atomic_load(&object->counter), 1);
    expect_function_call(on_destroy);
    assert_true(triggerfish_strong_release(object));
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_count_error_on_object_is_null(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    assert_false(triggerfish_strong_count(NULL, (void *)1));
    assert_int_equal(TRIGGERFISH_STRONG_ERROR_OBJECT_IS_NULL,
                     triggerfish_error);
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_count_error_on_out_is_null(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    assert_false(triggerfish_strong_count((void *)1, NULL));
    assert_int_equal(TRIGGERFISH_STRONG_ERROR_OUT_IS_NULL, triggerfish_error);
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_count(void **state) {
    srand(time(NULL));
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    struct triggerfish_strong object = {
            .counter = rand() % UINTMAX_MAX
    };
    uintmax_t out;
    assert_true(triggerfish_strong_count(&object, &out));
    assert_int_equal(out, object.counter);
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_retain_error_on_object_is_null(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    assert_false(triggerfish_strong_retain(NULL));
    assert_int_equal(TRIGGERFISH_STRONG_ERROR_OBJECT_IS_NULL,
                     triggerfish_error);
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_retain_error_on_object_is_invalid(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    struct triggerfish_strong object = {};
    assert_false(triggerfish_strong_retain(&object));
    assert_int_equal(TRIGGERFISH_STRONG_ERROR_OBJECT_IS_INVALID,
                     triggerfish_error);
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_retain(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    void *instance = malloc(1);
    struct triggerfish_strong *object;
    assert_true(triggerfish_strong_of(instance, on_destroy, &object));
    assert_int_equal(atomic_load(&object->counter), 1);
    assert_true(triggerfish_strong_retain(object));
    assert_int_equal(atomic_load(&object->counter), 2);
    expect_function_call(on_destroy);
    for (uintmax_t i = 0, l = atomic_load(&object->counter); i < l; i++) {
        assert_true(triggerfish_strong_release(object));
    }
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_release_error_on_object_is_null(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    assert_false(triggerfish_strong_release(NULL));
    assert_int_equal(TRIGGERFISH_STRONG_ERROR_OBJECT_IS_NULL,
                     triggerfish_error);
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_release(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    void *instance = malloc(1);
    struct triggerfish_strong *object;
    assert_true(triggerfish_strong_of(instance, on_destroy, &object));
    atomic_store(&object->counter, 2);
    assert_true(triggerfish_strong_release(object));
    assert_int_equal(atomic_load(&object->counter), 1);
    expect_function_call(on_destroy);
    assert_true(triggerfish_strong_release(object));
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_instance_error_on_object_is_null(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    assert_false(triggerfish_strong_instance(NULL, (void *)1));
    assert_int_equal(TRIGGERFISH_STRONG_ERROR_OBJECT_IS_NULL,
                     triggerfish_error);
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_instance_error_on_out_is_null(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    assert_false(triggerfish_strong_instance((void *)1, NULL));
    assert_int_equal(TRIGGERFISH_STRONG_ERROR_OUT_IS_NULL, triggerfish_error);
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_instance_error_on_object_is_invalid(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    struct triggerfish_strong object = {};
    void *out;
    assert_false(triggerfish_strong_instance(&object, &out));
    assert_int_equal(TRIGGERFISH_STRONG_ERROR_OBJECT_IS_INVALID,
                     triggerfish_error);
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_instance(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    void *instance = malloc(1);
    struct triggerfish_strong *object;
    assert_true(triggerfish_strong_of(instance, on_destroy, &object));
    void *out;
    assert_true(triggerfish_strong_instance(object, &out));
    assert_ptr_equal(out, instance);
    expect_function_call(on_destroy);
    assert_true(triggerfish_strong_release(object));
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_register_error_on_object_is_invalid(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    pthread_mutex_lock_is_overridden = true;
    will_return(cmocka_test_pthread_mutex_lock, EINVAL);
    assert_false(triggerfish_strong_register((void *)1, (void *)1));
    assert_int_equal(TRIGGERFISH_STRONG_ERROR_OBJECT_IS_INVALID,
                     triggerfish_error);
    pthread_mutex_lock_is_overridden = false;
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_register_error_on_weak_already_registered(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    struct triggerfish_strong *object;
    assert_true(triggerfish_strong_of(malloc(1), on_destroy, &object));
    struct triggerfish_weak *weak;
    assert_true(triggerfish_weak_of(object, &weak));
    assert_false(triggerfish_strong_register(object, weak));
    assert_int_equal(TRIGGERFISH_STRONG_ERROR_WEAK_ALREADY_REGISTERED,
                     triggerfish_error);
    assert_true(triggerfish_weak_destroy(weak));
    expect_function_call(on_destroy);
    assert_true(triggerfish_strong_release(object));
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_register_error_on_memory_allocation_failed(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    struct triggerfish_strong *object;
    assert_true(triggerfish_strong_of(malloc(1), on_destroy, &object));
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = true;
    assert_false(triggerfish_strong_register(object, (void *)1));
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = false;
    assert_int_equal(TRIGGERFISH_STRONG_ERROR_MEMORY_ALLOCATION_FAILED,
                     triggerfish_error);
    expect_function_call(on_destroy);
    assert_true(triggerfish_strong_release(object));
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

int main(int argc, char *argv[]) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(check_of_error_on_instance_is_null),
            cmocka_unit_test(check_of_error_on_on_destroy_is_null),
            cmocka_unit_test(check_of_error_on_out_is_null),
            cmocka_unit_test(check_of_error_on_memory_allocation_failed),
            cmocka_unit_test(check_of),
            cmocka_unit_test(check_count_error_on_object_is_null),
            cmocka_unit_test(check_count_error_on_out_is_null),
            cmocka_unit_test(check_count),
            cmocka_unit_test(check_retain_error_on_object_is_null),
            cmocka_unit_test(check_retain_error_on_object_is_invalid),
            cmocka_unit_test(check_retain),
            cmocka_unit_test(check_release_error_on_object_is_null),
            cmocka_unit_test(check_release),
            cmocka_unit_test(check_instance_error_on_object_is_null),
            cmocka_unit_test(check_instance_error_on_out_is_null),
            cmocka_unit_test(check_instance_error_on_object_is_invalid),
            cmocka_unit_test(check_instance),
            cmocka_unit_test(check_register_error_on_object_is_invalid),
            cmocka_unit_test(check_register_error_on_weak_already_registered),
            cmocka_unit_test(check_register_error_on_memory_allocation_failed),
    };
    //cmocka_set_message_output(CM_OUTPUT_XML);
    return cmocka_run_group_tests(tests, NULL, NULL);
}
