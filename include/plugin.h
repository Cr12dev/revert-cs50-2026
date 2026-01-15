#ifndef PLUGIN_H
#define PLUGIN_H

#include "graphics.h"

typedef struct {
    const char* name;
    const char* author;
    const char* version;
    bool show_in_advanced; // New: should it appear in the advanced panel?
} PluginInfo;

typedef struct {
    PluginInfo info;
    void (*apply)(Image* image);
    void (*apply_param)(Image* image, float value); // New: for +/- controls
} ImagePlugin;


// Function signature for plugin entry point
typedef ImagePlugin* (*PluginInitFunc)();

#endif
