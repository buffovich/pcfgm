#ifndef PCFGMOD_INTERNAL_UTILS
#define PCFGMOD_INTERNAL_UTILS

/**
 * Get program name.
 * Portable way to obtain current program name.
 * @return null-terminated string which represents name
 */
extern char* _util_get_program_name( void );
/**
 * Merge file path components.
 * Portable way to merge several path components to one string.
 * @param num number of components to merge
 * @return merged path
 */
extern char* _util_merge_file_path( int num, ... );

#endif
