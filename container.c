#include "container.h"

#include "stdlib.h"

typedef struct internal_handle_t {
    handle_t handle_; // handle itself
    // int lookup_; // where to look in object arr
    // int next_; // next internal handle
} internal_handle_t;

typedef struct container_t {
    enum { CAPACITY = 5 };
    // int object_ids_[CAPACITY];
    // object_t objects_[CAPACITY];
    internal_handle_t handles_[CAPACITY];

    // internal_handle_t next_;
    // internal_handle_t back_;
    int size_;
} container_t;

container_t* container_alloc() {
    container_t* container = malloc(sizeof *container);
    *container = (container_t) { .size_ = -1 };
    return container;
}

void container_dealloc(container_t* container) {
    free(container);
}

container_t* container_init(container_t* container) {
    for (int i = 0; i < CAPACITY; ++i) {
        container->handles_[i].handle_.id_ = i;
        container->handles_[i].handle_.gen_ = -1;
    //     container->handles_[i].lookup_ = i;
    //     container->handles_[i].next_ = i + 1;
    //     container->object_ids_[i] = i;
    }

    // container->next_ = container->handles_[0];
    // container->back_ = container->handles_[0];
    container->size_ = 0;
    return container;
}

int container_size(container_t* container) {
    return container->size_;
}

int container_capacity(container_t* container) {
    return CAPACITY;
}

handle_t container_add(container_t* container) {
    // handle size - return invalid handle
    const int next = container->size_++;
    if (next >= CAPACITY) {
        return (handle_t) { .id_ = -1, .gen_ = -1 };
    }

    handle_t* handle = &container->handles_[next].handle_;
    handle->gen_++;
    return *handle;

    // const internal_handle_t next = container->next_;
    // container->next_ = container->handles_[next.next_];
    // container->size_++;
    // return next.handle_;
}

object_t* container_get(container_t* container, handle_t handle) {
    return NULL;

    // const int last = container->size_;
    // const int lookup = container->handles_[handle.id_].lookup_;
    // if (lookup >= last) {
    //     return NULL;
    // }

    // if (container->handles_[handle.id_].handle_.gen_ != handle.gen_) {
    //     return NULL;
    // }

    // return &container->objects_[lookup];
}

bool container_has(container_t* container, handle_t handle) {
    if (handle.id_ >= CAPACITY) {
        return false;
    }

    handle_t h = container->handles_[handle.id_].handle_;
    return handle.gen_ == h.gen_;
}

bool container_remove(container_t* container, handle_t handle) {
    if (!container_has(container, handle)) {
        return false;
    }

    container->handles_[handle.id_].handle_.gen_++;

    return true;
    // todo - check size
    // todo - check generation of handle

    // container->handles_[handle.id_].next_ = container->next_.next_;
    // container->handles_[handle.id_].handle_.gen_++;
    // container->next_ = container->handles_[handle.id_];

    // container->objects_[container->handles_[handle.id_].lookup_]
    //     = container->objects_[container->size_ - 1];
    // container->object_ids_[container->handles_[handle.id_].lookup_]
    //     = container->object_ids_[container->size_ - 1];

    // container->handles_[container->object_ids_[container->handles_[handle.id_].lookup_]].lookup_ =
    //     container->handles_[handle.id_].lookup_;

    // container->size_--;
}
