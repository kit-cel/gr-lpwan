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


#ifndef INCLUDED_LPWAN_MAC_FIELD_FRAME_CTRL_H
#define INCLUDED_LPWAN_MAC_FIELD_FRAME_CTRL_H

#include <lpwan/api.h>
#include "mac_field.h"

namespace gr {
  namespace lpwan {

    #define FRAME_CTRL_TYPE_BEACON        0
    #define FRAME_CTRL_TYPE_DATA          1
    #define FRAME_CTRL_TYPE_ACK           2
    #define FRAME_CTRL_TYPE_MAC_COMMAND   3
    #define FRAME_CTRL_TYPE_RESERVED      4
    #define FRAME_CTRL_TYPE_MULTIPURPOSE  5
    #define FRAME_CTRL_TYPE_FRAGMENT      6
    #define FRAME_CTRL_TYPE_EXTENDED      7

    #define FRAME_CTRL_VER_2003           0
    #define FRAME_CTRL_VER_2006           1
    #define FRAME_CTRL_VER_2015           2
    #define FRAME_CTRL_VER_RESERVED       3

    #define FRAME_CTRL_ADDR_NOT_PRESENT   0
    #define FRAME_CTRL_ADDR_RESERVED      1
    #define FRAME_CTRL_ADDR_SHORT         2
    #define FRAME_CTRL_ADDR_EXTENDED      3

    #define FRAME_CTRL_SIZE               2




    /*!
     * \brief frame control field
     *
     */
    class lpwan_API mac_field_frame_ctrl : private mac_field
    {

    public:
        std::vector<uint8_t> get_data();
        void set_data(const std::vector<uint8_t> &v);

        void set_data(uint16_t d);

        uint8_t get_frame_type()    {return d_frame_type;}
        uint8_t get_sec_enable()    {return d_sec_enable;}
        uint8_t get_frame_pending() {return d_frame_pending;}
        uint8_t get_ar()            {return d_ar;}
        uint8_t get_pan_id_compr()  {return d_pan_id_compr;}
        uint8_t get_sequ_nr_suppr() {return d_sequ_nr_suppr;}
        uint8_t get_ie_pres()       {return d_ie_pres;}
        uint8_t get_dest_addr_mode(){return d_dest_addr_mode;}
        uint8_t get_frame_vers()    {return d_frame_vers;}
        uint8_t get_src_addr_mode() {return d_src_addr_mode;}

        void set_frame_type(uint8_t ft)     {d_frame_type = ft;}
        void set_sec_enable(uint8_t se)     {d_sec_enable = se;}
        void set_frame_pending(uint8_t fp)  {d_frame_pending = fp;}
        void set_ar(uint8_t ar)             {d_ar = ar;}
        void set_pan_id_compr(uint8_t pic)  {d_pan_id_compr = pic;}
        void set_sequ_nr_suppr(uint8_t sns) {d_sequ_nr_suppr = sns;}
        void set_ie_pres(uint8_t iep)       {d_ie_pres = iep;}
        void set_dest_addr_mode(uint8_t dam){d_dest_addr_mode = dam;}
        void set_frame_vers(uint8_t fv)     {d_frame_vers = fv;}
        void set_src_addr_mode(uint8_t srm) {d_src_addr_mode = srm;}

        mac_field_frame_ctrl(const std::vector<uint8_t> &v);
        mac_field_frame_ctrl(uint16_t d);
        mac_field_frame_ctrl();
        ~mac_field_frame_ctrl();

    private:

        uint8_t d_frame_type;
        uint8_t d_sec_enable;
        uint8_t d_frame_pending;
        uint8_t d_ar;
        uint8_t d_pan_id_compr;
        uint8_t d_sequ_nr_suppr;
        uint8_t d_ie_pres;
        uint8_t d_dest_addr_mode;
        uint8_t d_frame_vers;
        uint8_t d_src_addr_mode;
    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_MAC_FIELD_FRAME_CTRL_H */

