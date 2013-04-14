#include <api/plugin.h>
#include <stdarg.h>

node_t* on_create( node_t* cfg, node_t* me ) {
	char *path = CFG_VALUE_BY_PATH(cfg, "to", "cstr" );
	
	if( path == NULL )
		return NULL;
	//TODO: handle case when value is absent
	//TODO: handle case when cast is failed
	return _cfg_node_lookup( parent, ( char* ) ( path ) );
}
