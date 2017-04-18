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
#include <lpwan/mac_field_frame_ctrl.h>

namespace gr {
    namespace lpwan {


        mac_field_frame_ctrl::mac_field_frame_ctrl(const std::vector<uint8_t> &v)
        {
            set_data(v);
        }

        mac_field_frame_ctrl::mac_field_frame_ctrl(uint16_t d)
        {
            set_data(d);
        }

        void
        mac_field_frame_ctrl::set_data(const std::vector<uint8_t> &v){
            set_data((static_cast<uint16_t>(v[0]) << 8) | v[1]);
        }


        void
        mac_field_frame_ctrl::set_data(uint16_t d){
            d_src_addr_mode = d & 0b11;
            d >>= 2;
            d_frame_vers = d & 0b11;
            d >>= 2;
            d_dest_addr_mode = d & 0b11;
            d >>= 2;
            d_ie_pres = d & 1;
            d >>= 1;
            d_sequ_nr_suppr = d & 1;
            d >>= 2;
            d_pan_id_compr = d & 1;
            d >>= 1;
            d_ar = d & 1;
            d >>= 1;
            d_frame_pending = d & 1;
            d >>= 1;
            d_sec_enable = d & 1;
            d >>= 1;
            d_frame_type = d & 0b111;
            d >>= 3;
            //d_size = FRAME_CTRL_SIZE;
        }

        std::vector<uint8_t>
        mac_field_frame_ctrl::get_data(){
            std::vector<uint8_t> v;
            uint16_t d = 0;
            //TODO: use mac_format
            d |= d_frame_type & 0b111;
            d <<= 1;
            d |= d_sec_enable & 0b1;
            d <<= 1;
            d |= d_frame_pending & 0b1;
            d <<= 1;
            d |= d_ar & 0b1;
            d <<= 1;
            d |= d_pan_id_compr & 0b1;
            d <<= 2;
            d |= d_sequ_nr_suppr & 0b1;
            d <<= 1;
            d |= d_ie_pres  & 0b1;
            d <<= 2;
            d |= d_dest_addr_mode  & 0b11;
            d <<= 2;
            d |= d_frame_vers  & 0b11;
            d <<= 2;
            d |= d_src_addr_mode  & 0b11;

            v.push_back(static_cast<uint8_t>((d >> 8) & 0xFF));
            v.push_back(static_cast<uint8_t>((d >> 0) & 0xFF));

            return v;
        }



        mac_field_frame_ctrl::mac_field_frame_ctrl()
        {
        }

        mac_field_frame_ctrl::~mac_field_frame_ctrl()
        {
        }

    } /* namespace lpwan */
} /* namespace gr */

