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


#ifndef INCLUDED_LPWAN_DSSS_DESPREAD_SIMPLE_CC_H
#define INCLUDED_LPWAN_DSSS_DESPREAD_SIMPLE_CC_H

#include <lpwan/api.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
  namespace lpwan {

    /*!
     * \brief Despreads succeeding LECIM DSSS packets.
     *
     * This tagged stream block despreads timesynced succeeding spreaded payload data. Useful for simulations.
     *
     * \ingroup lpwan
     *
     */
    class lpwan_API dsss_despread_simple_cc : virtual public gr::tagged_stream_block
    {
     public:
      typedef boost::shared_ptr<dsss_despread_simple_cc> sptr;

      /*!
       * !
       * \brief Return a shared_ptr to a new instance of lpwan::dsss_despread_simple_cc.
       *
       * @param len_tag length tag key
       * @param spread_factor gold code spreading factor
       * @param seed seed of the cold gode generator for the data payload
       * @param reset_per_symbol
       * @param ovsf_code_index ovsf code index [0, 2^ovsf_log_sf-1]
       * @param ovsf_log2_spread_factor ovsf spreading factor [0, 8]
       * @return
       */
      static sptr make(std::string len_tag, int spread_factor, int seed, bool reset_per_symbol, int ovsf_code_index, int ovsf_log2_spread_factor);
    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_DSSS_DESPREAD_SIMPLE_CC_H */

