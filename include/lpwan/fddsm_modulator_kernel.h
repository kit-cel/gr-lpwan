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


#ifndef INCLUDED_LPWAN_FDDSM_MODULATOR_KERNEL_H
#define INCLUDED_LPWAN_FDDSM_MODULATOR_KERNEL_H

#include <lpwan/api.h>

namespace gr {
  namespace lpwan {

    /*!
     * \brief Kernel for modulating FD-DSM signals from unpacked bits.
     * Every block of bits is assumed to represent an entire frame after which the modulator is reset.
     * The parameter bps refers to the bits per space-time symbol, i.e., log2(2*L).
     *
     */
    class LPWAN_API fddsm_modulator_kernel
    {
    public:
      fddsm_modulator_kernel(unsigned int bps);
      ~fddsm_modulator_kernel();

      void modulate(gr_complex *dst_symbols, char *dst_antenna, const char *src_bits, unsigned long num_bits);

    private:
      unsigned int d_bps;
      unsigned int d_L;

      std::vector<char> d_q;
      std::vector<  std::vector<char>  > d_antenna_indices;
      std::vector<  std::vector<gr_complex>  > d_constellation_symbols;
    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_FDDSM_MODULATOR_KERNEL_H */

