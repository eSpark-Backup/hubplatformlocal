#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <znet/znet.h>
#include <math.h>
#include <inttypes.h>

#define UNUSED( x ) ( (void)x )

sig_atomic_t doit = 1;
int ftty = -1;
const char* tty = NULL;
size_t store_dat_size = 0;
void* store_dat = NULL;
const char* store = NULL;

void squit( int sig ) {
    UNUSED( sig );

    doit = 0;
}

void zn_print_hex( const void* data, size_t data_size ) {
    if( !data || !data_size )
        return;
    while( data_size ) {
        printf( "%016lX:", (uint64_t)data );
        for( size_t i = 0; data_size && ( i < 8 ); ++i, ++data, --data_size ) {
            printf( " %02hhX", *( (uint8_t*)data ) );
        }
        printf( "\n" );
    }
}

/////////////////////////////////////////////////////

typedef struct zn_cmd_arg_t zn_cmd_arg_t;
struct zn_cmd_arg_t {
    const char* name;
    const char* value;
    zn_cmd_arg_t* next;
};

const char* zn_args_get_value_by_name( const zn_cmd_arg_t* args, const char* name ) {
    while( args ) {
        if( strcmp( args->name, name ) == 0 ) {
            return args->value;
        }
        args = args->next;
    }
    return NULL;
}

void zn_factory( const zn_cmd_arg_t* args );
void zn_net_update( const zn_cmd_arg_t* args );
void zn_add( const zn_cmd_arg_t* args );
void zn_rem( const zn_cmd_arg_t* args );
void zn_rep_failed( const zn_cmd_arg_t* args );
void zn_learn( const zn_cmd_arg_t* args );
void zn_info( const zn_cmd_arg_t* args );
void zn_basic( const zn_cmd_arg_t* args );
void zn_binary_switch( const zn_cmd_arg_t* args );
void zn_clock_device( const zn_cmd_arg_t* args );
void zn_color_switch( const zn_cmd_arg_t* args );
void zn_door_lock( const zn_cmd_arg_t* args );
void zn_multichannel( const zn_cmd_arg_t* args );
void zn_powerlevel( const zn_cmd_arg_t* args );
void zn_meter( const zn_cmd_arg_t* args );
void zn_msensor( const zn_cmd_arg_t* args );
void zn_bsensor( const zn_cmd_arg_t* args );
void zn_asensor( const zn_cmd_arg_t* args );
void zn_quit_cli( const zn_cmd_arg_t* args );
void zn_help_cli( const zn_cmd_arg_t* args );

/////////////////////////////////////////////////////

typedef void ( *ZN_CMD_FUNC )( const zn_cmd_arg_t* args );

typedef struct zn_arg_entry_t zn_arg_entry_t;
struct zn_arg_entry_t {
    const char* label;
    int has_param;
};

typedef struct zn_cmd_entry_t zn_cmd_entry_t;
struct zn_cmd_entry_t {
    const char* label;
    ZN_CMD_FUNC func;
    zn_arg_entry_t* args;
    const char* desc;
};

zn_arg_entry_t zn_args_rep[] = { { "node", !0 }, { NULL, 0 } };

zn_arg_entry_t zn_args_info[] = { { "get", !0 }, { NULL, 0 } };

zn_arg_entry_t zn_args_basic[] = { { "get", !0 }, { "set", !0 }, { "value", !0 }, { NULL, 0 } };

zn_arg_entry_t zn_args_binary_switch[] = { { "get", !0 }, { "set", !0 }, { "value", !0 }, { NULL, 0 } };

zn_arg_entry_t zn_args_clock_device[] = { { "get", !0 },
                                          { "set", !0 },
                                          { "time", !0 },
                                          { "weekday", !0 },
                                          { NULL, 0 } };

zn_arg_entry_t zn_args_color_switch[] = { { "get", !0 },
                                          { "set", !0 },
                                          { "component", !0 },
                                          { "value", !0 },
                                          { "start_change", !0 },
                                          { "stop_change", !0 },
                                          { "up_down", !0 },
                                          { "duration", !0 },
                                          { "ignore_start_lvl", !0 },
                                          { "supported", !0 },
                                          { NULL, 0 } };

zn_arg_entry_t zn_args_door_lock[] = { { "get", !0 }, { "set", !0 }, { "value", !0 }, { NULL, 0 } };

zn_arg_entry_t zn_args_multichannel[] = { { "count", !0 },   { "find", !0 },     { "capability", !0 },
                                          { "generic", !0 }, { "specific", !0 }, { NULL, 0 } };

zn_arg_entry_t zn_arg_powerlevel[] = { { "get", !0 },     { "set", !0 },   { "level", !0 },
                                       { "timeout", !0 }, { "tget", !0 },  { "tset", !0 },
                                       { "node", !0 },    { "count", !0 }, { NULL, 0 } };

zn_arg_entry_t zn_arg_meter[] = { { "get", !0 }, { "scale", !0 }, { "supported", !0 }, { "reset", !0 } };

zn_arg_entry_t zn_arg_msensor[] = { { "supported", !0 }, { "scale", !0 },   { "get", !0 }, { "type", !0 },
                                    { "nscale", !0 },    { "version", !0 }, { NULL, 0 } };

zn_arg_entry_t zn_arg_bsensor[] = { { "supported", !0 },
                                    { "get", !0 },
                                    { "type", !0 },
                                    { "version", !0 },
                                    { NULL, 0 } };

zn_arg_entry_t zn_arg_asensor[] = { { "supported", !0 }, { "get", !0 }, { "type", !0 }, { NULL, 0 } };

zn_cmd_entry_t zn_cmds[] = {
    { "factory", zn_factory, NULL, "Reset z-wave module to factory default" },
    { "update", zn_net_update, NULL, "Request network update from SUC/SIS" },
    { "add", zn_add, NULL, "Start inclusion any device" },
    { "rem", zn_rem, NULL, "Start exclusion any device" },
    { "rep", zn_rep_failed, zn_args_rep, "Start replace failed device" },
    { "learn", zn_learn, NULL, "Start learn mode" },
    { "list", zn_list, NULL, "Show list of devices in network (cached)" },
    { "info", zn_info, zn_args_info, "Show device info: info get <addr>" },
    { "basic",
      zn_basic,
      zn_args_basic,
      "Command BASIC: basic get <addr>[:<chan>]; basic set <addr>[:<chan>] "
      "value <value>" },
    { "bswitch",
      zn_binary_switch,
      zn_args_binary_switch,
      "Command SWITCH_BINARY: bswitch get <addr>[:<chan>]; bswitch set "
      "<addr>[:<chan>] "
      "value <value>" },
    { "clockdev",
      zn_clock_device,
      zn_args_clock_device,
      "Command CLOCK_DEVICE: clockdev get <addr>[:<chan>]; clockdev set "
      "<addr>[:<chan>] time <HH:MM> weekday <weekday>;" },
    { "cswitch",
      zn_color_switch,
      zn_args_color_switch,
      "Command SWITCH_COLOR: cswitch get <addr>[:<chan>] component <component>; "
      "cswitch set <addr>[:<chan>] component <component> value <value>; "
      "cswitch start_change <addr>[:<chan>] component <component> value <value> "
      "up_down <up_down> ignore_start_lvl <ignore_start_lvl> duration <duration>; "
      "cswitch stop_change <addr>[:<chan>] component <component>; "
      "cswitch supported <addr>[:<chan>];" },
    { "dlock",
      zn_door_lock,
      zn_args_door_lock,
      "Command DOOR_LOCK: dlock get <addr>[:<chan>]; dlock set "
      "<addr>[:<chan>] "
      "value <value>" },
    { "channel",
      zn_multichannel,
      zn_args_multichannel,
      "Command MULTICHANNEL: channel count <addr>; channel capability "
      "<addr>:<chan>; channel find <addr> [generic <generic> specific "
      "<specific>]" },
    { "powerlvl",
      zn_powerlevel,
      zn_arg_powerlevel,
      "Command POWERLEVEL: powerlvl get <addr>; powerlvl set <addr> value "
      "<value: valid 0-9> [timeout <timeout: defualt 10>; powerlvl tget "
      "<addr>; powerlvl tset <addr> node <addr> level <level: valid 0-9> count "
      "<count>" },
    { "meter",
      zn_meter,
      zn_arg_meter,
      "Command METER: meter supported <addr>[:<chan>]; meter get "
      "<addr>[:<chan>] scale <scale>; meter reset <addr>[:<chan>]" },
    { "msensor",
      zn_msensor,
      zn_arg_msensor,
      "Command msensor: "
      "msensor supported <addr>[:<chan>]; "
      "msensor scale <addr> type <type: valid 1-68>; "
      "msensor get <addr>[:<chan>] type <type: valid 1-68> nscale <scale: valid 0-3>; version <version: valid 1-11>" },
    { "bsensor",
      zn_bsensor,
      zn_arg_bsensor,
      "Command bsensor: "
      "bsensor supported <addr>; "
      "bsensor get <addr> version <version> type <type: valid 1 - 13, 255>; " },
    { "asensor",
      zn_asensor,
      zn_arg_asensor,
      "Command asensor: "
      "asensor supported <addr>; "
      "asensor get <addr> type <type: valid 0, 1, 2, 4, 5, 255>; " },
    { "quit", zn_quit_cli, NULL, "Quit" },
    { "help", zn_help_cli, NULL, "This help" },
    { NULL, NULL, NULL, NULL }
};

// factory
// add
// rem
// list
// info get <addr>
// basic get <addr>[:<chan>]
// basic set <addr>[:<chan>] value <value>
// channel count <addr>
// channel capability <addr>:<chan>
// channel find <addr> [generic <generic> specific <specific>]

/////////////////////////////////////////////////////

void rl_cb( char* line ) {
    if( NULL == line ) {
        doit = 0;
        return;
    }

    if( strlen( line ) > 0 && line[0] != ' ' ) {
        add_history( line );
    }

    ZN_CMD_FUNC func = NULL;
    zn_cmd_arg_t* args = NULL;
    /// INFO: parse line
    zn_cmd_entry_t* cmd = zn_cmds;
    zn_arg_entry_t* arg = NULL;

    char* t = strtok( line, " \t\n" );
    while( t )  // search cmd
    {
        zn_cmd_entry_t* i = cmd;
        for( ; i && i->label; ++i ) {
            if( strstr( i->label, t ) == i->label ) {
                func = i->func;
                arg = i->args;
                break;
            }
        }
        if( !i || !i->label ) {
            t = NULL;
            break;
        }
        t = strtok( NULL, " \t\n" );
        if( func )  // func founded, next args
        {
            break;
        }
    }
    while( t )  // search args
    {
        for( zn_arg_entry_t* i = arg; i && i->label; ++i ) {
            if( strstr( i->label, t ) == i->label ) {
                zn_cmd_arg_t* n = (zn_cmd_arg_t*)malloc( sizeof( zn_cmd_arg_t ) );
                memset( n, 0, sizeof( zn_cmd_arg_t ) );
                n->name = i->label;
                if( i->has_param ) {
                    n->value = strtok( NULL, " \t\n" );
                    if( !n->value ) {
                        t = NULL;
                    }
                }
                else {
                    n->value = NULL;
                }
                n->next = args;
                args = n;

                break;
            }
        }
        if( t ) {
            t = strtok( NULL, " \t\n" );
        }
    }
    /// INFO: call
    if( func ) {
        func( args );
    }
    else {
        fprintf( stderr, "unknown command\n" );
    }
    while( args ) {
        zn_cmd_arg_t* p = args;
        args = args->next;
        free( p );
    }
    free( line );
}

/////////////////////////////////////////////////////

void zn_factory( const zn_cmd_arg_t* args ) {
    UNUSED( args );

    znet_set_default();
}

void zn_factory_result( znet_error_t err, void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "Factory reset fail!\n" );
    }
    else {
        printf( "Factory reset succ!\n" );
    }
}

void zn_net_update( const zn_cmd_arg_t* args ) {
    UNUSED( args );

    znet_network_update();
}

void zn_net_update_result( znet_error_t err, void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "Request network update fail!\n" );
    }
    else {
        printf( "Request network update succ!\n" );
    }
}

void zn_print_node_info( const znet_nodeinfo_t* node_info ) {
    printf( "Node info ->\n"
            "    id: %hhu\n"
            "    security: %hhu\n"
            "    basic: %hhu\n"
            "    generic: %hhu\n"
            "    specific: %hhu\n"
            "    commands count: %hhu\n"
            "\n",
            node_info->node_id,
            node_info->security,
            node_info->basic,
            node_info->generic,
            node_info->specific,
            node_info->commands_count );
    zn_print_hex( node_info->commands, node_info->commands_count * sizeof( znet_command_class_t ) );
}

void zn_add( const zn_cmd_arg_t* args ) {
    UNUSED( args );

    znet_node_add();
}

void zn_add_result( znet_error_t err, const znet_nodeinfo_t* node_info, void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "Add node fail!\n" );
    }
    else {
        printf( "Add node succ! Node ID: %hhu\n", node_info->node_id );
        zn_print_node_info( node_info );

        size_t size;
        znet_security_supported( node_info->node_id, NULL, &size );
        znet_security_supported_report_t* security_supported_report = (znet_security_supported_report_t*)malloc( size );
        memset( security_supported_report, 0, size );
        if( znet_security_supported( node_info->node_id, security_supported_report, &size ) == 0 ) {
            zn_print_hex( security_supported_report, size );
        }
        free( security_supported_report );
    }
}

void zn_security_supported_result( znet_error_t err,
                                   znet_node_id_t node_id,
                                   znet_node_channel_id_t channel_id,
                                   const znet_security_supported_report_t* value,
                                   void* arg ) {
    UNUSED( arg );
    UNUSED( channel_id );

    if( err ) {
        printf( "Security supported fail!\n" );
    }
    else {
        printf( "Security supported succ! Node ID: %hhu\n", node_id );
        if( !value )
            return;

        size_t size;
        znet_security_supported( node_id, NULL, &size );
        znet_security_supported_report_t* security_supported_report = (znet_security_supported_report_t*)malloc( size );
        memset( security_supported_report, 0, size );
        if( znet_security_supported( node_id, security_supported_report, &size ) == 0 ) {
            zn_print_hex( security_supported_report, size );
        }
        free( security_supported_report );
    }
}

void zn_rem( const zn_cmd_arg_t* args ) {
    UNUSED( args );

    znet_node_rem();
}

void zn_rem_result( znet_error_t err, const znet_nodeinfo_t* node_info, void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "Rem node fail!\n" );
    }
    else {
        printf( "Rem node succ! Node ID: %hhu\n", node_info->node_id );
        zn_print_node_info( node_info );
    }
}

void zn_rep_failed( const zn_cmd_arg_t* args ) {
    const char* addr_s = zn_args_get_value_by_name( args, "node" );
    if( !addr_s ) {
        fprintf( stderr, "info: invalid address!\n" );
        return;
    }
    uint8_t addr = 0;
    if( sscanf( addr_s, "%hhu", &addr ) < 1 ) {
        fprintf( stderr, "info: invalid address!\n" );
        return;
    }

    znet_node_replace_if_failed( addr );
}

void zn_rep_result( znet_error_t err, const znet_nodeinfo_t* node_info, void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "Replace node fail!\n" );
    }
    else {
        printf( "Replace node succ! Node ID: %hhu\n", node_info->node_id );
        zn_print_node_info( node_info );

        size_t size;
        znet_security_supported( node_info->node_id, NULL, &size );
        znet_security_supported_report_t* security_supported_report = (znet_security_supported_report_t*)malloc( size );
        memset( security_supported_report, 0, size );
        if( znet_security_supported( node_info->node_id, security_supported_report, &size ) == 0 ) {
            zn_print_hex( security_supported_report, size );
        }
        free( security_supported_report );
    }
}

void zn_learn( const zn_cmd_arg_t* args ) {
    UNUSED( args );

    znet_node_learn();
}

void zn_learn_result( znet_error_t err, const znet_nodeinfo_t* node_info, znet_node_id_t dest_node_id, void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "Learn node fail!\n" );
    }
    else {
        printf( "Learn node succ!\n" /* Node ID: %hhu\n", node_info->node_id*/ );
        //        zn_print_node_info( node_info );
    }
}

void zn_list_result( znet_error_t err, const znet_nodeinfo_t* node_info, void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "List nodes fail!\n" );
    }
    else {
        if( node_info ) {
            printf( "List nodes. Node ID: %hhu\n", node_info->node_id );
            zn_print_node_info( node_info );
        }
        else {
            printf( "List nodes. Done\n" );
        }
    }
}

void zn_update_info_result( znet_error_t err, const znet_nodeinfo_t* node_info, void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "Update info fail!\n" );
    }
    else {
        printf( "Update info succ! Node ID: %hhu\n", node_info->node_id );
        zn_print_node_info( node_info );
    }
}

static int _select_keys_do = 0;
static znet_S2_keys_t _select_keys;

void zn_s2_select_keys( znet_S2_keys_t select_keys, void* arg ) {
    // select all supported

    _select_keys = select_keys;
    _select_keys_do = 1;
    //    znet_node_s2_select_keys( select_keys & ZNET_SECURITY2_S2_CLASS_0 );
}

static int _pub_key_do = 0;

void zn_s2_enter_pub_key( znet_S2_keys_t select_keys, const znet_S2_pub_key_t* pub_key, void* arg ) {
    printf( "S2 selected keys: %hhu\n", select_keys );
    printf( "S2 remote public key:\n" );
    zn_print_hex( pub_key->key, pub_key->size );
    if( pub_key->size == 14 ) {
        printf( "Enter key>" );
        //    uint8_t key[sizeof( znet_S2_pub_key_t ) + 2];

        _pub_key_do = 20000;
        //    // test
        //    key[0] = 2;
        //    key[1] = 0x4F;
        //    key[2] = 0x07;
        ////    key[1] = 0x86;
        ////    key[2] = 0x66;

        //    znet_node_s2_response_pub_key( (const znet_S2_pub_key_t*)key );
    }
    if( pub_key->size == 16 ) {
        // enter key for CSA
    }
}

void zn_s2_show_pub_key( znet_S2_keys_t select_keys, const znet_S2_pub_key_t* pub_key, void* arg ) {
    printf( "S2 remote public key:\n" );
    zn_print_hex( pub_key->key, pub_key->size );
}

void zn_info( const zn_cmd_arg_t* args ) {
    const char* addr_s = zn_args_get_value_by_name( args, "get" );
    if( !addr_s ) {
        fprintf( stderr, "info: invalid address!\n" );
        return;
    }
    uint8_t addr = 0;
    if( sscanf( addr_s, "%hhu", &addr ) < 1 ) {
        fprintf( stderr, "info: invalid address!\n" );
        return;
    }

    size_t node_info_size = 0;
    size_t node_info_size_tmp;
    znet_nodeinfo_t* node_info = NULL;
    if( znet_node_info( addr, node_info, &node_info_size ) ) {
        fprintf( stderr, "info: " );
        return;
    }
    node_info = (znet_nodeinfo_t*)malloc( node_info_size );
    node_info_size_tmp = node_info_size;
    if( znet_node_info( addr, node_info, &node_info_size_tmp ) ) {
        fprintf( stderr, "info: not found" );
        return;
    }
    zn_print_node_info( node_info );
    free( node_info );
}

void zn_basic( const zn_cmd_arg_t* args ) {
    const char* addr_s = zn_args_get_value_by_name( args, "get" );
    if( addr_s ) {
        // get
        uint8_t addr = 0;
        uint8_t channel = ZNET_CHANNEL_ID_ROOT;
        if( sscanf( addr_s, "%hhu:%hhu", &addr, &channel ) < 1 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }

        znet_node_cmd_basic_get( addr, channel );
        return;
    }
    addr_s = zn_args_get_value_by_name( args, "set" );
    if( !addr_s ) {
        fprintf( stderr, "info: invalid command!\n" );
        return;
    }
    // set
    uint8_t addr = 0;
    uint8_t channel = ZNET_CHANNEL_ID_ROOT;
    if( sscanf( addr_s, "%hhu:%hhu", &addr, &channel ) < 1 ) {
        fprintf( stderr, "info: invalid address!\n" );
        return;
    }
    const char* value_s = zn_args_get_value_by_name( args, "value" );
    if( !value_s ) {
        fprintf( stderr, "info: invalid value!\n" );
        return;
    }
    uint8_t value = 0;
    if( sscanf( value_s, "%hhu", &value ) < 1 ) {
        fprintf( stderr, "info: invalid value!\n" );
        return;
    }

    znet_node_cmd_basic_set( addr, channel, value );
}

void zn_basic_result( znet_error_t err,
                      znet_node_id_t node_id,
                      znet_node_channel_id_t channel_id,
                      znet_cmd_basic_value_t value,
                      void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "Get basic fail!\n" );
    }
    else {
        printf( "Get basic succ! Node ID: %hhu:%hhu Value: %hhu\n", node_id, channel_id, value );
    }
}

void zn_binary_switch( const zn_cmd_arg_t* args ) {
    const char* addr_s = zn_args_get_value_by_name( args, "get" );
    if( addr_s ) {
        // get
        uint8_t addr = 0;
        uint8_t channel = ZNET_CHANNEL_ID_ROOT;
        if( sscanf( addr_s, "%hhu:%hhu", &addr, &channel ) < 1 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }

        znet_node_cmd_binary_switch_get( addr, channel );
        return;
    }
    addr_s = zn_args_get_value_by_name( args, "set" );
    if( !addr_s ) {
        fprintf( stderr, "info: invalid command!\n" );
        return;
    }
    // set
    uint8_t addr = 0;
    uint8_t channel = ZNET_CHANNEL_ID_ROOT;
    if( sscanf( addr_s, "%hhu:%hhu", &addr, &channel ) < 1 ) {
        fprintf( stderr, "info: invalid address!\n" );
        return;
    }
    const char* value_s = zn_args_get_value_by_name( args, "value" );
    if( !value_s ) {
        fprintf( stderr, "info: invalid value!\n" );
        return;
    }
    uint8_t value = 0;
    if( sscanf( value_s, "%hhu", &value ) < 1 ) {
        fprintf( stderr, "info: invalid value!\n" );
        return;
    }

    znet_node_cmd_binary_switch_set( addr, channel, value );
}

void zn_binary_switch_result( znet_error_t err,
                              znet_node_id_t node_id,
                              znet_node_channel_id_t channel_id,
                              znet_cmd_basic_value_t value,
                              void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "Get switch binary fail!\n" );
    }
    else {
        printf( "Get switch binary succ! Node ID: %hhu:%hhu Value: %hhu\n", node_id, channel_id, value );
    }
}

void zn_clock_device( const zn_cmd_arg_t* args ) {
    const char* arg_value_s = zn_args_get_value_by_name( args, "get" );
    if( arg_value_s ) {
        // get
        uint8_t addr = 0;
        uint8_t channel = ZNET_CHANNEL_ID_ROOT;
        if( sscanf( arg_value_s, "%hhu:%hhu", &addr, &channel ) < 1 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }

        if( znet_node_cmd_clock_get( addr, channel ) != ZNET_ERR_OK ) {
            fprintf( stderr, "info: error executing get clock command!\n" );
        };
        return;
    }
    arg_value_s = zn_args_get_value_by_name( args, "set" );
    if( arg_value_s ) {
        // set
        uint8_t addr = 0;
        uint8_t channel = ZNET_CHANNEL_ID_ROOT;
        if( sscanf( arg_value_s, "%hhu:%hhu", &addr, &channel ) < 1 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }
        znet_cmd_clock_hour_value_t hour;
        znet_cmd_clock_minute_value_t minute;
        znet_cmd_clock_weekday_value_t weekday;
        arg_value_s = zn_args_get_value_by_name( args, "time" );
        if( arg_value_s ) {
            if( sscanf( arg_value_s, "%hhu:%hhu", &hour, &minute ) < 1 ) {
                fprintf( stderr, "info: invalid time!\n" );
                return;
            }
        }
        else {
            fprintf( stderr, "info: invalid command!\n" );
            return;
        }
        arg_value_s = zn_args_get_value_by_name( args, "weekday" );
        if( arg_value_s ) {
            if( sscanf( arg_value_s, "%hhu", &weekday ) < 1 ) {
                fprintf( stderr, "info: invalid weekday!\n" );
                return;
            }
        }
        else {
            fprintf( stderr, "info: invalid command!\n" );
            return;
        }
        if( znet_node_cmd_clock_set( addr, channel, hour, minute, weekday ) != ZNET_ERR_OK ) {
            fprintf( stderr, "info: error executing set clock command!\n" );
        }
        return;
    }
    fprintf( stderr, "info: invalid command!\n" );
    return;
}

void zn_clock_device_result( znet_error_t err,
                             znet_node_id_t node_id,
                             znet_node_channel_id_t channel_id,
                             const znet_cmd_clock_report_t* value,
                             void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "Get clock device fail!\n" );
    }
    else {
        printf( "Get clock device succ! Node ID: %hhu:%hhu Time(HH:MM): %hhu:%hhu Weekday: %hhu\n",
                node_id,
                channel_id,
                value->hour,
                value->minute,
                value->weekday );
    }
}

void zn_color_switch( const zn_cmd_arg_t* args ) {
    const char* addr_s = zn_args_get_value_by_name( args, "get" );
    if( addr_s ) {
        // get
        znet_node_id_t node_id = 0;
        znet_node_channel_id_t channel_id = ZNET_CHANNEL_ID_ROOT;
        if( sscanf( addr_s, "%hhu:%hhu", &node_id, &channel_id ) < 1 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }

        const char* component_s = zn_args_get_value_by_name( args, "component" );
        uint8_t component_id = 0;
        if( component_s ) {
            if( sscanf( component_s, "%hhu", &component_id ) < 1 ) {
                fprintf( stderr, "info: invalid component id!\n" );
                return;
            }
        }
        else {
            fprintf( stderr, "info: invalid command!\n" );
            return;
        }

        znet_node_cmd_color_switch_get( node_id, channel_id, component_id );
    }

    addr_s = zn_args_get_value_by_name( args, "set" );
    if( addr_s ) {
        // set
        znet_node_id_t node_id = 0;
        znet_node_channel_id_t channel_id = ZNET_CHANNEL_ID_ROOT;
        if( sscanf( addr_s, "%hhu:%hhu", &node_id, &channel_id ) < 1 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }

        const char* component_s = zn_args_get_value_by_name( args, "component" );
        uint8_t component_id = 0;
        if( component_s ) {
            if( sscanf( component_s, "%hhu", &component_id ) < 1 ) {
                fprintf( stderr, "info: invalid component id!\n" );
                return;
            }
        }
        else {
            fprintf( stderr, "info: invalid component id!\n" );
            return;
        }

        const char* value_s = zn_args_get_value_by_name( args, "value" );
        znet_color_switch_value_t value = 0;
        if( value_s ) {
            if( sscanf( value_s, "%hhu", &value ) < 1 ) {
                fprintf( stderr, "info: invalid value!\n" );
                return;
            }
        }
        else {
            fprintf( stderr, "info: invalid value!\n" );
            return;
        }

        const znet_cl_component_t components = { .component_id = component_id, .value = value };
        znet_node_cmd_color_switch_set( node_id, channel_id, 1, &components );
    }

    addr_s = zn_args_get_value_by_name( args, "start_change" );
    if( addr_s ) {
        // start change
        znet_node_id_t node_id = 0;
        znet_node_channel_id_t channel_id = ZNET_CHANNEL_ID_ROOT;
        if( sscanf( addr_s, "%hhu:%hhu", &node_id, &channel_id ) < 1 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }

        const char* component_s = zn_args_get_value_by_name( args, "component" );
        uint8_t component_id = 0;
        if( component_s ) {
            if( sscanf( component_s, "%hhu", &component_id ) < 1 ) {
                fprintf( stderr, "info: invalid component id!\n" );
                return;
            }
        }
        else {
            fprintf( stderr, "info: invalid component id!\n" );
            return;
        }

        const char* value_s = zn_args_get_value_by_name( args, "value" );
        znet_color_switch_value_t value = 0;
        if( value_s ) {
            if( sscanf( value_s, "%hhu", &value ) < 1 ) {
                fprintf( stderr, "info: invalid value!\n" );
                return;
            }
        }
        else {
            fprintf( stderr, "info: invalid value!\n" );
            return;
        }

        const char* up_down_s = zn_args_get_value_by_name( args, "up_down" );
        uint8_t up_down = 0;
        if( up_down_s ) {
            if( sscanf( up_down_s, "%hhu", &up_down ) < 1 ) {
                fprintf( stderr, "info: invalid up/down value!\n" );
                return;
            }
        }
        else {
            fprintf( stderr, "info: invalid up/down value!\n" );
            return;
        }

        const char* ignore_start_lvl_s = zn_args_get_value_by_name( args, "ignore_start_lvl" );
        uint8_t ignore_start_lvl = 0;
        if( ignore_start_lvl_s ) {
            if( sscanf( ignore_start_lvl_s, "%hhu", &ignore_start_lvl ) < 1 ) {
                fprintf( stderr, "info: invalid ignore_start_lvl value!\n" );
                return;
            }
        }
        else {
            fprintf( stderr, "info: invalid ignore_start_lvl value!\n" );
            return;
        }

        const char* duration_s = zn_args_get_value_by_name( args, "duration" );
        uint8_t duration = 0;
        if( duration_s ) {
            if( sscanf( duration_s, "%hhu", &duration ) < 1 ) {
                fprintf( stderr, "info: invalid duration!\n" );
                return;
            }
        }
        else {
            fprintf( stderr, "info: invalid duration!\n" );
            return;
        }

        znet_node_cmd_color_switch_start_level_change(
            node_id, channel_id, up_down, ignore_start_lvl, component_id, value, duration );
    }

    addr_s = zn_args_get_value_by_name( args, "stop_change" );
    if( addr_s ) {
        // stop change
        znet_node_id_t node_id = 0;
        znet_node_channel_id_t channel_id = ZNET_CHANNEL_ID_ROOT;
        if( sscanf( addr_s, "%hhu:%hhu", &node_id, &channel_id ) < 1 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }

        const char* component_s = zn_args_get_value_by_name( args, "component" );
        uint8_t component_id = 0;
        if( component_s ) {
            if( sscanf( component_s, "%hhu", &component_id ) < 1 ) {
                fprintf( stderr, "info: invalid component id!\n" );
                return;
            }
        }
        else {
            fprintf( stderr, "info: invalid component id!\n" );
            return;
        }

        znet_node_cmd_color_switch_stop_level_change( node_id, channel_id, component_id );
    }

    addr_s = zn_args_get_value_by_name( args, "supported" );
    if( addr_s ) {
        // supported get
        znet_node_id_t node_id = 0;
        znet_node_channel_id_t channel_id = ZNET_CHANNEL_ID_ROOT;
        if( sscanf( addr_s, "%hhu:%hhu", &node_id, &channel_id ) < 1 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }

        znet_node_cmd_color_switch_supported_get( node_id, channel_id );
    }
}

void zn_color_switch_result( znet_error_t err,
                             znet_node_id_t node_id,
                             znet_node_channel_id_t channel_id,
                             const znet_color_switch_report_t* value,
                             void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "Get color switch fail!\n" );
    }
    else {
        printf( "Get color switch succ!\n"
                "    Node id: %hhu\n"
                "    Channel id: %hhu\n"
                "    Component id: %hhu\n"
                "    Value: %hhu\n",
                node_id,
                channel_id,
                value->cl_component_id,
                value->value );
    }
}

void zn_color_switch_supported_result( znet_error_t err,
                                       znet_node_id_t node_id,
                                       znet_node_channel_id_t channel_id,
                                       const znet_color_switch_supported_report_t* report,
                                       void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "Supported get color switch fail!\n" );
    }
    else {
        /// TODO: check component_mask_count
        uint8_t warm_white_supported = report->component_mask[0] >> 0 & 1;
        uint8_t cold_white_supported = report->component_mask[0] >> 1 & 1;
        uint8_t red_supported = report->component_mask[0] >> 2 & 1;
        uint8_t green_supported = report->component_mask[0] >> 3 & 1;
        uint8_t blue_supported = report->component_mask[0] >> 4 & 1;
        uint8_t amber_supported = report->component_mask[0] >> 5 & 1;
        uint8_t cyan_supported = report->component_mask[0] >> 6 & 1;
        uint8_t purple_supported = report->component_mask[0] >> 7 & 1;
        uint8_t indexed_supported = report->component_mask[1] >> 0 & 1;

        printf( "Supported get color switch succ!\n"
                "    Node id: %hhu\n"
                "    Channel id: %hhu\n"
                "    Warm white supported: %hhu\n"
                "    Cold white supported: %hhu\n"
                "    Red supported: %hhu\n"
                "    Green supported: %hhu\n"
                "    Blue supported: %hhu\n"
                "    Amber supported: %hhu\n"
                "    Cyan supported: %hhu\n"
                "    Purple supported: %hhu\n"
                "    Indexed supported: %hhu\n",
                node_id,
                channel_id,
                warm_white_supported,
                cold_white_supported,
                red_supported,
                green_supported,
                blue_supported,
                amber_supported,
                cyan_supported,
                purple_supported,
                indexed_supported );
    }
}

void zn_door_lock( const zn_cmd_arg_t* args ) {
    const char* addr_s = zn_args_get_value_by_name( args, "get" );
    if( addr_s ) {
        // get
        uint8_t addr = 0;
        uint8_t channel = ZNET_CHANNEL_ID_ROOT;
        if( sscanf( addr_s, "%hhu:%hhu", &addr, &channel ) < 1 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }

        znet_node_cmd_door_lock_operation_get( addr, channel );
        return;
    }
    addr_s = zn_args_get_value_by_name( args, "set" );
    if( !addr_s ) {
        fprintf( stderr, "info: invalid command!\n" );
        return;
    }
    // set
    uint8_t addr = 0;
    uint8_t channel = ZNET_CHANNEL_ID_ROOT;
    if( sscanf( addr_s, "%hhu:%hhu", &addr, &channel ) < 1 ) {
        fprintf( stderr, "info: invalid address!\n" );
        return;
    }
    const char* value_s = zn_args_get_value_by_name( args, "value" );
    if( !value_s ) {
        fprintf( stderr, "info: invalid value!\n" );
        return;
    }
    uint8_t value = 0;
    if( sscanf( value_s, "%hhu", &value ) < 1 ) {
        fprintf( stderr, "info: invalid value!\n" );
        return;
    }

    znet_node_cmd_door_lock_operation_set( addr, channel, value );
}

void zn_door_lock_configuration_result( znet_error_t err,
                                        znet_node_id_t node_id,
                                        znet_node_channel_id_t channel_id,
                                        znet_cmd_basic_value_t value,
                                        void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "Get door lock fail!\n" );
    }
    else {
        printf( "Get door lock succ! Node ID: %hhu:%hhu Value: %hhu\n", node_id, channel_id, value );
    }
}

void zn_door_lock_operation_result( znet_error_t err,
                                    znet_node_id_t node_id,
                                    znet_node_channel_id_t channel_id,
                                    znet_door_lock_operation_report_t* value,
                                    void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "Get switch binary fail!\n" );
    }
    else {
        printf( "Get door lock succ! Node ID: %hhu:%hhu Value: %hhu\n", node_id, channel_id, value->mode );
    }
}

void zn_multichannel( const zn_cmd_arg_t* args ) {
    const char* addr_s = zn_args_get_value_by_name( args, "count" );
    if( addr_s ) {
        // count
        uint8_t addr = 0;
        if( sscanf( addr_s, "%hhu", &addr ) < 1 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }

        znet_node_cmd_multichannel_endpoint_get( addr );
        return;
    }
    addr_s = zn_args_get_value_by_name( args, "capability" );
    if( addr_s ) {
        // capability
        uint8_t addr = 0;
        uint8_t channel = 0;
        if( sscanf( addr_s, "%hhu:%hhu", &addr, &channel ) < 2 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }

        znet_node_cmd_multichannel_capability_get( addr, channel );
        return;
    }
    addr_s = zn_args_get_value_by_name( args, "find" );
    if( addr_s ) {
        // find
        uint8_t addr = 0;
        uint8_t generic = 0xFF;
        uint8_t specific = 0xFF;
        if( sscanf( addr_s, "%hhu", &addr ) < 1 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }
        const char* generic_s = zn_args_get_value_by_name( args, "generic" );
        if( generic_s ) {
            if( sscanf( generic_s, "%hhu", &generic ) < 1 ) {
                fprintf( stderr, "info: invalid generic!\n" );
                return;
            }
        }
        const char* specific_s = zn_args_get_value_by_name( args, "specific" );
        if( specific_s ) {
            if( sscanf( specific_s, "%hhu", &specific ) < 1 ) {
                fprintf( stderr, "info: invalid specific!\n" );
                return;
            }
        }
        if( generic != specific && ( generic == 0xFF || specific == 0xFF ) ) {
            fprintf( stderr, "info: invalid generic or specific!\n" );
            return;
        }

        znet_node_cmd_multichannel_endpoint_find( addr, generic, specific );
        return;
    }

    fprintf( stderr, "info: invalid command!\n" );
}

void zn_multichannel_endpoint_result( znet_error_t err,
                                      znet_node_id_t node_id,
                                      const znet_multichannel_endpoint_report_t* value,
                                      void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "Get multichannel endpoint get fail!\n" );
    }
    else {
        printf( "Get multichannel endpoint get succ!\n"
                "    Node ID: %hhu\n"
                "    Dynamic: %s\n"
                "    Identical: %s\n"
                "    Channels Count: %hhu\n"
                "    Aggregated count: %hhu\n",
                node_id,
                value->dynamic ? "true" : "false",
                value->identical ? "true" : "false",
                value->channel_ids_count,
                value->agregated_channel_ids_count );
    }
}

void zn_multichannel_capability_result( znet_error_t err,
                                        znet_node_id_t node_id,
                                        const znet_multichannel_capability_report_t* value,
                                        void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "Get multichannel capability get fail!\n" );
    }
    else {
        printf( "Get multichannel capability get succ!\n"
                "    Node ID: %hhu:%hhu\n"
                "    Dinamic: %s\n"
                "    Generic: %hhu\n"
                "    Specific: %hhu\n"
                "    Commands Count: %hhu\n",
                node_id,
                value->channel_id,
                value->dynamic ? "true" : "false",
                value->generic,
                value->specific,
                value->commands_count );
    }
}

void zn_multichannel_endpoint_find_result( znet_error_t err,
                                           znet_node_id_t node_id,
                                           const znet_multichannel_endpoint_find_report_t* value,
                                           void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "Get multichannel endpoint find fail!\n" );
    }
    else {
        if( value ) {
            printf( "Get multichannel endpointfind succ!\n"
                    "    Node ID: %hhu\n"
                    "    Generic: %hhu\n"
                    "    Specific: %hhu\n"
                    "    Channels Count: %hhu\n",
                    node_id,
                    value->generic,
                    value->specific,
                    value->channel_ids_count );
        }
        else {
            printf( "Get multichannel endpoint find succ! done.\n" );
        }
    }
}

void zn_multichannel_aggregated_members_result( znet_error_t err,
                                                znet_node_id_t node_id,
                                                const znet_multichannel_aggregated_members_report_t* value,
                                                void* arg ) {
    UNUSED( arg );
}

void zn_powerlevel( const zn_cmd_arg_t* args ) {
    const char* addr_s = zn_args_get_value_by_name( args, "get" );
    if( addr_s ) {
        // get
        uint8_t addr = 0;
        if( sscanf( addr_s, "%hhu", &addr ) < 1 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }

        znet_node_cmd_powerlevel_get( addr );
        return;
    }
    addr_s = zn_args_get_value_by_name( args, "tget" );
    if( addr_s ) {
        // test node get
        uint8_t addr = 0;
        if( sscanf( addr_s, "%hhu", &addr ) < 1 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }

        znet_node_cmd_powerlevel_test_node_get( addr );
        return;
    }
    addr_s = zn_args_get_value_by_name( args, "tset" );
    if( addr_s ) {
        // test node set
        uint8_t addr = 0;
        if( sscanf( addr_s, "%hhu", &addr ) < 1 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }
        const char* taddr_s = zn_args_get_value_by_name( args, "node" );
        if( !taddr_s ) {
            fprintf( stderr, "info: invalid node!\n" );
            return;
        }
        uint8_t taddr = 0;
        if( sscanf( taddr_s, "%hhu", &taddr ) < 1 ) {
            fprintf( stderr, "info: invalid node!\n" );
            return;
        }
        const char* level_s = zn_args_get_value_by_name( args, "level" );
        if( !level_s ) {
            fprintf( stderr, "info: invalid level!\n" );
            return;
        }
        uint8_t level = 0;
        if( sscanf( level_s, "%hhu", &level ) < 1 || level > 9 ) {
            fprintf( stderr, "info: invalid level!\n" );
            return;
        }
        const char* count_s = zn_args_get_value_by_name( args, "level" );
        if( !count_s ) {
            fprintf( stderr, "info: invalid count!\n" );
            return;
        }
        uint16_t count = 0;
        if( sscanf( count_s, "%hu", &count ) < 1 || count > 0 ) {
            fprintf( stderr, "info: invalid count!\n" );
            return;
        }

        znet_node_cmd_powerlevel_test_node_set( addr, taddr, level, count );
        return;
    }
    addr_s = zn_args_get_value_by_name( args, "set" );
    if( !addr_s ) {
        fprintf( stderr, "info: invalid command!\n" );
        return;
    }
    // set
    uint8_t addr = 0;
    if( sscanf( addr_s, "%hhu", &addr ) < 1 ) {
        fprintf( stderr, "info: invalid address!\n" );
        return;
    }
    const char* level_s = zn_args_get_value_by_name( args, "level" );
    if( !level_s ) {
        fprintf( stderr, "info: invalid level!\n" );
        return;
    }
    uint8_t level = 0;
    if( sscanf( level_s, "%hhu", &level ) < 1 || level > 9 ) {
        fprintf( stderr, "info: invalid level!\n" );
        return;
    }
    uint8_t timeout = 10;
    const char* timeout_s = zn_args_get_value_by_name( args, "timeout" );
    if( timeout_s ) {
        sscanf( timeout_s, "%hhu", &timeout );
    }

    znet_node_cmd_powerlevel_set( addr, level, timeout );
}

void zn_powerlevel_result( znet_error_t err,
                           znet_node_id_t node_id,
                           const znet_powerlevel_report_t* value,
                           void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "Get power level fail!\n" );
    }
    else {
        printf( "Get power level succ!\n"
                "    Node ID: %hhu\n"
                "    Power level: %hhu\n"
                "    Timeout: %hhu\n",
                node_id,
                value->power_level,
                value->timeout );
    }
}

void zn_powerlevel_test_node_result( znet_error_t err,
                                     znet_node_id_t node_id,
                                     const znet_powerlevel_test_node_report_t* value,
                                     void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "Get power level fail!\n" );
    }
    else {
        printf( "Get power level succ!\n"
                "    Node ID: %hhu\n"
                "    Test ID: %hhu\n"
                "    Status: %hhu\n"
                "    Acks: %hu\n",
                node_id,
                value->test_id,
                value->status,
                value->test_count_ack );
    }
}

void zn_meter( const zn_cmd_arg_t* args ) {
    const char* addr_s = zn_args_get_value_by_name( args, "get" );
    if( addr_s ) {
        // get
        uint8_t addr = 0;
        uint8_t chan = ZNET_CHANNEL_ID_ROOT;
        if( sscanf( addr_s, "%hhu:%hhu", &addr, &chan ) < 1 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }
        const char* scale_s = zn_args_get_value_by_name( args, "scale" );
        if( !scale_s ) {
            fprintf( stderr, "info: invalid scale!\n" );
            return;
        }
        uint16_t scale = 0;
        if( sscanf( scale_s, "%hu", &scale ) < 1 ) {
            fprintf( stderr, "info: invalid scale!\n" );
            return;
        }

        znet_node_cmd_meter_get( addr, chan, scale );
        return;
    }
    addr_s = zn_args_get_value_by_name( args, "supported" );
    if( addr_s ) {
        // supported
        uint8_t addr = 0;
        uint8_t chan = ZNET_CHANNEL_ID_ROOT;
        if( sscanf( addr_s, "%hhu:%hhu", &addr, &chan ) < 1 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }

        znet_node_cmd_meter_supported_get( addr, chan );
        return;
    }
    addr_s = zn_args_get_value_by_name( args, "reset" );
    if( !addr_s ) {
        fprintf( stderr, "info: invalid address!\n" );
        return;
    }
    // reset
    uint8_t addr = 0;
    uint8_t chan = ZNET_CHANNEL_ID_ROOT;
    if( sscanf( addr_s, "%hhu:%hhu", &addr, &chan ) < 1 ) {
        fprintf( stderr, "info: invalid address!\n" );
        return;
    }

    znet_node_cmd_meter_reset( addr, chan );
    return;
}

void zn_meter_result( znet_error_t err,
                      znet_node_id_t node_id,
                      znet_node_channel_id_t channel_id,
                      const znet_meter_report_t* value,
                      void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "Get meter fail!\n" );
    }
    else {
        printf( "Get meter succ!\n"
                "    Node ID: %hhu\n"
                "    Channel ID: %hhu\n"
                "    Scale: %hu\n"
                "    Value: %u\n",
                node_id,
                channel_id,
                value->scale,
                value->value );
    }
}

void zn_meter_supported_result( znet_error_t err,
                                znet_node_id_t node_id,
                                znet_node_channel_id_t channel_id,
                                const znet_meter_supported_report_t* value,
                                void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "Get meter supported fail!\n" );
    }
    else {
        printf( "Get meter supported succ!\n"
                "    Node ID: %hhu\n"
                "    Channel ID: %hhu\n"
                "    Type: %hhu\n"
                "    Reset: %s\n",
                node_id,
                channel_id,
                value->type,
                value->reset ? "true" : "false" );
        zn_print_hex( value->scales, value->scount );
    }
}

void zn_msensor( const zn_cmd_arg_t* args ) {
    const char* addr_s = zn_args_get_value_by_name( args, "supported" );
    if( addr_s ) {
        uint8_t addr = 0;
        uint8_t chan = ZNET_CHANNEL_ID_ROOT;
        if( sscanf( addr_s, "%" SCNu8 ":%" SCNu8, &addr, &chan ) < 1 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }

        znet_node_cmd_multilevel_sensor_get_supported_sensor( addr, chan );
        return;
    }

    addr_s = zn_args_get_value_by_name( args, "scale" );
    if( addr_s ) {
        uint8_t addr = 0;
        if( sscanf( addr_s, "%hhu", &addr ) < 1 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }

        const char* scale_s = zn_args_get_value_by_name( args, "type" );
        if( !scale_s ) {
            fprintf( stderr, "info: type!\n" );
            return;
        }
        uint16_t sensor_type = 0;
        if( sscanf( scale_s, "%hu", &sensor_type ) < 1 ) {
            fprintf( stderr, "info: invalid type!\n" );
            return;
        }
        uint8_t chan = ZNET_CHANNEL_ID_ROOT;
        if( sscanf( addr_s, "%hhu:%hhu", &addr, &chan ) < 1 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }

        znet_node_cmd_multilevel_sensor_get_supported_scale( addr, chan, sensor_type );
        return;
    }

    addr_s = zn_args_get_value_by_name( args, "get" );
    if( addr_s ) {
        uint8_t addr = 0;
        uint8_t chan = ZNET_CHANNEL_ID_ROOT;
        if( sscanf( addr_s, "%hhu:%hhu", &addr, &chan ) < 1 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }

        const char* type_s = zn_args_get_value_by_name( args, "type" );
        if( !type_s ) {
            fprintf( stderr, "info: type!\n" );
            return;
        }
        uint8_t sensor_type = 0;
        if( sscanf( type_s, "%" SCNu8, &sensor_type ) < 1 ) {
            fprintf( stderr, "info: invalid type!\n" );
            return;
        }

        const char* scale_s = zn_args_get_value_by_name( args, "nscale" );
        if( !scale_s ) {
            fprintf( stderr, "info: nscale!\n" );
            return;
        }
        uint8_t scale = 0;
        if( sscanf( scale_s, "%" SCNu8, &scale ) < 1 ) {
            fprintf( stderr, "info: invalid nscale!\n" );
            return;
        }
        if( scale > 3 ) {
            fprintf( stderr, "info: invalid nscale!\n" );
        }

        uint8_t version = 0;
        if( sscanf( scale_s, "%" SCNu8, &version ) < 1 ) {
            fprintf( stderr, "info: invalid nscale!\n" );
            return;
        }
        if( version < 1 || version > 11 ) {
            fprintf( stderr, "info: invalid version!\n" );
        }

        znet_node_cmd_multilevel_sensor_get( addr, chan, sensor_type, scale, version );
        return;
    }
}

static const char* nameSensors[] = { "Reserved",
                                     "Air temperature",
                                     "General purpose [DEPRECATED by V11]",
                                     "Illuminance",
                                     "Power",
                                     "Humidity",
                                     "Velocity",
                                     "Direction",
                                     "Atmospheric pressure",
                                     "Barometric pressure",
                                     "Solar radiation",
                                     "Dew point",
                                     "Rain rate",
                                     "Tide level",
                                     "Weight",
                                     "Voltage",
                                     "Current",
                                     "Carbon dioxide CO2-level",
                                     "Air flow",
                                     "Tank capacity",
                                     "Distance",
                                     "Angle position [DEPRECATED by V8]",
                                     "Rotation",
                                     "Water temperature",
                                     "Soil temperature",
                                     "Seismic Intensity",
                                     "Seismic magnitude",
                                     "Ultraviolet",
                                     "Electrical resistivity",
                                     "Electrical conductivity",
                                     "Loudness",
                                     "Moisture",
                                     "Frequency",
                                     "Time",
                                     "Target temperature",
                                     "Particulate Matter 2.5",
                                     "Formaldehyde CH2O-level",
                                     "Radon concentration",
                                     "Methane (CH4) density",
                                     "Volatile Organic Compound level",
                                     "Carbon monoxide (CO) level",
                                     "Soil humidity",
                                     "Soil reactivity",
                                     "Soil salinity",
                                     "Heart rate",
                                     "Blood pressure",
                                     "Muscle mass",
                                     "Fat mass",
                                     "Bone mass",
                                     "Total body water (TBW)",
                                     "Basis metabolic rate (BM)",
                                     "Body Mass Index (BMI)",
                                     "Acceleration X-axis",
                                     "Acceleration Y-axis",
                                     "Acceleration Z-axis",
                                     "Smoke density",
                                     "Water flow",
                                     "Water pressure",
                                     "RF signal strength",
                                     "Particulate Matter 10",
                                     "Respiratory rate",
                                     "Relative Modulation level",
                                     "Boiler water temperature",
                                     "Domestic Hot Water (DHW) temperature",
                                     "Outside temperature",
                                     "Exhaust temperature",
                                     "Water Chlorine level",
                                     "Water acidity",
                                     "Water Oxidation reduction potential",
                                     "Heart Rate LF/HF ratio" };

void zn_msensor_get_supported_sensor_result( znet_error_t err,
                                             znet_node_id_t node_id,
                                             znet_node_channel_id_t channel_id,
                                             const uint8_t* data,
                                             uint8_t len,
                                             void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "msensor supported fail, err: %i\n", err );
    }
    else {
        printf( "msensor supported succ!\n"
                "    Node ID: %hhu\n"
                "    Channel ID: %hhu\n",
                node_id,
                channel_id );

        printf( "raw data:\n" );
        zn_print_hex( data, len );
        printf( "Sensor type:\n" );

        for( uint8_t bitnum = 0; bitnum < 8 * len; bitnum++ ) {
            if( data[bitnum / 8] & 1 << bitnum % 8 ) {
                printf( "\t%u - %s\n", bitnum + 1, nameSensors[bitnum + 1] );
            }
        }
    }
}

void zn_msensor_get_supported_scale_result( znet_error_t err,
                                            znet_node_id_t node_id,
                                            znet_node_channel_id_t channel_id,
                                            uint8_t sensor_type,
                                            uint8_t scale_mask,
                                            void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "msensor scale fail, err: %i\n", err );
    }
    else {
        printf( "msensor scale succ!\n"
                "    Node ID: %hhu\n"
                "    Channel ID: %hhu\n"
                "    Sensor type: %u - %s\n"
                "    Scale mask: 0x%02X\n",
                node_id,
                channel_id,
                sensor_type,
                nameSensors[sensor_type],
                scale_mask );
    }
}

void zn_msensor_get_result( znet_error_t err,
                            znet_node_id_t node_id,
                            znet_node_channel_id_t channel_id,
                            znet_multilevel_sensor_report_t* value,
                            void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "msensor get fail!\n" );
    }
    else {
        printf( "msensor succ!\n"
                "    Node ID: %hhu\n"
                "    Channel ID: %hhu\n"
                "    Value:\n"
                "        type:\t%u - %s\n"
                "        precision:\t%u\n"
                "        scale:\t%u\n"
                "        size:\t%u\n"
                "        value:\t%.*f\n",
                node_id,
                channel_id,
                value->type,
                nameSensors[value->type],
                value->precision,
                value->scale,
                value->size,
                value->precision,
                value->value / pow( 10, value->precision ) );
    }
}

void zn_bsensor( const zn_cmd_arg_t* args ) {
    const char* addr_s = zn_args_get_value_by_name( args, "supported" );
    if( addr_s ) {
        uint8_t addr = 0;
        if( sscanf( addr_s, "%" SCNu8, &addr ) < 1 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }

        uint8_t chan = ZNET_CHANNEL_ID_ROOT;
        if( sscanf( addr_s, "%hhu:%hhu", &addr, &chan ) < 1 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }
        const uint8_t version = 2;
        znet_error_t status = znet_node_cmd_sensor_binary_supported_get( addr, chan, version );
        if( status != ZNET_ERR_OK ) {
            fprintf( stderr, "info: invalid status: %i!\n", status );
        }
        return;
    }

    addr_s = zn_args_get_value_by_name( args, "get" );
    if( addr_s ) {
        uint8_t addr = 0;
        if( sscanf( addr_s, "%" SCNu8, &addr ) < 1 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }

        const char* type_s = zn_args_get_value_by_name( args, "type" );
        if( !type_s ) {
            fprintf( stderr, "info: type!\n" );
            return;
        }
        uint8_t sensor_type = 0;
        if( sscanf( type_s, "%" SCNu8, &sensor_type ) < 1 ) {
            fprintf( stderr, "info: invalid type!\n" );
            return;
        }

        const char* version_s = zn_args_get_value_by_name( args, "version" );
        if( !version_s ) {
            fprintf( stderr, "info: version!\n" );
            return;
        }
        uint8_t version = 0;
        if( sscanf( version_s, "%" SCNu8, &version ) < 1 ) {
            fprintf( stderr, "info: invalid version!\n" );
            return;
        }

        uint8_t chan = ZNET_CHANNEL_ID_ROOT;
        if( sscanf( addr_s, "%hhu:%hhu", &addr, &chan ) < 1 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }
        znet_error_t status = znet_node_cmd_sensor_binary_get( addr, chan, sensor_type, version );
        if( status != ZNET_ERR_OK ) {
            fprintf( stderr, "info: invalid status: %i!\n", status );
        }
        return;
    }
}

static const char* nameBinarySensors[] = { "Reserved", "General purpose", "Smoke",  "CO",  "CO2",         "Heat",
                                           "Water",    "Freeze",          "Tamper", "Aux", "Door/Window", "Tilt",
                                           "Motion",   "Glass Break" };

void zn_bsensor_get_result( znet_error_t err, znet_node_id_t node_id, uint8_t value, uint8_t sensor_type, void* arg ) {
    const char* sensorTypeString =
        sensor_type <= ZNET_CMD_BINARY_SENSOR_GLASSBREAK ? nameBinarySensors[sensor_type] : "";

    printf( "bsensor get result:\n"
            "   err: %i\n"
            "   value: 0x%02X\n"
            "   sensor_type: 0x%02X %s\n",
            err,
            value,
            sensor_type,
            sensorTypeString );
}

void zn_bsensor_get_supported_sensor_result( znet_error_t err,
                                             znet_node_id_t node_id,
                                             const uint8_t* data,
                                             uint8_t len,
                                             void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "bsensor supported fail, err: %i\n", err );
    }
    else {
        printf( "bsensor supported succ!\n"
                "    Node ID: %u\n",
                node_id );

        printf( "raw data:\n" );
        zn_print_hex( data, len );
        printf( "Sensor type: \n" );

        for( uint16_t bitnum = 0; bitnum < 8 * len; bitnum++ ) {
            if( data[bitnum / 8] & 1 << bitnum % 8 ) {
                printf( "\t0x%02X ", bitnum );
                if( bitnum < sizeof( nameBinarySensors ) / sizeof( nameBinarySensors[0] ) ) {
                    printf( "%s\n", nameBinarySensors[bitnum] );
                }
                else {
                    printf( "Reserved\n" );
                }
            }
        }
    }
}

void zn_asensor( const zn_cmd_arg_t* args ) {
    const char* addr_s = zn_args_get_value_by_name( args, "supported" );
    if( addr_s ) {
        uint8_t addr = 0;
        if( sscanf( addr_s, "%" SCNu8, &addr ) < 1 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }
        uint8_t chan = ZNET_CHANNEL_ID_ROOT;
        if( sscanf( addr_s, "%hhu:%hhu", &addr, &chan ) < 1 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }
        znet_error_t status = znet_node_cmd_sensor_alarm_supported_get( addr, chan );
        if( status != ZNET_ERR_OK ) {
            fprintf( stderr, "info: invalid status: %i!\n", status );
        }
        return;
    }

    addr_s = zn_args_get_value_by_name( args, "get" );
    if( addr_s ) {
        uint8_t addr = 0;
        if( sscanf( addr_s, "%" SCNu8, &addr ) < 1 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }

        const char* type_s = zn_args_get_value_by_name( args, "type" );
        if( !type_s ) {
            fprintf( stderr, "info: type!\n" );
            return;
        }
        uint8_t sensor_type = 0;
        if( sscanf( type_s, "%" SCNu8, &sensor_type ) < 1 ) {
            fprintf( stderr, "info: invalid type!\n" );
            return;
        }

        uint8_t chan = ZNET_CHANNEL_ID_ROOT;
        if( sscanf( addr_s, "%hhu:%hhu", &addr, &chan ) < 1 ) {
            fprintf( stderr, "info: invalid address!\n" );
            return;
        }
        znet_error_t status = znet_node_cmd_sensor_alarm_get( addr, chan, sensor_type );
        if( status != ZNET_ERR_OK ) {
            fprintf( stderr, "info: invalid status: %i!\n", status );
        }
        return;
    }
}

static const char* nameAlarmSensors[] = { "General Purpose", "Smoke", "CO", "CO2", "Heat", "Water Leak" };

void zn_asensor_get_supported_sensor_result( znet_error_t err,
                                             znet_node_id_t node_id,
                                             const uint8_t* data,
                                             uint8_t len,
                                             void* arg ) {
    UNUSED( arg );
    if( err ) {
        printf( "asensor supported fail, err: %i\n", err );
    }
    else {
        printf( "asensor supported succ!\n"
                "    Node ID: %u\n",
                node_id );

        printf( "raw data:\n" );
        zn_print_hex( data, len );
        printf( "Sensor type: \n" );

        for( uint16_t bitnum = 0; bitnum < 8 * len; bitnum++ ) {
            if( data[bitnum / 8] & 1 << bitnum % 8 ) {
                printf( "\t0x%02X ", bitnum );
                if( bitnum < sizeof( nameAlarmSensors ) / sizeof( nameAlarmSensors[0] ) ) {
                    printf( "%s\n", nameAlarmSensors[bitnum] );
                }
            }
        }
    }
}

void zn_asensor_get_result( znet_error_t err,
                            znet_node_id_t node_id,
                            uint8_t sensor_type,
                            uint8_t value,
                            uint16_t seconds,
                            void* arg ) {
    const char* sensorTypeString = sensor_type <= ZNET_CMD_ALARM_SENSOR_WATERLEAK ? nameAlarmSensors[sensor_type] : "";

    printf( "asensor get result:\n"
            "   node: %u\n"
            "   err: %i\n"
            "   value: 0x%02X\n"
            "   sensor_type: 0x%02X %s\n"
            "   seconds: %u\n",
            node_id,
            err,
            value,
            sensor_type,
            sensorTypeString,
            seconds );
}

void zn_association_result( znet_error_t err,
                            znet_node_id_t node_id,
                            const znet_association_report_t* value,
                            void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "Get association fail!\n" );
    }
    else {
        printf( "Get association succ!\n"
                "    Node ID: %hhu\n"
                "    Grouping ID: %hhu\n"
                "    Max nodes supported: %hhu\n"
                "    Nodes count: %hhu\n",
                node_id,
                value->grouping_id,
                value->max_nodes_supported,
                value->node_id_count );
        zn_print_hex( value->node_ids, value->node_id_count );
    }
}

void zn_association_supported_grouping_result( znet_error_t err, znet_node_id_t node_id, uint8_t value, void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "Get association supported grouping fail!\n" );
    }
    else {
        printf( "Get association supported grouping succ!\n"
                "    Node ID: %hhu\n"
                "    Max grouping supported: %hhu\n",
                node_id,
                value );
    }
}

void zn_association_specific_group_result( znet_error_t err,
                                           znet_node_id_t node_id,
                                           znet_grouping_id_t value,
                                           void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "Get association specific group fail!\n" );
    }
    else {
        printf( "Get association specific group succ!\n"
                "    Node ID: %hhu\n"
                "    Group: %hhu\n",
                node_id,
                value );
    }
}

void zn_association_group_name_result( znet_error_t err,
                                       znet_node_id_t node_id,
                                       const znet_association_group_name_report_t* value,
                                       void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "Get association group name fail!\n" );
    }
    else {
        printf( "Get association group name succ!\n"
                "    Node ID: %hhu\n"
                "    Grouping ID: %hhu\n"
                "    Name: %s\n",
                node_id,
                value->grouping_id,
                value->name );

        zn_print_hex( value->name, value->name_size );
    }
}

void zn_association_group_info_result( znet_error_t err,
                                       znet_node_id_t node_id,
                                       const znet_association_group_info_report_t* value,
                                       void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "Get association group info fail!\n" );
    }
    else {
        printf( "Get association group info succ!\n"
                "    Node ID: %hhu\n"
                "    List mode: %s\n"
                "    Dynamic info: %s\n",
                node_id,
                value->list_mode ? "true" : "false",
                value->dynamic_info ? "true" : "false" );
        zn_print_hex(
            value->group_infos,
            value->group_info_count * sizeof( ( (znet_association_group_info_report_t*)NULL )->group_infos[0] ) );
    }
}

void zn_association_group_command_list_result( znet_error_t err,
                                               znet_node_id_t node_id,
                                               const znet_association_group_command_list_report_t* value,
                                               void* arg ) {
    UNUSED( arg );

    if( err ) {
        printf( "Get association group command list fail!\n" );
    }
    else {
        printf( "Get association group command list succ!\n"
                "    Node ID: %hhu\n"
                "    Grouping ID: %hhu\n",
                node_id,
                value->grouping_id );
        zn_print_hex(
            value->commands,
            value->commands_count * sizeof( ( (znet_association_group_command_list_report_t*)NULL )->commands[0] ) );
    }
}

void zn_quit_cli( const zn_cmd_arg_t* args ) {
    UNUSED( args );

    doit = 0;
}

void zn_help_cli( const zn_cmd_arg_t* args ) {
    UNUSED( args );

    printf( "Commands:\n" );
    for( zn_cmd_entry_t* i = zn_cmds; i && i->label; ++i ) {
        printf( "    %-10s%s\n", i->label, i->desc ?: "" );
    }
}

// cb for znet

void* zn_alloc( void* ptr, size_t size, void* arg ) {
    UNUSED( arg );

    if( size != 0 ) {
        return realloc( ptr, size );
    }

    free( ptr );
    return NULL;
}

uint64_t zn_clock( void* arg ) {
    UNUSED( arg );

    struct timespec ts;
    if( clock_gettime( CLOCK_MONOTONIC, &ts ) != 0 ) {
        return 0;
    }

    uint64_t result = (uint64_t)ts.tv_sec * 1000LL;
    result += (uint64_t)ts.tv_nsec / 1000000LL;
    ++result;

    return result;
}

void zn_log( int lvl, const char* fmt, va_list list, void* arg ) {
    UNUSED( lvl );
    UNUSED( arg );

    fprintf( stdout, "\x1b[38;2;255;199;6m" );
    vfprintf( stdout, fmt, list );
    fprintf( stdout, "\x1b[0m" );
    fflush( stdout );
}

int zn_uart_write( const void* data, size_t size, size_t* ret_size, void* arg ) {
    UNUSED( arg );

    if( ftty < 0 ) {
        fprintf( stderr, "uart write: tty not opened!" );
        return -1;
    }

    if( !data ) {
        if( !ret_size ) {
            fprintf( stderr, "uart write: invalid args!" );
            return -1;
        }

        *ret_size = 512;
        return 0;
    }

    if( !size ) {
        fprintf( stderr, "uart write: invalid args!" );
        return -1;
    }

    //    zn_print_hex( data, size );
    int writeLen = write( ftty, (const char*)data, size );
    if( writeLen < 0 ) {
        return -1;
    }

    if( ret_size ) {
        *ret_size = writeLen;
    }

    return 0;
}

int zn_uart_read( void* data, size_t size, size_t* ret_size, void* arg ) {
    UNUSED( arg );

    if( ftty < 0 ) {
        fprintf( stderr, "uart write: tty not opened!" );
        return -1;
    }

    if( !data ) {
        if( !ret_size ) {
            fprintf( stderr, "uart read: invalid args!" );
            return -1;
        }

        int val = 0;
        if( ioctl( ftty, FIONREAD, &val ) < 0 ) {
            *ret_size = 0;
            return -1;
        }
        *ret_size = val;

        return 0;
    }

    if( !size ) {
        fprintf( stderr, "uart read: invalid args!" );
        return -1;
    }

    int readLen = read( ftty, data, size );
    if( readLen < 0 ) {
        return -1;
    }
    //    zn_print_hex( data, readLen );

    if( ret_size ) {
        *ret_size = readLen;
    }

    return 0;
}

int zn_store_save( size_t offset, const void* data, size_t size, void* arg ) {
    UNUSED( arg );

    if( !data || !size ) {
        fprintf( stderr, "store save: invalid args!" );
        return -1;
    }

    if( store ) {
        // file
        FILE* f = fopen( store, "rb+" );
        if( !f ) {
            fprintf( stderr, "open store for write fail!" );
            return -1;
        }

        if( fseek( f, offset, SEEK_SET ) != 0 && ferror( f ) ) {
            fprintf( stderr, "seek store for write fail!" );
            fclose( f );
            return -1;
        }

        if( fwrite( data, size, 1, f ) != 1 ) {
            fprintf( stderr, "write block to store fail!" );
            fclose( f );
            return -1;
        }

        fclose( f );
    }
    else {
        // memory
        if( !store_dat || ( offset + size > store_dat_size ) ) {
            void* n = realloc( store_dat, offset + size );
            if( !n ) {
                fprintf( stderr, "write block to store fail: no memory!" );
                return -1;
            }

            memset( (uint8_t*)n + store_dat_size, 0, offset + size - store_dat_size );
            store_dat = n;
            store_dat_size = offset + size;
        }

        memcpy( (uint8_t*)store_dat + offset, data, size );
    }

    return 0;
}

int zn_store_load( size_t offset, void* data, size_t size, void* arg ) {
    UNUSED( arg );

    if( !data || !size ) {
        fprintf( stderr, "store load: invalid args!" );
        return -1;
    }

    if( store ) {
        // file
        FILE* f = fopen( store, "rb+" );
        if( !f ) {
            fprintf( stderr, "open store for read fail!" );
            return -1;
        }

        if( fseek( f, offset, SEEK_SET ) != 0 && ferror( f ) ) {
            fprintf( stderr, "seek store for read fail!" );
            fclose( f );
            return -1;
        }

        if( fread( data, size, 1, f ) != 1 ) {
            if( !ferror( f ) && feof( f ) ) {
                memset( data, 0, size );
                fclose( f );
                return 0;
            }

            fprintf( stderr, "read block from store fail!" );
            fclose( f );
            return -1;
        }

        fclose( f );
    }
    else {
        // memory
        if( !store_dat || ( offset + size > store_dat_size ) ) {
            memset( data, 0, size );
            return 0;
        }

        memcpy( data, (uint8_t*)store_dat + offset, size );
    }

    return 0;
}

int zn_store_reset( void* arg ) {
    UNUSED( arg );

    if( store ) {
        // file
        FILE* f = fopen( store, "wb" );
        if( !f ) {
            fprintf( stderr, "open store for reset fail!" );
            return -1;
        }
        fclose( f );
    }
    else {
        // memory
        if( store_dat ) {
            free( store_dat );
            store_dat = NULL;
            store_dat_size = 0;
        }
    }

    return 0;
}

znet_callbacks_t zn_cb = {
    .arg = NULL,
    .alloc = zn_alloc,
    .clock = zn_clock,
    .log = zn_log,
    .uart_write = zn_uart_write,
    .uart_read = zn_uart_read,
    .uart_reset = NULL,
    .store_save = zn_store_save,
    .store_load = zn_store_load,
    .store_reset = zn_store_reset,

    .set_default = zn_factory_result,
    .network_update = zn_net_update_result,
    .private_s2_dsk = NULL,
    .node_add_result = zn_add_result,
    .node_rem_result = zn_rem_result,
    .node_replace_if_failed_result = zn_rep_result,
    .node_learn_result = zn_learn_result,
    .node_list_result = zn_list_result,
    .node_update_info_result = zn_update_info_result,
    .node_s2_select_keys = zn_s2_select_keys,
    .node_s2_request_pub_key = zn_s2_enter_pub_key,
    .node_s2_show_pub_key = zn_s2_show_pub_key,

    .node_cmd_version_result = NULL,
    .node_cmd_command_version_result = NULL,
    .node_cmd_manufacturer_specific_result = NULL,
    .node_cmd_notification_result = NULL,
    .node_cmd_notification_supported_report = NULL,
    .node_cmd_notification_event_supported_report = NULL,
    .node_cmd_device_specific_result = NULL,
    .node_cmd_zwaveplus_info_result = NULL,
    .node_cmd_basic_result = zn_basic_result,
    .node_cmd_binary_switch_result = zn_binary_switch_result,
    .node_cmd_clock_result = zn_clock_device_result,
    .node_cmd_time_parameters_result = zn_time_parameters_result,
    .node_cmd_alarm_sensor_get_result = zn_asensor_get_result,
    .node_cmd_alarm_sensor_get_supported_sensor_result = zn_asensor_get_supported_sensor_result,
    .node_cmd_binary_sensor_get_result = zn_bsensor_get_result,
    .node_cmd_binary_sensor_get_supported_sensor_result = zn_bsensor_get_supported_sensor_result,
    .node_cmd_color_switch_result = zn_color_switch_result,
    .node_cmd_color_switch_supported_result = zn_color_switch_supported_result,
    //    .node_cmd_door_lock_configuration_result = zn_door_lock_configuration_result,
    .node_cmd_door_lock_operation_result = zn_door_lock_operation_result,
    .node_cmd_meter_result = zn_meter_result,
    .node_cmd_multilevel_sensor_get_supported_sensor_result = zn_msensor_get_supported_sensor_result,
    .node_cmd_multilevel_sensor_get_supported_scale_result = zn_msensor_get_supported_scale_result,
    .node_cmd_multilevel_sensor_get_result = zn_msensor_get_result,
    .node_cmd_meter_supported_result = zn_meter_supported_result,
    .node_cmd_multilevel_switch_result = NULL,
    .node_cmd_multichannel_endpoint_result = zn_multichannel_endpoint_result,
    .node_cmd_multichannel_capability_result = zn_multichannel_capability_result,
    .node_cmd_multichannel_endpoint_find_result = zn_multichannel_endpoint_find_result,
    .node_cmd_multichannel_aggregated_members_result = zn_multichannel_aggregated_members_result,
    .node_cmd_powerlevel_result = zn_powerlevel_result,
    .node_cmd_powerlevel_test_node_result = zn_powerlevel_test_node_result,
    .node_cmd_device_reset_localy_result = NULL,
    .node_cmd_security_supported_result = zn_security_supported_result,
    .node_cmd_association_result = zn_association_result,
    .node_cmd_association_supported_grouping_result = zn_association_supported_grouping_result,
    .node_cmd_association_specific_group_result = zn_association_specific_group_result,
    .node_cmd_association_group_name_result = zn_association_group_name_result,
    .node_cmd_association_group_info_result = zn_association_group_info_result,
    .node_cmd_association_group_command_list_result = zn_association_group_command_list_result
};

void print_usage( const char* program ) {
    fprintf( stderr, "Usage: %s [-s <store>] <tty>\n", program );
}

int main( int argc, char* argv[] ) {
    // program name
    const char* program = strrchr( argv[0], '/' );
    if( program ) {
        ++program;
    }
    else {
        program = argv[0];
    }

    // parse options
    int opt;
    while( ( opt = getopt( argc, argv, "s:" ) ) != -1 ) {
        switch( opt ) {
        case 's':
            store = optarg;
            break;
        default:
            print_usage( program );
            return EXIT_FAILURE;
        }
    }
    if( optind >= argc ) {
        print_usage( program );
        return EXIT_FAILURE;
    }
    tty = argv[optind];

    // init tty
    /// INFO: open tty better in non block mode
    ftty = open( tty, O_RDWR | O_NOCTTY | O_CLOEXEC | O_NONBLOCK );
    if( ftty < 0 ) {
        fprintf( stderr, "Open TTY failed [err: %d]\n", errno );
        return EXIT_FAILURE;
    }
    if( fcntl( ftty, F_SETFL, fcntl( ftty, F_GETFL ) & ~O_NONBLOCK ) < 0 ) {
        fprintf( stderr, "Set TTY to block mode failed [err: %d]\n", errno );
        close( ftty );
        ftty = -1;
        return EXIT_FAILURE;
    }
    if( !isatty( ftty ) ) {
        fprintf( stderr, "Is not a TTY [err: %d]\n", errno );
        close( ftty );
        ftty = -1;
        return EXIT_FAILURE;
    }

    struct termios tio, btio;
    if( tcgetattr( ftty, &tio ) < 0 ) {
        fprintf( stderr, "Get TTY config failed\n" );
        close( ftty );
        ftty = -1;
        return EXIT_FAILURE;
    }
    memcpy( &btio, &tio,
            sizeof( struct termios ) );  // backup current TTY config

    // 115200 8N1
    cfmakeraw( &tio );
    if( cfsetspeed( &tio, B115200 ) < 0 ) {
        fprintf( stderr, "Set TTY baud rate failed\n" );
        close( ftty );
        ftty = -1;
        return EXIT_FAILURE;
    }
    tio.c_cflag &= ~CSIZE;
    tio.c_cflag |= CS8;
    tio.c_cflag &= ~( PARENB | PARODD );
    tio.c_cflag |= 0;
    tio.c_cflag &= ~CSTOPB;
    tio.c_cflag |= 0;
    tio.c_cflag &= ~CRTSCTS;
    tio.c_cflag |= 0;
    tio.c_cc[VMIN] = 1;
    tio.c_cc[VTIME] = 0;
    if( tcsetattr( ftty, TCSAFLUSH, &tio ) < 0 ) {
        fprintf( stderr, "Set TTY attrs failed\n" );
        close( ftty );
        ftty = -1;
        return EXIT_FAILURE;
    }

    // init znet
    if( znet_init( &zn_cb ) != 0 ) {
        fprintf( stderr, "Init znet failed\n" );
        close( ftty );
        ftty = -1;
        return EXIT_FAILURE;
    }

    // init readline
    rl_callback_handler_install( "> ", (rl_vcpfunc_t*)&rl_cb );

    // set signal handlers
    struct sigaction sa;
    memset( &sa, 0, sizeof( struct sigaction ) );
    sa.sa_handler = squit;
    sigemptyset( &sa.sa_mask );
    sigaction( SIGTERM, &sa, NULL );

    struct pollfd fds[2];
    while( doit ) {
        fds[0].fd = 0;  // stdin
        fds[0].events = POLLIN;
        fds[0].revents = 0;

        fds[1].fd = ftty;  // z-wave module
        fds[1].events = POLLIN;
        fds[1].revents = 0;

        int ret = poll( fds, sizeof( fds ) / sizeof( fds[0] ), 1 );
        if( ret < 0 ) {
            if( EINTR == errno ) {
                // not error
                continue;
            }
            break;
        }
        else if( ret == 0 ) {
            znet_proc();  // call by timer

            // for quick test
            static int shoot = 1000;
            if( shoot ) {
                if( --shoot ) {
                }
                else {
                    // uint8_t nodes[] = { 1 };
                    // znet_node_cmd_association_set( 2, 2, nodes, sizeof( nodes
                    // ) / sizeof( nodes[0] ) );
                    // znet_node_cmd_association_remove( 2, 1, nodes, sizeof(
                    // nodes ) / sizeof( nodes[0] ) );
                    // znet_node_cmd_association_get( 2, 1 );
                    // znet_node_cmd_association_supported_grouping_get( 2 );
                    // znet_node_cmd_association_specific_group_get( 2 );
                    // znet_node_cmd_association_group_name_get( 2, 1 );
                    // znet_node_cmd_association_group_info_get( 2, 1, 0, 0 );
                    // znet_node_cmd_association_group_command_list_get( 2, 1, 0
                    // );
                }
            }

            if( _select_keys_do ) {
                if( --_select_keys_do ) {
                }
                else {
                    _select_keys_do = 0;
                    znet_node_s2_select_keys( _select_keys );
                }
            }
            if( _pub_key_do ) {
                if( --_pub_key_do ) {
                }
                else {
                    uint8_t key[sizeof( znet_S2_pub_key_t ) + 4] = {};
                    // test
                    key[0] = 2;
                    key[1] = 0x2F;
                    key[2] = 0xE5;
                    // key[1] = 0x86;
                    // key[2] = 0x66;
                    // key[1] = 0x2F;
                    // key[2] = 0xE5;

                    znet_node_s2_response_pub_key( (const znet_S2_pub_key_t*)key );
                }
            }
        }
        else if( ret > 0 ) {
            if( fds[0].revents & POLLIN ) {
                rl_callback_read_char();
            }
            if( fds[1].revents & POLLIN ) {
                znet_proc();  // call by data on tty
            }
        }
    }

    // deinit readline
    rl_callback_handler_remove();

    // deinit znet

    // deinit tty
    tcsetattr( ftty, TCSAFLUSH, &btio );
    close( ftty );
    ftty = -1;

    // store in memory
    if( store_dat ) {
        // need save store_dat to file as dump?
        free( store_dat );
        store_dat = NULL;
    }

    return EXIT_SUCCESS;
}
