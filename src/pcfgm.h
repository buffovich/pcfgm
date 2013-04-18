#ifndef PCFGMOD_USER_API
#define PCFGMOD_USER_API

#include <stddef.h>
#include <pcfgm/types.h>

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
 * \todo Create utils.h with user wishes. I realize that menrioned
 * API may be too minimalistic sometimes.
 *
 * \section incapsulation Data incapsulation
 * You may be noticed already that some of standart structures has
 * a lack of fields. One of "the most noticeable" lacks is that you
 * have no some kind of type_id field. Don't worry. Everything will
 * be done for you by library (data type parsing, transorming and
 * checking). Stay tuned to see how.
 */

#endif
