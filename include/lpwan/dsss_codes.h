/* -*- c++ -*- */
/* 
 * Copyright 2016 Kristian Maier <kristian.maier@gmx.de>.
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


#ifndef INCLUDED_LPWAN_DSSS_CODES_H
#define INCLUDED_LPWAN_DSSS_CODES_H

#include <lpwan/api.h>

namespace gr {
    namespace lpwan {

        /*!
         * \brief generates the codes needed in PHY
         *
         */
        class lpwan_API dsss_codes {
        public:
            static const std::vector<int> intlvr_seq256;
            static const std::vector<int> intlvr_seq384;
            static const std::vector<int> intlvr_seq512;

            static const std::vector<float> preamble16;
            static const std::vector<float> preamble32;

            static const std::vector<float> sfd16;
            static const std::vector<float> sfd32;

            static std::vector<unsigned char> generate_gold_code_uc(int seed, int length = 20000000);
            static std::vector<float> generate_gold_code_f(int seed, int length  = 20000000);

            static std::vector<unsigned char> generate_ovsf_code_uc(int code_index, int log_spread_faktor);
            static std::vector<float> generate_ovsf_code_f(int code_index, int log_spread_faktor);

            static std::vector<unsigned char> generate_combined_code_uc(int seed, int code_index, int log_spread_faktor, int length  = 20000000);
            static std::vector<float> generate_combined_code_f(int seed, int code_index, int log_spread_faktor, int length  = 20000000);

            static std::vector<unsigned char> invert_code_uc(const std::vector<unsigned char> &code);

        private:
            static std::vector<float> byte_to_float_vector(std::vector<unsigned char> bytevec);
            dsss_codes();
            ~dsss_codes();
        };

    } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_DSSS_CODES_H */

