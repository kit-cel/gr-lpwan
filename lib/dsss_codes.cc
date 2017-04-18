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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <lpwan/dsss_codes.h>
#include <stdexcept>
#include <cmath>

namespace gr {
    namespace lpwan {


        /**
         * Generates the goldcode sequence. See 23.2.6.1
         *
         * @param seed      The goldcode seed is the initilization vector of the second LFSR.
         * @param length    The length to be saved, defaults to 20000000.
         * @return          The goldcode, saved as unpacked bytes (one bit per byte).
         */
        std::vector<unsigned char>
        dsss_codes::generate_gold_code_uc(int seed, int length)
        {

            unsigned int lfsr1 = 1;     //each bit represents the status of one one-bit-register
            unsigned int lfsr2 = seed;
            const int m = 25;
            const int n = (1 << 25) - 1;
            std::vector<unsigned char> gold_code;

            if (length <= 0 || length > n) {
                throw std::out_of_range("Requested goldcode length (" + std::to_string(length) + ") is out of range");
            }
            if (seed < 0 || seed > n) {
                throw std::out_of_range("Requested goldcode seed (" + std::to_string(length) + ") is out of range");
            }

            gold_code.resize(length);

            unsigned char fb1, fb2; //feedback

            for (int i = 0; i < length; i++) {
                gold_code[i] = ((lfsr1 & 1) ^ (lfsr2 & 1));
                fb1 = ((lfsr1 >> 3) ^ (lfsr1 >> 0)) & 1;
                fb2 = ((lfsr2 >> 3) ^ (lfsr2 >> 2) ^ (lfsr2 >> 1) ^ (lfsr2 >> 0)) & 1;
                lfsr1 = ((lfsr1 | (fb1 << m)) >> 1) & n;
                lfsr2 = ((lfsr2 | (fb2 << m)) >> 1) & n;
            }

            return gold_code;
        }

        /**
         * Generates the goldcode sequence as float vector [1,-1]
         * @param seed
         * @param length
         * @return
         */
        std::vector<float>
        dsss_codes::generate_gold_code_f(int seed, int length)
        {
            std::vector<unsigned char> uchar_gc;
            uchar_gc = generate_gold_code_uc(seed, length);
            return byte_to_float_vector(uchar_gc);
        }


        /**
         * Generates the OVSF sequence. See 23.2.6.2
         * @param code_index
         * @param log_spread_factor
         * @return
         */
        std::vector<unsigned char>
        dsss_codes::generate_ovsf_code_uc(int code_index, int log_spread_factor)
        {
            std::vector<unsigned char> ovsf_code;

            if (log_spread_factor < 0 || log_spread_factor > 8) {
                throw std::out_of_range("OVSF-Spreading factor is out of range");
            }

            int spread_factor = 1 << log_spread_factor;

            if (code_index >= spread_factor || code_index < 0) {
                throw std::out_of_range("OVSF-Codeindex is out of range");
            }

            //no spreading
            if (spread_factor == 1) {
                ovsf_code.resize(1);
                ovsf_code[0] = 1;
                return ovsf_code;
            }

            int r_max = log_spread_factor;
            ovsf_code.resize(spread_factor);
            ovsf_code[0] = 1;
            unsigned int ci = code_index;

            for (int i = 0; i < r_max; i++) {
                bool path_dir = ci & (1 << (r_max - i - 1));
                for (int k = 0; k < (1 << i); k++) {
                    if (path_dir) {
                        ovsf_code[(1 << i) + k] = ovsf_code[k] ^ 1;
                    } else {
                        ovsf_code[(1 << i) + k] = ovsf_code[k];
                    }
                }
            }

            return ovsf_code;
        }

        /**
         * Generates the OVSF sequence as float vector [-1,1]
         * @param code_index
         * @param log_spread_factor
         * @return
         */
        std::vector<float>
        dsss_codes::generate_ovsf_code_f(int code_index, int log_spread_factor)
        {
            std::vector<unsigned char> uchar_ovsf;
            uchar_ovsf = generate_ovsf_code_uc(code_index, log_spread_factor);
            return byte_to_float_vector(uchar_ovsf);
        }


        /***
         * Cast byte vector of unpacked bytes [0,1] to floats [-1,1]
         * @param bytevec
         * @return
         */
        std::vector<float>
        dsss_codes::byte_to_float_vector(std::vector<unsigned char> bytevec)
        {
            int size = bytevec.size();
            std::vector<float> outvec;
            outvec.resize(size);
            for (int i = 0; i < size; i++) {
                outvec[i] = ((float) bytevec[i]) * 2 - 1;
            }
            return outvec;

        }


        /**
         * Generates a combined (XOR) goldcode and ovsfcode.
         * @param seed
         * @param code_index
         * @param log_spread_faktor
         * @param length
         * @return
         */
        std::vector<unsigned char>
        dsss_codes::generate_combined_code_uc(int seed, int code_index, int log_spread_faktor, int length)
        {
            std::vector<unsigned char> ovsf;
            std::vector<unsigned char> gold;
            std::vector<unsigned char> outvec;
            outvec.resize(length);

            gold = generate_gold_code_uc(seed, length);
            ovsf = generate_ovsf_code_uc(code_index, log_spread_faktor);

            //merge codes and upsample
            int ovsf_size = ovsf.size();
            for (int i = 0; i < length; i++) {
                outvec[i] = (gold[i] ^ (1 - ovsf[i % ovsf_size])) & 1;
            }
            return outvec;
        }

        /**
         * Inverts the code (unpacked bytes), 0->1, 1->0,
         * @param code
         * @return
         */
        std::vector<unsigned char>
        dsss_codes::invert_code_uc(const std::vector<unsigned char> &code){

            std::vector<unsigned char> out(code.size(), 0);
            for(int i = 0; i<code.size(); ++i){
                out[i] = (~code[i]) & 1;
            }
            return out;
        }

        /**
         * Generates a combined (XOR) goldcode and ovsfcode as float vector [-1,1]
         * @param seed
         * @param code_index
         * @param log_spread_faktor
         * @param length
         * @return
         */
        std::vector<float>
        dsss_codes::generate_combined_code_f(int seed, int code_index, int log_spread_faktor, int length)
        {
            std::vector<unsigned char> outvec;
            outvec = generate_combined_code_uc(seed, code_index, log_spread_faktor, length);
            return byte_to_float_vector(outvec);
        }


        const std::vector<float> dsss_codes::preamble16 = {-1, -1, 1, 1, 1, 1, 1, 1, -1, 1, -1, 1, 1, -1, -1, 1};
        const std::vector<float> dsss_codes::preamble32 = {-1, -1, -1, -1, 1, 1, 1, 1, 1, 1, -1, 1, 1, -1, 1, 1, -1, 1,
                                                           1, -1, -1, 1, 1, 1, -1, -1, 1, -1, 1, -1, 1, -1};

        const std::vector<float> dsss_codes::sfd16 = {-1, -1, 1, 1, 1, -1, -1, -1};
        const std::vector<float> dsss_codes::sfd32 = {1, -1, -1, -1, -1, 1, -1, -1};

        const std::vector<int> dsss_codes::intlvr_seq256 = {0, 128, 64, 192, 32, 160, 96, 224, 16, 144, 80, 208, 48,
                                                            176, 112, 240, 8, 136, 72, 200, 40, 168, 104, 232, 24,
                                                            152, 88, 216, 56, 184, 120, 248, 4, 132, 68, 196, 36, 164,
                                                            100, 228, 20, 148, 84, 212, 52, 180, 116, 244, 12, 140,
                                                            76, 204, 44, 172, 108, 236, 28, 156, 92, 220, 60, 188, 124,
                                                            252, 2, 130, 66, 194, 34, 162, 98, 226, 18, 146, 82,
                                                            210, 50, 178, 114, 242, 10, 138, 74, 202, 42, 170, 106, 234,
                                                            26, 154, 90, 218, 58, 186, 122, 250, 6, 134, 70, 198,
                                                            38, 166, 102, 230, 22, 150, 86, 214, 54, 182, 118, 246, 14,
                                                            142, 78, 206, 46, 174, 110, 238, 30, 158, 94, 222, 62,
                                                            190, 126, 254, 1, 129, 65, 193, 33, 161, 97, 225, 17, 145,
                                                            81, 209, 49, 177, 113, 241, 9, 137, 73, 201, 41, 169,
                                                            105, 233, 25, 153, 89, 217, 57, 185, 121, 249, 5, 133, 69,
                                                            197, 37, 165, 101, 229, 21, 149, 85, 213, 53, 181, 117,
                                                            245, 13, 141, 77, 205, 45, 173, 109, 237, 29, 157, 93, 221,
                                                            61, 189, 125, 253, 3, 131, 67, 195, 35, 163, 99, 227,
                                                            19, 147, 83, 211, 51, 179, 115, 243, 11, 139, 75, 203, 43,
                                                            171, 107, 235, 27, 155, 91, 219, 59, 187, 123, 251, 7,
                                                            135, 71, 199, 39, 167, 103, 231, 23, 151, 87, 215, 55, 183,
                                                            119, 247, 15, 143, 79, 207, 47, 175, 111, 239, 31, 159,
                                                            95, 223, 63, 191, 127, 255};

        const std::vector<int> dsss_codes::intlvr_seq384 = {0, 256, 128, 64, 320, 192, 32, 288, 160, 96, 352, 224, 16,
                                                            272, 144, 80, 336, 208, 48, 304, 176, 112, 368, 240, 8,
                                                            264, 136, 72, 328, 200, 40, 296, 168, 104, 360, 232, 24,
                                                            280, 152, 88, 344, 216, 56, 312, 184, 120, 376, 248, 4,
                                                            260, 132, 68, 324, 196, 36, 292, 164, 100, 356, 228, 20,
                                                            276, 148, 84, 340, 212, 52, 308, 180, 116, 372, 244, 12,
                                                            268, 140, 76, 332, 204, 44, 300, 172, 108, 364, 236, 28,
                                                            284, 156, 92, 348, 220, 60, 316, 188, 124, 380, 252, 2,
                                                            258, 130, 66, 322, 194, 34, 290, 162, 98, 354, 226, 18, 274,
                                                            146, 82, 338, 210, 50, 306, 178, 114, 370, 242, 10,
                                                            266, 138, 74, 330, 202, 42, 298, 170, 106, 362, 234, 26,
                                                            282, 154, 90, 346, 218, 58, 314, 186, 122, 378, 250, 6,
                                                            262, 134, 70, 326, 198, 38, 294, 166, 102, 358, 230, 22,
                                                            278, 150, 86, 342, 214, 54, 310, 182, 118, 374, 246, 14,
                                                            270, 142, 78, 334, 206, 46, 302, 174, 110, 366, 238, 30,
                                                            286, 158, 94, 350, 222, 62, 318, 190, 126, 382, 254, 1,
                                                            257, 129, 65, 321, 193, 33, 289, 161, 97, 353, 225, 17, 273,
                                                            145, 81, 337, 209, 49, 305, 177, 113, 369, 241, 9,
                                                            265, 137, 73, 329, 201, 41, 297, 169, 105, 361, 233, 25,
                                                            281, 153, 89, 345, 217, 57, 313, 185, 121, 377, 249, 5,
                                                            261, 133, 69, 325, 197, 37, 293, 165, 101, 357, 229, 21,
                                                            277, 149, 85, 341, 213, 53, 309, 181, 117, 373, 245, 13,
                                                            269, 141, 77, 333, 205, 45, 301, 173, 109, 365, 237, 29,
                                                            285, 157, 93, 349, 221, 61, 317, 189, 125, 381, 253, 3,
                                                            259, 131, 67, 323, 195, 35, 291, 163, 99, 355, 227, 19, 275,
                                                            147, 83, 339, 211, 51, 307, 179, 115, 371, 243, 11,
                                                            267, 139, 75, 331, 203, 43, 299, 171, 107, 363, 235, 27,
                                                            283, 155, 91, 347, 219, 59, 315, 187, 123, 379, 251, 7,
                                                            263, 135, 71, 327, 199, 39, 295, 167, 103, 359, 231, 23,
                                                            279, 151, 87, 343, 215, 55, 311, 183, 119, 375, 247, 15,
                                                            271, 143, 79, 335, 207, 47, 303, 175, 111, 367, 239, 31,
                                                            287, 159, 95, 351, 223, 63, 319, 191, 127, 383, 255};

        const std::vector<int> dsss_codes::intlvr_seq512 = {0, 256, 128, 384, 64, 320, 192, 448, 32, 288, 160, 416, 96,
                                                            352, 224, 480, 16, 272, 144, 400, 80, 336, 208, 464,
                                                            48, 304, 176, 432, 112, 368, 240, 496, 8, 264, 136, 392, 72,
                                                            328, 200, 456, 40, 296, 168, 424, 104, 360, 232, 488,
                                                            24, 280, 152, 408, 88, 344, 216, 472, 56, 312, 184, 440,
                                                            120, 376, 248, 504, 4, 260, 132, 388, 68, 324, 196, 452,
                                                            36, 292, 164, 420, 100, 356, 228, 484, 20, 276, 148, 404,
                                                            84, 340, 212, 468, 52, 308, 180, 436, 116, 372, 244, 500,
                                                            12, 268, 140, 396, 76, 332, 204, 460, 44, 300, 172, 428,
                                                            108, 364, 236, 492, 28, 284, 156, 412, 92, 348, 220, 476,
                                                            60, 316, 188, 444, 124, 380, 252, 508, 2, 258, 130, 386, 66,
                                                            322, 194, 450, 34, 290, 162, 418, 98, 354, 226, 482,
                                                            18, 274, 146, 402, 82, 338, 210, 466, 50, 306, 178, 434,
                                                            114, 370, 242, 498, 10, 266, 138, 394, 74, 330, 202, 458,
                                                            42, 298, 170, 426, 106, 362, 234, 490, 26, 282, 154, 410,
                                                            90, 346, 218, 474, 58, 314, 186, 442, 122, 378, 250, 506,
                                                            6, 262, 134, 390, 70, 326, 198, 454, 38, 294, 166, 422, 102,
                                                            358, 230, 486, 22, 278, 150, 406, 86, 342, 214, 470,
                                                            54, 310, 182, 438, 118, 374, 246, 502, 14, 270, 142, 398,
                                                            78, 334, 206, 462, 46, 302, 174, 430, 110, 366, 238, 494,
                                                            30, 286, 158, 414, 94, 350, 222, 478, 62, 318, 190, 446,
                                                            126, 382, 254, 510, 1, 257, 129, 385, 65, 321, 193, 449,
                                                            33, 289, 161, 417, 97, 353, 225, 481, 17, 273, 145, 401, 81,
                                                            337, 209, 465, 49, 305, 177, 433, 113, 369, 241, 497,
                                                            9, 265, 137, 393, 73, 329, 201, 457, 41, 297, 169, 425, 105,
                                                            361, 233, 489, 25, 281, 153, 409, 89, 345, 217, 473,
                                                            57, 313, 185, 441, 121, 377, 249, 505, 5, 261, 133, 389, 69,
                                                            325, 197, 453, 37, 293, 165, 421, 101, 357, 229, 485,
                                                            21, 277, 149, 405, 85, 341, 213, 469, 53, 309, 181, 437,
                                                            117, 373, 245, 501, 13, 269, 141, 397, 77, 333, 205, 461,
                                                            45, 301, 173, 429, 109, 365, 237, 493, 29, 285, 157, 413,
                                                            93, 349, 221, 477, 61, 317, 189, 445, 125, 381, 253, 509,
                                                            3, 259, 131, 387, 67, 323, 195, 451, 35, 291, 163, 419, 99,
                                                            355, 227, 483, 19, 275, 147, 403, 83, 339, 211, 467,
                                                            51, 307, 179, 435, 115, 371, 243, 499, 11, 267, 139, 395,
                                                            75, 331, 203, 459, 43, 299, 171, 427, 107, 363, 235, 491,
                                                            27, 283, 155, 411, 91, 347, 219, 475, 59, 315, 187, 443,
                                                            123, 379, 251, 507, 7, 263, 135, 391, 71, 327, 199, 455,
                                                            39, 295, 167, 423, 103, 359, 231, 487, 23, 279, 151, 407,
                                                            87, 343, 215, 471, 55, 311, 183, 439, 119, 375, 247, 503,
                                                            15, 271, 143, 399, 79, 335, 207, 463, 47, 303, 175, 431,
                                                            111, 367, 239, 495, 31, 287, 159, 415, 95, 351, 223, 479,
                                                            63, 319, 191, 447, 127, 383, 255, 511};

    } /* namespace lpwan */
} /* namespace gr */

