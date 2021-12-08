#pragma once

#include <SKLib/sklib.hpp>

// reads into buffer, either block_len bytes, or alt_len if specified
// length includes 2 bytes CRC
unsigned read_input(sklib::base64_type& IO, uint8_t* buffer, unsigned block_len, unsigned alt_len = 0)
{
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
                if (pos_in > 2 && (pos_in == block_len || pos_in == alt_len) &&
                    sklib::crc_16_ccitt().update(buffer, pos_in - 2) == buffer[pos_in - 2] * 0x100 + buffer[pos_in - 1])
                {
                    IO.reset();
                    return pos_in - 2;  // successfull receive
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

// unlike reading, length does not include CRC
void write_output(sklib::base64_type& IO, uint8_t* buffer, unsigned length)
{
    for (unsigned i = 0; i < length; i++) IO.write_encode(buffer[i]);

    const uint16_t crcval = sklib::crc_16_ccitt().update(buffer, length);
    IO.write_encode((crcval >> 8) & 0xFF);
    IO.write_encode(crcval & 0xFF);
    IO.write_encode(EOF);
}
