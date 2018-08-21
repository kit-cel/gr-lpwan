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
 * aint with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "packet_demux_cc_impl.h"
#include <volk/volk.h>
#include <algorithm>


namespace gr {
  namespace lpwan {

    packet_demux_cc::sptr
    packet_demux_cc::make(std::string tag_key, unsigned int frame_length_samples, unsigned int payload_length_samples, std::vector<float> spreading_code, unsigned int spreading_factor, unsigned int sps, std::vector<float> pulse, bool reset_after_each_symbol, std::vector<float> delta_phi)
    {
      return gnuradio::get_initial_sptr
        (new packet_demux_cc_impl(tag_key, frame_length_samples, payload_length_samples, spreading_code, spreading_factor, sps, pulse, reset_after_each_symbol, delta_phi));
    }

    /*
     * The private constructor
     */
    packet_demux_cc_impl::packet_demux_cc_impl(std::string tag_key,
                                               unsigned int frame_length_samples,
                                               unsigned int payload_length_samples,
                                               std::vector<float> spreading_code,
                                               unsigned int spreading_factor,
                                               unsigned int sps,
                                               std::vector<float> pulse,
                                               bool reset_after_each_symbol,
                                               std::vector<float> delta_phi)
      : gr::block("packet_demux_cc",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex))),
        d_tag_key(tag_key),
        d_frame_length_samples(frame_length_samples),
        d_payload_length_samples(payload_length_samples),
        d_code(spreading_code),
        d_sf(spreading_factor),
        d_sps(sps),
        d_reset_after_each_symbol(reset_after_each_symbol),
        d_delta_phi(delta_phi)
    {
      //d_downsampling_factor = d_sf * d_sps - (d_sps - 1) + d_pulse.size() - 1; // remove trailing zeros after upsampling, convolve with pulse
      d_downsampling_factor = d_sf * d_sps + pulse.size() - 1;
      d_payload_length_symbols = d_payload_length_samples / d_downsampling_factor;
      auto nsymbols_in_code = d_code.size() / d_sf;
      d_filtered_code.resize(d_downsampling_factor * nsymbols_in_code); // reserve space for up to d_code / d_sf symbols (which should equal the payload length)
      //std::cout << "Downsampling factor: " << d_downsampling_factor << ". Filtered code length: " << d_filtered_code.size() << ". payload length (symbols): " << d_payload_length_symbols << std::endl;

      float tmp[pulse.size()];
      for(auto i = 0; i < nsymbols_in_code; ++i)
      {
        for(auto j = 0; j < d_sf; ++j)
        {
          volk_32f_s32f_multiply_32f(tmp, &pulse[0], d_code[i*d_sf + j], pulse.size());
          volk_32f_x2_add_32f(&d_filtered_code[0] + i * d_downsampling_factor + j * d_sps, &d_filtered_code[0] + i * d_downsampling_factor + j * d_sps, tmp, pulse.size());
        }
      }

      if(d_delta_phi.size() < 1)
      {
        throw std::runtime_error("packet_demux_cc requires at least one phase increment.");
      }
      for(auto i = 0; i < d_delta_phi.size(); ++i)
      {
        d_freqshifted_pulses.push_back(std::vector<gr_complex>(d_filtered_code.size()));
        for(auto j = 0; j < d_filtered_code.size(); ++j)
        {
          d_freqshifted_pulses[i][j] = d_filtered_code[j] * std::exp(gr_complex(0, - j * d_delta_phi[i]));
        }
      }

      set_tag_propagation_policy(TPP_DONT);
      set_output_multiple(d_payload_length_symbols);
    }

    /*
     * Our virtual destructor.
     */
    packet_demux_cc_impl::~packet_demux_cc_impl()
    {
    }

    void
    packet_demux_cc_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = d_downsampling_factor * 2;
    }

    int
    packet_demux_cc_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      auto *in = (const gr_complex *) input_items[0];
      auto *out = (gr_complex *) output_items[0];

      // search for frame start tags and append new buffers for them
      std::vector<tag_t> v;
      get_tags_in_range(v, 0, nitems_read(0), nitems_read(0) + ninput_items[0], pmt::intern(d_tag_key));
      for(auto i = 0; i < v.size(); ++i)
      {
        if(d_bufvec.size() <= d_max_buffers)
        {
          // NOTE: This could be optimized by statically allocating memory and keeping track of the position in the buffer,
          // therefore avoiding copy operations when new buffers are pushed to the back of the array.
          d_bufvec.push_back(std::vector<gr_complex>(d_payload_length_symbols, gr_complex(-1, -1)));
          d_buf_pos.push_back(0);
          d_next_abs_symbol_index.push_back(v[i].offset);
          pmt::pmt_t tag_value = v[i].value;
          d_phase_inc.push_back(pmt::to_float(pmt::dict_ref(tag_value, pmt::intern("delta_phi"), pmt::PMT_NIL)));
          d_phase.push_back(pmt::to_float(pmt::dict_ref(tag_value, pmt::intern("phi_start"), pmt::PMT_NIL))); // this is supposed to  create a continuous phase between preamble and payload
          d_phi_index.push_back(pmt::to_long(pmt::dict_ref(tag_value, pmt::intern("delta_phi_index"), pmt::PMT_NIL)));
          if(v[i].offset < nitems_read(0))
            throw std::runtime_error("Invalid tag offset!");
          d_tag_value.push_back(tag_value);
        }
        else
        {
          std::cout << "packet_demux_cc: Buffer queue full! Dropping " << v.size() - i << " tags." << std::endl;
        }
      }

      // fill existing buffers as far as possible
      for (auto i = 0; i < d_bufvec.size(); ++i) {
        // check if the buffer is already filled and if there are symbols in      //std::cout << "WORK index range: " << nitems_read(0) << " to " << nitems_read(0) + ninput_items[0] << " (" << ninput_items[0] << " items)" << std::endl;
        auto next_rel_symbol_index = d_next_abs_symbol_index[i] - nitems_read(0);
        gr_complex tmp;
        if (d_next_abs_symbol_index[i] < nitems_read(0) && d_buf_pos[i] < d_payload_length_symbols) {
          std::cerr << "i=" << i << "\tnext_abs_index=" << d_next_abs_symbol_index[i] << "\tnitems_read(0)="
                    << nitems_read(0) << "\tnext_rel_index=" << next_rel_symbol_index << "\tbuf_pos[i]="
                    << d_buf_pos[i] << std::endl;
          throw std::runtime_error("Invalid symbol index!");
        }
        while (d_buf_pos[i] < d_payload_length_symbols
               && (next_rel_symbol_index + d_downsampling_factor) < ninput_items[0]) {
          if (d_reset_after_each_symbol) {
            volk_32fc_x2_dot_prod_32fc(&tmp, in + next_rel_symbol_index, &d_freqshifted_pulses[d_phi_index[i]][0],
                                        d_downsampling_factor);
            d_bufvec[i][d_buf_pos[i]] = tmp * std::exp(gr_complex(0, - d_phase[i]));
            d_phase[i] = fmod(d_phase_inc[i] * d_downsampling_factor + d_phase[i], 2 * M_PI);
          } else {
            volk_32fc_x2_dot_prod_32fc(&d_bufvec[i][d_buf_pos[i]], in + next_rel_symbol_index,
                                       &d_freqshifted_pulses[d_phi_index[i]][0] + d_buf_pos[i] * d_downsampling_factor,
                                        d_downsampling_factor);

            //d_bufvec[i][d_buf_pos[i]] = tmp;// * std::exp(gr_complex(0, - d_phase[i]));
            //d_phase[i] = fmod(d_phase_inc[d_phi_index[i]] * d_downsampling_factor + d_phase[i], 2 * M_PI);
            /*d_bufvec[i][d_buf_pos[i]] = 0;
            for(int n = 0; n < d_downsampling_factor; ++n)
            {
              d_bufvec[i][d_buf_pos[i]] += in[next_rel_symbol_index + n] * d_filtered_code[d_buf_pos[i] * d_downsampling_factor + n] * std::exp(gr_complex(0, -((d_buf_pos[i] * d_downsampling_factor + n) * d_delta_phi[d_phi_index[i]])));
            }*/
          }
          d_next_abs_symbol_index[i] += d_downsampling_factor;
          next_rel_symbol_index += d_downsampling_factor;
          d_buf_pos[i] += 1;
        }
      }

      auto max_frames_to_return = noutput_items / d_payload_length_symbols;
      auto symbols_written = 0;
      for(auto i = 0; i < max_frames_to_return; ++i)
      {
        if(not d_bufvec.empty()) {
          if (d_buf_pos[0] == d_payload_length_symbols) // entire frame payload written to internal buffer
          {
            // copy to output and attach tag
            std::memcpy(out + symbols_written, &d_bufvec[0][0], d_payload_length_symbols * sizeof(gr_complex));
            add_item_tag(0, nitems_written(0) + i * d_payload_length_symbols, pmt::intern(d_tag_key), d_tag_value[0]);

            // cleanup
            d_bufvec.erase(d_bufvec.begin());
            d_buf_pos.erase(d_buf_pos.begin());
            d_tag_value.erase(d_tag_value.begin());
            d_next_abs_symbol_index.erase(d_next_abs_symbol_index.begin());
            d_phase.erase(d_phase.begin());
            d_phase_inc.erase(d_phase_inc.begin());
            d_phi_index.erase(d_phi_index.begin());

            symbols_written += d_payload_length_symbols;
          }
          else
          {
            break;
          }
        }
        else
        {
          break;
        }
      }

      auto samples_consumed = ninput_items[0] - d_downsampling_factor;
      consume_each(samples_consumed);
      //std::cout << "RETURN samples consumed: " << samples_consumed << std::endl << std::endl;
      return symbols_written;
    }

  } /* namespace lpwan */
} /* namespace gr */

