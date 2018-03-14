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


#ifndef INCLUDED_LPWAN_SLIDING_DOTPROD_32F_X2_32F_H
#define INCLUDED_LPWAN_SLIDING_DOTPROD_32F_X2_32F_H

#include <lpwan/api.h>
#include <vector>

namespace gr {
  namespace lpwan {

    /*!
     * \brief Realizes a real-valued dot-product while storing the shift-register state.
     * Can be used to create filters and correlators.
     *
     */
    class LPWAN_API sliding_dotprod_32f_x2_32f
    {
    public:
      sliding_dotprod_32f_x2_32f(const std::vector<float> &taps);
      ~sliding_dotprod_32f_x2_32f();
      void dotprod(float* output, const float* input, unsigned int len);

    private:
      std::vector<float> d_taps;
      std::vector<float> d_buf;
      long d_bufpos;
      float single_dotprod(const float input);

    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_SLIDING_DOTPROD_32F_X2_32F_H */

