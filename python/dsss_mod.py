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
import time


class dsss_modulator(dsss_phy.physical_layer):
    def modulate_random(self, n):
        """
        modulates n frames with random data
        :param n: nr of frames
        :return: [payload chips, baseband signal, random payload bits]
        """
        np.random.seed(int(time.time()*100000)%4294967295)
        psdu_bits = np.random.randint(0, 2, size=(self.psdu_size * 8 * n,), dtype=np.uint8)
        return self.modulate(psdu_bits)

    def modulate(self, bits):
        """
        :param bits:
        :return:
        """
        if hasattr(self, 'codes_generated') == False:
            self.init()
        nframes = len(bits) / self.psdu_size / 8
        payload = bits.reshape(nframes, self.psdu_size * 8)
        baseband_out = np.array([])
        symbols_out = np.array([])
        cc_coded_out = np.array([])

        # print "encode header"
        self.shr_encoded = self.encode_header()

        for i in range(nframes):
            # print "apply convolutional code to payload"
            cc_coded = self.cc_encode(payload[i])
            cc_coded_out = np.append(cc_coded_out, cc_coded)

            # print "interleave payload"
            interleaved = self.interleave(cc_coded)

            # print "encode payload differential"
            diff_coded = self.diff_encoder(interleaved)

            # print "spread payload with goldcode"
            chips = self.spread_goldcode(diff_coded, self.psdu_spreading_factor, self.psdu_gold_code,
                                         self.psdu_goldcode_reset_per_symbol)

            # print "apply ovsf code to payload"
            chips = self.apply_ovsf(chips, self.psdu_ovsf_code)

            # print "add header to payload"
            ppdu_chips = np.append(self.shr_encoded, chips)

            if self.ppdu_modulation == 'BPSK':
                # print "modulate frame BPSK"
                symbols = self.bpsk(ppdu_chips)
                baseband = self.rc_filter(symbols)
            else:
                # print "modulate frame OQPSK"
                symbols = self.oqpsk(ppdu_chips)
                if self.oqpsk_use_raised_cosine:
                    baseband = self.rc_filter(symbols, rate=2)
                else:
                    baseband = self.rect_filter(symbols, rate=2)

            baseband_out = np.append(baseband_out, baseband)
            symbols_out = np.append(symbols_out, symbols)

        return [symbols_out, baseband_out, bits, cc_coded_out]

    def encode_header(self):
        """
        generates the encoded SHR header to be prepended to the encoded PSDU
        :param header_len:
        :return:
        """

        # choose correct header
        if self.preamble_size == 16:
            header = dsss_const.preamble16
        elif self.preamble_size == 32:
            header = dsss_const.preamble32
        else:
            return np.array([], dtype=np.uint8)

        # append sfd if present
        if self.sfd_present:
            if self.preamble_size == 16:
                header = np.append(header, dsss_const.sfd16)
            elif self.preamble_size == 32:
                header = np.append(header, dsss_const.sfd32)

        # encode header
        header = self.diff_encoder(header)
        header = self.spread_goldcode(header, self.shr_spreading_factor, self.header_gold_code,
                                      self.shr_goldcode_reset_per_symbol)
        header = self.apply_ovsf(header, self.header_ovsf_code)
        return header

    def init(self):
        lmax = (32*8*2+50)*2**self.psdu_spreading_factor
        # generate codes for header
        self.header_gold_code = self.generate_goldcode(self.shr_goldcode_seed, lmax)
        self.header_ovsf_code = self.generate_ovsf(self.psdu_ovsf_spreading_factor, self.psdu_ovsf_code_index)
        # generate codes for psdu
        self.psdu_gold_code = self.generate_goldcode(self.psdu_goldcode_seed, lmax)
        self.psdu_ovsf_code = self.generate_ovsf(self.psdu_ovsf_spreading_factor, self.psdu_ovsf_code_index)
        self.codes_generated = True
        # generate RC filter
        self.rc_filter_taps = self.generate_rc_filter()
        self.rrc_filter_taps = self.generate_rrc_filter()

    def oqpsk(self, chips):
        """
        Std. 23.2.7.2
        OQPSK modulation, upsampling with factor 2
        :param chips: expect numpy array in range [0, 1]
        :return: oqpsk symbols, as complex64
        """
        chips = chips.astype(np.float32) * 2 - 1
        even = chips[0::2]
        odd = chips[1::2]

        # upsample factor 2
        z = np.zeros([1, len(even)])
        even = np.vstack((even, z))
        even = np.reshape(even.T, [1, -1])[0, :]
        z = np.zeros([1, len(odd)])
        odd = np.vstack((odd, z))
        odd = np.reshape(odd.T, [1, -1])[0, :]

        # resample factor 2
        # even=np.repeat(even, 2)
        # odd=np.repeat(odd, 2)

        # offset
        even = np.append(even, [0])
        odd = np.append([0], odd)

        # output signal @chiprate
        oqpsk_sig = even + 1j * odd

        return oqpsk_sig.astype(np.complex64)

    def bpsk(self, chips):
        """
        Std. 23.2.7.1
        BPSK modulation
        :param chips: expect numpy array in range [0, 1]
        :return: [-1,1]
        """
        chips = chips.astype(np.float32) * 2 - 1
        return chips

    def rc_filter(self, symbols, rate=1):
        """
        apply a rc or rrc filter
        :param symbols: input symbols, may be complex
        :param rate: specify rate if input symbols are already upsampled by rate, (needed for oqpsk)
        :return: baseband
        """
        l = len(symbols)
        # upsample
        z = np.zeros([self.filter_samples_per_symbol / rate - 1, l])
        z = np.vstack((symbols, z))
        upsampled = np.reshape(z.T, [1, -1])[0, :]

        # apply tx filter
        if self.use_rrc_instead_rc:
            baseband = np.convolve(upsampled, self.rrc_filter_taps)
        else:
            baseband = np.convolve(upsampled, self.rc_filter_taps)

        # cut away filter settling up
        setup_time = self.filter_samples_per_symbol * self.filter_span / 2 - self.filter_samples_per_symbol / 2
        end_time =   self.filter_samples_per_symbol * self.filter_span / 2 + self.filter_samples_per_symbol / 2
        baseband = baseband[setup_time:-end_time]

        return baseband


    def rect_filter(self, symbols, rate=1):
        """
        apply a rectangular filter
        :param symbols: input symbols, may be complex
        :param rate: specify rate if input symbols are already upsampled by rate, (needed for oqpsk)
        :return: baseband
        """
        l = len(symbols)
        # upsample
        z = np.zeros([self.filter_samples_per_symbol / rate - 1, l])
        z = np.vstack((symbols, z))
        upsampled = np.reshape(z.T, [1, -1])[0, :]
        # apply tx filter
        baseband = np.convolve(upsampled, np.ones(self.filter_samples_per_symbol))

        # cut away filter settling up
        setup_time = 0
        end_time =   self.filter_samples_per_symbol-1
        baseband = baseband[setup_time:-end_time]

        return baseband

    def cc_encode(self, bits):
        """
        Std. 23.2.3
        :param bits: input bits of one PSDU
        :return: convolutional coded PSDU
        """
        if self.fec_tail_biting_enabled == False:
            bits = np.append(bits, np.zeros(8))

        l = len(bits)
        if l not in dsss_const.psdu_size_allowed * 8:
            raise Exception("CC-Coding: PSDU input bitlength has to be " + str(dsss_const.psdu_size_allowed * 8) + \
                            " with tail biting enabled and " + str((dsss_const.psdu_size_allowed - 1) * 8) + \
                            " with tail biting disabled")

        k = dsss_const.cc_k
        out = np.zeros(0, dtype=np.uint8)
        fsm = np.zeros(k - 1, dtype=np.uint8)

        # init fsm with tail bited bits
        if self.fec_tail_biting_enabled:
            fsm = bits[-(k - 1):]

        poly1 = np.setdiff1d(dsss_const.cc_poly1, [0]) - 1
        poly0 = np.setdiff1d(dsss_const.cc_poly0, [0]) - 1

        for i in xrange(0, l):
            ak1 = (np.sum(fsm[poly1]) + bits[i]) % 2
            ak0 = (np.sum(fsm[poly0]) + bits[i]) % 2

            out = np.append(out, [ak1, ak0])
            fsm = np.append([bits[i]], fsm[:-1])
        return out.astype(dtype=np.uint8)

    def interleave(self, bits):
        """
        Block Interleaving Std. 23.2.4
        :param bits: cc coded bits of length 256, 384 or 512
        :return: Interleaved Block
        """
        l = len(bits)
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
            output[i] = bits[intlvr_seq[i]];
        return output.astype(dtype=np.uint8)

    def diff_encoder(self, bits):
        """
        Std. 23.2.5
        :param bits:
        :return:
        """
        l = len(bits)

        bits = np.array(bits, dtype=np.uint8)
        output = np.zeros(l, dtype=np.uint8)
        output[0] = bits[0]
        for i in range(1, l):
            output[i] = (bits[i] ^ output[i - 1]) & 1
        return output.astype(dtype=np.uint8)

    def spread_goldcode(self, bits, sf_exp, goldcode, reset_per_symbol):
        """
        Std. 23.2.6, upsample and spread with goldcode,
        note: output is in range [0,1] and not [-1, 1]
        :param bits:
        :param sf_exp:
        :param goldcode:
        :return:
        """

        if (sf_exp < dsss_const.spread_min or sf_exp > dsss_const.spread_max):
            raise Exception("Goldcode-Spreading: invalid spreadfactor")
        sf = pow(2, sf_exp)
        l = len(bits)
        chips = np.zeros(l * sf, dtype=np.uint8)
        gc_i = 0  # counter var for goldcode position

        # upsample and xor with goldcode
        for i in xrange(l):
            for k in xrange(sf):
                chips[i * sf + k] = bits[i] ^ goldcode[k + gc_i * sf]
            if (not reset_per_symbol):
                gc_i = gc_i + 1;
        return chips.astype(dtype=np.uint8)

    def apply_ovsf(self, chips, ovsf_code):

        l = len(chips)
        k = 0
        out = np.zeros(l, dtype=np.uint8)

        for i in xrange(l):
            out[i] = chips[i] * ovsf_code[k]
            k = np.mod(k + 1, len(ovsf_code))
        return out
