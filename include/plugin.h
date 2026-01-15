#ifndef PLUGIN_H
#define PLUGIN_H

#include "graphics.h"

typedef struct {
    const char* name;
    const char* author;
    const char* version;
} PluginInfo;

typedef struct {
    PluginInfo info;
    void (*apply)(Image* image);
} ImagePlugin;

// Function signature for plugin entry point
typedef ImagePlugin* (*PluginInitFunc)();

#endif
