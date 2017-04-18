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


#ifndef INCLUDED_LPWAN_MAC_FIELD_FRAME_H
#define INCLUDED_LPWAN_MAC_FIELD_FRAME_H

#include <lpwan/api.h>
#include <string>
#include "mac_format.h"
#include "mac_field.h"
#include "mac_crc.h"
#include "mac_field_header_ie.h"
#include "mac_field_ie_fscd.h"
#include "mac_field_frame_ctrl.h"


namespace gr {
  namespace lpwan {




    /*!
     * \brief mac frame
     *
     */
    class lpwan_API mac_field_frame : protected mac_field
    {
    public:

        virtual std::vector<uint8_t> get_data();
        virtual void set_data(const std::vector<uint8_t> &v);

        int get_padding(){return d_pad_to;}
        bool get_is_valid(){return d_valid;}
        uint8_t get_seq_nr(){return d_seq_nr;}
        address_info get_address_info(){return d_ai;}
        mac_field_frame_ctrl get_frame_ctrl(){return d_fr_ctrl;}
        std::vector<mac_field_header_ie*> get_header_ies(){ return d_h_ies;}
        std::vector<uint8_t> get_payload(){ return d_payload;}

        void set_padding(int p){d_pad_to = p;}
        void set_is_valid(bool v){d_valid = v;}
        void set_seq_nr(uint8_t sqn){d_seq_nr = sqn;}
        void set_address_info(address_info  &ai){d_ai = ai;}
        void set_frame_ctrl(mac_field_frame_ctrl &fc){d_fr_ctrl = fc;}
        void set_header_ies(std::vector<mac_field_header_ie*> &ie){ d_h_ies = ie;}
        void set_payload(std::vector<uint8_t> &p){d_payload = p;}

        void add_header_ie(mac_field_header_ie *ie);

        static bool src_pan_id_present(int dest_addr_mode, int src_addr_mode, int pan_id_compr);
        static bool dest_pan_id_present(int dest_addr_mode, int src_addr_mode, int pan_id_compr);

        mac_field_frame(const std::vector<uint8_t> &v, int crclen);
        mac_field_frame(int crclen);
        ~mac_field_frame();

    private:

        int d_pad_to = 0;

        bool d_valid = false;
        mac_crc *d_crc;
        uint8_t d_seq_nr = 0;
        address_info d_ai;
        mac_field_frame_ctrl d_fr_ctrl;
        std::vector<mac_field_header_ie*> d_h_ies;
        std::vector<uint8_t> d_payload;

    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_MAC_FIELD_FRAME_H */

