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
#include <lpwan/mac_field_frag_status.h>


namespace gr {
    namespace lpwan {


        mac_field_frag_status::mac_field_frag_status(const std::vector<uint8_t> &v) {
            set_data(v);
        }

        mac_field_frag_status::mac_field_frag_status() {
        }

        mac_field_frag_status::~mac_field_frag_status() {
        }

        void
        mac_field_frag_status::set_frag_rx_set_available(int set, bool avail) {
            if (avail) {
                d_frak_content |= (1 << (3 - set));
            } else {
                d_frak_content &= ~(1 << (3 - set));
            }
        }

        bool
        mac_field_frag_status::is_frag_rx_set_available(int set) {
            return d_frak_content & (1 << (3 - set)) != 0;
        }

        void
        mac_field_frag_status::set_frag_recv_bit(int frag_nr, bool bit) {
            int bitpos = (frag_nr % 16);
            int set = frag_nr / 16;
            if (bit) {
                d_frag_rx_set[set] |= (1 << bitpos);
            } else {
                d_frag_rx_set[set] &= ~(1 << bitpos);
            }
        }

        std::vector<uint8_t>
        mac_field_frag_status::get_data() {
            std::vector<uint8_t> v;

            v.push_back(d_frak_content << 4 | d_lqi);
            for (int i = 0; i < 4; ++i) {
                if (is_frag_rx_set_available(i)) {
                    v.push_back(static_cast<uint8_t>((d_frag_rx_set[i] >> 8) & 0xFF));
                    v.push_back(static_cast<uint8_t>((d_frag_rx_set[i] >> 0) & 0xFF));
                }
            }

            return v;
        }

        void
        mac_field_frag_status::set_data(const std::vector<uint8_t> &v) {
            d_frak_content = (v[0] >> 4) & 0xF;
            d_lqi = v[0] & 0xF;

            int v_pos = 1;
            for (int i = 0; i < 4; ++i) {
                if (is_frag_rx_set_available(i)) {
                    d_frag_rx_set[i] = (static_cast<uint8_t>(v[v_pos]) << 8) | v[v_pos + 1];
                    v_pos += 2;
                }
            }
            //d_size = v.size();
        }


    } /* namespace lpwan */
} /* namespace gr */

