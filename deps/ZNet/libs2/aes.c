
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

This is an implementation of the AES128 algorithm, specifically ECB and CBC
mode.

The implementation is verified against the test vectors in:
  National Institute of Standards and Technology Special Publication 800-38A
2001 ED

ECB-AES128
----------

  plain-text:
    6bc1bee22e409f96e93d7e117393172a
    ae2d8a571e03ac9c9eb76fac45af8e51
    30c81c46a35ce411e5fbc1191a0a52ef
    f69f2445df4f9b17ad2b417be66c3710

  key:
    2b7e151628aed2a6abf7158809cf4f3c

  resulting cipher
    3ad77bb40d7a3660a89ecaf32466ef97
    f5d3d58503b9699de785895a96fdbaaf
    43b1cd7f598ece23881b00e3ed030688
    7b0c785e27e8ad3f8223207104725dd4


NOTE:   String length must be evenly divisible by 16byte (str_len % 16 == 0)
        You should pad the end of the string with zeros if this is not the case.

*/

/*****************************************************************************/
/* Includes:                                                                 */
/*****************************************************************************/
#include <stdint.h>
#include <string.h>  // CBC mode, for memset
#include "aes.h"

/*****************************************************************************/
/* Defines:                                                                  */
/*****************************************************************************/
// The number of columns comprising a state in AES. This is a constant in AES.
// Value=4
#define Nb 4
// The number of 32 bit words in a key.
#define Nk 4
// Key length in bytes [128 bit]
#define KEYLEN 16
// The number of rounds in AES Cipher.
#define Nr 10

// jcallan@github points out that declaring Multiply as a function
// reduces code size considerably with the Keil ARM compiler.
// See this link for more information:
// https://github.com/kokke/tiny-AES128-C/pull/3
#ifndef MULTIPLY_AS_A_FUNCTION
#define MULTIPLY_AS_A_FUNCTION 0
#endif

/*****************************************************************************/
/* Private variables:                                                        */
/*****************************************************************************/
// state - array holding the intermediate results during decryption.
typedef uint8_t state_t[4][4];
static state_t* state;

// The array that stores the round keys.
static uint8_t RoundKey[176];

// The Key input to the AES Program
static const uint8_t* Key;

#if defined( CBC ) && CBC
// Initial Vector used only for CBC mode
static uint8_t* Iv;
#endif

// The round constant word array, Rcon[i], contains the values given by
// x to th e power (i-1) being powers of x (x is denoted as {02}) in the field
// GF(2^8) Note that i starts at 1, not 0).
static const uint8_t Rcon[16] = { 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40,
                                  0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a };

extern uint32_t Get_Te( uint8_t Te_N, uint32_t index );
extern const uint32_t znTd4[256];
/*****************************************************************************/
/* Private functions:                                                        */
/*****************************************************************************/
uint8_t getSBoxValue( uint8_t num ) {
    return (uint8_t)Get_Te( 1, num );
}

uint8_t getSBoxInvert( uint8_t num ) {
    return (uint8_t)znTd4[num];
}

// This function produces Nb(Nr+1) round keys. The round keys are used in each
// round to decrypt the states.
static void KeyExpansion( void ) {
    uint32_t i, j, k;
    uint8_t tempa[4];  // Used for the column/row operations

    // The first round key is the key itself.
    for( i = 0; i < Nk; ++i ) {
        RoundKey[( i * 4 ) + 0] = Key[( i * 4 ) + 0];
        RoundKey[( i * 4 ) + 1] = Key[( i * 4 ) + 1];
        RoundKey[( i * 4 ) + 2] = Key[( i * 4 ) + 2];
        RoundKey[( i * 4 ) + 3] = Key[( i * 4 ) + 3];
    }

    // All other round keys are found from the previous round keys.
    for( ; ( i < ( Nb * ( Nr + 1 ) ) ); ++i ) {
        for( j = 0; j < 4; ++j ) {
            tempa[j] = RoundKey[( i - 1 ) * 4 + j];
        }
        if( i % Nk == 0 ) {
            // This function rotates the 4 bytes in a word to the left once.
            // [a0,a1,a2,a3] becomes [a1,a2,a3,a0]

            // Function RotWord()
            {
                k = tempa[0];
                tempa[0] = tempa[1];
                tempa[1] = tempa[2];
                tempa[2] = tempa[3];
                tempa[3] = k;
            }

            // SubWord() is a function that takes a four-byte input word and
            // applies the S-box to each of the four bytes to produce an output
            // word.

            // Function Subword()
            {
                tempa[0] = getSBoxValue( tempa[0] );
                tempa[1] = getSBoxValue( tempa[1] );
                tempa[2] = getSBoxValue( tempa[2] );
                tempa[3] = getSBoxValue( tempa[3] );
            }

            tempa[0] = tempa[0] ^ Rcon[i / Nk];
        }
#if Nk > 6
        else if( Nk > 6 && i % Nk == 4 ) {
            // Function Subword()
            {
                tempa[0] = getSBoxValue( tempa[0] );
                tempa[1] = getSBoxValue( tempa[1] );
                tempa[2] = getSBoxValue( tempa[2] );
                tempa[3] = getSBoxValue( tempa[3] );
            }
        }
#endif
        RoundKey[i * 4 + 0] = RoundKey[( i - Nk ) * 4 + 0] ^ tempa[0];
        RoundKey[i * 4 + 1] = RoundKey[( i - Nk ) * 4 + 1] ^ tempa[1];
        RoundKey[i * 4 + 2] = RoundKey[( i - Nk ) * 4 + 2] ^ tempa[2];
        RoundKey[i * 4 + 3] = RoundKey[( i - Nk ) * 4 + 3] ^ tempa[3];
    }
}

// This function adds the round key to state.
// The round key is added to the state by an XOR function.
static void AddRoundKey( uint8_t round ) {
    uint8_t i, j;
    for( i = 0; i < 4; ++i ) {
        for( j = 0; j < 4; ++j ) {
            ( *state )[i][j] ^= RoundKey[round * Nb * 4 + i * Nb + j];
        }
    }
}

// The SubBytes Function Substitutes the values in the
// state matrix with values in an S-box.
void SubBytes( void ) {
    uint8_t i, j;
    for( i = 0; i < 4; ++i ) {
        for( j = 0; j < 4; ++j ) {
            ( *state )[j][i] = getSBoxValue( ( *state )[j][i] );
        }
    }
}

// The ShiftRows() function shifts the rows in the state to the left.
// Each row is shifted with different offset.
// Offset = Row number. So the first row is not shifted.
void ShiftRows( void ) {
    uint8_t temp;

    // Rotate first row 1 columns to left
    temp = ( *state )[0][1];
    ( *state )[0][1] = ( *state )[1][1];
    ( *state )[1][1] = ( *state )[2][1];
    ( *state )[2][1] = ( *state )[3][1];
    ( *state )[3][1] = temp;

    // Rotate second row 2 columns to left
    temp = ( *state )[0][2];
    ( *state )[0][2] = ( *state )[2][2];
    ( *state )[2][2] = temp;

    temp = ( *state )[1][2];
    ( *state )[1][2] = ( *state )[3][2];
    ( *state )[3][2] = temp;

    // Rotate third row 3 columns to left
    temp = ( *state )[0][3];
    ( *state )[0][3] = ( *state )[3][3];
    ( *state )[3][3] = ( *state )[2][3];
    ( *state )[2][3] = ( *state )[1][3];
    ( *state )[1][3] = temp;
}

uint8_t xtime( uint8_t x ) {
    return ( ( x << 1 ) ^ ( ( ( x >> 7 ) & 1 ) * 0x1b ) );
}

// MixColumns function mixes the columns of the state matrix
/* clang-format off */
void MixColumns( void )
{
  uint8_t i;
  uint8_t Tmp,Tm,t;
  for(i = 0; i < 4; ++i)
  {
    t   = (*state)[i][0];
    Tmp = (*state)[i][0] ^ (*state)[i][1] ^ (*state)[i][2] ^ (*state)[i][3] ;
    Tm  = (*state)[i][0] ^ (*state)[i][1] ; Tm = xtime(Tm);  (*state)[i][0] ^= Tm ^ Tmp ;
    Tm  = (*state)[i][1] ^ (*state)[i][2] ; Tm = xtime(Tm);  (*state)[i][1] ^= Tm ^ Tmp ;
    Tm  = (*state)[i][2] ^ (*state)[i][3] ; Tm = xtime(Tm);  (*state)[i][2] ^= Tm ^ Tmp ;
    Tm  = (*state)[i][3] ^ t ;        Tm = xtime(Tm);  (*state)[i][3] ^= Tm ^ Tmp ;
  }
}
/* clang-format on */

// Multiply is used to multiply numbers in the field GF(2^8)
#if MULTIPLY_AS_A_FUNCTION
/* clang-format off */
static uint8_t Multiply( uint8_t x, uint8_t y )
{
  return (((y & 1) * x) ^
       ((y>>1 & 1) * xtime(x)) ^
       ((y>>2 & 1) * xtime(xtime(x))) ^
       ((y>>3 & 1) * xtime(xtime(xtime(x)))) ^
       ((y>>4 & 1) * xtime(xtime(xtime(xtime(x))))));
  }
/* clang-format on */
#else
#define Multiply( x, y )                                                                               \
    ( ( ( y & 1 ) * x ) ^ ( ( y >> 1 & 1 ) * xtime( x ) ) ^ ( ( y >> 2 & 1 ) * xtime( xtime( x ) ) ) ^ \
      ( ( y >> 3 & 1 ) * xtime( xtime( xtime( x ) ) ) ) ^ ( ( y >> 4 & 1 ) * xtime( xtime( xtime( xtime( x ) ) ) ) ) )

#endif

// MixColumns function mixes the columns of the state matrix.
// The method used to multiply may be difficult to understand for the
// inexperienced. Please use the references to gain more information.
/* clang-format off */
void InvMixColumns( void )
{
  int i;
  uint8_t a,b,c,d;
  for(i=0;i<4;++i)
  {
    a = (*state)[i][0];
    b = (*state)[i][1];
    c = (*state)[i][2];
    d = (*state)[i][3];

    (*state)[i][0] = Multiply(a, 0x0e) ^ Multiply(b, 0x0b) ^ Multiply(c, 0x0d) ^ Multiply(d, 0x09);
    (*state)[i][1] = Multiply(a, 0x09) ^ Multiply(b, 0x0e) ^ Multiply(c, 0x0b) ^ Multiply(d, 0x0d);
    (*state)[i][2] = Multiply(a, 0x0d) ^ Multiply(b, 0x09) ^ Multiply(c, 0x0e) ^ Multiply(d, 0x0b);
    (*state)[i][3] = Multiply(a, 0x0b) ^ Multiply(b, 0x0d) ^ Multiply(c, 0x09) ^ Multiply(d, 0x0e);
  }
}
/* clang-format on */

// The SubBytes Function Substitutes the values in the
// state matrix with values in an S-box.
void InvSubBytes( void ) {
    uint8_t i, j;
    for( i = 0; i < 4; ++i ) {
        for( j = 0; j < 4; ++j ) {
            ( *state )[j][i] = getSBoxInvert( ( *state )[j][i] );
        }
    }
}

void InvShiftRows( void ) {
    uint8_t temp;

    // Rotate first row 1 columns to right
    temp = ( *state )[3][1];
    ( *state )[3][1] = ( *state )[2][1];
    ( *state )[2][1] = ( *state )[1][1];
    ( *state )[1][1] = ( *state )[0][1];
    ( *state )[0][1] = temp;

    // Rotate second row 2 columns to right
    temp = ( *state )[0][2];
    ( *state )[0][2] = ( *state )[2][2];
    ( *state )[2][2] = temp;

    temp = ( *state )[1][2];
    ( *state )[1][2] = ( *state )[3][2];
    ( *state )[3][2] = temp;

    // Rotate third row 3 columns to right
    temp = ( *state )[0][3];
    ( *state )[0][3] = ( *state )[1][3];
    ( *state )[1][3] = ( *state )[2][3];
    ( *state )[2][3] = ( *state )[3][3];
    ( *state )[3][3] = temp;
}

// Cipher is the main function that encrypts the PlainText.
void Cipher( void ) {
    uint8_t round = 0;

    // Add the First round key to the state before starting the rounds.
    AddRoundKey( 0 );

    // There will be Nr rounds.
    // The first Nr-1 rounds are identical.
    // These Nr-1 rounds are executed in the loop below.
    for( round = 1; round < Nr; ++round ) {
        SubBytes();
        ShiftRows();
        MixColumns();
        AddRoundKey( round );
    }

    // The last round is given below.
    // The MixColumns function is not here in the last round.
    SubBytes();
    ShiftRows();
    AddRoundKey( Nr );
}

void InvCipher( void ) {
    uint8_t round = 0;

    // Add the First round key to the state before starting the rounds.
    AddRoundKey( Nr );

    // There will be Nr rounds.
    // The first Nr-1 rounds are identical.
    // These Nr-1 rounds are executed in the loop below.
    for( round = Nr - 1; round > 0; round-- ) {
        InvShiftRows();
        InvSubBytes();
        AddRoundKey( round );
        InvMixColumns();
    }

    // The last round is given below.
    // The MixColumns function is not here in the last round.
    InvShiftRows();
    InvSubBytes();
    AddRoundKey( 0 );
}

void BlockCopy( uint8_t* output, uint8_t* input ) {
    uint8_t i;
    for( i = 0; i < KEYLEN; ++i ) {
        output[i] = input[i];
    }
}

/*****************************************************************************/
/* Public functions:                                                         */
/*****************************************************************************/
#if defined( ECB ) && ECB

void AES128_ECB_encrypt( uint8_t* input, const uint8_t* key, uint8_t* output ) {
    // Copy input to output, and work in-memory on output
    BlockCopy( output, input );
    state = (state_t*)output;

    Key = key;
    KeyExpansion();

    // The next function call encrypts the PlainText with the Key using AES
    // algorithm.
    Cipher();
}

void AES128_ECB_decrypt( uint8_t* input, const uint8_t* key, uint8_t* output ) {
    // Copy input to output, and work in-memory on output
    BlockCopy( output, input );
    state = (state_t*)output;

    // The KeyExpansion routine must be called before encryption.
    Key = key;
    KeyExpansion();

    InvCipher();
}

#endif  // #if defined(ECB) && ECB

#if defined( CBC ) && CBC

void XorWithIv( uint8_t* buf ) {
    uint8_t i;
    for( i = 0; i < KEYLEN; ++i ) {
        buf[i] ^= Iv[i];
    }
}

void AES128_CBC_encrypt_buffer( uint8_t* output,
                                uint8_t* input,
                                uint32_t length,
                                const uint8_t* key,
                                const uint8_t* iv ) {
    uintptr_t i;
    uint8_t remainders = length % KEYLEN; /* Remaining bytes in the last non-full block */

    BlockCopy( output, input );
    state = (state_t*)output;

    // Skip the key expansion if key is passed as 0
    if( 0 != key ) {
        Key = key;
        KeyExpansion();
    }

    if( iv != 0 ) {
        Iv = (uint8_t*)iv;
    }

    for( i = 0; i < length; i += KEYLEN ) {
        XorWithIv( input );
        BlockCopy( output, input );
        state = (state_t*)output;
        Cipher();
        Iv = output;
        input += KEYLEN;
        output += KEYLEN;
    }

    if( remainders ) {
        BlockCopy( output, input );
        memset( output + remainders, 0, KEYLEN - remainders ); /* add 0-padding */
        state = (state_t*)output;
        Cipher();
    }
}

void AES128_CBC_decrypt_buffer( uint8_t* output,
                                uint8_t* input,
                                uint32_t length,
                                const uint8_t* key,
                                const uint8_t* iv ) {
    uintptr_t i;
    uint8_t remainders = length % KEYLEN; /* Remaining bytes in the last non-full block */

    BlockCopy( output, input );
    state = (state_t*)output;

    // Skip the key expansion if key is passed as 0
    if( 0 != key ) {
        Key = key;
        KeyExpansion();
    }

    // If iv is passed as 0, we continue to encrypt without re-setting the Iv
    if( iv != 0 ) {
        Iv = (uint8_t*)iv;
    }

    for( i = 0; i < length; i += KEYLEN ) {
        BlockCopy( output, input );
        state = (state_t*)output;
        InvCipher();
        XorWithIv( output );
        Iv = input;
        input += KEYLEN;
        output += KEYLEN;
    }

    if( remainders ) {
        BlockCopy( output, input );
        memset( output + remainders, 0, KEYLEN - remainders ); /* add 0-padding */
        state = (state_t*)output;
        InvCipher();
    }
}

#endif  // #if defined(CBC) && CBC
