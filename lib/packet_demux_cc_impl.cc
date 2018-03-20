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

namespace gr {
  namespace lpwan {

    packet_demux_cc::sptr
    packet_demux_cc::make(std::string tag_key, unsigned int frame_length_samples)
    {
      return gnuradio::get_initial_sptr
        (new packet_demux_cc_impl(tag_key, frame_length_samples));
    }

    /*
     * The private constructor
     */
    packet_demux_cc_impl::packet_demux_cc_impl(std::string tag_key, unsigned int frame_length_samples)
      : gr::block("packet_demux_cc",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex))),
        d_tag_key(tag_key),
        d_frame_length_samples(frame_length_samples)
    {
      set_tag_propagation_policy(TPP_DONT);
      set_output_multiple(d_frame_length_samples);
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
      ninput_items_required[0] = d_frame_length_samples * 2; // x2 to have some headroom
    }

    int
    packet_demux_cc_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      auto *in = (const gr_complex *) input_items[0];
      auto *out = (gr_complex *) output_items[0];

      auto nitems_consumed = 0l;
      auto nitems_produced = 0l;

      unsigned long max_num_frames = noutput_items / d_frame_length_samples;

      // search only in the first half of the buffer such that any tags found have an entire frame following them
      std::vector<tag_t> v;
      get_tags_in_range(v, 0, nitems_read(0), nitems_read(0) + d_frame_length_samples);
      
      // if no tags are found, do nothing an consume the buffer
      if(v.empty()) {
        nitems_consumed = d_frame_length_samples;
        nitems_produced = 0;
      }
      // if there are frame start tags, copy as many frames as the output buffer allows
      else {
        unsigned long vlen = v.size();
        auto num_output_frames = std::min(vlen, max_num_frames);
        for (auto i = 0; i < num_output_frames; ++i) {
          std::memcpy(out + i * d_frame_length_samples,
                      in + v[i].offset - nitems_read(0),
                      d_frame_length_samples * sizeof(gr_complex));
          add_item_tag(0, nitems_written(0) + i*d_frame_length_samples, pmt::intern(d_tag_key), pmt::PMT_T);
        }

        if(num_output_frames == vlen) // all detected frames were copied to the output, consume entire search window
        {
          nitems_consumed = d_frame_length_samples;
        }
        else // there were more frames than could be copied, consume only up to the beginning of the first frame that was not copied
        {
          nitems_consumed = v[num_output_frames].offset - nitems_read(0);
        }
        nitems_produced = num_output_frames * d_frame_length_samples;
      }

      consume_each (nitems_consumed);
      return nitems_produced;
    }

  } /* namespace lpwan */
} /* namespace gr */

