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


#ifndef INCLUDED_LPWAN_MAC_FIELD_FRAG_STATUS_H
#define INCLUDED_LPWAN_MAC_FIELD_FRAG_STATUS_H

#include <lpwan/api.h>
#include "mac_field.h"

namespace gr {
  namespace lpwan {

    /*!
     * \brief frak packet status field
     *
     */
    class lpwan_API mac_field_frag_status : private mac_field
    {
    public:

        uint8_t get_frak_content(){ return  d_frak_content;}
        uint8_t get_lqi(){ return d_lqi;}

        void set_frak_content(uint8_t fc){d_frak_content = fc;}
        void set_lqi(uint8_t lqi){d_lqi = lqi;}

        void set_frag_rx_set_available(int set, bool avail);
        bool is_frag_rx_set_available(int set);
        void set_frag_recv_bit(int frag_nr, bool bit);


        std::vector<uint8_t> get_data();
        void set_data(const std::vector<uint8_t> &v);

        mac_field_frag_status(const std::vector<uint8_t> &v);
        mac_field_frag_status();
      ~mac_field_frag_status();
    private:
        uint8_t d_frak_content;
        uint8_t d_lqi;
        uint16_t d_frag_rx_set[4];


    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_MAC_FIELD_FRAG_STATUS_H */

