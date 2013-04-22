#ifndef PCFGM_ERR
#define PCFGM_ERR

typedef enum {
	CFG_OK = 0,
	// everything is clear here
	CFG_NODE_DOES_NOT_EXIST,
	// we tried to convert blob to wrong type
	CFG_TYPE_UNAPPLICABLE_TYPE,
	// unexpected NULL node
	CFG_NODE_NULL,
	// unexpected NULL class
	CFG_CLASS_NULL,
	// unexpected NULL data
	CFG_DATA_NULL,
	// unexpected non-NULL data
	CFG_DATA_UNEXPECTED
} err_t;

/**1
 * Returns error code and error message.
 * If troubles strike then you can read what is happened through
 * mentioned routine. It returns error code and (optionally)
 * human-readable error message.
 * @param message pointer to variable where pointer to string with error
 *                will be stored; if NULL then no error messsage will be
 *                returned;
 * @return error code
 */
// TODO: do it in thread-local storage
extern err_t cfg_error( char** message );

extern void cfg_clear_error( void );

#endif
