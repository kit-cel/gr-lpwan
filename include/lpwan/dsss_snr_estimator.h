/* -*- c++ -*- */
/* 
 * Copyright 2017 Kristian Maier <kristian.maier@gmx.de>.
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


#ifndef INCLUDED_LPWAN_DSSS_SNR_ESTIMATOR_H
#define INCLUDED_LPWAN_DSSS_SNR_ESTIMATOR_H

#include <lpwan/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace lpwan {

    /*!
     * \brief Estimates the SNR of received packets. It discards CRC-failed packets and operates
     * only on packets with correct CRC checks. The estimation is based on comparison of the remodulated
     * data (convolutional encoding) and the soft demodulated rx data (before convolutional decoding).
     *
     * \ingroup lpwan
     *
     */
    class lpwan_API dsss_snr_estimator : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<dsss_snr_estimator> sptr;

      /*!
       *
       * \brief Return a shared_ptr to a new instance of lpwan::dsss_snr_estimator.
       *
       * @param psdu_size size of the psdu in bytes (16,24,32)
       * @param sf spreading factor
       * @return
       */
      static sptr make(int psdu_size, int sf);
    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_DSSS_SNR_ESTIMATOR_H */

