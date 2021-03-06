#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <errno.h>

struct Vector {
    void*  _items;
    size_t _itemSize;
    size_t _capacity;
    size_t _itemCount;
    int (*DisposeFn)(void*);
};

int VectorNew(struct Vector*, size_t itemSize, int (*DisposeFn)(void*));
int VectorDispose(struct Vector*);
int VectorAdd(struct Vector*, const void* item);
size_t VectorGetCount(struct Vector*);
int VectorGet(struct Vector*, size_t index, void* OutResult);
int VectorSet(struct Vector*, size_t index, void* item);
int VectorClear(struct Vector*);
int VectorToArray(struct Vector*, void* OutArray, size_t OutArrayLength);

#endif /* VECTOR_H */
