// This file is part of E-Key project - hardware/software platform to support certain cryptography applications.
// Copyright [2021] Secoh  https://github.com/Secoh/EKey
// This application uses SKLib: https://github.com/Secoh/SKLib
//
// Licensed under the GNU General Public License, Version 3 or later. See: https://www.gnu.org/licenses/
// You may not use this file except in compliance with the License. Any derivative work must retain the License.
// Software is distributed on "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//

#include <SKLib/sklib.hpp>
#include "serial_io.h"

// -------------------------------------------------------------

void hdw_init();
struct self_init { self_init() { hdw_init(); } };
static self_init CHECK;

// -------------------------------------------------------------

#ifdef EMULATION_SOCKET

void hdw_init() {}
static sklib::stream_tcpip_type SOCKET_IO(false, SOCKET_EKEY_PORT);  // client mode

bool ser_autodetect()
{
    return SOCKET_IO.is_connected();
}

bool ser_getchar(int& ch)
{
    uint8_t c=0;
    if (!SOCKET_IO.read(&c)) return false;
    ch = ((c<' ') ? EOF : c);
    return true;
}

void ser_putchar(int ch)
{
    SOCKET_IO.write((ch<0) ? '\n' : (uint8_t)ch);
}

#endif

// -------------------------------------------------------------

#ifdef REAL_HARDRDWARE

// TODO: implement communication via serial port

void hdw_init()
{
}

bool ser_autodetect()
{
    return false;
}

bool ser_getchar(uint8_t& ch)
{
    return false;
}

void ser_putchar(uint8_t ch);
{
}

#endif
