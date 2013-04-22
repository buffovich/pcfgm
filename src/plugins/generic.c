#include <pcfgm/internal/plugin.h>

#include <pcfgm/types.h>
#include <pcfgm/internal/alloc.h>

static struct {
	cache_t *nodes;
	cache_t *blobs;
} _G_objects;

node_t* on_create( node_t *cfg, node_t *me ) {
	
}
