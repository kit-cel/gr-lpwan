/* -*- c++ -*- */
/* 
 * Copyright 2017 Kristian Maier <kristian.maier@gmx.de>.
 * //standard is mixing psdu and mpdu
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
#include <lpwan/mac_field_frame.h>
#include <lpwan/mac_format.h>

namespace gr {
    namespace lpwan {


        std::vector<uint8_t>
        mac_field_frame::get_data() {
            std::vector<uint8_t> data;

            data = d_fr_ctrl.get_data();

            //frame ctrl field assert
            if (d_fr_ctrl.get_frame_type() == FRAME_CTRL_TYPE_MULTIPURPOSE) {
                throw std::runtime_error("no support for multipurpose");
            }
            if (d_fr_ctrl.get_sec_enable()) {
                throw std::runtime_error("no support for security");
            }

            //seq nr
            if(d_fr_ctrl.get_sequ_nr_suppr() == 0){
                data.push_back(d_seq_nr);
            }

            //dest pan id
            if(d_ai.dest_pan_id_pres){
                data.push_back((d_ai.dest_pan_id>>8) & 0xFF);
                data.push_back((d_ai.dest_pan_id>>0) & 0xFF);
            }

            //dest addr
            int addr_len = 0;
            if (d_ai.dest_addr_mode == FRAME_CTRL_ADDR_SHORT){
                addr_len = 2;
            } else if (d_ai.dest_addr_mode == FRAME_CTRL_ADDR_EXTENDED){
                addr_len == 8;
            }
            while(addr_len--) {
                data.push_back((d_ai.dest_addr >> addr_len*8) & 0xFF);
            }

            //src pan id
            if(d_ai.src_pan_id_pres){
                data.push_back((d_ai.src_pan_id>>8) & 0xFF);
                data.push_back((d_ai.src_pan_id>>0) & 0xFF);
            }

            //src addr
            addr_len = 0;
            if (d_ai.src_addr_mode == FRAME_CTRL_ADDR_SHORT){
                addr_len = 2;
            } else if (d_ai.src_addr_mode == FRAME_CTRL_ADDR_EXTENDED){
                addr_len == 8;
            }
            while(addr_len--) {
                data.push_back((d_ai.src_addr >> addr_len*8) & 0xFF);
            }


            //header and payload ie
            //TODO: support more than one header IE and payload IE
            if(d_fr_ctrl.get_ie_pres()){
                std::vector<uint8_t> tmp;
                tmp = d_h_ies[0]->get_data();
                data.insert(data.end(), tmp.begin(), tmp.end());
            }

            //padding
            if(d_pad_to != 0 && data.size()<d_pad_to-d_crc->get_crclen()){
                srand(time(NULL));
                while(data.size()!= d_pad_to-d_crc->get_crclen()){
                    data.push_back((uint8_t) 0x34/*(rand() & 0xFF)*/);
                }
            }

            //fcs
            d_crc->append_crc(data);
            return data;
        }

        void
        mac_field_frame::set_data(const std::vector<uint8_t> &v) {


            if (!d_crc->check_crc(v)) {
                d_valid = false;
                return;
            }


            int len = v.size();

            d_fr_ctrl.set_data(v);
            if (d_fr_ctrl.get_frame_type() == FRAME_CTRL_TYPE_FRAGMENT) {
                d_valid = false;
                throw std::runtime_error("Wrong frame type.\n");
            } else if (d_fr_ctrl.get_frame_type() == FRAME_CTRL_TYPE_MULTIPURPOSE) {
                d_valid = false;
                throw std::runtime_error("No support for multipurpose frame.\n");
            }
            if (d_fr_ctrl.get_sec_enable()) {
                d_valid = false;
                throw std::runtime_error("No support for security things.\n");
            }

            int byte_pos = 2;
            if (d_fr_ctrl.get_sequ_nr_suppr() == 0) {
                d_seq_nr = v[byte_pos++];
            }


            d_ai.dest_pan_id_pres = dest_pan_id_present(d_fr_ctrl.get_dest_addr_mode(), d_fr_ctrl.get_src_addr_mode(),
                                                        d_fr_ctrl.get_pan_id_compr());
            d_ai.src_pan_id_pres = src_pan_id_present(d_fr_ctrl.get_dest_addr_mode(), d_fr_ctrl.get_src_addr_mode(),
                                                      d_fr_ctrl.get_pan_id_compr());
            d_ai.dest_addr_mode = d_fr_ctrl.get_dest_addr_mode();
            d_ai.src_addr_mode = d_fr_ctrl.get_src_addr_mode() ;



            if(d_ai.dest_pan_id_pres){
                d_ai.dest_pan_id = mac_format::bytes2word(v[byte_pos],v[byte_pos+1]);
                byte_pos+=2;
            }

            if(d_ai.dest_addr_mode == FRAME_CTRL_ADDR_SHORT){
                d_ai.dest_addr = mac_format::bytes2word(v[byte_pos],v[byte_pos+1]);
                byte_pos+=2;
            }else if(d_ai.dest_addr_mode == FRAME_CTRL_ADDR_EXTENDED){
                throw std::runtime_error("No support for extended address length.\n");
            }

            if(d_ai.src_pan_id_pres){
                d_ai.src_pan_id = mac_format::bytes2word(v[byte_pos],v[byte_pos+1]);
                byte_pos+=2;
            }

            if(d_ai.src_addr_mode == FRAME_CTRL_ADDR_SHORT){
                d_ai.src_addr = mac_format::bytes2word(v[byte_pos],v[byte_pos+1]);
                byte_pos+=2;
            }else if(d_ai.src_addr_mode ==  FRAME_CTRL_ADDR_EXTENDED){
                throw std::runtime_error("No support for extended address length.\n");
            }


            if(d_fr_ctrl.get_ie_pres()){
                //TODO: support more than one ie
                mac_field_header_ie ie(std::vector<uint8_t> (v.begin()+byte_pos, v.end()));

                switch(ie.get_ele_id()){
                    case HEAD_IE_ELE_ID_FSCD: {
                        mac_field_ie_fscd* fscd = new mac_field_ie_fscd(std::vector<uint8_t>(v.begin() + byte_pos, v.end()));
                        d_h_ies.push_back(fscd);
                        break;
                    }
                    default:
                        std::runtime_error("Header IE not supported.\n");
                        break;
                }

                byte_pos += ie.get_length();
            }

            d_payload = std::vector<uint8_t> (v.begin()+byte_pos, v.end()-d_crc->get_crclen());
            d_valid  = true;
        }


        mac_field_frame::mac_field_frame(const std::vector<uint8_t> &v, int crclen) {
            d_crc = new mac_crc(crclen);
            set_data(v);
        }



        void
        mac_field_frame::add_header_ie(mac_field_header_ie *ie){
            d_h_ies.push_back(ie);
            d_fr_ctrl.set_ie_pres(1);
        }


        /**
         * Helper function to determine if the src pan id is present in mac header Std 7.2.1.5
         * @param dest_addr_mode
         * @param src_addr_mode
         * @param pan_id_compr
         * @return
         */
        bool
        mac_field_frame::src_pan_id_present(int dest_addr_mode, int src_addr_mode, int pan_id_compr) {
            bool is_present = false;
            is_present = is_present || (dest_addr_mode == FRAME_CTRL_ADDR_NOT_PRESENT &&
                                        src_addr_mode != FRAME_CTRL_ADDR_NOT_PRESENT &&
                                        pan_id_compr == 0);
            is_present = is_present || (dest_addr_mode == FRAME_CTRL_ADDR_SHORT &&
                                        src_addr_mode != FRAME_CTRL_ADDR_SHORT &&
                                        pan_id_compr == 0);
            is_present = is_present || (dest_addr_mode == FRAME_CTRL_ADDR_SHORT &&
                                        src_addr_mode != FRAME_CTRL_ADDR_EXTENDED &&
                                        pan_id_compr == 0);
            is_present = is_present || (dest_addr_mode == FRAME_CTRL_ADDR_EXTENDED &&
                                        src_addr_mode != FRAME_CTRL_ADDR_SHORT &&
                                        pan_id_compr == 0);
            return is_present;
        }

        /**
         * Helper function to determine if the dest pan id is present in mac header Std 7.2.1.5
         * @param dest_addr_mode
         * @param src_addr_mode
         * @param pan_id_compr
         * @return
         */
        bool
        mac_field_frame::dest_pan_id_present(int dest_addr_mode, int src_addr_mode, int pan_id_compr) {
            bool is_present = true;
            //Table 7-2
            is_present = is_present && !(dest_addr_mode == FRAME_CTRL_ADDR_NOT_PRESENT &&
                                         src_addr_mode == FRAME_CTRL_ADDR_NOT_PRESENT &&
                                         pan_id_compr == 0);
            is_present = is_present && !(dest_addr_mode != FRAME_CTRL_ADDR_NOT_PRESENT &&
                                         src_addr_mode == FRAME_CTRL_ADDR_NOT_PRESENT &&
                                         pan_id_compr == 1);
            is_present = is_present && !(dest_addr_mode == FRAME_CTRL_ADDR_NOT_PRESENT &&
                                         src_addr_mode != FRAME_CTRL_ADDR_NOT_PRESENT &&
                                         pan_id_compr == 0);
            is_present = is_present && !(dest_addr_mode == FRAME_CTRL_ADDR_NOT_PRESENT &&
                                         src_addr_mode != FRAME_CTRL_ADDR_NOT_PRESENT &&
                                         pan_id_compr == 1);
            is_present = is_present && !(dest_addr_mode == FRAME_CTRL_ADDR_EXTENDED &&
                                         src_addr_mode == FRAME_CTRL_ADDR_EXTENDED &&
                                         pan_id_compr == 1);
            return is_present;

        }

        mac_field_frame::mac_field_frame(int crclen) {
            d_crc = new mac_crc(crclen);
        }

        mac_field_frame::~mac_field_frame() {
            delete d_crc;
        }

    } /* namespace lpwan */
} /* namespace gr */

