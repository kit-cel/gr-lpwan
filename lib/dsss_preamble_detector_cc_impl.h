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

#ifndef INCLUDED_LPWAN_DSSS_PREAMBLE_DETECTOR_CC_IMPL_H
#define INCLUDED_LPWAN_DSSS_PREAMBLE_DETECTOR_CC_IMPL_H

#include <lpwan/dsss_preamble_detector_cc.h>

namespace gr {
  namespace lpwan {

    class dsss_preamble_detector_cc_impl : public dsss_preamble_detector_cc
    {
     private:
        std::vector<float> d_freqs;
        int d_shr_len;
        int d_sf;
        int d_sps;
        float* d_volk_buffer;
        int d_samples_per_peak_slot;
        int d_chiprate;
        std::vector<float> d_filter_taps;
        float d_threshold_factor;

        float d_h2_sum_2;
        float d_h_sum_2;


     public:
      dsss_preamble_detector_cc_impl(std::vector<float> freqs, int shr_len, int sf, int sps, int chiprate, std::vector<float> filter_taps);
      ~dsss_preamble_detector_cc_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_DSSS_PREAMBLE_DETECTOR_CC_IMPL_H */

