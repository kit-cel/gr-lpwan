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

#ifndef INCLUDED_LPWAN_DSSS_DESPREAD_SIMPLE_CC_IMPL_H
#define INCLUDED_LPWAN_DSSS_DESPREAD_SIMPLE_CC_IMPL_H

#include <lpwan/dsss_despread_simple_cc.h>

namespace gr {
  namespace lpwan {

    class dsss_despread_simple_cc_impl : public dsss_despread_simple_cc
    {
     private:
        std::vector<float> d_code;
        int d_sf;
        int d_seed;
        int d_reset_per_symbol;
        int d_ovsf_code_index;
        int d_ovsf_log2_spread_factor;

     protected:
      int calculate_output_stream_length(const gr_vector_int &ninput_items);

     public:
      dsss_despread_simple_cc_impl(std::string len_tag, int spread_factor, int seed, bool reset_per_symbol, int ovsf_code_index, int ovsf_log2_spread_factor);
      ~dsss_despread_simple_cc_impl();

      // Where all the action really happens
      int work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_DSSS_DESPREAD_SIMPLE_CC_IMPL_H */

