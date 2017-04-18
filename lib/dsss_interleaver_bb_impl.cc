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
#include "dsss_interleaver_bb_impl.h"
#include "lpwan/dsss_codes.h"

namespace gr {
    namespace lpwan {

        dsss_interleaver_bb::sptr
        dsss_interleaver_bb::make(std::string len_tag) {
            return gnuradio::get_initial_sptr
                    (new dsss_interleaver_bb_impl(len_tag));
        }

        /*
         * The private constructor
         */
        dsss_interleaver_bb_impl::dsss_interleaver_bb_impl(std::string len_tag)
                : gr::tagged_stream_block("dsss_interleaver_bb",
                                          gr::io_signature::make(1, 1, sizeof(unsigned char)),
                                          gr::io_signature::make(1, 1, sizeof(unsigned char)), len_tag) {

        }

        /*
         * Our virtual destructor.
         */
        dsss_interleaver_bb_impl::~dsss_interleaver_bb_impl() {
        }

        int
        dsss_interleaver_bb_impl::calculate_output_stream_length(const gr_vector_int &ninput_items) {
            return ninput_items[0];
        }

        int
        dsss_interleaver_bb_impl::work(int noutput_items,
                                       gr_vector_int &ninput_items,
                                       gr_vector_const_void_star &input_items,
                                       gr_vector_void_star &output_items) {
            const unsigned char *in = (const unsigned char *) input_items[0];
            unsigned char *out = (unsigned char *) output_items[0];

            int nin = ninput_items[0];

            for (int i = 0; i < nin; i++) {
                if (nin == 256)
                {
                    out[i] = in[dsss_codes::intlvr_seq256[i]];
                }
                else if (nin == 384)
                {
                    out[i] = in[dsss_codes::intlvr_seq384[i]];
                }
                else if (nin == 512)
                {
                    out[i] = in[dsss_codes::intlvr_seq512[i]];
                }
                else {
                    throw std::runtime_error("wrong input length");
                }
            }

            return nin;
        }

    } /* namespace lpwan */
} /* namespace gr */

