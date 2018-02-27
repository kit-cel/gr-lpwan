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


#ifndef INCLUDED_LPWAN_PARTIAL_REPEAT_CC_H
#define INCLUDED_LPWAN_PARTIAL_REPEAT_CC_H

#include <lpwan/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace lpwan {

    /*!
     * \brief Returns chunks of length len_total of the incoming stream with overlap len_overlap.
     * \ingroup lpwan
     *
     */
    class LPWAN_API partial_repeat_cc : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<partial_repeat_cc> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of lpwan::partial_repeat_cc.
       *
       * To avoid accidental use of raw pointers, lpwan::partial_repeat_cc's
       * constructor is in a private implementation
       * class. lpwan::partial_repeat_cc::make is the public interface for
       * creating new instances.
       */
      static sptr make(int len_overlap, int len_total);
    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_PARTIAL_REPEAT_CC_H */

