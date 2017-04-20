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
    dsss_diff_decoding_ff::make(std::string len_tag, bool phase_ref, int shr_len)
    {
      return gnuradio::get_initial_sptr
        (new dsss_diff_decoding_ff_impl(len_tag, phase_ref, shr_len));
    }

    /*
     * The private constructor
     */
    dsss_diff_decoding_ff_impl::dsss_diff_decoding_ff_impl(std::string len_tag, bool phase_ref, int shr_len)
      : gr::tagged_stream_block("dsss_diff_decoding_ff",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex)), len_tag),
        d_phase_ref(phase_ref),
        d_shr_len(shr_len)
    {
    }

    /*
     * Our virtual destructor.
     */
    dsss_diff_decoding_ff_impl::~dsss_diff_decoding_ff_impl()
    {
    }

    int
    dsss_diff_decoding_ff_impl::calculate_output_stream_length(const gr_vector_int &ninput_items)
    {

        if(d_phase_ref){
            int noutput_items = ninput_items[0]-1;
            //for correct tag propagation
            set_relative_rate((double)noutput_items/ninput_items[0]);
            return noutput_items;
        }else{
            int noutput_items = ninput_items[0];
            set_relative_rate(1);
            return noutput_items;
        }


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

        //phase_ref needed for first bit in stream
        //there is one special case (preamble16 + sfd = 24) where the differential modded bits
        //end with an one

        if(d_phase_ref){
            if(d_shr_len != 24){
                //normal case
                for(int i=1; i<nin; i++){
                    out[i-1] = -in[i] * std::conj(in[i-1]);
                }
            }else{
                out[0] = +in[1] * std::conj(in[0]);
                for(int i=2; i<nin; i++){
                    out[i-1] = -in[i] * std::conj(in[i-1]);
                }
            }
            return nin-1;
        }else{
            for(int i=1; i<nin; i++){
                out[i] = -in[i] * std::conj(in[i-1]);
            }
            return nin;
        }


    }

  } /* namespace lpwan */
} /* namespace gr */

