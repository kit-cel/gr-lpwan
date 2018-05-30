/* -*- c++ -*- */
/* 
 * Copyright 2018 Felix Wunsch.
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

#ifndef INCLUDED_LPWAN_INTERPOLATING_SPREADING_CC_IMPL_H
#define INCLUDED_LPWAN_INTERPOLATING_SPREADING_CC_IMPL_H

#include <lpwan/interpolating_spreading_cc.h>

namespace gr {
  namespace lpwan {

    class interpolating_spreading_cc_impl : public interpolating_spreading_cc
    {
     private:
      int d_sf;
      int d_nreset;
      std::vector<gr_complex> d_code;
      int d_ctr;

     public:
      interpolating_spreading_cc_impl(std::vector<float> spreading_sequence, int spreading_factor, int nsymbols_before_reset);
      ~interpolating_spreading_cc_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_INTERPOLATING_SPREADING_CC_IMPL_H */

