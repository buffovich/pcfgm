#include <utils/internal.h>

#define _DEFAULT_FILE_BACKEND "libyaml.so"
#define _PATH_DELIMITER '/'

static const char __g_path_to_plugins[] = '/usr/lib/libpcfgm.d';
static const char __g_path_to_meta_config[] = "/etc/pcfgm.d";

/**
 * \todo Portable way to get program title
 */
char* _util_get_program_name( void ) {
}

char* _util_merge_file_path( int num, ... ) {
}
