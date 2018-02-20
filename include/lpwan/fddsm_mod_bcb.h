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


#ifndef INCLUDED_LPWAN_FDDSM_MOD_BCB_H
#define INCLUDED_LPWAN_FDDSM_MOD_BCB_H

#include <lpwan/api.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
  namespace lpwan {

    /*!
     * \brief Modulates a bit stream using Full Diversity Differential Spatial Modulation (FD-DSM). Input are unpacked
     * bits, output are a stream of antenna indices (0/1) and corresponding complex constellation symbols.
     * \ingroup lpwan
     *
     */
    class LPWAN_API fddsm_mod_bcb : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<fddsm_mod_bcb> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of lpwan::fddsm_mod_bcb.
       *
       * To avoid accidental use of raw pointers, lpwan::fddsm_mod_bcb's
       * constructor is in a private implementation
       * class. lpwan::fddsm_mod_bcb::make is the public interface for
       * creating new instances.
       */
      static sptr make(int bps, int packet_len_bytes);
    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_FDDSM_MOD_BCB_H */

