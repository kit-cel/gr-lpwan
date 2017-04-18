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


#ifndef INCLUDED_LPWAN_DSSS_NORMALIZE_FF_H
#define INCLUDED_LPWAN_DSSS_NORMALIZE_FF_H

#include <lpwan/api.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
  namespace lpwan {

    /*!
     * \brief Normalizes one tagged stream block with length N.
     *
     * out = in / (Sum_N_x(abs(in[x])) / N)
     *
     * \ingroup lpwan
     *
     */
    class lpwan_API dsss_normalize_ff : virtual public gr::tagged_stream_block
    {
     public:
      typedef boost::shared_ptr<dsss_normalize_ff> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of lpwan::dsss_normalize_ff.
       *
       * @param len_tag length tag key
       * @return
       */
      static sptr make(std::string len_tag);
    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_DSSS_NORMALIZE_FF_H */

