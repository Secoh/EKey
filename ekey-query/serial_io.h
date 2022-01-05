// This file is part of E-Key project - hardware/software platform to support certain cryptography applications.
// Copyright [2021] Secoh  https://github.com/Secoh/EKey
// This application uses SKLib: https://github.com/Secoh/SKLib
//
// Licensed under the GNU General Public License, Version 3 or later. See: https://www.gnu.org/licenses/
// You may not use this file except in compliance with the License. Any derivative work must retain the License.
// Software is distributed on "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//

#pragma once
#include "../ekey-model/interface.h"

// communications counterpart for ekey-model project
// unlike file I/O, there is no EOF condition in USB-Serial terminal looking from inside
// getchar returns true if charachter has arrived - stored in ch

bool ser_autodetect();
bool ser_getchar(int& ch);
void ser_putchar(int ch);
