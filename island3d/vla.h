#pragma once

#include <stdlib.h>

template<typename T>
struct vla {
    const static int default_backing_size = 10;

    T *items;
    int backing_size;
    int length;

    vla() {
        printf("make vla\n");
        items = (T *)malloc(sizeof(T) * default_backing_size);
        backing_size = default_backing_size;
        length = 0;
    }
    
    void push(T item) {
        if (length == backing_size - 1) { 
            backing_size *= 2;
            items = (T *)realloc(items, backing_size * sizeof(T));
        }

        items[length++] = item;
    }

    ~vla() {
        // rn this frees it if theres ever a copy or anything being made
        // can put in arc with copy constructor if u want, or just be careful
        printf("vla destructor\n");
        free(items);
    }
};