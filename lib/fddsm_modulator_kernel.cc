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
#include <lpwan/fddsm_modulator_kernel.h>

#include <cstring>

namespace gr {
  namespace lpwan {

    fddsm_modulator_kernel::fddsm_modulator_kernel(int bps)
    {
      d_bps = bps;
      d_L = 1 << (d_bps-1);

      if(d_L > 8){ throw std::runtime_error("L > 8 is not supported.");}

      // Generate lookup tables for Aq and Vl
      d_antenna_indices = {{0,1}, {1,0}};
      if(d_L == 2)
      {
        auto omega = std::exp(gr_complex(0, M_PI/2));
        d_constellation_symbols = {{gr_complex(1,0), gr_complex(1,0)},
                                   {gr_complex(1,0)*omega, gr_complex(1,0)*omega},
                                   {gr_complex(-1,0)*omega, gr_complex(-1,0)*omega},
                                   {gr_complex(-1,0), gr_complex(-1,0)}};
        d_q = {0, 1, 1, 0};
      }
      else if (d_L == 4)
      {
        //std::cout << "ATTENTION: For L=4, the symbols don't align nicely with byte boundaries." << std::endl;
        auto omega = std::exp(gr_complex(0, M_PI/4));
        d_constellation_symbols = {{gr_complex(1,0), gr_complex(1,0)},
                                   {gr_complex(1,0)*omega, gr_complex(1,0)*omega},
                                   {gr_complex(0,1)*omega, gr_complex(0,1)*omega},
                                   {gr_complex(0,1), gr_complex(0,1)},
                                   {gr_complex(0,-1), gr_complex(0,-1)},
                                   {gr_complex(0,-1)*omega, gr_complex(0,-1)*omega},
                                   {gr_complex(-1,0), gr_complex(-1,0)},
                                   {gr_complex(-1,0)*omega, gr_complex(-1,0)*omega}};
        d_q = {0, 1, 1, 0, 1, 0, 0, 1};
      }
      else if(d_L == 8) {
        auto omega = std::exp(gr_complex(0, M_PI / 4));
        auto u2 = 3;
        for (auto l = 0; l < 2*d_L; ++l)
        {
          if(l < d_L)
          {
            d_q.push_back(0);
            d_constellation_symbols.push_back(
                std::vector<gr_complex>(
                    {std::exp(gr_complex(0,2*M_PI*l/d_L)), std::exp(gr_complex(0,2*M_PI*u2*l/d_L))}
                )
            );
          }
          else
          {
            d_q.push_back(1);
            d_constellation_symbols.push_back(
                std::vector<gr_complex>(
                    {std::exp(gr_complex(0,2*M_PI*l/d_L))*omega, std::exp(gr_complex(0,2*M_PI*u2*l/d_L))*omega}
                )
            );
          }
        }
      }
    }

    fddsm_modulator_kernel::~fddsm_modulator_kernel()
    {
    }

    void
    fddsm_modulator_kernel::modulate(gr_complex *dst_symbols, char *dst_antenna, const char *src_bits, unsigned long num_bits)
    {
      // check if all bits can be encoded properly
      if(num_bits % d_bps)
      {
          throw std::runtime_error("num_bits must be an integer multiple of bps.");
      }
      // this is the beginning of a frame, so reset s and q
      gr_complex s_prev[2] = {gr_complex(1, 0), gr_complex(1, 0)};
      char q_prev_state = 0;
      
      auto num_symbols = num_bits / d_bps;

      for(auto i = 0; i < num_symbols; ++i) // symbols means STBC matrices with two time slots each
      {
        // decimal index from input bit word
        auto idx = 0;
        for(auto n = 0; n < d_bps; ++n)
        {
          idx += (*src_bits) << (d_bps-1-n); // convert bps bits to decimal
          src_bits++;
        }

        // antenna indices
        auto q = d_q[idx];
        std::memcpy(dst_antenna, &d_antenna_indices[q_prev_state^q][0], 2);
        dst_antenna += 2;

        // symbol indices, multiply previous with current symbols
        // NOTE: This scheme results for the non-zero elements after multiplication of 2 (possibly permutated) diagonal matrices
        *(dst_symbols) = s_prev[q] * d_constellation_symbols[idx][0];
        *(dst_symbols+1) = s_prev[q^1] * d_constellation_symbols[idx][1];
        dst_symbols += 2;

        /*(std::cout << "b0b1b2b3: " << int(*(src_bits-4)) << int(*(src_bits-3)) << int(*(src_bits-2)) << int(*(src_bits-1)) << "; qprev/q: " << int(q_prev_state) << "/" << int(q);
        std::cout << "; x0/x1: " << x0 << "/" << x1 << "; s0/s1: " << s0 << "/" << s1 << "; idx: " << idx << std::endl;*/

        q_prev_state = q_prev_state^q; // if q==0, the order is maintained, else it is switched
        std::memcpy(s_prev, dst_symbols-2, 2 * sizeof(gr_complex));
      }
    }

  } /* namespace lpwan */
} /* namespace gr */

