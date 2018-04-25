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
#include <lpwan/fddsm_demodulator_kernel.h>

namespace gr {
  namespace lpwan {

    fddsm_demodulator_kernel::fddsm_demodulator_kernel(unsigned int bps, bool reset_after_each_call) :
    d_bps(bps),
    d_reset_after_each_call(reset_after_each_call)
    {
      if(d_bps != 2)
      {
        throw std::runtime_error("Invalid choice of bps. Only bps=2 (L=2, BPSK) is implemented so far.");
      }
      this->reset();
    }

    fddsm_demodulator_kernel::~fddsm_demodulator_kernel()
    {
    }

    void
    fddsm_demodulator_kernel::demodulate_soft(float *dst_softbits, const gr_complex *src_symbols,
                                              unsigned long num_bits, unsigned int stepsize)
    {
      if(num_bits % d_bps)
      {
        throw std::runtime_error("Invalid choice of num_bits, must be an integer multiple of bps.");
      }

      // initialize Y(t-1) = I, allocate Y(t), imaginary unit j and soft bits u
      gr_complex y[2];
      auto j = gr_complex(0, 1);
      float u0, u1;

      float d[2][2]; // distance to the transmit symbol S(q,l)
      for(auto i = 0; i < num_bits/2; ++i){
        y[0] = *src_symbols;
        y[1] = *(src_symbols+stepsize);
        src_symbols += 2 * stepsize;

        d[0][0] = std::real(std::conj(y[0]) * d_yp[0] + std::conj(y[1]) * d_yp[1]); // distance to 00b
        d[0][1] = - d[0][0]; // 11b
        d[1][0] = std::real(j* (std::conj(y[0]) * d_yp[1] + std::conj(y[1]) * d_yp[0])); // 01b
        d[1][1] = -d[1][0]; // 10b

        d_yp[0] = y[0];
        d_yp[1] = y[1];

        u0 = std::min(d[0][1], d[1][1]) - std::min(d[0][0], d[1][0]);
        u1 = std::min(d[0][1], d[1][0]) - std::min(d[0][0], d[1][1]);
        *dst_softbits = u0;
        *(dst_softbits+1) = u1;
        dst_softbits += 2;
      }

      if(d_reset_after_each_call)
      {
        this->reset();
      }
    }
  } /* namespace lpwan */
} /* namespace gr */

