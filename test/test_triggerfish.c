#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdatomic.h>
#include <coral.h>
#include <triggerfish.h>

#include "private/weak.h"
#include "test/cmocka.h"

static void on_destroy(void *instance) {
    assert_non_null(instance);
    function_called();
}

static void check_case_where_strong_outlives_weak(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    struct triggerfish_strong *strong;
    assert_true(triggerfish_strong_of(malloc(1), on_destroy, &strong));
    struct triggerfish_weak *weak;
    assert_true(triggerfish_weak_of(strong, &weak));
    expect_function_call(on_destroy);
    assert_true(triggerfish_strong_release(strong));
    assert_ptr_equal(atomic_load(&weak->strong), 0);
    assert_true(triggerfish_weak_destroy(weak));
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

static void check_case_where_weak_outlives_strong(void **state) {
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
    struct triggerfish_strong *strong;
    assert_true(triggerfish_strong_of(malloc(1), on_destroy, &strong));
    struct triggerfish_weak *weak;
    assert_true(triggerfish_weak_of(strong, &weak));
    coral_error = CORAL_ERROR_NONE;
    assert_true(triggerfish_weak_destroy(weak));
    /* ensure that we found the weak reference within the strong reference
     * by checking the absence of an error ... */
    assert_int_equal(coral_error, CORAL_ERROR_NONE);
    expect_function_call(on_destroy);
    assert_true(triggerfish_strong_release(strong));
    triggerfish_error = TRIGGERFISH_ERROR_NONE;
}

int main(int argc, char *argv[]) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(check_case_where_strong_outlives_weak),
            cmocka_unit_test(check_case_where_weak_outlives_strong),
    };
    //cmocka_set_message_output(CM_OUTPUT_XML);
    return cmocka_run_group_tests(tests, NULL, NULL);
}
