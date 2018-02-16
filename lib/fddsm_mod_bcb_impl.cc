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
#include "fddsm_mod_bcb_impl.h"

namespace gr {
  namespace lpwan {

    fddsm_mod_bcb::sptr
    fddsm_mod_bcb::make(int bps, int packet_len_bytes)
    {
      return gnuradio::get_initial_sptr
        (new fddsm_mod_bcb_impl(bps, packet_len_bytes));
    }

    /*
     * The private constructor
     */
    fddsm_mod_bcb_impl::fddsm_mod_bcb_impl(int bps, int packet_len_bytes)
      : gr::block("fddsm_mod_bcb",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make2(2, 2, sizeof(char), sizeof(gr_complex))),
              d_bps(bps),
              d_packet_len_bytes(packet_len_bytes)
    {
      // Generate lookup tables for Aq and Vl
    }

    /*
     * Our virtual destructor.
     */
    fddsm_mod_bcb_impl::~fddsm_mod_bcb_impl()
    {
    }

    void
    fddsm_mod_bcb_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    fddsm_mod_bcb_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const char *bits_in = (const char *) input_items[0];
      char *antenna_out = (char *) output_items[0];
      gr_complex *symbol_out = (gr_complex *) output_items[1];

      consume_each (noutput_items);
      return noutput_items;
    }

  } /* namespace lpwan */
} /* namespace gr */

