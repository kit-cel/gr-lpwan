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
#include "conj_multiply_delay_ccc_impl.h"
#include <volk/volk.h>

namespace gr {
  namespace lpwan {

    conj_multiply_delay_ccc::sptr
    conj_multiply_delay_ccc::make(int delay)
    {
      return gnuradio::get_initial_sptr
        (new conj_multiply_delay_ccc_impl(delay));
    }

    /*
     * The private constructor
     */
    conj_multiply_delay_ccc_impl::conj_multiply_delay_ccc_impl(int delay)
      : gr::sync_block("conj_multiply_delay_ccc",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex))),
        d_delay(delay)
    {
        set_history(d_delay);

    }

    /*
     * Our virtual destructor.
     */
    conj_multiply_delay_ccc_impl::~conj_multiply_delay_ccc_impl()
    {
    }

    int
    conj_multiply_delay_ccc_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
        const gr_complex *in0 = (const gr_complex *) input_items[0];
        gr_complex *out = (gr_complex *) output_items[0];

        volk_32fc_x2_multiply_conjugate_32fc(out, in0, in0+d_delay, noutput_items);

      return noutput_items;
    }

  } /* namespace lpwan */
} /* namespace gr */

