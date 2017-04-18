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


#ifndef INCLUDED_LPWAN_DSSS_SPREADING_BB_H
#define INCLUDED_LPWAN_DSSS_SPREADING_BB_H

#include <lpwan/api.h>
#include <gnuradio/sync_interpolator.h>

namespace gr {
  namespace lpwan {

    /*!
     * \brief Spreads the already upsampled/repeated data with the goldcode.
     *
     * Input and output as unpacked bytes.
     *
     * \ingroup lpwan
     *
     */
    class lpwan_API dsss_spreading_bb : virtual public gr::sync_interpolator
    {
     public:
      typedef boost::shared_ptr<dsss_spreading_bb> sptr;

      /*!
       *
       * \brief Return a shared_ptr to a new instance of lpwan::dsss_spreading_bb.
       *
       * Spreads the incoming samples with the configured code. This block behaves like a tagged stream block,
       * although it is an interpolator, because the output size of one packet could be very big. (big buffer, latency)
       *
       * @param len_tag length tag key
       * @param spread_factor spreading factor
       * @param seed goldcode seed
       * @param reset_per_symbol reset the goldcode after each symbol / after spread_factor input samples
       * @param ovsf_code_index
       * @param ovsf_log_spread_factor
       * @return
       */
      static sptr make(std::string len_tag, int spread_factor, int seed, bool reset_per_symbol, int ovsf_code_index, int ovsf_log_spread_factor);
    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_DSSS_SPREADING_BB_H */

