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


#ifndef INCLUDED_LPWAN_FDDSM_PREAMBLE_DETECTOR_CC_H
#define INCLUDED_LPWAN_FDDSM_PREAMBLE_DETECTOR_CC_H

#include <lpwan/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace lpwan {

    /*!
     * \brief Searches for the given preamble based on the demodulator's soft-bit output.
     * If a preamble is detected, a tag is placed on the last symbol such that the payload decoder can use it as
     * reference.
     *
     * @param shr SHR in NRZ-coding (+-1)
     * @param sps Samples per symbol
     * @param spreading_factor Spreading factor
     * @param num_chips_gap length of the time gap between symbols in chips
     * @param alpha alpha for single-pole IIR used to average energy per polyphase
     * @param beta factor to control false alarm probability per polyphase
     * \ingroup lpwan
     *
     */
    class LPWAN_API fddsm_preamble_detector_cc : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<fddsm_preamble_detector_cc> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of lpwan::fddsm_preamble_detector_cc.
       *
       * To avoid accidental use of raw pointers, lpwan::fddsm_preamble_detector_cc's
       * constructor is in a private implementation
       * class. lpwan::fddsm_preamble_detector_cc::make is the public interface for
       * creating new instances.
       */
      static sptr make(std::vector<float> shr,
                       unsigned int sps,
                       unsigned int spreading_factor,
                       unsigned int num_chips_gap,
                       float alpha,
                       float beta,
                       std::vector<float> phase_increments,
                       unsigned int output_correlator_index);

      virtual void set_alpha(float alpha) = 0;
      virtual void set_beta(float beta) = 0;
      virtual void set_output_correlator(unsigned index) = 0;
    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_FDDSM_PREAMBLE_DETECTOR_CC_H */

