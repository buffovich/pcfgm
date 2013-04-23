#include <pcfgm/internal/plugin.h>

#include <pcfgm/types.h>
#include <pcfgm/internal/alloc.h>
#include <pcfgm/>

typedef struct {
	cache_t *nodes;
	cache_t *blobs;
} object_cache_t;

static void __init_cache( object_cache_t *oc,
	unsigned int nblobs,
	unsigned int nnodes
) {
	oc->nodes = _cfg_cache_create
}

static class_t _G_generic = {
	.options = 0,
	.vtable = {
		[ DESTROY ] = _oc_destroy
	}
}

static class_t _G_oc_sanitizer = {
	.options = CLASS_UNINHERITABLE,
	.vtable = {
		[ DESTROY ] = _oc_destroy
	}
}

node_t* on_create( node_t *cfg, node_t *me ) {
	uint32_t reserve_nodes = 1,
		reserve_blobs = 1;

	CFG_VALUE( cfg, "reserve_nodes", uint32, &reserve_nodes );
	CFG_VALUE( cfg, "reserve_blobs", uint32, &reserve_blobs );

	object_cache_t *oc = calloc( 1, sizeof( object_cache_t ) );
	
}
