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
#include "dsss_preamble_detector_cc_impl.h"
#include <volk/volk.h>
#include <gnuradio/filter/firdes.h>

namespace gr {
    namespace lpwan {

        dsss_preamble_detector_cc::sptr
        dsss_preamble_detector_cc::make(std::vector<float> freqs, int shr_len, int sf, int sps, int chiprate, std::vector<float> filter_taps)
        {
            return gnuradio::get_initial_sptr
                    (new dsss_preamble_detector_cc_impl(freqs, shr_len, sf, sps, chiprate, filter_taps));
        }

        /*
         * The private constructor
         */
        dsss_preamble_detector_cc_impl::dsss_preamble_detector_cc_impl(std::vector<float> freqs, int shr_len,
                                                                       int sf, int sps, int chiprate, std::vector<float> filter_taps)
                : gr::sync_block("dsss_preamble_detector_cc",
                                 gr::io_signature::make3(freqs.size() + 2, freqs.size() + 2, sizeof(gr_complex),
                                                         sizeof(float), sizeof(gr_complex)),
                                 gr::io_signature::make(1, 1, sizeof(gr_complex))),
                  d_freqs(freqs),
                  d_shr_len(shr_len),
                  d_sf(sf),
                  d_sps(sps),
                  d_samples_per_peak_slot(10 * 256 * sps * 2),
                  d_chiprate(chiprate),
                  d_filter_taps(filter_taps)
        {
            d_volk_buffer = (float *) volk_malloc(d_samples_per_peak_slot * sizeof(float), volk_get_alignment());
            set_output_multiple(d_samples_per_peak_slot);

            //calculate filter params for snr estimator
            float sum;
            int len=d_filter_taps.size();
            volk_32f_accumulator_s32f(&sum, &d_filter_taps[0], len);
            volk_32f_s32f_normalize(&d_filter_taps[0], sum, len);
            volk_32f_x2_dot_prod_32f(&d_h_sum_2, &d_filter_taps[0], &d_filter_taps[0], len);
            d_h2_sum_2 = d_h_sum_2*d_h_sum_2;
            for(int i=1; i<len; i++){
                float tmp;
                volk_32f_x2_dot_prod_32f(&tmp, &d_filter_taps[i], &d_filter_taps[0], len-i);
                d_h2_sum_2 += 2*tmp*tmp;
            }
            d_threshold_factor=1;

        }

        /*
         * Our virtual destructor.
         */
        dsss_preamble_detector_cc_impl::~dsss_preamble_detector_cc_impl()
        {
            volk_free(d_volk_buffer);
        }

        int
        dsss_preamble_detector_cc_impl::work(int noutput_items,
                                             gr_vector_const_void_star &input_items,
                                             gr_vector_void_star &output_items)
        {
            const gr_complex *in_rx = (const gr_complex *) input_items[0];
            const float *in_power = (const float *) input_items[1];
            gr_complex *out = (gr_complex *) output_items[0];

            memcpy(out, in_rx, noutput_items * sizeof(gr_complex));

            //calculate threshold from sigma factor, rayleigh distributed
            float sigma = 4;
            //squared because volk mag square is faster
            float threshold = pow(sigma * 2 * in_power[0] * d_sf * std::sqrt(1.f * d_shr_len), 2) * d_threshold_factor;

            /*
             * Algo: Divide correlation output into slots. In every slot only one peak above the threshold is detected for
             * all frequency hyptothesis. This is due to too many detected packets in high SNR environment.
             */
            int slots = noutput_items / d_samples_per_peak_slot;
            int flen = d_freqs.size();


            int max_pos = 0;
            float max_abs;
            gr_complex max;
            int max_freq_index;

            //iterate over slots
            for (int n = 0; n < slots; n++) {
                max = -1;
                max_abs = -1;

                //iterate over freqs
                for (int f = 0; f < flen; f++) {
                    uint16_t pos;
                    gr_complex *in = (gr_complex *) input_items[2 + f];
                    volk_32fc_magnitude_squared_32f_u(d_volk_buffer, in + n * d_samples_per_peak_slot,
                                                      d_samples_per_peak_slot);
                    volk_32f_index_max_16u(&pos, d_volk_buffer, d_samples_per_peak_slot);
                    if (d_volk_buffer[pos] > max_abs) {
                        max_abs = d_volk_buffer[pos];
                        max_pos = pos + n * d_samples_per_peak_slot;
                        max_freq_index = f;
                        max = in[max_pos];
                    }

                }

                //set tag on detected position and calculate estimation for snr and fo
                if (max_abs > threshold) {
                    pmt::pmt_t d = pmt::make_dict();

                    //relative frequency offset
                    float df = static_cast<float>(std::arg(max) / 2.0 / M_PI / d_sf);
                    //absolute frequency offset in Hz
                    float freq_offset = d_freqs[max_freq_index] - df * d_chiprate;
                    d = pmt::dict_add(d, pmt::intern("fo_est"), pmt::from_float(freq_offset));

                    //Peak amplitude
                    float s = static_cast<float>(sqrt(max_abs) / d_sf / d_sf / (d_shr_len - 1))/d_h_sum_2/d_h_sum_2;
                    //Frequency offset correction factor
                    s = s * std::pow(static_cast<float>(sin(M_PI * df) / sin(M_PI * df * d_sf) * d_sf), 2);
                    float r = (in_power[0]-s*d_h2_sum_2/2)/d_h_sum_2*2;

                    float snr_est = 10 * std::log10(s/ r);
                    d = pmt::dict_add(d, pmt::intern("snr_est"), pmt::from_float(snr_est));

                    //d = pmt::dict_add(d, pmt::intern("debug_s"), pmt::from_float(s));
                    //d = pmt::dict_add(d, pmt::intern("debug_in_power"), pmt::from_float(in_power[0]));
                    //d = pmt::dict_add(d, pmt::intern("debug_peak"), pmt::from_float(sqrt(max_abs)));

                    //printf("\na: %f\n", a);
                    //printf("in_power[0]: %f\n", in_power[0]);
                    //printf("snr_est: %f\n", snr_est);

                    add_item_tag(0, nitems_written(0) + max_pos, pmt::intern("sop"), d);
                }

            }

            return noutput_items;
        }

    } /* namespace lpwan */
} /* namespace gr */

