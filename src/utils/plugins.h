#ifndef PCFGMOD_PLUGINS
#define PCFGMOD_PLUGINS

typedef enum _plugin_type {
    PLUGIN_STORAGE,
    PLUGIN_ACCESSOR
} cfg_plugin_type_t;

typedef struct _plugin {
    void* handle;
    cfg_plugin_type_t type;
} cfg_plugin_t;

typedef struct _storage_plugin {
    cfg_plugin_t header;
    cfg_t ( *parse )( const char* uri );
    int ( *update )( cfg_t config );
    int ( *save_as )( cfg_t config, const char* uri );
} cfg_storage_plugin_t;

typedef struct _accessor_plugin {
    cfg_plugin_t header;
    cfg_t ( *initialize )( cfg_node_t your_config );
} cfg_accessor_plugin_t;

extern cfg_plugin_t* cfg_plugin_load( const char* name )

#endif
