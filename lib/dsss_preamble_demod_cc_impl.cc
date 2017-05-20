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
#include <lpwan/dsss_codes.h>
#include "dsss_preamble_demod_cc_impl.h"

namespace gr {
    namespace lpwan {

        dsss_preamble_demod_cc::sptr
        dsss_preamble_demod_cc::make(int sf, int sps, int preamble_length, bool sfd_present)
        {
            return gnuradio::get_initial_sptr
                    (new dsss_preamble_demod_cc_impl(sf, sps, preamble_length, sfd_present));
        }

        /*
         * The private constructor
         */
        dsss_preamble_demod_cc_impl::dsss_preamble_demod_cc_impl(int sf, int sps, int preamble_length, bool sfd_present)
                : gr::sync_block("dsss_preamble_demod_cc",
                                 gr::io_signature::make(1, 1, sizeof(gr_complex)),
                                 gr::io_signature::make(1, 1, sizeof(gr_complex))),
                  d_sps(sps),
                  d_sf(sf)
        {
            if (preamble_length == 16) {
                d_shr = dsss_codes::preamble16;
                if (sfd_present) {
                    d_shr.insert(d_shr.end(), dsss_codes::sfd16.begin(), dsss_codes::sfd16.end());
                }
            } else if (preamble_length == 32) {
                d_shr = dsss_codes::preamble32;
                if (sfd_present) {
                    d_shr.insert(d_shr.end(), dsss_codes::sfd32.begin(), dsss_codes::sfd32.end());
                }
            } else {
                std::runtime_error("Preamble Length has to be 16 or 32");
            }

            d_shr_length = preamble_length + (sfd_present ? 8 : 0);

            set_history(d_sps * d_sf * d_shr_length);
        }

        /*
         * Our virtual destructor.
         */
        dsss_preamble_demod_cc_impl::~dsss_preamble_demod_cc_impl()
        {
        }


        int
        dsss_preamble_demod_cc_impl::work(int noutput_items,
                                          gr_vector_const_void_star &input_items,
                                          gr_vector_void_star &output_items)
        {
            const gr_complex *in = (const gr_complex *) input_items[0];
            gr_complex *out = (gr_complex *) output_items[0];

            for (int n = 0; n < noutput_items; n++) {
                out[n] = 0;
                for (int i = 1; i < d_shr_length; i++) {
                    out[n] -= (in[n + i * d_sps * d_sf]) * d_shr[i];
                }
            }
            return noutput_items;
        }

    } /* namespace lpwan */
} /* namespace gr */

