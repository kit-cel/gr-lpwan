import numpy as np
import matplotlib.pyplot as plt


def calculate_loss(case="A", SF=64, K=10):
    L = SF + K  # approximately the length of one symbol
    fn = np.linspace(-1./SF, 1./SF, 1000)
    ret = np.zeros(fn.size)
    for n in range(1000): # average over many realizations to get a picture of the average behavior
        ret += (C(fn, L) * X(fn, L, case))**2
    ret /= 1000
    return fn, ret 


def C(fn, L):
    return (np.sin(np.pi * fn * L) / np.sin(np.pi * fn) / L) **2


def X(fn, L, case):
    if case == "double-step":
        return np.cos(2 * 2 * np.pi * fn * L)
    elif case == "mixed-single-triple-step":
        h = 1./np.sqrt(2) * (np.random.randn(2) + 1j * np.random.randn(2))
        return (np.abs(h[0])**2 * np.cos(2 * np.pi * fn * L) + np.abs(h[1])**2 * np.cos(3 * 2 * np.pi * fn * L)) / np.sum(np.abs(h)**2)
    elif case == "single-step":
        return np.cos(2 * np.pi * fn * L)


if __name__ == "__main__":
    fs = 1e6
    SF = 256 
    K = 10
    L = SF + K
    fn, lossA = calculate_loss(case="double-step", SF=SF, K=10)
    fn, lossB = calculate_loss(case="mixed-single-triple-step", SF=SF, K=10)
    XAsq = X(fn, L, "double-step")**2
    XBsq = X(fn, L, "mixed-single-triple-step")**2
    XCsq = X(fn, L, "single-step")**2
    Csq = C(fn, SF + K)**2
    plt.plot(fn*fs, lossA, label='double')
    plt.plot(fn*fs, lossB, label='mixed single/triple')
    plt.plot(fn*fs, XCsq, label='demodulation loss (single-step)')
    plt.plot(fn*fs, (lossA + lossB)/2, label='avg loss FD-DSM')
    plt.plot(fn*fs, Csq, label='despreading decorrelation')
    plt.plot(fn*fs, XAsq, label='demodulation loss (double step)')
    plt.plot(fn*fs, XBsq, label='demodulation loss (mixed single/triple step)')
    plt.axvline(x=0.5 * fs / SF)
    plt.axvline(x=-0.5 * fs / SF)
    plt.axhline(y=0.5)
    plt.title("SNR loss due to frequency offset(SF={})".format(SF))
    plt.legend()
    plt.show()

