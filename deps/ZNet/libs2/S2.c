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
/*
 * S2.c
 *
 *  Created on: Jun 25, 2015
 *      Author: aes
 */
//#pragma src
//#define DEBUG
#include "S2.h"
#include "s2_protocol.h"
#include "s2_classcmd.h"
//#include "assert.h"

#include <string.h>
#include "ccm.h"
#include "aes_cmac.h"
#include "nextnonce.h"
#include "kderiv.h"
#include <bigint.h>
#include "aes.h"

#include <platform.h>
#include "ZW_classcmd.h"
#include "logger.h"
//#include <prompt.h>
#include "S2_wrap.h"

#define DEBUG_S2_FSM
#ifdef SINGLE_CONTEXT
struct S2 xdata the_context;
#define ctxt ( &the_context )
#define CTX_DEF

#define S2_fsm_post_event( a, b, c ) __S2_fsm_post_event( b, c )
#define S2_set_peer( a, b, c, d ) __S2_set_peer( b, c, d )
#define S2_span_ok( a, b ) __S2_span_ok( b )
#define S2_register_nonce( a, b, c ) __S2_register_nonce( b, c )
#define S2_verify_seq( a, b, c ) __S2_verify_seq( b, c )
#define S2_send_nonce_report( a, b, c ) __S2_send_nonce_report( b, c )
#define S2_send_raw( a, b, c ) __S2_send_raw( b, c )
#define S2_send_nonce_get( a ) __S2_send_nonce_get()
#define S2_encrypt_and_send( a ) __S2_encrypt_and_send()
#define S2_is_peernode( a, b ) __S2_is_peernode( b )
#define find_span_by_node( a, b ) __find_span_by_node( b )
#define S2_decrypt_msg( a, b, c, d, e, f ) __S2_decrypt_msg( b, c, d, e, f )
#define S2_make_aad( a, b, c, d, e, f, g, h ) __S2_make_aad( b, c, d, e, f, g, h )
#define S2_is_mos( a, b, c ) __S2_is_mos( b, c )
#define find_mpan_by_group_id( a, b, c, d ) __find_mpan_by_group_id( b, c, d )
#define S2_is_node_mos( a, b ) __S2_is_node_mos( b )
#define S2_add_mpan_extensions( a, b ) __S2_add_mpan_extensions( b )
#define S2_encrypt_and_send_multi( a ) __S2_encrypt_and_send_multi()
#define S2_register_mpan( a, b, c, d, e ) S2_register_mpan( b, c, d, e )
#define S2_post_send_done_event( a, b ) __S2_post_send_done_event( b )
#else
#define CTX_DEF struct S2* ctxt = p_context;
#endif

// static CTR_DRBG_CTX the_ctr_drbg;
CTR_DRBG_CTX the_ctr_drbg;  // JIA

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wunused-function"  // TODO: Remove after
                                                    // merging in complete
                                                    // multicast support
#endif

#ifdef DEBUG_S2_FSM1
const char* s2_state_name( states_t st ) {
    static char str[25];
    switch( st ) {
    case IDLE:
        return "IDLE";
    case WAIT_NONCE_RAPORT:
        return "WAIT_NONCE_RAPORT";
    case SENDING_MSG:
        return "SENDING_MSG";
    case SENDING_MULTICAST:
        return "SENDING_MULTICAST";
    case VERIFYING_DELIVERY:
        return "VERIFYING_DELIVERY";
    case IS_MOS_WAIT_REPLY:
        return "IS_MOS_WAIT_REPLY";

    default:
        //    snprintf(str, sizeof str, "%d", st);
        return str;
    }
}

const char* s2_event_name( event_t ev ) {
    static char str[25];
    switch( ev ) {
    case SEND_MSG:
        return "SEND_MSG";
    case SEND_MULTICAST:
        return "SEND_MULTICAST";
    case SEND_DONE:
        return "SEND_DONE";
    case GOT_NONCE_GET:
        return "GOT_NONCE_GET";
    case GOT_NONCE_RAPORT:
        return "GOT_NONCE_RAPORT";
    case GOT_ENC_MSG:
        return "GOT_ENC_MSG";
    case GOT_BAD_ENC_MSG:
        return "GOT_BAD_ENC_MSG";
    case GOT_ENC_MSG_MOS:
        return "GOT_ENC_MSG_MOS";
    case TIMEOUT:
        return "TIMEOUT";
    case ABORT:
        return "ABORT";

    default:
        //    snprintf(str, sizeof str, "%d", ev);
        return str;
    }
}

#endif /* DEBUG_S2_FSM */

#define AUTH_TAG_LEN 8

static const uint8_t zeros[32] = { 0 };

// Forwards
static void S2_fsm_post_event( struct S2* p_context, event_t e, event_data_t* d );

static void S2_set_peer( struct S2* p_context, const s2_connection_t* peer, const uint8_t* buf, uint16_t len );
static int S2_span_ok( struct S2* p_context, const s2_connection_t* con );
static uint8_t S2_register_nonce( struct S2* p_context, const uint8_t* buf, uint16_t len );
static void S2_send_nonce_get( struct S2* p_context );
static int S2_verify_seq( struct S2* p_context, const s2_connection_t* peer, uint8_t seq );
static void S2_encrypt_and_send( struct S2* p_context );
static void S2_send_nonce_report( struct S2* p_context, const s2_connection_t* conn, uint8_t flags );
static int S2_is_peernode( struct S2* p_context, const s2_connection_t* peer );

static void next_mpan_state( struct MPAN xdata* mpan );

static decrypt_return_code_t S2_decrypt_msg( struct S2* p_context,
                                             s2_connection_t* conn,
                                             uint8_t* msg,
                                             uint16_t msg_len,
                                             uint8_t** plain_text,
                                             uint16_t* plain_text_len );
static struct SPAN xdata* find_span_by_node( struct S2* p_context, const s2_connection_t* con );
static int S2_make_aad( struct S2* p_context,
                        uint8_t sender,
                        uint8_t receiver,
                        uint8_t* msg,
                        uint16_t hdr_len,
                        uint16_t msg_len,
                        uint8_t* aad,
                        uint16_t max_size );

static void S2_send_raw( struct S2* p_context, uint8_t* buf, uint16_t len );
static uint8_t S2_is_mos( struct S2* p_context, node_t node_id, uint8_t clear );

uint8_t sec2_supported_get = 0;
uint8_t sec2_security = 0;

/*
static void
S2_register_mpan(struct S2* p_context, uint8_t node_id, security_class_t
class_id, uint8_t group_id, const uint8_t* inner_state);
*/
//#define DEBUG
#ifdef DEBUG
#ifdef __C51__

#define dbg_print_hex( p, c ) print_hex( p, c )
#define dbg_print_num( x ) ZW_DEBUG_SEND_NUM( x )

void print_hex( const uint8_t* pData, int count ) {
    uint8_t i;
    ZW_DEBUG_SEND_STR( "\r\n" );
    for( i = 0; i < count; ++i ) {
        if( i > 0 ) {
            ZW_DEBUG_SEND_STR( "," );
        }
        else {
            ZW_DEBUG_SEND_STR( " " );
        }
        ZW_DEBUG_SEND_NUM( pData[i] );
        if( i % 8 == 7 ) {
            ZW_DEBUG_SEND_STR( "\r\n" );
        }
    }
}

void dbg_printf( const char* fmt, ... ) {
    ZW_DEBUG_SEND_STR( fmt );  // ZW050x does not have a full printf included,
                               // just print format string
}

#else /* ifdef __C51__ */
#define dbg_print_hex( b, l ) print_hex( b, l )

static void print_hex( const uint8_t* buf, int len ) {
    int i;
    for( i = 0; i < len; i++ ) {
        printf( "%02X", buf[i] );
    }
    printf( "\n" );
}
#define dbg_printf printf

#endif /* ifdef __C51__ */
#else  /* ifdef DEBUG */
#ifdef __C51__
#define slash /
#define dbg_printf slash /
#define dbg_print_num( x )
#define dbg_print_hex( p, c )
#define print_hex( p, c )

#else /* ifdef __C51__ */
#define dbg_printf( ... )
#define dbg_print_num( x )
#define print_hex( ... )
#define dbg_print_hex( p, c )
#endif /* #ifdef __C51__  */
#endif /* #ifdef DEBUG */

/**
 * Find or allocate an mpan by group_id id no match can be found
 * we use a new entry.
 */
static struct MPAN* find_mpan_by_group_id( struct S2* p_context,
                                           uint8_t owner_id,
                                           uint8_t group_id,
                                           uint8_t create_new ) {
    CTX_DEF
    uint8_t rnd[RANDLEN];
    int i;

    for( i = 0; i < MPAN_TABLE_SIZE; i++ ) {
        if( ( ctxt->mpan_table[i].state != MPAN_NOT_USED ) && ( ctxt->mpan_table[i].group_id == group_id ) &&
            ( ctxt->mpan_table[i].owner_id == owner_id ) &&
            ( 1 << ctxt->mpan_table[i].class_id & ctxt->loaded_keys ) ) {
            return &ctxt->mpan_table[i];
        }
    }
    if( !create_new ) {
        return 0;
    }
    /*We need to be find an unused group handle */
    AES_CTR_DRBG_Generate( &the_ctr_drbg, rnd );

    /*Allocate new entry if possible */
    for( i = 0; i < MPAN_TABLE_SIZE; i++ ) {
        if( ctxt->mpan_table[i].state == MPAN_NOT_USED ) {
            break;
        }
    }

    /*Just select a random entry Note this will overwrite existing entries
     * TODO we should really select the oldest entry
     * */
    if( i == MPAN_TABLE_SIZE ) {
        i = rnd[0] % MPAN_TABLE_SIZE;
        dbg_printf( "dropping random span entry\n" );
    }

    ctxt->mpan_table[i].state = owner_id ? MPAN_MOS : MPAN_SET;
    ctxt->mpan_table[i].group_id = group_id;
    ctxt->mpan_table[i].owner_id = owner_id;
    ctxt->mpan_table[i].class_id = ctxt->peer.class_id;  // Here we assume that peer is set...

    AES_CTR_DRBG_Generate( &the_ctr_drbg, ctxt->mpan_table[i].inner_state );

    return &ctxt->mpan_table[i];
}

static struct SPAN xdata* find_span_by_node( struct S2* p_context, const s2_connection_t* con ) {
    CTX_DEF
    uint8_t rnd[RANDLEN];
    int i;
    /* Locate existing entry */
    for( i = 0; i < SPAN_TABLE_SIZE; i++ ) {
        if( ctxt->span_table[i].state != SPAN_NOT_USED && ( ctxt->span_table[i].lnode == con->l_node ) &&
            ( ctxt->span_table[i].rnode == con->r_node ) ) {
            return &ctxt->span_table[i];
        }
    }

    AES_CTR_DRBG_Generate( &the_ctr_drbg, rnd );

    /*Allocate new entry if possible */
    for( i = 0; i < SPAN_TABLE_SIZE; i++ ) {
        if( ctxt->span_table[i].state == SPAN_NOT_USED ) {
            break;
        }
    }

    /*Just select a random entry Note this will overwrite existing entries*/
    if( i == SPAN_TABLE_SIZE ) {
        i = rnd[0] % SPAN_TABLE_SIZE;
        dbg_printf( "dropping random span entry\n" );
    }

    ctxt->span_table[i].state = SPAN_NO_SEQ;
    ctxt->span_table[i].lnode = con->l_node;
    ctxt->span_table[i].rnode = con->r_node;
    ctxt->span_table[i].tx_seq = rnd[1];
    return &ctxt->span_table[i];
}

/**
 * Check if the span is synchronized.
 */
static int S2_span_ok( struct S2* p_context, const s2_connection_t* con ) {
    CTX_DEF
    ctxt->span = find_span_by_node( ctxt, con );

    if( ctxt->span ) {
        return ( ( ctxt->span->state == SPAN_NEGOTIATED ) || ( ctxt->span->state == SPAN_SOS_REMOTE_NONCE ) ) &&
               ( ctxt->span->class_id == con->class_id );
    }
    else {
        return 0;
    }
}

/*
 * Send nonce get to ctxt->peer
 */
static void S2_send_nonce_get( struct S2* p_context ) {
    CTX_DEF
    static uint8_t nonce_get[3] = { COMMAND_CLASS_SECURITY_2, SECURITY_2_NONCE_GET, 0 };

    //  assert(ctxt->span);

    nonce_get[2] = ctxt->span->tx_seq;
    S2_send_raw( ctxt, nonce_get, 3 );
}

/**
 * Verify the sequence of the received frame.
 */
static int S2_verify_seq( struct S2* p_context, const s2_connection_t* peer, uint8_t seq ) {
    CTX_DEF
    ctxt->span = find_span_by_node( ctxt, peer );
    if( ctxt->span->state == SPAN_NO_SEQ || ctxt->span->rx_seq != seq ) {  // If this is a completely new entry, we will
                                                                           // just copy seq number and accept it
        ctxt->span->rx_seq = seq;
        return 1;
    }
    else {
        dbg_printf( "Duplicate frame dropped with seq %d\n", seq );

        return 0;
    }

#if 0
  if (ctxt->span->rx_seq < seq)
  {
    return 1;
  }
  else
  {
    if ((seq - ctxt->span->rx_seq) > (0xFF - 4))
    { //TODO verify this algorithm
      return 1;
    }
    else
    {
      return 0;
    }
  }
#endif
}

/**
 * Return node if the node in question has reported MOS
 */
static uint8_t S2_is_node_mos( struct S2* p_context, uint8_t nodeid ) {
    CTX_DEF
    uint8_t i;
    for( i = 0; i < MOS_LIST_LENGTH; i++ ) {
        if( ctxt->mos_list[i].node_id == nodeid ) {
            return 1;
        }
    }
    return 0;
}
/* Add MPAN extensions for the current ctxt->peer by checks our mpan table
 * for nodes who is reported MOS.
 *
 *
 */
static uint16_t S2_add_mpan_extensions( struct S2* p_context, uint8_t* ext_data ) {
    CTX_DEF
    uint8_t i, k;
    uint8_t* p;
    struct MPAN* mpan;

    p = ext_data;
    k = 0;
    for( i = 0; i < MOS_LIST_LENGTH; i++ ) {
        if( ctxt->mos_list[i].node_id == ctxt->peer.r_node ) {
            dbg_printf( "Adding MPAN for node %d:%d\n", ctxt->mos_list[i].node_id, ctxt->mos_list[i].group_id );
            mpan = find_mpan_by_group_id( ctxt, 0, ctxt->mos_list[i].group_id, 0 );
            if( !mpan ) {
                dbg_printf( "could not find MPAN" );
                continue;
            }
            k++;
            *p++ = 19;
            *p++ = S2_MSG_EXTHDR_TYPE_MPAN | S2_MSG_EXTHDR_MORE_FLAG;
            *p++ = ctxt->mos_list[i].group_id;
            memcpy( p, mpan->inner_state, 16 );

            // Remove the node from the mos list
            ctxt->mos_list[i].node_id = 0;
        }
    }
    /*Clear the more flag for the last extension, FIXME this does not quite work
     * if we append extensions after this one */
    if( k ) {
        ext_data[( k - 1 ) * 19 + 1] &= ~S2_MSG_EXTHDR_MORE_FLAG;
    }
    return k * 19;
}

/**
 * Encrypt a single cast message stored in ctxt and send it
 */
void S2_encrypt_and_send( struct S2* p_context ) {
    CTX_DEF
    //    Log_Write( "S2_encrypt_and_send \r\n" );
    uint8_t aad[64];
    uint16_t aad_len;
    uint8_t ei_sender[RANDLEN];  // Note we are actually only using the first 16
                                 // bytes
    uint8_t ei_receiver[16];
    uint8_t nonce[16];

    uint8_t* ciphertext;

    uint8_t* ext_data;
    uint16_t hdr_len;   // Length of unencrypted data
    uint16_t shdr_len;  // Length of encrypted header
    uint8_t* msg;
    uint8_t n_ext;
    uint16_t msg_len;

    msg = ctxt->workbuf;
    msg[0] = COMMAND_CLASS_SECURITY_2;
    msg[1] = SECURITY_2_MESSAGE_ENCAPSULATION;
    msg[2] = ctxt->span->tx_seq;

    msg[3] = 0;

    hdr_len = 4;
    n_ext = 0;
    //  ZW_DEBUG_SEND_STR("S2_encrypt_and_send\r\n");
    //  dbg_printf("S2_encrypt_and_send\r\n");
    //  dbg_printf("encrypting message of length %d is: ", ctxt->length);
    Log_Write( "encrypting message of length %d is: \r\n", ctxt->length );
    print_hex_buf( ctxt->buf, ctxt->length );
    /*If span is not negotiated, include senders nonce (SN) */
    ext_data = &msg[4];

    if( ctxt->span->state == SPAN_SOS_REMOTE_NONCE ) {
        //    ZW_DEBUG_SEND_STR("SPAN_SOS_REMOTE_NONCE. class_id: ");
        //    ZW_DEBUG_SEND_NUM(ctxt->peer.class_id); ZW_DEBUG_SEND_NL();
        AES_CTR_DRBG_Generate( &the_ctr_drbg, ei_sender );
        memcpy( ei_receiver, ctxt->span->d.r_nonce, sizeof( ei_receiver ) );

        next_nonce_instantiate( &ctxt->span->d.rng, ei_sender, ei_receiver, ctxt->sg[ctxt->peer.class_id].nonce_key );

        ctxt->span->class_id = ctxt->peer.class_id;
        ctxt->span->state = SPAN_NEGOTIATED;  // TODO is it better to set this
                                              // on send_data complete?

        *ext_data++ = 2 + sizeof( ctxt->span->d.r_nonce );  // Extension length
        *ext_data++ = S2_MSG_EXTHDR_CRITICAL_FLAG | S2_MSG_EXTHDR_TYPE_SN;
        memcpy( ext_data, ei_sender, 16 );
        hdr_len += 2 + 16;
        ext_data += 16;
        n_ext++;
    }

    if( ( ctxt->peer.tx_options & ( S2_TXOPTION_SINGLECAST_FOLLOWUP | S2_TXOPTION_FIRST_SINGLECAST_FOLLOWUP ) ) &&
        ctxt->mpan ) {
        /* If the destination is mos, then we will add the MPAN extension
         * instead */
        if( !S2_is_node_mos( ctxt, ctxt->peer.r_node ) ) {
            /* Add the MGRP header extension */
            *ext_data++ = 3;
            *ext_data++ = S2_MSG_EXTHDR_CRITICAL_FLAG | S2_MSG_EXTHDR_TYPE_MGRP;
            *ext_data++ = ctxt->mpan->group_id;
            hdr_len += 3;
            n_ext++;
        }

        if( ( ctxt->peer.tx_options & S2_TXOPTION_FIRST_SINGLECAST_FOLLOWUP ) && ctxt->retry == 2 ) {
            next_mpan_state( ctxt->mpan );
        }
    }

    /*Add MOS extension */
    if( ctxt->mpan && ctxt->mpan->state == MPAN_MOS ) {
        ctxt->mpan->state = MPAN_NOT_USED;
        ctxt->mpan = 0;
        *ext_data++ = 2;
        *ext_data++ = S2_MSG_EXTHDR_TYPE_MOS;
        hdr_len += 2;
        n_ext++;
    }

    /*Insert more flag*/
    if( n_ext ) {
        msg[3] |= SECURITY_2_MESSAGE_ENCAPSULATION_PROPERTIES1_EXTENSION_BIT_MASK;
        ext_data = &msg[4];
        while( --n_ext ) {
            ext_data[1] |= S2_MSG_EXTHDR_MORE_FLAG;
        }
        ext_data += *ext_data;
    }

    ciphertext = &msg[hdr_len];
    ZW_DEBUG_SEND_STR( "before mpan\r\n" );
    /* Add the secure extensions */
    shdr_len = S2_add_mpan_extensions( ctxt, ciphertext );
    if( shdr_len ) {
        msg[3] |= SECURITY_2_MESSAGE_ENCAPSULATION_PROPERTIES1_ENCRYPTED_EXTENSION_BIT_MASK;
    }

    memcpy( ciphertext + shdr_len, ctxt->buf, ctxt->length );
    // ZW_DEBUG_SEND_STR("after memcpy\r\n");
    ctxt->peer.l_node = MY_NODE_ID;
    aad_len = S2_make_aad( ctxt,
                           ctxt->peer.l_node,
                           ctxt->peer.r_node,
                           msg,
                           hdr_len,
                           ctxt->length + shdr_len + hdr_len + AUTH_TAG_LEN,
                           aad,
                           sizeof( aad ) );
    // ZW_DEBUG_SEND_STR("before next_nonce_generate\r\n");
    /*TODO we should consider to roll the nonce when we have recevied in ACK*/
    next_nonce_generate( &ctxt->span->d.rng, nonce );  // Create the new nonce
    ZW_DEBUG_SEND_STR( "after next_nonce_generate\r\n" );
#ifdef DEBUG
    dbg_printf( "%p Encryption class %d\n", ctxt, ctxt->peer.class_id );
    dbg_printf( "Nonce \n" );
    dbg_print_hex( nonce, 16 );
    dbg_printf( "key \n" );
    dbg_print_hex( ctxt->sg[ctxt->peer.class_id].enc_key, 16 );
    dbg_printf( "AAD \n" );
    dbg_print_hex( aad, aad_len );
#endif
    // ZW_DEBUG_SEND_STR("CCM enc auth\r\n");

    msg_len = CCM_encrypt_and_auth(
        ctxt->sg[ctxt->peer.class_id].enc_key, nonce, aad, aad_len, ciphertext, ctxt->length + shdr_len );

    //  assert(msg_len > 0);
    S2_send_raw( ctxt, msg, msg_len + hdr_len );
}

static void next_mpan_state( struct MPAN xdata* mpan ) {
    static const uint8_t one[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
    // TODO check that it will roll around
    bigint_add( mpan->inner_state, mpan->inner_state, one, 16 );
}

void S2_encrypt_and_send_multi( struct S2* p_context ) {
    CTX_DEF
    uint8_t aad[64];
    uint16_t aad_len;
    uint8_t nonce[16];
    uint8_t* ciphertext;
    uint16_t hdr_len;
    uint8_t* msg;
    uint16_t msg_len;
    event_data_t e;
    msg = ctxt->workbuf;
    msg[0] = COMMAND_CLASS_SECURITY_2;
    msg[1] = SECURITY_2_MESSAGE_ENCAPSULATION;
    msg[2] = 0xFF;
    msg[3] = SECURITY_2_MESSAGE_ENCAPSULATION_PROPERTIES1_EXTENSION_BIT_MASK;

    /* Add the encrypted header extension */
    msg[4] = 3;
    msg[5] = S2_MSG_EXTHDR_CRITICAL_FLAG | S2_MSG_EXTHDR_TYPE_MGRP;
    msg[6] = ctxt->mpan->group_id;

    hdr_len = 4 + 3;

    ciphertext = &msg[hdr_len];

    memcpy( ciphertext, ctxt->buf, ctxt->length );

    aad_len = S2_make_aad( ctxt,
                           ctxt->peer.l_node,
                           ctxt->peer.r_node,
                           msg,
                           hdr_len,
                           ctxt->length + hdr_len + AUTH_TAG_LEN,
                           aad,
                           sizeof( aad ) );

    AES128_ECB_encrypt( ctxt->mpan->inner_state, ctxt->sg[ctxt->mpan->class_id].mpan_key, nonce );
    next_mpan_state( ctxt->mpan );

#ifdef DEBUG
    dbg_printf( "%p Encryption\n", ctxt );
    dbg_printf( "Nonce \n" );
    dbg_print_hex( nonce, 16 );
    dbg_printf( "key \n" );
    dbg_print_hex( ctxt->sg[ctxt->mpan->class_id].enc_key, 16 );
    dbg_printf( "AAD \n" );
    dbg_print_hex( aad, aad_len );
#endif

    msg_len =
        CCM_encrypt_and_auth( ctxt->sg[ctxt->mpan->class_id].enc_key, nonce, aad, aad_len, ciphertext, ctxt->length );
    //  assert(msg_len > 0);

    if( S2_send_frame_multi( ctxt, &ctxt->peer, msg, msg_len + hdr_len ) ) {
        // TX seq?
    }
    else {
        e.d.tx.status = S2_TRANSMIT_COMPLETE_FAIL;
        S2_fsm_post_event( ctxt, SEND_DONE, &e );
    }
}

void S2_send_frame_done_notify( struct S2* p_context, s2_tx_status_t status, uint16_t tx_time ) {
    CTX_DEF
    event_data_t e;
    e.d.tx.status = status;
    e.d.tx.time = tx_time;
    S2_fsm_post_event( ctxt, SEND_DONE, &e );
}
/**
 * Wrapper function to send_data, which increases tx_seq and guarantees a
 * SendDone event.
 */
static void S2_send_raw( struct S2* p_context, uint8_t* buf, uint16_t len ) {
    CTX_DEF
    event_data_t e;
    if( ZNET_ERR_OK == S2_send_frame( ctxt, &ctxt->peer, buf, len ) ) {
        ctxt->span->tx_seq++;
    }
    else {
        e.d.tx.status = S2_TRANSMIT_COMPLETE_FAIL;
        S2_fsm_post_event( ctxt, SEND_DONE, &e );
    }
}

/**
 * Returns true if we are MOS with node_id
 * \param clear if set the mos state will be
 * cleared
 */
uint8_t S2_is_mos( struct S2* p_context, node_t node_id, uint8_t clear ) {
    CTX_DEF
    uint8_t i;
    for( i = 0; i < MPAN_TABLE_SIZE; i++ ) {
        if( ( ctxt->mpan_table[i].owner_id == node_id ) && ( ctxt->mpan_table[i].state == MPAN_MOS ) ) {
            if( clear ) {
                ctxt->mpan_table[i].state = MPAN_NOT_USED;
            }
            return 1;
        }
    }
    return 0;
}

/**
 * Send calculate our part of a new nonce and
 * send nonce-report, it checks if we are MOS
 * or SOS before sending.
 *
 * flags 1 : force_new_nonce
 * flags 2 : mos
 */
static void S2_send_nonce_report( struct S2* p_context, const s2_connection_t* conn, uint8_t flags ) {
    CTX_DEF
    struct SPAN* span;
    uint8_t rnd[RANDLEN];
    uint8_t* nonce_report;  //[2 + 2 + sizeof(ctxt->span->d.r_nonce)];

    /*TODO we need to protect the workbuf in the FSM*/
    nonce_report = ctxt->workbuf;

    span = find_span_by_node( ctxt, conn );

    nonce_report[0] = COMMAND_CLASS_SECURITY_2;
    nonce_report[1] = SECURITY_2_NONCE_REPORT;
    nonce_report[3] = flags;
    nonce_report[2] = span->tx_seq;

    if( flags & SECURITY_2_NONCE_REPORT_PROPERTIES1_SOS_BIT_MASK ) {
        span->state = SPAN_SOS_LOCAL_NONCE;
        AES_CTR_DRBG_Generate( &the_ctr_drbg, rnd );
        memcpy( span->d.r_nonce, rnd, 16 );
        memcpy( &nonce_report[4], span->d.r_nonce, sizeof( span->d.r_nonce ) );
    }

    span->tx_seq++;
    /*Return code is ignored here */
    S2_send_frame(
        ctxt, conn, nonce_report, nonce_report[3] & SECURITY_2_NONCE_REPORT_PROPERTIES1_SOS_BIT_MASK ? 20 : 4 );
}

static void S2_set_node_mos( struct S2* p_context, uint8_t node ) {
    CTX_DEF
    uint8_t i;
    if( ( ctxt->fsm == VERIFYING_DELIVERY || ctxt->fsm == SENDING_MSG ) &&
        ( ctxt->peer.tx_options & ( S2_TXOPTION_SINGLECAST_FOLLOWUP | S2_TXOPTION_FIRST_SINGLECAST_FOLLOWUP ) ) &&
        ctxt->mpan ) {
        for( i = 0; i < MOS_LIST_LENGTH; i++ ) {
            if( ctxt->mos_list[i].node_id == 0 ) {
                ctxt->mos_list[i].group_id = ctxt->mpan->group_id;
                ctxt->mos_list[i].node_id = node;
                break;
            }
        }
    }
}

static uint8_t S2_register_nonce( struct S2* p_context, const uint8_t* buf, uint16_t len ) {
    CTX_DEF
    Log_Write( "S2_register_nonce\r\n" );

    if( len >= ( 4 + 16 ) && ( buf[3] & SECURITY_2_NONCE_REPORT_PROPERTIES1_SOS_BIT_MASK ) ) {
        memcpy( ctxt->span->d.r_nonce, &buf[4], sizeof( ctxt->span->d.r_nonce ) );
        //        Log_Write( "ctxt->span->d.r_nonce \r\n" );
        //        print_hex_buf( ctxt->span->d.r_nonce, sizeof(
        //        ctxt->span->d.r_nonce ) );
        ctxt->span->state = SPAN_SOS_REMOTE_NONCE;
    }

    /*Register MOS, but only if we are expecting it */
    if( ( buf[3] & SECURITY_2_NONCE_REPORT_PROPERTIES1_MOS_BIT_MASK ) && ( len >= 3 ) ) {
        S2_set_node_mos( ctxt, ctxt->peer.r_node );
    }

    return buf[3];
}

/*
 * Generate Additional authentication data (AAD)
 * \param msg pointer to start of encapsulated message, ie. the first byte is
 * COMMAND_CLASS_SECURITY_2 ... \param hdr_len the length of the security
 * header, ie. the offset of the first ciphertext byte \param msg_len the total
 * length of the S2 ENCAP message \param aad buffer to write the aad into.
 * \param max_size the size of the aad buffer.
 * \return the number of bytes written into the AAD. 0 indicates that the buffer
 * was not big enough.
 */
static int S2_make_aad( struct S2* p_context,
                        uint8_t sender,
                        uint8_t receiver,
                        uint8_t* msg,
                        uint16_t hdr_len,
                        uint16_t msg_len,
                        uint8_t* aad,
                        uint16_t max_size ) {
    CTX_DEF
    if( max_size < ( hdr_len - 2 + 8 ) ) {
        return 0;
    }

    aad[0] = sender;
    aad[1] = receiver;
    aad[2] = ( ctxt->my_home_id >> 24 ) & 0xFF;
    aad[3] = ( ctxt->my_home_id >> 16 ) & 0xFF;
    aad[4] = ( ctxt->my_home_id >> 8 ) & 0xFF;
    aad[5] = ( ctxt->my_home_id >> 0 ) & 0xFF;
    aad[6] = ( msg_len >> 8 ) & 0xFF;
    aad[7] = ( msg_len >> 0 ) & 0xFF;

    memcpy( &aad[8], &msg[2], hdr_len - 2 );
    return 8 + hdr_len - 2;
}

/* Decrypt message
 * emits AuthOK or auth fail
 *  */
static decrypt_return_code_t S2_decrypt_msg( struct S2* p_context,
                                             s2_connection_t* conn,
                                             uint8_t* msg,
                                             uint16_t msg_len,
                                             uint8_t** plain_text,
                                             uint16_t* plain_text_len ) {
    CTX_DEF
    Log_Write( "S2_decrypt_msg\r\n" );
    uint8_t aad_buf[64];  // We could reduce this spec says min 30 bytes
    uint8_t nonce[16];
    uint8_t* aad = NULL;
    uint16_t aad_len;
    uint8_t flags;
    uint8_t* ciphertext = NULL;
    uint16_t ciphertext_len;
    uint16_t decrypt_len;

    uint8_t* ext_data = NULL;
    uint8_t ext_len;
    uint16_t hdr_len;
    struct SPAN* span = NULL;
    struct MPAN* mpan = NULL;
    uint8_t r_nonce[16];
    uint8_t s_nonce[16];
    uint8_t i;

    hdr_len = 4;
    decrypt_len = 0;
    *plain_text = 0;
    *plain_text_len = 0;

    flags = msg[3];
    if( msg_len < ( hdr_len + AUTH_TAG_LEN ) ) {
        goto parse_fail;
    }

    //    Log_Write( "conn->rx_options & S2_RXOPTION_MULTICAST\r\n" );
    mpan = 0;
    if( conn->rx_options & S2_RXOPTION_MULTICAST ) {
        span = 0;
    }
    else {
        /* Verify sequence */
        if( !S2_verify_seq( ctxt, conn, msg[2] ) ) {
            Log_Write( "SEQUENCE_FAIL\n" );
            return SEQUENCE_FAIL;
        }

        span = find_span_by_node( ctxt, conn );
    }

    /* Parse clear text extensions */
    if( flags & SECURITY_2_MESSAGE_ENCAPSULATION_PROPERTIES1_EXTENSION_BIT_MASK ) {
        ext_data = &msg[4];
        do {
            ext_len = ext_data[0];
            hdr_len += ext_len;

            if( msg_len < ( hdr_len + AUTH_TAG_LEN ) || 0 == ext_len ) {
                goto parse_fail;
            }

            switch( ext_data[1] & S2_MSG_EXTHDR_TYPE_MASK ) {
            case S2_MSG_EXTHDR_TYPE_SN:
                //                Log_Write( "S2_MSG_EXTHDR_TYPE_SN\r\n" );
                /*We only update SPAN if we expect an update */
                if( span && span->state == SPAN_SOS_LOCAL_NONCE &&
                    ( ext_len == ( 2 + sizeof( span->d.r_nonce ) ) ) ) { /*Save the nonces */
                    memcpy( s_nonce, &ext_data[2], 16 );
                    memcpy( r_nonce, span->d.r_nonce, 16 );
                    span->state = SPAN_INSTANTIATE;

                    next_nonce_instantiate( &span->d.rng, s_nonce, r_nonce, ctxt->sg[span->class_id].nonce_key );
                }
                break;
            case S2_MSG_EXTHDR_TYPE_MGRP:
                //                Log_Write( "S2_MSG_EXTHDR_TYPE_MGRP" );
                if( ext_len != 3 ) {
                    goto parse_fail;
                }
                /*Only create new MPAN if this was a single cast followup*/
                mpan = find_mpan_by_group_id(
                    ctxt, conn->r_node, ext_data[2], ( conn->rx_options & S2_RXOPTION_MULTICAST ) == 0 );
                break;
            case S2_MSG_EXTHDR_TYPE_MOS:
                //                Log_Write( "S2_MSG_EXTHDR_TYPE_MOS" );
                if( ext_len != 2 ) {
                    goto parse_fail;
                }
                S2_set_node_mos( ctxt, conn->r_node );
                break;
            default:
                //                Log_Write( "default" );
                if( ext_data[1] & S2_MSG_EXTHDR_CRITICAL_FLAG ) {  // Unsupported critical option
                    goto parse_fail;
                }
            }

            if( ext_data[1] & S2_MSG_EXTHDR_MORE_FLAG ) {
                ext_data += ext_len;
            }
            else {
                break;
            }
        } while( 1 );
    }

    if( conn->rx_options & S2_RXOPTION_MULTICAST ) {
        //        Log_Write( "mpan == 0 || mpan->state\r\n" );
        if( mpan == 0 || mpan->state != MPAN_SET ) {
            Log_Write( "goto auth_fail\r\n" );
            goto auth_fail;
        }
        conn->l_node = mpan->group_id;  // Used to form the aad
    }
    else {
        if( span->state != SPAN_NEGOTIATED && span->state != SPAN_INSTANTIATE ) {
            Log_Write( "Unexpected span state %d l:%d-r:%d\n", span->state, conn->l_node, conn->r_node );
            goto auth_fail;
        }
        conn->l_node = MY_NODE_ID;
    }

    ciphertext = &msg[hdr_len];
    ciphertext_len = msg_len - hdr_len;

    aad = &aad_buf[0];
    aad_len = S2_make_aad( ctxt, conn->r_node, conn->l_node, msg, hdr_len, msg_len, aad, sizeof( aad_buf ) );

    if( span ) {
        /*Single cast decryption */

        /*In this state we don't know which class_id was used to encrypt the
         * frame, so we will try de-crypting with all our classes */

        /*Check the fsm before using the workbuf */
        if( ctxt->fsm == IDLE && span->state == SPAN_INSTANTIATE ) {
            memcpy( ctxt->workbuf, ciphertext, ciphertext_len );
        }

        for( i = 0; i < N_SEC_CLASS; i++ ) {
            /*Only decrypt with a key which is loaded */
            if( ctxt->loaded_keys & ( 1 << span->class_id ) ) {
                next_nonce_generate( &span->d.rng, nonce );
#ifdef DEBUG
                dbg_printf( "%p Decryption class %d\n", ctxt, span->class_id );
                dbg_printf( "Nonce \n" );
                print_hex( nonce, 16 );
                dbg_printf( "key \n" );
                print_hex( ctxt->sg[span->class_id].enc_key, 16 );
                dbg_printf( "AAD \n" );
                print_hex( aad, aad_len );
#endif
                decrypt_len = CCM_decrypt_and_auth(
                    ctxt->sg[span->class_id].enc_key, nonce, aad, aad_len, ciphertext, ciphertext_len );

                if( decrypt_len ) {
                    span->state = SPAN_NEGOTIATED;
                    conn->class_id = span->class_id;

                    if( mpan ) {  // This means that a MGRP extension was included in the
                                  // message
                        conn->rx_options |= S2_RXOPTION_FOLLOWUP;
                        if( mpan->state == MPAN_MOS ) {
                            event_data_t e;
                            e.con = conn;
                            ctxt->mpan = mpan;
                            S2_fsm_post_event( ctxt, GOT_ENC_MSG_MOS, &e );

                            // S2_send_nonce_report(ctxt, conn,
                            // SECURITY_2_NONCE_REPORT_PROPERTIES1_MOS_BIT_MASK);
                        }
                        else {
                            next_mpan_state( mpan );
                        }
                    }
                    break;
                }
            }

            if( ctxt->fsm != IDLE || span->state == SPAN_NEGOTIATED ) {
                /*We were not able to backup the cipher-text so we will not be
                 * able to decrypt the message*/
                goto auth_fail;
            }

            /*try the next security class */
            span->class_id++;
            if( span->class_id >= N_SEC_CLASS ) {
                span->class_id = 0;
            }

            // Restore the ciphertext
            memcpy( ciphertext, ctxt->workbuf, ciphertext_len );

            /*reset prng to the negotiated state with the right new test key */
            next_nonce_instantiate( &span->d.rng, s_nonce, r_nonce, ctxt->sg[span->class_id].nonce_key );
        }
    }
    else {
        /*Multicast decryption*/
        AES128_ECB_encrypt( mpan->inner_state, ctxt->sg[mpan->class_id].mpan_key, nonce );
        next_mpan_state( mpan );

        decrypt_len =
            CCM_decrypt_and_auth( ctxt->sg[mpan->class_id].enc_key, nonce, aad, aad_len, ciphertext, ciphertext_len );
        conn->class_id = mpan->class_id;
    }

    if( decrypt_len == 0 || aad_len == 0 ) {
        goto auth_fail;
    }

    hdr_len = 0;
    /* Parse encrypted extensions */
    if( flags & SECURITY_2_MESSAGE_ENCAPSULATION_PROPERTIES1_ENCRYPTED_EXTENSION_BIT_MASK ) {
        ext_data = ciphertext;
        do {
            ext_len = ext_data[0];
            hdr_len += ext_len;

            if( hdr_len > decrypt_len || 0 == ext_len ) {
                goto parse_fail;
            }

            switch( ext_data[1] & S2_MSG_EXTHDR_TYPE_MASK ) {
            case S2_MSG_EXTHDR_TYPE_MPAN:
                if( conn->rx_options & S2_RXOPTION_MULTICAST ) {
                    /* This extension is only allowed in singlecast messages, drop it */
                    goto parse_fail;
                }
                if( ext_len != 19 ) {
                    goto parse_fail;
                }
                mpan = find_mpan_by_group_id( ctxt, conn->r_node, ext_data[2], 1 );
                memcpy( mpan->inner_state, &ext_data[3], 16 );
                mpan->state = MPAN_SET;
                mpan->class_id = span->class_id;
                /* If a new mpan was created and it wasn't a multicast, then it was a multicast followup */
                if( ( conn->rx_options & S2_RXOPTION_MULTICAST ) == 0 ) {
                    conn->rx_options |= S2_RXOPTION_FOLLOWUP;
                }
                break;
            default:
                if( ext_data[1] & S2_MSG_EXTHDR_CRITICAL_FLAG ) {  // Unsupported critical option
                    goto parse_fail;
                }
            }

            if( ext_data[1] & S2_MSG_EXTHDR_MORE_FLAG ) {
                ext_data += ext_len;
            }
            else {
                break;
            }
        } while( 1 );
    }

    *plain_text = ciphertext + hdr_len;
    *plain_text_len = decrypt_len - hdr_len;
    return AUTH_OK;

parse_fail:
    dbg_printf( "Parse fail!\n" );

    return PARSE_FAIL;

auth_fail:
    if( mpan ) {
        mpan->state = MPAN_MOS;
    }

    if( span ) {
        span->state = SPAN_SOS;  // Just invalidate the span
    }

    /*Send nonce report if this is not a multicast*/
    if( ( conn->rx_options & S2_RXOPTION_MULTICAST ) == 0 ) {
        //        Log_Write( "conn->rx_options & S2_RXOPTION_MULTICAST\r\n" );
        //    if (inclusion_dsk_challenge == 0)
        S2_send_nonce_report( ctxt,
                              conn,
                              mpan ? ( SECURITY_2_NONCE_REPORT_PROPERTIES1_SOS_BIT_MASK |
                                       SECURITY_2_NONCE_REPORT_PROPERTIES1_MOS_BIT_MASK ) :
                                     SECURITY_2_NONCE_REPORT_PROPERTIES1_SOS_BIT_MASK );
    }

    return AUTH_FAIL;
}

/*Return true if the node is the same node as we are currently handling*/
int S2_is_peernode( struct S2* p_context, const s2_connection_t* peer ) {
    CTX_DEF
    return peer->l_node == ctxt->peer.l_node && peer->r_node == ctxt->peer.r_node;
}

/*
 * Set the peer and message data
 */
static void S2_set_peer( struct S2* p_context, const s2_connection_t* peer, const uint8_t* buf, uint16_t len ) {
    CTX_DEF
    ctxt->peer = *peer;
    ctxt->buf = buf;  // TODO decide if we want a local copy?
    ctxt->length = len;
}

void S2_set_inclusion_peer( struct S2* p_context, uint8_t inclusion_peer_nodeid, uint8_t my_nodeid ) {
    CTX_DEF
    ctxt->inclusion_peer.l_node = my_nodeid;
    ctxt->inclusion_peer.r_node = inclusion_peer_nodeid;
}

/***************** PUBLIC functions ********************/
/* Send S2 encrypted frame
 * \param dst Destination nodeid. Security scheme as 0=UNAUTH, 1=AUTH, 2=ACCES,
 * (3=Sec0, not allowed here)
 *
 */
uint8_t S2_send_data( struct S2* p_context, const s2_connection_t* dst, const uint8_t* buf, uint16_t len ) {
    CTX_DEF
    event_data_t d;
    //    Log_Write( "S2_send_data len = %d, ctxt->fsm = %d, buf = %d\r\n", len,
    //               (int)ctxt->fsm, (int)buf );

    /* Validate data length */
    if( len == 0 || len > 1280 || buf == 0 || ( ( ctxt->fsm != IDLE ) && ( ctxt->fsm != IS_MOS_WAIT_REPLY ) ) ) {
        return 0;
    }

    d.d.buf.buffer = buf;
    d.d.buf.len = len;
    d.con = dst;
    S2_fsm_post_event( ctxt, SEND_MSG, &d );

    return 1;
}

void S2_init_prng( void ) {
    uint8_t entropy[32];  // declare array of 32 bytes, each with type uint8_t

    S2_get_hw_random( entropy, sizeof( entropy ) );
    AES_CTR_DRBG_Instantiate( &the_ctr_drbg, entropy, zeros );
}

struct S2* S2_init_ctx( uint32_t home ) {
    struct S2* ctx;

#ifdef SINGLE_CONTEXT
    ctx = &the_context;
#else
    static struct S2 _ctx = {};
    ctx = &_ctx;
#endif
    memset( ctx, 0, sizeof( struct S2 ) );

    //  dbg_printf("s2_init_ctx\r\n");
    Log_Write( "S2_init_ctx\r\n" );

    S2_init_prng();
    //  /* HKR: Init PRNG */
    //  uint8_t entropy[32];
    //  S2_get_hw_random(entropy, sizeof(entropy));
    //  AES_CTR_DRBG_Instantiate(&the_ctr_drbg, entropy, zeros);
    //  /* HKR: Init PRNG */

    ctx->my_home_id = home;
    ctx->loaded_keys = 0;

    ctx->fsm = IDLE;
    s2_restore_keys( ctx );

    return ctx;
}

uint8_t S2_network_key_update( struct S2* p_context,
                               security_class_t class_id,
                               const network_key_t net_key,
                               uint8_t temp_key_expand ) {
    CTX_DEF
    if( class_id >= N_SEC_CLASS ) {
        return 0;
    }
#ifdef DEBUG
    dbg_printf( "Registered class %d\n", class_id );
    dbg_print_num( class_id );
    print_hex( net_key, 16 );
#endif
    if( temp_key_expand ) {
        tempkey_expand(
            net_key, ctxt->sg[class_id].enc_key, ctxt->sg[class_id].nonce_key, ctxt->sg[class_id].mpan_key );
    }
    else {
        networkkey_expand(
            net_key, ctxt->sg[class_id].enc_key, ctxt->sg[class_id].nonce_key, ctxt->sg[class_id].mpan_key );
    }

    ctxt->loaded_keys |= 1 << class_id;
    return 1;
}

void S2_destroy( struct S2** pp_context ) {
    struct S2* p_context = *pp_context;

    CTX_DEF
    memset( ctxt, 0, sizeof( struct S2 ) );
}

void S2_application_command_handler( struct S2* p_context, s2_connection_t* src, uint8_t* buf, uint16_t len ) {
    CTX_DEF
    uint8_t* plain_text;
    uint16_t plain_text_len;
    decrypt_return_code_t rc;
    uint8_t n_commands_supported;
    uint8_t* classes;
    event_data_t d;

    d.d.buf.buffer = buf;
    d.d.buf.len = len;
    d.con = src;
    Log_Write( "S2_application_command_handler \r\n" );
    //    print_hex_buf( buf, len );
    //  prompt_printf("buf[0] is: %x", buf[0]);
    //  prompt_printf("buf[1] is: %x", buf[1]);
    if( buf[0] != COMMAND_CLASS_SECURITY_2 ) {
        return;
    }

    switch( buf[1] ) {
    case SECURITY_2_NONCE_GET:
        ZW_DEBUG_SEND_STR( "Got NONCE Get\r\n" );
        if( ( src->rx_options & S2_RXOPTION_MULTICAST ) != S2_RXOPTION_MULTICAST ) {
            S2_fsm_post_event( ctxt, GOT_NONCE_GET, &d );
        }
        break;
    case SECURITY_2_NONCE_REPORT:
        ZW_DEBUG_SEND_STR( "Got NONCE Report " );
        ZW_DEBUG_SEND_NUM( ctxt->fsm );
        ZW_DEBUG_SEND_STR( "\r\n" );
        S2_fsm_post_event( ctxt, GOT_NONCE_RAPORT, &d );
        Log_Write( "SECURITY_2_NONCE_REPORT  ctxt->fsm = %d\r\n", ctxt->fsm );
        if( ctxt->fsm == IDLE ) {
            if( sec2_supported_get ) {
                switch( sec2_security ) {
                case Encapsulation_Sec_2:
                    sec2_security = Encapsulation_Sec_AUTHENTICATED;
                    src->class_id = SECURITY_SCHEME_2_ACCESS;
                    break;
                case Encapsulation_Sec_AUTHENTICATED:
                    sec2_security = Encapsulation_Sec_UNAUTHENTICATED;
                    src->class_id = SECURITY_SCHEME_2_AUTHENTICATED;
                    break;
                case Encapsulation_Sec_UNAUTHENTICATED:
                    sec2_security = Encapsulation_None;
                    src->class_id = SECURITY_SCHEME_2_UNAUTHENTICATED;
                    break;
                case Encapsulation_None: {
                    uint8_t s2_supp_rep[] = { COMMAND_CLASS_SECURITY_2, SECURITY_2_COMMANDS_SUPPORTED_REPORT };
                    struct _ZFunction func = {};
                    func._security = sec2_security;
                    znet_cc_security_2_supported_report_lib( &func, src->r_node, sizeof( s2_supp_rep ), s2_supp_rep );
                    src->class_id = 0;
                } break;
                }
                if( src->class_id != 0 ) {
                    src->class_id -= SECURITY_SCHEME_2_UNAUTHENTICATED;
                    Log_Write( "SUPPORTED_GET class_id %d sec2_security %d\r\n", src->class_id, sec2_security );
                    static uint8_t s2_supp_get[] = { COMMAND_CLASS_SECURITY_2, SECURITY_2_COMMANDS_SUPPORTED_GET };
                    S2_send_data( ctxt, src, s2_supp_get, sizeof( s2_supp_get ) );
                }
            }
        }
        break;
    case SECURITY_2_MESSAGE_ENCAPSULATION:
        rc = S2_decrypt_msg( ctxt, src, buf, len, &plain_text, &plain_text_len );
        if( rc == AUTH_OK ) {
            dbg_printf( "decrypt ok %d\n", rc );

            S2_fsm_post_event( ctxt, GOT_ENC_MSG, &d );
            if( plain_text_len ) {
                if( plain_text[0] == COMMAND_CLASS_SECURITY_2 &&
                    !( plain_text[1] == SECURITY_2_COMMANDS_SUPPORTED_REPORT ||
                       plain_text[1] == SECURITY_2_CAPABILITIES_REPORT ) ) {
                    if( src->rx_options & S2_RXOPTION_MULTICAST ) {
                        // S2 encrypted multi-cast frames shouln't exist.
                        return;
                    }

                    if( plain_text[1] == SECURITY_2_COMMANDS_SUPPORTED_GET ) {
                        ctxt->u.commands_sup_report_buf[0] = COMMAND_CLASS_SECURITY_2;
                        ctxt->u.commands_sup_report_buf[1] = SECURITY_2_COMMANDS_SUPPORTED_REPORT;

                        S2_get_commands_supported( src->l_node, src->class_id, &classes, &n_commands_supported );

                        if( n_commands_supported + 2 > (uint16_t)sizeof( ctxt->u.commands_sup_report_buf ) ) {
                            return;
                        }
                        memcpy( &ctxt->u.commands_sup_report_buf[2], classes, n_commands_supported );
                        /*TODO If ctxt->fsm is busy the report is not going to
                         * be sent*/
                        S2_send_data( ctxt, src, ctxt->u.commands_sup_report_buf, n_commands_supported + 2 );
                    }
#if 0  /* Command was removed from spec */
          else if (plain_text[1] == SECURITY_2_CAPABILITIES_GET) //TODO Should this only be supported on the highest S2 class?
          {
            ctxt->u.capabilities_report_buf[0] = COMMAND_CLASS_SECURITY_2;
            ctxt->u.capabilities_report_buf[1] = SECURITY_2_CAPABILITIES_REPORT;
            ctxt->u.capabilities_report_buf[2] = SPAN_TABLE_SIZE;
            ctxt->u.capabilities_report_buf[3] = MPAN_TABLE_SIZE;
            /*TODO If ctxt->fsm is busy the report is not going to be sent*/
            S2_send_data(ctxt, src, ctxt->u.capabilities_report_buf,4);
          }
#endif /* #if 0 */
                    /* Don't validate inclusion_peer.l_node as it may not be
                     * initialized yet due to early start */
                    else if( ctxt->inclusion_peer.r_node == src->r_node ) {
                        ctxt->buf = plain_text;
                        ctxt->length = plain_text_len;
                        ctxt->inclusion_peer.class_id = src->class_id;
                        // Default just send the command to the inclusion fsm
                        s2_inclusion_post_event( ctxt );
                    }
                }
                else {
                    if( ( src->rx_options & S2_RXOPTION_MULTICAST ) == 0 ) {
                        S2_msg_received_event( ctxt, src, plain_text, plain_text_len );
                    }
                }
            }
        }
        else if( rc == AUTH_FAIL ) {
            dbg_printf( "decrypt auth fail %d\n", rc );

            S2_fsm_post_event( ctxt, GOT_BAD_ENC_MSG, &d );
            s2_inclusion_decryption_failure( ctxt );
        }
        else {
            dbg_printf( "decrypt error %d\n", rc );
        }
        break;
    default:
        if( buf[1] == SECURITY_2_COMMANDS_SUPPORTED_REPORT ) {
            /// INFO: moved to separate handler (
            /// Security.c:znet_cc_security_2_supported_report )
            Log_Write( "SECURITY_2_COMMANDS_SUPPORTED_REPORT (moved!!!)\r\n" );
        }
        if( ( src->rx_options & S2_RXOPTION_MULTICAST ) != S2_RXOPTION_MULTICAST ) {
            ctxt->buf = buf;  // TODO is this a good idea?
            ctxt->length = len;
            ctxt->inclusion_peer.class_id = UNENCRYPTED_CLASS;
            s2_inclusion_post_event( ctxt );
        }
    }
}

void S2_timeout_notify( struct S2* p_context ) {
    CTX_DEF
    S2_fsm_post_event( ctxt, TIMEOUT, 0 );
}

static void S2_post_send_done_event( struct S2* p_context, s2_tx_status_t status ) {
    CTX_DEF

    s2_inclusion_send_done( ctxt,
                            ( status == S2_TRANSMIT_COMPLETE_OK ) || ( status == S2_TRANSMIT_COMPLETE_VERIFIED ) );
    S2_send_done_event( ctxt, status );
}
/**
 * Update state machine
 */
void S2_fsm_post_event( struct S2* p_context, event_t e, event_data_t* d ) {
    CTX_DEF
    //    Log_Write( "S2_fsm_post_event state=%d\r\n", ctxt->fsm );

    uint8_t nr_flag;
    //  ZW_DEBUG_SEND_STR("Got S2 fsm event "); ZW_DEBUG_SEND_NUM((uint8_t)e);
    //  ZW_DEBUG_SEND_NL(); ZW_DEBUG_SEND_STR("Is peer node: ");
    //  ZW_DEBUG_SEND_NUM(S2_is_peernode(ctxt, d->con)); ZW_DEBUG_SEND_STR("
    //  event data: "); ZW_DEBUG_SEND_NUM(d->con->l_node);
    //  ZW_DEBUG_SEND_NUM(d->con->r_node); ZW_DEBUG_SEND_NUM(ctxt->peer.l_node);
    //  ZW_DEBUG_SEND_NUM(ctxt->peer.r_node); ZW_DEBUG_SEND_NL();
    //  // S2_dbg_printf("S2_fsm_post_event event: %s, state %s\n",
    //  s2_event_name(e), s2_state_name(ctxt->fsm));

    switch( ctxt->fsm ) {
    case IS_MOS_WAIT_REPLY:
    case IDLE:
        // S2_set_peer(ctxt, d->con, d->buffer, d->len);
        if( e == SEND_MSG && S2_span_ok( ctxt, d->con ) ) {
            S2_set_peer( ctxt, d->con, d->d.buf.buffer, d->d.buf.len );
            ctxt->retry = 2;

            goto send_msg_state_enter;
        }
        else if( e == SEND_MSG ) {
            ctxt->fsm = WAIT_NONCE_RAPORT;
            ctxt->retry = 2;
            ZW_DEBUG_SEND_STR( "WAIT_NONCE_RAPORT\r\n" );

            S2_set_peer( ctxt, d->con, d->d.buf.buffer, d->d.buf.len );
            S2_send_nonce_get( ctxt );
            S2_set_timeout( ctxt, 10000 );
        }
        else if( e == GOT_NONCE_GET && ( d->d.buf.len >= 3 ) && S2_verify_seq( ctxt, d->con, d->d.buf.buffer[2] ) ) {
            S2_send_nonce_report( ctxt, d->con, SECURITY_2_NONCE_REPORT_PROPERTIES1_SOS_BIT_MASK );
        }
        else if( e == GOT_NONCE_RAPORT && S2_verify_seq( ctxt, d->con, d->d.buf.buffer[2] ) ) {
            Log_Write( "GOT_NONCE_RAPORT  IDLE\r\n" );
            S2_set_peer( ctxt, d->con, d->d.buf.buffer, d->d.buf.len );
            S2_register_nonce( ctxt, d->d.buf.buffer, d->d.buf.len );
        }
        else if( e == SEND_MULTICAST ) {
            S2_set_peer( ctxt, d->con, d->d.buf.buffer, d->d.buf.len );
            ctxt->mpan = find_mpan_by_group_id( ctxt, 0, d->con->r_node, 1 );
            ctxt->fsm = SENDING_MSG;
            S2_encrypt_and_send_multi( ctxt );
        }
        else if( e == SEND_DONE ) {
            /*parse message to the inclusion FSM*/
            s2_inclusion_send_done( ctxt, d->d.tx.status == S2_TRANSMIT_COMPLETE_OK );
        }
        else if( e == GOT_ENC_MSG_MOS ) {
            S2_set_timeout( ctxt, 1000 );
            S2_set_peer( ctxt, d->con, 0, 0 );
            ctxt->fsm = IS_MOS_WAIT_REPLY;
        }
        else if( e == TIMEOUT && ctxt->fsm == IS_MOS_WAIT_REPLY ) {
            ctxt->mpan = 0;
            ctxt->fsm = IDLE;
            S2_send_nonce_report( ctxt, &ctxt->peer, SECURITY_2_NONCE_REPORT_PROPERTIES1_MOS_BIT_MASK );
        }
        break;
    case WAIT_NONCE_RAPORT:
        if( ( e == SEND_DONE ) && ( d->d.tx.status == S2_TRANSMIT_COMPLETE_OK ) ) {
            S2_set_timeout( ctxt,
                            d->d.tx.time );  // Just shorten timer but stay in this state
        }
        else if( ( e == SEND_DONE ) || ( e == TIMEOUT ) ) {
            ctxt->fsm = IDLE;
            if( e == TIMEOUT ) {
                S2_post_send_done_event( ctxt, S2_TRANSMIT_COMPLETE_FAIL );
            }
            else {
                S2_post_send_done_event( ctxt, d->d.tx.status );
            }
        }
        else if( ( e == GOT_NONCE_RAPORT ) && S2_is_peernode( ctxt, d->con ) ) {
            Log_Write( "GOT_NONCE_RAPORT  WAIT_NONCE_RAPORT\r\n" );
            if( S2_register_nonce( ctxt, d->d.buf.buffer, d->d.buf.len ) &
                SECURITY_2_NONCE_REPORT_PROPERTIES1_SOS_BIT_MASK ) {
                goto send_msg_state_enter;
            }
        }
        break;
    case SENDING_MSG:
        if( e == SEND_DONE ) {
            ctxt->fsm = IDLE;
            S2_post_send_done_event( ctxt, d->d.tx.status );
        }
        else {
            // S2_dbg_printf("Warning got event %d while in state %d", e,
            // ctxt->fsm);
        }

        break;
    case VERIFYING_DELIVERY:
        Log_Write( "VERIFYING_DELIVERY e = %d\r\n", e );
        if( e == SEND_DONE ) { /* shorten timer on ACK */
            if( d->d.tx.status == S2_TRANSMIT_COMPLETE_OK ) {
                S2_set_timeout( ctxt,
                                d->d.tx.time );  // Just shorten timer but stay in this state
            }
            else { /* bail out */
                ctxt->fsm = IDLE;
                S2_post_send_done_event( ctxt, S2_TRANSMIT_COMPLETE_NO_ACK );
            }
        }
        else if( e == GOT_ENC_MSG && S2_is_peernode( ctxt, d->con ) ) {
            if( S2_is_node_mos( ctxt, d->con->r_node ) && ( ctxt->retry > 0 ) ) {
                ctxt->length = 0;
                goto send_msg_state_enter;
            }
            else {
                ctxt->fsm = IDLE;
                S2_post_send_done_event( ctxt, S2_TRANSMIT_COMPLETE_VERIFIED );
            }
        }
        else if( e == GOT_BAD_ENC_MSG && S2_is_peernode( ctxt, d->con ) ) {
            ctxt->fsm = IDLE;  // No more retries
            S2_post_send_done_event( ctxt, S2_TRANSMIT_COMPLETE_FAIL );
        }
        else if( e == TIMEOUT ) {
            ctxt->fsm = IDLE;  // The frame seems to be handled but we don't
                               // know for sure
            S2_post_send_done_event( ctxt, S2_TRANSMIT_COMPLETE_OK );
        }
        else if( e == GOT_NONCE_RAPORT && S2_is_peernode( ctxt, d->con ) ) {
            nr_flag = S2_register_nonce( ctxt, d->d.buf.buffer, d->d.buf.len );
            if( nr_flag == 0 ) {
                return;
            }
            else if( nr_flag == SECURITY_2_NONCE_REPORT_PROPERTIES1_MOS_BIT_MASK ) {
                /* if we only get a MOS flag back, set the payload length to 0,
                 * as we don't need to retransmit the payload, we should only
                 * send the MPAN*/
                ctxt->length = 0;
            }

            if( ctxt->retry == 0 ) {
                ctxt->fsm = IDLE;  // No more retries
                S2_post_send_done_event( ctxt, S2_TRANSMIT_COMPLETE_FAIL );
            }
            else {
                goto send_msg_state_enter;
                // send again
            }
        }
        else {
            dbg_printf( "Warning got event %d while in state %d", e, ctxt->fsm );
        }
        break;
    default:
        //    assert(0);
        break;
    }

    return;
send_msg_state_enter:
    ctxt->fsm = SENDING_MSG;
    S2_encrypt_and_send( ctxt );

    ctxt->retry--;
    if( ctxt->peer.tx_options & S2_TXOPTION_VERIFY_DELIVERY ) {
        ctxt->fsm = VERIFYING_DELIVERY;
        S2_set_timeout( ctxt, 10000 );
    }
    return;
}

uint8_t S2_send_data_multicast( struct S2* p_context, const s2_connection_t* con, const uint8_t* buf, uint16_t len ) {
    CTX_DEF
    event_data_t e;

    if( ctxt->fsm != IDLE ) {
        return 0;
    }

    e.d.buf.buffer = buf;
    e.d.buf.len = len;
    e.con = con;
    dbg_printf( "S2 send data to %d->%d class %d\n", con->l_node, con->r_node, con->class_id );
    S2_fsm_post_event( ctxt, SEND_MULTICAST, &e );
    return 1;
}
