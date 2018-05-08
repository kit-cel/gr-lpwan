/* -*- c++ -*- */
/* 
 * Copyright 2018 <+YOU OR YOUR COMPANY+>.
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
 * aint with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_LPWAN_PACKET_DEMUX_CC_IMPL_H
#define INCLUDED_LPWAN_PACKET_DEMUX_CC_IMPL_H

#include <lpwan/packet_demux_cc.h>


namespace gr {
  namespace lpwan {

    class packet_demux_cc_impl : public packet_demux_cc
    {
     private:
      std::string d_tag_key;
      unsigned int d_frame_length_samples;
      unsigned int d_payload_length_samples;
      unsigned int d_downsampling_factor;
      unsigned int d_payload_length_symbols;
      static const unsigned int d_max_buffers = 100; // constrain maximum memory usage
      std::vector< std::vector< gr_complex > > d_bufvec; // queue holding the actual symbol buffers
      std::vector< uint64_t > d_next_abs_symbol_index; // absolute starting offset for every buffer in d_bufvec
      std::vector< unsigned int > d_buf_pos; // current writing position in a specific buffer in d_bufvec
      std::vector< pmt::pmt_t > d_tag_value; // value of frame start tag (initialization for differential demodulator)

     public:
      packet_demux_cc_impl(std::string tag_key, unsigned int frame_length_samples, unsigned int payload_length_samples, unsigned int downsampling_factor);
      ~packet_demux_cc_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_PACKET_DEMUX_CC_IMPL_H */

