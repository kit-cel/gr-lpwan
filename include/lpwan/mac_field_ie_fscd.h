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


#ifndef INCLUDED_LPWAN_MAC_FIELD_IE_FSCD_H
#define INCLUDED_LPWAN_MAC_FIELD_IE_FSCD_H

#include <lpwan/api.h>
#include "mac_field_header_ie.h"
#include "mac_field_frame.h"
#include "mac_format.h"

namespace gr {
  namespace lpwan {

#define FSCD_BITSTART_SEC_FRAG          1
#define FSCD_BITLEN_SEC_FRAG            1

#define FSCD_BITSTART_TID               7
#define FSCD_BITLEN_TID                 6

#define FSCD_BITSTART_FRAK_POLICY       13
#define FSCD_BITLEN_FRAK_POLICY         2

#define FSCD_BITSTART_FICS_LEN          15
#define FSCD_BITLEN_FICS_LEN            1

#define FSCD_BITSTART_PSDU_SIZE         16
#define FSCD_BITLEN_PSDU_SIZE           10

#define FSCD_BITSTART_SRC_PAN_PRES      26
#define FSCD_BITLEN_SRC_PAN_PRES        1

#define FSCD_BITSTART_DEST_PAN_PRES     27
#define FSCD_BITLEN_DEST_PAN_PRES       1

#define FSCD_BITSTART_SRC_ADDR_MODE     28
#define FSCD_BITLEN_SRC_ADDR_MODE       2

#define FSCD_BITSTART_DEST_ADDR_MODE    30
#define FSCD_BITLEN_DEST_ADDR_MODE      2


        /*!
     * \brief fscd header ie
     *
     */
    class lpwan_API mac_field_ie_fscd : public mac_field_header_ie
    {
    public:

        std::vector<uint8_t> get_data();
        void set_data(const std::vector<uint8_t> &v);

        uint8_t get_sec_frag()      {return d_sec_frag;}
        uint8_t get_tid()           {return d_tid;}
        uint8_t get_frak_policy()   {return d_frak_policy;}
        uint8_t get_fics_len()      {return d_fics_len;}
        uint16_t get_psdu_size()    {return d_psdu_size;}
        address_info get_address_info(){return d_ai;}
        uint32_t get_psdu_ctr()     {return d_psdu_ctr;}

        void set_sec_frag(uint8_t sf)      {d_sec_frag = sf;}
        void set_tid(uint8_t tid)           {d_tid = tid;}
        void set_frak_policy(uint8_t fp)   {d_frak_policy = fp;}
        void set_fics_len(uint8_t fl)      {d_fics_len = fl;}
        void set_psdu_size(uint16_t ps)     {d_psdu_size = ps;}
        void set_address_info(address_info &a)  {d_ai = a;}

        mac_field_ie_fscd(std::vector<uint8_t> v);
        mac_field_ie_fscd();
      ~mac_field_ie_fscd();
    private:

        uint8_t d_sec_frag;
        uint8_t d_tid;
        uint8_t d_frak_policy;
        uint8_t d_fics_len;
        uint16_t d_psdu_size;

        address_info d_ai;
        uint32_t d_psdu_ctr;

    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_MAC_FIELD_IE_FSCD_H */

