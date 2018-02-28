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
#include "partial_repeat_cc_impl.h"

namespace gr {
  namespace lpwan {

    partial_repeat_cc::sptr
    partial_repeat_cc::make(int stepsize, int len_total)
    {
      return gnuradio::get_initial_sptr
        (new partial_repeat_cc_impl(stepsize, len_total));
    }

    /*
     * The private constructor
     */
    partial_repeat_cc_impl::partial_repeat_cc_impl(int stepsize, int len_total)
      : gr::block("partial_repeat_cc",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex))),
        d_stepsize(stepsize),
        d_total(len_total)
    {
      set_output_multiple(d_total);
    }

    /*
     * Our virtual destructor.
     */
    partial_repeat_cc_impl::~partial_repeat_cc_impl()
    {
    }

    void
    partial_repeat_cc_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = d_total;
    }

    int
    partial_repeat_cc_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];

      memcpy(out, in, sizeof(gr_complex) * d_total);

      consume_each (d_stepsize);

      // Tell runtime system how many output items we produced.
      return d_total;
    }

  } /* namespace lpwan */
} /* namespace gr */

