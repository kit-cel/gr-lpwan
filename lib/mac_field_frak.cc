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
#include <lpwan/mac_field_frak.h>

namespace gr {
  namespace lpwan {



      std::vector<uint8_t>
      mac_field_frak::get_data(){

          mac_crc crc(d_crclen);
          std::vector<uint8_t> v_fr;

          uint8_t frak_content = 0;
          uint8_t last_frag = 0;
          uint16_t frags_received = 0;
          int cnt_frags = d_frag_status.size();

          //iterate over set
          for(int k=0; k<=cnt_frags/16; k++){
              //iterate over frags
              for(int i=k*16; i<cnt_frags && i<(k+1)*16; i++){
                  if(d_frag_status[i] == FRAG_STATUS_SUCCESS){
                      last_frag = i;
                      frags_received |= 1 << (15 - (i % 16));
                  }
              }
              frak_content |= 0b1 << (3 - k);
              //TODO: omit fully acked group from frak
              mac_format::append_word(v_fr, frags_received);
              frags_received=0;
          }

          std::vector<uint8_t> frak;
          d_header.set_fragment_nr(last_frag);
          d_header.append_data(frak);

          frak.push_back((frak_content << 4) | (d_lqi & 0xF));
          frak.insert(frak.end(), v_fr.begin(), v_fr.end());
          while(frak.size() < d_pad_to-d_crclen){
              frak.push_back(0xA);
          }

          crc.append_crc(frak);
          d_crc_ok = true;
          return frak;
      }

      void
      mac_field_frak::set_data(const std::vector<uint8_t> &v)
      {

          mac_crc crc(d_crclen);
          std::vector<uint8_t> tmp = v;
          d_frag_status.resize(0);

          //stop if crc check fails
          if(!crc.check_crc_and_remove(tmp))
          {
              d_crc_ok = false;
              return;
          }

          uint8_t frak_content;
          int last_frag;

          d_crc_ok = true;
          d_header.set_data(tmp);
          d_lqi = (tmp[2] >> 0) & 0xF;
          frak_content = (tmp[2] >> 4) & 0xF;
          last_frag = d_header.get_fragment_nr();

          if(frak_content == 0){
              d_abort = true;
              return;
          }

          bool loop_stop_flag = false;

          //decode frag status received field,
          int frag_status_pos=0;
          for(int i=0; i<4 && !loop_stop_flag; i++){
              if(frak_content & (1 << (3-i)) )
              {
                  for(int k=0; k<16; k++){
                      if(tmp[3+frag_status_pos/8] & (1 << (7-frag_status_pos%8))){
                          d_frag_status.push_back(FRAG_STATUS_SUCCESS);
                      }else{
                          d_frag_status.push_back(FRAG_STATUS_FAILED);
                      }
                      if(frag_status_pos == last_frag){
                          loop_stop_flag = true;
                          break;
                      }
                      frag_status_pos++;
                  }
              }
              else {
                  for(int k=0; k<16; k++)
                     d_frag_status.push_back(FRAG_STATUS_SUCCESS);
              }
          }

          while(d_frag_status.size() < d_nr_of_frags) {
              d_frag_status.push_back(FRAG_STATUS_FAILED);
          }

      }

      mac_field_frak::mac_field_frak(const std::vector<uint8_t> &v, int crc_len, int nr_of_frags)
      {
          d_nr_of_frags = nr_of_frags;
          d_crclen = crc_len;
          set_data(v);
      }


      mac_field_frak::mac_field_frak(const std::vector<int> &frag_status, const mac_field_frag_header &h, int pad_to, int crc_len)
      {
          d_header = h;
          d_frag_status = frag_status;
          d_pad_to = pad_to;
          d_crclen = crc_len;
      }

    mac_field_frak::mac_field_frak()
    {
    }

    mac_field_frak::~mac_field_frak()
    {
    }

  } /* namespace lpwan */
} /* namespace gr */

