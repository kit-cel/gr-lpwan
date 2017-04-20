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


#ifndef INCLUDED_LPWAN_DSSS_DESPREAD_CC_H
#define INCLUDED_LPWAN_DSSS_DESPREAD_CC_H

#include <lpwan/api.h>
#include <gnuradio/block.h>

namespace gr {
    namespace lpwan {

        /*!
         * \brief Despreading of already timesynced LECIM DSSS packets.
         *
         * This block despreads packets marked with a stream tag in the input stream. The tag has to be on the
         * first chip of the payload, with the tag key "sop" and an estimation of the frequency offset as float
         * value. It is possible to despread overlapping packets.
         *
         * The output is a tagged stream with one despreaded payload packet per tagged block.
         *
         * \ingroup lpwan
         *
         */
        class lpwan_API dsss_despread_cc : virtual public gr::block {
        public:
            typedef boost::shared_ptr<dsss_despread_cc> sptr;

            /*!
             * \brief Return a shared_ptr to a new instance of lpwan::dsss_despread_cc.
             *
             * @param sf gold code spreading factor
             * @param seed seed of the cold gode generator for the data payload
             * @param preamble_seed seed of the cold gode generator for the preamble, assumed reset_per_symbol = true
             * @param ovsf_code_index ovsf code index [0, 2^ovsf_log_sf-1]
             * @param ovsf_log_sf ovsf spreading factor [0, 8]
             * @param sps samples per symbol, interpolation factor
             * @param psdu_len length of the psdu payload (16, 24, 32)
             * @param modulation OQPSK or BPSK
             * @param chiprate absolute chiprate, for frequency synchronisation
             * @param reset_per_symbol reset gold code after each symbol / after sf chips
             * @param dll_active true if the dll should be used
             * @param dll_delta sample delay of early late to prompt
             * @param dll_gain loop gain of dll
             * @param dll_error_reset single gain multiplier after sample error detection
             * @param dll_cmp cmp value for dll
             * @return
             */
            static sptr
            make(int sf, int seed, int preamble_seed, int ovsf_code_index, int ovsf_log_sf, int sps, int psdu_len, int modulation,
                 int chiprate, bool reset_per_symbol, bool dll_active, int dll_delta, float dll_gain,
                 float dll_error_reset, float dll_cmp);
        };

    } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_DSSS_DESPREAD_CC_H */

