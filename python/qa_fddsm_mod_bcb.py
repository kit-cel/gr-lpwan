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


class qa_fddsm_mod_bcb (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        self.snk_ant = blocks.vector_sink_b()
        self.snk_sym = blocks.vector_sink_c()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        self.src = blocks.vector_source_b([0, 0, 1, 0, 1, 1, 0, 1], repeat=True)
        self.head = blocks.head(1, 8*2)
        self.mod = lpwan.fddsm_mod_bcb(bps=2, packet_len_bytes=1)
        self.tb.connect(self.src, self.head, self.mod)
        self.tb.connect((self.mod, 0), self.snk_ant)
        self.tb.connect((self.mod, 1), self.snk_sym)
        self.tb.run ()
        # check data
        res_ant = self.snk_ant.data()
        res_sym = self.snk_sym.data()
        #print "res_ant (L=2)", res_ant
        self.assertTrue(res_ant == (0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1))
        self.assertTrue(np.sum(np.abs(res_sym - np.array([1, 1, -1j, -1j, 1j, 1j, -1, -1, 1, 1, -1j, -1j, 1j, 1j, -1, -1]))) < 1e-5)

    # def test_002_t (self):
    #     # set up fg. NOTE: This will probably fail after the first 8 bits. Output needs to be recalculated.
    #     self.src = blocks.vector_source_b([0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0], repeat=False)
    #     self.head = blocks.head(1, 12*2)
    #     self.mod = lpwan.fddsm_mod_bcb(bps=3, packet_len_bytes=3)
    #     self.tb.connect(self.src, self.head, self.s2ts, self.mod)
    #     self.tb.connect((self.mod, 0), self.snk_ant)
    #     self.tb.connect((self.mod, 1), self.snk_sym)
    #     self.tb.run ()
    #     # check data
    #     res_ant = self.snk_ant.data()
    #     res_sym = self.snk_sym.data()
    #     print "res_ant (L=4)", res_ant
    #     self.assertTrue(res_ant == (1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0))
    #     w = np.exp(1j*np.pi/4)
    #     error = np.sum(np.abs(res_sym - np.array([w, w, w**3, w**3, w**6, w**6, w**2, w**2, w, w, w**3, w**3, w**6, w**6, w**2, w**2])))
    #     self.assertTrue(error < 1e-5)

    def test_003_t (self):
        # set up fg
        self.src = blocks.vector_source_b([0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0], repeat=False)
        self.mod = lpwan.fddsm_mod_bcb(bps=4, packet_len_bytes=2)
        self.tb.connect(self.src, self.mod)
        self.tb.connect((self.mod, 0), self.snk_ant)
        self.tb.connect((self.mod, 1), self.snk_sym)
        self.tb.run ()
        # check data
        res_ant = self.snk_ant.data()
        res_sym = self.snk_sym.data()
        #print "res_ant (L=8)", res_ant
        self.assertTrue(res_ant == (0, 1, 1, 0, 1, 0, 1, 0))
        w = np.exp(1j*np.pi/4)
        error = np.sum(np.abs(res_sym - np.array([w**2, w**6, w**4, w**2, w**2, w**4, w**2, w**4])))
        self.assertTrue(error < 1e-5)

if __name__ == '__main__':
    gr_unittest.run(qa_fddsm_mod_bcb)
