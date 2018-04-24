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

#ifndef INCLUDED_LPWAN_FDDSM_PREAMBLE_DETECTOR_CC_IMPL_H
#define INCLUDED_LPWAN_FDDSM_PREAMBLE_DETECTOR_CC_IMPL_H

#include <lpwan/fddsm_preamble_detector_cc.h>
#include <lpwan/fddsm_demodulator_kernel.h>
#include <lpwan/sliding_dotprod_32f_x2_32f.h>

#include <memory>

namespace gr {
  namespace lpwan {

    class fddsm_preamble_detector_cc_impl : public fddsm_preamble_detector_cc
    {
     private:
      std::vector<float> d_shr;
      float d_threshold;
      std::vector<std::unique_ptr<fddsm_demodulator_kernel> > d_demod_kernels;
      std::vector<std::unique_ptr<sliding_dotprod_32f_x2_32f> > d_dotprod_kernels;
      unsigned int d_sps;
      unsigned int d_spreading_factor;
      unsigned int d_num_chips_gap;

      unsigned d_stepsize;
      unsigned d_num_branches;
      std::vector<std::vector<float> > d_buf;

     public:
      fddsm_preamble_detector_cc_impl(std::vector<float> shr, float threshold, unsigned int sps, unsigned int spreading_factor, unsigned int num_chips_gap);
      ~fddsm_preamble_detector_cc_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);

      void set_threshold(float threshold){ d_threshold = threshold; }; // not threadsafe
    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_FDDSM_PREAMBLE_DETECTOR_CC_IMPL_H */

