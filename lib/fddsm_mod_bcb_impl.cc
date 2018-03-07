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
              d_packet_len_bytes(packet_len_bytes),
              d_packet_len_symbols(d_packet_len_bytes * 8 / bps)
    {
      d_mod_kernel = std::unique_ptr<fddsm_modulator_kernel>(new fddsm_modulator_kernel(d_bps));

      set_relative_rate(2.0/d_bps);
      set_output_multiple(d_packet_len_bytes * 8 / d_bps * 2);
    }

    /*
     * Our virtual destructor.
     */
    fddsm_mod_bcb_impl::~fddsm_mod_bcb_impl()
    {
    }

    void
    fddsm_mod_bcb_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = d_packet_len_bytes * 8; // bytes come in unpacked
    }

    int
    fddsm_mod_bcb_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      auto *bits_in = (const char *) input_items[0];
      auto *antenna_out = (char *) output_items[0];
      auto *symbol_out = (gr_complex *) output_items[1];

      // modulate an entire frame, write directly to output buffers
      d_mod_kernel->modulate(symbol_out, antenna_out, bits_in, d_packet_len_bytes * 8);

      consume_each(d_packet_len_bytes * 8);
      return 2 * d_packet_len_symbols;
    }

  } /* namespace lpwan */
} /* namespace gr */

