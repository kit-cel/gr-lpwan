#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2017 Kristian Maier <kristian.maier@gmx.de>.
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

class message_counter(gr.sync_block):
    """
    counts the input messages over the last <interval> s,
    set interval=0 to count all messages
    scale is multiplied to the count value
    """
    def __init__(self, interval, scale):
        gr.sync_block.__init__(self,
            name="message_counter",
            in_sig=None,
            out_sig=[np.float32])

        #self.message_port_register_out(pmt.intern("out"))
        self.message_port_register_in(pmt.intern("in"))
        self.set_msg_handler(pmt.intern("in"), self.input_msg)
        self.cnt = 0
        self.interval = interval
        self.scale = scale
        self.msges = []

    def input_msg(self, msg):
        if self.interval != 0:
            self.msges.append(time.time())
        else:
            self.msges.append(1)

    def update_count(self):
        if self.interval == 0:
            self.cnt = len(self.msges)
            return
        self.msges = filter(self.is_in_interval, self.msges)
        self.cnt = len(self.msges)

    def is_in_interval(self, timestamp):
        if timestamp+self.interval > time.time():
            return True
        return False


    def work(self, input_items, output_items):
        out = output_items[0]

        self.update_count()
        out[0] = self.cnt*self.scale
        return 1



