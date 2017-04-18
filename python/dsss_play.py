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
import dsss_mod
import dsss_demod
import matplotlib.pyplot as plt
import scipy.signal as sig
import numpy.random as rnd


if __name__ == "__main__":

    nframes = 2
    m = dsss_mod.dsss_modulator()
    d = dsss_demod.dsss_demodulator()
    mod_data = m.modulate_random(nframes)

    # add some noise
    bb=mod_data[1]
    l=len(bb)
    print "Sigpower BB:", sum(abs(mod_data[0]**2))/len(mod_data[0])
    noise_power = 1
    bb = bb + rnd.normal(size=l, scale=np.sqrt(noise_power)) + 1j * rnd.normal(size=l, scale=np.sqrt(noise_power))

    demod_data = d.demod(bb, nframes, m.psdu_gold_code, m.psdu_ovsf_code)

    print "Produced", nframes, "Frames"
    print "Spreading Factor for Payload is", m.psdu_spreading_factor
    print "Spreading Factor for PHY header is", m.shr_spreading_factor
    print "Reset the goldcode per symbol for payload:", m.psdu_goldcode_reset_per_symbol
    print "Reset the goldcode per symbol for PHY header:", m.shr_goldcode_reset_per_symbol
    print "Using", m.ppdu_modulation,"Modulation"

    if(m.ppdu_modulation == 'BPSK'):
        rate = 1000
    else:
        rate = 2000

    bbrate = rate*m.filter_samples_per_symbol



    #spectrum
    fig, ax = plt.subplots(3)
    f, Pwelch = sig.welch(mod_data[1], bbrate / 1000, nperseg=2048)
    ax[0].semilogy(np.fft.fftshift(f), np.fft.fftshift(Pwelch))
    ax[0].set_xlabel('Frequency / MHz')
    ax[0].set_ylabel('Spectrum')
    ax[0].set_title('Spectrum Baseband')
    #timesignal
    ax[1].plot(mod_data[1][0:200].real)
    ax[1].set_ylim([-1.2,1.2])
    ax[1].plot(mod_data[1][0:200].imag)
    ax[1].set_xlabel('time / '+ str(1.0/bbrate*1000.0) + 's')
    ax[1].set_ylabel('')
    ax[1].set_title('Timesignal')
    ax[2].plot(mod_data[1][0:10000].real, mod_data[1][0:10000].imag)
    ax[2].set_xlim([-2.2, 2.2])
    ax[2].set_ylim([-2.2, 2.2])
    ax[2].set_title('IQ')

    #spectrum with noise
    fig, ax = plt.subplots(1)
    f, Pwelch = sig.welch(bb, bbrate / 1000, nperseg=2048)
    ax.semilogy(np.fft.fftshift(f), np.fft.fftshift(Pwelch))
    ax.set_xlabel('Frequency / MHz')
    ax.set_ylabel('Spectrum')
    ax.set_title('Spectrum with noise')

    #goldcode AKF
    fig, ax = plt.subplots(1)
    gc = m.psdu_gold_code[0:200000].astype(np.float32) * 2 - 1
    corr = np.correlate(gc,gc, mode='full')
    ax.plot(corr)
    ax.set_title('AKF Gold Code')

    #2 goldcodes with noise
    fig, ax = plt.subplots(2)
    gc1 = m.generate_goldcode(1, 20000).astype(np.float32)*2-1
    gc2 = m.generate_goldcode(99, 20000).astype(np.float32)*2-1
    sig = rnd.normal(size=100000, scale=np.sqrt(50))
    sig[20000:40000]+=gc1
    sig[50000:70000]+=gc2
    corr1 = np.correlate(sig,gc1, mode='full')
    corr2 = np.correlate(sig,gc2, mode='full')
    ax[0].plot(corr1[20000:-20000])
    ax[0].set_title('ccf with goldcode 1 and sum of 2 shifted different goldcodes with noise')
    ax[1].plot(corr2[20000:-20000])
    ax[1].set_title('ccf with goldcode 2 and sum of 2 shifted different goldcodes with noise')

    #test interleaver/deinterleaver
    random=np.random.randint(0, 2, size=(32 * 8 * 2,), dtype=np.uint8)
    interlvd = m.interleave(random)
    deinterleaved = d.deinterleave(interlvd)
    print "Intlvertest", str(sum(abs(random-deinterleaved)))

    #test diffcoder
    random=np.random.randint(0, 2, size=(32 * 8 * 2,), dtype=np.uint8)
    diffcoded = m.diff_encoder(random)
    decoded = d.demod_diffcoding((diffcoded.astype(np.float32)*2-1))
    print "Diffcodertest:", str(sum(abs(random[0:10]+(decoded[0:10]-1)/(2))))

    #spread/despread
    random=np.random.randint(0, 2, size=(32 * 8 * 2,), dtype=np.uint8)
    chips = m.spread_goldcode(random, 4, m.psdu_gold_code, False)
    despread = (d.despread(chips.astype(np.float32)*2-1, m.psdu_gold_code, np.array([1]), False, 4)-1)/-2
    print "spread:", str(random[0:10]), str(despread[0:10])

    #BER before convolutional coding
    cc_coded=mod_data[3]
    #hard demod
    demoded=np.zeros(len(demod_data))
    for i in range(len(demod_data)):
        demoded[i] = 1 if demod_data[i]<0 else 0
    print "BER without coding:", str(sum(abs(cc_coded-demoded)/len(demod_data)))

    plt.show()