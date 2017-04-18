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
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "dsss_normalize_ff_impl.h"
#include "volk/volk.h"

namespace gr {
  namespace lpwan {

    dsss_normalize_ff::sptr
    dsss_normalize_ff::make(std::string len_tag)
    {
      return gnuradio::get_initial_sptr
        (new dsss_normalize_ff_impl(len_tag));
    }

    /*
     * The private constructor
     */
    dsss_normalize_ff_impl::dsss_normalize_ff_impl(std::string len_tag)
      : gr::tagged_stream_block("dsss_normalize_ff",
              gr::io_signature::make(1, 1, sizeof(float)),
              gr::io_signature::make(1, 1, sizeof(float)), len_tag)
    {}

    /*
     * Our virtual destructor.
     */
    dsss_normalize_ff_impl::~dsss_normalize_ff_impl()
    {}

    int
    dsss_normalize_ff_impl::calculate_output_stream_length(const gr_vector_int &ninput_items)
    {
      return ninput_items[0] ;
    }

    int
    dsss_normalize_ff_impl::work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const float *in = (const float *) input_items[0];
        float *out = (float *) output_items[0];

        int nin = ninput_items[0];

        float mean=0;
        for (int i = 0; i < nin; ++i) {
            mean += in[i] > 0 ? in[i] : -in[i];
        }
        mean = mean / nin;
        memcpy(out, in, nin*sizeof(float));
        volk_32f_s32f_normalize(out, mean, (unsigned int)nin);

        return nin;
    }

  } /* namespace lpwan */
} /* namespace gr */

