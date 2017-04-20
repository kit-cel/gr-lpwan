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

#ifndef INCLUDED_LPWAN_DSSS_DESPREAD_CC_IMPL_H
#define INCLUDED_LPWAN_DSSS_DESPREAD_CC_IMPL_H

#include <lpwan/dsss_despread_cc.h>

#define DSSS_MODULATION_BPSK 0
#define DSSS_MODULATION_OQPSK 1

namespace gr {
    namespace lpwan {

        class dsss_despread_cc_impl : public dsss_despread_cc {
        private:
            //general
            int d_sf;
            int d_seed;
            int d_preamble_seed;
            int d_ovsf_code_index;
            int d_ovsf_log_sf;
            int d_sps;
            int d_packet_bitsize;
            int d_chiprate;
            int d_modulation;
            bool d_reset_per_symbol;
            std::vector<float> d_code_payload;
            std::vector<float> d_code_preamble;

            //buffers
            gr_complex *d_volk_buffer;
            int d_volk_buffer_length;

            //dll
            bool d_dll_active;
            int d_dll_delta=1;
            float d_dll_gain=0.05;
            float d_dll_error_reset=0.8;
            float d_dll_error_cmp=0.25;

            struct s_packet_despread {
                std::vector<gr_complex> despreaded;
                int bitpos = 0;
                int chippos = 0;
                bool oqpsk_q_pos = false;
                int carry_samples = 0;      //to save the correct sample offset over different work calls
                unsigned long sample_start_in_stream;
                bool finished = false;
                bool started = false;
                int debug_var;
                bool reset_per_symbol = false;

                gr_complex early=0, late=0;
                float dll_error = 0;
                float dll_prompt= 0;
//                std::vector<float> dll_error_debug;
//                std::vector<float> dll_prompt_debug;

                float freq;
                gr_complex phase;
                gr_complex phase_inc;

                pmt::pmt_t dict;

                std::vector<float> *code;
            };
            //for despreading packets
            std::deque<s_packet_despread> d_packets;

        public:
            dsss_despread_cc_impl(int sf, int seed, int preamble_seed, int ovsf_code_index, int ovsf_log_sf, int sps, int psdu_len,
                                  int modulation, int chiprate, bool reset_per_symbol, bool dll_active, int dll_delta,
                                  float dll_gain, float dll_error_reset, float dll_cmp);

            ~dsss_despread_cc_impl();

            // Where all the action really happens
            void forecast(int noutput_items, gr_vector_int &ninput_items_required);

            int general_work(int noutput_items,
                             gr_vector_int &ninput_items,
                             gr_vector_const_void_star &input_items,
                             gr_vector_void_star &output_items);
        };

    } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_DSSS_DESPREAD_CC_IMPL_H */

