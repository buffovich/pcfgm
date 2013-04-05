#ifndef API_ERR
#define API_ERR

typedef enum {
	CFG_OK = 0,
	CFG_NODE_DOES_NOT_EXIST
} err_t;

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
// TODO: do it in thread-local storage
extern err_t cfg_error( char** message );

#endif
