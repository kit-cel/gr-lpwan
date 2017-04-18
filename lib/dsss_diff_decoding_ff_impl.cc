/* -*- c++ -*- */
/* 
 * Copyright 2016 Kristian Maier <kristian.maier@gmx.de>.
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
 * the Free Software Foundation0, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "dsss_diff_decoding_ff_impl.h"

namespace gr {
  namespace lpwan {

    dsss_diff_decoding_ff::sptr
    dsss_diff_decoding_ff::make(std::string len_tag)
    {
      return gnuradio::get_initial_sptr
        (new dsss_diff_decoding_ff_impl(len_tag));
    }

    /*
     * The private constructor
     */
    dsss_diff_decoding_ff_impl::dsss_diff_decoding_ff_impl(std::string len_tag)
      : gr::tagged_stream_block("dsss_diff_decoding_ff",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex)), len_tag)
    {}

    /*
     * Our virtual destructor.
     */
    dsss_diff_decoding_ff_impl::~dsss_diff_decoding_ff_impl()
    {
    }

    int
    dsss_diff_decoding_ff_impl::calculate_output_stream_length(const gr_vector_int &ninput_items)
    {
      int noutput_items = ninput_items[0];
      return noutput_items ;
    }

    int
    dsss_diff_decoding_ff_impl::work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const gr_complex *in = (const gr_complex *) input_items[0];
        gr_complex *out = (gr_complex *) output_items[0];

        int nin = ninput_items[0];

        //TODO: decode first bit with known phase from preamble, first bit is random for now :(
        out[0] = -in[0]*std::abs(in[0]);
        for(int i=1; i<nin; i++){
            out[i] = -in[i] * std::conj(in[i-1]);
        }

      return nin;
    }

  } /* namespace lpwan */
} /* namespace gr */

