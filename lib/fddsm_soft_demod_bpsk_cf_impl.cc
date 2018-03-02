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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "fddsm_soft_demod_bpsk_cf_impl.h"

namespace gr {
  namespace lpwan {

    fddsm_soft_demod_bpsk_cf::sptr
    fddsm_soft_demod_bpsk_cf::make(int packet_len_coded_bits, std::string len_tag)
    {
      return gnuradio::get_initial_sptr
        (new fddsm_soft_demod_bpsk_cf_impl(packet_len_coded_bits, len_tag));
    }

    /*
     * The private constructor
     */
    fddsm_soft_demod_bpsk_cf_impl::fddsm_soft_demod_bpsk_cf_impl(int packet_len_coded_bits, std::string len_tag)
      : gr::tagged_stream_block("fddsm_soft_demod_bpsk_cf",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(float)), len_tag),
        d_packet_len(packet_len_coded_bits)
    {}

    /*
     * Our virtual destructor.
     */
    fddsm_soft_demod_bpsk_cf_impl::~fddsm_soft_demod_bpsk_cf_impl()
    {
    }

    int
    fddsm_soft_demod_bpsk_cf_impl::calculate_output_stream_length(const gr_vector_int &ninput_items)
    {
      return d_packet_len;
    }

    int
    fddsm_soft_demod_bpsk_cf_impl::work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      auto *in = (const gr_complex *) input_items[0];
      auto *out = (float *) output_items[0];
      
      // initialize Y(t-1) = I, allocate Y(t), imaginary unit j and soft bits u
      auto yp1 = gr_complex(1, 0);
      auto yp2 = gr_complex(1, 0);
      gr_complex y1, y2;
      auto j = gr_complex(0, 1);
      float u0, u1;

      float d[2][2]; // distance to the transmit symbol S(q,l)
      for(auto i = 0; i < d_packet_len/2; ++i){
        y1 = *in;
        y2 = *(in+1);
        in += 2;

        d[0][0] = std::real(std::conj(y1) * yp1 + std::conj(y2) * yp2); // distance to 00b
        d[0][1] = - d[0][0]; // 11b
        d[1][0] = std::real(j* (std::conj(y1) * yp2 + std::conj(y2) * yp1)); // 01b
        d[1][1] = -d[1][0]; // 10b

        yp1 = y1;
        yp2 = y2;

        u0 = std::min(d[0][1], d[1][1]) - std::min(d[0][0], d[1][0]);
        u1 = std::min(d[0][1], d[1][0]) - std::min(d[0][0], d[1][1]);
        *out = u0;
        *(out+1) = u1;
        out += 2;
      }

      // Tell runtime system how many output items we produced.
      return d_packet_len;
    }

  } /* namespace lpwan */
} /* namespace gr */

