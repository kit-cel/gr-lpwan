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
#include "dsss_despread_simple_cc_impl.h"
#include <lpwan/dsss_codes.h>
#include <volk/volk.h>

namespace gr {
  namespace lpwan {

    dsss_despread_simple_cc::sptr
    dsss_despread_simple_cc::make(std::string len_tag, int spread_factor, int seed, bool reset_per_symbol, int ovsf_code_index, int ovsf_log2_spread_factor)
    {
      return gnuradio::get_initial_sptr
        (new dsss_despread_simple_cc_impl(len_tag, spread_factor, seed, reset_per_symbol, ovsf_code_index, ovsf_log2_spread_factor));
    }

    /*
     * The private constructor
     */
    dsss_despread_simple_cc_impl::dsss_despread_simple_cc_impl(std::string len_tag, int spread_factor, int seed, bool reset_per_symbol, int ovsf_code_index, int ovsf_log2_spread_factor)
      : gr::tagged_stream_block("dsss_despread_simple_cc",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex)), len_tag),
               d_sf(spread_factor),
               d_seed(seed),
               d_reset_per_symbol(reset_per_symbol),
               d_ovsf_code_index(ovsf_code_index),
               d_ovsf_log2_spread_factor(ovsf_log2_spread_factor)
    {
        //generate spreading codes
        d_code = dsss_codes::generate_combined_code_f(seed, d_ovsf_code_index, d_ovsf_log2_spread_factor, d_sf * 512);
    }

    /*
     * Our virtual destructor.
     */
    dsss_despread_simple_cc_impl::~dsss_despread_simple_cc_impl()
    {
    }

    int
    dsss_despread_simple_cc_impl::calculate_output_stream_length(const gr_vector_int &ninput_items)
    {
      return ninput_items[0]/d_sf ;
    }

    int
    dsss_despread_simple_cc_impl::work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const gr_complex *in = (const gr_complex *) input_items[0];
        gr_complex *out = (gr_complex *) output_items[0];

        int nin = ninput_items[0];
        int nout = nin / d_sf;
        if(nin % d_sf != 0)
            throw std::runtime_error("Wrong Packetsize for Despreading");

        int sf = d_sf;

        int code_pos=0;
        for(int i=0; i<nout; i++){
            volk_32fc_32f_dot_prod_32fc_u(out+i, in+i*sf, &d_code[code_pos], sf);
            out[i] /= sf;
            code_pos+=sf;
            if(d_reset_per_symbol){
                code_pos=0;
            }


        }


        return nout;
    }

  } /* namespace lpwan */
} /* namespace gr */


