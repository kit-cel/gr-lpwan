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


#ifndef INCLUDED_LPWAN_DSSS_PREAMBLE_DEMOD_CC_H
#define INCLUDED_LPWAN_DSSS_PREAMBLE_DEMOD_CC_H

#include <lpwan/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace lpwan {

    /*!
     * \brief Preamble Demodulation/Correlation
     *
     * out[n] = Sum_N_i(in(n+i*sf*sps) * preamble(i))
     *
     * \ingroup lpwan
     *
     */
    class lpwan_API dsss_preamble_demod_cc : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<dsss_preamble_demod_cc> sptr;

      /*!
       *
       * \brief Return a shared_ptr to a new instance of lpwan::dsss_preamble_demod_cc.
       *
       * @param sf  spreading factor of shr
       * @param sps samples per symbol
       * @param preamble_length length of the preamble in bits
       * @param sfd_present
       * @return
       */
      static sptr make(int sf, int sps, int preamble_length, bool sfd_present);
    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_DSSS_PREAMBLE_DEMOD_CC_H */

