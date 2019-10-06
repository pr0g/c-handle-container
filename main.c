#include "stdint.h"
#include "stdio.h"

#include "unity.h"

#include "container.h"

container_t* test_container = NULL;

void setUp(void) {
    test_container = container_alloc();
}

void tearDown(void) {
    container_dealloc(test_container);
}

void test_canAddElementToContainer(void) {
    container_init(test_container);
    // TEST_ASSERT_EQUAL_INT64(0, test_container->size_);
    TEST_FAIL_MESSAGE("oh no");
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_canAddElementToContainer);
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
