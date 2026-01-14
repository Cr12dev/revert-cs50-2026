#ifndef MEMORY_H
#define MEMORY_H

#ifdef _WIN32
    #include <SDL.h>
#else
    #include <SDL2/SDL.h>
#endif
#include <stdlib.h>

static inline void* my_malloc(size_t size) { 
    return malloc(size); 
}

static inline void* my_calloc(size_t nmemb, size_t size) { 
    return calloc(nmemb, size); 
}

static inline void* my_realloc(void* ptr, size_t size) { 
    return realloc(ptr, size); 
}

static inline void my_free(void* ptr) { 
    free(ptr); 
}

#endif
