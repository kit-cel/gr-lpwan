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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "fddsm_preamble_detector_cc_impl.h"
#include <volk/volk.h>

namespace gr {
  namespace lpwan {

    fddsm_preamble_detector_cc::sptr
    fddsm_preamble_detector_cc::make(std::vector<float> shr, float threshold, unsigned int sps, unsigned int spreading_factor, unsigned int num_chips_gap)
    {
      return gnuradio::get_initial_sptr
        (new fddsm_preamble_detector_cc_impl(shr, threshold, sps, spreading_factor, num_chips_gap));
    }

    /*
     * The private constructor
     */
    fddsm_preamble_detector_cc_impl::fddsm_preamble_detector_cc_impl(std::vector<float> shr, float threshold, unsigned int sps, unsigned int spreading_factor, unsigned int num_chips_gap)
      : gr::sync_block("fddsm_preamble_detector_cc",
              gr::io_signature::make3(3, 3, sizeof(gr_complex), sizeof(gr_complex), sizeof(float)),
              gr::io_signature::make3(3, 3, sizeof(gr_complex), sizeof(float), sizeof(float))),
        d_shr(shr),
        d_threshold(threshold),
        d_sps(sps),
        d_spreading_factor(spreading_factor),
        d_num_chips_gap(num_chips_gap)

    {
      // num branches == length of one space-time block in samples
      d_stepsize = d_sps * (d_spreading_factor + d_num_chips_gap);
      d_num_branches = 2 * d_stepsize;

      // initialize the intermediate buffer, the FD-DSM decoders and SHR filters
      auto len_buf_per_filter_branch = 100;
      for(auto i = 0; i < d_num_branches; ++i)
      {
        d_buf.push_back(std::vector<float>(len_buf_per_filter_branch));
        d_demod_kernels.push_back(std::unique_ptr<fddsm_demodulator_kernel>(new fddsm_demodulator_kernel(2, false)));
        d_dotprod_kernels.push_back(std::unique_ptr<sliding_dotprod_32f_x2_32f>(new sliding_dotprod_32f_x2_32f(d_shr)));
      }

      // make sure that even the latest polyphase can access the following symbol for demodulation
      set_output_multiple(d_num_branches + d_stepsize);
    }

    /*
     * Our virtual destructor.
     */
    fddsm_preamble_detector_cc_impl::~fddsm_preamble_detector_cc_impl()
    {
    }

    int
    fddsm_preamble_detector_cc_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const auto *corr_in = (const gr_complex *) input_items[0];// + d_hist_len - 1;
      const auto *signal_in = (const gr_complex *) input_items[1];
      const auto *power_in = (const float *) input_items[2];// + d_hist_len - 1; // FIXME offset here?
      auto *signal_out = (gr_complex *) output_items[0];
      auto *corr_out = (float *) output_items[1];
      auto *threshold_out = (float *) output_items[2];

      // Demodulate FD-DSM signal, output soft bits to intermediate buffer and correlate with preamble.
      // Effectively, we create a polyphase filterbank by splitting/deinterleaving the input in 2 * d_stepsize polyphase
      // components, filtering each component and interleaving the filter/correlator output again.
      //auto nbits_to_process = std::min(static_cast<size_t>(noutput_items) / d_num_branches * 2, d_buf[0].size());
      auto nbits_to_process = 2; // FIXME: DEBUG configuration.
      for(auto i = 0; i < d_num_branches; ++i)
      {
        d_demod_kernels[i]->demodulate_soft(&d_buf[i][0], corr_in + i, nbits_to_process, d_stepsize);
      }
      for(auto i = 0; i < nbits_to_process/2; ++i)
      {
        float tmp[2];
        for(auto j = 0; j < d_num_branches; ++j)
        {
          d_dotprod_kernels[j]->dotprod(tmp, &d_buf[j][0], 2);
          corr_out[i * d_num_branches + j] = tmp[1];
          //d_dotprod_kernels[i]->dotprod(corr_out + j * d_stepsize + i, &d_buf[i][j], 1);
        }
      }

      // Copy signal from input to output
      auto nitems_processed = nbits_to_process * d_stepsize;
      std::memcpy(signal_out, signal_in, sizeof(gr_complex) * nitems_processed);

      // Find correlation peaks that exceed the threshold and attach tags at the respective positions.
      for(auto i = 0; i < nitems_processed; ++i)
      {
        threshold_out[i] = std::sqrt(power_in[i]) * d_spreading_factor * d_shr.size() * d_threshold; //FIXME find a sensible value
        if(corr_out[i] > threshold_out[i])
        {
          auto offset = i;// + (d_shr.size() - 1) * (d_spreading_factor + d_num_chips_gap) * d_sps;
          auto value = pmt::make_tuple(
              pmt::from_complex(std::real(corr_in[offset]), std::imag(corr_in[offset])),
              pmt::from_complex(std::real(corr_in[offset + d_stepsize]), std::imag(corr_in[offset + d_stepsize])));
          add_item_tag(0, nitems_written(0) + offset, pmt::intern("sop"), value);
          add_item_tag(1, nitems_written(1) + offset, pmt::intern("sop"), value);
        }

      }

      // Tell runtime system how many output items we produced.
      return nitems_processed;
    }

  } /* namespace lpwan */
} /* namespace gr */

