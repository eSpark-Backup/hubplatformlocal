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
 * S2_wrap.h
 *
 *  Created on: Aug 25, 2015
 *      Author: aes
 */

#ifndef SRC_TRANSPORT_S2_WRAP_H_
#define SRC_TRANSPORT_S2_WRAP_H_
#include "ZW_SendDataAppl.h"
#include "ZW_classcmd.h"
#include "znet/znet_errors.h"

typedef void ( *sec2_inclusion_cb_t )( uint8_t status, uint8_t dnode, uint8_t arg );

extern uint8_t MY_NODE_ID;
extern uint32_t DEVICE_HOME_ID;

extern uint8_t SecureClasses[];
extern uint8_t nSecureClasses;
// extern uint8_t learn_mode_security;
//#define DEVICE_HOME_ID 3458298405   //CE 21 72 25
//#define DEVICE_HOME_ID 3642800362   //D9 20 B8 EA

/* JIA */
#define NODE_FLAG_SECURITY0 0x01
#define NODE_FLAG_KNOWN_BAD 0x02
#define NODE_FLAG_JUST_ADDED 0x04
#define NODE_FLAG_INFO_ONLY 0x08 /* Only probe the node info */
#define NODE_FLAG_SECURITY2_UNAUTHENTICATED 0x10
#define NODE_FLAG_SECURITY2_AUTHENTICATED 0x20
#define NODE_FLAG_SECURITY2_ACCESS 0x40

/*Initialize the s2 layer*/
void sec2_init( uint32_t );
/*Deinitialize the s2 layer*/
void sec2_deinit( void );

#define STATE_KEYS_REQ 0x71
#define STATE_KEYS_RES 0x72
#define STATE_PUB_KEY_REQ 0x73
#define STATE_PUB_KEY_REQ_C 0x77
#define STATE_PUB_KEY_RES 0x74
#define STATE_SHOW_PUB_KEY 0x79
uint8_t inclusion_dsk_challenge;
extern encap_type_t ICACHE_FLASH_ATTR get_security_by_id( uint8_t node_id );
extern uint8_t ICACHE_FLASH_ATTR CC_present_in_NIF_by_ID( uint8_t node_id, uint8_t CC );
extern void ICACHE_FLASH_ATTR S2_request_incl( uint8_t state, uint8_t keys, uint8_t node_id, uint8_t* pub_key );
void ICACHE_FLASH_ATTR S2_response_incl( uint8_t state, uint8_t keys, const uint8_t* pub_key );

znet_error_t sec2_send_data( ts_param_t* p,
                             const uint8_t* data,
                             uint16_t len,
                             ZW_SendDataAppl_Callback_t Callback,
                             void* user );

void /*RET Nothing                  */
security2_CommandHandler( ts_param_t* p,
                          const ZW_APPLICATION_TX_BUFFER* pCmd, /* IN Payload from the received frame, the union */
                          // const uint8_t *pCmd, //JIA changed from ZW_APP_TX_BUFFER to this..
                          uint16_t cmdLength ); /* IN Number of command bytes including the command */

void sec2_start_learn_mode( uint8_t node_id );

void sec2_start_add_node( uint8_t node_id, sec2_inclusion_cb_t cb );

/**
 * Create a new Elliptic curve key for learn mode
 */
void sec2_create_new_static_ecdh_key();

/**
 * Create a new Elliptic curve key for add node mode
 */
void sec2_create_new_dynamic_ecdh_key();

/**
 * Create new network keys
 */
void sec2_create_new_network_keys();

/**
 * Get the node cache security flags for this node
 */
uint8_t sec2_get_my_node_flags();

/*
 * grant keys durring inclusion
 */
void sec2_key_grant( uint8_t accept, uint8_t keys, uint8_t csa );
void sec2_dsk_accept( uint8_t accept, const uint8_t* dsk );

/**
 * Convert gateway security flags to keystore security flags
 */
uint8_t sec2_gw_node_flags2keystore_flags( uint8_t gw_flags );

/**
 * Abort the current secure add /learn
 */
void sec2_abort_join();

void store_network_keys();

void create_new_network_keys();

void S2_start_incl_timer( sec2_inclusion_cb_t cb );

uint8_t ICACHE_FLASH_ATTR S2_get_assigned_keys( void );

#endif /* SRC_TRANSPORT_S2_WRAP_H_ */
