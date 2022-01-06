// This file is part of E-Key project - hardware/software platform to support certain cryptography applications.
// Copyright [2021] Secoh  https://github.com/Secoh/EKey
// This application uses SKLib: https://github.com/Secoh/SKLib
//
// Licensed under the GNU General Public License, Version 3 or later. See: https://www.gnu.org/licenses/
// You may not use this file except in compliance with the License. Any derivative work must retain the License.
// Software is distributed on "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//

#include <SKLib/sklib.hpp>
#include "hardware_model.h"

// -------------------------------------------------------------

void hdw_init();
struct self_init { self_init() { hdw_init(); } };
static self_init CHECK;

// -------------------------------------------------------------

#ifdef EMULATION_SOCKET

static std::shared_ptr<uint8_t> KEY_STORE_ROOT;
static std::shared_ptr<uint8_t> BLOCK_STORE_ROOT;
static sklib::stream_tcpip_type SOCKET_IO(true, SOCKET_EKEY_PORT);  // server mode
static bool MODE_RECEIVE = false;

void hdw_init()
{
    srand((unsigned)time(nullptr));
    KEY_STORE_ROOT = std::shared_ptr<uint8_t>(new uint8_t[2*KEY_COUNT*KEY_SIZE]());      // arrays are initialized with all 0-s
    BLOCK_STORE_ROOT = std::shared_ptr<uint8_t>(new uint8_t[BLOCK_COUNT*BLOCK_SIZE]());

    fputs("EKey simulation ON\n", stdout);
    fflush(stdout);
}

uint8_t* hdw_get_key_ptr(uint8_t idx)
{
    return KEY_STORE_ROOT.get() + 2*KEY_SIZE*idx;
}

void hdw_store_key_pair(uint8_t idx, uint8_t *key, uint8_t *payload)
{
    uint8_t *ptr = hdw_get_key_ptr(idx);
    memcpy(ptr, key, KEY_SIZE);
    memcpy(ptr+KEY_SIZE, payload, KEY_SIZE);
}

uint8_t hdw_get_noise()
{
    return (uint8_t)(rand() & sklib::OCTET_MASK);
}

uint8_t* hdw_get_block_ptr(uint8_t idx)
{
    return BLOCK_STORE_ROOT.get() + BLOCK_SIZE*idx;
}

void hdw_store_block(uint8_t idx, uint8_t *block)
{
    memcpy(hdw_get_block_ptr(idx), block, BLOCK_SIZE);
}

bool hdw_getchar(int& ch)
{
    uint8_t c = ' ';
    auto R = SOCKET_IO.read(&c);
    if (!R) return false;

    if (!MODE_RECEIVE)
    {
        MODE_RECEIVE = true;
        fputs("\ninp> ", stdout);
    }

    putchar(c);
    fflush(stdout);

    ch = ((c<' ') ? EOF : c);
    return true;
}

void hwd_putchar(int ch)
{
    if (MODE_RECEIVE)
    {
        MODE_RECEIVE = false;
        fputs("\nout> ", stdout);
    }

    putc(SOCKET_IO.write(ch) ? ch : '#', stdout);
    fflush(stdout);
}

#endif

// -------------------------------------------------------------

#ifdef REAL_HARDRDWARE

void hdw_init()
{}

uint8_t* hdw_get_key_ptr(uint8_t idx)
{
    return nullptr;
}

void hdw_store_key_pair(uint8_t idx, uint8_t *key, uint8_t *payload)
{
}

uint8_t hdw_get_noise()
{
    return 0;
}

uint8_t* hdw_get_block_ptr(uint8_t idx)
{
    return nullptr;
}

void hdw_store_block(uint8_t idx, uint8_t *block)
{
}

bool hdw_getchar(uint8_t& ch)
{
    return false;
}

void hwd_putchar(uint8_t ch);
{
}

#endif

