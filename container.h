#ifndef CONTAINER_H
#define CONTAINER_H

typedef struct handle_t {
    int id_;
    int gen_;
} handle_t;

// typedef struct object_t object_t;
typedef struct object_t {
    int value_;
} object_t;

typedef struct container_t container_t;

container_t* container_alloc();
void container_dealloc(container_t* container);
void container_init(container_t* container);
handle_t container_add(container_t* container);
object_t* container_get(container_t* container, handle_t handle);
handle_t container_has(container_t* container);
void container_remove(container_t* container, handle_t handle);

#endif // CONTAINER_H
