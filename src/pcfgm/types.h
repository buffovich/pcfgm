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
	uint8_t data[];
} data_t;

//  == 1 if saved on big-endian host
#define BLOB_BIG_ENDIAN 256
// == 1 if is floating point number
#define BLOB_FLOAT 512
// == 1 if blob is array of elements
#define BLOB_ARRAY 1024
// == 0 if blob is allocated dynamically, for example
// == 1 if blob came from mapping, for example
#define BLOB_SHARED 2048
// block size (or size of element in the case of array)
#define BLOB_LENGTH_MASK 255

typedef struct {
	uint32_t options; // I placed uint32_t to deal with alignment because
					// data.length has uint32_t
	
	union {
		uint8_t value[];
		data_t data;
	};
} blob_t;

typedef struct {
	method_table_t *vtable; // mixin's methods table
	data_t *instance; // mixin's part of run-time data
} mixin_t;

/*
 * Confession:
 * node struct is not very mathematically perfect because there is the special
 * case of "non-virtual" node. Non-virtual node is handled by iternal framework
 * getters and setters. There is no need to scan methods table. It will work
 * faster than virtual node. Faster means better for config.
 */

#define NODE_EMPTY 1
typedef struct {
	unsigned int options;

	node_t *parent;
	int ref_count;

	/*
	 * Stack on top off dynamic array for mixins.
	 */
	mixin_t **head;
	blob_t *value; // if not empty
} node_t;

// Methods:
#define CLASS_HAS_GET_ITER 1
#define CLASS_HAS_GET_NODE 4
#define CLASS_HAS_ADD_NODE 16
#define CLASS_HAS_DEL_NODE 64

#define CLASS_HAS_GET_VALUE 256
#define CLASS_HAS_SET_VALUE 512

#define CLASS_HAS_DESTROY 1024

typedef node_t* ( *get_node_m )( int mindex,
	node_t *node,
	const char *name
);

typedef int ( *add_node_m )( int mindex,
	node_t *node,
	const char *name,
	node_t *what
);

/*
 * For "primary" parent of node
 */
typedef int ( *create_node_m )( int mindex,
	node_t *node,
	unsigned int options
);

typedef int ( *del_node_m )( int mindex, node_t *node, const char *name );

/*
 * For advice from high-level routines about number of nodes p;anned for adding
 */
typedef int ( *accept_advice_m )( int mindex, node_t *node, blob_t *value );

typedef cfg_iter_t ( *get_iter_m )( int mindex, node_t *node );

typedef int ( *get_value_m )( int mindex, node_t *node, blob_t *value );
typedef int ( *set_value_m )( int mindex, node_t *node, blob_t *value );

typedef int ( *destroy_m )( int mindex, node_t *node );

typedef struct {
	/*
	 * Particular mixin may desire to override their predecessor's methods
	 * just partially. It's set describes which methods are overriden.
	 */
	unsigned int mset;
	/*
	 * Array of methods. They go according to mset and sequential number of
	 * bit.
	 */
	void* methods[];
} method_table_t;
