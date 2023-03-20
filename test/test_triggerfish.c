#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdatomic.h>
#include <coral.h>
#include <triggerfish.h>

#include "private/weak.h"
#include "private/strong.h"

#include <test/cmocka.h>

static void on_destroy(void *instance) {
    assert_non_null(instance);
    function_called();
}

static void
check_case_where_weak_is_cleared_after_strong_is_reclaimed(void **state) {
    struct triggerfish_strong *strong;
    assert_int_equal(triggerfish_strong_of(malloc(1), on_destroy, &strong), 0);
    struct triggerfish_weak *weak;
    assert_int_equal(triggerfish_weak_of(strong, &weak), 0);
    expect_function_call(on_destroy);
    assert_int_equal(triggerfish_strong_release(strong), 0);
    /* check that weak is cleared when strong is released */
    assert_ptr_equal(atomic_load(&weak->strong), 0);
    assert_int_equal(triggerfish_weak_destroy(weak), 0);
}

static void
check_case_where_weak_is_added_and_removed_from_strong(void **state) {
    struct triggerfish_strong *strong;
    assert_int_equal(triggerfish_strong_of(malloc(1), on_destroy, &strong), 0);
    struct triggerfish_weak *weak;
    uintmax_t count;
    assert_int_equal(coral_red_black_tree_set_count(
            &strong->weak_refs, &count), 0);
    assert_int_equal(count, 0);
    assert_int_equal(triggerfish_weak_of(strong, &weak), 0);
    assert_int_equal(coral_red_black_tree_set_count(
            &strong->weak_refs, &count), 0);
    assert_int_equal(count, 1);
    assert_int_equal(triggerfish_weak_destroy(weak), 0);
    assert_int_equal(coral_red_black_tree_set_count(
            &strong->weak_refs, &count), 0);
    assert_int_equal(count, 0);
    expect_function_call(on_destroy);
    assert_int_equal(triggerfish_strong_release(strong), 0);
}

int main(int argc, char *argv[]) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(check_case_where_weak_is_cleared_after_strong_is_reclaimed),
            cmocka_unit_test(check_case_where_weak_is_added_and_removed_from_strong),
    };
    //cmocka_set_message_output(CM_OUTPUT_XML);
    return cmocka_run_group_tests(tests, NULL, NULL);
}
