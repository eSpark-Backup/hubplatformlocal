/* © 2014 Sigma Designs, Inc. This is an unpublished work protected by Sigma
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
 * control, direct life support machines, or weapons systems, in which the
 * failure of the Sigma Program, or Company Applications created using the Sigma
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

#include "security_layer.h"
#include "Security_Scheme0.h"
#include "S2_wrap.h"
#include "S2.h"
#include "s2_keystore.h"
#include "ZW_basis_api_duplicate.h"
#include "logger.h"
#include "ZW_PRNG.h"

extern ZNet_t znet_instance;
#include "znet_lib_fc.h"

#define SECURITY_SCHEME_0_BIT 0x1

// Number of implemented schemes
#define N_SCHEMES 1

ts_param_t cur_tsparm;

typedef enum {
    NONE,
    KEY_SET,
    KEY_VERIFY,
    SCHEME_INHERIT,
    SUPPORTED_GET,
} include_state_t;
include_state_t include_state;

sec_learn_complete_t sec_learn_complete;
sec_learn_complete_t sec_add_node_complete;
uint8_t node_is_contrl;

extern uint8_t SIS_id;

static void ICACHE_FLASH_ATTR timeout( void* user );

/************************************* Common functions
 * *******************************************/

/*
 * Load security states from eeprom
 */
void ICACHE_FLASH_ATTR security_load_state() {
    int8_t scheme;
    keystore_network_key_read( KEY_CLASS_S0, networkKey );
    nvm_config_get( security_scheme, &scheme );
    sec0_set_key( networkKey );
}

/*
 * Save security states to eeprom
 */
void ICACHE_FLASH_ATTR security_save_state() {
    keystore_network_key_write( KEY_CLASS_S0, networkKey );
}

void ICACHE_FLASH_ATTR security_init() {
    static int prng_initialized = 0;
    if( !prng_initialized ) {
        S2_init_prng();
        InitPRNG();
        prng_initialized = 1;
    }

    /* Initialize the transport layer*/

    sec2_init( DEVICE_HOME_ID );

    sec0_init();

    message_sending = Message_Not_Sending;

    security_load_state();

    znet_timer_init( &timer, timeout, NULL );
    znet_timer_stop( &timer );
    include_state = NONE;

    ss0_enable = ENABLE_GET_MESSAGE | ENABLE_GET_NONCE;
}

void ICACHE_FLASH_ATTR security_set_default() {
    sec0_reset_netkey();
    security_save_state();
    //    security_init();
}

/**
 * timeout function used by the statemachine timer
 */
static void ICACHE_FLASH_ATTR timeout( void* user ) {
    UNUSED( user );
    Log_Write( "(%u) Security timer triggered\r\n", SystemTime_Decode_ms( SystemTime_Now() ) );
    if( ( ss0_enable & LEARN_MODE ) != 0 ) {
        ss0_enable = ENABLE_GET_NONCE;
        include_state = NONE;
        if( sec_learn_complete )
            sec_learn_complete( ZNET_ERR_TIME, cur_tsparm.dnode, 0 );
    }
    else {
        if( ( ss0_enable & ADD_NODE ) != 0 ) {
            ss0_enable = 0;
            if( sec_add_node_complete )
                sec_add_node_complete( ZNET_ERR_OK, cur_tsparm.dnode, 0 );
        }
        else
            ss0_enable = ENABLE_GET_NONCE;
    }
    message_sending = Message_Not_Sending;
    //  process_poll(NULL);
}

void ICACHE_FLASH_ATTR /*RET Nothing                  */
security_CommandHandler( ts_param_t* p,
                         const ZW_APPLICATION_TX_BUFFER* pCmd, /* IN Payload from the received frame, the union */
                                                               /*    should be used to access the fields */
                         BYTE cmdLength )                      /* IN Number of command bytes including the command */
{
    uint8_t assigned_keys;
    /*
    BYTE txOption;

    txOption = ((p->rx_flags & RECEIVE_STATUS_LOW_POWER) ?
    TRANSMIT_OPTION_LOW_POWER : 0) | TRANSMIT_OPTION_ACK |
    TRANSMIT_OPTION_EXPLORE;
  */
    Log_Write( "security_CommandHandler \r\n" );
    print_hex_buf( (uint8_t*)pCmd, cmdLength );
    switch( pCmd->ZW_CommonInfoFrame.cmd ) {
    /*Learn mode */
    case NETWORK_KEY_SET:
        Log_Write( "NETWORK_KEY_SET \r\n" );
        if( ( p->scheme == SECURITY_SCHEME_0 ) || ( KEY_SET == include_state ) ) {
            include_state = KEY_VERIFY;
            memcpy( networkKey, &pCmd->ZW_NetworkKeySet1byteFrame.networkKeyByte1, 16 );
            //            sec0_set_key( networkKey );
            assigned_keys = KEY_CLASS_S0;
            nvm_config_set( assigned_keys, &assigned_keys );
            keystore_network_key_write( KEY_CLASS_S0, networkKey );

            uint8_t dat[] = { COMMAND_CLASS_SECURITY, NETWORK_KEY_VERIFY };
            ts_param_make_reply( &cur_tsparm, p );
            cur_tsparm.scheme = SECURITY_SCHEME_0;
            sec0_send_data( &cur_tsparm, dat, sizeof( dat ), NULL, NULL );
            znet_timer_set( &timer, TIME_OUT_SEC0 );

            znet_fc_serial_api_application_node_info( TRUE, NULL, NULL );
        }
        break;
    case SECURITY_SCHEME_GET:
        Log_Write( "SECURITY_SCHEME_GET \r\n" );
        if( ( ss0_enable & LEARN_MODE ) == 0 )
            break;
        znet_timer_set( &timer, TIME_OUT_SEC0 );

        MY_NODE_ID = p->dnode;

        if( p->scheme == NO_SCHEME && cmdLength >= sizeof( ZW_SECURITY_SCHEME_GET_FRAME ) ) {
            sec0_send_security_report( p );
        }
        //        learn_mode_security = KEY_CLASS_S0;
        //        znet_fc_serial_api_application_node_info( TRUE, NULL,
        //        NULL );
        break;
    case SECURITY_SCHEME_INHERIT:
        Log_Write( "SECURITY_SCHEME_INHERIT \r\n" );
        /* MUST be received securely */
        if( ( ss0_enable & LEARN_MODE ) == 0 )
            break;
        znet_timer_stop( &timer );
        ss0_enable = ENABLE_GET_NONCE;

        if( ( p->scheme == SECURITY_SCHEME_0 && cmdLength >= sizeof( ZW_SECURITY_SCHEME_INHERIT_FRAME ) ) ||
            ( KEY_VERIFY == include_state ) ) {
            include_state = SCHEME_INHERIT;
            /* Must support SCHEME_0*/
            if( ( pCmd->ZW_SecuritySchemeInheritFrame.supportedSecuritySchemes & SECURITY_SCHEME_0_BIT ) == 0 ) {
                uint8_t dat[] = { COMMAND_CLASS_SECURITY, SECURITY_SCHEME_REPORT, 0 };
                ts_param_make_reply( &cur_tsparm, p );
                cur_tsparm.scheme = SECURITY_SCHEME_0;
                sec0_send_data( &cur_tsparm, dat, sizeof( dat ), NULL, NULL );
                store_network_keys();
                if( sec_learn_complete ) {
                    sec_learn_complete( ZNET_ERR_OK, cur_tsparm.dnode, 1 );
                }
            }
            break;
        }
        if( sec_learn_complete ) {
            sec_learn_complete( ZNET_ERR_FAULT, cur_tsparm.dnode, 0 );
        }
        break;

        /* Add node */
    case SECURITY_SCHEME_REPORT:
        Log_Write( "SECURITY_SCHEME_REPORT p->scheme = %d\r\n", p->scheme );
        if( ( ss0_enable & ADD_NODE ) == 0 )
            break;
        if( cmdLength >= sizeof( ZW_SECURITY_SCHEME_REPORT_FRAME ) ) {
            /*
                        if(
               pCmd->ZW_SecuritySchemeReportFrame.supportedSecuritySchemes != 0
               )
                        {
                            ss0_enable &= ~ADD_NODE;
                            if( sec_add_node_complete )
                                sec_add_node_complete( 0 );
                            znet_timer_stop( &timer );
                            break;
                        }
            */
            if( p->scheme == SECURITY_SCHEME_0 ) {
                if( node_is_contrl == 1 ) {
                    ss0_enable &= ~ADD_NODE;
                    if( sec_add_node_complete )
                        sec_add_node_complete( ZNET_ERR_OK, cur_tsparm.dnode, 1 );
                    znet_timer_stop( &timer );
                }
            }
            else {
                uint8_t dat_len = 2 + sizeof( networkKey );
                uint8_t* dat = (uint8_t*)Heap_Alloc( dat_len );
                if( dat == NULL )
                    break;
                dat[0] = COMMAND_CLASS_SECURITY;
                dat[1] = NETWORK_KEY_SET;
                memcpy( &dat[2], networkKey, sizeof( networkKey ) );
                uint8_t node = p->snode;
                cur_tsparm.snode = p->dnode;
                cur_tsparm.dnode = node;
                cur_tsparm.scheme = SECURITY_SCHEME_0;
                cur_tsparm.tx_flags = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
                sec0_send_data( &cur_tsparm, dat, dat_len, NULL, NULL );
                Heap_Free( dat );
                znet_timer_set( &timer, TIME_OUT_SEC0 );
                ss0_enable |= ADD_NODE;
            }
        }

        break;
    case NETWORK_KEY_VERIFY:
        Log_Write( "NETWORK_KEY_VERIFY \r\n" );
        if( p->scheme == SECURITY_SCHEME_0 ) {
            if( node_is_contrl == 1 ) {
                uint8_t dat[] = { COMMAND_CLASS_SECURITY, SECURITY_SCHEME_INHERIT, 0 };
                ts_param_make_reply( &cur_tsparm, p );
                cur_tsparm.scheme = SECURITY_SCHEME_0;
                sec0_send_data( &cur_tsparm, dat, sizeof( dat ), NULL, NULL );
                znet_timer_set( &timer, TIME_OUT_SEC0 );
            }
            else {
                ss0_enable &= ~ADD_NODE;
                if( sec_add_node_complete )
                    sec_add_node_complete( ZNET_ERR_OK, cur_tsparm.dnode, 1 );
                znet_timer_stop( &timer );
            }
        }
        break;

        /* General */
    case SECURITY_COMMANDS_SUPPORTED_GET:
        Log_Write( "SECURITY_COMMANDS_SUPPORTED_GET \r\n" );
        if( ( p->scheme == SECURITY_SCHEME_0 ) || ( SCHEME_INHERIT == include_state ) ) {
            nvm_config_get( assigned_keys, &assigned_keys );
            include_state = SUPPORTED_GET;
            ts_param_make_reply( &cur_tsparm, p );
            cur_tsparm.scheme = SECURITY_SCHEME_0;
            if( ( assigned_keys & KEY_CLASS_S0 ) != assigned_keys ) {
                uint8_t dat[] = { COMMAND_CLASS_SECURITY, SECURITY_COMMANDS_SUPPORTED_REPORT };
                sec0_send_data( &cur_tsparm, dat, sizeof( dat ), NULL, NULL );
            }
            else {
                uint8_t dat[3 + nSecureClasses];
                dat[0] = COMMAND_CLASS_SECURITY;
                dat[1] = SECURITY_COMMANDS_SUPPORTED_REPORT;
                dat[2] = 0;
                for( uint8_t i = 0; i < nSecureClasses; i++ )
                    dat[3 + i] = SecureClasses[i];
                sec0_send_data( &cur_tsparm, dat, sizeof( dat ), NULL, NULL );
            }
        }
        break;
    case SECURITY_NONCE_GET:
        Log_Write( "SECURITY_NONCE_GET ss0_enable = %d\r\n", ss0_enable );
        if( ( ss0_enable & ENABLE_GET_NONCE ) == 0 )
            break;

        znet_timer_set( &timer, TIME_OUT_SEC0 );

        if( p->scheme == NO_SCHEME ) {
            ss0_enable |= ENABLE_GET_MESSAGE;
            sec0_send_nonce( p );
        }
        break;
    case SECURITY_NONCE_REPORT:
        Log_Write( "SECURITY_NONCE_REPORT ss0_enable = %d, include_state  %d\r\n", ss0_enable, include_state );
        if( p->scheme == NO_SCHEME && cmdLength >= sizeof( ZW_SECURITY_NONCE_REPORT_FRAME ) ) {
            sec0_register_nonce( p->snode, p->dnode, pCmd->ZW_SecurityNonceReportFrame.nonceByte1 );
            if( ( ss0_enable & ADD_NODE ) == 0 )
                znet_timer_stop( &timer );
            else
                znet_timer_set( &timer, TIME_OUT_SEC0 );
        }
        if( KEY_SET == include_state ) {
            include_state = KEY_VERIFY;
        }
        break;
    case SECURITY_COMMANDS_SUPPORTED_REPORT:
        /// INFO: moved to separate handler (
        /// Security.c:znet_cc_security_supported_report )
        Log_Write( "SECURITY_COMMANDS_SUPPORTED_REPORT (moved!!!)\r\n" );
        break;

    case SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET:
    case SECURITY_MESSAGE_ENCAPSULATION: {
        Log_Write( "SECURITY_MESSAGE_ENCAPSULATION ss0_enable = %d\r\n", ss0_enable );
        if( ( ss0_enable & ENABLE_GET_MESSAGE ) == 0 )
            break;

        znet_timer_stop( &timer );

        uint8_t rxBuffer[64];
        uint8_t len;

        if( KEY_SET == include_state ) {
            uint8_t inclusionKey[16];
            memset( inclusionKey, 0, 16 );
            sec0_set_key( inclusionKey );
        }

        len = sec0_decrypt_message( p->snode, p->dnode, (uint8_t*)pCmd, cmdLength, rxBuffer );
        if( len ) {
            p->scheme = SECURITY_SCHEME_0;
            Log_Write( "decrypt message: \r\n" );
            print_hex_buf( rxBuffer, len );

            ZNet_ApplicationCommandHandler( rxBuffer, len, p->snode, Encapsulation_Sec_0, 0 );
        }
        if( pCmd->ZW_CommonInfoFrame.cmd == SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET ) {
            sec0_send_nonce( p );
        }
    } break;
    }

    // process_poll(&serial_api_process);
}

void ICACHE_FLASH_ATTR security_learn_begin( sec_learn_complete_t __cb ) {
    Log_Write( "security_learn_begin\r\n" );
    ss0_enable = LEARN_MODE | ENABLE_GET_MESSAGE | ENABLE_GET_NONCE;
    znet_timer_set( &timer, TIME_OUT_SEC0 );
    sec_learn_complete = __cb;
    include_state = KEY_SET;
}

znet_error_t ICACHE_FLASH_ATTR security_add_begin( uint8_t node, uint8_t controller, sec_learn_complete_t __cb ) {
    node_is_contrl = controller;
    Log_Write( "(%u) security_add_begin\r\n", SystemTime_Decode_ms( SystemTime_Now() ) );
    ss0_enable = ADD_NODE | ENABLE_GET_MESSAGE | ENABLE_GET_NONCE;
    znet_timer_set( &timer, TIME_OUT_SEC0 );
    sec_add_node_complete = __cb;

    uint8_t dat[] = { COMMAND_CLASS_SECURITY, SECURITY_SCHEME_GET, 0 };
    cur_tsparm.snode = MY_NODE_ID;
    cur_tsparm.dnode = node;
    cur_tsparm.scheme = NO_SCHEME;
    cur_tsparm.tx_flags = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
    return send_data( &cur_tsparm, dat, sizeof( dat ), 0 );
    // Heap_Free( dat );
}
