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
from dsss_preamble_search_cc import dsss_preamble_search_cc
from dsss_mod import dsss_modulator
import numpy as np
import pmt

class qa_dsss_preamble_search_cc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        #BPSK, no frequency offset

        mod = dsss_modulator()
        mod.shr_goldcode_reset_per_symbol = True
        mod.shr_goldcode_seed = np.random.randint(500, 500000)
        mod.shr_spreading_factor = 8
        mod.sfd_present = False
        mod.preamble_size = 32
        mod.init()
        sps = 1
        sf = 2**mod.shr_spreading_factor

        #init preamble block
        self.preamble_search = dsss_preamble_search_cc(sf, mod.shr_goldcode_seed, 0,0,[0], mod.preamble_size, False, sps, 0, 1000000, 'bpsk', [0])

        #generate data with preamble only
        preamble = mod.bpsk(mod.encode_header())
        n = 20
        data_in = []
        positions = []
        for i in range(n):
            data_in = np.append(data_in, np.zeros(np.random.randint(10000,20000)))
            data_in = np.append(data_in, preamble)
            positions = np.append(positions, len(data_in)+self.preamble_search.rx_delay) #delay in data signal in filterbank
        data_in = np.append(data_in, np.zeros(np.random.randint(10000,20000)))
        #add some noise
        data_in = data_in + np.random.randn(len(data_in)) * 5

        # set up fg
        self.src = blocks.vector_source_c_make(data_in, False, 1, [])
        self.snk_corr = blocks.vector_sink_c_make(1)
        self.snk_data = blocks.vector_sink_c_make(1)
        self.tb.connect(self.src, self.preamble_search)
        self.tb.connect((self.preamble_search,0), self.snk_data)
        self.tb.connect((self.preamble_search,1), self.snk_corr)
        self.tb.run()

        # check data
        tags = self.snk_data.tags()
        searched_positions = np.array([])
        for i in range(len(tags)):
            searched_positions = np.append(searched_positions,tags[i].offset + sf)

        found = np.in1d(positions, searched_positions.astype(int))
        self.assertTrue(np.all(found))


    def test_002_t (self):
        #OQPSK, no frequency offset

        mod = dsss_modulator()
        mod.shr_goldcode_reset_per_symbol = True
        mod.shr_goldcode_seed = np.random.randint(500, 500000)
        mod.shr_spreading_factor = 8
        mod.sfd_present = False
        mod.preamble_size = 32
        mod.init()
        sps = 2
        sf = 2**mod.shr_spreading_factor

        #init preamble block
        self.preamble_search = dsss_preamble_search_cc(sf, mod.shr_goldcode_seed, 0,0,[0], mod.preamble_size, False, sps, 0, 1000000, 'oqpsk', [0])

        #generate data with preamble only
        preamble = mod.oqpsk(mod.encode_header())
        n = 6
        data_in = []
        positions = []
        for i in range(n):
            data_in = np.append(data_in, np.zeros(np.random.randint(10000,20000)))
            data_in = np.append(data_in, preamble)
            positions = np.append(positions, len(data_in)+self.preamble_search.rx_delay-1) #delay in data signal in filterbank
        data_in = np.append(data_in, np.zeros(np.random.randint(10000,20000)))
        data_in = data_in + np.random.randn(len(data_in)) * 5

        # set up fg
        self.src = blocks.vector_source_c_make(data_in, False, 1, [])
        self.snk_corr = blocks.vector_sink_c_make(1)
        self.snk_data = blocks.vector_sink_c_make(1)
        self.tb.connect(self.src, self.preamble_search)
        self.tb.connect((self.preamble_search,0), self.snk_data)
        self.tb.connect((self.preamble_search,1), self.snk_corr)
        self.tb.run()

        # check data
        tags = self.snk_data.tags()
        searched_positions = np.array([])
        for i in range(len(tags)):
            searched_positions = np.append(searched_positions,tags[i].offset + sf)

        found = np.in1d(positions, searched_positions.astype(int))
        self.assertTrue(np.all(found))

    def test_003_t (self):
        #BPSK, frequency offset

        mod = dsss_modulator()
        mod.shr_goldcode_reset_per_symbol = True
        mod.shr_goldcode_seed = np.random.randint(500, 500000)
        mod.shr_spreading_factor = 8
        mod.sfd_present = False
        mod.preamble_size = 32
        mod.init()
        sps = 1
        sf = 2**mod.shr_spreading_factor
        chiprate = 1000000
        fo = 1000 # Hz

        #init preamble block
        self.preamble_search = dsss_preamble_search_cc(sf, mod.shr_goldcode_seed, 0,0,[0], mod.preamble_size, False, sps, 0, chiprate, 'bpsk', [0])

        #generate data with preamble only
        preamble = mod.bpsk(mod.encode_header())
        n = 20
        data_in = []
        positions = []
        for i in range(n):
            data_in = np.append(data_in, np.zeros(np.random.randint(10000,20000)))
            data_in = np.append(data_in, preamble)
            positions = np.append(positions, len(data_in)+self.preamble_search.rx_delay) #delay in data signal in filterbank
        data_in = np.append(data_in, np.zeros(np.random.randint(10000,20000)))
        #frequency offset
        data_in = data_in * np.exp(1j*2.0*np.pi*fo/chiprate*np.arange(0,len(data_in)))
        #add some noise
        data_in = data_in + np.random.randn(len(data_in)) * 4

        # set up fg
        self.src = blocks.vector_source_c_make(data_in, False, 1, [])
        self.snk_corr = blocks.vector_sink_c_make(1)
        self.snk_data = blocks.vector_sink_c_make(1)
        self.tb.connect(self.src, self.preamble_search)
        self.tb.connect((self.preamble_search,0), self.snk_data)
        self.tb.connect((self.preamble_search,1), self.snk_corr)
        self.tb.run()

        # check data
        #time offset estimation
        tags = self.snk_data.tags()
        searched_positions = np.array([])
        for i in range(len(tags)):
            searched_positions = np.append(searched_positions,tags[i].offset + sf)
        found = np.in1d(positions, searched_positions.astype(int))
        self.assertTrue(np.all(found))

        #freq offset estimation
        mask = np.in1d(searched_positions.astype(int),positions)
        v = np.asarray(tags)[mask]

        for t in v:
            self.assertTrue(pmt.to_python(t.value)['fo_est'] < fo*1.1)
            self.assertTrue(pmt.to_python(t.value)['fo_est'] > fo*0.9)


    def test_004_t (self):
        #OQPSK, frequency offset

        mod = dsss_modulator()
        mod.shr_goldcode_reset_per_symbol = True
        mod.shr_goldcode_seed = np.random.randint(500, 500000)
        mod.shr_spreading_factor = 8
        mod.sfd_present = False
        mod.preamble_size = 32
        mod.init()
        sps = 2
        sf = 2**mod.shr_spreading_factor
        chiprate = 2000000
        fo = 1234 # Hz

        #init preamble block
        self.preamble_search = dsss_preamble_search_cc(sf, mod.shr_goldcode_seed, 0,0,[0], mod.preamble_size, False, sps, 0, chiprate, 'oqpsk',[0])

        #generate data with preamble only
        preamble = mod.oqpsk(mod.encode_header())
        n = 20
        data_in = []
        positions = []
        for i in range(n):
            data_in = np.append(data_in, np.zeros(np.random.randint(10000,20000)))
            data_in = np.append(data_in, preamble)
            positions = np.append(positions, len(data_in)+self.preamble_search.rx_delay-1) #delay in data signal in filterbank
        data_in = np.append(data_in, np.zeros(np.random.randint(10000,20000)))
        #frequency offset
        data_in = data_in * np.exp(1j*2.0*np.pi*fo/chiprate*np.arange(0,len(data_in)))
        #add some noise
        data_in = data_in + np.random.randn(len(data_in)) * 4

        # set up fg
        self.src = blocks.vector_source_c_make(data_in, False, 1, [])
        self.snk_corr = blocks.vector_sink_c_make(1)
        self.snk_data = blocks.vector_sink_c_make(1)
        self.tb.connect(self.src, self.preamble_search)
        self.tb.connect((self.preamble_search,0), self.snk_data)
        self.tb.connect((self.preamble_search,1), self.snk_corr)
        self.tb.run()

        # check data
        #time offset estimation
        tags = self.snk_data.tags()
        searched_positions = np.array([])
        for i in range(len(tags)):
            searched_positions = np.append(searched_positions,tags[i].offset + sf)
        found = np.in1d(positions, searched_positions.astype(int))
        self.assertTrue(np.all(found))

        #freq offset estimation
        mask = np.in1d(searched_positions.astype(int),positions)
        v = np.asarray(tags)[mask]

        for t in v:
            self.assertTrue(pmt.to_python(t.value)['fo_est'] < fo*1.1)
            self.assertTrue(pmt.to_python(t.value)['fo_est'] > fo*0.9)

    def test_005_t (self):
        #OQPSK, frequency offset

        mod = dsss_modulator()
        mod.shr_goldcode_reset_per_symbol = True
        mod.shr_goldcode_seed = np.random.randint(500, 500000)
        mod.shr_spreading_factor = 4
        mod.sfd_present = False
        mod.preamble_size = 32
        mod.init()
        sps = 2
        sf = 2**mod.shr_spreading_factor
        chiprate = 2000000
        fo = 1000 # Hz

        #init preamble block
        self.preamble_search = dsss_preamble_search_cc(sf, mod.shr_goldcode_seed, 0,0,[0], mod.preamble_size, False, sps, 0, chiprate, 'oqpsk',[0])

        #generate data with preamble only
        preamble = mod.oqpsk(mod.encode_header())
        n = 20
        data_in = []
        positions = []
        for i in range(n):
            data_in = np.append(data_in, np.zeros(np.random.randint(10000,20000)))
            data_in = np.append(data_in, preamble)
            positions = np.append(positions, len(data_in)+self.preamble_search.rx_delay-1) #delay in data signal in filterbank
        data_in = np.append(data_in, np.zeros(np.random.randint(10000,20000)))
        #frequency offset
        data_in = data_in * np.exp(1j*2.0*np.pi*fo/chiprate*np.arange(0,len(data_in)))
        #add some noise
        data_in = data_in + np.random.randn(len(data_in)) * 0.2

        # set up fg
        self.src = blocks.vector_source_c_make(data_in, False, 1, [])
        self.snk_corr = blocks.vector_sink_c_make(1)
        self.snk_data = blocks.vector_sink_c_make(1)
        self.tb.connect(self.src, self.preamble_search)
        self.tb.connect((self.preamble_search,0), self.snk_data)
        self.tb.connect((self.preamble_search,1), self.snk_corr)
        self.tb.run()

        # check data
        #time offset estimation
        tags = self.snk_data.tags()
        searched_positions = np.array([])
        for i in range(len(tags)):
            searched_positions = np.append(searched_positions,tags[i].offset + sf)
        found = np.in1d(positions, searched_positions.astype(int))
        self.assertTrue(np.all(found))

        #freq offset estimation
        mask = np.in1d(searched_positions.astype(int),positions)
        v = np.asarray(tags)[mask]

        for t in v:
            self.assertTrue(pmt.to_python(t.value)['fo_est'] < fo*1.2)
            self.assertTrue(pmt.to_python(t.value)['fo_est'] > fo*0.8)

    def test_006_t (self):
        #OQPSK, frequency offset, multiple frequency paths

        mod = dsss_modulator()
        mod.shr_goldcode_reset_per_symbol = True
        mod.shr_goldcode_seed = np.random.randint(500, 500000)
        mod.shr_spreading_factor = 8
        mod.sfd_present = False
        mod.preamble_size = 32
        mod.init()
        sps = 2
        sf = 2**mod.shr_spreading_factor
        chiprate = 2000000
        fo = 8000 # Hz

        #init preamble block
        self.preamble_search = dsss_preamble_search_cc(sf, mod.shr_goldcode_seed, 0,0,[0, 7900, -7900], mod.preamble_size,False, sps, 0, chiprate, 'oqpsk',[0])

        #generate data with preamble only
        preamble = mod.oqpsk(mod.encode_header())
        n = 20
        data_in = []
        positions = []
        for i in range(n):
            data_in = np.append(data_in, np.zeros(np.random.randint(10000,20000)))
            data_in = np.append(data_in, preamble)
            positions = np.append(positions, len(data_in)+self.preamble_search.rx_delay-1) #delay in data signal in filterbank
        data_in = np.append(data_in, np.zeros(np.random.randint(10000,20000)))
        #frequency offset
        data_in = data_in * np.exp(1j*2.0*np.pi*fo/chiprate*np.arange(0,len(data_in)))
        #add some noise
        data_in = data_in + np.random.randn(len(data_in)) * 1

        # set up fg
        self.src = blocks.vector_source_c_make(data_in, False, 1, [])
        self.snk_corr = blocks.vector_sink_c_make(1)
        self.snk_data = blocks.vector_sink_c_make(1)
        self.tb.connect(self.src, self.preamble_search)
        self.tb.connect((self.preamble_search,0), self.snk_data)
        self.tb.connect((self.preamble_search,1), self.snk_corr)
        self.tb.run()

        # check data
        #time offset estimation
        tags = self.snk_data.tags()
        searched_positions = np.array([])
        for i in range(len(tags)):
            searched_positions = np.append(searched_positions,tags[i].offset + sf)
        found = np.in1d(positions, searched_positions.astype(int))
        self.assertTrue(np.all(found))

        #freq offset estimation
        mask = np.in1d(searched_positions.astype(int),positions)
        v = np.asarray(tags)[mask]

        for t in v:
            self.assertTrue(pmt.to_python(t.value)['fo_est'] < fo*1.2)
            self.assertTrue(pmt.to_python(t.value)['fo_est'] > fo*0.8)


if __name__ == '__main__':
    gr_unittest.run(qa_dsss_preamble_search_cc, "qa_dsss_preamble_search_cc.xml")
