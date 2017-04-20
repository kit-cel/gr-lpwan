#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2016 Kristian Maier <kristian.maier@gmx.de>.
# 
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

from gnuradio import gr
import numpy as np
from lpwan._lpwan_swig import dsss_codes_generate_combined_code_f
from lpwan._lpwan_swig import dsss_preamble_demod_cc_make, dsss_preamble_detector_cc_make
from lpwan._lpwan_swig import conj_multiply_delay_ccc_make
import gnuradio.filter.filter_swig as filter
import gnuradio.blocks

class dsss_preamble_search_cc(gr.hier_block2):
    """
    Generates the correlators to test the frequency hypotheses.
    """
    def __init__(self, sf, seed, ovsf_code_index, ovsf_log_sf, freqs, preamble_length, sfd_present, sps, debug_out, chiprate, mod, filt_taps):
        gr.hier_block2.__init__(self,
            "dsss_preamble_search_cc",
            gr.io_signature(1, 1, gr.sizeof_gr_complex),
            gr.io_signature(2, 2, gr.sizeof_gr_complex))



        #generate code
        code = dsss_codes_generate_combined_code_f(seed, ovsf_code_index, ovsf_log_sf, sf)

        #generate taps
        if mod == "bpsk":
            taps = np.zeros(sf*sps)
            for i in xrange(sf):
                taps[i*sps] = code[i]
        else:
            taps = np.zeros(sf*sps/2).astype(np.complex64)
            for i in xrange(sf/2):
                taps[i*sps] = code[i*2]
                taps[i*sps+sps/2] = 1j*code[i*2+1]

        ###############################################################################################################
        # WARNING: if the filterbank structure has changed, it is maybe necessary to change the delay in the data-path,
        #          because the tags has to be aligned to the preamble position
        ###############################################################################################################

        shr_len = preamble_length + (8 if sfd_present else 0)

        #delayed signal to detector
        if mod == "bpsk":
            self.rx_delay = sf*sps-3 + sf*sps
            detector = dsss_preamble_detector_cc_make(freqs, shr_len, sf, sps, chiprate, filt_taps)
            delay_sig = gnuradio.blocks.delay(gr.sizeof_gr_complex, self.rx_delay)
            print "Filterbank for BPSK"
        else:
            self.rx_delay = sf*sps/2 - 3  + sf*sps/2
            detector = dsss_preamble_detector_cc_make(freqs, shr_len, sf, sps/2, chiprate, filt_taps)
            delay_sig = gnuradio.blocks.delay(gr.sizeof_gr_complex, self.rx_delay)
            print "Filterbank for OQPSK"

        self.connect(self, delay_sig, (detector,0), (self,0))

        #signal power measurement
        c2mag = gnuradio.blocks.complex_to_mag_squared_make(1)
        sp_iir = gnuradio.filter.single_pole_iir_filter_ff_make(0.001, 1)
        avg = gnuradio.blocks.moving_average_ff(shr_len*sf, 1.0/(shr_len*sf))
        delay2 = gnuradio.blocks.delay(gr.sizeof_float, self.rx_delay)
        self.connect(self, c2mag, avg, delay2, (detector,1))

        print "Generating {0} filters for frequency offsets of {1} Hz".format(len(freqs), freqs)
        #filterbank with frequency shifts
        for i in xrange(len(freqs)):
            if mod == "bpsk":
                rot = gnuradio.blocks.rotator_cc_make(-2*np.pi*freqs[i]/chiprate/sps)
                filt = filter.fft_filter_ccf(1, np.flipud(taps).tolist())
                conj_mult = conj_multiply_delay_ccc_make(sf*sps)
                demod = dsss_preamble_demod_cc_make(sf, sps, preamble_length, sfd_present)
            else:
                rot = gnuradio.blocks.rotator_cc_make(-2*np.pi*freqs[i]/chiprate/sps*2)
                filt = filter.fft_filter_ccc(1, np.conj(np.flipud(taps).tolist()))
                conj_mult = conj_multiply_delay_ccc_make(sf*sps/2)
                demod = dsss_preamble_demod_cc_make(sf, sps/2, preamble_length, sfd_present)
            self.connect(self, rot, filt, conj_mult, demod, (detector, i+2))
            #debug output
            if debug_out == i:
                self.connect(demod,(self,1))
                #self.connect(filt,(self,1))

