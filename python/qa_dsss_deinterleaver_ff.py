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

from gnuradio import gr, gr_unittest
from gnuradio import blocks
import lpwan_swig as lpwan
import pmt
import dsss_const
import numpy as np

class qa_dsss_deinterleaver_ff (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        """
        test 256 bit packet
        """
        # set up fg
        data_in = range(256)  # np.random.randint(0, 2, size=256)
        key = pmt.intern("len_tag")
        val = pmt.from_long(256)
        tag = gr.tag_utils.python_to_tag((0, key, val))
        self.src = blocks.vector_source_f(data_in, False, 1, [tag])
        self.deintlvr = lpwan.dsss_deinterleaver_ff("len_tag")
        self.snk = blocks.vector_sink_f(1)
        self.tb.connect(self.src, self.deintlvr, self.snk)
        self.tb.run()

        # check data
        self.assertFloatTuplesAlmostEqual(dsss_const.intlvr_seq_256, self.snk.data()[0:256])

    def test_002_t (self):
        """
        test 384 bit packet with interleaver
        """
        # set up fg
        data_in = np.random.randint(0, 2, size=384)
        key = pmt.intern("len_tag")
        val = pmt.from_long(384)
        tag = gr.tag_utils.python_to_tag((0, key, val))
        self.src = blocks.vector_source_b(data_in, False, 1, [tag])
        self.intlvr = lpwan.dsss_interleaver_bb("len_tag")
        self.cast = blocks.char_to_float()
        self.deintlvr = lpwan.dsss_deinterleaver_ff("len_tag")
        self.snk = blocks.vector_sink_f(1)
        self.tb.connect(self.src,self.intlvr, self.cast, self.deintlvr, self.snk)
        self.tb.run()

        # check data
        self.assertFloatTuplesAlmostEqual(data_in, self.snk.data())


if __name__ == '__main__':
    gr_unittest.run(qa_dsss_deinterleaver_ff, "qa_dsss_deinterleaver_ff.xml")
