/* -*- c++ -*- */
/* 
 * Copyright 2018 Felix Wunsch.
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


#ifndef INCLUDED_LPWAN_INTERPOLATING_SPREADING_CC_H
#define INCLUDED_LPWAN_INTERPOLATING_SPREADING_CC_H

#include <lpwan/api.h>
#include <gnuradio/sync_interpolator.h>

namespace gr {
  namespace lpwan {

    /*!
     * \brief <+description of block+>
     * \ingroup lpwan
     *
     */
    class LPWAN_API interpolating_spreading_cc : virtual public gr::sync_interpolator
    {
     public:
      typedef boost::shared_ptr<interpolating_spreading_cc> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of lpwan::interpolating_spreading_cc.
       *
       * To avoid accidental use of raw pointers, lpwan::interpolating_spreading_cc's
       * constructor is in a private implementation
       * class. lpwan::interpolating_spreading_cc::make is the public interface for
       * creating new instances.
       */
      static sptr make(std::vector<float> spreading_sequence, int spreading_factor, int nsymbols_before_reset);
    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_INTERPOLATING_SPREADING_CC_H */

