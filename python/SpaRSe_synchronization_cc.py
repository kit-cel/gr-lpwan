#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2018 Felix Wunsch.
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

import gnuradio.blocks as blocks
import gnuradio.filter as filter
from . import fddsm_preamble_detector_cc as preamble_detector_cc
from gnuradio import gr
import numpy as np

class SpaRSe_synchronization_cc(gr.hier_block2):
    """
    Performs time and frequency synchronization for SpaRSe based on the preamble.
    """
    def __init__(self, samp_rate_hz, SF, shr, pulse, alpha=1e-3, beta=5, time_gap_chips=11, max_offset_hz=0, max_num_filters=1):
        gr.hier_block2.__init__(self,
            "SpaRSe_synchronization_cc",
            gr.io_signature(1, 1, gr.sizeof_gr_complex),  # Input signature
            gr.io_signature3(3, 3, gr.sizeof_gr_complex, gr.sizeof_gr_complex, gr.sizeof_float)) # Output signature

        filter_spacing_normalized = 0.2 * 1.0 / SF  # 0.44 corresponds to about 3 dB maximum loss due to decorrelation for conventional IEEE LECIM with BPSK
        max_offset_normalized = float(max_offset_hz) / samp_rate_hz
        num_filters = int(np.ceil(max_offset_normalized * 2 / (2 * filter_spacing_normalized)))
        if num_filters > max_num_filters:
            print("WARNING:", num_filters, "required to cover the possible frequency offset, but only", max_num_filters, "possible")
            num_filters = max_num_filters
        delta_phi = [filter_spacing_normalized * i - filter_spacing_normalized * num_filters * 1. / 2 for i in range(num_filters)]

        # Define blocks
        self.rotators = [blocks.rotator_cc(-phi) for phi in delta_phi]
        self.matched_filters = [filter.fft_filter_ccf(1, np.conj(pulse))] * num_filters
        sps = samp_rate_hz / SF
        self.preamble_detector = preamble_detector_cc(shr, sps, SF, time_gap_chips, alpha, beta, delta_phi)
        self.skiphead = blocks.skiphead(gr.sizeof_gr_complex, sps * (SF + time_gap_chips) + 4)  # the +4 is kind of "empirical" but well tested for sps=4

        # Connect blocks with preamble detector and outputs
        for i in xrange(num_filters):
            self.connect(self, self.rotators[i], self.matched_filters[i], (self.preamble_detector, i))
        self.connect(self, self.skiphead, (self.preamble_detector, num_filters))
        for i in xrange(3):
            self.connect((self.preamble_detector, i), (self, i))

    def set_alpha(self, alpha):
        self.preamble_detector.set_alpha(alpha)

    def set_beta(self, beta):
        self.preamble_detector.set_beta(beta)

