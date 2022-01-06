// This file is part of E-Key project - hardware/software platform to support certain cryptography applications.
// Copyright [2021] Secoh  https://github.com/Secoh/EKey
// This application uses SKLib: https://github.com/Secoh/SKLib
//
// Licensed under the GNU General Public License, Version 3 or later. See: https://www.gnu.org/licenses/
// You may not use this file except in compliance with the License. Any derivative work must retain the License.
// Software is distributed on "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//

#include <iostream>

#include <SKLib/sklib.hpp>
#include "ekey-model.h"

// Shared IO settings, common IO functions, KEY, BLOCK sizes, counts
#include "interface.h"

// I/O with hardware (or model), and with serial line
#include "hardware_model.h"

static_assert(KEY_SIZE == sklib::supplement::bits_data_mask<uint8_t>() + 1, "Key size and uint8_t range must be the same");
static_assert(KEY_COUNT <= sklib::supplement::bits_data_mask<uint8_t>() + 1, "Key count must be addressable by uint8_t");

static constexpr unsigned BUFFER_SIZE = 1536;

// transformation of received 
uint8_t KeyPermutation[KEY_SIZE];
uint8_t KeyRedirect[KEY_COUNT];
uint8_t BUFFER[std::max({ BUFFER_SIZE, Interface::read_buffer_size(KEY_SIZE), Interface::read_buffer_size(BLOCK_SIZE) })];

// given the current Permutation and Hardware storage (received by hdw_get_key_ptr() function)

bool compare_key_is_pattern_A_less_than_B(const uint8_t* pattern_A, uint8_t idx_B)
{
    uint8_t* key_B = hdw_get_key_ptr(idx_B);

    for (unsigned k = 0; k < KEY_SIZE; k++)
    {
        if (pattern_A[KeyPermutation[k]] < key_B[KeyPermutation[k]]) return true;
    }

    return false;
}

bool compare_keys_is_A_less_than_B(uint8_t idx_A, uint8_t idx_B)
{
    return compare_key_is_pattern_A_less_than_B(hdw_get_key_ptr(idx_A), idx_B);
}

void recalculate_key_sorting()
{
    for (unsigned k = 0; k < KEY_COUNT; k++) KeyRedirect[k] = k;
    std::make_heap(KeyRedirect, KeyRedirect + KEY_COUNT, compare_keys_is_A_less_than_B);
    std::sort_heap(KeyRedirect, KeyRedirect + KEY_COUNT, compare_keys_is_A_less_than_B);
}

// 0..KEY_COUNT-1 => index of the Key; payload is offset by KEY_SIZE
// return NEGATIVE if not found
int find_key_index(uint8_t* pattern)
{
    int idx_K = (int)(std::upper_bound(KeyPermutation, KeyPermutation + KEY_COUNT, pattern, compare_key_is_pattern_A_less_than_B) - KeyPermutation);
    return ((idx_K == KEY_COUNT) ? -1 : idx_K);
}

/*
* input/output:
*   base64= string encoding 256 bytes + 4 byte CRC32 => find the match in table, if format and CRC are correct, and key is present, return response, another 256 bytes + their 4 byte CRC32
*   write=base64= 1 byte address, 256 bytes key, 256 bytes response, 4 byte CRC32 of the transmission => raw write into the key table (verifies CRC)
*   erase= => delete all entries in the table
*   prime=base64= 256 bytes vector + 4 byte CRC32 => loads precalculated 256 byte permutation vector for use (verified CRC), replacing old one if any
*   noise= => calculates and prints 256 random bytes + 4 byte CRC32
*   put=base64= 2 bytes address, 1024 bytes record, 4 bytes CRC32 => write to "file" memory (vierified CRC), granularity is 1 kb.
*   get=base64= 2 bytes address, 4 bytes CRC32 => read from address 1024 bytes and return with 4 bytes CRC
*   status= => return error status, including CRC test
*/

/* //sk delete
static const int keySize        = 256;
static const int keyAddrSize    = 1;
static const int recordSize     = 1024;
static const int recordAddrSize = 2;
static const int crcSize        = 4;
*/



void func_prime_keys();
void func_write_key();
void func_erase_keys();
void func_get_noise();
void func_get_record();
void func_put_record();

/*
struct FunctionDescr
{
    KeyFunction opcode;
    const char* command;
    size_t data_length;    // input data; not including CRC-32 or trailing '='
};

static constexpr FunctionDescr Interface[] =
{
    { KeyFunction::exchange_key, "",       sklib::base64_type::encoded_length(keySize + crcSize)        },
    { KeyFunction::write_key,    "write",  sklib::base64_type::encoded_length(keyAddrSize + 2*keySize + crcSize) },
    { KeyFunction::erase_keys,   "erase",  0                                                            },
    { KeyFunction::prime_keys,   "prime",  sklib::base64_type::encoded_length(keySize + crcSize)        },
    { KeyFunction::get_noise,    "noise",  0                                                            },
    { KeyFunction::put_record,   "put",    sklib::base64_type::encoded_length(recordAddrSize + recordSize + crcSize) },
    { KeyFunction::get_record,   "get",    sklib::base64_type::encoded_length(recordAddrSize + crcSize) },
    { KeyFunction::get_status,   "status", 0                                                            }
};

static const size_t InterfaceSize = sizeof(Interface) / sizeof(Interface[0]);

static constexpr size_t svc_strlen(const char* str)
{
    const char* str_org = str;
    while (*str) str++;
    return str - str_org;
}

static constexpr size_t svc_ifacedim()
{
    size_t R = 0;
    for (size_t k=0; k<InterfaceSize; k++)
    {
        R = std::max(std::max<size_t>(Interface[k].data_length, svc_strlen(Interface[k].command)), R);
    }
    return R;
}

static const size_t MinBufferLength = svc_ifacedim();
*/


int main()
{
    // initialization

    Interface Serial(hdw_getchar, hwd_putchar);

    for (unsigned k = 0; k < KEY_SIZE; k++) KeyPermutation[k] = (uint8_t)k;
    recalculate_key_sorting();

    // main loop

    while (true)
    {
        int sym_in = EOF;

        // command loop

        auto L = Serial.read_input(BUFFER, 1, KEY_SIZE);
        if (!L) continue;

        if (L == KEY_SIZE)
        {
            // search
            Serial.write_output(hdw_get_block_ptr(0), KEY_SIZE);
        }
        else if (L == 1)
        {
            uint8_t Rcode = (uint8_t)KeyResponse::NAK;

            switch (BUFFER[0])
            {
            case (int)KeyFunction::prime_keys: // remember rotator, build index
                if (Serial.read_input(BUFFER, KEY_SIZE) == KEY_SIZE)
                {
                    for (unsigned k = 0; k < KEY_SIZE; k++) KeyPermutation[k] = BUFFER[k];
                    recalculate_key_sorting();
                    Rcode = (uint8_t)KeyResponse::ACK;
                }
                Serial.write_output(&Rcode, 1);
                break;
            }

        }

    }
}

/*
void func_prime_keys();
void func_write_key();
void func_erase_keys();
void func_get_noise();
void func_get_record();
void func_put_record();
*/
