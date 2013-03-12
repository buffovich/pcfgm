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
extern int cfg_lock( cfg_node_t what,
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
extern int cfg_unlock( cfg_node_t what,
    char is_recursive
);
