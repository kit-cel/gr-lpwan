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


#ifndef INCLUDED_LPWAN_DSSS_FILTER_CRC_PACKETS_H
#define INCLUDED_LPWAN_DSSS_FILTER_CRC_PACKETS_H

#include <lpwan/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace lpwan {

    /*!
     * \brief Filters out packets with unsuccessful CRC check.
     * \ingroup lpwan
     *
     */
    class lpwan_API dsss_filter_crc_packets : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<dsss_filter_crc_packets> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of lpwan::dsss_filter_crc_packets.
       *
       */
      static sptr make();
    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_DSSS_FILTER_CRC_PACKETS_H */

