/* © 2017 Sigma Designs, Inc. This is an unpublished work protected by Sigma
 * Designs, Inc. as a trade secret, and is not to be used or disclosed except as
 * provided Z-Wave Controller Development Kit Limited License Agreement. All
 * rights reserved.
 *
 * Notice: All information contained herein is confidential and/or proprietary
 * to Sigma Designs and may be covered by U.S. and Foreign Patents, patents in
 * process, and are protected by trade secret or copyright law. Dissemination or
 * reproduction of the source code contained herein is expressly forbidden to
 * anyone except Licensees of Sigma Designs  who have executed a Sigma Designs’
 * Z-WAVE CONTROLLER DEVELOPMENT KIT LIMITED LICENSE AGREEMENT. The copyright
 * notice above is not evidence of any actual or intended publication of the
 * source code. THE RECEIPT OR POSSESSION OF  THIS SOURCE CODE AND/OR RELATED
 * INFORMATION DOES NOT CONVEY OR IMPLY ANY RIGHTS  TO REPRODUCE, DISCLOSE OR
 * DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE, USE, OR SELL A PRODUCT THAT IT
 * MAY DESCRIBE.
 *
 * THE SIGMA PROGRAM AND ANY RELATED DOCUMENTATION OR TOOLS IS PROVIDED TO
 * COMPANY "AS IS" AND "WITH ALL FAULTS", WITHOUT WARRANTY OF ANY KIND FROM
 * SIGMA. COMPANY ASSUMES ALL RISKS THAT LICENSED MATERIALS ARE SUITABLE OR
 * ACCURATE FOR COMPANY’S NEEDS AND COMPANY’S USE OF THE SIGMA PROGRAM IS AT
 * COMPANY’S OWN DISCRETION AND RISK. SIGMA DOES NOT GUARANTEE THAT THE USE OF
 * THE SIGMA PROGRAM IN A THIRD PARTY SERVICE ENVIRONMENT OR CLOUD SERVICES
 * ENVIRONMENT WILL BE: (A) PERFORMED ERROR-FREE OR UNINTERRUPTED; (B) THAT
 * SIGMA WILL CORRECT ANY THIRD PARTY SERVICE ENVIRONMENT OR CLOUD SERVICE
 * ENVIRONMENT ERRORS; (C) THE THIRD PARTY SERVICE ENVIRONMENT OR CLOUD SERVICE
 * ENVIRONMENT WILL OPERATE IN COMBINATION WITH COMPANY’S CONTENT OR COMPANY
 * APPLICATIONS THAT UTILIZE THE SIGMA PROGRAM; (D) OR WITH ANY OTHER HARDWARE,
 * SOFTWARE, SYSTEMS, SERVICES OR DATA NOT PROVIDED BY SIGMA. COMPANY
 * ACKNOWLEDGES THAT SIGMA DOES NOT CONTROL THE TRANSFER OF DATA OVER
 * COMMUNICATIONS FACILITIES, INCLUDING THE INTERNET, AND THAT THE SERVICES MAY
 * BE SUBJECT TO LIMITATIONS, DELAYS, AND OTHER PROBLEMS INHERENT IN THE USE OF
 * SUCH COMMUNICATIONS FACILITIES. SIGMA IS NOT RESPONSIBLE FOR ANY DELAYS,
 * DELIVERY FAILURES, OR OTHER DAMAGE RESULTING FROM SUCH ISSUES. SIGMA IS NOT
 * RESPONSIBLE FOR ANY ISSUES RELATED TO THE PERFORMANCE, OPERATION OR SECURITY
 * OF THE THIRD PARTY SERVICE ENVIRONMENT OR CLOUD SERVICES ENVIRONMENT THAT
 * ARISE FROM COMPANY CONTENT, COMPANY APPLICATIONS OR THIRD PARTY CONTENT.
 * SIGMA DOES NOT MAKE ANY REPRESENTATION OR WARRANTY REGARDING THE RELIABILITY,
 * ACCURACY, COMPLETENESS, CORRECTNESS, OR USEFULNESS OF THIRD PARTY CONTENT OR
 * SERVICE OR THE SIGMA PROGRAM, AND DISCLAIMS ALL LIABILITIES ARISING FROM OR
 * RELATED TO THE SIGMA PROGRAM OR THIRD PARTY CONTENT OR SERVICES. TO THE
 * EXTENT NOT PROHIBITED BY LAW, THESE WARRANTIES ARE EXCLUSIVE. SIGMA OFFERS NO
 * WARRANTY OF NON-INFRINGEMENT, TITLE, OR QUIET ENJOYMENT. NEITHER SIGMA NOR
 * ITS SUPPLIERS OR LICENSORS SHALL BE LIABLE FOR ANY INDIRECT, SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES OR LOSS (INCLUDING DAMAGES FOR LOSS OF
 * BUSINESS, LOSS OF PROFITS, OR THE LIKE), ARISING OUT OF THIS AGREEMENT
 * WHETHER BASED ON BREACH OF CONTRACT, INTELLECTUAL PROPERTY INFRINGEMENT, TORT
 * (INCLUDING NEGLIGENCE), STRICT LIABILITY, PRODUCT LIABILITY OR OTHERWISE,
 * EVEN IF SIGMA OR ITS REPRESENTATIVES HAVE BEEN ADVISED OF OR OTHERWISE SHOULD
 * KNOW ABOUT THE POSSIBILITY OF SUCH DAMAGES. THERE ARE NO OTHER EXPRESS OR
 * IMPLIED WARRANTIES OR CONDITIONS INCLUDING FOR SOFTWARE, HARDWARE, SYSTEMS,
 * NETWORKS OR ENVIRONMENTS OR FOR MERCHANTABILITY, NONINFRINGEMENT,
 * SATISFACTORY QUALITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * The Sigma Program  is not fault-tolerant and is not designed, manufactured or
 * intended for use or resale as on-line control equipment in hazardous
 * environments requiring fail-safe performance, such as in the operation of
 * nuclear facilities, aircraft navigation or communication systems, air traffic
 * of the Sigma Program, or Company Applications created using the Sigma
 * Program, could lead directly to death, personal injury, or severe physical or
 * environmental damage ("High Risk Activities").  Sigma and its suppliers
 * specifically disclaim any express or implied warranty of fitness for High
 * Risk Activities.Without limiting Sigma’s obligation of confidentiality as
 * further described in the Z-Wave Controller Development Kit Limited License
 * Agreement, Sigma has no obligation to establish and maintain a data privacy
 * and information security program with regard to Company’s use of any Third
 * Party Service Environment or Cloud Service Environment. For the avoidance of
 * doubt, Sigma shall not be responsible for physical, technical, security,
 * administrative, and/or organizational safeguards that are designed to ensure
 * the security and confidentiality of the Company Content or Company
 * Application in any Third Party Service Environment or Cloud Service
 * Environment that Company chooses to utilize.
 */
/*
 * S2_wrap.c
 *
 *  Created on: Aug 25, 2015
 *      Author: aes
 */

#define DllExport
#undef BYTE
#undef UNUSED
#define x86

//#include <ZW_transport_api.h>
#include <misc.h>
#if( defined( ESP_PLATFORM ) )
#include <esp_system.h>
#endif  // ( defined( ESP_PLATFORM ) )
#include <ZW_transport_api.h>
//#include <serialapi.h>
//#include <prompt.h>
#include <ZW_classcmd.h>
#include <s2_keystore.h>
//#include <s2_event.h>
//#include <transport_service2_external.h>
#include "S2.h"
#include "s2_inclusion.h"
#include "s2_classcmd.h"
#include "ZW_SendDataAppl.h"  //JIA
#include "S2_wrap.h"
#include "curve25519.h"
#include "ZW_typedefs.h"

#include "znet_lib_fc.h"
#include "znet_lib_cc.h"

#include "logger.h"
#include "system_time.h"
#include "security_layer.h"
#include "s2_protocol.h"
#include "SendData.h"

extern zip_nvm_config_t zip_nvm_config;
//#include "utils_clock.h"

#define MAG_KEY 0x55aa55aa

#define ECDH_PRIVATE_KEY_NVM_OFFSET 0x43
#define ECDH_KEY_NVM_SIZE 32

uint8_t MY_NODE_ID = 1;
uint32_t DEVICE_HOME_ID = 3642800362;  // D9 20 B8 EA
// uint32_t DEVICE_HOME_ID = 3776657338;   //E1 1B 37 BA
// uint32_t DEVICE_HOME_ID = 286331156;   //11 11 11 14

#define S2_NUM_KEY_CLASSES 4 /* Includes the S0 key */

// extern uint8_t send_data(ts_param_t *p, const uint8_t * data, uint16_t len,
// ZW_SendDataAppl_Callback_t cb, void *user);

// extern void print_hex(uint8_t *buf, int len);

extern ZNet_t znet_instance;

// extern serialapi_device_t device; // Is this the right way to do this here?

struct S2* s2_ctx = NULL;
s2_connection_t s2_conn;

uint8_t* transfer_data = NULL;
uint8_t transfer_data_length = 0;

static ZW_SendDataAppl_Callback_t s2_send_callback;
static sec2_inclusion_cb_t sec_incl_cb;

static void* s2_send_user;
uint8_t s2_timer;
uint8_t s2_inclusion_timer;
SystemTime_t transmit_start_time;
uint8_t start_incl_timer;
uint8_t sec2_learn_mode = FALSE;
extern uint8_t learn_mode;
extern uint8_t SIS_id;

// extern s2_data_t s2_data;
uint8_t req_key;

ts_param_t p;

uint8_t s2_report_a_timer;
uint8_t pub_resp[4];

/* clang-format off */
uint8_t SecureClasses[] = {
    COMMAND_CLASS_ASSOCIATION_GRP_INFO_V3,
    COMMAND_CLASS_ASSOCIATION,
    COMMAND_CLASS_VERSION,
    COMMAND_CLASS_MANUFACTURER_SPECIFIC_V2,
    COMMAND_CLASS_DEVICE_RESET_LOCALLY,
    COMMAND_CLASS_POWERLEVEL,
    COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V3,
    COMMAND_CLASS_INDICATOR,
    COMMAND_CLASS_NODE_PROVISIONING,
    COMMAND_CLASS_FIRMWARE_UPDATE_MD,
};
/* clang-format on */
uint8_t nSecureClasses = sizeof( SecureClasses );
// uint8_t learn_mode_security = 0;

static uint8_t keystore_flags_2_node_flags( uint8_t key_store_flags ) {
    uint8_t flags = 0;
    if( key_store_flags & KEY_CLASS_S0 ) {
        flags |= NODE_FLAG_SECURITY0;
    }
    if( key_store_flags & KEY_CLASS_S2_ACCESS ) {
        flags |= NODE_FLAG_SECURITY2_ACCESS;
    }
    if( key_store_flags & KEY_CLASS_S2_AUTHENTICATED ) {
        flags |= NODE_FLAG_SECURITY2_AUTHENTICATED;
    }
    if( key_store_flags & KEY_CLASS_S2_UNAUTHENTICATED ) {
        flags |= NODE_FLAG_SECURITY2_UNAUTHENTICATED;
    }
    return flags;
}

// uint8_t response[2] = {0x93, 0xDE}; //37854 translated to two hex bytes DOOR
// LOCK
uint8_t response[4] = { 0xB0, 0x4F, 0x00, 0x00 };  // 45135 translated to two hex bytes Z-Link

static void sec2_event_handler( zwave_event_t* ev ) {
    uint8_t flags;
    uint8_t sec_keys;

    /* FIXME Using hardcoded DSK being used for S2 Access class.
     *It is known that the PC Controller always has 34080 as the SDK*/
    //    uint8_t response[2] = {0x85, 0x20}; //34080 translated to two hex
    //    bytes PC Controller
    //  uint8_t response[2] = {0xA0, 0x86}; //41094 translated to two hex bytes
    //  prompt_printf("S2 inclusion event %d\n", ev->event_type);

    switch( ev->event_type ) {
    case S2_NODE_INCLUSION_INITIATED_EVENT:
        //      sec0_abort_inclusion(); //Here we abort S0 inclusion
        break;

    case S2_NODE_INCLUSION_PUBLIC_KEY_CHALLENGE_EVENT:
        //     s2_inclusion_challenge_response(s2_ctx, TRUE, response);
        Log_Write( "S2_NODE_INCLUSION_PUBLIC_KEY_CHALLENGE_EVENT \r\n" );

        if( learn_mode && ( MY_NODE_ID != SIS_id ) /* &&
            ( CC_present_in_NIF_by_ID( SIS_id, COMMAND_CLASS_INCLUSION_CONTROLLER ) || ( SIS_id == 0 ) )*/ ) {
            sec_keys = ev->evt.s2_event.s2_data.kex_report.security_keys;
            sec_keys = sec_keys | ( ev->evt.s2_event.s2_data.kex_report.csa ? 0x40 : 0x00 );
            uint8_t public_key[32];
            keystore_public_key_read(
                public_key, ( sec_keys & ( SECURITY_2_SECURITY_2_CLASS_1 | SECURITY_2_SECURITY_2_CLASS_2 ) ) == 0 );
            S2_request_incl( STATE_SHOW_PUB_KEY, sec_keys, ev->evt.s2_event.peer.r_node, public_key );
            s2_inclusion_challenge_response( s2_ctx, TRUE, response );
            break;
        }
        sec_keys = ev->evt.s2_event.s2_data.challenge_req.granted_keys;
        if( ev->evt.s2_event.s2_data.challenge_req.csa == INCLUSION_MODE_CSA )
            sec_keys |= 0x40;
        S2_request_incl( STATE_PUB_KEY_REQ,
                         sec_keys,
                         ev->evt.s2_event.peer.r_node,
                         (uint8_t*)ev->evt.s2_event.s2_data.challenge_req.public_key );
        inclusion_dsk_challenge = 1;
        break;

    case S2_NODE_INCLUSION_KEX_REPORT_EVENT:
        //      prompt_printf("csa %d keys %d\n",
        //      ev->evt.s2_event.s2_data.kex_report.csa,
        //                    ev->evt.s2_event.s2_data.kex_report.security_keys);
        /* TODO Here I am just granting all the requested keys from the KEX
         * REPORT frame normally, controller application can check and decide
         * which keys to grant*/

        //      sec2_key_grant(1, req_key, false);

        sec_keys = ev->evt.s2_event.s2_data.kex_report.security_keys & SECURITY_2_KEY_MASK;
        sec_keys = sec_keys | ( ev->evt.s2_event.s2_data.kex_report.csa ? 0x40 : 0x00 );
        S2_request_incl( STATE_KEYS_REQ, sec_keys, ev->evt.s2_event.peer.r_node, NULL );

        break;

    case S2_NODE_INCLUSION_COMPLETE_EVENT:
        if( sec_incl_cb ) {
            sec_incl_cb( ZNET_ERR_OK,
                         s2_ctx->inclusion_peer.r_node,
                         keystore_flags_2_node_flags( ev->evt.s2_event.s2_data.inclusion_complete.exchanged_keys ) );
        }
        sec2_learn_mode = false;
        //< Security 2 event, inclusion of the node  to the network has been
        // completed.
        break;

    case S2_NODE_JOINING_COMPLETE_EVENT:
        nvm_config_get( assigned_keys, &flags );
        store_network_keys();
        if( ( flags & ( ~KEY_CLASS_S0 ) ) == 0 ) {
            security_init();
        }
        if( sec_incl_cb ) {
            sec_incl_cb( ZNET_ERR_OK, s2_ctx->inclusion_peer.r_node, keystore_flags_2_node_flags( flags ) );
        }
        sec2_learn_mode = false;
        break;

    case S2_NODE_INCLUSION_FAILED_EVENT:
        flags = 0;
        if( sec2_learn_mode )
            nvm_config_set( assigned_keys, &flags );
        store_network_keys();
        if( sec_incl_cb ) {
            sec_incl_cb( ZNET_ERR_FAULT, s2_ctx->inclusion_peer.r_node, 0 );
            /*
                      sec_incl_cb( NODE_FLAG_KNOWN_BAD |
                                   (
               ev->evt.s2_event.s2_data.inclusion_fail.kex_fail_type
                                     << 16 ) );
            */
        }
        sec2_learn_mode = false;
        //< Security 2 event, inclusion of the node  to the network has failed.
        // See the details
        break;
    }
}

void sec2_key_grant( uint8_t accept, uint8_t keys, uint8_t csa ) {
    Log_Write( "sec2_key_grant  accept=%d, keys=%d, csa=%d\r\n", accept, keys, csa );
    sec2_create_new_dynamic_ecdh_key();
    s2_inclusion_key_grant( s2_ctx, accept, keys, csa );
}

void sec2_dsk_accept( uint8_t accept, const uint8_t* dsk ) {
    Log_Write( "sec2_dsk_accept  accept=%d\r\n", accept );
    print_hex_buf( dsk, 4 );
    s2_inclusion_challenge_response( s2_ctx, accept, dsk );
}
// security_scheme_t net_scheme;  // JIA

void S2_get_commands_supported( uint8_t lnode, uint8_t class_id, uint8_t** cmdClasses, uint8_t* length ) {
    uint8_t keys;
    /*Only answer to ACCESS class*/

    nvm_config_get( assigned_keys, &keys );
    Log_Write( "S2_get_commands_supported  assigned_keys = %d, class_id = %d, "
               "lnode = %d\r\n",
               keys,
               class_id,
               lnode );
    if( ( ( keys & SECURITY_2_SECURITY_2_CLASS_2 ) && class_id == 2 ) ||
        ( ( ( keys & SECURITY_2_SECURITY_2_CLASS_2 ) == 0 ) && class_id == 1 ) ||
        ( ( ( keys & ( SECURITY_2_SECURITY_2_CLASS_2 | SECURITY_2_SECURITY_2_CLASS_1 ) ) == 0 ) && class_id == 0 ) ) {
        if( lnode == MY_NODE_ID ) {
            *cmdClasses = &SecureClasses[0];
            *length = nSecureClasses;
            return;
        }
    }

    *length = 0;
}

void ICACHE_FLASH_ATTR S2_timeout( void* arg );
void incl_timeout( void* ctxt );

void create_new_network_keys() {
    security_set_default();
    sec2_create_new_static_ecdh_key();
    sec2_create_new_network_keys();
}

void sec2_init( uint32_t home_id ) {
    Log_Write( "sec2_init\r\n" );
    //  static uint8_t s2_cmd_class_sup_report[64];
    // TODO JIA - initialize PRNG here?
    if( transfer_data == NULL ) {
        transfer_data =
            Heap_Alloc( WORKBUF_SIZE + 10 + 8 + 4 );  // similar with ctxt->workbuf[WORKBUF_SIZE + 10 + 8 + 4];
    }
    if( s2_ctx ) {
        S2_destroy( &s2_ctx );
        znet_timer_deinit( &s2_timer );
        znet_timer_deinit( &s2_inclusion_timer );
    }

    extern uint8_t ICACHE_FLASH_ATTR read_fl( int dis, char* str, int len );
    read_fl( 3 * 4096, (char*)&zip_nvm_config, sizeof( zip_nvm_config_t ) );
    if( zip_nvm_config.magic != MAG_KEY ) {
        create_new_network_keys();
    }

    s2_ctx = S2_init_ctx( home_id );  // initialize the context with the controllers home ID

    s2_inclusion_init( SECURITY_2_SCHEME_1_SUPPORT,
                       KEX_REPORT_CURVE_25519,
                       SECURITY_2_SECURITY_2_CLASS_0 | SECURITY_2_SECURITY_2_CLASS_1 | SECURITY_2_SECURITY_2_CLASS_2 |
                           SECURITY_2_SECURITY_0_NETWORK_KEY );

    //  s2_ctx = S2_init_ctx(home_id); //initialize the context with the
    //  controllers home ID

    s2_inclusion_set_event_handler( &sec2_event_handler );  // initialize the S2
                                                            // security handler,
                                                            // implemented in
                                                            // s2_wrap.c.
    sec2_create_new_dynamic_ecdh_key();

    s2_conn.class_id = 0;

    znet_timer_stop( &s2_timer );
    znet_timer_init( &s2_timer, S2_timeout, s2_ctx );

    znet_timer_stop( &s2_inclusion_timer );
    znet_timer_init( &s2_inclusion_timer, incl_timeout, s2_ctx );

    inclusion_dsk_challenge = 0;
}

void sec2_deinit( void ) {
    if( s2_ctx ) {
        Log_Write( "sec2_deinit\r\n" );
        S2_destroy( &s2_ctx );
        znet_timer_deinit( &s2_timer );
        znet_timer_deinit( &s2_inclusion_timer );
        if( transfer_data ) {
            Heap_Free( transfer_data );
            transfer_data = 0;
        }
    }
}

//
// uint8_t ecdh_dynamic_key[32] =
// {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};
uint8_t ecdh_dynamic_key[32];
//

static void sec2_on_ecdh_static_private_key( ZFunction func,
                                             void* arg,
                                             ZFuncFailures_e reason,
                                             const void* dataPtr,
                                             size_t dataSize ) {
    nvm_config_set( ecdh_priv_key, dataPtr );
}

void sec2_create_new_static_ecdh_key() {
    // sec2_create_new_dynamic_ecdh_key();
    // nvm_config_set( ecdh_priv_key, ecdh_static_private_key );
    // sec2_create_new_dynamic_ecdh_key();

    znet_fc_nvr_get_value( ECDH_PRIVATE_KEY_NVM_OFFSET, ECDH_KEY_NVM_SIZE, sec2_on_ecdh_static_private_key, NULL );
}

void sec2_create_new_dynamic_ecdh_key() {
    AES_CTR_DRBG_Generate( &s2_ctr_drbg, ecdh_dynamic_key );
    AES_CTR_DRBG_Generate( &s2_ctr_drbg, &ecdh_dynamic_key[16] );
}

uint8_t sec2_get_my_node_flags() {
    uint8_t flags;
    nvm_config_get( assigned_keys, &flags );
    return keystore_flags_2_node_flags( flags );
}

void store_network_keys() {
    extern void ICACHE_FLASH_ATTR save_fl( int dis, char* str, uint32_t len );
    uint8_t net_key[16];
    int c;

    for( c = 0; c < S2_NUM_KEY_CLASSES - 1; c++ ) {
        keystore_network_key_read( 1 << c, net_key );
    }
    keystore_network_key_read( KEY_CLASS_S0, net_key );

    zip_nvm_config.magic = MAG_KEY;
    save_fl( 3 * 4096, (char*)&zip_nvm_config, sizeof( zip_nvm_config_t ) );
}

void sec2_create_new_network_keys() {
    uint8_t net_key[16];
    int c;
    //  LOG_PRINTF("Creating new network keys\n");
    for( c = 0; c < S2_NUM_KEY_CLASSES - 1; c++ ) {
        S2_get_hw_random( net_key, 16 );
        //    print_hex(net_key,16);
        keystore_network_key_write( 1 << c, net_key );
    }
    store_network_keys();
    //  memset(net_key,0, sizeof(net_key));
}

void sec2_start_learn_mode( uint8_t node_id ) {
    Log_Write( "sec2_start_learn_mode\r\n" );
    s2_connection_t s2_con;
    // ZW_SendDataAppl_Callback_t cb_save;

    /*We update the context here becase the homeID has changed */
    sec2_init( DEVICE_HOME_ID );

    s2_con.l_node = MY_NODE_ID;
    s2_con.r_node = node_id;
    s2_con.zw_tx_options = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
    sec2_learn_mode = TRUE;

    s2_inclusion_joining_start( s2_ctx, &s2_con, 0 );
}

void sec2_start_add_node( uint8_t node_id, sec2_inclusion_cb_t cb ) {
    Log_Write( "starting sec2_add_node\n" );
    s2_connection_t s2_con;

    // ZW_SendDataAppl_Callback_t cb_save;

    s2_con.l_node = MY_NODE_ID;  // JIA
    s2_con.r_node = node_id;
    s2_con.zw_tx_options = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;

    sec_incl_cb = cb;
    s2_inclusion_including_start( s2_ctx, &s2_con );
}

znet_error_t sec2_send_data( ts_param_t* p,
                             const uint8_t* data2,
                             uint16_t len,
                             ZW_SendDataAppl_Callback_t callback,
                             void* user ) {
    s2_connection_t s2_con;

    ZW_SendDataAppl_Callback_t cb_save;
    void* user_save;

    s2_con.l_node = p->snode;
    s2_con.r_node = p->dnode;
    s2_con.zw_tx_options = p->tx_flags;
    s2_con.tx_options = 0;
    /* This enables mailbox to set S2_TXOPTION_VERIFY_DELIVERY which waits for
      the sleeping ndoe to send SOS/resync if needed. It puts delay of 500ms,
      but only for first frame popped out of Mailbox after the node is woken
      up*/
    //  if(mb_is_busy() || CommandAnalyzerIsGet(data[0],data[1]))
    //  {
    //     s2_con.tx_options |= S2_TXOPTION_VERIFY_DELIVERY;
    //  }
    transfer_data_length = len;
    memcpy( transfer_data, data2, transfer_data_length );
    s2_con.class_id = p->scheme - SECURITY_SCHEME_2_UNAUTHENTICATED;
    //    Log_Write( "sec2_send_data   s2_con.class_id  %d \r\n",
    //    s2_con.class_id );

    cb_save = s2_send_callback;
    user_save = s2_send_user;

    s2_send_callback = callback;
    s2_send_user = user;

    if( S2_send_data( s2_ctx, &s2_con, transfer_data, len ) ) {
        return ZNET_ERR_OK;
    }
    else {
        s2_send_callback = cb_save;
        s2_send_user = user_save;
        return ZNET_ERR_COMM;
    }
}

/**
 * Emitted when the security engine is done.
 * Note that this is also emitted when the security layer sends a
 * SECURE_COMMANDS_SUPPORTED_REPORT
 */
void S2_send_done_event( struct S2* ctxt, s2_tx_status_t status ) {
    const uint8_t s2zw_codes[] = {
        TRANSMIT_COMPLETE_OK, TRANSMIT_COMPLETE_OK, TRANSMIT_COMPLETE_NO_ACK, TRANSMIT_COMPLETE_FAIL
    };
    Log_Write( "S2_send_done_event learn_mode=%d\r\n", sec2_learn_mode );
    if( s2_send_callback && !sec2_learn_mode && s2_send_user ) {
        ZFuncFailures_e reason;
        switch( s2zw_codes[status] ) {
        case TRANSMIT_COMPLETE_OK:
            reason = FUNC_OK;
            break;
        case TRANSMIT_COMPLETE_NO_ACK:
            reason = FUNC_FAILURE_NO_ACK;
            break;
        default:
            reason = FUNC_FAILURE_GENERAL;
            break;
        }
        s2_send_callback( reason, &s2_send_user );
    }
}

/**
 * Emitted when a messages has been received and decrypted
 */
void S2_msg_received_event( struct S2* ctxt, s2_connection_t* src, uint8_t* buf, uint16_t len ) {
    UNUSED( ctxt );

    Log_Write( "S2_msg_received_event class %d, node %d\r\n", src->class_id, src->r_node );
    print_hex_buf( buf, len );

    encap_type_t security_type = Encapsulation_None;
    switch( src->class_id + 1 ) {
    case SECURITY_SCHEME_2_UNAUTHENTICATED:
        security_type = Encapsulation_Sec_UNAUTHENTICATED;
        break;
    case SECURITY_SCHEME_2_AUTHENTICATED:
        security_type = Encapsulation_Sec_AUTHENTICATED;
        break;
    case SECURITY_SCHEME_2_ACCESS:
        security_type = Encapsulation_Sec_2;
        break;
    default:
        Log_Write( "S2 security level is wrong \n" );
        break;
    }

    ZNet_ApplicationCommandHandler( buf, len, src->r_node, security_type, 0 );
}

// This is the time it takes a z-wave node to do 3*S2 frame decryption +
// generating a nonce report #define NONCE_REP_TIME 50 #define NONCE_REP_TIME
// 250
#define NONCE_REP_TIME 1000
// static void S2_send_frame_callback(BYTE txStatus,void* user) {
//  S2_send_frame_done_notify((struct S2*) user,
//      txStatus == TRANSMIT_COMPLETE_OK ? S2_TRANSMIT_COMPLETE_OK :
//      S2_TRANSMIT_COMPLETE_NO_ACK,
//          clock()-transmit_start_time + NONCE_REP_TIME);
//}

struct S2* pointer_S2;

void S2_send_frame_Callback( ZFunction this, void* arg, ZFuncFailures_e reason, const void* dataPtr, size_t dataSize ) {
    if( FUNC_OK == reason ) {
        UNUSED( this );
        UNUSED( arg );
        Log_Write( "S2_send_frame_OnSuccess\n" );

        S2_send_frame_done_notify( pointer_S2,
                                   S2_TRANSMIT_COMPLETE_OK,
                                   SystemTime_Decode_ms( SystemTime_Now() ) - transmit_start_time + NONCE_REP_TIME );
    }
    else {
        UNUSED( this );
        UNUSED( arg );
        UNUSED( reason );
        Log_Write( "S2_send_frame_Callback\n" );

        S2_send_frame_done_notify( pointer_S2,
                                   S2_TRANSMIT_COMPLETE_NO_ACK,
                                   SystemTime_Decode_ms( SystemTime_Now() ) - transmit_start_time + NONCE_REP_TIME );
    }
}

/** Must be implemented elsewhere maps to ZW_SendData or ZW_SendDataBridge note
 * that ctxt is passed as a handle. The ctxt MUST be provided when the \ref
 * S2_send_frame_done_notify is called */
znet_error_t S2_send_frame( struct S2* ctxt, const s2_connection_t* conn, uint8_t* buf, uint16_t len ) {
    ts_param_t p;
    ts_set_std( &p, 0 );

    //    Log_Write( "S2_SEND_FRAME BEING CALLED\n" );
    pointer_S2 = ctxt;

    p.snode = conn->l_node;
    p.dnode = conn->r_node;
    p.tx_flags = conn->zw_tx_options;
    transmit_start_time = SystemTime_Decode_ms( SystemTime_Now() );

    /// TODO: check channel support
    return znet_fc_send_data( (uint8_t)conn->r_node, 0, len, buf, S2_send_frame_Callback, NULL, Encapsulation_None );
}
/**
 * TODO
 */
znet_error_t S2_send_frame_multi( struct S2* ctxt, s2_connection_t* conn, uint8_t* buf, uint16_t len ) {
    UNUSED( ctxt );
    UNUSED( conn );
    UNUSED( buf );
    UNUSED( len );
    return ZNET_ERR_OK;
}

// uint8_t t_inf_nod;

void ICACHE_FLASH_ATTR S2_timeout( void* arg ) {
    Log_Write( "S2_timeout_notify\n" );
    S2_timeout_notify( (struct S2*)arg );
}

/**
 * Must be implemented elsewhere maps to ZW_TimerStart. Note that this must
 * start the same timer every time. Ie. two calls to this function must reset
 * the timer to a new value. On timout \ref S2_timeout_event must be called.
 *
 */
void S2_set_timeout( struct S2* ctxt, uint32_t interval ) {
    UNUSED( ctxt );
    Log_Write( "S2_set_timeout interval =%d ms\n", interval );
    znet_timer_set( &s2_timer, interval );
}

void incl_timeout( void* ctxt ) {
    ZNET_LOGW( "s2_inclusion_notify_timeout\n" );
    if( start_incl_timer ) {
        sec2_abort_join();
        if( sec_incl_cb ) {
            sec_incl_cb( ZNET_ERR_TIME, s2_ctx->inclusion_peer.r_node, 0 );
        }
        start_incl_timer = false;
    }
    else {
        s2_inclusion_notify_timeout( (struct S2*)ctxt );
    }
}

uint8_t s2_inclusion_set_timeout( struct S2* ctxt, uint32_t interval ) {
    UNUSED( ctxt );
    //  DBG_PRINTF("s2_inclusion_set_timeout interval =%d ms\n",interval * 10);
    Log_Write( "s2_inclusion_set_timeout %d\r\n", interval * 10 );
    znet_timer_set( &s2_inclusion_timer, interval * 10 );
    return 0;
}

/**
 * Get a number of bytes from a hardware random number generator
 */
void S2_get_hw_random( uint8_t* buf, uint8_t len ) {
#if( defined( _WINDOWS ) || defined( __APPLE__ ) || defined( __linux__ ) )
    for( uint8_t i = 0; i < len; ++i )
        buf[i] = rand();
#else
    // os_get_random( buf, len );
    esp_fill_random( buf, len );

#endif  // ( defined( _WINDOWS) || defined(__APPLE__) || defined(__linux__)
}

void /*RET Nothing                  */
security2_CommandHandler( ts_param_t* p,
                          const ZW_APPLICATION_TX_BUFFER* pCmd,
                          uint16_t cmdLength ) /* IN Number of command bytes including the command */
// security2_CommandHandler(ts_param_t* p, const uint8_t *pCmd, uint16_t
// cmdLength) /* IN Number of command bytes including the command */ //JIA
// changed
{
    const uint8_t s2_cap_report[] = { COMMAND_CLASS_SECURITY_2, SECURITY_2_CAPABILITIES_REPORT, 0 };

    s2_conn.r_node = p->snode;
    s2_conn.l_node = p->dnode;
    s2_conn.zw_tx_options = p->tx_flags;
    s2_conn.zw_rx_status = p->rx_flags;
    s2_conn.tx_options = 0;
    s2_conn.rx_options =
        p->rx_flags & ( RECEIVE_STATUS_TYPE_BROAD | RECEIVE_STATUS_TYPE_MULTI ) ? S2_RXOPTION_MULTICAST : 0;
    if( ( !sec2_learn_mode ) && ( pCmd->ZW_CommonInfoFrame.cmd == SECURITY_2_NETWORK_KEY_REPORT ) )
        return;
    if( ( pCmd->ZW_CommonInfoFrame.cmd == SECURITY_2_CAPABILITIES_GET ) && ( p->scheme == SECURITY_SCHEME_2_ACCESS ) ) {
        //  if( (pCmd[1] == SECURITY_2_CAPABILITIES_GET)  && (p->scheme ==
        //  SECURITY_SCHEME_2_ACCESS)) { //JIA changed
        S2_send_data( s2_ctx, &s2_conn, s2_cap_report, sizeof( s2_cap_report ) );
    }
    else {
        S2_application_command_handler( s2_ctx, &s2_conn, (uint8_t*)pCmd, cmdLength );
    }
}

uint8_t sec2_gw_node_flags2keystore_flags( uint8_t gw_flags ) {
    uint8_t f = 0;

    if( gw_flags & NODE_FLAG_SECURITY0 ) {
        f |= KEY_CLASS_S0;
    }
    if( gw_flags & NODE_FLAG_SECURITY2_ACCESS ) {
        f |= KEY_CLASS_S2_ACCESS;
    }
    if( gw_flags & NODE_FLAG_SECURITY2_AUTHENTICATED ) {
        f |= KEY_CLASS_S2_AUTHENTICATED;
    }
    if( gw_flags & NODE_FLAG_SECURITY2_UNAUTHENTICATED ) {
        f |= KEY_CLASS_S2_UNAUTHENTICATED;
    }
    return f;
}

void sec2_abort_join() {
    //  DBG_PRINTF("S2 inclusion was aborted\n");
    //  prompt_printf("S2 inclusion was aborted\n");
    if( !s2_ctx || s2_ctx->inclusion_mode == INCLUSION_MODE_CSA )
        return;

    s2_inclusion_abort( s2_ctx );
}

void report_a_timer( void* ctxt ) {
    Log_Write( "report_a_timer\n" );
    znet_timer_stop( &s2_report_a_timer );
    znet_timer_deinit( &s2_report_a_timer );
    sec2_dsk_accept( 1, pub_resp );
    inclusion_dsk_challenge = 0;
}

void ICACHE_FLASH_ATTR S2_response_incl( uint8_t state, uint8_t keys, const uint8_t* pub_key ) {
    Log_Write( "S2_response_incl state=%02X, keys=%02X\r\n", state, keys );
    print_hex_buf( pub_key, 4 );

    if( state == STATE_KEYS_RES ) {
        sec2_key_grant( 1, ( keys & 0x87 ), ( keys & 0x40 ) ? 1 : 0 );
    }

    if( state == STATE_PUB_KEY_RES ) {
        memcpy( pub_resp, pub_key, 4 );
        znet_timer_init( &s2_report_a_timer, report_a_timer, NULL );
        znet_timer_set( &s2_report_a_timer, 15000 );
    }
}

void ICACHE_FLASH_ATTR S2_start_incl_timer( sec2_inclusion_cb_t cb ) {
    Log_Write( "S2_start_incl_timer\r\n" );
    s2_inclusion_set_timeout( NULL, 3000 );
    start_incl_timer = true;
    sec_incl_cb = cb;
}

uint8_t ICACHE_FLASH_ATTR S2_get_assigned_keys( void ) {
    uint8_t keys;
    nvm_config_get( assigned_keys, &keys );
    return keys;
}
