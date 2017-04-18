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
#include "dsss_spreading_bb_impl.h"
#include "lpwan/dsss_codes.h"

namespace gr {
    namespace lpwan {

        dsss_spreading_bb::sptr
        dsss_spreading_bb::make(std::string len_tag, int spread_factor, int seed, bool reset_per_symbol,
                                int ovsf_code_index, int ovsf_log_spread_factor)
        {
            return gnuradio::get_initial_sptr
                    (new dsss_spreading_bb_impl(len_tag, spread_factor, seed, reset_per_symbol, ovsf_code_index,
                                                ovsf_log_spread_factor));
        }

        /*
         * The private constructor
         */
        dsss_spreading_bb_impl::dsss_spreading_bb_impl(std::string len_tag, int spread_factor, int seed,
                                                       bool reset_per_symbol, int ovsf_code_index,
                                                       int ovsf_log_spread_factor)
                : gr::sync_interpolator("dsss_spreading_bb",
                                        gr::io_signature::make(1, 1, sizeof(unsigned char)),
                                        gr::io_signature::make(1, 1, sizeof(unsigned char)), spread_factor),
                  d_spread_factor(spread_factor),
                  d_seed(seed),
                  d_reset_per_symbol(reset_per_symbol),
                  d_code_index(ovsf_code_index),
                  d_ovsf_log_sf(ovsf_log_spread_factor)
        {
            //generate code with max needed length
            d_code = dsss_codes::generate_combined_code_uc(seed, d_code_index, d_ovsf_log_sf, d_spread_factor * 512);
            d_code_invert = dsss_codes::invert_code_uc(d_code);
            d_len_tag_key = pmt::mp(len_tag);
            set_tag_propagation_policy(TPP_DONT);
        }

        /*
         * Our virtual destructor.
         */
        dsss_spreading_bb_impl::~dsss_spreading_bb_impl()
        {
        }


        int
        dsss_spreading_bb_impl::work(int noutput_items,
                                     gr_vector_const_void_star &input_items,
                                     gr_vector_void_star &output_items)
        {
            const unsigned char *in = (const unsigned char *) input_items[0];
            unsigned char *out = (unsigned char *) output_items[0];

            int nin = noutput_items / d_spread_factor;

            //printf("\nnin: %d, nout: %d\n", nin, noutput_items);

            if (d_packet_len == -1) {

                std::vector<tag_t> v;
                get_tags_in_window(v, 0, 0, 1, d_len_tag_key);

                if (v.size() != 0) {

                    d_packet_len = pmt::to_long(v[0].value);

                    //set new tag with new length
                    pmt::pmt_t val;
                    val = pmt::from_long(d_packet_len * d_spread_factor);
                    add_item_tag(0, nitems_written(0), d_len_tag_key, val);

                    //printf("new packet: %d\n", d_packet_len);
                    //printf("new packet offset tag: %d\n", v[0].offset);
                }
            }

            if (d_packet_len == -1) {
                throw std::runtime_error("Error: Missing a required length tag on port 0 at item #0.\n");
            }


            for (int i = 0; i < nin; i++) {
                //spreading, copy code or negative code
                if (in[i] == 0) {
                    memcpy(out + i * d_spread_factor, &d_code[0] + d_chip_pos, d_spread_factor * sizeof(unsigned char));
                } else {
                    memcpy(out + i * d_spread_factor, &d_code_invert[0] + d_chip_pos,
                           d_spread_factor * sizeof(unsigned char));
                }

                d_bit_pos++;

                if (!d_reset_per_symbol) {
                    d_chip_pos += d_spread_factor;
                }
                if (d_bit_pos == d_packet_len) {
                    //one packet is finished, stop spreading and reset all for next work call
                    d_chip_pos = 0;
                    d_bit_pos = 0;
                    d_packet_len = -1;

                    //printf("return finish:%d\n", (i+1)*d_spread_factor);
                    return (i + 1) * d_spread_factor;
                }
            }
            //printf("return %d\n", noutput_items);
            return noutput_items;
        }

    } /* namespace lpwan */
} /* namespace gr */

