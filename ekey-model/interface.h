// This file is part of E-Key project - hardware/software platform to support certain cryptography applications.
// Copyright [2021] Secoh  https://github.com/Secoh/EKey
// This application uses SKLib: https://github.com/Secoh/SKLib
//
// Licensed under the GNU General Public License, Version 3 or later. See: https://www.gnu.org/licenses/
// You may not use this file except in compliance with the License. Any derivative work must retain the License.
// Software is distributed on "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//

#include <SKLib/sklib.hpp>

// package can be compiled in the self-test (debug) mode, or in real hardware mode, which is Atmel Cortex M0+ w/ serial over USB
// the compilation mode can be specified at the compiler command line

#if !defined(REAL_HARDRDWARE) && !defined(EMULATION_SOCKET)

// --- when -D compiler option is not used, enable one of the following:
//#define REAL_HARDRDWARE
#define EMULATION_SOCKET
// ---

#endif

// This header contains common definitions for EKey service and client(s), for software simulator and
// for communication with real hardware.
// The firmware will use hardware I/O specific to Atmel Cortex M0+ platform with USB serial.
// The simulator uses TCP sockets.

#pragma once

// data sizes

static const unsigned KEY_COUNT = sklib::OCTET_ADDRESS_SPAN;    // keys are arranged in pairs
static const unsigned KEY_SIZE  = sklib::OCTET_ADDRESS_SPAN;

static const unsigned BLOCK_COUNT = 32;         // blocks are "file"
static const unsigned BLOCK_SIZE  = 1024;       // 1 kb

// communications

static constexpr unsigned SOCKET_EKEY_PORT = 13579;
static constexpr unsigned SERIAL_SPEED     = 9600;
static constexpr unsigned USB_VID = 0xF055u;   // "FOSS"
static constexpr unsigned USB_PID = 0xE4E7u;   // "EKEY"

// configuration test

#if defined(EMULATION_SOCKET)
#ifdef REAL_HARDRDWARE
#error EKEY ** INTERNAL ERROR ** Cannot declare both Emulator and Hardware at the same time
#endif
#pragma message("Emulation mode is selected. To compile for real hardware, uncomment #define REAL_HARDRDWARE in ekey-model/interface.h")
#elif defined(REAL_HARDRDWARE)
// for the time being...
#error Not implemented
#else
#error Invalid Target
#endif

// functions

class Interface
{
private:
    sklib::base64_type IO;  // will be initialized in constructor
    static constexpr unsigned crc_size = 2;

public:
    Interface(bool (*cget)(int&), void (*cput)(int)) : IO(cget, cput) {}

    // helper function to provide offset between data payload and total packet size
    // caller shall only care about I/O data size, buffer size is calculated
    static constexpr unsigned read_buffer_size(unsigned data_size) { return data_size + crc_size; }

    // reads into buffer, either block_len bytes, or alt_len if specified
    // NB: length _does not_ include CRC
    // returns 0 for no read or length of data packet
    unsigned read_input(uint8_t* buffer, unsigned block_len, unsigned alt_len = 0)
    {
        block_len = read_buffer_size(block_len);
        alt_len = read_buffer_size(alt_len);       // now lengths include CRC size

        unsigned pos_in = 0;
        const unsigned max_len = std::max(block_len, alt_len);

        int sym_in = EOF;
        IO.have_errors();                                     // clear error
        if (!IO.read_decode(sym_in) || sym_in < 0) return 0;  // no wait if idle or EOF

        sklib::timer_stopwatch_type timeout(500_ms_sklib);
        buffer[pos_in++] = sym_in;

        while (!timeout)  // break for error condition
        {
            if (IO.read_decode(sym_in))
            {
                if (sym_in < 0)
                {
                    if (pos_in > crc_size && (pos_in == block_len || pos_in == alt_len) &&
                        sklib::crc_16_ccitt().update(buffer, pos_in-crc_size) == stream_to_uint16(buffer+pos_in-crc_size))
                    {
                        IO.reset();
                        return pos_in - crc_size;  // successfull receive
                    }

                    break;
                }

                if (pos_in >= max_len || IO.have_errors()) break;

                buffer[pos_in++] = sym_in;
                timeout.reset();
            }
        }

        IO.reset();
        return 0;
    }

    // send data packet, length does not include CRC
    void write_output(uint8_t* buffer, unsigned length)
    {
        for (unsigned i = 0; i < length; i++) IO.write_encode(buffer[i]);

        const uint16_t crcval = sklib::crc_16_ccitt().update(buffer, length);
        IO.write_encode((crcval >> 8) & 0xFF);
        IO.write_encode(crcval & 0xFF);
        IO.write_encode(EOF);
    }

private:
    uint16_t stream_to_uint16(const uint8_t* buffer)
    {
        return uint16_t(buffer[0]) << 8 | buffer[1];
    }
};

