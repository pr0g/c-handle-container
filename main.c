#include "stdint.h"
#include "stdio.h"

#include "unity.h"

#include "container.h"

// reminders
// TEST_ASSERT_EQUAL_INT64(0, test_container->size_);
// TEST_FAIL_MESSAGE("oh no");

container_t* test_container = NULL;

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

    TEST_ASSERT_EQUAL_INT(5, container_size(test_container));
    handle_t next_handle = container_add(test_container);
    TEST_ASSERT_EQUAL_INT(-1, next_handle.gen_);
    TEST_ASSERT_EQUAL_INT(-1, next_handle.id_);
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

    TEST_ASSERT_EQUAL_INT(5, container_size(test_container));

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

    TEST_ASSERT_EQUAL_INT(5, container_size(test_container));

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
    const int capacity = 5;
    handle_t handles[capacity];

    for (int i = 0; i < capacity; i++) {
        handles[i] = container_add(test_container);
    }

    container_remove(test_container, handles[2]);

    object_t* begin = container_get(test_container, handles[0]);
    object_t* was_end = container_get(test_container, handles[4]);

    TEST_ASSERT_EQUAL_INT(2, was_end - begin);
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
    RUN_TEST(test_cannotRemoveInvalidHandle);
    RUN_TEST(test_canRemoveAddedHandle);
    RUN_TEST(test_addAndRemoveHandlesReverseOrder);
    RUN_TEST(test_addAndRemoveHandlesOrdered);
    RUN_TEST(test_canGetObjectViaHandle);
    RUN_TEST(test_addTwoHandlesAndUpdateObjects);
    RUN_TEST(test_originalHandleCannotAccessObjectAfterRemoval);
    RUN_TEST(test_objectsRemainPackedAfterRemoval);

    return UNITY_END();

    // container_t* container = container_alloc();
    // container_init(container);
    // handle_t handle1 = container_add(container);
    // handle_t handle2 = container_add(container);
    // handle_t handle3 = container_add(container);
    // handle_t handle4 = container_add(container);
    // object_t* obj1 = container_get(container, handle1);
    // obj1->value_ = 1;
    // object_t* obj2 = container_get(container, handle2);
    // obj2->value_ = 2;
    // object_t* obj3 = container_get(container, handle3);
    // obj3->value_ = 3;
    // object_t* obj4 = container_get(container, handle4);
    // obj4->value_ = 4;
    // container_remove(container, handle2);
    // object_t* obj4_again = container_get(container, handle4);
    // container_dealloc(container);
    // return 0;
}
