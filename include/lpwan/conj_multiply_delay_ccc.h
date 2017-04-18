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


#ifndef INCLUDED_LPWAN_CONJ_MULTIPLY_DELAY_CCC_H
#define INCLUDED_LPWAN_CONJ_MULTIPLY_DELAY_CCC_H

#include <lpwan/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace lpwan {

    /*!
     * \brief <+description of block+>
     * \ingroup lpwan
     *
     */
    class lpwan_API conj_multiply_delay_ccc : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<conj_multiply_delay_ccc> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of lpwan::conj_multiply_delay_ccc.
       *
       * To avoid accidental use of raw pointers, lpwan::conj_multiply_delay_ccc's
       * constructor is in a private implementation
       * class. lpwan::conj_multiply_delay_ccc::make is the public interface for
       * creating new instances.
       */
      static sptr make(int delay);
    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_CONJ_MULTIPLY_DELAY_CCC_H */

