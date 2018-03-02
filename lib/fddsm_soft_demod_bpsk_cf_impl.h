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
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_LPWAN_FDDSM_SOFT_DEMOD_BPSK_CF_IMPL_H
#define INCLUDED_LPWAN_FDDSM_SOFT_DEMOD_BPSK_CF_IMPL_H

#include <lpwan/fddsm_soft_demod_bpsk_cf.h>

namespace gr {
  namespace lpwan {

    class fddsm_soft_demod_bpsk_cf_impl : public fddsm_soft_demod_bpsk_cf
    {
     private:
      static constexpr auto d_L = 2;
      static constexpr auto d_omega = gr_complex(0, 1);
      int d_packet_len; // packet len in coded bits

     protected:
      int calculate_output_stream_length(const gr_vector_int &ninput_items);

     public:
      fddsm_soft_demod_bpsk_cf_impl(int packet_len_coded_bits, std::string len_tag);
      ~fddsm_soft_demod_bpsk_cf_impl();

      // Where all the action really happens
      int work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_FDDSM_SOFT_DEMOD_BPSK_CF_IMPL_H */

