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
#include <lpwan/mac_field_frag_header.h>

namespace gr {
    namespace lpwan {

        std::vector<uint8_t>
        mac_field_frag_header::get_data() {
            std::vector<uint8_t> v;
            uint16_t d = 0;

            d |= FRAME_CTRL_TYPE_FRAGMENT;
            d <<= 7;
            d |= d_tid;
            d <<= 6;
            d |= d_fragment_nr;

            v.push_back(static_cast<uint8_t>((d >> 8) & 0xFF));
            v.push_back(static_cast<uint8_t>((d >> 0) & 0xFF));
            return v;
        }

        void
        mac_field_frag_header::set_data(const std::vector<uint8_t> &v) {
            set_data((static_cast<uint16_t>(v[0]) << 8) | v[1]);
        }

        void
        mac_field_frag_header::set_data(uint16_t d) {
            d_fragment_nr = d & 0b111111;
            d >>= 6;
            d_tid = d & 0b1111111;
            d >>= 7;
            if (d & 0b111 != FRAME_CTRL_TYPE_FRAGMENT) {
                throw std::runtime_error("Initialized fragment with non fragment frame type field.\n");
            }
        }


        mac_field_frag_header::mac_field_frag_header(const std::vector<uint8_t> &v) {
            set_data(v);
        }

        mac_field_frag_header::mac_field_frag_header(uint16_t d) {
            set_data(d);
        }

        mac_field_frag_header::mac_field_frag_header() {

        }

        mac_field_frag_header::mac_field_frag_header(uint8_t tid, uint8_t fragment_nr) {
            d_tid = tid;
            d_fragment_nr = fragment_nr;
        }

        mac_field_frag_header::~mac_field_frag_header() {
        }

    } /* namespace lpwan */
} /* namespace gr */

