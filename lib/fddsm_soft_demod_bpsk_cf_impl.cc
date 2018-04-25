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
    fddsm_soft_demod_bpsk_cf::make(unsigned int packet_len_coded_bits, std::string len_tag)
    {
      return gnuradio::get_initial_sptr
        (new fddsm_soft_demod_bpsk_cf_impl(packet_len_coded_bits, len_tag));
    }

    /*
     * The private constructor
     */
    fddsm_soft_demod_bpsk_cf_impl::fddsm_soft_demod_bpsk_cf_impl(unsigned int packet_len_coded_bits,
                                                                 std::string len_tag)
      : gr::tagged_stream_block("fddsm_soft_demod_bpsk_cf",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(float)), len_tag),
        d_packet_len(packet_len_coded_bits)
    {
      // create demodulator kernel instance for L=2 (BPSK)
      unsigned int bps = 2;
      bool reset_after_each_call = true;
      d_demod_kernel = std::unique_ptr<fddsm_demodulator_kernel>(new fddsm_demodulator_kernel(bps, reset_after_each_call));
    }

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

      // check if there is a tag with initialization values on the first samples
      std::vector<tag_t> v;
      get_tags_in_range(v, 0, nitems_read(0), nitems_read(0)+1, pmt::intern("sop"));
      if(not v.empty())
      {
        gr_complex yp[2] = {gr_complex(pmt::to_complex(pmt::tuple_ref(v[0].value, 0))),
                            gr_complex(pmt::to_complex(pmt::tuple_ref(v[0].value, 1)))};
        d_demod_kernel->set(yp);
      }
      
      d_demod_kernel->demodulate_soft(out, in, d_packet_len, 1);

      // Tell runtime system how many output items we produced.
      return d_packet_len;
    }

  } /* namespace lpwan */
} /* namespace gr */

