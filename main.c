#include "stdint.h"
#include "stdio.h"

#include "unity.h"

#include "container.h"

#include "string.h"
// TEST_FAIL_MESSAGE("oh no");

container_t* test_container = NULL;

static void initContainerWithFiveHandles(handle_t handles[static 5]) {
    for (int i = 0; i < 5; ++i) {
        handles[i] = container_add(test_container);
    }
}

void setUp(void) {
    test_container = container_alloc();
    container_init(test_container);
}

void tearDown(void) {
    container_dealloc(test_container);
}

void test_canAllocContainer(void) {
    container_t* container = container_alloc();
    TEST_ASSERT_NOT_NULL(container);
}

void test_containerSizeIsZeroAfterInit(void) {
    container_t* container = container_alloc();
    container_init(container);
    TEST_ASSERT_EQUAL_INT(0, container_size(container));
}

void test_initialHandleReturnedIsZero() {
    handle_t handle = container_add(test_container);
    TEST_ASSERT_EQUAL_INT(0, handle.id_);
}

void test_containerSizeIsOneAfterSingleAdd() {
    container_add(test_container);
    TEST_ASSERT_EQUAL_INT(1, container_size(test_container));
}

void test_containerSizeGrowsWithConsecutiveAdds() {
    handle_t handle1 = container_add(test_container);
    handle_t handle2 = container_add(test_container);
    handle_t handle3 = container_add(test_container);

    TEST_ASSERT_EQUAL_INT(0, handle1.id_);
    TEST_ASSERT_EQUAL_INT(1, handle2.id_);
    TEST_ASSERT_EQUAL_INT(2, handle3.id_);
    TEST_ASSERT_EQUAL_INT(3, container_size(test_container));
}

void test_containerHasAddedHandle() {
    handle_t handle = container_add(test_container);
    TEST_ASSERT_TRUE(container_has(test_container, handle));
}

void test_containerDoesNotHaveHandle() {
    handle_t handle = {};
    TEST_ASSERT_FALSE(container_has(test_container, handle));
}

void test_cannotAddMoreHandlesThanCapacity() {
    const int capacity = container_capacity(test_container);
    for (int i = 0; i < capacity; i++) {
        container_add(test_container);
    }

    TEST_ASSERT_EQUAL_INT(capacity, container_size(test_container));
    handle_t next_handle = container_add(test_container);
    TEST_ASSERT_EQUAL_INT(-1, next_handle.gen_);
    TEST_ASSERT_EQUAL_INT(-1, next_handle.id_);
}

void test_removeDecreasesSize() {
    const int handle_count = 5;
    handle_t handles[handle_count];
    for (int i = 0; i < handle_count; ++i) {
        handles[i] = container_add(test_container);
    }

    TEST_ASSERT_EQUAL_INT(handle_count, container_size(test_container));

    container_remove(test_container, handles[2]);
    container_remove(test_container, handles[0]);

    TEST_ASSERT_EQUAL_INT(3, container_size(test_container));
}

void test_handleReusedAfterRemoval() {
    handle_t initial_handle = container_add(test_container);
    container_remove(test_container, initial_handle);
    handle_t next_handle = container_add(test_container);
    TEST_ASSERT_EQUAL(0, next_handle.id_);
    TEST_ASSERT_EQUAL(1, next_handle.gen_);
}

void test_cannotRemoveInvalidHandle() {
    handle_t handle = { .id_ = -1, .gen_ = -1 };
    bool removed = container_remove(test_container, handle);
    TEST_ASSERT_EQUAL(false, removed);
}

void test_canRemoveAddedHandle() {
    handle_t handle = container_add(test_container);
    bool removed = container_remove(test_container, handle);
    bool has = container_has(test_container, handle);
    TEST_ASSERT_EQUAL(true, removed);
    TEST_ASSERT_EQUAL(false, has);
}

void test_addAndRemoveHandlesReverseOrder() {
    const int capacity = container_capacity(test_container);
    handle_t handles[capacity];

    for (int i = 0; i < capacity; i++) {
        handles[i] = container_add(test_container);
    }

    TEST_ASSERT_EQUAL_INT(capacity, container_size(test_container));

    bool removed = true;
    for (int i = capacity - 1; i >= 0; i--) {
        removed &= !!container_remove(test_container, handles[i]);
    }

    TEST_ASSERT_EQUAL(true, removed);
    TEST_ASSERT_EQUAL_INT(0, container_size(test_container));

    for (int i = 0; i < capacity; i++) {
        TEST_ASSERT_EQUAL(false, container_has(test_container, handles[i]));
    }
}

void test_addAndRemoveHandlesOrdered() {
    const int capacity = container_capacity(test_container);
    handle_t handles[capacity];

    for (int i = 0; i < capacity; i++) {
        handles[i] = container_add(test_container);
    }

    TEST_ASSERT_EQUAL_INT(capacity, container_size(test_container));

    bool removed = true;
    for (int i = 0; i < capacity; ++i) {
        removed &= !!container_remove(test_container, handles[i]);
    }

    TEST_ASSERT_EQUAL(true, removed);
    TEST_ASSERT_EQUAL_INT(0, container_size(test_container));

    for (int i = 0; i < capacity; i++) {
        TEST_ASSERT_EQUAL(false, container_has(test_container, handles[i]));
    }
}

void test_canGetObjectViaHandle() {
    handle_t handle = container_add(test_container);
    object_t* object = container_get(test_container, handle);
    TEST_ASSERT_NOT_NULL(object);
}

void test_addTwoHandlesAndUpdateObjects() {
    handle_t handle_1 = container_add(test_container);
    handle_t handle_2 = container_add(test_container);

    {
        object_t* object_1 = container_get(test_container, handle_1);
        object_t* object_2 = container_get(test_container, handle_2);

        object_1->value_ = 4;
        object_2->value_ = 5;
    }

    {
        object_t* object_1 = container_get(test_container, handle_1);
        object_t* object_2 = container_get(test_container, handle_2);

        TEST_ASSERT_EQUAL_INT(4, object_1->value_);
        TEST_ASSERT_EQUAL_INT(5, object_2->value_);
    }
}

void test_originalHandleCannotAccessObjectAfterRemoval() {
    handle_t handle = container_add(test_container);
    container_remove(test_container, handle);
    object_t* object = container_get(test_container, handle);
    TEST_ASSERT_NULL(object);
}

void test_objectsRemainPackedAfterRemoval() {
    handle_t handles[5];
    initContainerWithFiveHandles(handles);

    container_remove(test_container, handles[2]);

    object_t* begin = container_get(test_container, handles[0]);
    object_t* was_end = container_get(test_container, handles[4]);

    TEST_ASSERT_EQUAL_INT(2, was_end - begin);
}

void test_containerDebugVisualization() {
    handle_t handles[5];
    initContainerWithFiveHandles(handles);

    container_remove(test_container, handles[2]);
    container_remove(test_container, handles[0]);

    const int buffer_size = debug_container_handles(test_container, 0, NULL);

    char buffer[buffer_size];
    buffer[0] = '\0';

    debug_container_handles(test_container, buffer_size, buffer);

    char expected_buffer[buffer_size];
    expected_buffer[0] = '\0';

    for (int i = 0; i < container_capacity(test_container); i++) {
        strcat(expected_buffer, "[x]");
    }

    memcpy(expected_buffer, "[x][o][x][o][o]", 15);

    TEST_ASSERT_EQUAL_STRING(expected_buffer, buffer);
}

void test_containerDebugVisualizationBufferTooSmall() {
    handle_t handles[5];
    initContainerWithFiveHandles(handles);

    const int buffer_size = 2;
    char buffer[buffer_size];
    buffer[0] = '\0';

    int result = debug_container_handles(test_container, buffer_size, buffer);
    TEST_ASSERT_EQUAL_INT(-1, result);

    int glyph_size = 3;
    int expected_size = container_capacity(test_container) * glyph_size + 1 /*null terminator*/;
    int required_buffer_size = debug_container_handles(test_container, 0, NULL);
    TEST_ASSERT_EQUAL_INT(expected_size, required_buffer_size);
}

void test_ensureHandlesReaddedInOrder() {
    handle_t handles[5];
    initContainerWithFiveHandles(handles);

    const int buffer_size = debug_container_handles(test_container, 0, NULL);

    char expected_buffer[buffer_size];
    expected_buffer[0] = '\0';

    for (int i = 0; i < container_capacity(test_container); i++) {
        strcat(expected_buffer, "[x]");
    }

    for (int i = 0; i < 5; ++i) {
        container_remove(test_container, handles[i]);
    }

    char buffer[buffer_size];
    buffer[0] = '\0';
    debug_container_handles(test_container, buffer_size, buffer);
    TEST_ASSERT_EQUAL_STRING(expected_buffer, buffer);

    handle_t first_new_handle = container_add(test_container);
    buffer[0] = '\0';
    debug_container_handles(test_container, buffer_size, buffer);
    memcpy(expected_buffer, "[x][x][x][x][o]", 15);
    TEST_ASSERT_EQUAL_STRING(expected_buffer, buffer);

    handle_t second_new_handle = container_add(test_container);
    buffer[0] = '\0';
    debug_container_handles(test_container, buffer_size, buffer);
    memcpy(expected_buffer, "[x][x][x][o][o]", 15);
    TEST_ASSERT_EQUAL_STRING(expected_buffer, buffer);

    object_t* begin = container_get(test_container, first_new_handle);
    object_t* end = container_get(test_container, second_new_handle);

    // ensure objects are tightly packed
    ptrdiff_t size = end - begin;
    TEST_ASSERT_EQUAL_INT64(1, size);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_canAllocContainer);
    RUN_TEST(test_containerSizeIsZeroAfterInit);
    RUN_TEST(test_initialHandleReturnedIsZero);
    RUN_TEST(test_containerSizeIsOneAfterSingleAdd);
    RUN_TEST(test_containerSizeGrowsWithConsecutiveAdds);
    RUN_TEST(test_cannotAddMoreHandlesThanCapacity);
    RUN_TEST(test_handleReusedAfterRemoval);
    RUN_TEST(test_containerHasAddedHandle);
    RUN_TEST(test_containerDoesNotHaveHandle);
    RUN_TEST(test_removeDecreasesSize);
    RUN_TEST(test_cannotRemoveInvalidHandle);
    RUN_TEST(test_canRemoveAddedHandle);
    RUN_TEST(test_addAndRemoveHandlesReverseOrder);
    RUN_TEST(test_addAndRemoveHandlesOrdered);
    RUN_TEST(test_canGetObjectViaHandle);
    RUN_TEST(test_addTwoHandlesAndUpdateObjects);
    RUN_TEST(test_originalHandleCannotAccessObjectAfterRemoval);
    RUN_TEST(test_objectsRemainPackedAfterRemoval);
    RUN_TEST(test_containerDebugVisualization);
    RUN_TEST(test_containerDebugVisualizationBufferTooSmall);
    RUN_TEST(test_ensureHandlesReaddedInOrder);
    return UNITY_END();
}
