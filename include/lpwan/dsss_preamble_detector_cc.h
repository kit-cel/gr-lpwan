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


#ifndef INCLUDED_LPWAN_DSSS_PREAMBLE_DETECTOR_CC_H
#define INCLUDED_LPWAN_DSSS_PREAMBLE_DETECTOR_CC_H

#include <lpwan/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace lpwan {

    /*!
     * \brief Detects the preamble.
     *
     * This block is basically a peak detector. It tags the raw rx filtered input data with the
     * detected peak positions (key: "sop"), an estimation for the frequency offset and an estimation of the
     * SNR. It works together with the despreading block.
     *
     * \ingroup lpwan
     *
     */
    class lpwan_API dsss_preamble_detector_cc : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<dsss_preamble_detector_cc> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of lpwan::dsss_preamble_detector_cc.
       *
       * @param freqs vector with the frequency hypothesis
       * @param shr_len length of the shr in bits
       * @param sf preamble spreading factor
       * @param sps samples per symbol
       * @param chiprate absolute chiprate
       * @param filter_taps filter taps of the Matched Filter, needed for SNR estimation
       * @return
       * */
      static sptr make(std::vector<float> freqs, int shr_len, int sf, int sps, int chiprate, std::vector<float> filter_taps);
    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_DSSS_PREAMBLE_DETECTOR_CC_H */

