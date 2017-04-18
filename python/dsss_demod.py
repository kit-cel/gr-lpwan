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
import dsss_const
import dsss_phy

from gnuradio import fec

class dsss_demodulator(dsss_phy.physical_layer):
    def demod(self, bb, nframes, goldcode=[], ovsf_code=[]):
        """

        :param bb:
        :param nframes:
        :param goldcode:
        :param ovsf_code:
        :return:
        """
        framelen_given = len(bb) / nframes
        framelen_calc = 2 ** self.psdu_spreading_factor * self.filter_samples_per_symbol * 8 * (
            self.psdu_size * 2 + self.preamble_size / 8 + (1 if self.sfd_present else 0)) -1
        if self.ppdu_modulation != 'BPSK':
            framelen_calc = framelen_calc / 2 + self.filter_samples_per_symbol / 2 - 1

        if framelen_given != framelen_calc:
            raise Exception("Input not matching to settings: Wrong framelength, got", framelen_given, "but expected",
                            framelen_calc)

        bb_frames = np.reshape(bb, (nframes, -1))
        if goldcode == [] or ovsf_code == []:
            self.init()

        softbits_out=[]

        for i in range(nframes):
            if self.ppdu_modulation == 'BPSK':
                print "Demodulate BSPK"
                chips = self.demod_bpsk(bb_frames[i])
            else:
                print "Demodulate OQPSK"
                chips = self.demod_oqpsk(bb_frames[i])
            header, payload = self.split_header_payload(chips)

            softbits = self.despread(payload, goldcode, ovsf_code, self.psdu_goldcode_reset_per_symbol,
                                     self.psdu_spreading_factor)
            softbits = self.demod_diffcoding(softbits)
            softbits = self.deinterleave(softbits)
            bits = self.viterbi(softbits)
            softbits_out = np.append(softbits_out,softbits)
        return softbits_out


    def demod_bpsk(self, bb):
        """

        :param bb: baseband signal, assume that first symbol starts at sample self.filter_samples_per_symbol/2
        :return:
        """
        sps = self.filter_samples_per_symbol
        downsample = bb.real[sps / 2::sps]
        chips = downsample
        # hard decision
        # l=len(downsample)
        # chips = np.zeros(l, dtype=np.uint8)
        # for i in range(l):
        #     if downsample[i] > 0:
        #         chips[i] = 1
        #     else:
        #         chips[i] = 0
        return chips

    def demod_oqpsk(self, bb):
        """

        :param bb: baseband signal, assume that first symbol starts at sample self.filter_samples_per_symbol/2
        :return:
        """
        sps = self.filter_samples_per_symbol
        downsample_real = bb.real[sps / 2::sps]
        downsample_imag = bb.imag[sps::sps]
        l = len(downsample_real)
        chips = np.zeros(2 * l, dtype=np.float32)
        for i in xrange(l):
            chips[2 * i] = downsample_real[i]
            chips[2 * i + 1] = downsample_imag[i]

        # hard decision
        # l = len(downsample_real)
        # chips = np.zeros(2*l, dtype=np.uint8)
        # for i in range(l):
        #     if downsample_real[i] > 0:
        #         chips[2*i] = 1
        #     else:
        #         chips[2*i] = -1
        #     if downsample_imag[i] > 0:
        #         chips[2*i+1] = 1
        #     else:
        #         chips[2*i+1] = -1
        return chips

    def despread(self, chips, goldcode, ovsf_code, reset_per_symbol, sf):
        l = len(chips)

        sf = 2 ** sf

        if np.mod(l, sf):
            raise Exception("Despreading: Wrong input size")

        bit_len = l / sf
        ovsf_code = 2 * ovsf_code.astype(np.float32) - 1
        goldcode = 2 * goldcode.astype(np.float32) - 1

        # apply ovsf code
        if len(ovsf_code) != 1:
            k = 0;
            for i in xrange(l):
                chips[i] = chips[i] * ovsf_code[k]
                k = np.mod(k + 1, len(ovsf_code))

        gc_i = 0
        # apply goldcode
        for i in xrange(bit_len):
            for k in xrange(sf):
                chips[i * sf + k] = chips[i * sf + k] * goldcode[k + gc_i * sf]
            if not reset_per_symbol:
                gc_i = gc_i + 1;

        # despread
        softbits = np.zeros(bit_len)
        for i in xrange(bit_len):
            softbits[i] = np.sum(chips[i * sf:(i + 1) * sf]) / sf

        return softbits

    def split_header_payload(self, phyframe_chips):
        header_end = (self.preamble_size + (8 if self.sfd_present else 0)) * 2 ** self.shr_spreading_factor
        header = phyframe_chips[0:header_end]
        payload = phyframe_chips[header_end:]
        return header, payload

    def deinterleave(self, softbits):
        l = len(softbits)
        if l not in dsss_const.psdu_size_allowed * 16:
            raise Exception("Interleaver: wrong input bit length")

        # chose the right sequence
        if (l == 256):
            intlvr_seq = dsss_const.intlvr_seq_256
        elif (l == 384):
            intlvr_seq = dsss_const.intlvr_seq_384
        elif (l == 512):
            intlvr_seq = dsss_const.intlvr_seq_512

        output = np.zeros(l)
        for i in range(l):
            output[i] = softbits[intlvr_seq[i]];
        return output

    def demod_diffcoding(self, bits):

        #assume perfekt channel = no change in phase
        l=len(bits)
        out=np.zeros(l)
        out[0]=bits[0]
        for i in xrange(1,l):
            out[i] = bits[i] * np.conjugate(bits[i-1])
        return out

    def viterbi(self, bits):
        if self.fec_tail_biting_enabled:
            decoder = fec.cc_decoder_make(self.psdu_size*8, 7, 2, [171, 133], 0, -1, fec.CC_TAILBITING)
        else:
            decoder = fec.cc_decoder_make(self.psdu_size*8, 7, 2, [171, 133], 0, -1, fec.CC_TERMINATED)
        out = np.zeros(self.psdu_size)
        #decoder.generic_work(2,2)
        return out


    def init(self):
        # generate codes for header
        self.header_gold_code = self.generate_goldcode(self.shr_goldcode_seed)
        self.header_ovsf_code = self.generate_ovsf(self.psdu_ovsf_spreading_factor, self.psdu_ovsf_code_index)
        # generate codes for psdu
        self.psdu_gold_code = self.generate_goldcode(self.psdu_goldcode_seed)
        self.psdu_ovsf_code = self.generate_ovsf(self.psdu_ovsf_spreading_factor, self.psdu_ovsf_code_index)
        self.codes_generated = True
        # generate RC filter
        self.rc_filter_taps = self.generate_rc_filter()
