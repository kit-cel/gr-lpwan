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
#include <lpwan/mac_field_header_ie.h>

namespace gr {
  namespace lpwan {

      std::vector<uint8_t>
      mac_field_header_ie::get_data(){

          std::vector<uint8_t> v;
          uint16_t h=0;

          h |= d_length;
          h <<= 8;
          h |= d_ele_id;
          h <<= 1;
          h |= d_type;

          v.push_back(static_cast<uint8_t>((h >> 8) & 0xFF));
          v.push_back(static_cast<uint8_t>((h >> 0) & 0xFF));

          return v;
      }


      void
      mac_field_header_ie::set_data(const std::vector<uint8_t> &v){
          uint16_t h = mac_format::bytes2word(v[0], v[1]);

          d_type = h & 0b1;
          h >>= 1;
          d_ele_id = h & 0xFF;
          h >>= 8;
          d_length = h & 0x7F;
      }


      mac_field_header_ie::mac_field_header_ie(const std::vector<uint8_t> &v)
      : mac_field_header_ie()
      {
          set_data(v);
      }
      mac_field_header_ie::mac_field_header_ie()
      {

      }

    mac_field_header_ie::~mac_field_header_ie()
    {
    }

  } /* namespace lpwan */
} /* namespace gr */

