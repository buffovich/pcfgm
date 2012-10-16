#ifndef PCFGMOD_INTERFACE
#define PCFGMOD_INTERFACE

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
 * \section incapsulation Data incapsulation
 * You may be noticed already that some of standart structures has
 * a lack of fields. One of "the most noticeable" lacks is that you
 * have no some kind of type_id field. Don't worry. Everything will
 * be done for you by library (data type parsing, transorming and
 * checking). Stay tuned to see how.
 */

/**
 * Descriptor of config.
 * When we open particular instance of config, this value is returned.
 * It reflects session of config using. We can open several instances
 * of the same config if it's needed. All changes made in one instance
 * will be propagated to all instances. Hooks will be invoked, change
 * descriptors will be changed. After using,
 * descriptor should be closed.
 * @see cfg_state_descr_t
 * @see cfg_init();
 * @see cfg_init_as();
 * @see cfg_destroy_desc()
 */
typedef void * cfg_descr_t;
/**
 * Opens new session for default config.
 * Opens new session for config for current application name (argv[0]).
 * @return config session descriptor, NULL if error
 * @see cfg_init_as()
 * @see cfg_destroy_desc()
 */
extern cfg_descr_t cfg_init( void );
/**
 * Opens new session for config.
 * Opens new session for config for specified app name.
 * @param app_id  application name (or ID), default if NULL
 * @return config session descriptor, NULL if error
 * @see cfg_init()
 * @see cfg_destroy_desc()
 */
extern cfg_descr_t cfg_init_as( const char* app_id );
/**
 * Close config session/destroy any desriptor with associated buffers.
 * @param cfg config session descriptor
 * @return >0 if everything OK, 0 otherwise
 * @see cfg_init()
 * @see cfg_init_as()
 */
extern int cfg_destroy_desc( cfg_descr_t cfg );
/**
 * Splitted key's path.
 * This structure accelerates access to elements of path and it's
 * more convinient for library code. But there is just a little extra
 * space is used because "components" point to different parts of
 * the same string which was passed to cfg_split_path. So, it means that
 * you should handle string copy yourself.
 * @see cfg_path_split()
 */
typedef struct _cfg_key_path {
    int num; /**< Number of components */
    const char* components[]; /**< Components themselves. There aren't
        NULL-terminated strings because these are just pointers to
        different parts of source string. So, key name will end up on
        '.' or \0 */
} cfg_key_path_t;
/**
 * Splitting routine for path.
 * Pass path and allocated structure for splitted path to it and get
 * splitted path linked to source string.
 * @param path source string which will be splitted
 * @param spath pointer to variable where address of newly
 *              created 'splitting' will be stored; should be cleaned
 *              later by caller
 * @return number of components if everything is OK, 0 - otherwise
 */
extern int cfg_path_split( const char* path, cfg_key_path_t** spath);
/**
 * Represents sub-tree as separate config
 * Allows reflecting of particular branch of config to separate config.
 * It may be useful if some part of code needs only particular sub-tree.
 * This saves CPU cycles and prevent walking from root of config through
 * identical path.
 * @param cfg source config which contains target subtree
 * @param path to subtree from root; if == NULL then source descriptor
 *             will be returned
 * @return descriptor binded to mentioned subtree or (spath == NULL)
 *         source
 * @see cfg_destroy_desc()
 */
extern cfg_descr_t cfg_path_as_cfg( cfg_descr_t cfg,
    const cfg_key_path_t* spath );
/**
 * Descriptor of particular state of config.
 * Some value which is linked to particular state
 * of particular config session. Potentially, we can produce diff
 * between two states treating two descriptors as, for example,
 * two revision numbers.
 * @see cfg_state_get()
 * @see cfg_state_as_cfg()
 */ 
typedef cfg_descr_t cfg_state_descr_t;
/**
 * Get descriptor for current state.
 * Get thing which descripts current config state for specified subtree.
 * If subpath == NULL then entire config will be snapshoted.
 * @param cfg config descriptor (don't think that passing
 *            config-from-state is a good idea)
 * @param subpath subpath for snapshot
 * @return thing which desripts current config revision
 */
extern cfg_state_descr_t cfg_state_get( cfg_descr_t cfg,
    const cfg_key_path_t* subpath
);
/**
 * Converts state to full-weight config.
 * Config state descriptor is just an anchor on the timeline. It
 * should be post-processed to use it as config. This routine does
 * mentioned cast.
 * @param state state descriptor
 * @return config descriptor which is ready for use in
 *         get/set/enumerate operations
 */
extern cfg_descr_t cfg_state_as_cfg( cfg_state_descr_t state );
/**
 * Change type.
 * Value od such type is passed to cakkback to tell what kind oj change
 * has appeared.
 * @see cfg_change_callback_set()
 * @see cfg_change_callback_get()
 * @see cfg_change_callback_reset()
 * @see cfg_change_callback_t
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
 * 
 * Also there is checker procedure which controls correctness and can
 * generate formatted error messages if something wrong in data (for
 * example, if we have fixed-length array of chars describes IPv4
 * address then the message might be "Incorrect IP address:...").
 * @see cfg_param_checker_t
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
 * @see cfg_byte_block_t
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
 * @param key key under subpath which is modified
 * @param ch_type change type
 * @param info either pointer to cfg_key_path_t (if changes related to
 *             adding or deleting of keys) or pointer to
 *             cfg_key_change_t (if changes are in key value)
 * @param tag user value specified at the hook creation
 * @return >0 if change accepted, 0 if declined (yes, callback is
 *         called before actual operation performed)
 * @see cfg_change_callback_set()
 * @see cfg_change_callback_get()
 * @see cfg_key_path_t
 * @see cfg_change_type_t
 */
typedef int ( *cfg_change_callback_t )( cfg_descr_t cfg,
    const cfg_key_path_t* key,
    cfg_change_type_t ch_type,
    void* info,
    void* tag
);
/**
 * Hang change callback on subpath.
 * Set hook for all chenges which may be made with specified subpath
 * (and all of subkeys if we set recursive callback).
 * @param cfg config where we are going to watch for changes
 * @param subpath start point in config tree for watch
 * @param callback callback routine specified by user
 * @param is_recursive if >0 then callback will be set gor all subkeys
 *                     of specified subpath; otherwise we will watch for
 *                     changes in key specified in subpath only
 * @param tag user value which will be passed to callback
 * @return >0 if success, =0 if something is wrong
 * @see cfg_change_callback_t
 * @see cfg_change_callback_get()
 * @see cfg_change_callback_reset()
 */
extern int cfg_change_callback_set( cfg_descr_t cfg,
    const cfg_key_path_t* subpath,
    cfg_change_callback_t callback,
    char is_recursive,
    void* tag
);
/**
 * Get callback hanged on specified key.
 * @param cfg config which callnack was hanged on
 * @param subpath subpath in config which callnack was hanged on
 * @param tag if != NULL then it should be a pointer to variable
 *            which has type void* where tag passed to
 *            cfg_change_callback_set will be stored
 * @return pointer to routine, NULL if there is nothing set
 * @see cfg_change_callback_set()
 * @see cfg_change_callback_reset()
 */
extern cfg_change_callback_t cfg_change_callback_get( cfg_descr_t cfg,
    const cfg_key_path_t* subpath,
    void** tag
);
/**
 * Reset change callback(s) for specified subpath.
 * Reset change callback set by user for specified subpath. If specified
 * it resets callbacks for entire subtree.
 * @param cfg config which we are working on
 * @param subpath subpath in config where we are going to reset
 *                callback(s)
 * @param is_recursive if >0 then reset will be performed recursively
 *                     for all subkeys
 * @return >0 - everything is fine, =0 - we got a trouble in reset
 * @see cfg_change_callback_get()
 * @see cfg_change_callback_set()
 */
extern int cfg_change_callback_reset( cfg_descr_t cfg,
    const cfg_key_path_t* subpath,
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
 * Locks key or subpath.
 * Locks specified key (or path in case if is_recursive >0)
 * @param cfg config which contains particular key
 * @param subpath key or path we are going to lock
 * @param type type of lock
 * @param is_recursive if >0 then lock will be applied for entire
 *                     subtree, othetwise - just for one key
 * @return >0 - everything is fine, =0 - otherwise
 * @see cfg_key_unlock()
 */
extern int cfg_key_lock( cfg_descr_t cfg,
    const cfg_key_path_t* subpath,
    cfg_lock_type_t type,
    char is_recursive
);
/**
 * Unlocks key or subpath.
 * Remove lock from specified key (or recursively removes locks from
 * keys in subtree in case if is_recursive >0).
 * @param cfg config which contains particular key
 * @param subpath key or path we are going to lock
 * @param type type of lock
 * @param is_recursive if >0 then lock will be applied for entire
 *                     subtree, othetwise - just for one key
 * @return >0 - everything is fine, =0 - otherwise
 * @see cfg_key_lock()
 */
extern int cfg_key_unlock( cfg_descr_t cfg,
    const cfg_key_path_t* subpath,
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
 * Get param data.
 * Gets param data and return pointer to structure which represents it.
 * You should transform it to your type by casting .data field casting
 * yourself. You may do everything you want with returned value.
 * Structure should be utilized with cfg_param_destroy.
 * @param cfg config descriptor as usual
 * @param path path to key which data we want to get
 * @return pointer to structure represents data; NULL if error is
 *         encountered
 * @see cfg_param_get_binded_raw()
 * @see cfg_param_set_raw()
 * @see cfg_param_destroy()
 * @see cfg_param_data_t
 */
extern cfg_param_data_t* cfg_param_get_raw( cfg_descr_t cfg,
    const cfg_key_path_t* path
);
/**
 * Set param data.
 * Sets value for particular parameter with specified key in specified
 * config. Not very cheap operation. You should check
 * cfg_param_update_raw procedure to ensure that you need this one. The
 * differences are mentioned in cfg_param_update_raw description. If
 * we're talking briefly, cfg_param_set_raw walk through entire tree
 * from config root to set parameter. cfg_param_update_raw doesn't.
 * @param cfg config descriptor
 * @param path path to key which data we want to change
 * @param data_str value we want to set for the parameter
 * @return >0 if everything is OK; 0 - otherwise
 * @see cfg_param_get_raw()
 * @see cfg_param_update_raw()
 * @see cfg_param_dup()
 * @see cfg_param_data_t
 */
extern int cfg_param_set_raw( cfg_descr_t cfg,
    const cfg_key_path_t* path,
    const cfg_param_data_t* data_str
);
/**
 * Get "binded" or "attended" structure.
 * The main difference between cfg_param_get_binded_raw and
 * cfg_param_get_raw is that the former returns a pointer to not cloned
 * but original structure from iternal buffer which is binded to
 * parameter's tree. You have to deal with returned value with
 * additional care but you don't need to utilize it. If you pass it to
 * cfg_param_destroy then nothing will be done. Advantages and
 * disadvantages of this approach are obvious. Moreover, you aren't
 * permitted to use anything but "binded" structure in
 * cfg_param_update_raw().
 * @param cfg config descriptor
 * @param path path to key which data we want to get
 * @return pointer to "binded" structure represents data;
 *         NULL if error is encountered
 * @see cfg_param_update_raw()
 * @see cfg_param_data_t
 */
extern cfg_param_data_t* cfg_param_get_binded_raw( cfg_descr_t cfg,
    const cfg_key_path_t* path
);
/**
 * Update parameter binded to passed structure.
 * This is a tricky one. But, please, don't think that trick is a rule
 * for library. I'd like to state that this routine is a way to provide
 * optimized way to operate with parameters if you know what you are
 * doing. OK. Now, let's think about particular use case. For instance,
 * we have variable in some language which is binded to particular
 * parameter. Eventually, we will want to change value of this variable.
 * If we use cfg_param_set_raw then it will walk through entire path
 * in the tree from config root. It's a bit expensive. The fact is that
 * structure with data which is returned by cfg_param_get_raw is binded
 * to particular parameter (not duplicate of this structure !!!). We may
 * use it as direct cursor to parameter. So, we may just change its data
 * field and invoke cfg_param_update_raw. If structure isn't binded to
 * any parameter (when it's generated manually or duplicated from
 * original) then routine returns error. Update is performing very
 * efficiently and it's based on pointers reassigning. Additionally
 * you may be interested in synchronising. See cfg_param_hold() for
 * that.
 * @param cfg config descriptor; nothing interesting
 * @param data_str "binded" or "original" data structure
 * @return >0 if everything has been done successfully; 0 - otherwise
 * @see cfg_param_get_binded_raw()
 * @see cfg_param_data_t
 */
extern int cfg_param_update_raw( cfg_descr_t cfg,
    const cfg_param_data_t* data_str
);
/**
 * Synchronize on passed binded parameter's structure.
 * This is a way to control access to binded structure on
 * multi-threaded environment. Don't think that you don't need it in
 * single-threaded program because you may imagine situation with
 * remote source of config. It may be, for instance, LDAP server. Quite
 * the same thing as cfg_lock_set() and cfg_lock_reset() but more
 * convinient and faster if you deal with "binded" structure.
 * @param cfg config descriptor
 * @param data_str object of synchronization
 * @param lock_type type of the lock
 * @return >0 if everything is OK, 0 - otherwise
 * @see cfg_param_unlock()
 * @see cfg_lock_type_t
 * @see cfg_param_data_t
 */
extern int cfg_param_lock( cfg_descr_t cfg,
    const cfg_param_data_t* data_str,
    cfg_lock_type_t lock_type
);
/**
 * Release lock.
 * This operation releases parameter value locked by cfg_param_lock().
 * For expanded description see its description.
 * @param cfg config descriptor
 * @param data_str object of synchronization
 * @return >0 - OK; 0 - error
 * @see cfg_param_lock()
 * @see cfg_param_get_binded_raw()
 */
extern int cfg_param_unlock( cfg_descr_t cfg,
    const cfg_param_data_t* data_str
);
/**
 * Duplicates passed structure deeply.
 * Function creates entire duplicate of passed parameter structure
 * (including sub-array if this is multi-dimensional array).
 * @param cfg config descriptor
 * @param source value which will be cloned
 * @return clone or NULL if error has occured
 * @see cfg_param_get_binded_raw()
 * @see cfg_param_destroy()
 */
extern cfg_param_data_t* cfg_param_dup( cfg_descr_t cfg,
    const cfg_param_data_t* source
);
/**
 * Utilize cloned value.
 * Recycles clones of "binded" values created by cfg_param_get_raw() and
 * cfg_param_dup().
 * @param clone structure to utilize
 * @return >0 - ok; 0 - error
 * @see cfg_param_get_raw()
 * @see cfg_param_dup()                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
 */
extern int cfg_param_destroy( cfg_param_data_t* clone );
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
extern int cfg_param_resize_array( cfg_param_vector_data_t* data,
    size_t new_length
);
/**
 * Represents iterator.
 * This thing is used in within operations related to iteration through
 * node children. As it has been mentioned before, we have tree-like
 * config structure. Some nodes may have variadic number of subnodes.
 * It may be used to construct hashtables in config. So we have ability
 * to iterate through keys and its values.
 * @see cfg_param_iterator_get()
 * @see cfg_param_iterator_next()
 */
typedef void* cfg_param_iterator_t;
/**
 * Creates iterator.
 * Everything is simple - we create iterator for iterating through
 * children of passed subnode.
 * @param cfg config descriptor
 * @param path subnode path
 * @return iterator if everything is fine; NULL - otherwise; iterator
 *         should be utilized via cfg_destroy_desc routine
 * @see cfg_param_iterator_t
 * @see cfg_param_iterator_next()
 */
extern cfg_param_iterator_t cfg_param_iterator_get( cfg_descr_t cfg,
    const cfg_key_path_t* path
);
/**
 * Get next key-value pair.
 * Get key-value pair of the next child according to passed iterator's
 * position. We can get only key or only value. It depends on what we
 * pass as argument for key and value parameters.
 * @param cfg config descriptor
 * @param iter iterator
 * @param key pointer to variable where pointer to key name will be
 *            stored; if NULL then key name won't be returned
 * @param value pointer to variable where pointer to "binded" parameter
 *              value will be stored; if NULL then key name won't be
 *              returned
 * @return >0 - ok; 0 - otherwise
 * @see cfg_param_iterator_get()
 * @see cfg_param_iterator_t
 * @see cfg_param_get_binded_raw()
 */
extern int cfg_param_iterator_next( cfg_descr_t cfg,
    cfg_param_iterator_t iter,
    const char** key,
    cfg_param_data_t** value
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
