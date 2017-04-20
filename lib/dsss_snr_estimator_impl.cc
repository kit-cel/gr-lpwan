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
#include "dsss_snr_estimator_impl.h"
#include <valarray>

namespace gr {
    namespace lpwan {

        dsss_snr_estimator::sptr
        dsss_snr_estimator::make(int psdu_size, int sf)
        {
            return gnuradio::get_initial_sptr
                    (new dsss_snr_estimator_impl(psdu_size, sf));
        }

        /*
         * The private constructor
         */
        dsss_snr_estimator_impl::dsss_snr_estimator_impl(int psdu_size, int sf)
                : gr::sync_block("dsss_snr_estimator",
                                 gr::io_signature::make(0, 0, 0),
                                 gr::io_signature::make(0, 0, 0)),
                  d_codebits_len(psdu_size*8*2),
                  d_sf(sf)
        {

            message_port_register_in(pmt::mp("in_hard_fec_enc"));
            set_msg_handler(pmt::mp("in_hard_fec_enc"),
                            boost::bind(&dsss_snr_estimator_impl::in_hard_fec_enc, this, _1));
            message_port_register_in(pmt::mp("in_hard"));
            set_msg_handler(pmt::mp("in_hard"), boost::bind(&dsss_snr_estimator_impl::in_hard, this, _1));
            message_port_register_in(pmt::mp("in_hard_fec_dec"));
            set_msg_handler(pmt::mp("in_hard_fec_dec"),
                            boost::bind(&dsss_snr_estimator_impl::in_hard_fec_dec, this, _1));
            message_port_register_in(pmt::mp("in_soft"));
            set_msg_handler(pmt::mp("in_soft"), boost::bind(&dsss_snr_estimator_impl::in_soft, this, _1));

            message_port_register_out(pmt::mp("out_snr_est"));

        }


        void
        dsss_snr_estimator_impl::in_hard_fec_enc(pmt::pmt_t msg)
        {
            pmt::pmt_t h = pmt::cdr(msg);
            d_hard_bits.push_back(pmt::u8vector_elements(h));
            calc_snr();
        }

        void
        dsss_snr_estimator_impl::in_hard(pmt::pmt_t msg)
        {
            pmt::pmt_t h = pmt::cdr(msg);
            d_hard_bits_checked.push_back(pmt::u8vector_elements(h));
            calc_snr();
        }

        void
        dsss_snr_estimator_impl::in_hard_fec_dec(pmt::pmt_t msg)
        {
            pmt::pmt_t h = pmt::cdr(msg);
            d_hard_bits_unchecked.push_back(pmt::u8vector_elements(h));
            calc_snr();
        }

        void
        dsss_snr_estimator_impl::in_soft(pmt::pmt_t msg)
        {
            pmt::pmt_t h = pmt::cdr(msg);
            d_soft_bits.push_back(pmt::f32vector_elements(h));
            calc_snr();
        }

        void
        dsss_snr_estimator_impl::calc_snr(){


            if(d_soft_bits.size()==0){
                return;
            }
            if(d_hard_bits.size()==0){
                return;
            }
            if(d_hard_bits_checked.size()==0){
                return;
            }
            if(d_hard_bits_unchecked.size()==0){
                return;
            }

            //printf("%d, %d, %d, %d, \n", d_soft_bits.size() ,d_hard_bits.size(), d_hard_bits_checked.size() ,d_hard_bits_unchecked.size() );

            //filter out packets which didn't passed the crc check
            if(d_hard_bits_checked.front() != d_hard_bits_unchecked.front()){
                d_hard_bits_unchecked.pop_front();
                d_soft_bits.pop_front();
                calc_snr();
                return;
            }
            //extract from vectors
            std::vector<float> soft = d_soft_bits.front();
            std::vector<uint8_t> hard_byte = d_hard_bits.front();
            std::vector<float> hard_float(d_codebits_len,0.0);

            //packed bytes to +-1 floats
            for(int i=0; i<d_codebits_len; i++){
                hard_float[i] = (float)(hard_byte[i])*2.0-1;
            }


            int n = d_codebits_len;

            std::valarray<float> rx(&soft[0], n);
            std::valarray<float> tx(&hard_float[0], n);

            //delete corresponing vectors
            d_hard_bits_unchecked.pop_front();
            d_hard_bits_checked.pop_front();
            d_soft_bits.pop_front();
            d_hard_bits.pop_front();

            //debug
            //printf("rx[500]: %f\noise", rx[500]);
            //printf("tx[500]: %f\noise", tx[500]);

            //calculate snr estimation
            float alpha = (tx*rx).sum() / n;
            std::valarray<float> noise = rx-alpha*tx;
            noise = noise - noise.sum()/noise.size();
            float var_n = (noise*noise).sum() / n;
            float snr = 10*log10(alpha*alpha/var_n*2/d_sf);     // * 1/2 because of differential decoding

            message_port_pub(pmt::mp("out_snr_est"), pmt::from_float(snr));
            //debug
            //printf("alpha: %f\noise", alpha);
            //printf("var_n: %f\noise", var_n);
        }

        /*
         * Our virtual destructor.
         */
        dsss_snr_estimator_impl::~dsss_snr_estimator_impl()
        {
        }

    int
    dsss_snr_estimator_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      return noutput_items;
    }

    } /* namespace lpwan */
} /* namespace gr */

