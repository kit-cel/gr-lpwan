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

//#define VERBOSE_OUTPUT

#include <gnuradio/io_signature.h>
#include "dsss_despread_cc_impl.h"
#include "lpwan/dsss_codes.h"

#include <volk/volk.h>

namespace gr {
    namespace lpwan {

        dsss_despread_cc::sptr
        dsss_despread_cc::make(int sf, int seed, int preamble_seed, int ovsf_code_index, int ovsf_log_sf, int sps,
                               int psdu_len, int modulation, int chiprate, bool reset_per_symbol,
                               bool dll_active, int dll_delta, float dll_gain, float dll_error_reset,
                               float dll_cmp)
        {
            return gnuradio::get_initial_sptr
                    (new dsss_despread_cc_impl(sf, seed, preamble_seed, ovsf_code_index, ovsf_log_sf, sps, psdu_len,
                                               modulation, chiprate, reset_per_symbol, dll_active,
                                               dll_delta, dll_gain, dll_error_reset, dll_cmp));
        }

        dsss_despread_cc_impl::dsss_despread_cc_impl(int sf, int seed, int preamble_seed, int ovsf_code_index,
                                                     int ovsf_log_sf, int sps, int psdu_len,
                                                     int modulation, int chiprate, bool reset_per_symbol,
                                                     bool dll_active, int dll_delta, float dll_gain,
                                                     float dll_error_reset, float dll_cmp)
                : gr::block("dsss_despread_cc",
                            gr::io_signature::make(1, 1, sizeof(gr_complex)),
                            gr::io_signature::make2(1, 3, sizeof(gr_complex), sizeof(float))),
                  d_sf(sf),
                  d_seed(seed),
                  d_preamble_seed(preamble_seed),
                  d_ovsf_code_index(ovsf_code_index),
                  d_ovsf_log_sf(ovsf_log_sf),
                  d_packet_bitsize(psdu_len * 8 * 2 + 1),//+1 because of phase reference for diff-demod
                  d_chiprate(chiprate),
                  d_modulation(modulation),
                  d_reset_per_symbol(reset_per_symbol),
                  d_dll_active(dll_active),
                  d_dll_delta(dll_delta),
                  d_dll_gain(dll_gain),
                  d_dll_error_reset(dll_error_reset),
                  d_dll_error_cmp(dll_cmp)
        {
            //generate spreading codes
            d_code_payload = dsss_codes::generate_combined_code_f(seed, d_ovsf_code_index, d_ovsf_log_sf, d_sf * 512);
            //assumed preamble is configured with reset_per_symbol = true
            d_code_preamble = dsss_codes::generate_combined_code_f(d_preamble_seed, d_ovsf_code_index, d_ovsf_log_sf, d_sf);

            //volk buffer
            d_volk_buffer_length = d_packet_bitsize * sps * d_sf * 2;
            d_volk_buffer = (gr_complex *) volk_malloc(d_volk_buffer_length * sizeof(gr_complex), volk_get_alignment());

            set_min_output_buffer(d_packet_bitsize);

            //internally d_sps is samples per chip
            if (d_modulation == DSSS_MODULATION_BPSK) {
                d_sps = sps;
            } else if (d_modulation == DSSS_MODULATION_OQPSK) {
                d_sps = sps / 2;
            }

            set_tag_propagation_policy(TPP_DONT);
        }

        dsss_despread_cc_impl::~dsss_despread_cc_impl()
        {
            volk_free(d_volk_buffer);
        }

        void
        dsss_despread_cc_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
        {
            ninput_items_required[0] = noutput_items * d_sf * d_sps;
        }

        int
        dsss_despread_cc_impl::general_work(int noutput_items,
                                            gr_vector_int &ninput_items,
                                            gr_vector_const_void_star &input_items,
                                            gr_vector_void_star &output_items)
        {
            const gr_complex *in = (const gr_complex *) input_items[0];
            gr_complex *out = (gr_complex *) output_items[0];
//            float *out_dll_error = (float *) output_items[1];
//            float *out_dll_prompt = (float *) output_items[2];

            const int nin = ninput_items[0];
            uint64_t nin_read = nitems_read(0);
            gr_complex minus_one_j = gr_complex(0, -1);

#ifdef VERBOSE_OUTPUT
            static int debug_sum_incomming_packets=0;
            static int debug_sum_despreaded_packets=0;
            printf("#######NEW WORK CALL########\n");
            printf("Packets in Queue: %ld, sum_incomming: %d, sum_despreaded: %d\n", d_packets.size(), debug_sum_incomming_packets, debug_sum_despreaded_packets);
            printf("nin: %ld, nout: %ld, nin_read: %ld, nwritten: %ld\n", nin, noutput_items, nin_read, nitems_written(0));
#endif

            //output of finished and despreaded packets
            if (d_packets.size() && d_packets[0].finished) {
#ifdef VERBOSE_OUTPUT
                printf("\nFINISHED DESPREADING A PACKET\n\n");
                debug_sum_despreaded_packets++;
                printf("Debug var:%d\n", d_packets[0].debug_var);
#endif
                //FIXME:flip first sign in despreaded bits depending on preamble bit
                //d_packets[0].despreaded[0] *=-1;
                memcpy(out, &(d_packets[0].despreaded[0]), d_packet_bitsize * sizeof(gr_complex));
//                memcpy(out_dll_error, &(d_packets[0].dll_error_debug[0]), d_packet_bitsize * sizeof(float));
//                memcpy(out_dll_prompt, &(d_packets[0].dll_prompt_debug[0]), d_packet_bitsize * sizeof(float));

                add_item_tag(0, nitems_written(0), pmt::mp("packet_len"), pmt::from_long(d_packet_bitsize));
                add_item_tag(0, nitems_written(0), pmt::mp("est"), d_packets[0].dict);
                d_packets.pop_front();

                consume_each(0);
                return d_packet_bitsize;
            }


            std::vector<tag_t> v;
            get_tags_in_window(v, 0, 0, nin, pmt::intern("sop"));

            //new packets detected, append them to despreading queue
            for (int i = 0; i < v.size(); i++) {
                s_packet_despread s;

                s.despreaded.resize(d_packet_bitsize);
                s.sample_start_in_stream = v[i].offset;
                s.code = &d_code_preamble;
                s.reset_per_symbol = d_reset_per_symbol;

                if (pmt::is_dict(v[i].value)) {
                    s.dict = v[i].value;
                } else {
                    s.dict = pmt::make_dict();
                }

//                s.dll_error_debug.resize(d_packet_bitsize);
//                s.dll_prompt_debug.resize(d_packet_bitsize);
                s.early = 0;
                s.late = 0;

                //frequency correction
                s.freq = pmt::to_float(pmt::dict_ref(s.dict, pmt::mp("fo_est"), pmt::from_float(0)));
                s.phase = gr_complex(1, 0);
                s.phase_inc = gr_complex((float) std::cos(2 * M_PI * s.freq / d_chiprate / d_sps),
                                         (float) -std::sin(2 * M_PI * s.freq / d_chiprate / d_sps));

                d_packets.push_back(s);
#ifdef VERBOSE_OUTPUT
                printf("DETECTED NEW PACKET, absolute pos: %ld, relpos in buffer: %d\n", v[i].offset, v[i].offset-nitems_read(0));
                debug_sum_incomming_packets++;
#endif
            }

            //iterate through despreading queue
            for (int i = 0; i < d_packets.size(); ++i) {
                s_packet_despread &s = d_packets[i];

                //samples of packet available in input buffer
                int samples_in_inbuffer;
                int pos_in_inbuffer;

                if (s.started == false) {
                    //in first decoding work call
                    samples_in_inbuffer = nin + nin_read - s.sample_start_in_stream;
                    pos_in_inbuffer = s.sample_start_in_stream - nin_read;
                    s.started = true;
                } else {
                    samples_in_inbuffer = nin;
                    pos_in_inbuffer = (2 * d_sps - s.carry_samples) % d_sps;
                }

                //correct frequency offset
                volk_32fc_s32fc_x2_rotator_32fc(d_volk_buffer, in, s.phase_inc, &s.phase, nin);

#ifdef VERBOSE_OUTPUT
                printf("Packet # in Queue: %d, ssis: %ld, siib: %ld, posinbuf: %ld, chippos: %ld, carry_samples: %ld\n", i,
                       s.sample_start_in_stream, samples_in_inbuffer, pos_in_inbuffer, s.chippos, s.carry_samples);
                printf("freq correction: freq: %f, phase: %f+j%f, phase_inc: %f+j%f\n", s.freq,
                       s.phase.real(), s.phase.imag(), s.phase_inc.real(), s.phase_inc.imag());
#endif

                //skip samples in the next work call
                s.carry_samples = (samples_in_inbuffer + s.carry_samples) % d_sps;

                //actual despreading here
                while (pos_in_inbuffer < nin) {
                    if (d_modulation == DSSS_MODULATION_BPSK) {
                        //DLL, small error at work buffer boundary is negligible
                        if (d_dll_active && (pos_in_inbuffer > 0) && (pos_in_inbuffer < nin - d_dll_delta)) {
                            s.early += (*s.code)[s.chippos] * d_volk_buffer[pos_in_inbuffer - d_dll_delta];
                            s.late += (*s.code)[s.chippos] * d_volk_buffer[pos_in_inbuffer + d_dll_delta];
                        }
                        //despreading at prompt position
                        s.despreaded[s.bitpos] += (*s.code)[s.chippos++] * d_volk_buffer[pos_in_inbuffer];
                    } else if (d_modulation == DSSS_MODULATION_OQPSK) {
                        if (!s.oqpsk_q_pos) {
                            //DLL, small error at work buffer boundary is negligible
                            if (d_dll_active && (pos_in_inbuffer > 0) && (pos_in_inbuffer < nin - d_dll_delta)) {
                                s.early += (*s.code)[s.chippos] * d_volk_buffer[pos_in_inbuffer - d_dll_delta];
                                s.late += (*s.code)[s.chippos] * d_volk_buffer[pos_in_inbuffer + d_dll_delta];
                            }
                            //despreading at prompt position
                            s.despreaded[s.bitpos] += (*s.code)[s.chippos++] * d_volk_buffer[pos_in_inbuffer];
                        } else {
                            //DLL, small error at work buffer boundary is negligible
                            if (d_dll_active && (pos_in_inbuffer > 0) && (pos_in_inbuffer < nin - d_dll_delta)) {
                                s.early += (*s.code)[s.chippos] * d_volk_buffer[pos_in_inbuffer - d_dll_delta] *
                                           minus_one_j;
                                s.late += (*s.code)[s.chippos] * d_volk_buffer[pos_in_inbuffer + d_dll_delta] *
                                          minus_one_j;

                            }
                            //despreading at prompt position
                            s.despreaded[s.bitpos] +=
                                    (*s.code)[s.chippos++] * d_volk_buffer[pos_in_inbuffer] * minus_one_j;
                        }
                        s.oqpsk_q_pos = !s.oqpsk_q_pos;
                    }
                    pos_in_inbuffer += d_sps;


                    //despreaded one symbol?
                    if (s.chippos % d_sf == 0) {

                        //Delay Locked Loop eval
                        //not for every chip, bit should be enough
                        if (d_dll_active) {
                            float early_val = std::abs(s.early * s.early);
                            float late_val = std::abs(s.late * s.late);
                            float prompt_val = std::abs(s.despreaded[s.bitpos]);

                            if (s.bitpos == 0) {
                                s.dll_prompt = prompt_val;
                                s.dll_error = (early_val - late_val) / (prompt_val * prompt_val) * d_dll_error_reset;
                            } else {
                                s.dll_prompt = (1 - d_dll_gain) * s.dll_prompt + d_dll_gain * prompt_val;
                                s.dll_error = (1 - d_dll_gain) * s.dll_error +
                                              d_dll_gain * (early_val - late_val) / (prompt_val * prompt_val);
                            }

                            //debug output
//                            s.dll_prompt_debug[s.bitpos] = s.dll_prompt;
//                            s.dll_error_debug[s.bitpos] = s.dll_error;

                            //tracking late
                            if (s.dll_error < -d_dll_error_cmp) {
                                pos_in_inbuffer += d_dll_delta;
                                s.carry_samples -= d_dll_delta;
                                s.dll_error *= d_dll_error_reset;
                            }

                            //tracking early
                            if (s.dll_error > d_dll_error_cmp) {
                                pos_in_inbuffer -= d_dll_delta;
                                s.carry_samples += d_dll_delta;
                                s.dll_error *= d_dll_error_reset;
                            }

                            //reset energy
                            s.early = 0;
                            s.late = 0;

                            //hacky :P
                            if (s.dll_error > d_dll_error_cmp * 2) {
                                s.dll_error = d_dll_error_cmp * 2;
                            } else if (s.dll_error < -d_dll_error_cmp * 2) {
                                s.dll_error = -d_dll_error_cmp * 2;
                            }
                        }

                        s.bitpos++;

                        //despreaded last preamble symbol?
                        if (s.bitpos == 1) {
                            s.code = &d_code_payload;
                            s.chippos = 0;
                        }

                        //reset gold code every bit
                        if (s.reset_per_symbol) {
                            s.chippos = 0;
                        }

                        //packet fully despreaded?
                        if (s.bitpos == d_packet_bitsize) {
                            s.finished = true;
                            break;
                        }
                    }
                }


            }
            consume_each(nin);
            return 0;
        }

    } /* namespace lpwan */
} /* namespace gr */

