#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2018 <+YOU OR YOUR COMPANY+>.
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

from gnuradio import gr, gr_unittest
from gnuradio import blocks
import lpwan_swig as lpwan
import numpy as np

class qa_partial_repeat_cc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        self.src = blocks.vector_source_c(np.arange(7))
        self.pr = lpwan.partial_repeat_cc(2, 4)
        self.snk = blocks.vector_sink_c()
        self.tb.connect(self.src, self.pr, self.snk)
        self.tb.run ()
        # check data
        data = self.snk.data()
        ref = np.array((0,1,2,3, 2,3,4,5))
        self.assertTrue(np.sum(np.abs(data - ref)**2) < 1e-5)


if __name__ == '__main__':
    gr_unittest.run(qa_partial_repeat_cc)
