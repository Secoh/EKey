// This file is part of E-Key project - hardware/software platform to support certain cryptography applications.
// Copyright [2021] Secoh  https://github.com/Secoh/EKey
// This application uses SKLib: https://github.com/Secoh/SKLib
//
// Licensed under the GNU General Public License, Version 3 or later. See: https://www.gnu.org/licenses/
// You may not use this file except in compliance with the License. Any derivative work must retain the License.
// Software is distributed on "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//

// This header is for firmware simulator communicating with key manager via TCP sockets.
// The firmware will use hardware I/O specific to Atmel Cortex M0+ platform with USB serial.
// The logic part is the same for both firmware and simulator.

#pragma once
#include "interface.h"

// references to hardware-specific functions (or emulation)

// will use autoinit by static instantination of a class wi all initializations

uint8_t* hdw_get_key_ptr(uint8_t idx);
void hdw_store_key_pair(uint8_t idx, uint8_t* key, uint8_t* payload);

uint8_t hdw_get_noise();

uint8_t* hdw_get_block_ptr(uint8_t idx);
void hdw_store_block(uint8_t idx, uint8_t* block);

// unlike file I/O, there is no EOF condition in USB-Serial terminal looking from inside
// getchar returns true if charachter has arrived - stored in ch

bool hdw_getchar(int& ch);
void hwd_putchar(int ch);

