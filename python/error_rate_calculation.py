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

class error_rate_calculation(gr.basic_block):
    """
    docstring for block error_rate_calculation
    """
    def __init__(self, averaging_length = 100, filename = ""):
        gr.basic_block.__init__(self,
            name="error_rate_calculation",
            in_sig=None,
            out_sig=None)
        self.ma_len = averaging_length
        self.message_port_register_in(pmt.intern("in"))
        self.set_msg_handler(pmt.intern("in"), self.input_msg)
        self.PER = 1
        self.seq_numbers = collections.deque(self.ma_len * [0], self.ma_len)
        self.ctr = 0
        self.unique_msg_ctr = 0
        self.textfile = None
        self.first_seqnr = None
        self.count_packets = True
        if filename is not "" and filename is not None:
            self.textfile = open(filename, "w")

    def get_PER(self):
        return self.PER

    def get_overall_PER(self):
        try:
            per = 1 - float(self.unique_msg_ctr) / (self.seq_numbers[0] - self.first_seqnr + 1)
        except TypeError:  # happens as long as no packets have been received
            per = 1.0
        #print("unique packets:", self.unique_msg_ctr, ", last nr:", self.seq_numbers[0], ", first no:", self.first_seqnr)
        return per

    def get_unique_packets(self):
        return self.unique_msg_ctr

    def stop_counting_packets(self):
        self.count_packets = False

    def start_counting_packets(self):
        self.count_packets = True

    def input_msg(self, msg):
        if self.count_packets:
            self.ctr += 1
            pdu = pmt.cdr(msg)
            b0 = pmt.u8vector_ref(pdu, 0)
            b1 = pmt.u8vector_ref(pdu, 1)
            b2 = pmt.u8vector_ref(pdu, 2)
            b3 = pmt.u8vector_ref(pdu, 3)
            if self.textfile:
                self.textfile.write("PACKET meta:" + str(pmt.car(msg)) + "\n")
                self.textfile.write("\tfirst bytes:")
                for i in range(4):
                    self.textfile.write(str(pmt.u8vector_ref(pdu, i)) + "\t")
                self.textfile.write("\n")
            new_seq_nr = (b0 << 0) + (b1 << 8) + (b2 << 16) + (b3 << 24)
            if new_seq_nr > self.seq_numbers[0]:
                if self.first_seqnr is None:
                    self.first_seqnr = new_seq_nr
                self.unique_msg_ctr += 1
                # if new_seq_nr - self.seq_numbers[0] > 1:
                #    print "\t missed", new_seq_nr - self.seq_numbers[0] - 1, "frame(s)!"
                # print "new seq #:", new_seq_nr
                self.seq_numbers.appendleft(new_seq_nr)
                self.PER = 1 - len(self.seq_numbers) / float(self.seq_numbers[0] - self.seq_numbers[-1] + 1)
