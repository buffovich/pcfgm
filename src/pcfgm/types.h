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
		uint8_t value[ 1 ];
		data_t data;
	};
} blob_t;

typedef struct {
	methods_table_t *vtable; // mixin's methods table
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
typedef enum {
	/*
	 * Third parameter - cfg_iter_t *iter
	 */
	GET_ITER = 0,
	/*
	 * Third parameter - const char *name
	 * Forth parameter - node_t **ret
	 */
	GET_NODE,
	/*
	 * Third parameter - const char *name
	 * Forth parameter - node_t *what
	 */
	ADD_NODE,
	/*
	 * For "primary" parent of node
	 * Third parameter - unsigned int options
	 * Forth parameter - node_t **node
	 */
	CREATE_NODE,
	/*
	 * Third parameter - const char *name
	 */
	DEL_NODE,
	/*
	 * For advice from high-level routines about number of nodes
	 * planned for adding
	 * Third parameter - size_t num
	 */
	ACCEPT_ADVICE,
	/*
	 * Third value - blob_t *value
	 */
	GET_VALUE,
	/*
	 * Third value - blob_t *value
	 */
	SET_VALUE,
	/*
	 * No extra parameters
	 */
	DESTROY
} method_id_t;

#define METHODS_NUM 9

/*
 * Universal template for all methods.
 */

typedef int ( *method_t )( int mindex, node_t *node, va_list args );

extern int _cfg_invoke_next( int mindex, node_t *node, method_id_t m, ... );

/*
 * Array of methods. See method_t
 */
typedef method_t methods_table_t[ METHODS_NUM ];
