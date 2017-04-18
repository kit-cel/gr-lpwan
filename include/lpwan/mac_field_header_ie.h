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


#ifndef INCLUDED_LPWAN_MAC_FIELD_HEADER_IE_H
#define INCLUDED_LPWAN_MAC_FIELD_HEADER_IE_H

#include <lpwan/api.h>
#include "mac_field.h"
#include "mac_format.h"

namespace gr {
  namespace lpwan {

    #define HEAD_IE_ELE_ID_VENDOR_SPECIFIC   0x00
    #define HEAD_IE_ELE_ID_CSL               0x1a
    #define HEAD_IE_ELE_ID_RIT               0x1b
    #define HEAD_IE_ELE_ID_FSCD              0x22
    #define HEAD_IE_ELE_ID_LECIM_CAP         0x25
    #define HEAD_IE_ELE_ID_TERMINATION_1     0x7e
    #define HEAD_IE_ELE_ID_TERMINATION_2     0x7f


    /*!
     * \brief header ie base class
     *
     */
    class lpwan_API mac_field_header_ie : private mac_field
    {
    public:

        virtual std::vector<uint8_t> get_data();
        virtual void set_data(const std::vector<uint8_t> &v);

        uint8_t get_length(){return d_length;}
        uint8_t get_ele_id(){return d_ele_id;}
        uint8_t get_type()  {return d_type;}

        void set_length(uint8_t l)  {d_length = l;}
        void set_ele_id(uint8_t ei) {d_ele_id = ei;}
        void set_type(uint8_t t)    {d_type = t;}

        mac_field_header_ie(const std::vector<uint8_t> &v);
        mac_field_header_ie();
        ~mac_field_header_ie();
    private:

        uint8_t d_length = 0;
        uint8_t d_ele_id = 0;
        uint8_t d_type = 0;


    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_MAC_FIELD_HEADER_IE_H */

