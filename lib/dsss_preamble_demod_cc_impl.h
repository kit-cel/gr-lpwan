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

#ifndef INCLUDED_LPWAN_DSSS_PREAMBLE_DEMOD_CC_IMPL_H
#define INCLUDED_LPWAN_DSSS_PREAMBLE_DEMOD_CC_IMPL_H

#include <lpwan/dsss_preamble_demod_cc.h>

namespace gr {
  namespace lpwan {

    class dsss_preamble_demod_cc_impl : public dsss_preamble_demod_cc
    {
     private:
        std::vector<float> d_shr;
        int d_sps;
        int d_shr_length;
        int d_sf;

     public:
      dsss_preamble_demod_cc_impl(int sf, int sps, int preamble_length, bool sfd_present);
      ~dsss_preamble_demod_cc_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_DSSS_PREAMBLE_DEMOD_CC_IMPL_H */

