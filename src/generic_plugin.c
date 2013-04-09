#include <api/plugin.h>

void* before_create( blob_t* icfg ) {
	return 0;
}

node_t* on_create( void* with, node_t* parent ) {
	
}

int on_enter( node_t *to ) {
}

int on_leaving( node_t *from ) {
}
