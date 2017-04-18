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

import numpy as np

# some general physical layer functions
class physical_layer():
    def __init__(self, psdu_size=32, spread=10):
        # WARNING: Implementation only works with SHR and PSDU using the same goldcode and ovsf code for now

        # preamble
        self.preamble_size = 32;  # [0, 16, 32]
        self.sfd_present = True;  # [false, true]
        self.shr_spreading_factor = spread  # [4 to 15]
        self.shr_goldcode_seed = 1234  # [0 to(2 ^ 15 - 1)]
        self.shr_goldcode_reset_per_symbol = True

        # PSDU
        self.psdu_size = psdu_size  # [16, 24, 32]
        self.psdu_spreading_factor = self.shr_spreading_factor  # [4 to 15]
        self.psdu_goldcode_seed = 12345  # [0 to(2 ^ 15 - 1)]
        self.psdu_goldcode_reset_per_symbol = False

        self.ppdu_modulation = 'OQPSK'  # ['BPSK', 'OQOSK']
        self.ppdu_modulation_rate = 100  # [100, 200, 400, 600, 800, 1000, 2000]
        self.ppdu_txat = 0  # [0 to(2 ^ 32-1)]

        # OVSF
        self.psdu_ovsf_spreading_factor = 1  # [1 to 256]
        self.psdu_ovsf_code_index = 0  # [0 to psdu_ovsf_spreading_factor]

        # FEC
        self.fec_tail_biting_enabled = True

        # Transmit Filter
        self.filter_span = 16  # truncated to span symbols, EVEN!
        self.filter_samples_per_symbol = 16  # each symbol period contains that much samples, (=upsampling faktor), EVEN!
        self.oqpsk_use_raised_cosine = True  # instead a rect filter is used
        self.use_rrc_instead_rc = True

    def generate_rc_filter(self):
        """
        Std. 13.2.5, Rolloff = 1
        :return: Filter taps
        """
        span = self.filter_span
        sps = self.filter_samples_per_symbol
        return self.rc_taps(span, sps)

    def rc_taps(self, span, sps):
        delay = span * sps / 2
        timevec = np.array(range(1, delay), dtype=np.float32) / sps;
        rc = np.ones(span * sps)
        i = 1
        for t in timevec:
            if t == 1 / 2.0:  # there is a point of discontinuity
                rc[delay + i] = np.sinc(t) * np.pi / 4
            else:
                rc[delay + i] = np.sinc(t) * np.cos(np.pi * t) / (1 - 4 * t * t)
            rc[delay - i] = rc[delay + i]
            i = i + 1
        return rc

    def generate_rrc_filter(self):
        span = self.filter_span
        sps = self.filter_samples_per_symbol
        return self.rrcosfilter(span * sps, 1, sps)

    def rrcosfilter(self, N, Ts=1, Fs=2):
        beta = 1.0
        T_delta = 1 / float(Fs)
        sample_num = np.arange(N)
        rrc = np.zeros(N, dtype=float)

        for x in sample_num:
            t = (x - N / 2) * T_delta
            if t == 0.0:
                rrc[x] = 1.0 - beta + (4 * beta / np.pi)
            elif beta != 0 and t == Ts / (4 * beta):
                rrc[x] = (beta / np.sqrt(2)) * (((1 + 2 / np.pi) * (np.sin(np.pi / (4 * beta)))) + (
                    (1 - 2 / np.pi) * (np.cos(np.pi / (4 * beta)))))
            elif beta != 0 and t == -Ts / (4 * beta):
                rrc[x] = (beta / np.sqrt(2)) * (((1 + 2 / np.pi) * (np.sin(np.pi / (4 * beta)))) + (
                    (1 - 2 / np.pi) * (np.cos(np.pi / (4 * beta)))))
            else:
                rrc[x] = (np.sin(np.pi * t * (1 - beta) / Ts) +
                            4 * beta * (t / Ts) * np.cos(np.pi * t * (1 + beta) / Ts)) / \
                           (np.pi * t * (1 - (4 * beta * t / Ts) * (4 * beta * t / Ts)) / Ts)

        return rrc

    def generate_goldcode(self, seed, length=19398656):
        """
        Std: 23.2.6.1, generates the goldcode of length 2^25-1
        the polynomials are hardcoded in src
        :param seed: initial value of second lfsr
        :return: bytearray with goldcode, first bit of goldcode in MSB of byte 0, 8th bit in LSB of byte 0, 9th MSB byte 1...
        """
        print "Generating Goldcode with seed", seed, "and length", length
        lfsr1 = 1
        lfsr2 = seed
        m = 25
        n = pow(2, 25) - 1

        seq = np.zeros(length, dtype=np.uint8)
        bitcount = 0
        bytecount = 0
        tmp = 0

        for i in xrange(length):
            # output
            seq[i] = (lfsr1 ^ lfsr2) & 1
            # poly1: 25, 3, 0
            fb1 = ((lfsr1 >> 3) ^ (lfsr1 >> 0)) & 1
            # poly2: 25, 3, 2, 1, 0
            fb2 = ((lfsr2 >> 3) ^ (lfsr2 >> 2) ^ (lfsr2 >> 1) ^ (lfsr2 >> 0)) & 1
            # feedback and shift registers
            lfsr1 = ((lfsr1 | (fb1 << m)) >> 1) & n
            lfsr2 = ((lfsr2 | (fb2 << m)) >> 1) & n
        return seq.astype(dtype=np.uint8)

    def generate_ovsf(self, ovsf_sf, code_index):
        """
        Std. 23.2.6.2
        :param ovsf_sf:
        :param code_index:
        :return:
        """

        ovsf_code = np.array([1], dtype=np.uint8)

        if (ovsf_sf == 1):
            return ovsf_code
        r_max = int(np.log2(ovsf_sf))
        code_index_binary = np.binary_repr(code_index, width=r_max)

        for r in xrange(r_max):
            if code_index_binary[r] == '0':
                ovsf_code = np.append(ovsf_code, ovsf_code)
            else:
                ovsf_code = np.append(ovsf_code, ovsf_code ^ 1)
        return 1 - ovsf_code




