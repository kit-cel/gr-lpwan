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


#ifndef INCLUDED_LPWAN_MAC_FIELD_FRAG_HEADER_H
#define INCLUDED_LPWAN_MAC_FIELD_FRAG_HEADER_H

#include <lpwan/api.h>
#include "mac_field.h"
#include "mac_field_frame_ctrl.h"

namespace gr {
  namespace lpwan {

    /*!
     * \brief fragmentation packet header
     *
     */
    class lpwan_API mac_field_frag_header : public mac_field
    {
    public:
        std::vector<uint8_t> get_data();
        void set_data(const std::vector<uint8_t> &v);
        void set_data(uint16_t d);

        uint8_t get_tid()           {return d_tid;}
        uint8_t get_fragment_nr()  {return d_fragment_nr;}

        void set_tid(uint8_t tid)           {d_tid = tid;}
        void set_fragment_nr(uint8_t fnr)   {d_fragment_nr = fnr;}

        mac_field_frag_header(const std::vector<uint8_t> &v);
        mac_field_frag_header(uint16_t d);
        mac_field_frag_header(uint8_t tid, uint8_t fragment_nr);
        mac_field_frag_header();

        ~mac_field_frag_header();
    private:
        uint8_t d_tid;
        uint8_t d_fragment_nr;

    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_MAC_FIELD_FRAG_HEADER_H */

