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


#ifndef INCLUDED_LPWAN_MAC_FIELD_FRAK_H
#define INCLUDED_LPWAN_MAC_FIELD_FRAK_H

#include <lpwan/api.h>
#include "mac_field.h"
#include "mac_field_frag_header.h"
#include "mac_field_fragment.h"
#include "mac_crc.h"
#include "mac_format.h"

namespace gr {
  namespace lpwan {

    /*!
     * \brief frak packet
     *
     */
    class lpwan_API mac_field_frak : public mac_field
    {
    public:
        std::vector<uint8_t> get_data();
        void set_data(const std::vector<uint8_t> &v);

        void set_lqi(uint8_t lqi){d_lqi = lqi;}
        void set_abort(bool abort){d_abort = abort;}
        void set_header(mac_field_frag_header &h){d_header = h;}

        uint8_t get_lqi(){ return d_lqi;}
        bool get_abort(){ return d_abort;}
        const mac_field_frag_header* get_header(){ return &d_header;}
        std::vector<int> get_frag_status(){return d_frag_status;}

        mac_field_frak(const std::vector<uint8_t> &v, int crc_len, int nr_of_frags);
        mac_field_frak(const std::vector<int> &frag_status, const mac_field_frag_header &h, int pad_to, int crc_len);
        mac_field_frak();
      ~mac_field_frak();
    private:

        mac_field_frag_header d_header;
        std::vector<int> d_frag_status;
        int d_pad_to;
        int d_crclen;
        int d_nr_of_frags;

        uint8_t d_lqi = 0;
        bool d_abort;
        bool d_crc_ok;

    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_MAC_FIELD_FRAK_H */

