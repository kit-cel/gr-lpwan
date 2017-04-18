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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <lpwan/mac_format.h>

namespace gr {
    namespace lpwan {


        /**
         * Extract bits out of the byte vector. Useful for header decoding. Max 31 bits.
         * @param vec byte vector
         * @param bitstart start bit, eg. 0 is the msb of vec[0], 9 the 2nd bit of vec[1]
         * @param bitlen number of return bits
         * @return extracted bits
         */
        uint32_t
        mac_format::extract_bits(const uint8_t* vec, int bitstart, int bitlen){
                uint32_t out = 0;
                int bytes = (bitlen-1+bitstart)/8+1;

                for(int i=0; i<bytes; i++){
                    out <<= 8;
                    out |= vec[i];
                }

                out >>= bytes*8-bitstart-bitlen;
                out &= (1<<bitlen)-1;
                return out;
        }


        /**
         * Insert bits into byte vector <vec>. Useful for header encoding. Max 31 bits.
         * Only the specified bits are altered.
         *
         * Example:
         *      bits = 0b1100101
         *      bitstart = 5
         *      bitlen = 7
         *
         *      --> vec = 0bXXXX X110 0100 1XXX
         *      Only first 2 elements of <vec> are accessed.
         *
         * @param vec byte vector
         * @param bits number of bits
         * @param bitstart bitposition in <vec>, starts with 0
         * @param bitlen number of bits in <bits>
         */
        void
        mac_format::insert_bits(uint8_t* vec, uint32_t bits, int bitstart, int bitlen){
            int byte_start = bitstart/8;
            int byte_end = (bitstart+bitlen-1)/8;
            //int byte_cnt = byte_end-byte_start+1;

            uint32_t ones = (1<<bitlen)-1;
            bits &= ones;

            ones <<= 32-bitlen-bitstart;
            bits <<= 32-bitlen-bitstart;

            for(int i=byte_start; i<=byte_end; i++){
                vec[i] &= ~(ones >> (24-i*8)) & 0xFF;
                vec[i] |=  (bits >> (24-i*8)) & 0xFF;
            }
        }

        /**
         * Simple bitwise join of <msb> and <lsb>
         * @param msb
         * @param lsb
         * @return
         */
        uint16_t
        mac_format::bytes2word(uint8_t msb, uint8_t lsb){
            uint16_t word = msb;
            word <<= 8;
            word |= lsb;
            return word;
        }


        /**
         * Append <word> to vector <v> with msb first.
         * @param v
         * @param word
         */
        void
        mac_format::append_word(std::vector<uint8_t> &v, uint16_t word){
            v.push_back((word >> 8) & 0xFF);
            v.push_back((word >> 0) & 0xFF);
        }


        /**
         * Append the first (MSB) <num_bytes> in <bytes> to vector <v>.
         * @param v
         * @param num_bytes
         * @param bytes
         */
        void
        mac_format::append_bytes(std::vector<uint8_t> &v, int num_bytes, uint64_t bytes){
            for(int i=0; i<num_bytes; i++){
                v.push_back( (bytes>>(56-i*8)) & 0xFF );
            }
        }

        /**
         * Print max 8 bits, "0bxxxxxxxx\n"
         * @param v
         */
        void printbits(unsigned char v) {

            printf("0b");
            if(v==0){
                printf("0");
                return;
            }
            for (; v; v >>= 1) printf("%d", (v & 1));
            printf("\n");
        }

    } /* namespace lpwan */
} /* namespace gr */

