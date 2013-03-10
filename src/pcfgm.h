#ifndef PCFGMOD_USER_API
#define PCFGMOD_USER_API

#include <stddef.h>

/**
 * \mainpage How to plug configuration
 * \section idea General Idea
 * The main idea is to provide effective modularized
 * and unattended way to get some configuration
 * parameters. "Effective" means that we want to provide
 * lightweight and fast facility to do config fetching
 * with robust high-level API. "Modularized" means that
 * you can write plugins for anything you want:
 * - config parser backends;
 * - reader/writer backends;
 * - config routing backends;
 * - ACL backends.
 *
 * And you can change configuration sources and
 * configuration storage/engine/place/host with
 * simple changing of special configuration file as
 * it's done in PAM authentication framework.
 * "Unattended" means that we don't have any
 * external dependencies for basic API and modules other
 * than libc6.
 *
 * \section arch Architecture
 * I'd like to mention that config parser is reader/writer
 * backend itself. Config routing backend is reader/writer
 * backend also. ACL either. So, entire structure boils
 * down to:
 * - user API;
 * - stackable modules interconnection API;
 * - tree walker;
 * - multi-thread and multi-client synchronize;
 * - structure allocating and purging logic.
 *
 * Config is a graph with nodes which have values.
 * Nodes can have different types.
 * Actually there are two types: SCALAR, ARRAY.
 * HASHes aren't needed because graph node marks
 * play the same role as keys in hash.
 *
 * \section facters Facters
 * Neat idea to gather data from system and merge
 * or assign it to specified configuration parameters.
 * These modules belongs to reader/writer backends class.
 * For exmaple, if we have module cpu.so (UNIX-like systems)
 * then we can gather following parameters:
 * cpu.cpuid, cpu.frequency, cpu.number and so on.
 *
 * \section scripting Scripting plug-ins
 * Much more neat idea to allow to embed scripting
 * languages as reader/writer plugins.
 *
 * \section caching Caching plugins
 * Pretty obviously to write stackable module to cache
 * some parameters locally on disk/memory.
 *
 * \section routing Routing and calculating
 * We can do load-balancing, merging and assigning with
 * such kind of modules.
 *
 * \todo I suppose that it would be convinient to have ability to
 * translate scheme description to C code for neat getting/setting.
 * Something like "uint32 get_cfg_param_log_size( cfg_descr_t cfg );".
 * 
 * \todo Create utils.h with user wishes. I realize that menrioned
 * API may be too minimalistic sometimes.
 *
 * \section incapsulation Data incapsulation
 * You may be noticed already that some of standart structures has
 * a lack of fields. One of "the most noticeable" lacks is that you
 * have no some kind of type_id field. Don't worry. Everything will
 * be done for you by library (data type parsing, transorming and
 * checking). Stay tuned to see how.
 */
 
/**
 * Opens new session for default config.
 * Opens new session for config for current application name (argv[0]).
 * @return config tree descriptor, NULL if error
 * @see cfg_init_as()
 * @see cfg_destroy()
 */
extern cfg_node_t cfg_init( void );
/**
 * Opens new session for config.
 * Opens new session for config for specified app name.
 * @param app_id  application name (or ID), default if NULL
 * @return config tree descriptor, NULL if error
 * @see cfg_init()
 * @see cfg_destroy()
 */
extern cfg_node_t cfg_init_as( const char* app_id );

/**
 * Get current state as new config.
 * Get thing which descripts current config state for specified subtree.
 * If subpath == NULL then entire config will be snapshoted.
 * @param cfg config descriptor (don't think that passing
 *            config-from-state is a good idea)
 * @param subpath subpath for snapshot
 * @return thing which descripts current config revision; should be
 *         utlize through cfg_destroy()
 * @see cfg_destroy()
 */
extern cfg_t cfg_state_get( cfg_t cfg,
    const cfg_key_path_t* subpath
);
/**
 * Change type.
 * Value od such type is passed to callback to tell what kind of change
 * has appeared.
 * @see cfg_node_watch()
 * @see cfg_node_get_watcher()
 * @see cfg_node_unwatch()
 * @see cfg_watcher_t
 */
typedef enum _change_type {
    KEY_CHANGED, /**< Value of key has been changed */
    KEY_ADDED, /**< Subkey has been added */
    KEY_DELETED /**< Subkey has been deleted */
} cfg_change_type_t;
/**
 * Decribes type in general.
 * This structure is, actually, header for conrete types. There 2 types
 * classes:
 * - scalar type;
 * - vector type.
 * 
 * We can define the rest content of the structure with help of
 * is_vector flag. If is_vector >0 then considered structure describes
 * vector type. Otherwise - it's scalar type.
 * @see cfg_param_scalar_type_t
 * @see cfg_param_vector_type_t
 */
typedef struct _cfg_param_type {
    unsigned char is_vector; /**< >0 - the rest will describe
                                       a vector type. */
} cfg_param_type_t;
/**
 * Describes parameter value.
 * This is common part of all possible raw values of all types. I'd like
 * to suppose that you've already noticed the same idea in
 * cfg_param_*_type_t family.
 * @see cfg_param_type_t
 * @see cfg_param_scalar_data_t
 * @see cfg_param_vector_data_t
 */
typedef struct _cfg_param_data {
    cfg_param_type_t* type; /**< Pointer to type descriptor.*/
    unsigned char data[]; /**< Data itself. If it's scalar values then
                               this field can be safely casted to
                               appropriate high-level type. If it's
                               one-level array then it can be casted to
                               array of values. If it's multidimensional
                               array then each element is the pointer
                               to another vector structure.*/
} cfg_param_data_t;
/**
 * Change made in node value.
 * Used for passing to change callback changes if KEY_CHANGED is
 * passed too.
 * @see cfg_param_data_t
 * @see cfg_change_callback_t
 */
typedef struct _key_change {
    cfg_param_data_t* oldv;
    cfg_param_data_t* newv;
} cfg_key_change_t;
/**
 * Definition of change callback.
 * This type reflects expected semantic of callback which can be hanged
 * on an event of config change and, if it is set then it will be called
 * each time when changes happen under specified subpath.
 * @param cfg config where changes are aimed to occur
 * @param node node in subtree which is modified
 * @param ch_type change type
 * @param info either pointer to cfg_key_path_t (if changes related to
 *             adding or deleting of keys) or pointer to
 *             cfg_key_change_t (if changes are in key value)
 * @param tag user value specified at the hook creation
 * @return >0 if change accepted, 0 if declined (yes, callback is
 *         called before actual operation performed)
 * @see cfg_node_watch()
 * @see cfg_node_get_watcher()
 * @see cfg_key_path_t
 * @see cfg_change_type_t
 */
typedef int ( *cfg_watcher_t )( cfg_t cfg,
    cfg_node_t node,
    cfg_change_type_t ch_type,
    void* info,
    void* tag
);
/**
 * Hang change callback on subtree.
 * Set hook for all chenges which may be made with specified subpath
 * (and all of subkeys if we set recursive callback).
 * @param cfg config descriptor
 * @param subtree start point in config tree for watch
 * @param callback callback routine specified by user
 * @param is_recursive if >0 then callback will be set for all subkeys
 *                     of specified subpath; otherwise we will watch for
 *                     changes in node specified in subtree only
 * @param tag user value which will be passed to callback
 * @return >0 if success, =0 if something is wrong
 * @see cfg_watcher_t
 * @see cfg_node_get_watcher()
 * @see cfg_node_unwatch()
 */
extern int cfg_node_watch( cfg_t cfg,
    cfg_node_t subtree,
    cfg_watcher_t callback,
    char is_recursive,
    void* tag
);
/**
 * Get callback hanged on specified node.
 * @param subtree subtree in config which callback was hanged on
 * @param tag if != NULL then it should be a pointer to variable
 *            which has type void* where tag passed to
 *            cfg_change_callback_set will be stored
 * @return pointer to routine, NULL if there is nothing set
 * @see cfg_node_watch()
 * @see cfg_node_unwatch()
 */
extern cfg_watcher_t cfg_node_get_watcher( cfg_node_t subtree,
    void** tag
);
/**
 * Reset change callback(s) for specified subtree.
 * Reset change callback set by user for specified subtree. If specified
 * it resets callbacks for entire subtree.
 * @param cfg config descriptor
 * @param subtree subtree in config where we are going to reset
 *                callback(s)
 * @param is_recursive if >0 then reset will be performed recursively
 *                     for all subkeys
 * @return >0 - everything is fine, =0 - we got a trouble in reset
 * @see cfg_node_get_watcher()
 * @see cfg_node_watch()
 */
extern int cfg_node_unwatch( cfg_t cfg,
    cfg_node_t subtree,
    char is_recursive
);
/**
 * Possible types of lock.
 * @see cfg_key_lock()
 * @see cfg_key_unlock()
 */
typedef enum _lock_type {
    LOCK_READ_ONLY, /**< Everyone can read but no one can write*/
    LOCK_EXCLUSIVE /**< No one except caller can read or write*/
} cfg_lock_type_t;
/**
 * Locks node or subtree.
 * Locks specified key (or path in case if is_recursive >0)
 * @param cfg config descriptor
 * @param subtree node or subtree we are going to lock
 * @param type type of lock
 * @param is_recursive if >0 then lock will be applied for entire
 *                     subtree, othetwise - just for one node
 * @return >0 - everything is fine, =0 - otherwise
 * @see cfg_node_unlock()
 */
extern int cfg_node_lock( cfg_t cfg,
    cfg_node_t subtree,
    cfg_lock_type_t type,
    unsigned char is_recursive
);
/**
 * Unlocks node or subtree.
 * Remove lock from specified node (or recursively removes locks from
 * nodes in subtree in case if is_recursive >0).
 * @param cfg config descriptor
 * @param subtree node or subtree we are going to unlock
 * @param is_recursive if >0 then lock will be applied for entire
 *                     subtree, othetwise - just for one key
 * @return >0 - everything is fine, =0 - otherwise
 * @see cfg_node_lock()
 */
extern int cfg_node_unlock( cfg_t cfg,
    cfg_node_t subtree,
    char is_recursive
);
/**
 * Scalar type definition.
 * Structure maintains scalar type definition. It has header with type
 * cfg_param_type_t. So every pointer to cfg_param_scalar_type_t is
 * a pointer to cfg_param_type_t also. It's a way to introduce
 * polimorphism. There is nothing special. Just the byte size of value.
 * @see cfg_param_type_t
 */
typedef struct _cfg_param_scalar_type {
    cfg_param_type_t header; /**< Common description for all types. */
    size_t el_size; /**< Byte size of instance of this type. */
} cfg_param_scalar_type_t;
/**
 * Vector type definition.
 * Structure maintains vector type definition. It has header with type
 * cfg_param_type_t. So, every pointer to cfg_param_scalar_type_t is
 * the pointer to cfg_param_type_t also. It's a way to introduce
 * polimorphism. There are max capacity limit (upper limit for number
 * of elements in array) and pointer to elements type (it might be
 * another vector; so we can have multi-dimensional vectors). Vector may
 * be fixed-sized or dynamic. It depends on max_capacity field. If it is
 * >0 then vector has fixed capacity and memory consumption. If it is ==0
 * then array is dynamic without capacity limit.
 * @see cfg_param_type_t
 */
typedef struct _cfg_param_vector_type {
    cfg_param_type_t header; /**< Common description for all types. */
    size_t max_capacity; /**< Upper limit for number of elements;
                              if ==0 then vector is dynamic and it
                              doesn't have any limit for
                              capacity (size). */
    cfg_param_type_t* el_type; /**< Pointer to element type's
                                    descripting structure. */
} cfg_param_vector_type_t;
/**
 * Describes scalar parameter value.
 * Structure maintains scalar parameter value. It's the same thing as
 * cfg_param_data_t. But I'd like to reflect possibility of future
 * modifications and express "artificial" polymorphism explicitly.
 * @see cfg_param_t
 * @see cfg_param_type_t
 */
typedef struct _cfg_param_scalar_data {
    cfg_param_data_t header; /**< Common part of all parameters. */
} cfg_param_scalar_data_t;
/**
 * Describes vector parameter value.
 * Structure maintains vector parameter value. As it was described
 * before pointer to this structure can be safely casted to pointer to
 * cfg_param_data_t. Structure includes addditional fields for
 * describing actual capacity and size of the vector.
 * @see cfg_param_t
 * @see cfg_param_type_t
 */
typedef struct _cfg_param_vector_data {
    cfg_param_data_t header; /**< Common part of all parameters. */
    size_t capacity; /**< Actual array capacity. */
    size_t length; /**< Array length. */
} cfg_param_vector_data_t;
/**
 * Fetch param data.
 * Fetch param data to local thread's snapshot and return pointer
 * to structure which represents data. You should transform it to your
 * type by casting .data field yourself. Please, be aware that you get
 * pointer to iternal structure. Use cloning to pass the data between
 * threads. Passing original data isn't prohibited enterily but you
 * have been notified. If thread buffer exists it will be updated.
 * @param cfg config descriptor
 * @param node node which data we want to get
 * @return pointer to structure represents data; NULL if error is
 *         encountered
 * @see cfg_data_commit()
 * @see cfg_data_dup()
 * @see cfg_param_data_t
 */
extern cfg_param_data_t* cfg_data_checkout( cfg_t cfg,
    cfg_node_t node
);
/**
 * Commit param data changes and release thread buffer.
 * Commits param data changes of particular node and release
 * thread buffer. If passed structure is thread buffer then copy will
 * be avoided.
 * @param cfg config descriptor
 * @param node node which data we want to change
 * @param data_str value we want to commit for the parameter; if NULL
 *                 then current content of thread buffer will be
 *                 committed
 * @return >0 if everything is OK; 0 - otherwise
 * @see cfg_data_checkout()
 * @see cfg_data_dup()
 * @see cfg_param_data_t
 */
extern int cfg_data_commit( cfg_t cfg,
    cfg_node_t root,
    const cfg_param_data_t* data_str
);
/**
 * Duplicates passed structure deeply.
 * Function creates entire duplicate of passed parameter structure
 * (including sub-array if this is multi-dimensional array).
 * @param cfg config descriptor
 * @param source value which will be cloned
 * @return clone or NULL if error has occured
 * @see cfg_data_get()
 * @see cfg_data_destroy()
 */
extern cfg_param_data_t* cfg_data_dup( const cfg_param_data_t* source );
/**
 * Utilize cloned value.
 * Recycles clones of "binded" values created by cfg_param_get_raw() and
 * cfg_param_dup().
 * @param clone structure to utilize
 * @return >0 - ok; 0 - error
 * @see cfg_data_get()
 * @see cfg_data_dup()                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
 */
extern int cfg_data_destroy( cfg_param_data_t* clone );
/**
 * Convinient way to work with library's arrays.
 * This routine serves for array resizing. If size exceeds capacity
 * then there are two ways to decide what we should do:
 * - if array isn't dynamic - throw error;
 * - if array is dynamic - create new storage with bigger capacity and
 *   copy content to it.
 * If size drops lower than 1/4 capacity then capacity wil lbe decreased
 * by half. If we resizes multidimensional vector then if subarrays will
 * be created/destroyed automatically.
 * @param data vector which we want to expand
 * @param new_length new size for passed vector
 * @return >0 - ok; 0 - error
 * @see cfg_param_vector_data_t
 */
extern int cfg_data_resize_array( cfg_param_vector_data_t* data,
    size_t new_length
);
/**
 * Returns error code and error message.
 * If troubles strike then you can read what is happened through
 * mentioned routine. It returns error code and (optionally)
 * human-readable error message.
 * @param message pointer to variable where pointer to string with error
 *                will be stored; if NULL then no error messsage will be
 *                returned;
 * @return error code
 */
extern int cfg_error( char** message );

#endif
