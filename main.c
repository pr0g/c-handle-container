#include "stdint.h"
#include "stdio.h"

typedef struct object_t {
    int value_;
} object_t;

typedef struct handle_t {
    int id_;
    int gen_;
} handle_t;

typedef struct internal_handle_t {
    handle_t handle_; // handle itself
    int lookup_; // where to look in object arr
    int next_; // next internal handle
} internal_handle_t;

typedef struct container_t {
    enum { SIZE = 100 };
    object_t objects_[SIZE];
    int object_ids_[SIZE];
    internal_handle_t handles_[SIZE];

    internal_handle_t next_;
    internal_handle_t back_;
    int size_;
} container_t;

void container_init(container_t* container) {
    for (int i = 0; i < SIZE; ++i) {
        container->handles_[i].handle_.id_ = i;
        container->handles_[i].handle_.gen_ = 0;
        container->handles_[i].lookup_ = i;
        container->handles_[i].next_ = i + 1;
        container->object_ids_[i] = i;
    }

    container->next_ = container->handles_[0];
    container->back_ = container->handles_[0];
    container->size_ = 0;
}

handle_t container_add(container_t* container) {
    // handle size - return invalid handle
    const internal_handle_t next = container->next_;
    container->next_ = container->handles_[next.next_];
    container->size_++;
    return next.handle_;
}

object_t* container_get(container_t* container, handle_t handle) {
    const int last = container->size_;
    const int lookup = container->handles_[handle.id_].lookup_;
    if (lookup >= last) {
        return NULL;
    }

    if (container->handles_[handle.id_].handle_.gen_ != handle.gen_) {
        return NULL;
    }

    return &container->objects_[lookup];
}

handle_t container_has(container_t* container) {
    return (handle_t) { .id_ = 0, .gen_ = 0 };
}

void container_remove(container_t* container, handle_t handle) {
    // todo - check size
    // todo - check generation of handle
    container->handles_[handle.id_].next_ = container->next_.next_;
    container->handles_[handle.id_].handle_.gen_++;
    container->next_ = container->handles_[handle.id_];

    container->objects_[container->handles_[handle.id_].lookup_]
        = container->objects_[container->size_ - 1];
    container->object_ids_[container->handles_[handle.id_].lookup_]
        = container->object_ids_[container->size_ - 1];

    container->handles_[container->object_ids_[container->handles_[handle.id_].lookup_]].lookup_ =
        container->handles_[handle.id_].lookup_;

    container->size_--;
}

int main(int argc, char** argv) {
    container_t container;
    container_init(&container);

    handle_t handle1 = container_add(&container);
    handle_t handle2 = container_add(&container);
    handle_t handle3 = container_add(&container);
    handle_t handle4 = container_add(&container);

    object_t* obj1 = container_get(&container, handle1);
    obj1->value_ = 1;
    object_t* obj2 = container_get(&container, handle2);
    obj2->value_ = 2;
    object_t* obj3 = container_get(&container, handle3);
    obj3->value_ = 3;
    object_t* obj4 = container_get(&container, handle4);
    obj4->value_ = 4;

    container_remove(&container, handle2);

    object_t* obj4_again = container_get(&container, handle4);

    return 0;
}

// [4] [7] [8] [7] [9] // val
// [0] [1] [2] [3] [4] // id
// _next

// before
// [4] [7] [8] [7] [9] // val
// [0] [1] [2] [3] [4] // id

// after
// [4] [7] [8] [9] [-] // val
// [0] [1] [2] [3] [4] // id

// remove 3
