/**
 * Descriptor of node.
 * Whenever we open particular instance of config or pick particular
 * sub-tree/destination node by abolute or relative path, a value of this type
 * is returned. It's a handle for particular point in config tree/graph.
 * After using, handle should be destroyed.
 * @see cfg_state_t
 * @see cfg_init();
 * @see cfg_init_as();
 * @see cfg_destroy()
 */
typedef void* cfg_node_t;

/**
 * Represents iterator.
 * This thing is used in within operations related to iteration through
 * node children. As it has been mentioned before, we have tree-like
 * config structure. Some nodes may have variadic number of subnodes.
 * It may be used to construct hashtables in config. So we have ability
 * to iterate through keys and its values.
 * @see cfg_iterator_get()
 * @see cfg_iterator_next()
 */
typedef void* cfg_iter_t;

// What blob is:

/*
 * TODO: consider cash thrashing. Try to maintain spatial locality! Try to
 * use arrays instead of lists or place list nodes near each other.
 */

typedef struct {
	uint32_t length;
	char data[];
} data_t;

//  == 1 if saved on big-endian host
#define BLOB_BIG_ENDIAN 256
// == 1 if is floating point number
#define BLOB_FLOAT 512
// == 1 if blob is array of elements
#define BLOB_ARRAY 1024
// block size (or size of element in the case of array)
#define BLOB_LENGTH_MASK 255

typedef struct {
	int options;
	
	union {
		char value[];
		data_t data;
	};
} blob_t;

typedef struct {
	mixin_t *next;
	method_table_t *vtable; // mixin's methods table
	data_t instance; // mixin's part of run-time data
} mixin_t;

/*
 * Confession:
 * node struct is not very mathematically perfect because there is the special
 * case of "non-virtual" node. Non-virtual node is handled by iternal framework
 * getters and setters. There is no need to scan methods table. It will work
 * faster than vrtual node. Faster means better for config.
 */

#define NODE_VIRTUAL 1
typedef struct {
	int options;

	node_t parent;
	int ref_count;

	/*
	 * Here is next "part" of node. The fact is that node is a instance of
	 * private class constructed on-the-fly. Class is a stack of "mixins".
	 * Each mixin desires to have their one private instance data in resulting
	 * object. And each node part is responsible for information for
	 * particular mixin.
	 */
	mixin_t *head;
	
	union {
		blob_t value; // if not virtual
		mixin_t mixin; // virtual
	}
} node_t;

// Methods:
#define CLASS_HAS_GET_ITER_SUCC 1
#define CLASS_HAS_GET_SUCC 4
#define CLASS_HAS_ADD_SUCC 16
#define CLASS_HAS_DEL_SUCC 64

#define CLASS_HAS_GET_VALUE 256
#define CLASS_HAS_SET_VALUE 512

#define CLASS_HAS_DESTROY 1024

typedef node_t* ( *get_succ_m )( mixin_t *self, node_t *node, blob_t *name );

typedef int ( *add_succ_m )( mixin_t *self,
	node_t *node,
	blob_t *name,
	node_t *succ
);

typedef int ( *del_succ_m )( mixin_t *self, node_t *node, blob_t *name );

typedef cfg_iter_t ( *get_iter_succ_m )( mixin_t *self, node_t *node );

typedef int ( *get_value_m )( mixin_t *self, node_t *node );
typedef int ( *set_value_m )( mixin_t *self, node_t *node, blob_t *value );

typedef int ( *destroy_m )( mixin_t *self, node_t *node );

typedef struct {
	/*
	 * Particular mixin may desire to override their predecessor's methods
	 * just partially. It's set describes which methods are overriden.
	 */
	int mset;
	/*
	 * Array of methods. They go according to mset and sequential number of
	 * bit.
	 */
	void* methods[];
} method_table_t;
