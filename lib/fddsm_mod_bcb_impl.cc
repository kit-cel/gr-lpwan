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
#include "fddsm_mod_bcb_impl.h"

namespace gr {
  namespace lpwan {

    fddsm_mod_bcb::sptr
    fddsm_mod_bcb::make(int bps, int packet_len_bytes)
    {
      return gnuradio::get_initial_sptr
        (new fddsm_mod_bcb_impl(bps, packet_len_bytes));
    }

    /*
     * The private constructor
     */
    fddsm_mod_bcb_impl::fddsm_mod_bcb_impl(int bps, int packet_len_bytes)
      : gr::block("fddsm_mod_bcb",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make2(2, 2, sizeof(char), sizeof(gr_complex))),
              d_bps(bps),
              d_packet_len_bytes(packet_len_bytes),
              d_q_prev_state(0),
              d_s_prev({1, 1})
    {
      d_L = 1 << (bps-1); // 1 bit is encoded into the antenna information
      if(d_L > 8){ throw std::runtime_error("L > 0 is not supported.");}

      // Generate lookup tables for Aq and Vl
      if(d_L == 2)
      {
        auto omega = std::exp(gr_complex(0, M_PI/2));
        d_constellation_symbols = {{gr_complex(1,0), gr_complex(1,0)},
                                   {gr_complex(1,0)*omega, gr_complex(1,0)*omega},
                                   {gr_complex(-1,0)*omega, gr_complex(-1,0)*omega},
                                   {gr_complex(-1,0), gr_complex(-1,0)}};
        d_antenna_indices = {{0,1}, {1,0}, {1,0}, {0,1}};
      }
      else if (d_L == 4)
      {
        auto omega = std::exp(gr_complex(0, M_PI/4));
        d_constellation_symbols = {{gr_complex(1,0), gr_complex(1,0)},
                                   {gr_complex(1,0)*omega, gr_complex(1,0)*omega},
                                   {gr_complex(0,1)*omega, gr_complex(0,1)*omega},
                                   {gr_complex(0,1), gr_complex(0,1)},
                                   {gr_complex(0,-1), gr_complex(0,-1)},
                                   {gr_complex(0,-1)*omega, gr_complex(0,-1)*omega},
                                   {gr_complex(-1,0), gr_complex(-1,0)},
                                   {gr_complex(-1,0)*omega, gr_complex(-1,0)*omega}};
        d_antenna_indices = {{0,1}, {1,0}, {1,0}, {0,1}, {0,1}, {1,0}, {1,0}, {0,1}};
      }
      else if(d_L == 8) {
        auto omega = std::exp(gr_complex(0, M_PI / 4));
        auto u2 = 3;
        for (auto l = 0; l < 2*d_L; ++l)
        {
          if(l < d_L)
          {
            d_antenna_indices.push_back(std::vector<char>({0, 1}));
            d_constellation_symbols.push_back(
                std::vector<gr_complex>(
                    {std::exp(gr_complex(0,2*M_PI*l/d_L)), std::exp(gr_complex(0,2*M_PI*u2*l/d_L))}
                )
            );
          }
          else
          {
            d_antenna_indices.push_back(std::vector<char>({1, 0}));
            d_constellation_symbols.push_back(
                std::vector<gr_complex>(
                    {std::exp(gr_complex(0,2*M_PI*l/d_L))*omega, std::exp(gr_complex(0,2*M_PI*u2*l/d_L))*omega}
                )
            );
          }
        }
      }

      set_output_multiple(2);
      set_tag_propagation_policy(TPP_DONT);
      set_relative_rate(2.0/d_bps);
    }

    /*
     * Our virtual destructor.
     */
    fddsm_mod_bcb_impl::~fddsm_mod_bcb_impl()
    {
    }

    void
    fddsm_mod_bcb_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = d_bps;
    }

    int
    fddsm_mod_bcb_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const char *bits_in = (const char *) input_items[0];
      char *antenna_out = (char *) output_items[0];
      gr_complex *symbol_out = (gr_complex *) output_items[1];

      auto nbits_in = ninput_items[0];
      auto nsym_out = nbits_in / d_bps; // refers to symbol matrices, i.e. 2 constellation symbols with antenna index.

      std::cout << "work(): bits_in/symbols_out " << nbits_in << "/" << nsym_out << std::endl;

      for(auto i = 0; i < nsym_out; ++i)
      {
        // decimal index from input bit word
        auto idx = 0;
        for(auto n = 0; n < d_bps; ++n)
        {
          idx += (*bits_in) << (d_bps-1-n); // convert bps bits to decimal
          bits_in++;
        }

        // antenna indices
        auto q = d_antenna_indices[idx][0]; // if q==0, the order is maintained, else it is switched
        memcpy(antenna_out, &d_antenna_indices[(d_q_prev_state + q) % 2][0], 2);
        antenna_out += 2;

        // symbol indices, multiply previous with current symbols
        gr_complex s0, s1;
        if(q == 0)
        {
          if(d_q_prev_state == 0)
          {
            s0 = d_s_prev[0] * d_constellation_symbols[idx][0];
            s1 = d_s_prev[1] * d_constellation_symbols[idx][1];
          }
          else
          {
            s0 = d_s_prev[1] * d_constellation_symbols[idx][1];
            s1 = d_s_prev[0] * d_constellation_symbols[idx][0];
          }
        }
        else // q == 1
        {
          if (d_q_prev_state == 0)
          {
            s0 = d_s_prev[1] * d_constellation_symbols[idx][0];
            s1 = d_s_prev[0] * d_constellation_symbols[idx][1];
          }
          else
          {
            s0 = d_s_prev[1] * d_constellation_symbols[idx][0];
            s1 = d_s_prev[0] * d_constellation_symbols[idx][1];
          }
        }
        *(symbol_out) = s0;
        *(symbol_out+1) = s1;
        symbol_out += 2;

        std::cout << "b0b1: " << int(*(bits_in-2)) << int(*(bits_in-1)) << "; qprev/q: " << int(d_q_prev_state) << "/" << int(q);
        std::cout << "; " << "s0/s1: " << s0 << "/" << s1 << std::endl;

        d_q_prev_state = (d_q_prev_state + q) % 2;
        d_s_prev = {*(symbol_out-2), *(symbol_out-1)};
      }

      consume_each(nsym_out * d_bps);
      return 2 * nsym_out;
    }

  } /* namespace lpwan */
} /* namespace gr */

