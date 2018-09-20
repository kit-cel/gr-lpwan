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

class qa_sliding_dotprod_32f_x2_32f (gr_unittest.TestCase):

    def setUp (self):
        pass

    def tearDown (self):
        pass

    def test_single_dotprod_001_t (self):
        # set up kernel
        taps = np.array([1, 2, 3])
        dotprod = lpwan.sliding_dotprod_32f_x2_32f(taps)
        data_in = np.array(range(1, 100))

        # check data
        data_out = np.zeros(data_in.size + taps.size - 1)

        ref_out = np.correlate(data_in, taps, 'full')

        for i in range(data_in.size):
            data_out[i] = dotprod.single_dotprod(data_in[i])
            #print(i, data_out[i], ref_out[i])
        self.assertFloatTuplesAlmostEqual(data_out[:-taps.size+1], ref_out[:-taps.size+1], places=4)

    def test_single_dotprod_002_t (self):
        # set up kernel
        taps = np.random.randn(100)
        dotprod = lpwan.sliding_dotprod_32f_x2_32f(taps)
        data_in = np.random.randn(100000)

        # check data
        data_out = np.zeros(data_in.size + taps.size - 1)

        ref_out = np.correlate(data_in, taps, 'full')

        for i in range(data_in.size):
            data_out[i] = dotprod.single_dotprod(data_in[i])
        self.assertFloatTuplesAlmostEqual(data_out[:-taps.size+1], ref_out[:-taps.size+1], places=4)

if __name__ == '__main__':
    #import time
    #time.sleep(10)
    gr_unittest.run(qa_sliding_dotprod_32f_x2_32f)
