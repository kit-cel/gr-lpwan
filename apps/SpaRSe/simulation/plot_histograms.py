import numpy as np
import matplotlib.pyplot as plt

softbits = np.fromfile('demod_softbits.bin', dtype=np.float32)
after_despreading = np.fromfile('despreading_filter.bin', dtype=np.complex64)
preamble_correlation = np.fromfile('preamble_correlation.bin', dtype=np.float32)

print("Input power:", 10*np.log10(np.mean(np.abs(after_despreading)**2)), "dB")
### plot histograms for all signals with complex signals split in I/Q

# despreading filter output
plt.hist(np.real(after_despreading), bins=1000, normed=True, label='real')
plt.hist(np.imag(after_despreading), normed=True, bins=1000, label='imag')
plt.title('Despreading filter output')
plt.show()

# softbits
plt.hist(softbits, normed=True, bins=1000)
plt.title('Soft bits')
plt.show()

# preamble correlation
plt.hist(preamble_correlation, normed=True, cumulative=False, bins=1000)
x = np.linspace(-10, 10, 1000)
import matplotlib.mlab as mlab
plt.plot(x, mlab.normpdf(x, 0, 1)) 
plt.title('Preamble correlation')
plt.show()
