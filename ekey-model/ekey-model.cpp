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

#include "hardware_model.h"

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

static const int keySize        = 256;
static const int keyAddrSize    = 1;
static const int recordSize     = 1024;
static const int recordAddrSize = 2;
static const int crcSize        = 4;

enum class KeyFunction {
    prime_keys = 0x11,
    write_key  = 0x22,
    erase_keys = 0x33,
    get_noise  = 0x44,
    get_record = 0x55,
    put_record = 0x66 };

static const unsigned MAX_BUF_LEN = 1536;
uint8_t BUF[MAX_BUF_LEN];

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

// transform between Base64 interface and hardware interface
bool hdw_getchar(sklib::base64_type*, int& ch)
{
    uint8_t data=0;
    if (!hdw_getchar(data)) return false;
    ch = data;
    return true;
}
void hwd_putchar(sklib::base64_type*, int ch)
{
    hwd_putchar((uint8_t)ch);
}

int main()
{
    sklib::base64_type IO(hdw_getchar, hwd_putchar);
    sklib::crc_16_ccitt data_crc;

    while (true)
    {
        // command loop

    }
}
