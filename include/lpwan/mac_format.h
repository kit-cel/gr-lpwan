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


#ifndef INCLUDED_LPWAN_MAC_FORMAT_H
#define INCLUDED_LPWAN_MAC_FORMAT_H

#include <lpwan/api.h>
#include <boost/crc.hpp>


namespace gr {
    namespace lpwan {

        typedef struct{
            bool dest_pan_id_pres = false;
            bool src_pan_id_pres = false;

            uint16_t dest_pan_id;
            uint16_t src_pan_id;

            uint8_t dest_addr_mode = 0;
            uint64_t dest_addr;

            uint8_t src_addr_mode;
            uint64_t src_addr;

        } address_info;

        class lpwan_API mac_format {

            public:

            static uint32_t extract_bits(const uint8_t* vec, int bitstart, int bitlen);
            static void insert_bits(uint8_t* vec, uint32_t bits, int bitstart, int bitlen);
            static uint16_t bytes2word(uint8_t msb, uint8_t lsb);
            static void append_word(std::vector<uint8_t> &v, uint16_t word);
            static void append_bytes(std::vector<uint8_t> &v, int num_bytes, uint64_t bytes);

        private:

        };

    } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_MAC_FORMAT_H */