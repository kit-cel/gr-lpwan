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

#ifndef INCLUDED_LPWAN_DSSS_FRAGMENTATION_IMPL_H
#define INCLUDED_LPWAN_DSSS_FRAGMENTATION_IMPL_H

#include <chrono>
#include <lpwan/dsss_fragmentation.h>
#include "lpwan/mac_field.h"
#include "lpwan/mac_field_fragment.h"
#include "lpwan/mac_field_frame_ctrl.h"
#include "lpwan/mac_field_frame.h"
#include "lpwan/mac_field_frak.h"


namespace gr {
    namespace lpwan {


        typedef struct {
            //general
            int nr_of_frags;
            int mpdu_size;
            int tid;
            int frak_policy_rx;
            address_info ai;

            //fragments data transmission
            std::deque<mac_field_fragment> fragments;
            std::vector<int> recv_frags;
            int cnt_frags = 0;
            std::chrono::time_point<std::chrono::steady_clock> last_frag_time;

            //fraks (= fragment-ack)
            std::chrono::time_point<std::chrono::steady_clock> last_frak_time;
            int cnt_fraks = 0;
            bool force_one_frak=false;

            //FSCD
            mac_field_ie_fscd *fscd_ie;
            bool fscd_acknowledged = false;
            bool fscd_sent = false;
            int fscd_retry;
            std::chrono::time_point<std::chrono::steady_clock> fscd_time;

            //flags
            bool abort_me = false;
            bool aborted = false;
            bool finished = false;
        } s_mpdu_frag_packet;


        class dsss_fragmentation_impl : public dsss_fragmentation {
        private:
            //frag settings
            int d_psdu_size;
            int d_frak_policy_tx;
            int d_fics_size_tx;
            int d_frame_max_retry;
            int d_frag_data_size;
            int d_dest_addr_short;
            uint8_t d_pad_value;

            bool d_verbose_print = false;

            //address
            //uint16_t d_pan_id;
            uint16_t d_device_addr_short;
            bool d_is_coordinator;

            //some timings
            std::chrono::duration<long, std::milli> d_fscd_timeout_duration;
            std::chrono::duration<long, std::milli> d_frak_tx_timeout_duration;
            std::chrono::duration<long, std::milli> d_frak_rx_timeout_duration;
            std::chrono::duration<long, std::milli> d_psdu_duration;

            //tx and rx packets/frags
            std::deque<s_mpdu_frag_packet> d_packets_tx;
            std::deque<s_mpdu_frag_packet> d_packets_rx;


            void msg_from_mac(pmt::pmt_t msg);

            void msg_from_phy(pmt::pmt_t msg);

            void frag_processing();

            //receiving special packets
            void recv_frag_or_frak(std::vector<uint8_t> &f);

            void recv_ack(const std::vector<uint8_t> &ack);

            void reassembly_frags_if_complete(s_mpdu_frag_packet *p);

            bool search_data_packet_for_fscd(const std::vector<uint8_t> &data_frame);

            //sending special packets
            void send_fscd_ack(const s_mpdu_frag_packet *p);

            void send_fscd_packet(const s_mpdu_frag_packet *p);

            void send_frak(s_mpdu_frag_packet *p);

            void send_frag(s_mpdu_frag_packet *p);

            //thread stuff for frag_processing()
            int d_period_ms;
            boost::shared_ptr<gr::thread::thread> d_thread;
            bool d_finished;

            bool start();

            bool stop();

            void run();

            void erase_packet(std::deque<s_mpdu_frag_packet> *pl, int nr);

            bool is_tid_in_use(int tid);

        public:
            dsss_fragmentation_impl(int psdu_size, int frak_policy_tx, int frak_tx_timeout_ms, int frak_rx_timeout_ms,
                                    int frame_max_retry, int fics_size_tx, int device_addr_short, bool is_coordinator,
                                    int psdu_tx_dur, int dest_addr_short, bool verbose);

            ~dsss_fragmentation_impl();

        };

    } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_DSSS_FRAGMENTATION_IMPL_H */

