/* -*- c++ -*- */
/* 
 * Copyright 2017 Kristian Maier <kristian.maier@gmx.de>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <lpwan/mac_crc.h>

namespace gr {
    namespace lpwan {


        unsigned long
        mac_crc::calc_crc(const std::vector<uint8_t> &data) {
            unsigned long cs;
            if (d_crclen == 2) {
                d_crc16.reset();
                d_crc16.process_bytes(&data[0], data.size());
                cs = d_crc16.checksum();
            } else if (d_crclen == 4) {
                d_crc32.reset();
                d_crc32.process_bytes(&data[0], data.size());
                cs = d_crc32.checksum();
            }
            return cs;
        }


        bool
        mac_crc::check_crc(const std::vector<uint8_t> &data) {
            bool ok;
            if (d_crclen == 2)
            {
                d_crc16.reset();
                d_crc16.process_bytes(&data[0], data.size());
                ok = (d_crc16.checksum() == crc16_remainder);
            }
            else if (d_crclen == 4)
            {
                d_crc32.reset();
                d_crc32.process_bytes(&data[0], data.size());
                ok = (d_crc32.checksum() == crc32_remainder);
            }
            return ok;
        }

        /**
        * Appends the crc checksum to the given data.
        * @param data to generate the checksum
        */
        void
        mac_crc::append_crc(std::vector<uint8_t> &data) {

            unsigned long cs;
            cs = calc_crc(data);

            if (d_crclen == 2) {
                data.push_back(static_cast<uint8_t> ((cs >> 8) & 0xFF));
                data.push_back(static_cast<uint8_t> ((cs >> 0) & 0xFF));

            } else if (d_crclen == 4) {
                data.push_back(static_cast<uint8_t> ((cs >> 24) & 0xFF));
                data.push_back(static_cast<uint8_t> ((cs >> 16) & 0xFF));
                data.push_back(static_cast<uint8_t> ((cs >>  8) & 0xFF));
                data.push_back(static_cast<uint8_t> ((cs >>  0) & 0xFF));
            }
        }


        /**
        * Checks the CRC at the end of the data and removes it.
        * @param data data to check with the crc appended
        * @return true if check is ok, false otherwise
        */
        bool
        mac_crc::check_crc_and_remove(std::vector<uint8_t> &data) {
            bool ok;
            ok = check_crc(data);
            data.resize(data.size() - d_crclen);
            return ok;
        }


        mac_crc::mac_crc(int crclen) {
            if (crclen != 4 && crclen != 2) {
                throw std::runtime_error("CRC Length has to be 2 or 4 bytes");
            }

            d_crclen = crclen;
        }

        mac_crc::~mac_crc() {
        }

    } /* namespace lpwan */
} /* namespace gr */

