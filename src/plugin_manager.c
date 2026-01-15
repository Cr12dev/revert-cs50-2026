#include "plugin_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#ifdef _WIN32
    #include <windows.h>
    #define LIB_EXT ".dll"
    #define LOAD_LIB(p) LoadLibrary(p)
    #define GET_PROC(h, n) GetProcAddress(h, n)
    #define CLOSE_LIB(h) FreeLibrary(h)
#else
    #include <dlfcn.h>
    #define LIB_EXT ".so"
    #define LOAD_LIB(p) dlopen(p, RTLD_LAZY)
    #define GET_PROC(h, n) dlsym(h, n)
    #define CLOSE_LIB(h) dlclose(h)
#endif

void plugin_manager_init(PluginManager* manager) {
    manager->count = 0;
}

static void load_plugin(PluginManager* manager, const char* path) {
    if (manager->count >= MAX_PLUGINS) return;

    void* handle = LOAD_LIB(path);
    if (!handle) {
#ifndef _WIN32
        fprintf(stderr, "Error loading plugin %s: %s\n", path, dlerror());
#endif
        return;
    }

    PluginInitFunc init_func = (PluginInitFunc)GET_PROC(handle, "plugin_init");
    if (!init_func) {
        fprintf(stderr, "Plugin %s missing plugin_init\n", path);
        CLOSE_LIB(handle);
        return;
    }

    ImagePlugin* plugin = init_func();
    if (!plugin) {
        CLOSE_LIB(handle);
        return;
    }

    manager->plugins[manager->count].handle = handle;
    manager->plugins[manager->count].plugin = plugin;
    manager->count++;

    printf("Loaded plugin: %s v%s by %s\n", 
           plugin->info.name, plugin->info.version, plugin->info.author);
}

void plugin_manager_load_dir(PluginManager* manager, const char* dir_path) {
    DIR* dir = opendir(dir_path);
    if (!dir) return;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, LIB_EXT)) {
            char path[512];
            snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);
            load_plugin(manager, path);
        }
    }
    closedir(dir);
}

void plugin_manager_cleanup(PluginManager* manager) {
    for (int i = 0; i < manager->count; i++) {
        CLOSE_LIB(manager->plugins[i].handle);
    }
    manager->count = 0;
}
