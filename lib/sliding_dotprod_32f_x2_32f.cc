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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <lpwan/sliding_dotprod_32f_x2_32f.h>
#include <volk/volk.h>
#include <cstring>

namespace gr {
  namespace lpwan {

    sliding_dotprod_32f_x2_32f::sliding_dotprod_32f_x2_32f(const std::vector<float> &taps)
    : d_taps(taps)
    {
      d_buf.resize(10 * d_taps.size());  // make the buffer 10x the size of the taps to avoid too frequent copying.
      std::memset(&d_buf[0], 0, sizeof(float) * d_buf.size());
      d_bufpos = -1;
    }

    sliding_dotprod_32f_x2_32f::~sliding_dotprod_32f_x2_32f()
    {
    }

    void
    sliding_dotprod_32f_x2_32f::dotprod(float* output, const float* input, unsigned int len)
    {
      for(auto i = 0; i < len; ++i)
      {
        output[i] = single_dotprod(input[i]);
      }
    }

    float
    sliding_dotprod_32f_x2_32f::single_dotprod(const float input)
    {
      if(d_bufpos < 0)
      {
        std::memcpy(&d_buf[d_buf.size()-d_taps.size()], &d_buf[0], sizeof(float) * (d_taps.size()-1));
        d_bufpos = d_buf.size()-d_taps.size()-1;
      }

      d_buf[d_bufpos] = input;
      float ret;
      volk_32f_x2_dot_prod_32f(&ret, &d_buf[0]+d_bufpos, &d_taps[0], d_taps.size());
      d_bufpos--;

      return ret;
    }

  } /* namespace lpwan */
} /* namespace gr */

