#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H

#include "plugin.h"

#define MAX_PLUGINS 32

typedef struct {
    void* handle;
    ImagePlugin* plugin;
} LoadedPlugin;

typedef struct {
    LoadedPlugin plugins[MAX_PLUGINS];
    int count;
} PluginManager;

void plugin_manager_init(PluginManager* manager);
void plugin_manager_compile_sources(const char* src_dir, const char* bin_dir);
void plugin_manager_load_dir(PluginManager* manager, const char* dir_path);
void plugin_manager_cleanup(PluginManager* manager);


#endif
