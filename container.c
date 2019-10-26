#include "container.h"

#include "stdlib.h"
#include "string.h"

typedef struct internal_handle_t {
    handle_t handle_; // handle itself
    int lookup_; // where to look in object arr
    int next_; // next internal handle
} internal_handle_t;

typedef struct container_t {
    enum { CAPACITY = 10 }; // small capacity to make testing edge cases easier
    int object_ids_[CAPACITY];
    object_t objects_[CAPACITY];
    internal_handle_t handles_[CAPACITY];

    int next_;
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
        container->handles_[i].lookup_ = -1;
        container->handles_[i].next_ = i + 1;
        container->object_ids_[i] = i;
    }

    container->next_ = 0;
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
    const int insert = container->size_++;
    if (insert >= CAPACITY) {
        return (handle_t) { .id_ = -1, .gen_ = -1 };
    }

    const int next = container->next_;
    container->handles_[next].lookup_ = insert;
    handle_t* handle = &container->handles_[next].handle_;
    handle->gen_++;

    container->object_ids_[next] = handle->id_;

    container->next_ = container->handles_[next].next_;

    return *handle;
}

object_t* container_get(container_t* container, handle_t handle) {
    if (!container_has(container, handle)) {
        return NULL;
    }

    return &container->objects_[container->handles_[handle.id_].lookup_];
}

bool container_has(container_t* container, handle_t handle) {
    if (handle.id_ >= CAPACITY) {
        return false;
    }

    internal_handle_t ih = container->handles_[handle.id_];
    return handle.gen_ == ih.handle_.gen_ && ih.lookup_ != -1;
}

bool container_remove(container_t* container, handle_t handle) {
    if (!container_has(container, handle)) {
        return false;
    }

    container->handles_[container->object_ids_[container->size_ - 1]].lookup_
        = container->handles_[handle.id_].lookup_;
    container->objects_[container->handles_[handle.id_].lookup_]
        = container->objects_[container->size_ - 1];
    container->object_ids_[container->handles_[handle.id_].lookup_]
        = container->object_ids_[container->size_ - 1];

    container->handles_[handle.id_].lookup_ = -1;
    container->handles_[handle.id_].next_ = container->next_;
    container->next_ = handle.id_;

    container->size_--;

    return true;
}

static int max(int lhs, int rhs) {
    return lhs > rhs ? lhs : rhs;
}

int debug_container_handles(container_t* container, int buffer_size, char buffer[buffer_size]) {
    const char* filled_glyph = "[o]";
    const char* empty_glyph = "[x]";

    const int filled_glyph_len = strlen(filled_glyph);
    const int empty_glyph_len = strlen(empty_glyph);
    const int required_buffer_size = CAPACITY * max(filled_glyph_len, empty_glyph_len) + 1;

    if (buffer == NULL) {
        return required_buffer_size;
    } else if (buffer_size < required_buffer_size) {
        return -1;
    } else {
        for (int i = 0; i < CAPACITY; i++) {
            const char* glyph = NULL;
            if (container->handles_[i].lookup_ == -1) {
                glyph = empty_glyph;
            } else {
                glyph = filled_glyph;
            }

            strcat(buffer, glyph);
        }

        return 0;
    }
}
