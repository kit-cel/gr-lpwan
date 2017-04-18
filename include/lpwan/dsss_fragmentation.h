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


#ifndef INCLUDED_LPWAN_DSSS_FRAGMENTATION_H
#define INCLUDED_LPWAN_DSSS_FRAGMENTATION_H

#include <lpwan/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
    namespace lpwan {

        /*!
         * \brief Fragmentation Layer Std. 23.3
         *
         * Takes a MAC frame and divides them into smaller fragments, which are
         * tranmitted and acked as a seperate unit.
         *
         * For now the destination address is fixed and not extracted from the
         * MAC packet. This could be relevant in a testbed with more than two
         * participants, where the coordinator wants to send to different
         * transceivers. As a workaround it should be possible to use multiple
         * fragmentation blocks.
         *
         * \ingroup lpwan
         *
         */
        class lpwan_API dsss_fragmentation : virtual public gr::block {
        public:
            typedef boost::shared_ptr<dsss_fragmentation> sptr;

            /*!
             * \brief Return a shared_ptr to a new instance of lpwan::dsss_fragmentation.
             * @param psdu_size in bytes
             * @param frak_policy_tx only policy 1 supported so far
             * @param frak_tx_timeout_ms (frak policy 1) timeout for sending a frak and retransmission of fscd packet and fscd-ack
             * @param frak_rx_timeout_ms for receiving a frak, tx-abortion after timeout
             * @param frame_max_retry is the maximum number of retries of the same fragment/fraks until abortion
             * @param fics_size_tx fics = crc length, 2 or 4 bytes
             * @param device_addr_short address of this device, only short address (16bit) supported
             * @param is_coordinator true if pan coordinator, (FOR NOW NO EFFEKT )
             * @param psdu_tx_dur duration of one psdu on air, so fragments don't flood phy queue
             * @param dest_addr_short address of the destination (coordinator)
             * @param verbose print some (debug) info
             * @return
             */
            static sptr
            make(int psdu_size, int frak_policy_tx, int frak_tx_timeout_ms, int frak_rx_timeout_ms, int frame_max_retry, int fics_size_tx,
                 int device_addr_short, bool is_coordinator, int psdu_tx_dur, int dest_addr_short, bool verbose);
        };

    } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_DSSS_FRAGMENTATION_H */

