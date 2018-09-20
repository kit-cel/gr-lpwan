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


#ifndef INCLUDED_LPWAN_PACKET_DEMUX_CC_H
#define INCLUDED_LPWAN_PACKET_DEMUX_CC_H

#include <lpwan/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace lpwan {

    /*!
     * \brief Demultiplexes overlapping frames of length frame_length_samples with frame starts denoted by tag_key.
     * Whenever a tag is found, the payload symbols are stored into a buffer until the entire frame is recorded and the
     * symbols can be written to the output buffer.
     * \ingroup lpwan
     *
     */
    class LPWAN_API packet_demux_cc : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<packet_demux_cc> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of lpwan::packet_demux_cc.
       *
       * To avoid accidental use of raw pointers, lpwan::packet_demux_cc's
       * constructor is in a private implementation
       * class. lpwan::packet_demux_cc::make is the public interface for
       * creating new instances.
       */
      static sptr make(std::string tag_key, unsigned int frame_length_samples, unsigned int payload_length_samples, std::vector<float> spreading_code, unsigned int spreading_factor, unsigned int sps, std::vector<float> pulse, bool reset_after_each_symbol, std::vector<float> delta_phi);
    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_PACKET_DEMUX_CC_H */

