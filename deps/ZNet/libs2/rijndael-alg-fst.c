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
//#include <assert.h>
#include <stdlib.h>

#include "rijndael-alg-fst.h"
//#include "c_types.h"

#ifdef __ASIX_C51__
#define FAR far
#else
#define FAR
#endif

/*
Get_Te(0, x] = S [x].[02, 01, 01, 03];
Get_Te(1, x] = S [x].[03, 02, 01, 01];
Get_Te(2, x] = S [x].[01, 03, 02, 01];
Get_Te(3, x] = S [x].[01, 01, 03, 02];
Get_Te(4, x] = S [x].[01, 01, 01, 01];

Get_Td(0, x) = Si[x].[0e, 09, 0d, 0b];
Get_Td(1, x) = Si[x].[0b, 0e, 09, 0d];
Get_Td(2, x) = Si[x].[0d, 0b, 0e, 09];
Get_Td(3, x) = Si[x].[09, 0d, 0b, 0e];
Get_Td(4, x] = Si[x].[01, 01, 01, 01];
*/

/* clang-format off */
const uint32_t znTe0[256]  = {
    0xc66363a5U, 0xf87c7c84U, 0xee777799U, 0xf67b7b8dU,
    0xfff2f20dU, 0xd66b6bbdU, 0xde6f6fb1U, 0x91c5c554U,
    0x60303050U, 0x02010103U, 0xce6767a9U, 0x562b2b7dU,
    0xe7fefe19U, 0xb5d7d762U, 0x4dababe6U, 0xec76769aU,
    0x8fcaca45U, 0x1f82829dU, 0x89c9c940U, 0xfa7d7d87U,
    0xeffafa15U, 0xb25959ebU, 0x8e4747c9U, 0xfbf0f00bU,
    0x41adadecU, 0xb3d4d467U, 0x5fa2a2fdU, 0x45afafeaU,
    0x239c9cbfU, 0x53a4a4f7U, 0xe4727296U, 0x9bc0c05bU,
    0x75b7b7c2U, 0xe1fdfd1cU, 0x3d9393aeU, 0x4c26266aU,
    0x6c36365aU, 0x7e3f3f41U, 0xf5f7f702U, 0x83cccc4fU,
    0x6834345cU, 0x51a5a5f4U, 0xd1e5e534U, 0xf9f1f108U,
    0xe2717193U, 0xabd8d873U, 0x62313153U, 0x2a15153fU,
    0x0804040cU, 0x95c7c752U, 0x46232365U, 0x9dc3c35eU,
    0x30181828U, 0x379696a1U, 0x0a05050fU, 0x2f9a9ab5U,
    0x0e070709U, 0x24121236U, 0x1b80809bU, 0xdfe2e23dU,
    0xcdebeb26U, 0x4e272769U, 0x7fb2b2cdU, 0xea75759fU,
    0x1209091bU, 0x1d83839eU, 0x582c2c74U, 0x341a1a2eU,
    0x361b1b2dU, 0xdc6e6eb2U, 0xb45a5aeeU, 0x5ba0a0fbU,
    0xa45252f6U, 0x763b3b4dU, 0xb7d6d661U, 0x7db3b3ceU,
    0x5229297bU, 0xdde3e33eU, 0x5e2f2f71U, 0x13848497U,
    0xa65353f5U, 0xb9d1d168U, 0x00000000U, 0xc1eded2cU,
    0x40202060U, 0xe3fcfc1fU, 0x79b1b1c8U, 0xb65b5bedU,
    0xd46a6abeU, 0x8dcbcb46U, 0x67bebed9U, 0x7239394bU,
    0x944a4adeU, 0x984c4cd4U, 0xb05858e8U, 0x85cfcf4aU,
    0xbbd0d06bU, 0xc5efef2aU, 0x4faaaae5U, 0xedfbfb16U,
    0x864343c5U, 0x9a4d4dd7U, 0x66333355U, 0x11858594U,
    0x8a4545cfU, 0xe9f9f910U, 0x04020206U, 0xfe7f7f81U,
    0xa05050f0U, 0x783c3c44U, 0x259f9fbaU, 0x4ba8a8e3U,
    0xa25151f3U, 0x5da3a3feU, 0x804040c0U, 0x058f8f8aU,
    0x3f9292adU, 0x219d9dbcU, 0x70383848U, 0xf1f5f504U,
    0x63bcbcdfU, 0x77b6b6c1U, 0xafdada75U, 0x42212163U,
    0x20101030U, 0xe5ffff1aU, 0xfdf3f30eU, 0xbfd2d26dU,
    0x81cdcd4cU, 0x180c0c14U, 0x26131335U, 0xc3ecec2fU,
    0xbe5f5fe1U, 0x359797a2U, 0x884444ccU, 0x2e171739U,
    0x93c4c457U, 0x55a7a7f2U, 0xfc7e7e82U, 0x7a3d3d47U,
    0xc86464acU, 0xba5d5de7U, 0x3219192bU, 0xe6737395U,
    0xc06060a0U, 0x19818198U, 0x9e4f4fd1U, 0xa3dcdc7fU,
    0x44222266U, 0x542a2a7eU, 0x3b9090abU, 0x0b888883U,
    0x8c4646caU, 0xc7eeee29U, 0x6bb8b8d3U, 0x2814143cU,
    0xa7dede79U, 0xbc5e5ee2U, 0x160b0b1dU, 0xaddbdb76U,
    0xdbe0e03bU, 0x64323256U, 0x743a3a4eU, 0x140a0a1eU,
    0x924949dbU, 0x0c06060aU, 0x4824246cU, 0xb85c5ce4U,
    0x9fc2c25dU, 0xbdd3d36eU, 0x43acacefU, 0xc46262a6U,
    0x399191a8U, 0x319595a4U, 0xd3e4e437U, 0xf279798bU,
    0xd5e7e732U, 0x8bc8c843U, 0x6e373759U, 0xda6d6db7U,
    0x018d8d8cU, 0xb1d5d564U, 0x9c4e4ed2U, 0x49a9a9e0U,
    0xd86c6cb4U, 0xac5656faU, 0xf3f4f407U, 0xcfeaea25U,
    0xca6565afU, 0xf47a7a8eU, 0x47aeaee9U, 0x10080818U,
    0x6fbabad5U, 0xf0787888U, 0x4a25256fU, 0x5c2e2e72U,
    0x381c1c24U, 0x57a6a6f1U, 0x73b4b4c7U, 0x97c6c651U,
    0xcbe8e823U, 0xa1dddd7cU, 0xe874749cU, 0x3e1f1f21U,
    0x964b4bddU, 0x61bdbddcU, 0x0d8b8b86U, 0x0f8a8a85U,
    0xe0707090U, 0x7c3e3e42U, 0x71b5b5c4U, 0xcc6666aaU,
    0x904848d8U, 0x06030305U, 0xf7f6f601U, 0x1c0e0e12U,
    0xc26161a3U, 0x6a35355fU, 0xae5757f9U, 0x69b9b9d0U,
    0x17868691U, 0x99c1c158U, 0x3a1d1d27U, 0x279e9eb9U,
    0xd9e1e138U, 0xebf8f813U, 0x2b9898b3U, 0x22111133U,
    0xd26969bbU, 0xa9d9d970U, 0x078e8e89U, 0x339494a7U,
    0x2d9b9bb6U, 0x3c1e1e22U, 0x15878792U, 0xc9e9e920U,
    0x87cece49U, 0xaa5555ffU, 0x50282878U, 0xa5dfdf7aU,
    0x038c8c8fU, 0x59a1a1f8U, 0x09898980U, 0x1a0d0d17U,
    0x65bfbfdaU, 0xd7e6e631U, 0x844242c6U, 0xd06868b8U,
    0x824141c3U, 0x299999b0U, 0x5a2d2d77U, 0x1e0f0f11U,
    0x7bb0b0cbU, 0xa85454fcU, 0x6dbbbbd6U, 0x2c16163aU,
};
/* clang-format on */

/* clang-format off */
const uint32_t znTd0[256] = {
    0x51f4a750U, 0x7e416553U, 0x1a17a4c3U, 0x3a275e96U,
    0x3bab6bcbU, 0x1f9d45f1U, 0xacfa58abU, 0x4be30393U,
    0x2030fa55U, 0xad766df6U, 0x88cc7691U, 0xf5024c25U,
    0x4fe5d7fcU, 0xc52acbd7U, 0x26354480U, 0xb562a38fU,
    0xdeb15a49U, 0x25ba1b67U, 0x45ea0e98U, 0x5dfec0e1U,
    0xc32f7502U, 0x814cf012U, 0x8d4697a3U, 0x6bd3f9c6U,
    0x038f5fe7U, 0x15929c95U, 0xbf6d7aebU, 0x955259daU,
    0xd4be832dU, 0x587421d3U, 0x49e06929U, 0x8ec9c844U,
    0x75c2896aU, 0xf48e7978U, 0x99583e6bU, 0x27b971ddU,
    0xbee14fb6U, 0xf088ad17U, 0xc920ac66U, 0x7dce3ab4U,
    0x63df4a18U, 0xe51a3182U, 0x97513360U, 0x62537f45U,
    0xb16477e0U, 0xbb6bae84U, 0xfe81a01cU, 0xf9082b94U,
    0x70486858U, 0x8f45fd19U, 0x94de6c87U, 0x527bf8b7U,
    0xab73d323U, 0x724b02e2U, 0xe31f8f57U, 0x6655ab2aU,
    0xb2eb2807U, 0x2fb5c203U, 0x86c57b9aU, 0xd33708a5U,
    0x302887f2U, 0x23bfa5b2U, 0x02036abaU, 0xed16825cU,
    0x8acf1c2bU, 0xa779b492U, 0xf307f2f0U, 0x4e69e2a1U,
    0x65daf4cdU, 0x0605bed5U, 0xd134621fU, 0xc4a6fe8aU,
    0x342e539dU, 0xa2f355a0U, 0x058ae132U, 0xa4f6eb75U,
    0x0b83ec39U, 0x4060efaaU, 0x5e719f06U, 0xbd6e1051U,
    0x3e218af9U, 0x96dd063dU, 0xdd3e05aeU, 0x4de6bd46U,
    0x91548db5U, 0x71c45d05U, 0x0406d46fU, 0x605015ffU,
    0x1998fb24U, 0xd6bde997U, 0x894043ccU, 0x67d99e77U,
    0xb0e842bdU, 0x07898b88U, 0xe7195b38U, 0x79c8eedbU,
    0xa17c0a47U, 0x7c420fe9U, 0xf8841ec9U, 0x00000000U,
    0x09808683U, 0x322bed48U, 0x1e1170acU, 0x6c5a724eU,
    0xfd0efffbU, 0x0f853856U, 0x3daed51eU, 0x362d3927U,
    0x0a0fd964U, 0x685ca621U, 0x9b5b54d1U, 0x24362e3aU,
    0x0c0a67b1U, 0x9357e70fU, 0xb4ee96d2U, 0x1b9b919eU,
    0x80c0c54fU, 0x61dc20a2U, 0x5a774b69U, 0x1c121a16U,
    0xe293ba0aU, 0xc0a02ae5U, 0x3c22e043U, 0x121b171dU,
    0x0e090d0bU, 0xf28bc7adU, 0x2db6a8b9U, 0x141ea9c8U,
    0x57f11985U, 0xaf75074cU, 0xee99ddbbU, 0xa37f60fdU,
    0xf701269fU, 0x5c72f5bcU, 0x44663bc5U, 0x5bfb7e34U,
    0x8b432976U, 0xcb23c6dcU, 0xb6edfc68U, 0xb8e4f163U,
    0xd731dccaU, 0x42638510U, 0x13972240U, 0x84c61120U,
    0x854a247dU, 0xd2bb3df8U, 0xaef93211U, 0xc729a16dU,
    0x1d9e2f4bU, 0xdcb230f3U, 0x0d8652ecU, 0x77c1e3d0U,
    0x2bb3166cU, 0xa970b999U, 0x119448faU, 0x47e96422U,
    0xa8fc8cc4U, 0xa0f03f1aU, 0x567d2cd8U, 0x223390efU,
    0x87494ec7U, 0xd938d1c1U, 0x8ccaa2feU, 0x98d40b36U,
    0xa6f581cfU, 0xa57ade28U, 0xdab78e26U, 0x3fadbfa4U,
    0x2c3a9de4U, 0x5078920dU, 0x6a5fcc9bU, 0x547e4662U,
    0xf68d13c2U, 0x90d8b8e8U, 0x2e39f75eU, 0x82c3aff5U,
    0x9f5d80beU, 0x69d0937cU, 0x6fd52da9U, 0xcf2512b3U,
    0xc8ac993bU, 0x10187da7U, 0xe89c636eU, 0xdb3bbb7bU,
    0xcd267809U, 0x6e5918f4U, 0xec9ab701U, 0x834f9aa8U,
    0xe6956e65U, 0xaaffe67eU, 0x21bccf08U, 0xef15e8e6U,
    0xbae79bd9U, 0x4a6f36ceU, 0xea9f09d4U, 0x29b07cd6U,
    0x31a4b2afU, 0x2a3f2331U, 0xc6a59430U, 0x35a266c0U,
    0x744ebc37U, 0xfc82caa6U, 0xe090d0b0U, 0x33a7d815U,
    0xf104984aU, 0x41ecdaf7U, 0x7fcd500eU, 0x1791f62fU,
    0x764dd68dU, 0x43efb04dU, 0xccaa4d54U, 0xe49604dfU,
    0x9ed1b5e3U, 0x4c6a881bU, 0xc12c1fb8U, 0x4665517fU,
    0x9d5eea04U, 0x018c355dU, 0xfa877473U, 0xfb0b412eU,
    0xb3671d5aU, 0x92dbd252U, 0xe9105633U, 0x6dd64713U,
    0x9ad7618cU, 0x37a10c7aU, 0x59f8148eU, 0xeb133c89U,
    0xcea927eeU, 0xb761c935U, 0xe11ce5edU, 0x7a47b13cU,
    0x9cd2df59U, 0x55f2733fU, 0x1814ce79U, 0x73c737bfU,
    0x53f7cdeaU, 0x5ffdaa5bU, 0xdf3d6f14U, 0x7844db86U,
    0xcaaff381U, 0xb968c43eU, 0x3824342cU, 0xc2a3405fU,
    0x161dc372U, 0xbce2250cU, 0x283c498bU, 0xff0d9541U,
    0x39a80171U, 0x080cb3deU, 0xd8b4e49cU, 0x6456c190U,
    0x7bcb8461U, 0xd532b670U, 0x486c5c74U, 0xd0b85742U,
};
/* clang-format on */

/* clang-format off */
const uint8_t znTd4[256] = {
    0x52U, 0x09U, 0x6aU, 0xd5U,
    0x30U, 0x36U, 0xa5U, 0x38U,
    0xbfU, 0x40U, 0xa3U, 0x9eU,
    0x81U, 0xf3U, 0xd7U, 0xfbU,
    0x7cU, 0xe3U, 0x39U, 0x82U,
    0x9bU, 0x2fU, 0xffU, 0x87U,
    0x34U, 0x8eU, 0x43U, 0x44U,
    0xc4U, 0xdeU, 0xe9U, 0xcbU,
    0x54U, 0x7bU, 0x94U, 0x32U,
    0xa6U, 0xc2U, 0x23U, 0x3dU,
    0xeeU, 0x4cU, 0x95U, 0x0bU,
    0x42U, 0xfaU, 0xc3U, 0x4eU,
    0x08U, 0x2eU, 0xa1U, 0x66U,
    0x28U, 0xd9U, 0x24U, 0xb2U,
    0x76U, 0x5bU, 0xa2U, 0x49U,
    0x6dU, 0x8bU, 0xd1U, 0x25U,
    0x72U, 0xf8U, 0xf6U, 0x64U,
    0x86U, 0x68U, 0x98U, 0x16U,
    0xd4U, 0xa4U, 0x5cU, 0xccU,
    0x5dU, 0x65U, 0xb6U, 0x92U,
    0x6cU, 0x70U, 0x48U, 0x50U,
    0xfdU, 0xedU, 0xb9U, 0xdaU,
    0x5eU, 0x15U, 0x46U, 0x57U,
    0xa7U, 0x8dU, 0x9dU, 0x84U,
    0x90U, 0xd8U, 0xabU, 0x00U,
    0x8cU, 0xbcU, 0xd3U, 0x0aU,
    0xf7U, 0xe4U, 0x58U, 0x05U,
    0xb8U, 0xb3U, 0x45U, 0x06U,
    0xd0U, 0x2cU, 0x1eU, 0x8fU,
    0xcaU, 0x3fU, 0x0fU, 0x02U,
    0xc1U, 0xafU, 0xbdU, 0x03U,
    0x01U, 0x13U, 0x8aU, 0x6bU,
    0x3aU, 0x91U, 0x11U, 0x41U,
    0x4fU, 0x67U, 0xdcU, 0xeaU,
    0x97U, 0xf2U, 0xcfU, 0xceU,
    0xf0U, 0xb4U, 0xe6U, 0x73U,
    0x96U, 0xacU, 0x74U, 0x22U,
    0xe7U, 0xadU, 0x35U, 0x85U,
    0xe2U, 0xf9U, 0x37U, 0xe8U,
    0x1cU, 0x75U, 0xdfU, 0x6eU,
    0x47U, 0xf1U, 0x1aU, 0x71U,
    0x1dU, 0x29U, 0xc5U, 0x89U,
    0x6fU, 0xb7U, 0x62U, 0x0eU,
    0xaaU, 0x18U, 0xbeU, 0x1bU,
    0xfcU, 0x56U, 0x3eU, 0x4bU,
    0xc6U, 0xd2U, 0x79U, 0x20U,
    0x9aU, 0xdbU, 0xc0U, 0xfeU,
    0x78U, 0xcdU, 0x5aU, 0xf4U,
    0x1fU, 0xddU, 0xa8U, 0x33U,
    0x88U, 0x07U, 0xc7U, 0x31U,
    0xb1U, 0x12U, 0x10U, 0x59U,
    0x27U, 0x80U, 0xecU, 0x5fU,
    0x60U, 0x51U, 0x7fU, 0xa9U,
    0x19U, 0xb5U, 0x4aU, 0x0dU,
    0x2dU, 0xe5U, 0x7aU, 0x9fU,
    0x93U, 0xc9U, 0x9cU, 0xefU,
    0xa0U, 0xe0U, 0x3bU, 0x4dU,
    0xaeU, 0x2aU, 0xf5U, 0xb0U,
    0xc8U, 0xebU, 0xbbU, 0x3cU,
    0x83U, 0x53U, 0x99U, 0x61U,
    0x17U, 0x2bU, 0x04U, 0x7eU,
    0xbaU, 0x77U, 0xd6U, 0x26U,
    0xe1U, 0x69U, 0x14U, 0x63U,
    0x55U, 0x21U, 0x0cU, 0x7dU,
};
/* clang-format on */

const uint32_t FAR rcon[] = {
    0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000,
    0x40000000, 0x80000000, 0x1B000000, 0x36000000, /* for 128-bit blocks,
                                                       Rijndael never uses
                                                       more than 10 rcon
                                                       values */
};

#define SWAP( x ) ( _lrotl( x, 8 ) & 0x00ff00ff | _lrotr( x, 8 ) & 0xff00ff00 )

#ifdef _MSC_VER
#define GETuint32_t( p ) SWAP( *( (uint32_t*)( p ) ) )
#define PUTuint32_t( ct, st ) \
    { *( (uint32_t*)( ct ) ) = SWAP( ( st ) ); }
#else
#define GETuint32_t( pt )                                                                                  \
    ( ( ( uint32_t )( pt )[0] << 24 ) ^ ( ( uint32_t )( pt )[1] << 16 ) ^ ( ( uint32_t )( pt )[2] << 8 ) ^ \
      ( ( uint32_t )( pt )[3] ) )
#define PUTuint32_t( ct, st )                    \
    {                                            \
        ( ct )[0] = ( uint8_t )( ( st ) >> 24 ); \
        ( ct )[1] = ( uint8_t )( ( st ) >> 16 ); \
        ( ct )[2] = ( uint8_t )( ( st ) >> 8 );  \
        ( ct )[3] = ( uint8_t )( st );           \
    }
#endif

uint32_t Get_Te( uint8_t Te_N, uint32_t index ) {
    uint32_t tmp, low_byte;
    tmp = znTe0[index];
    if( Te_N == 4 ) {
        low_byte = ( uint8_t )( tmp >> 8 ) & 0x000000FF;
        tmp = ( low_byte << 24 ) | ( low_byte << 16 ) | ( low_byte << 8 ) | low_byte;
    }
    else {
        for( uint8_t i = 0; i < Te_N; ++i ) {
            low_byte = (uint8_t)tmp;
            tmp = ( tmp >> 8 ) | ( low_byte << 24 );
        }
    }
    return tmp;
}

uint32_t Get_Td( uint8_t Td_N, uint32_t index ) {
    uint32_t tmp, low_byte;
    tmp = znTd0[index];
    if( Td_N == 4 ) {
        low_byte = znTd4[index];
        tmp = ( low_byte << 24 ) | ( low_byte << 16 ) | ( low_byte << 8 ) | low_byte;
    }
    else {
        for( uint8_t i = 0; i < Td_N; ++i ) {
            low_byte = (uint8_t)tmp;
            tmp = ( tmp >> 8 ) | ( low_byte << 24 );
        }
    }
    return tmp;
}
/**
 * Expand the cipher key into the encryption key schedule.
 *
 * @return	the number of rounds for the given cipher key size.
 */
int rijndaelKeySetupEnc1( uint32_t rk[/*4*(Nr + 1)*/], const uint8_t cipherKey[], int keyBits ) {
    int i = 0;
    uint32_t temp;

    rk[0] = GETuint32_t( cipherKey );
    rk[1] = GETuint32_t( cipherKey + 4 );
    rk[2] = GETuint32_t( cipherKey + 8 );
    rk[3] = GETuint32_t( cipherKey + 12 );
    if( keyBits == 128 ) {
        for( ;; ) {
            temp = rk[3];
            rk[4] = rk[0] ^ ( Get_Te( 4, ( temp >> 16 ) & 0xff ) & 0xff000000 ) ^
                    ( Get_Te( 4, ( temp >> 8 ) & 0xff ) & 0x00ff0000 ) ^ ( Get_Te( 4, (temp)&0xff ) & 0x0000ff00 ) ^
                    ( Get_Te( 4, ( temp >> 24 ) ) & 0x000000ff ) ^ rcon[i];
            rk[5] = rk[1] ^ rk[4];
            rk[6] = rk[2] ^ rk[5];
            rk[7] = rk[3] ^ rk[6];
            if( ++i == 10 ) {
                return 10;
            }
            rk += 4;
        }
    }
    rk[4] = GETuint32_t( cipherKey + 16 );
    rk[5] = GETuint32_t( cipherKey + 20 );
    if( keyBits == 192 ) {
        for( ;; ) {
            temp = rk[5];
            rk[6] = rk[0] ^ ( Get_Te( 4, ( temp >> 16 ) & 0xff ) & 0xff000000 ) ^
                    ( Get_Te( 4, ( temp >> 8 ) & 0xff ) & 0x00ff0000 ) ^ ( Get_Te( 4, (temp)&0xff ) & 0x0000ff00 ) ^
                    ( Get_Te( 4, ( temp >> 24 ) ) & 0x000000ff ) ^ rcon[i];
            rk[7] = rk[1] ^ rk[6];
            rk[8] = rk[2] ^ rk[7];
            rk[9] = rk[3] ^ rk[8];
            if( ++i == 8 ) {
                return 12;
            }
            rk[10] = rk[4] ^ rk[9];
            rk[11] = rk[5] ^ rk[10];
            rk += 6;
        }
    }
    rk[6] = GETuint32_t( cipherKey + 24 );
    rk[7] = GETuint32_t( cipherKey + 28 );
    if( keyBits == 256 ) {
        for( ;; ) {
            temp = rk[7];
            rk[8] = rk[0] ^ ( Get_Te( 4, ( temp >> 16 ) & 0xff ) & 0xff000000 ) ^
                    ( Get_Te( 4, ( temp >> 8 ) & 0xff ) & 0x00ff0000 ) ^ ( Get_Te( 4, (temp)&0xff ) & 0x0000ff00 ) ^
                    ( Get_Te( 4, ( temp >> 24 ) ) & 0x000000ff ) ^ rcon[i];
            rk[9] = rk[1] ^ rk[8];
            rk[10] = rk[2] ^ rk[9];
            rk[11] = rk[3] ^ rk[10];
            if( ++i == 7 ) {
                return 14;
            }
            temp = rk[11];
            rk[12] = rk[4] ^ ( Get_Te( 4, ( temp >> 24 ) ) & 0xff000000 ) ^
                     ( Get_Te( 4, ( temp >> 16 ) & 0xff ) & 0x00ff0000 ) ^
                     ( Get_Te( 4, ( temp >> 8 ) & 0xff ) & 0x0000ff00 ) ^ ( Get_Te( 4, (temp)&0xff ) & 0x000000ff );
            rk[13] = rk[5] ^ rk[12];
            rk[14] = rk[6] ^ rk[13];
            rk[15] = rk[7] ^ rk[14];

            rk += 8;
        }
    }
    return 0;
}

/**
 * Expand the cipher key into the decryption key schedule.
 *
 * @return	the number of rounds for the given cipher key size.
 */
#if 0
int rijndaelKeySetupDec(uint32_t rk[/*4*(Nr + 1)*/], const uint8_t cipherKey[], int keyBits) {
	int Nr, i, j;
	uint32_t temp;

	/* expand the cipher key: */
	Nr = rijndaelKeySetupEnc(rk, cipherKey, keyBits);
	/* invert the order of the round keys: */
	for (i = 0, j = 4*Nr; i < j; i += 4, j -= 4) {
		temp = rk[i    ]; rk[i    ] = rk[j    ]; rk[j    ] = temp;
		temp = rk[i + 1]; rk[i + 1] = rk[j + 1]; rk[j + 1] = temp;
		temp = rk[i + 2]; rk[i + 2] = rk[j + 2]; rk[j + 2] = temp;
		temp = rk[i + 3]; rk[i + 3] = rk[j + 3]; rk[j + 3] = temp;
	}
	/* apply the inverse MixColumn transform to all round keys but the first and the last: */
	for (i = 1; i < Nr; i++) {
		rk += 4;
		rk[0] =
			Get_Td(0, Get_Te(4, (rk[0] >> 24)       ) & 0xff) ^
			Get_Td(1, Get_Te(4, (rk[0] >> 16) & 0xff) & 0xff) ^
			Get_Td(2, Get_Te(4, (rk[0] >>  8) & 0xff) & 0xff) ^
			Get_Td(3, Get_Te(4, (rk[0]      ) & 0xff) & 0xff);
		rk[1] =
		  Get_Td(0, Get_Te(4, (rk[1] >> 24)       ) & 0xff) ^
		  Get_Td(1, Get_Te(4, (rk[1] >> 16) & 0xff) & 0xff) ^
			Get_Td(2, Get_Te(4, (rk[1] >>  8) & 0xff) & 0xff) ^
			Get_Td(3, Get_Te(4, (rk[1]      ) & 0xff) & 0xff);
		rk[2] =
			Get_Td(0, Get_Te(4, (rk[2] >> 24)       ) & 0xff) ^
			Get_Td(1, Get_Te(4, (rk[2] >> 16) & 0xff) & 0xff) ^
			Get_Td(2, Get_Te(4, (rk[2] >>  8) & 0xff) & 0xff) ^
			Get_Td(3, Get_Te(4, (rk[2]      ) & 0xff) & 0xff);
		rk[3] =
			Get_Td(0, Get_Te(4, (rk[3] >> 24)       ) & 0xff) ^
			Get_Td(1, Get_Te(4, (rk[3] >> 16) & 0xff) & 0xff) ^
			Get_Td(2, Get_Te(4, (rk[3] >>  8) & 0xff) & 0xff) ^
			Get_Td(3, Get_Te(4, (rk[3]      ) & 0xff) & 0xff);
	}
	return Nr;
}
#endif

#ifdef __ASIX_C51__
void rijndaelEncrypt1( const uint32_t* rk, int Nr, const uint8_t pt[16], uint8_t ct[16] ) {
#else
/* clang-format off */
void rijndaelEncrypt1( const uint32_t rk[/*4*(Nr + 1)*/], int Nr,
                       const uint8_t pt[16], uint8_t ct[16] )
{
#endif

    uint32_t s0, s1, s2, s3, t0, t1, t2, t3;
#ifndef FULL_UNROLL
    int r;
#endif /* ?FULL_UNROLL */

    /*
	 * map byte array block to cipher state
	 * and add initial round key:
	 */
	s0 = GETuint32_t(pt     ) ^ rk[0];
	s1 = GETuint32_t(pt +  4) ^ rk[1];
	s2 = GETuint32_t(pt +  8) ^ rk[2];
	s3 = GETuint32_t(pt + 12) ^ rk[3];
#ifdef FULL_UNROLL
    /* round 1: */
   	t0 = Get_Te(0, s0 >> 24) ^ Get_Te(1, (s1 >> 16) & 0xff) ^ Get_Te(2, (s2 >>  8) & 0xff) ^ Get_Te(3, s3 & 0xff) ^ rk[ 4];
   	t1 = Get_Te(0, s1 >> 24) ^ Get_Te(1, (s2 >> 16) & 0xff) ^ Get_Te(2, (s3 >>  8) & 0xff) ^ Get_Te(3, s0 & 0xff) ^ rk[ 5];
   	t2 = Get_Te(0, s2 >> 24) ^ Get_Te(1, (s3 >> 16) & 0xff) ^ Get_Te(2, (s0 >>  8) & 0xff) ^ Get_Te(3, s1 & 0xff) ^ rk[ 6];
   	t3 = Get_Te(0, s3 >> 24) ^ Get_Te(1, (s0 >> 16) & 0xff) ^ Get_Te(2, (s1 >>  8) & 0xff) ^ Get_Te(3, s2 & 0xff) ^ rk[ 7];
   	/* round 2: */
   	s0 = Get_Te(0, t0 >> 24) ^ Get_Te(1, (t1 >> 16) & 0xff) ^ Get_Te(2, (t2 >>  8) & 0xff) ^ Get_Te(3, t3 & 0xff) ^ rk[ 8];
   	s1 = Get_Te(0, t1 >> 24) ^ Get_Te(1, (t2 >> 16) & 0xff) ^ Get_Te(2, (t3 >>  8) & 0xff) ^ Get_Te(3, t0 & 0xff) ^ rk[ 9];
   	s2 = Get_Te(0, t2 >> 24) ^ Get_Te(1, (t3 >> 16) & 0xff) ^ Get_Te(2, (t0 >>  8) & 0xff) ^ Get_Te(3, t1 & 0xff) ^ rk[10];
   	s3 = Get_Te(0, t3 >> 24) ^ Get_Te(1, (t0 >> 16) & 0xff) ^ Get_Te(2, (t1 >>  8) & 0xff) ^ Get_Te(3, t2 & 0xff) ^ rk[11];
    /* round 3: */
   	t0 = Get_Te(0, s0 >> 24) ^ Get_Te(1, (s1 >> 16) & 0xff) ^ Get_Te(2, (s2 >>  8) & 0xff) ^ Get_Te(3, s3 & 0xff) ^ rk[12];
   	t1 = Get_Te(0, s1 >> 24) ^ Get_Te(1, (s2 >> 16) & 0xff) ^ Get_Te(2, (s3 >>  8) & 0xff) ^ Get_Te(3, s0 & 0xff) ^ rk[13];
   	t2 = Get_Te(0, s2 >> 24) ^ Get_Te(1, (s3 >> 16) & 0xff) ^ Get_Te(2, (s0 >>  8) & 0xff) ^ Get_Te(3, s1 & 0xff) ^ rk[14];
   	t3 = Get_Te(0, s3 >> 24) ^ Get_Te(1, (s0 >> 16) & 0xff) ^ Get_Te(2, (s1 >>  8) & 0xff) ^ Get_Te(3, s2 & 0xff) ^ rk[15];
   	/* round 4: */
   	s0 = Get_Te(0, t0 >> 24) ^ Get_Te(1, (t1 >> 16) & 0xff) ^ Get_Te(2, (t2 >>  8) & 0xff) ^ Get_Te(3, t3 & 0xff) ^ rk[16];
   	s1 = Get_Te(0, t1 >> 24) ^ Get_Te(1, (t2 >> 16) & 0xff) ^ Get_Te(2, (t3 >>  8) & 0xff) ^ Get_Te(3, t0 & 0xff) ^ rk[17];
   	s2 = Get_Te(0, t2 >> 24) ^ Get_Te(1, (t3 >> 16) & 0xff) ^ Get_Te(2, (t0 >>  8) & 0xff) ^ Get_Te(3, t1 & 0xff) ^ rk[18];
   	s3 = Get_Te(0, t3 >> 24) ^ Get_Te(1, (t0 >> 16) & 0xff) ^ Get_Te(2, (t1 >>  8) & 0xff) ^ Get_Te(3, t2 & 0xff) ^ rk[19];
    /* round 5: */
   	t0 = Get_Te(0, s0 >> 24) ^ Get_Te(1, (s1 >> 16) & 0xff) ^ Get_Te(2, (s2 >>  8) & 0xff) ^ Get_Te(3, s3 & 0xff) ^ rk[20];
   	t1 = Get_Te(0, s1 >> 24) ^ Get_Te(1, (s2 >> 16) & 0xff) ^ Get_Te(2, (s3 >>  8) & 0xff) ^ Get_Te(3, s0 & 0xff) ^ rk[21];
   	t2 = Get_Te(0, s2 >> 24) ^ Get_Te(1, (s3 >> 16) & 0xff) ^ Get_Te(2, (s0 >>  8) & 0xff) ^ Get_Te(3, s1 & 0xff) ^ rk[22];
   	t3 = Get_Te(0, s3 >> 24) ^ Get_Te(1, (s0 >> 16) & 0xff) ^ Get_Te(2, (s1 >>  8) & 0xff) ^ Get_Te(3, s2 & 0xff) ^ rk[23];
   	/* round 6: */
   	s0 = Get_Te(0, t0 >> 24) ^ Get_Te(1, (t1 >> 16) & 0xff) ^ Get_Te(2, (t2 >>  8) & 0xff) ^ Get_Te(3, t3 & 0xff) ^ rk[24);
   	s1 = Get_Te(0, t1 >> 24) ^ Get_Te(1, (t2 >> 16) & 0xff) ^ Get_Te(2, (t3 >>  8) & 0xff) ^ Get_Te(3, t0 & 0xff) ^ rk[25];
   	s2 = Get_Te(0, t2 >> 24) ^ Get_Te(1, (t3 >> 16) & 0xff) ^ Get_Te(2, (t0 >>  8) & 0xff) ^ Get_Te(3, t1 & 0xff) ^ rk[26];
   	s3 = Get_Te(0, t3 >> 24) ^ Get_Te(1, (t0 >> 16) & 0xff) ^ Get_Te(2, (t1 >>  8) & 0xff) ^ Get_Te(3, t2 & 0xff) ^ rk[27];
    /* round 7: */
   	t0 = Get_Te(0, s0 >> 24) ^ Get_Te(1, (s1 >> 16) & 0xff) ^ Get_Te(2, (s2 >>  8) & 0xff) ^ Get_Te(3, s3 & 0xff) ^ rk[28];
   	t1 = Get_Te(0, s1 >> 24) ^ Get_Te(1, (s2 >> 16) & 0xff) ^ Get_Te(2, (s3 >>  8) & 0xff) ^ Get_Te(3, s0 & 0xff) ^ rk[29];
   	t2 = Get_Te(0, s2 >> 24) ^ Get_Te(1, (s3 >> 16) & 0xff) ^ Get_Te(2, (s0 >>  8) & 0xff) ^ Get_Te(3, s1 & 0xff) ^ rk[30];
   	t3 = Get_Te(0, s3 >> 24) ^ Get_Te(1, (s0 >> 16) & 0xff) ^ Get_Te(2, (s1 >>  8) & 0xff) ^ Get_Te(3, s2 & 0xff) ^ rk[31];
   	/* round 8: */
   	s0 = Get_Te(0, t0 >> 24) ^ Get_Te(1, (t1 >> 16) & 0xff) ^ Get_Te(2, (t2 >>  8) & 0xff) ^ Get_Te(3, t3 & 0xff) ^ rk[32];
   	s1 = Get_Te(0, t1 >> 24) ^ Get_Te(1, (t2 >> 16) & 0xff) ^ Get_Te(2, (t3 >>  8) & 0xff) ^ Get_Te(3, t0 & 0xff) ^ rk[33];
   	s2 = Get_Te(0, t2 >> 24) ^ Get_Te(1, (t3 >> 16) & 0xff) ^ Get_Te(2, (t0 >>  8) & 0xff) ^ Get_Te(3, t1 & 0xff) ^ rk[34];
   	s3 = Get_Te(0, t3 >> 24) ^ Get_Te(1, (t0 >> 16) & 0xff) ^ Get_Te(2, (t1 >>  8) & 0xff) ^ Get_Te(3, t2 & 0xff) ^ rk[35];
    /* round 9: */
   	t0 = Get_Te(0, s0 >> 24) ^ Get_Te(1, (s1 >> 16) & 0xff) ^ Get_Te(2, (s2 >>  8) & 0xff) ^ Get_Te(3, s3 & 0xff) ^ rk[36];
   	t1 = Get_Te(0, s1 >> 24) ^ Get_Te(1, (s2 >> 16) & 0xff) ^ Get_Te(2, (s3 >>  8) & 0xff) ^ Get_Te(3, s0 & 0xff) ^ rk[37];
   	t2 = Get_Te(0, s2 >> 24) ^ Get_Te(1, (s3 >> 16) & 0xff) ^ Get_Te(2, (s0 >>  8) & 0xff) ^ Get_Te(3, s1 & 0xff) ^ rk[38];
   	t3 = Get_Te(0, s3 >> 24) ^ Get_Te(1, (s0 >> 16) & 0xff) ^ Get_Te(2, (s1 >>  8) & 0xff) ^ Get_Te(3, s2 & 0xff) ^ rk[39];
    if (Nr > 10) 
    {
        /* round 10: */
        s0 = Get_Te(0, t0 >> 24) ^ Get_Te(1, (t1 >> 16) & 0xff) ^ Get_Te(2, (t2 >>  8) & 0xff) ^ Get_Te(3, t3 & 0xff) ^ rk[40];
        s1 = Get_Te(0, t1 >> 24) ^ Get_Te(1, (t2 >> 16) & 0xff) ^ Get_Te(2, (t3 >>  8) & 0xff) ^ Get_Te(3, t0 & 0xff) ^ rk[41];
        s2 = Get_Te(0, t2 >> 24) ^ Get_Te(1, (t3 >> 16) & 0xff) ^ Get_Te(2, (t0 >>  8) & 0xff) ^ Get_Te(3, t1 & 0xff) ^ rk[42];
        s3 = Get_Te(0, t3 >> 24) ^ Get_Te(1, (t0 >> 16) & 0xff) ^ Get_Te(2, (t1 >>  8) & 0xff) ^ Get_Te(3, t2 & 0xff) ^ rk[43];
        /* round 11: */
        t0 = Get_Te(0, s0 >> 24) ^ Get_Te(1, (s1 >> 16) & 0xff) ^ Get_Te(2, (s2 >>  8) & 0xff) ^ Get_Te(3, s3 & 0xff) ^ rk[44];
        t1 = Get_Te(0, s1 >> 24) ^ Get_Te(1, (s2 >> 16) & 0xff) ^ Get_Te(2, (s3 >>  8) & 0xff) ^ Get_Te(3, s0 & 0xff) ^ rk[45];
        t2 = Get_Te(0, s2 >> 24) ^ Get_Te(1, (s3 >> 16) & 0xff) ^ Get_Te(2, (s0 >>  8) & 0xff) ^ Get_Te(3, s1 & 0xff) ^ rk[46];
        t3 = Get_Te(0, s3 >> 24) ^ Get_Te(1, (s0 >> 16) & 0xff) ^ Get_Te(2, (s1 >>  8) & 0xff) ^ Get_Te(3, s2 & 0xff) ^ rk[47];
        if (Nr > 12) 
        {
            /* round 12: */
            s0 = Get_Te(0, t0 >> 24) ^ Get_Te(1, (t1 >> 16) & 0xff) ^ Get_Te(2, (t2 >>  8) & 0xff) ^ Get_Te(3, t3 & 0xff) ^ rk[48];
            s1 = Get_Te(0, t1 >> 24) ^ Get_Te(1, (t2 >> 16) & 0xff) ^ Get_Te(2, (t3 >>  8) & 0xff) ^ Get_Te(3, t0 & 0xff) ^ rk[49];
            s2 = Get_Te(0, t2 >> 24) ^ Get_Te(1, (t3 >> 16) & 0xff) ^ Get_Te(2, (t0 >>  8) & 0xff) ^ Get_Te(3, t1 & 0xff) ^ rk[50];
            s3 = Get_Te(0, t3 >> 24) ^ Get_Te(1, (t0 >> 16) & 0xff) ^ Get_Te(2, (t1 >>  8) & 0xff) ^ Get_Te(3, t2 & 0xff) ^ rk[51];
            /* round 13: */
            t0 = Get_Te(0, s0 >> 24) ^ Get_Te(1, (s1 >> 16) & 0xff) ^ Get_Te(2, (s2 >>  8) & 0xff) ^ Get_Te(3, s3 & 0xff) ^ rk[52];
            t1 = Get_Te(0, s1 >> 24) ^ Get_Te(1, (s2 >> 16) & 0xff) ^ Get_Te(2, (s3 >>  8) & 0xff) ^ Get_Te(3, s0 & 0xff) ^ rk[53];
            t2 = Get_Te(0, s2 >> 24) ^ Get_Te(1, (s3 >> 16) & 0xff) ^ Get_Te(2, (s0 >>  8) & 0xff) ^ Get_Te(3, s1 & 0xff) ^ rk[54];
            t3 = Get_Te(0, s3 >> 24) ^ Get_Te(1, (s0 >> 16) & 0xff) ^ Get_Te(2, (s1 >>  8) & 0xff) ^ Get_Te(3, s2 & 0xff) ^ rk[55];
        }
    }
    rk += Nr << 2;
#else  /* !FULL_UNROLL */
    /*
	 * Nr - 1 full rounds:
	 */
    r = Nr >> 1;
    for (;;) 
    {
        t0 =
            Get_Te(0, (s0 >> 24)       ) ^
            Get_Te(1, (s1 >> 16) & 0xff) ^
            Get_Te(2, (s2 >>  8) & 0xff) ^
            Get_Te(3, (s3      ) & 0xff) ^
            rk[4];
        t1 =
            Get_Te(0, (s1 >> 24)       ) ^
            Get_Te(1, (s2 >> 16) & 0xff) ^
            Get_Te(2, (s3 >>  8) & 0xff) ^
            Get_Te(3, (s0      ) & 0xff) ^
            rk[5];
        t2 =
            Get_Te(0, (s2 >> 24)       ) ^
            Get_Te(1, (s3 >> 16) & 0xff) ^
            Get_Te(2, (s0 >>  8) & 0xff) ^
            Get_Te(3, (s1      ) & 0xff) ^
            rk[6];
        t3 =
            Get_Te(0, (s3 >> 24)       ) ^
            Get_Te(1, (s0 >> 16) & 0xff) ^
            Get_Te(2, (s1 >>  8) & 0xff) ^
            Get_Te(3, (s2      ) & 0xff) ^
            rk[7];

        rk += 8;
        if (--r == 0)
        {
            break;
        }

        s0 =
            Get_Te(0, (t0 >> 24)       ) ^
            Get_Te(1, (t1 >> 16) & 0xff) ^
            Get_Te(2, (t2 >>  8) & 0xff) ^
            Get_Te(3, (t3      ) & 0xff) ^
            rk[0];
        s1 =
            Get_Te(0, (t1 >> 24)       ) ^
            Get_Te(1, (t2 >> 16) & 0xff) ^
            Get_Te(2, (t3 >>  8) & 0xff) ^
            Get_Te(3, (t0      ) & 0xff) ^
            rk[1];
        s2 =
            Get_Te(0, (t2 >> 24)       ) ^
            Get_Te(1, (t3 >> 16) & 0xff) ^
            Get_Te(2, (t0 >>  8) & 0xff) ^
            Get_Te(3, (t1      ) & 0xff) ^
            rk[2];
        s3 =
            Get_Te(0, (t3 >> 24)       ) ^
            Get_Te(1, (t0 >> 16) & 0xff) ^
            Get_Te(2, (t1 >>  8) & 0xff) ^
            Get_Te(3, (t2      ) & 0xff) ^
            rk[3];
    }
#endif /* ?FULL_UNROLL */
    /*
	 * apply last round and
	 * map cipher state to byte array block:
	 */
	s0 =
		(Get_Te(4, (t0 >> 24)       ) & 0xff000000) ^
		(Get_Te(4, (t1 >> 16) & 0xff) & 0x00ff0000) ^
		(Get_Te(4, (t2 >>  8) & 0xff) & 0x0000ff00) ^
		(Get_Te(4, (t3      ) & 0xff) & 0x000000ff) ^
		rk[0];
	PUTuint32_t(ct     , s0);
	s1 =
		(Get_Te(4, (t1 >> 24)       ) & 0xff000000) ^
		(Get_Te(4, (t2 >> 16) & 0xff) & 0x00ff0000) ^
		(Get_Te(4, (t3 >>  8) & 0xff) & 0x0000ff00) ^
		(Get_Te(4, (t0      ) & 0xff) & 0x000000ff) ^
		rk[1];
	PUTuint32_t(ct +  4, s1);
	s2 =
		(Get_Te(4, (t2 >> 24)       ) & 0xff000000) ^
		(Get_Te(4, (t3 >> 16) & 0xff) & 0x00ff0000) ^
		(Get_Te(4, (t0 >>  8) & 0xff) & 0x0000ff00) ^
		(Get_Te(4, (t1      ) & 0xff) & 0x000000ff) ^
		rk[2];
	PUTuint32_t(ct +  8, s2);
	s3 =
		(Get_Te(4, (t3 >> 24)       ) & 0xff000000) ^
		(Get_Te(4, (t0 >> 16) & 0xff) & 0x00ff0000) ^
		(Get_Te(4, (t1 >>  8) & 0xff) & 0x0000ff00) ^
		(Get_Te(4, (t2      ) & 0xff) & 0x000000ff) ^
		rk[3];
	PUTuint32_t(ct + 12, s3);
}
/* clang-format on */
#ifdef __ASIX_C51__
/* clang-format off */
void rijndaelDecrypt( const uint32_t* rk, int Nr, const uint8_t ct[16],
                      uint8_t pt[16] )
{
#else
void rijndaelDecrypt1( const uint32_t rk[/*4*(Nr + 1)*/], int Nr,
                       const uint8_t ct[16], uint8_t pt[16] )
{
#endif
    uint32_t s0, s1, s2, s3, t0, t1, t2, t3;
#ifndef FULL_UNROLL
    int r;
#endif /* ?FULL_UNROLL */

    /*
	 * map byte array block to cipher state
	 * and add initial round key:
	 */
    s0 = GETuint32_t(ct     ) ^ rk[0];
    s1 = GETuint32_t(ct +  4) ^ rk[1];
    s2 = GETuint32_t(ct +  8) ^ rk[2];
    s3 = GETuint32_t(ct + 12) ^ rk[3];
#ifdef FULL_UNROLL
    /* round 1: */
    t0 = Get_Td(0, s0 >> 24) ^ Get_Td(1, (s3 >> 16) & 0xff) ^ Get_Td(2, (s2 >>  8) & 0xff) ^ Get_Td(3, s1 & 0xff) ^ rk[ 4];
    t1 = Get_Td(0, s1 >> 24) ^ Get_Td(1, (s0 >> 16) & 0xff) ^ Get_Td(2, (s3 >>  8) & 0xff) ^ Get_Td(3, s2 & 0xff) ^ rk[ 5];
    t2 = Get_Td(0, s2 >> 24) ^ Get_Td(1, (s1 >> 16) & 0xff) ^ Get_Td(2, (s0 >>  8) & 0xff) ^ Get_Td(3, s3 & 0xff) ^ rk[ 6];
    t3 = Get_Td(0, s3 >> 24) ^ Get_Td(1, (s2 >> 16) & 0xff) ^ Get_Td(2, (s1 >>  8) & 0xff) ^ Get_Td(3, s0 & 0xff) ^ rk[ 7];
    /* round 2: */
    s0 = Get_Td(0, t0 >> 24) ^ Get_Td(1, (t3 >> 16) & 0xff) ^ Get_Td(2, (t2 >>  8) & 0xff) ^ Get_Td(3, t1 & 0xff) ^ rk[ 8];
    s1 = Get_Td(0, t1 >> 24) ^ Get_Td(1, (t0 >> 16) & 0xff) ^ Get_Td(2, (t3 >>  8) & 0xff) ^ Get_Td(3, t2 & 0xff) ^ rk[ 9];
    s2 = Get_Td(0, t2 >> 24) ^ Get_Td(1, (t1 >> 16) & 0xff) ^ Get_Td(2, (t0 >>  8) & 0xff) ^ Get_Td(3, t3 & 0xff) ^ rk[10];
    s3 = Get_Td(0, t3 >> 24) ^ Get_Td(1, (t2 >> 16) & 0xff) ^ Get_Td(2, (t1 >>  8) & 0xff) ^ Get_Td(3, t0 & 0xff) ^ rk[11];
    /* round 3: */
    t0 = Get_Td(0, s0 >> 24) ^ Get_Td(1, (s3 >> 16) & 0xff) ^ Get_Td(2, (s2 >>  8) & 0xff) ^ Get_Td(3, s1 & 0xff) ^ rk[12];
    t1 = Get_Td(0, s1 >> 24) ^ Get_Td(1, (s0 >> 16) & 0xff) ^ Get_Td(2, (s3 >>  8) & 0xff) ^ Get_Td(3, s2 & 0xff) ^ rk[13];
    t2 = Get_Td(0, s2 >> 24) ^ Get_Td(1, (s1 >> 16) & 0xff) ^ Get_Td(2, (s0 >>  8) & 0xff) ^ Get_Td(3, s3 & 0xff) ^ rk[14];
    t3 = Get_Td(0, s3 >> 24) ^ Get_Td(1, (s2 >> 16) & 0xff) ^ Get_Td(2, (s1 >>  8) & 0xff) ^ Get_Td(3, s0 & 0xff) ^ rk[15];
    /* round 4: */
    s0 = Get_Td(0, t0 >> 24) ^ Get_Td(1, (t3 >> 16) & 0xff) ^ Get_Td(2, (t2 >>  8) & 0xff) ^ Get_Td(3, t1 & 0xff) ^ rk[16];
    s1 = Get_Td(0, t1 >> 24) ^ Get_Td(1, (t0 >> 16) & 0xff) ^ Get_Td(2, (t3 >>  8) & 0xff) ^ Get_Td(3, t2 & 0xff) ^ rk[17];
    s2 = Get_Td(0, t2 >> 24) ^ Get_Td(1, (t1 >> 16) & 0xff) ^ Get_Td(2, (t0 >>  8) & 0xff) ^ Get_Td(3, t3 & 0xff) ^ rk[18];
    s3 = Get_Td(0, t3 >> 24) ^ Get_Td(1, (t2 >> 16) & 0xff) ^ Get_Td(2, (t1 >>  8) & 0xff) ^ Get_Td(3, t0 & 0xff) ^ rk[19];
    /* round 5: */
    t0 = Get_Td(0, s0 >> 24) ^ Get_Td(1, (s3 >> 16) & 0xff) ^ Get_Td(2, (s2 >>  8) & 0xff) ^ Get_Td(3, s1 & 0xff) ^ rk[20];
    t1 = Get_Td(0, s1 >> 24) ^ Get_Td(1, (s0 >> 16) & 0xff) ^ Get_Td(2, (s3 >>  8) & 0xff) ^ Get_Td(3, s2 & 0xff) ^ rk[21];
    t2 = Get_Td(0, s2 >> 24) ^ Get_Td(1, (s1 >> 16) & 0xff) ^ Get_Td(2, (s0 >>  8) & 0xff) ^ Get_Td(3, s3 & 0xff) ^ rk[22];
    t3 = Get_Td(0, s3 >> 24) ^ Get_Td(1, (s2 >> 16) & 0xff) ^ Get_Td(2, (s1 >>  8) & 0xff) ^ Get_Td(3, s0 & 0xff) ^ rk[23];
    /* round 6: */
    s0 = Get_Td(0, t0 >> 24) ^ Get_Td(1, (t3 >> 16) & 0xff) ^ Get_Td(2, (t2 >>  8) & 0xff) ^ Get_Td(3, t1 & 0xff) ^ rk[24);
    s1 = Get_Td(0, t1 >> 24) ^ Get_Td(1, (t0 >> 16) & 0xff) ^ Get_Td(2, (t3 >>  8) & 0xff) ^ Get_Td(3, t2 & 0xff) ^ rk[25];
    s2 = Get_Td(0, t2 >> 24) ^ Get_Td(1, (t1 >> 16) & 0xff) ^ Get_Td(2, (t0 >>  8) & 0xff) ^ Get_Td(3, t3 & 0xff) ^ rk[26];
    s3 = Get_Td(0, t3 >> 24) ^ Get_Td(1, (t2 >> 16) & 0xff) ^ Get_Td(2, (t1 >>  8) & 0xff) ^ Get_Td(3, t0 & 0xff) ^ rk[27];
    /* round 7: */
    t0 = Get_Td(0, s0 >> 24) ^ Get_Td(1, (s3 >> 16) & 0xff) ^ Get_Td(2, (s2 >>  8) & 0xff) ^ Get_Td(3, s1 & 0xff) ^ rk[28];
    t1 = Get_Td(0, s1 >> 24) ^ Get_Td(1, (s0 >> 16) & 0xff) ^ Get_Td(2, (s3 >>  8) & 0xff) ^ Get_Td(3, s2 & 0xff) ^ rk[29];
    t2 = Get_Td(0, s2 >> 24) ^ Get_Td(1, (s1 >> 16) & 0xff) ^ Get_Td(2, (s0 >>  8) & 0xff) ^ Get_Td(3, s3 & 0xff) ^ rk[30];
    t3 = Get_Td(0, s3 >> 24) ^ Get_Td(1, (s2 >> 16) & 0xff) ^ Get_Td(2, (s1 >>  8) & 0xff) ^ Get_Td(3, s0 & 0xff) ^ rk[31];
    /* round 8: */
    s0 = Get_Td(0, t0 >> 24) ^ Get_Td(1, (t3 >> 16) & 0xff) ^ Get_Td(2, (t2 >>  8) & 0xff) ^ Get_Td(3, t1 & 0xff) ^ rk[32];
    s1 = Get_Td(0, t1 >> 24) ^ Get_Td(1, (t0 >> 16) & 0xff) ^ Get_Td(2, (t3 >>  8) & 0xff) ^ Get_Td(3, t2 & 0xff) ^ rk[33];
    s2 = Get_Td(0, t2 >> 24) ^ Get_Td(1, (t1 >> 16) & 0xff) ^ Get_Td(2, (t0 >>  8) & 0xff) ^ Get_Td(3, t3 & 0xff) ^ rk[34];
    s3 = Get_Td(0, t3 >> 24) ^ Get_Td(1, (t2 >> 16) & 0xff) ^ Get_Td(2, (t1 >>  8) & 0xff) ^ Get_Td(3, t0 & 0xff) ^ rk[35];
    /* round 9: */
    t0 = Get_Td(0, s0 >> 24) ^ Get_Td(1, (s3 >> 16) & 0xff) ^ Get_Td(2, (s2 >>  8) & 0xff) ^ Get_Td(3, s1 & 0xff) ^ rk[36];
    t1 = Get_Td(0, s1 >> 24) ^ Get_Td(1, (s0 >> 16) & 0xff) ^ Get_Td(2, (s3 >>  8) & 0xff) ^ Get_Td(3, s2 & 0xff) ^ rk[37];
    t2 = Get_Td(0, s2 >> 24) ^ Get_Td(1, (s1 >> 16) & 0xff) ^ Get_Td(2, (s0 >>  8) & 0xff) ^ Get_Td(3, s3 & 0xff) ^ rk[38];
    t3 = Get_Td(0, s3 >> 24) ^ Get_Td(1, (s2 >> 16) & 0xff) ^ Get_Td(2, (s1 >>  8) & 0xff) ^ Get_Td(3, s0 & 0xff) ^ rk[39];
    if (Nr > 10)
    {
        /* round 10: */
        s0 = Get_Td(0, t0 >> 24) ^ Get_Td(1, (t3 >> 16) & 0xff) ^ Get_Td(2, (t2 >>  8) & 0xff) ^ Get_Td(3, t1 & 0xff) ^ rk[40];
        s1 = Get_Td(0, t1 >> 24) ^ Get_Td(1, (t0 >> 16) & 0xff) ^ Get_Td(2, (t3 >>  8) & 0xff) ^ Get_Td(3, t2 & 0xff) ^ rk[41];
        s2 = Get_Td(0, t2 >> 24) ^ Get_Td(1, (t1 >> 16) & 0xff) ^ Get_Td(2, (t0 >>  8) & 0xff) ^ Get_Td(3, t3 & 0xff) ^ rk[42];
        s3 = Get_Td(0, t3 >> 24) ^ Get_Td(1, (t2 >> 16) & 0xff) ^ Get_Td(2, (t1 >>  8) & 0xff) ^ Get_Td(3, t0 & 0xff) ^ rk[43];
        /* round 11: */
        t0 = Get_Td(0, s0 >> 24) ^ Get_Td(1, (s3 >> 16) & 0xff) ^ Get_Td(2, (s2 >>  8) & 0xff) ^ Get_Td(3, s1 & 0xff) ^ rk[44];
        t1 = Get_Td(0, s1 >> 24) ^ Get_Td(1, (s0 >> 16) & 0xff) ^ Get_Td(2, (s3 >>  8) & 0xff) ^ Get_Td(3, s2 & 0xff) ^ rk[45];
        t2 = Get_Td(0, s2 >> 24) ^ Get_Td(1, (s1 >> 16) & 0xff) ^ Get_Td(2, (s0 >>  8) & 0xff) ^ Get_Td(3, s3 & 0xff) ^ rk[46];
        t3 = Get_Td(0, s3 >> 24) ^ Get_Td(1, (s2 >> 16) & 0xff) ^ Get_Td(2, (s1 >>  8) & 0xff) ^ Get_Td(3, s0 & 0xff) ^ rk[47];
        if (Nr > 12) 
        {
            /* round 12: */
            s0 = Get_Td(0, t0 >> 24) ^ Get_Td(1, (t3 >> 16) & 0xff) ^ Get_Td(2, (t2 >>  8) & 0xff) ^ Get_Td(3, t1 & 0xff) ^ rk[48];
            s1 = Get_Td(0, t1 >> 24) ^ Get_Td(1, (t0 >> 16) & 0xff) ^ Get_Td(2, (t3 >>  8) & 0xff) ^ Get_Td(3, t2 & 0xff) ^ rk[49];
            s2 = Get_Td(0, t2 >> 24) ^ Get_Td(1, (t1 >> 16) & 0xff) ^ Get_Td(2, (t0 >>  8) & 0xff) ^ Get_Td(3, t3 & 0xff) ^ rk[50];
            s3 = Get_Td(0, t3 >> 24) ^ Get_Td(1, (t2 >> 16) & 0xff) ^ Get_Td(2, (t1 >>  8) & 0xff) ^ Get_Td(3, t0 & 0xff) ^ rk[51];
            /* round 13: */
            t0 = Get_Td(0, s0 >> 24) ^ Get_Td(1, (s3 >> 16) & 0xff) ^ Get_Td(2, (s2 >>  8) & 0xff) ^ Get_Td(3, s1 & 0xff) ^ rk[52];
            t1 = Get_Td(0, s1 >> 24) ^ Get_Td(1, (s0 >> 16) & 0xff) ^ Get_Td(2, (s3 >>  8) & 0xff) ^ Get_Td(3, s2 & 0xff) ^ rk[53];
            t2 = Get_Td(0, s2 >> 24) ^ Get_Td(1, (s1 >> 16) & 0xff) ^ Get_Td(2, (s0 >>  8) & 0xff) ^ Get_Td(3, s3 & 0xff) ^ rk[54];
            t3 = Get_Td(0, s3 >> 24) ^ Get_Td(1, (s2 >> 16) & 0xff) ^ Get_Td(2, (s1 >>  8) & 0xff) ^ Get_Td(3, s0 & 0xff) ^ rk[55];
        }
    }
	rk += Nr << 2;
#else  /* !FULL_UNROLL */
    /*
     * Nr - 1 full rounds:
     */
    r = Nr >> 1;
    for (;;) 
    {
        t0 =
            Get_Td(0, (s0 >> 24)        ) ^
            Get_Td(1, (s3 >> 16) & 0xff ) ^
            Get_Td(2, (s2 >>  8) & 0xff ) ^
            Get_Td(3, (s1      ) & 0xff ) ^
            rk[4];
        t1 =
            Get_Td(0, (s1 >> 24)        ) ^
            Get_Td(1, (s0 >> 16) & 0xff ) ^
            Get_Td(2, (s3 >>  8) & 0xff ) ^
            Get_Td(3, (s2      ) & 0xff ) ^
            rk[5];
        t2 =
            Get_Td(0, (s2 >> 24)        ) ^
            Get_Td(1, (s1 >> 16) & 0xff ) ^
            Get_Td(2, (s0 >>  8) & 0xff ) ^
            Get_Td(3, (s3      ) & 0xff ) ^
            rk[6];
        t3 =
            Get_Td(0, (s3 >> 24)        ) ^
            Get_Td(1, (s2 >> 16) & 0xff ) ^
            Get_Td(2, (s1 >>  8) & 0xff ) ^
            Get_Td(3, (s0      ) & 0xff ) ^
            rk[7];

        rk += 8;
        if (--r == 0) 
        {
            break;
        }

        s0 =
            Get_Td(0, (t0 >> 24)       ) ^
            Get_Td(1, (t3 >> 16) & 0xff) ^
            Get_Td(2, (t2 >>  8) & 0xff) ^
            Get_Td(3, (t1      ) & 0xff) ^
            rk[0];
        s1 =
            Get_Td(0, (t1 >> 24)       ) ^
            Get_Td(1, (t0 >> 16) & 0xff) ^
            Get_Td(2, (t3 >>  8) & 0xff) ^
            Get_Td(3, (t2      ) & 0xff) ^
            rk[1];
        s2 =
            Get_Td(0, (t2 >> 24)       ) ^
            Get_Td(1, (t1 >> 16) & 0xff) ^
            Get_Td(2, (t0 >>  8) & 0xff) ^
            Get_Td(3, (t3      ) & 0xff) ^
            rk[2];
        s3 =
            Get_Td(0, (t3 >> 24)       ) ^
            Get_Td(1, (t2 >> 16) & 0xff) ^
            Get_Td(2, (t1 >>  8) & 0xff) ^
            Get_Td(3, (t0      ) & 0xff) ^
            rk[3];
    }
#endif /* ?FULL_UNROLL */
    /*
	 * apply last round and
	 * map cipher state to byte array block:
	 */
   	s0 =
   		(Get_Td(4, (t0 >> 24)       ) & 0xff000000) ^
   		(Get_Td(4, (t3 >> 16) & 0xff) & 0x00ff0000) ^
   		(Get_Td(4, (t2 >>  8) & 0xff) & 0x0000ff00) ^
   		(Get_Td(4, (t1      ) & 0xff) & 0x000000ff) ^
   		rk[0];
	PUTuint32_t(pt     , s0);
   	s1 =
   		(Get_Td(4, (t1 >> 24)       ) & 0xff000000) ^
   		(Get_Td(4, (t0 >> 16) & 0xff) & 0x00ff0000) ^
   		(Get_Td(4, (t3 >>  8) & 0xff) & 0x0000ff00) ^
   		(Get_Td(4, (t2      ) & 0xff) & 0x000000ff) ^
   		rk[1];
	PUTuint32_t(pt +  4, s1);
   	s2 =
   		(Get_Td(4, (t2 >> 24)       ) & 0xff000000) ^
   		(Get_Td(4, (t1 >> 16) & 0xff) & 0x00ff0000) ^
   		(Get_Td(4, (t0 >>  8) & 0xff) & 0x0000ff00) ^
   		(Get_Td(4, (t3      ) & 0xff) & 0x000000ff) ^
   		rk[2];
	PUTuint32_t(pt +  8, s2);
   	s3 =
   		(Get_Td(4, (t3 >> 24)       ) & 0xff000000) ^
   		(Get_Td(4, (t2 >> 16) & 0xff) & 0x00ff0000) ^
   		(Get_Td(4, (t1 >>  8) & 0xff) & 0x0000ff00) ^
   		(Get_Td(4, (t0      ) & 0xff) & 0x000000ff) ^
   		rk[3];
	PUTuint32_t(pt + 12, s3);
}
/* clang-format on */

#ifdef INTERMEDIATE_VALUE_KAT

/* clang-format off */
void rijndaelEncryptRound( const uint32_t rk[/*4*(Nr + 1)*/], int Nr,
                           uint8_t block[16], int rounds )
{
    int r;
    uint32_t s0, s1, s2, s3, t0, t1, t2, t3;

    /*
	 * map byte array block to cipher state
	 * and add initial round key:
	 */
	s0 = GETuint32_t(block     ) ^ rk[0];
	s1 = GETuint32_t(block +  4) ^ rk[1];
	s2 = GETuint32_t(block +  8) ^ rk[2];
	s3 = GETuint32_t(block + 12) ^ rk[3];
    rk += 4;

    /*
	 * Nr - 1 full rounds:
	 */
	for (r = (rounds < Nr ? rounds : Nr - 1); r > 0; r--) 
        {
		t0 =
			Get_Te(0, (s0 >> 24)       ) ^
			Get_Te(1, (s1 >> 16) & 0xff) ^
			Get_Te(2, (s2 >>  8) & 0xff) ^
			Get_Te(3, (s3      ) & 0xff) ^
			rk[0];
		t1 =
			Get_Te(0, (s1 >> 24)       ) ^
			Get_Te(1, (s2 >> 16) & 0xff) ^
			Get_Te(2, (s3 >>  8) & 0xff) ^
			Get_Te(3, (s0      ) & 0xff) ^
			rk[1];
		t2 =
			Get_Te(0, (s2 >> 24)       ) ^
			Get_Te(1, (s3 >> 16) & 0xff) ^
			Get_Te(2, (s0 >>  8) & 0xff) ^
			Get_Te(3, (s1      ) & 0xff) ^
			rk[2];
		t3 =
			Get_Te(0, (s3 >> 24)       ) ^
			Get_Te(1, (s0 >> 16) & 0xff) ^
			Get_Te(2, (s1 >>  8) & 0xff) ^
			Get_Te(3, (s2      ) & 0xff) ^
			rk[3];

		s0 = t0;
		s1 = t1;
		s2 = t2;
		s3 = t3;
		rk += 4;

    }

    /*
	 * apply last round and
	 * map cipher state to byte array block:
	 */
	if (rounds == Nr) 
        {
    	t0 =
    		(Get_Te(4, (s0 >> 24)       ) & 0xff000000) ^
    		(Get_Te(4, (s1 >> 16) & 0xff) & 0x00ff0000) ^
    		(Get_Te(4, (s2 >>  8) & 0xff) & 0x0000ff00) ^
    		(Get_Te(4, (s3      ) & 0xff) & 0x000000ff) ^
    		rk[0];
    	t1 =
    		(Get_Te(4, (s1 >> 24)       ) & 0xff000000) ^
    		(Get_Te(4, (s2 >> 16) & 0xff) & 0x00ff0000) ^
    		(Get_Te(4, (s3 >>  8) & 0xff) & 0x0000ff00) ^
    		(Get_Te(4, (s0      ) & 0xff) & 0x000000ff) ^
    		rk[1];
    	t2 =
    		(Get_Te(4, (s2 >> 24)       ) & 0xff000000) ^
    		(Get_Te(4, (s3 >> 16) & 0xff) & 0x00ff0000) ^
    		(Get_Te(4, (s0 >>  8) & 0xff) & 0x0000ff00) ^
    		(Get_Te(4, (s1      ) & 0xff) & 0x000000ff) ^
    		rk[2];
    	t3 =
    		(Get_Te(4, (s3 >> 24)       ) & 0xff000000) ^
    		(Get_Te(4, (s0 >> 16) & 0xff) & 0x00ff0000) ^
    		(Get_Te(4, (s1 >>  8) & 0xff) & 0x0000ff00) ^
    		(Get_Te(4, (s2      ) & 0xff) & 0x000000ff) ^
    		rk[3];
		
		s0 = t0;
		s1 = t1;
		s2 = t2;
		s3 = t3;
	}

	PUTuint32_t(block     , s0);
	PUTuint32_t(block +  4, s1);
	PUTuint32_t(block +  8, s2);
	PUTuint32_t(block + 12, s3);
}
/* clang-format on */

/* clang-format off */
void rijndaelDecryptRound( const uint32_t rk[/*4*(Nr + 1)*/], int Nr,
                           uint8_t block[16], int rounds )
{
    int r;
    uint32_t s0, s1, s2, s3, t0, t1, t2, t3;

    /*
	 * map byte array block to cipher state
	 * and add initial round key:
	 */
	s0 = GETuint32_t(block     ) ^ rk[0];
	s1 = GETuint32_t(block +  4) ^ rk[1];
	s2 = GETuint32_t(block +  8) ^ rk[2];
	s3 = GETuint32_t(block + 12) ^ rk[3];
    rk += 4;

    /*
	 * Nr - 1 full rounds:
	 */
	for (r = (rounds < Nr ? rounds : Nr) - 1; r > 0; r--) 
        {
		t0 =
			Get_Td(0, (s0 >> 24)       ) ^
			Get_Td(1, (s3 >> 16) & 0xff ) ^
			Get_Td(2, (s2 >>  8) & 0xff ) ^
			Get_Td(3, (s1      ) & 0xff ) ^
			rk[0];
		t1 =
			Get_Td(0, (s1 >> 24)       ) ^
			Get_Td(1, (s0 >> 16) & 0xff ) ^
			Get_Td(2, (s3 >>  8) & 0xff ) ^
			Get_Td(3, (s2      ) & 0xff ) ^
			rk[1];
		t2 =
			Get_Td(0, (s2 >> 24)       ) ^
			Get_Td(1, (s1 >> 16) & 0xff ) ^
			Get_Td(2, (s0 >>  8) & 0xff ) ^
			Get_Td(3, (s3      ) & 0xff ) ^
			rk[2];
		t3 =
			Get_Td(0, (s3 >> 24)       ) ^
			Get_Td(1, (s2 >> 16) & 0xff ) ^
			Get_Td(2, (s1 >>  8) & 0xff ) ^
			Get_Td(3, (s0      ) & 0xff ) ^
			rk[3];

		s0 = t0;
		s1 = t1;
		s2 = t2;
		s3 = t3;
		rk += 4;

    }

    /*
	 * complete the last round and
	 * map cipher state to byte array block:
	 */
	t0 =
		(Get_Td(4, (s0 >> 24)       ) & 0xff000000) ^
		(Get_Td(4, (s3 >> 16) & 0xff) & 0x00ff0000) ^
		(Get_Td(4, (s2 >>  8) & 0xff) & 0x0000ff00) ^
		(Get_Td(4, (s1      ) & 0xff) & 0x000000ff);
	t1 =
		(Get_Td(4, (s1 >> 24)       ) & 0xff000000) ^
		(Get_Td(4, (s0 >> 16) & 0xff) & 0x00ff0000) ^
		(Get_Td(4, (s3 >>  8) & 0xff) & 0x0000ff00) ^
		(Get_Td(4, (s2      ) & 0xff) & 0x000000ff);
	t2 =
		(Get_Td(4, (s2 >> 24)       ) & 0xff000000) ^
		(Get_Td(4, (s1 >> 16) & 0xff) & 0x00ff0000) ^
		(Get_Td(4, (s0 >>  8) & 0xff) & 0x0000ff00) ^
		(Get_Td(4, (s3      ) & 0xff) & 0x000000ff);
	t3 =
		(Get_Td(4, (s3 >> 24)       ) & 0xff000000) ^
		(Get_Td(4, (s2 >> 16) & 0xff) & 0x00ff0000) ^
		(Get_Td(4, (s1 >>  8) & 0xff) & 0x0000ff00) ^
		(Get_Td(4, (s0      ) & 0xff) & 0x000000ff);

	if (rounds == Nr) 
        {
	    t0 ^= rk[0];
	    t1 ^= rk[1];
	    t2 ^= rk[2];
	    t3 ^= rk[3];
	}

	PUTuint32_t(block     , t0);
	PUTuint32_t(block +  4, t1);
	PUTuint32_t(block +  8, t2);
	PUTuint32_t(block + 12, t3);
}
/* clang-format on */

#endif /* INTERMEDIATE_VALUE_KAT */
