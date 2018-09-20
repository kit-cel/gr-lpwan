from SpaRSe_TX_USRP_noGUI import SpaRSe_TX_USRP_noGUI as tx
from SpaRSe_RX_USRP_noGUI import SpaRSe_RX_USRP_noGUI as rx

import numpy as np
import time
import matplotlib.pyplot as plt


if __name__ == "__main__":
    niterations = 5
    avg_PER = []
    for n in range(niterations):
        print "iteration no.", n, "out of ", niterations
        SNR_range = np.arange(0, 16, 1)
        SF = 256
        tx_gain_range = SNR_range + 36.2  # this transforms actually to Eb/N0 (02/07/18)!
        rx_gain = 0.8
        txsps = 2
        rxsps = 4
        alpha = 1e-3
        beta = 5

        num_packets = 200
        time_per_packet = float((SF + 10) * (256 + 32)) / 1e6
        measurement_time = num_packets * time_per_packet
        datestr = str(int(time.time()))

        print "estimated time per iteration:", (10 + num_packets * time_per_packet) * len(SNR_range), "seconds"
        with open("SpaRSe_OTA_results_"+datestr+".txt", 'w') as f:
            f.write("Settings: \nSNR_range={}\nSF={}\nnum_packets={}\nalpha={}\nbeta={}\n\n".format(SNR_range, SF, num_packets, alpha, beta))
            PER = []
            packets_received = []
            npackets_sent = []
            f.write("\nResults:\n")
            for idx, gain in enumerate(tx_gain_range):
                TX = tx(SF=SF, sps=txsps, gain=gain)
                TX.start()

                RX = rx(SF=SF, sps=rxsps, alpha=alpha, beta=beta, gain=rx_gain)
                RX.lpwan_error_rate_calculation_0.stop_counting_packets()
                RX.start()
                print("Sleep for 10 seconds to allow for receiver settling")
                time.sleep(10)
                RX.lpwan_error_rate_calculation_0.start_counting_packets()
                Tstart = time.time()
                print "Sleep for", measurement_time, "seconds and measure PER"
                time.sleep(measurement_time)
                RX.lpwan_error_rate_calculation_0.stop_counting_packets()

                TX.stop()
                TX.wait()
                T = time.time() - Tstart
                npackets_sent.append(T / time_per_packet)
                RX.stop()
                RX.wait()

                PER.append(RX.lpwan_error_rate_calculation_0.get_overall_PER())
                packets_received.append(RX.lpwan_error_rate_calculation_0.get_unique_packets())
                print "PER / packets received/packets sent @", gain, "dB gain aka ", SNR_range[idx], "dB SNR:", PER[-1], "/", packets_received[-1], "/", npackets_sent[-1]
                f.write("Gain={}\tSNR={}\tPER={}\tPackets={}/{}\n".format(gain, SNR_range[idx], PER[-1], packets_received[-1], npackets_sent[-1]))
                f.flush()

        result = zip(SNR_range, PER, packets_received)
        PER2 = 1 - np.array(packets_received, dtype=float) / np.array(npackets_sent)
        print "final result:", result

        plt.plot(SNR_range, PER, label="measured by receiver")
        plt.plot(SNR_range, PER2, label="estimated from received packets")
        plt.title("PER")
        plt.savefig("SpaRSe_OTA_results_"+datestr+".pdf")
        plt.close()

        avg_PER.append(PER2)

    avg_PER = np.array(avg_PER)
    print "averaged PER:", avg_PER
    np.save("avg_PER_"+str(int(time.time()))+".npy", avg_PER)
    print(np.mean(avg_PER, axis=0))
    print(np.mean(avg_PER, axis=1))

