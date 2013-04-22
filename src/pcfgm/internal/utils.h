#ifndef PCFGM_INTERNAL_UTILS
#define PCFGM_INTERNAL_UTILS

/*
 * Framework has high-level API and plugin API. The trick is that just before
 * return from each high-level method, all work should be done.
 * Put the work into thread-local variable.
 */
extern int _cfg_work_add( int (*handler)( void* ), void *data );

/*
 * Returns zero if we are in work processing cycle now
 */
extern int _cfg_work_handle( void );

static inline int _cfg_null_check( void *n, err_t err ) {
	if( n == NULL ) {
		_cfg_fire_error( err );
		return 0;
	}

	return 1;
}

#endif PCFGM_INTERNAL_UTILS
