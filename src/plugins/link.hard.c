#include <pcfgm/internal/plugin.h>

#include <pcfgm/graph.h>
#include <pcfgm/values.h>
#include <pcfgm/types.h>
#include <pcfgm/types/basic.h>

node_t* on_create( node_t* cfg, node_t* me ) {
	size_t psize = CFG_VALUE( cfg, "to", "cstr", NULL );
	
	if( psize == 0 )
		return NULL;
	//TODO: handle case when value is absent
	//TODO: handle case when cast is failed
	
	const char *path = alloca( psize );
	CFG_VALUE( cfg, "to", "cstr", path );
	
	return cfg_node_lookup( parent, ( char* ) ( path ) );
}
