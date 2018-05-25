/* -*- c++ -*- */
/* 
 * Copyright 2018 Felix Wunsch.
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
#include "interpolating_spreading_cc_impl.h"
#include <volk/volk.h>

namespace gr {
  namespace lpwan {

    interpolating_spreading_cc::sptr
    interpolating_spreading_cc::make(std::vector<float> spreading_sequence, int spreading_factor, int nsymbols_before_reset)
    {
      return gnuradio::get_initial_sptr
        (new interpolating_spreading_cc_impl(spreading_sequence, spreading_factor, nsymbols_before_reset));
    }

    /*
     * The private constructor
     */
    interpolating_spreading_cc_impl::interpolating_spreading_cc_impl(std::vector<float> spreading_sequence, int spreading_factor, int nsymbols_before_reset)
      : gr::sync_interpolator("interpolating_spreading_cc",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex)), spreading_factor),
        d_sf(spreading_factor),
        d_nreset(nsymbols_before_reset),
        d_ctr(0)
    {
      d_code = std::vector<gr_complex>(spreading_sequence.size(), gr_complex(0, 0));
      for(auto i = 0; i < d_code.size(); ++i)
      {
        d_code[i] = gr_complex(spreading_sequence[i], 0.0f);
      }
      set_output_multiple(d_sf);
    }

    /*
     * Our virtual destructor.
     */
    interpolating_spreading_cc_impl::~interpolating_spreading_cc_impl()
    {
    }

    int
    interpolating_spreading_cc_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      auto *in = (const gr_complex *) input_items[0];
      auto *out = (gr_complex *) output_items[0];

      for(auto i = 0; i < noutput_items/d_sf; ++i)
      {
        volk_32fc_s32fc_multiply_32fc(out + d_sf * i, &d_code[d_ctr * d_sf], in[i], d_sf);
        d_ctr = (d_ctr + 1) % d_nreset;
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace lpwan */
} /* namespace gr */

