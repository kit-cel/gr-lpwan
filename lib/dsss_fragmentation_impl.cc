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
#include "dsss_fragmentation_impl.h"

#define dprintf(...) if(d_verbose_print) {printf(__VA_ARGS__);}

namespace gr {
    namespace lpwan {

        dsss_fragmentation::sptr
        dsss_fragmentation::make(int psdu_size, int frak_policy_tx, int frak_tx_timeout_ms, int frak_rx_timeout_ms,
                                 int frame_max_retry,
                                 int fics_size_tx, int device_addr_short, bool is_coordinator, int psdu_tx_dur,
                                 int dest_addr_short, bool verbose)
        {
            return gnuradio::get_initial_sptr
                    (new dsss_fragmentation_impl(psdu_size, frak_policy_tx, frak_tx_timeout_ms, frak_rx_timeout_ms,
                                                 frame_max_retry,
                                                 fics_size_tx, device_addr_short, is_coordinator, psdu_tx_dur,
                                                 dest_addr_short, verbose));
        }

        dsss_fragmentation_impl::dsss_fragmentation_impl(int psdu_size, int frak_policy_tx, int frak_tx_timeout_ms,
                                                         int frak_rx_timeout_ms,
                                                         int frame_max_retry, int fics_size_tx,
                                                         int device_addr_short, bool is_coordinator, int psdu_tx_dur,
                                                         int dest_addr_short, bool verbose)
                : gr::block("dsss_fragmentation",
                            gr::io_signature::make(0, 0, 0),
                            gr::io_signature::make(0, 0, 0)),
                  d_psdu_size(psdu_size),
                  d_frak_policy_tx(frak_policy_tx),
                  d_frame_max_retry(frame_max_retry),
                  d_fics_size_tx(fics_size_tx),
                  d_device_addr_short(device_addr_short),
                  d_pad_value(0b01010101),
                  d_frag_data_size(d_psdu_size - d_fics_size_tx - 2),
                  d_is_coordinator(is_coordinator),
                  d_dest_addr_short(dest_addr_short),
                  d_verbose_print(verbose)
        {

            if (d_frak_policy_tx != 1) {
                std::runtime_error("Only frak policy 1 is supported for now\n");
            }

            //register ports
            message_port_register_in(pmt::mp("in_mac"));
            message_port_register_in(pmt::mp("in_phy"));
            message_port_register_out(pmt::mp("out_mac"));
            message_port_register_out(pmt::mp("out_phy"));
            set_msg_handler(pmt::mp("in_mac"), boost::bind(&dsss_fragmentation_impl::msg_from_mac, this, _1));
            set_msg_handler(pmt::mp("in_phy"), boost::bind(&dsss_fragmentation_impl::msg_from_phy, this, _1));

            //calls to the frag processing
            d_period_ms = 10;

            //timeouts
            d_fscd_timeout_duration = std::chrono::duration<long, std::milli>(frak_tx_timeout_ms);
            d_frak_tx_timeout_duration = std::chrono::duration<long, std::milli>(frak_tx_timeout_ms);
            d_frak_rx_timeout_duration = std::chrono::duration<long, std::milli>(frak_rx_timeout_ms);
            d_psdu_duration = std::chrono::duration<long, std::milli>(psdu_tx_dur);

        }


        /**
         * Takes a MAC frame from MAC layer and divides it into fragments. The fragments are appended to a
         * sending queue and processed according to the frak policy.
         * @param msg PDU from MAC with data
         */
        void
        dsss_fragmentation_impl::msg_from_mac(pmt::pmt_t msg)
        {
            //dprintf("Fragmentation Sublayer: new message from mac\n");

            //check PDU
            if (!pmt::is_pair(msg)) {
                throw std::runtime_error("Fragmentation Sublayer: received non PDU input");
            }
            pmt::pmt_t meta = pmt::car(msg);
            pmt::pmt_t data = pmt::cdr(msg);

            std::vector<unsigned char> v_data;
            v_data = pmt::u8vector_elements(data);

            s_mpdu_frag_packet p;

            //generate a TID
            srand(time(NULL));
            int i = 0;
            do {
                if (i++ >= 100) {
                    dprintf("couldn't find unused tid, dropping mac frame.\n");
                    return;
                }
                p.tid = rand() % (1 << 6);
            } while (is_tid_in_use(p.tid));

            //size
            p.mpdu_size = v_data.size();
            if (p.mpdu_size >= 1024) {
                printf("Maximum data size for frag sublayer is 1023, limited by the bitsize(10) of the data size "
                               "field in FSCD packet. Dropping frame.\n");
                return;
            }

            //addresses
            address_info ai;
            ai.dest_pan_id_pres = false;
            ai.src_addr_mode = FRAME_CTRL_ADDR_SHORT;
            ai.dest_addr_mode = FRAME_CTRL_ADDR_NOT_PRESENT;
            //TODO: use addresses from mac meta field
            ai.dest_addr = d_dest_addr_short;
            p.ai = ai;

            dprintf("\n\n-------Fragmentation Sublayer debug info-------\n");
            dprintf("new fragmentation sequence\n");
            dprintf("TID: %d\n", p.tid);
            dprintf("dest addr: %ld\n", p.ai.dest_addr);
            dprintf("bytes count: %ld\n", v_data.size());
            dprintf("bytes per fragment: %d\n", d_frag_data_size);

            //pad data if necessary
            while (v_data.size() % d_frag_data_size != 0) { v_data.push_back(d_pad_value); }

            dprintf("bytes count after padding: %ld\n", v_data.size());
            dprintf("frag count: %ld\n", v_data.size() / d_frag_data_size);
            dprintf("\n-----------------------------------------------\n\n");

            int pos;
            p.nr_of_frags = 0;

            //fragment data and add headers and fics(crc)
            for (pos = 0; pos < p.mpdu_size; pos += d_frag_data_size) {
                mac_field_frag_header h;
                h.set_fragment_nr(p.nr_of_frags);
                h.set_tid(p.tid);
                std::vector<uint8_t> payload(v_data.begin() + pos, v_data.begin() + pos + d_frag_data_size);

                mac_field_fragment frag(h, payload, d_fics_size_tx, d_psdu_size);
                p.fragments.push_back(frag);
                p.nr_of_frags++;
            }

            if (p.nr_of_frags >= 64) {
                printf("Frag Sublayer: Dropping MAC frame, to big for current configuration, due to a maximum of 63 "
                               "fragments \n");
                return;
            }

            //make a fscd mac packet
            p.fscd_ie = new mac_field_ie_fscd();
            p.fscd_ie->set_sec_frag(0);
            p.fscd_ie->set_tid(p.tid);
            p.fscd_ie->set_frak_policy(d_frak_policy_tx);
            p.fscd_ie->set_fics_len(d_fics_size_tx);
            p.fscd_ie->set_psdu_size(p.mpdu_size);  //OK
            //address_info fscd_ie_ai = p.ai;
            address_info fscd_ie_ai;
            fscd_ie_ai.dest_pan_id_pres = false;
            fscd_ie_ai.src_pan_id_pres = false;
            fscd_ie_ai.src_addr_mode = FRAME_CTRL_ADDR_SHORT;
            fscd_ie_ai.dest_addr_mode = FRAME_CTRL_ADDR_SHORT;
            fscd_ie_ai.src_addr = d_device_addr_short;
            fscd_ie_ai.dest_addr = p.ai.dest_addr;
            p.fscd_ie->set_address_info(fscd_ie_ai);

            p.recv_frags = std::vector<int>(p.nr_of_frags, FRAG_STATUS_NOT_SENT);

            //append packets to the sending queue
            d_packets_tx.push_back(p);

        }

        /**
         * Takes raw data fragments from the PHY layer (which may be wrong). The fragments are CRC checked and
         * appended to a processing queue.
         * @param msg data from the PHY layer
         */
        void
        dsss_fragmentation_impl::msg_from_phy(pmt::pmt_t msg)
        {
            //frag_processing();

            //check PDU
            if (!pmt::is_pair(msg)) {
                return;
                throw std::runtime_error("Fragmentation Sublayer: received non PDU input");
            }

            pmt::pmt_t meta = pmt::car(msg);
            pmt::pmt_t data = pmt::cdr(msg);

            std::vector<unsigned char> v_data;
            v_data = pmt::u8vector_elements(data);
            if (v_data.size() != d_psdu_size) {
                throw std::runtime_error("Fragmentation Sublayer: wrong PSDU length");
            }

            mac_crc crc(d_fics_size_tx);

            //drop packet if wrong crc
            if (!crc.check_crc(v_data)) {
                //dprintf("received CORRUPT packet (CRC), dropping\n");
                return;
            }


            mac_field_frame_ctrl frame_ctrl(v_data);

            switch (frame_ctrl.get_frame_type()) {
                case FRAME_CTRL_TYPE_FRAGMENT:
                    dprintf("received FRAG or FRAK packet (CRC OK)\n");
                    recv_frag_or_frak(v_data);
                    break;
                case FRAME_CTRL_TYPE_ACK:
                    dprintf("received ACK packet (CRC OK)\n");
                    recv_ack(v_data);
                    break;
                case FRAME_CTRL_TYPE_DATA:
                    if (search_data_packet_for_fscd(v_data)) {
                        dprintf("received FSCD packet (CRC OK) \n");
                        break;
                    }
                    dprintf("Frag Sublayer: Received plain data packet (not fragmented), dropping. TODO: pass to MAC "
                                    "layer?\n");
                    break;
                default:
                    dprintf("Frag Sublayer: Received unknow packet type, dropping\n");
                    break;
            }
        }


        /**
         * Sends the next fragment for packet p
         * @param p
         */
        void
        dsss_fragmentation_impl::send_frag(s_mpdu_frag_packet *p)
        {
            mac_field_fragment *tmp = NULL;

            if (p->fscd_acknowledged == false) {
                std::runtime_error("I did something wrong.\n");
            }

            switch (d_frak_policy_tx) {
                case 0:
                    std::runtime_error("FRAK policy 0 not supported\n");
                    break;
                case 1:
                    //send last not sent or failed frag
                    for (int i = 0; i < p->fragments.size(); i++) {
                        if (p->fragments[i].get_sent_count() >= d_frame_max_retry) {
                            p->abort_me = true;
                            dprintf("Fragment Receiving: max retries for a frag, aborting transmission.\n");
                        }
                        if (p->recv_frags[i] == FRAG_STATUS_FAILED || p->recv_frags[i] == FRAG_STATUS_NOT_SENT) {
                            p->fragments[i].inc_sent_count();
                            p->recv_frags[i] = FRAG_STATUS_NO_ACK;
                            tmp = &p->fragments[i];
                            dprintf("Sending Fragment nr=%d\n", i);
                            break;
                        }
                    }

                    break;
                case 2:
                    std::runtime_error("FRAK policy 2 not supported\n");
                    break;
                default:
                    std::runtime_error("FRAK policy is unknown\n");
                    break;
            }

            //frag not set
            if (tmp == NULL) {
                return;
            }

            p->cnt_frags++;
            p->last_frag_time = std::chrono::steady_clock::now();
            pmt::pmt_t to_phy = pmt::init_u8vector(d_psdu_size, tmp->get_data());
            to_phy = pmt::cons(pmt::make_dict(), to_phy);
            message_port_pub(pmt::mp("out_phy"), to_phy);
        }


        /**
         *
         * @param f data with fragment or frak
         */
        void
        dsss_fragmentation_impl::recv_frag_or_frak(std::vector<uint8_t> &f)
        {
            mac_field_frag_header h(f);

            //received frags
            for (int i = 0; i < d_packets_rx.size(); i++) {
                s_mpdu_frag_packet *p = &d_packets_rx[i];
                if (h.get_tid() == p->tid) {
                    //save frag
                    mac_field_fragment mfrag(f, d_fics_size_tx);
                    int fnr = h.get_fragment_nr();

                    p->fragments[fnr].set_data(f, d_fics_size_tx);
                    p->recv_frags[fnr] = FRAG_STATUS_SUCCESS;
                    p->last_frag_time = std::chrono::steady_clock::now();
                    p->cnt_frags++;
                    dprintf("received frag, nr=%d\n", fnr);
                    reassembly_frags_if_complete(p);
                    return;
                }
            }

            //received fraks
            for (int i = 0; i < d_packets_tx.size(); i++) {
                s_mpdu_frag_packet *p = &d_packets_tx[i];
                if (h.get_tid() == p->tid) {
                    mac_field_frak mfrak(f, d_fics_size_tx, p->nr_of_frags);
                    p->recv_frags = mfrak.get_frag_status();
                    p->last_frak_time = std::chrono::steady_clock::now();
                    p->cnt_fraks++;

                    dprintf("Received a frak, size = %ld, frak status (2=OK, 3=MISSING):\n", p->recv_frags.size());
                    for (int j = 0; j < p->recv_frags.size(); ++j) {
                        dprintf("%d,", p->recv_frags[j]);
                    }
                    dprintf("\n");

                    return;
                }
            }

        }


        /**
         * Received a pure ack frame. Process Ack as response to fscd.
         * @param v
         */
        void
        dsss_fragmentation_impl::recv_ack(const std::vector<uint8_t> &ack)
        {

            mac_field_frame f(ack, d_fics_size_tx);
            if (!f.get_is_valid()) {
                return;
            }
            //test for valid frame header settings
            mac_field_frame_ctrl frame_ctrl = f.get_frame_ctrl();

            bool valid_settings = frame_ctrl.get_sec_enable() == 0
                                  && frame_ctrl.get_frame_pending() == 0
                                  && frame_ctrl.get_ar() == 0
                                  && frame_ctrl.get_pan_id_compr() == 1
                                  && frame_ctrl.get_sequ_nr_suppr() == 1
                                  && frame_ctrl.get_ie_pres() == 0
                                  && frame_ctrl.get_frame_vers() == FRAME_CTRL_VER_2015
                                  && frame_ctrl.get_dest_addr_mode() != FRAME_CTRL_ADDR_NOT_PRESENT
                                  && frame_ctrl.get_src_addr_mode() != FRAME_CTRL_ADDR_NOT_PRESENT
                                  && f.get_header_ies().size() == 0;

            if (!valid_settings) { return; }

            if (f.get_address_info().dest_addr == d_device_addr_short &&
                f.get_address_info().src_addr == d_packets_tx[0].ai.dest_addr) {
                dprintf("RECEIVED VALID ACK FRAME for first tx packet in queue\n");
                d_packets_tx[0].fscd_acknowledged = true;
            }


        }


        /**
         * Reassembly mpdu packet if it is complete and send it to the mac layer.
         * @param packet_nr index of packet in d_packet_rx
         */
        void
        dsss_fragmentation_impl::reassembly_frags_if_complete(s_mpdu_frag_packet *p)
        {
            //check if packet is complete
            p->finished = true;
            for (int j = 0; j < p->recv_frags.size(); ++j) {
                if (p->recv_frags[j] != FRAG_STATUS_SUCCESS) {
                    p->finished = false;
                    return;
                }
            }

            //reassemble packet
            std::vector<uint8_t> reassembled_frame;
            reassembled_frame.reserve(p->mpdu_size + 32);
            for (int j = 0; j < p->nr_of_frags; ++j) {
                std::vector<uint8_t> data = p->fragments[j].get_payload();
                reassembled_frame.insert(reassembled_frame.end(), data.begin(), data.end());
            }

            //delete padded bytes from last fragment
            while (reassembled_frame.size() > p->mpdu_size) {
                reassembled_frame.pop_back();
            }

            //Juhu
            dprintf("SUCCESS: Received a complete fragment sequence. Passing to mac layer!\n");

            //to mac layer
            pmt::pmt_t to_mac = pmt::init_u8vector(p->mpdu_size, reassembled_frame);
            to_mac = pmt::cons(pmt::make_dict(), to_mac);
            message_port_pub(pmt::mp("out_mac"), to_mac);

        }


        /**
         * Searches a received data packet for a FSCD fragment sequence initialisation and initialise
         * the new fragment transmission.
         * @param data_frame to search
         * @return true if fscd is found
         */
        bool
        dsss_fragmentation_impl::search_data_packet_for_fscd(const std::vector<uint8_t> &data_frame)
        {

            mac_field_frame f(data_frame, d_fics_size_tx);
            if (!f.get_is_valid()) {
                return false;
            }

            //test for valid frame header settings
            mac_field_frame_ctrl frame_ctrl = f.get_frame_ctrl();
            bool valid_settings = frame_ctrl.get_sec_enable() == 0
                                  && frame_ctrl.get_frame_pending() == 0
                                  && frame_ctrl.get_ar() == 1
                                  && frame_ctrl.get_pan_id_compr() == 0
                                  && frame_ctrl.get_sequ_nr_suppr() == 1
                                  && frame_ctrl.get_ie_pres() == 1
                                  && frame_ctrl.get_dest_addr_mode() == FRAME_CTRL_ADDR_NOT_PRESENT
                                  && frame_ctrl.get_src_addr_mode() == FRAME_CTRL_ADDR_NOT_PRESENT
                                  && frame_ctrl.get_frame_vers() == FRAME_CTRL_VER_2015
                                  && f.get_header_ies().size() == 1;
            if (!valid_settings) { return false; }

            mac_field_ie_fscd *fscd = dynamic_cast<mac_field_ie_fscd *>(f.get_header_ies()[0]);
            if (fscd == NULL) { return false; }

            address_info ai = fscd->get_address_info();
            if (ai.dest_pan_id_pres) {
                //ignore for now
            }
            if (ai.src_pan_id_pres) {
                //ignore for now
            }

            if (ai.dest_addr_mode == FRAME_CTRL_ADDR_SHORT) {
                //don't process fragment sequence if addresses are not matching
                //coordinator accepts all ?
                if (/*!d_is_coordinator && */ai.dest_addr != d_device_addr_short) {
                    dprintf("Frag Sublayer: Received FSCD but addresses aren't matching. Ignoring.\n");
                    return false;
                }
            }

            if (fscd->get_sec_frag()) {
                dprintf("Security not supported in FSCD, but sec_enable field is set.\n");
                return false;
            }

            if (fscd->get_fics_len() != (d_fics_size_tx == 2 ? 0 : 1)) {
                dprintf("Frag Sublayer: FSCD fics length differs from local fics length, this is not supported in this implementation.\n");
                return false;
            }

            for (int i = 0; i < d_packets_rx.size(); ++i) {
                if (d_packets_rx[i].tid == fscd->get_tid()) {
                    //TID already in use,
                    //addresses are matching (tested above), resend fscd ack
                    d_packets_rx[i].fscd_acknowledged = false;
                    dprintf("Received fscd for already known TID\n");
                    return true;
                }
            }
            if (is_tid_in_use(fscd->get_tid())) {
                //dprintf("Received fscd for already known TID\n");
                return true;
            }


            //init new fragmentation transmission rx
            s_mpdu_frag_packet new_mpdu;
            new_mpdu.tid = fscd->get_tid();
            new_mpdu.frak_policy_rx = fscd->get_frak_policy();
            new_mpdu.mpdu_size = fscd->get_psdu_size();
            int ms = new_mpdu.mpdu_size;
            int fds = d_frag_data_size;
            new_mpdu.nr_of_frags = (ms + (fds - ms % fds)) / fds;
            new_mpdu.fragments.resize(new_mpdu.nr_of_frags);
            new_mpdu.recv_frags = std::vector<int>(new_mpdu.nr_of_frags, FRAG_STATUS_NO_ACK);
            new_mpdu.fscd_ie = NULL;

            new_mpdu.ai = ai;
            new_mpdu.fscd_acknowledged = false;
            new_mpdu.last_frag_time = std::chrono::steady_clock::now();;

            d_packets_rx.push_back(new_mpdu);
            return true;
        }


        /**
         * Sends an ack frame (as response of the fscd) for the rx packet <p>.
         *
         */
        void
        dsss_fragmentation_impl::send_fscd_ack(const s_mpdu_frag_packet *p)
        {
            //construct frame control field
            mac_field_frame_ctrl f;
            f.set_ar(0);
            f.set_dest_addr_mode(p->ai.src_addr_mode);
            f.set_src_addr_mode(p->ai.dest_addr_mode);
            f.set_frame_pending(0);
            f.set_frame_type(FRAME_CTRL_TYPE_ACK);
            f.set_frame_vers(FRAME_CTRL_VER_2015);
            f.set_ie_pres(0);
            f.set_pan_id_compr(1);
            f.set_sec_enable(0);
            f.set_sequ_nr_suppr(1);

            //construct mac frame
            mac_field_frame m(d_fics_size_tx);
            m.set_padding(d_psdu_size);
            m.set_frame_ctrl(f);

            //construct address field
            address_info ai;
            ai.dest_addr = p->ai.src_addr;
            ai.dest_addr_mode = p->ai.src_addr_mode;
            ai.src_addr = p->ai.dest_addr;
            ai.src_addr_mode = p->ai.dest_addr_mode;
            ai.dest_pan_id = 0xDD;  //TODO: dummy dest pan id for now, not used but needed present for ack
            ai.dest_pan_id_pres = true;
            m.set_address_info(ai);

            //to phy
            pmt::pmt_t to_phy = pmt::init_u8vector(d_psdu_size, m.get_data());
            to_phy = pmt::cons(pmt::make_dict(), to_phy);
            message_port_pub(pmt::mp("out_phy"), to_phy);
        }


        /**
         * Sends a fragment initlisation (fscd) packet.
         * @param p
         */
        void
        dsss_fragmentation_impl::send_fscd_packet(const s_mpdu_frag_packet *p)
        {

            //construct frame control field
            mac_field_frame_ctrl f;
            f.set_ar(1);
            f.set_dest_addr_mode(FRAME_CTRL_ADDR_NOT_PRESENT);
            f.set_src_addr_mode(FRAME_CTRL_ADDR_NOT_PRESENT);
            f.set_frame_pending(0);
            f.set_frame_type(FRAME_CTRL_TYPE_DATA);
            f.set_frame_vers(FRAME_CTRL_VER_2015);
            f.set_ie_pres(1);
            f.set_pan_id_compr(0);
            f.set_sec_enable(0);
            f.set_sequ_nr_suppr(1);


            //construct frame with one fscd ie
            mac_field_frame m(p->fscd_ie->get_fics_len());
            m.add_header_ie(p->fscd_ie);
            m.set_padding(d_psdu_size);
            m.set_frame_ctrl(f);

            //send to phy layer
            pmt::pmt_t to_phy = pmt::init_u8vector(d_psdu_size, m.get_data());
            to_phy = pmt::cons(pmt::make_dict(), to_phy);
            message_port_pub(pmt::mp("out_phy"), to_phy);

        }


        /**
         * Sends a frak for the given mpdu packet p
         * @param p
         */
        void
        dsss_fragmentation_impl::send_frak(s_mpdu_frag_packet *p)
        {
            //frak forced if eg all frags are successfully transmitted
            bool send_frak_flag = p->force_one_frak;
            p->force_one_frak = false;

            std::chrono::time_point<std::chrono::steady_clock> last;
            switch (p->frak_policy_rx) {
                case 0:
                    std::runtime_error("FRAK policy 0 not supported\n");
                    break;
                case 1:
                    //last transmission time
                    last = p->fscd_time;
                    if (last < p->last_frag_time) {
                        last = p->last_frag_time;
                    }
                    if (last < p->last_frak_time) {
                        last = p->last_frak_time;
                    }

                    //frak if last frag is received the first time
                    if (p->cnt_fraks == 0 && p->recv_frags.back() == FRAG_STATUS_SUCCESS) {
                        send_frak_flag = true;
                    }
                    //limit fraks
                    if (p->cnt_fraks >= d_frame_max_retry) {
                        p->abort_me = true;
                    }
                    //frag timeout -> trigger frak
                    if (std::chrono::steady_clock::now() - last > d_frak_tx_timeout_duration) {
                        send_frak_flag = true;
                        dprintf("FRAK TIMEOUT, sending new frak\n");
                    }
                    break;
                case 2:
                    std::runtime_error("FRAK policy 2 not supported\n");
                    break;
                default:
                    std::runtime_error("FRAK policy not supported\n");
                    break;
            }

            if (!send_frak_flag) {
                return;
            }

            //generate new frak
            mac_field_frag_header h(p->tid, 0);
            mac_field_frak frak(p->recv_frags, h, d_psdu_size, d_fics_size_tx);
            p->cnt_fraks++;

            p->last_frak_time = std::chrono::steady_clock::now();

            //send to phy layer
            pmt::pmt_t to_phy = pmt::init_u8vector(d_psdu_size, frak.get_data());
            to_phy = pmt::cons(pmt::make_dict(), to_phy);
            message_port_pub(pmt::mp("out_phy"), to_phy);
        }


        /**
         * call periodically to send fracks and acks(for fscd) asynchronous to triggered message functions
         */
        void
        dsss_fragmentation_impl::frag_processing()
        {

            std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();

            //process only first tx packet, all others have to wait
            //phy (current LECIM DSSS) cannot transmit multiple packets at the same time
            //exception are fraks
            if (d_packets_tx.size()) {

                s_mpdu_frag_packet *p = &d_packets_tx.front();

                //test for successful transmission
                p->finished = true;
                for (int j = 0; j < p->recv_frags.size(); ++j) {
                    if (p->recv_frags[j] != FRAG_STATUS_SUCCESS) {
                        p->finished = false;
                        break;
                    }
                }

                //send fscd packet
                if (p->fscd_sent == false) {
                    dprintf("Sending first fscd\n");
                    send_fscd_packet(p);
                    p->fscd_sent = true;
                    p->fscd_time = now;
                    p->fscd_retry = 0;
                }
                    //retry fscd packet if necessary
                else if (p->fscd_acknowledged == false) {
                    //ack timeout, resend
                    if (now - p->fscd_time > d_fscd_timeout_duration) {
                        send_fscd_packet(p);
                        p->fscd_retry += 1;
                        p->fscd_time = now;
                    }
                    if (p->fscd_retry >= d_frame_max_retry) {
                        dprintf("FSCD packet was not acked after several retries.\n");
                        p->abort_me = true;
                    }
                } else {
                    //abort if fraks are missing for a long time
                    if (p->cnt_frags > 0 && now - p->last_frag_time > d_frak_rx_timeout_duration) {
                        dprintf("TX Timeout: Frak is missing,\n");
                        p->abort_me = true;
                    }

                    //do not flood phy
                    if (now - p->last_frag_time > d_psdu_duration) {
                        send_frag(p);
                    }
                }

                if (p->abort_me) {
                    dprintf("Tx-Packet aborted, dropping.\n");
                    p->aborted = true;
                    erase_packet(&d_packets_tx, 0);
                }else if(p->finished){
                    dprintf("Tx-Packet successful transmission.\n");
                    erase_packet(&d_packets_tx, 0);
                }
            }

            //process rx packets
            for (int k = 0; k < d_packets_rx.size(); k++) {
                s_mpdu_frag_packet *p = &d_packets_rx[k];
                //first fscd ack
                if (p->fscd_acknowledged == false) {
                    send_fscd_ack(p);
                    p->fscd_acknowledged = true;
                    p->fscd_time = now;
                    p->fscd_retry = 0;
                }
                    //no frag since last fscd ack and timeout, retry
                else if (p->cnt_frags == 0 && now - p->fscd_time > d_fscd_timeout_duration) {
                    send_fscd_ack(p);
                    p->fscd_time = now;
                    p->fscd_retry++;
                } else {
                    if(!p->finished)
                        send_frak(p);
                }

                if (p->fscd_retry >= d_frame_max_retry) {
                    p->abort_me = true;
                }

                if (p->abort_me) {
                    dprintf("RX-Packet aborted, dropping.\n");
                    p->aborted = true;
                    erase_packet(&d_packets_rx, 0);
                } else if (p->finished) {
                    //send a last frak
                    p->force_one_frak = true;
                    send_frak(p);
                    erase_packet(&d_packets_rx, 0);
                }

            }

        }


        /**
         * Checks if the requested Transaction ID (<tid>) is in use in any recognized
         * transactions.
         *
         * @param tid
         * @return true if tid is in use
         */
        bool
        dsss_fragmentation_impl::is_tid_in_use(int tid)
        {
            //check rx packets
            for (int j = 0; j < d_packets_rx.size(); ++j) {
                if (tid == d_packets_rx[j].tid) {
                    return true;
                }
            }

            //check only first tx packet, others aren't relevant yet
            if (d_packets_tx.size()!=0 && tid == d_packets_tx[0].tid) {
                return true;
            }

            //tid not in use
            return false;
        }


        /**
         * Erase packet in deque.
         * @param pl
         * @param nr
         */
        void
        dsss_fragmentation_impl::erase_packet(std::deque<s_mpdu_frag_packet> *pl, int nr)
        {
            delete (*pl).at(0).fscd_ie;
            pl->erase(pl->begin() + nr);
        }


        bool
        dsss_fragmentation_impl::start()
        {
            d_finished = false;
            d_thread = boost::shared_ptr<gr::thread::thread>
                    (new gr::thread::thread(boost::bind(&dsss_fragmentation_impl::run, this)));

            return block::start();
        }

        bool
        dsss_fragmentation_impl::stop()
        {
            // Shut down the thread
            d_finished = true;
            d_thread->interrupt();
            d_thread->join();

            return block::stop();
        }

        void dsss_fragmentation_impl::run()
        {
            while (!d_finished) {
                boost::this_thread::sleep(boost::posix_time::milliseconds(d_period_ms));
                if (d_finished) {
                    return;
                }

                frag_processing();
            }
        }


        dsss_fragmentation_impl::~dsss_fragmentation_impl()
        {
            //TODO delete all packets
        }


    } /* namespace lpwan */
} /* namespace gr */

