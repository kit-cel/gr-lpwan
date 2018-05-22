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

import numpy as np
from gnuradio import gr
import pmt
import time
import collections

class error_rate_calculation(gr.sync_block):
    """
    docstring for block error_rate_calculation
    """
    def __init__(self, averaging_length = 100):
        gr.sync_block.__init__(self,
            name="error_rate_calculation",
            in_sig=None,
            out_sig=[np.float32,])
        self.ma_len = averaging_length
        self.message_port_register_in(pmt.intern("in"))
        self.set_msg_handler(pmt.intern("in"), self.input_msg)
        self.PER = 1
        self.seq_numbers = collections.deque(self.ma_len * [0], self.ma_len)
        self.ctr = 0

    def get_PER(self):
        return self.PER

    def input_msg(self, msg):
        self.ctr += 1
        pdu = pmt.cdr(msg)
        b0 = pmt.u8vector_ref(pdu, 0)
        b1 = pmt.u8vector_ref(pdu, 1)
        b2 = pmt.u8vector_ref(pdu, 2)
        b3 = pmt.u8vector_ref(pdu, 3)
        print("#", self.ctr, "possible duplicate bytes: ", b0, b1, b2, b3)
        new_seq_nr = (b0 << 0) + (b1 << 8) + (b2 << 16) + (b3 << 24)
        if new_seq_nr > self.seq_numbers[0]:
            print("\tnew seq #:", new_seq_nr)
            self.seq_numbers.appendleft(new_seq_nr)
            self.PER = 1 - self.ma_len / float(self.seq_numbers[0] - self.seq_numbers[-1] + 1)

    def work(self, input_items, output_items):
        out = output_items[0]
        out[0] = self.PER
        time.sleep(0.01)
        return 1

