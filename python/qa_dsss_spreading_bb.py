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
from dsss_mod import dsss_modulator
import numpy as np
import pmt

class qa_dsss_spreading_bb (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        """
        test one packet spreading and compare it with python implementation
        """

        sf = 2**10
        l=32*2*8
        seed = 12345
        reset_per_symbol = False

        #init data
        data_in = np.random.randint(0, 2, size=l)

        #init tag
        key = pmt.intern("len_tag")
        val = pmt.from_long(l)
        tag = gr.tag_utils.python_to_tag([0, key, val])

        # set up fg
        self.src = blocks.vector_source_b(data_in, False, 1, [tag])
        self.spread = lpwan.dsss_spreading_bb("len_tag", sf, seed, reset_per_symbol, 0, 0)
        self.snk = blocks.vector_sink_b(1)
        self.tb.connect(self.src, self.spread, self.snk)

        m = dsss_modulator()
        goldcode = m.generate_goldcode(seed, l*sf)
        out_ref = m.spread_goldcode(data_in, int(np.log2(sf)),goldcode, reset_per_symbol)

        self.tb.run ()

        # check data
        self.assertFloatTuplesAlmostEqual(out_ref, self.snk.data())


if __name__ == '__main__':
    gr_unittest.run(qa_dsss_spreading_bb, "qa_dsss_spreading_bb.xml")
