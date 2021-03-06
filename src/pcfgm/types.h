#ifndef PCFGM_TYPES
#define PCFGM_TYPES

#include <utarray.h>
#include <stdint.h>

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
typedef struct {
	node_t *( *parent )( void *clojure, const char **name );
	void *clojure;
} iter_t;

// What blob is:

/*
 * TODO: consider cash thrashing. Try to maintain spatial locality! Try to
 * use arrays instead of lists or place list nodes near each other.
 */
//  == 1 if saved on big-endian host
#define BLOB_BIG_ENDIAN 256
// == 1 if is floating point number
#define BLOB_FLOAT 512
#define BLOB_SIGNED 1024
// == 1 if blob is array of elements
#define BLOB_ARRAY 2048
// == 1 if blob is allocated dynamically from created slab
// == 0 if blob came from mapping, created in stack or through
// 		standard malloc/free routines
#define BLOB_TRACKED 4096
// block size (or size of element in the case of array)
#define BLOB_LENGTH_MASK 255

typedef struct {
	uint32_t options; // I placed uint32_t to deal with alignment because
					// data.length has uint32_t
	
	union {
		uint8_t value[ 1 ];
		
		struct {
			uint32_t length;
			uint8_t data[];
		} array;
	};
} blob_t;

// class won't be inherited by children nodes
#define CLASS_UNINHERITABLE 1
// class instance has its own blob
#define CLASS_INSTANCE_HAS_NO_DATA 2

typedef struct {
	unsigned int options;
	methods_table_t vtable; // mixin's methods table
} class_t;

// TODO: link tracking should be implemented inside of slab logic
typedef struct {
	node_t *parent;

	/*
	 * Stack on top off dynamic array for mixins.
	 */
	UT_array head;
	UT_array classes;
} node_t;

// Methods:
typedef enum {
	/*
	 * Third parameter - cfg_iter_t *iter
	 * If return value == 0 then previous class method won't be invoked.
	 * == 1 - framework will continue calling sequence
	 */
	GET_ITER = 0,
	/*
	 * Third parameter - const char *name
	 * Forth parameter - node_t **ret
	 * If return value == 0 then previous class method won't be invoked.
	 * == 1 - framework will continue calling sequence
	 */
	LOOKUP_NODE,
	/*
	 * Third parameter - const char *name
	 * Forth parameter - node_t *what
	 * If return value == 0 then previous class method won't be invoked.
	 * == 1 - framework will continue calling sequence
	 */
	LINK_NODE,
	/*
	 * Factory method 
	 * For "primary" parent of node
	 * Third parameter - node_t **node
	 * Pointing value which is pointer to node should be == NULL in the case if
	 * nothing has been allocated yet. So, the first create routine in the chain
	 * is guaranteed to be recieved NULL as the value of pointer to node.
	 * Third parameter can't be NULL itself.
	 * create method should invoke previous create explicitly.
	 * You do not have to implement this method. If you don't implement it
	 * then framework will just copy instance data pointer from parent to child
	 * TODO: I don't know how to treat return value.
	 */
	CREATE_NODE,
	/*
	 * Third parameter - const char *name
	 * If return value == 0 then previous class method won't be invoked.
	 * == 1 - framework will continue calling sequence
	 */
	UNLINK_NODE,
	/*
	 * For advice from high-level routines about number of nodes
	 * planned for adding
	 * Third parameter - size_t num
	 * If return value == 0 then previous class method won't be invoked.
	 * == 1 - framework will continue calling sequence
	 */
	ACCEPT_ADVICE,
	/*
	 * Third value - blob_t *value
	 * If return value == 0 then previous class method won't be invoked.
	 * == 1 - framework will continue calling sequence
	 */
	GET_VALUE,
	/*
	 * Third value - blob_t * value
	 * If return value == 0 then previous class method won't be invoked.
	 * == 1 - framework will continue calling sequence
	 */
	SET_VALUE,
	/*
	 * No extra parameters
	 * If destroy method returns 0 then no extra efforts needed from framework.
	 * Otherwise - next destructor will be invoked automatically.
	 */
	DESTROY
} method_id_t;

#define METHODS_NUM 9

/*
 * Universal template for all methods.
 */

typedef int ( *method_t )( node_t *node,
	class_t **klass,
	void **datapp,
	va_list args
);

/*
 * Array of methods. See method_t
 */
typedef method_t methods_table_t[ METHODS_NUM ];

extern int __endian_test;

static inline int is_cstr( blob_t *b ) {
	return (
		( b->options & BLOB_ARRAY ) &&
		!( b->options & BLOB_FLOAT ) &&
		( ( b->options & BLOB_LENGTH_MASK ) = sizeof( char ) )
	);
}

static inline int is_host_endian( blob_t *b ) {
	return (
		(
			( b->options & BLOB_BIG_ENDIAN ) &&
			( *( ( char* ) &__endian_test ) == 0 )
		) ||
		(
			!( b->options & BLOB_BIG_ENDIAN ) &&
			( *( ( char* ) &__endian_test ) == 1 )
		)
	);
}

#endif PCFGM_TYPES
