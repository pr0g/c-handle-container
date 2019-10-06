#include "container.h"

#include "stdint.h"
#include "stdlib.h"

typedef struct internal_handle_t {
    handle_t handle_; // handle itself
    int lookup_; // where to look in object arr
    int next_; // next internal handle
} internal_handle_t;

typedef struct container_t {
    enum { SIZE = 100 };
    int object_ids_[SIZE];
    object_t objects_[SIZE];
    internal_handle_t handles_[SIZE];

    internal_handle_t next_;
    internal_handle_t back_;
    int size_;
} container_t;

container_t* container_alloc() {
    container_t* container = malloc(sizeof *container);
    *container = (container_t) { 0 };
    return container;
}
void container_dealloc(container_t* container) {
    free(container);
}

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
