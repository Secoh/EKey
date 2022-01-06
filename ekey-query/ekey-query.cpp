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

// Shared IO settings, common IO functions, KEY, BLOCK sizes, counts
#include "../ekey-model/interface.h"

// serial I/O actual functions
#include "serial_io.h"

static constexpr unsigned BUFFER_SIZE = 1536;
uint8_t BUFFER[std::max({ BUFFER_SIZE, Interface::read_buffer_size(KEY_SIZE), Interface::read_buffer_size(BLOCK_SIZE) })];

int main()
{
    // initialization

    if (!ser_autodetect())
    {
        std::cout << "Cannot connect to EKey, exiting...\n";
        return -1;
    }

    Interface Serial(ser_getchar, ser_putchar);

    std::cout << "Query: 1\n";
    uint8_t Code = (uint8_t)KeyFunction::prime_keys;
    Serial.write_output(&Code, 1);

    std::cout << "Data: " << KEY_SIZE << "\n";
    for (unsigned k = 0; k < KEY_SIZE; k++) BUFFER[k] = ~k;
    Serial.write_output(BUFFER, KEY_SIZE);

    std::cout << "Received: " << Serial.read_input(&Code, 1) << "\n";
    std::cout << "Code = " << (unsigned)Code << "\n";

    return 0;
}
