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

#endif PCFGM_INTERNAL_UTILS
