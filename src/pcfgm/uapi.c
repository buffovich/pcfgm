#include <pcfgm/uapi.h>

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
// For convinient handling of file path
#include <libgen.h>
/**
 * \todo Provide clean way to specify this path through compiler
 * directives.
 */
#define _PATH_DELIMITER '/'
#define _DEFAULT_FILE_BACKEND "libyaml.so"
static const char _g_path_to_plugins[] = '/usr/lib/libpcfgm.d';
static const char _g_path_to_meta_config[] = "/etc/pcfgm.d";

/**
 * \todo Portable way to get program title
 */
char* _get_program_name( void ) {
}

cfg_t cfg_init( void ) {
    char* pname = _get_program_name();
    cfg_t ret = cfg_init_as( pname );
    free( pname );
    return ret;
}

cfg_t cfg_init_as( const char* app_id ) {
    char* buffer = calloc( 
        strlen( _g_path_to_meta_config ) + strlen( app_id ) + 2,
        sizeof( char )
    );
    
    sprintf( buffer, "%s%c%s",
        _g_path_to_meta_config,
        _PATH_DELIMITER,
        app_id
    );
    
    free( buffer );
}
