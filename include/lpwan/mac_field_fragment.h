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


#ifndef INCLUDED_LPWAN_MAC_FIELD_FRAGMENT_H
#define INCLUDED_LPWAN_MAC_FIELD_FRAGMENT_H

#include <lpwan/api.h>
#include "mac_field.h"
#include "mac_field_frag_header.h"
#include "mac_crc.h"

namespace gr {
    namespace lpwan {


        #define FRAG_STATUS_NOT_SENT    0
        #define FRAG_STATUS_NO_ACK      1
        #define FRAG_STATUS_SUCCESS     2
        #define FRAG_STATUS_FAILED      3

        /*!
         * \brief fragment packet
         *
         */
        class lpwan_API mac_field_fragment : public mac_field {
        public:

            std::vector<uint8_t> get_data();
            void set_data(const std::vector<uint8_t> &v);
            void set_data(const std::vector<uint8_t> &v, int crc_len);

            const std::vector<uint8_t> get_payload(){return d_payload;}
            mac_field_frag_header* get_header(){ return &d_header;}
            int get_crclen(){return d_crclen;}
            int get_frag_len(){return d_frag_len;}
            bool get_crc_ok(){return d_crc_ok;}
            int get_sent_count(){return d_sent_count;}

            void set_header(mac_field_frag_header &h){d_header = h;}
            void set_payload(std::vector<uint8_t> &p){d_payload = p;}
            void inc_sent_count(){d_sent_count++;}
            void set_sent_count(int r){d_sent_count = r;}

            mac_field_fragment(const std::vector<uint8_t> &v, int crc_len);

            mac_field_fragment(const mac_field_frag_header &h, const std::vector<uint8_t> &payload, int crc_len,
                               int frag_len);

            mac_field_fragment();

            ~mac_field_fragment();

        private:
            mac_field_frag_header d_header;
            std::vector<uint8_t> d_payload;
            int d_crclen;
            int d_frag_len;
            bool d_crc_ok;
            int d_sent_count = 0;

        };

    } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_MAC_FIELD_FRAGMENT_H */

