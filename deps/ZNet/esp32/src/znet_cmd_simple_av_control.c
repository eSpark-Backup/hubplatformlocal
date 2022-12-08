#include "znet/znet.h"
#include "znet_main.h"
#include "znet_log.h"
#include "znet_lib_cc_application.h"

#include <assert.h>

// node cmd simple AV control set report /////////////////////
/// INFO: Simple AV control v1..v4

void znet_cc_simple_av_set_report( const ZFunction func, uint8_t node_id, int cc_data_len, const uint8_t* cc_data ) {
    assert( cc_data );
    assert( cc_data_len >= 6 );
    assert( cc_data[0] == ZNET_COMMAND_CLASS_SIMPLE_AV_CONTROL );
    assert( cc_data[1] == SIMPLE_AV_CONTROL_SET );
    //    assert( cc_data[3] + 4 <= cc_data_len );

    const size_t commands_count = ( cc_data_len - 6 ) / 2;

    const size_t buff_size = sizeof( znet_simple_av_set_report_t ) + ( sizeof( uint16_t ) * commands_count );
    uint8_t buff[buff_size];
    memset( buff, 0, buff_size );

    znet_simple_av_set_report_t* report = (znet_simple_av_set_report_t*)&buff;
    report->sequence_id = cc_data[2];
    report->key_attribute = cc_data[3] & 0x07;
    report->commands_count = commands_count;

    for( size_t i = 0; i < commands_count; ++i ) {
        uint8_t msb = cc_data[6 + ( i * 2 )];
        uint8_t lsb = cc_data[6 + ( i * 2 ) + 1];

        report->commands[i] = ( uint16_t )( ( lsb & 0xFF ) | ( msb << 8 ) );
    }

    if( znet_cb->node_cmd_simple_av_control_set_result )
        znet_cb->node_cmd_simple_av_control_set_result( 0, node_id, func->_endpoint, report, znet_cb->arg );
}
