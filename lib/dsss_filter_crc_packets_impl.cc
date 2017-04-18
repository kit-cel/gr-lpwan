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
#include "dsss_filter_crc_packets_impl.h"

namespace gr {
  namespace lpwan {

    dsss_filter_crc_packets::sptr
    dsss_filter_crc_packets::make()
    {
      return gnuradio::get_initial_sptr
        (new dsss_filter_crc_packets_impl());
    }

    /*
     * The private constructor
     */
    dsss_filter_crc_packets_impl::dsss_filter_crc_packets_impl()
      : gr::sync_block("dsss_filter_crc_packets",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0))
    {
        message_port_register_in(pmt::mp("in_hard_crc_ok"));
        set_msg_handler(pmt::mp("in_hard_crc_ok"),
                        boost::bind(&dsss_filter_crc_packets_impl::in_hard_crc_ok, this, _1));

        message_port_register_in(pmt::mp("in_hard"));
        set_msg_handler(pmt::mp("in_hard"), boost::bind(&dsss_filter_crc_packets_impl::in_hard, this, _1));

        message_port_register_in(pmt::mp("in_soft"));
        set_msg_handler(pmt::mp("in_soft"), boost::bind(&dsss_filter_crc_packets_impl::in_soft, this, _1));

        message_port_register_out(pmt::mp("out_soft"));
    }


      void
      dsss_filter_crc_packets_impl::in_hard_crc_ok(pmt::pmt_t msg)
      {
          pmt::pmt_t h = pmt::cdr(msg);
          d_hard_crc_ok.push_back(pmt::u8vector_elements(h));
          filter();
      }

      void
      dsss_filter_crc_packets_impl::in_hard(pmt::pmt_t msg)
      {
          pmt::pmt_t h = pmt::cdr(msg);
          d_hard_bits.push_back(pmt::u8vector_elements(h));
          filter();
      }

      void
      dsss_filter_crc_packets_impl::in_soft(pmt::pmt_t msg)
      {
          pmt::pmt_t h = pmt::cdr(msg);
          d_soft.push_back(pmt::c32vector_elements(h));
          filter();
      }

      void
      dsss_filter_crc_packets_impl::filter()
      {
          if(d_soft.size()==0){
              return;
          }
          if(d_hard_bits.size()==0){
              return;
          }
          if(d_hard_crc_ok.size()==0){
              return;
          }

          //filter out packets which didn't passed the crc check
          if(d_hard_bits.front() != d_hard_crc_ok.front()){
              d_soft.pop_front();
              d_hard_bits.pop_front();
              filter();
              return;
          }

          pmt::pmt_t vec = pmt::init_c32vector(d_soft.front().size(), d_soft.front());
          message_port_pub(pmt::mp("out_soft"), pmt::cons(pmt::make_dict(), vec));

          d_soft.pop_front();
          d_hard_bits.pop_front();
          d_hard_crc_ok.pop_front();
      }



    dsss_filter_crc_packets_impl::~dsss_filter_crc_packets_impl()
    {
    }

    int
    dsss_filter_crc_packets_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      return noutput_items;
    }

  } /* namespace lpwan */
} /* namespace gr */

