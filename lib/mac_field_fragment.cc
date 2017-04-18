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
#include <lpwan/mac_field_fragment.h>

namespace gr {
    namespace lpwan {


        std::vector<uint8_t>
        mac_field_fragment::get_data()
        {
            mac_crc crc(d_crclen);
            std::vector<uint8_t> v;
            v.reserve(d_frag_len);
            d_header.append_data(v);
            v.insert(v.end(), d_payload.begin(), d_payload.end());
            crc.append_crc(v);
            d_crc_ok = true;
            return v;
        }


        void
        mac_field_fragment::set_data(const std::vector<uint8_t> &v)
        {
            mac_crc crc(d_crclen);
            std::vector<uint8_t> tmp = v;
            d_frag_len = v.size();
            if(crc.check_crc_and_remove(tmp)){
                d_crc_ok = true;
                d_header.set_data(tmp);
                d_payload = std::vector<uint8_t> (tmp.begin()+2, tmp.end());
            }else{
                d_crc_ok = false;
            }
        }

        void
        mac_field_fragment::set_data(const std::vector<uint8_t> &v, int crc_len)
        {
            d_crclen = crc_len;
            set_data(v);
        }

        mac_field_fragment::mac_field_fragment(const std::vector<uint8_t> &v, int crc_len)
        {
            d_crclen = crc_len;
            set_data(v);
        }

        mac_field_fragment::mac_field_fragment(const mac_field_frag_header &h, const std::vector<uint8_t> &payload,
                                               int crc_len, int frag_len)
        {
            d_header = h;
            d_payload = payload;
            d_crc_ok = true;
            d_frag_len = frag_len;
            d_crclen = crc_len;
        }

        mac_field_fragment::mac_field_fragment()
        {

        }

        mac_field_fragment::~mac_field_fragment()
        {
        }

    } /* namespace lpwan */
} /* namespace gr */

