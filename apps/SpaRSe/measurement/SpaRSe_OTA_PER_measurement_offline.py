from SpaRSe_RX_fromFile_noGUI import SpaRSe_RX_fromFile_noGUI as rx

import numpy as np
import time
import matplotlib.pyplot as plt


if __name__ == "__main__":

    EbN0_range = np.arange(0, 16, 1)
    SF = 256
    rxsps = 4
    alpha = 1e-3
    beta = 3
    basepath = "/home/wunsch/Documents/grcon18/figures/Messung_02-07-18_2/"

    filenames = [basepath+"ebn0_" + str(ebn0) + "dB.bin" for ebn0 in EbN0_range]

    time_per_packet = float((SF + 10) * (256 + 32)) / 1e6
    measurement_time = 20
    num_packets = int(measurement_time / time_per_packet)

    PER = []
    packets_received = []
    for i, fname in enumerate(filenames):
        print("opening " + fname)
        RX = rx(SF=SF, sps=rxsps, alpha=alpha, beta=beta, fname=fname)
        RX.lpwan_error_rate_calculation_0.start_counting_packets()
        RX.start()
        print "Sleep for", measurement_time, "seconds and measure PER"
        time.sleep(measurement_time)
        RX.stop()
        RX.wait()
        RX.lpwan_error_rate_calculation_0.stop_counting_packets()

        PER.append(RX.lpwan_error_rate_calculation_0.get_overall_PER())
        packets_received.append(RX.lpwan_error_rate_calculation_0.get_unique_packets())
        print "PER / packets received/packets sent", PER[-1], "/", packets_received[-1], "/", num_packets

    result = zip(EbN0_range, PER, packets_received)
    PER2 = 1 - np.array(packets_received, dtype=float) / np.array(num_packets)
    print "final result:", result

    plt.plot(EbN0_range, PER, label="measured by receiver")
    plt.plot(EbN0_range, PER2, label="estimated from received packets")
    plt.title("PER, beta={}".format(beta))
    plt.savefig("PER_beta{}.pdf".format(beta))
    plt.close()

    np.save("PER_beta{}.npy".format(beta), PER2)
    for i in range(len(PER2)):
        print("{}\t{}\\\\".format(EbN0_range[i], PER2[i]))

