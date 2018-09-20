/* -*- c++ -*- */
/* 
 * Copyright 2018 Felix Wunsch <felix.wunsch@kit.edu>.
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

#ifndef INCLUDED_LPWAN_FDDSM_MOD_BCB_IMPL_H
#define INCLUDED_LPWAN_FDDSM_MOD_BCB_IMPL_H

#include <lpwan/fddsm_mod_bcb.h>
#include <lpwan/fddsm_modulator_kernel.h>

#include <vector>

namespace gr {
  namespace lpwan {

    class fddsm_mod_bcb_impl : public fddsm_mod_bcb
    {
     private:
      unsigned int d_bps;
      unsigned int d_packet_len_bytes;

      std::unique_ptr<fddsm_modulator_kernel> d_mod_kernel;
      unsigned int d_packet_len_symbols;

     public:
      fddsm_mod_bcb_impl(unsigned int bps, unsigned int packet_len_bytes);
      ~fddsm_mod_bcb_impl();

      void forecast(int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);
    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_FDDSM_MOD_BCB_IMPL_H */

