/* -*- c++ -*- */
/* 
 * Copyright 2017 Kristian Maier <kristian.maier@gmx.de>.
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


#ifndef INCLUDED_LPWAN_MAC_CRC_H
#define INCLUDED_LPWAN_MAC_CRC_H

#include <lpwan/api.h>
#include <boost/crc.hpp>

namespace gr {
  namespace lpwan {

    /*!
     * \brief calculation of CRC
     *
     */
    class lpwan_API mac_crc
    {
    public:
        void append_crc(std::vector<uint8_t> &data);
        bool check_crc_and_remove(std::vector<uint8_t> &data);

        unsigned long calc_crc(const std::vector<uint8_t> &data);
        bool check_crc(const std::vector<uint8_t> &data);

        int get_crclen(){ return d_crclen;}

        mac_crc(int crclen);
      ~mac_crc();
    private:
        int d_crclen;

        boost::crc_optimal<16, 0x1021, 0, 0, false, false> d_crc16;
        boost::crc_optimal<32, 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, false, false> d_crc32;

        const unsigned long crc32_remainder = 955982468;
        const unsigned long crc16_remainder = 0;
    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_MAC_CRC_H */

