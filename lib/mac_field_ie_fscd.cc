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
#include <lpwan/mac_field_ie_fscd.h>
#include <lpwan/mac_field_frame.h>
#include <lpwan/mac_format.h>

namespace gr {
    namespace lpwan {

        std::vector<uint8_t>
        mac_field_ie_fscd::get_data() {

            std::vector<uint8_t> v (4,0);

            mac_format::insert_bits(&v[0], d_sec_frag,        FSCD_BITSTART_SEC_FRAG,    FSCD_BITLEN_SEC_FRAG);
            mac_format::insert_bits(&v[0], d_tid,             FSCD_BITSTART_TID,         FSCD_BITLEN_TID);
            mac_format::insert_bits(&v[0], d_frak_policy,     FSCD_BITSTART_FRAK_POLICY, FSCD_BITLEN_FRAK_POLICY);
            mac_format::insert_bits(&v[0], d_fics_len==2?0:1, FSCD_BITSTART_FICS_LEN,    FSCD_BITLEN_FICS_LEN);
            mac_format::insert_bits(&v[0], d_psdu_size,       FSCD_BITSTART_PSDU_SIZE,   FSCD_BITLEN_PSDU_SIZE);
            mac_format::insert_bits(&v[0], d_ai.src_pan_id_pres?1:0,  FSCD_BITSTART_SRC_PAN_PRES,   FSCD_BITLEN_SRC_PAN_PRES);
            mac_format::insert_bits(&v[0], d_ai.dest_pan_id_pres?1:0, FSCD_BITSTART_DEST_PAN_PRES,  FSCD_BITLEN_DEST_PAN_PRES);
            mac_format::insert_bits(&v[0], d_ai.src_addr_mode,    FSCD_BITSTART_SRC_ADDR_MODE,  FSCD_BITLEN_SRC_ADDR_MODE);
            mac_format::insert_bits(&v[0], d_ai.dest_addr_mode,   FSCD_BITSTART_DEST_ADDR_MODE, FSCD_BITLEN_DEST_ADDR_MODE);


            if(d_ai.src_pan_id_pres){
                mac_format::append_word(v, d_ai.src_pan_id);
            }
            if(d_ai.dest_pan_id_pres){
                mac_format::append_word(v, d_ai.dest_pan_id);
            }

            if(d_ai.src_addr_mode == FRAME_CTRL_ADDR_SHORT){
                mac_format::append_word(v, static_cast<uint16_t>(d_ai.src_addr));
            }else if(d_ai.src_addr_mode == FRAME_CTRL_ADDR_EXTENDED){
                mac_format::append_bytes(v, 8, d_ai.src_addr);
            }

            if(d_ai.dest_addr_mode == FRAME_CTRL_ADDR_SHORT){
                mac_format::append_word(v, static_cast<uint16_t>(d_ai.dest_addr));
            }else if(d_ai.dest_addr_mode == FRAME_CTRL_ADDR_EXTENDED){
                mac_format::append_bytes(v, 8, d_ai.dest_addr);
            }

            //mac_field_header_ie::set_length(v.size());
            std::vector<uint8_t > h = mac_field_header_ie::get_data();
            h.insert(h.end(),v.begin(), v.end());

            return h;
        }



        void
        mac_field_ie_fscd::set_data(const std::vector<uint8_t> &v) {
            //first 2 byte
            mac_field_header_ie::set_data(v);

            d_sec_frag =    mac_format::extract_bits(&v[2], FSCD_BITSTART_SEC_FRAG,    FSCD_BITLEN_SEC_FRAG);
            d_tid =         mac_format::extract_bits(&v[2], FSCD_BITSTART_TID,         FSCD_BITLEN_TID);
            d_frak_policy = mac_format::extract_bits(&v[2], FSCD_BITSTART_FRAK_POLICY, FSCD_BITLEN_FRAK_POLICY);
            d_fics_len =    mac_format::extract_bits(&v[2], FSCD_BITSTART_FICS_LEN,    FSCD_BITLEN_FICS_LEN);
            d_psdu_size =   mac_format::extract_bits(&v[2], FSCD_BITSTART_PSDU_SIZE,   FSCD_BITLEN_PSDU_SIZE);
            d_ai.src_pan_id_pres = mac_format::extract_bits(&v[2], FSCD_BITSTART_SRC_PAN_PRES,   FSCD_BITLEN_SRC_PAN_PRES);
            d_ai.dest_pan_id_pres= mac_format::extract_bits(&v[2], FSCD_BITSTART_DEST_PAN_PRES,  FSCD_BITLEN_DEST_PAN_PRES);
            d_ai.src_addr_mode   = mac_format::extract_bits(&v[2], FSCD_BITSTART_SRC_ADDR_MODE,  FSCD_BITLEN_SRC_ADDR_MODE);
            d_ai.dest_addr_mode  = mac_format::extract_bits(&v[2], FSCD_BITSTART_DEST_ADDR_MODE, FSCD_BITLEN_DEST_ADDR_MODE);

            int bytepos = 6;
            if(d_ai.src_pan_id_pres){
                    d_ai.src_pan_id = v[bytepos++];
                    d_ai.src_pan_id <<= 8;
                    d_ai.src_pan_id |= v[bytepos++];
            }
            if(d_ai.dest_pan_id_pres){
                    d_ai.dest_pan_id = v[bytepos++];
                    d_ai.dest_pan_id <<= 8;
                    d_ai.dest_pan_id |= v[bytepos++];
            }
            if(d_ai.src_addr_mode == FRAME_CTRL_ADDR_SHORT){
                    d_ai.src_addr = v[bytepos++];
                    d_ai.src_addr <<= 8;
                    d_ai.src_addr |= v[bytepos++];
            }else if(d_ai.src_addr_mode == FRAME_CTRL_ADDR_EXTENDED){
                    throw std::runtime_error("not supported");
            }

            if(d_ai.dest_addr_mode == FRAME_CTRL_ADDR_SHORT){
                    d_ai.dest_addr = v[bytepos++];
                    d_ai.dest_addr <<= 8;
                    d_ai.dest_addr |= v[bytepos++];
            }else if(d_ai.dest_addr_mode == FRAME_CTRL_ADDR_EXTENDED){
                    throw std::runtime_error("not supported");
            }


            if(d_sec_frag){
                    throw std::runtime_error("Security not supported.");
            }


        }

        mac_field_ie_fscd::mac_field_ie_fscd(std::vector<uint8_t> v)
        : mac_field_ie_fscd()
        {
                set_data(v);
        }

        mac_field_ie_fscd::mac_field_ie_fscd()
        {
            set_ele_id(HEAD_IE_ELE_ID_FSCD);
        }

        mac_field_ie_fscd::~mac_field_ie_fscd() {
        }

    } /* namespace lpwan */
} /* namespace gr */

