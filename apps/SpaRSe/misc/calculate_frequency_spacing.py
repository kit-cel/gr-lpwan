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
    if case == "A":
        return np.cos(2 * 2 * np.pi * fn * L)
    elif case == "B":
        h = 1./np.sqrt(2) * (np.random.randn(2) + 1j * np.random.randn(2))
        return (np.abs(h[0])**2 * np.cos(2 * np.pi * fn * L) + np.abs(h[1])**2 * np.cos(3 * 2 * np.pi * fn * L)) / np.sum(np.abs(h)**2)


if __name__ == "__main__":
    fs = 1e6
    SF = 256 
    K = 10
    L = SF + K
    fn, lossA = calculate_loss(case="A", SF=SF, K=10)
    fn, lossB = calculate_loss(case="B", SF=SF, K=10)
    XAsq = X(fn, L, "A")**2
    XBsq = X(fn, L, "B")**2
    Csq = C(fn, SF + K)**2
    plt.plot(fn*fs, lossA, label='A')
    plt.plot(fn*fs, lossB, label='B')
    plt.plot(fn*fs, (lossA + lossB)/2, label='avg loss')
    plt.plot(fn*fs, Csq, label='despreading decorrelation')
    plt.plot(fn*fs, XAsq, label='demodulation loss (A)')
    plt.plot(fn*fs, XBsq, label='demodulation loss (B)')
    plt.axvline(x=0.5 * fs / SF)
    plt.axvline(x=-0.5 * fs / SF)
    plt.title("SNR loss due to frequency offset(SF={})".format(SF))
    plt.legend()
    plt.show()

