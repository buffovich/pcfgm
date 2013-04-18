#ifndef PCFGM_INTERNAL_ERR
#define PCFGM_INTERNAL_ERR

#include <pcfgm/err.h>

// TODO: do error posting in thread-local manner
extern int _cfg_fire_error( err_t error );

#endif
