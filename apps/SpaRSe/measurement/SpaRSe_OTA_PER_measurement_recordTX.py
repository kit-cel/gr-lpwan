from SpaRSe_TX_USRP_noGUI import SpaRSe_TX_USRP_noGUI as tx
from SpaRSe_RX_USRP_noGUI_recordOnly import SpaRSe_RX_USRP_noGUI_recordOnly as rx

import numpy as np
import time
import matplotlib.pyplot as plt


if __name__ == "__main__":
        EbN0_range = np.arange(13, 14, 1)
        SF = 256
        tx_gain_range = EbN0_range + 36.2  # this transforms actually to Eb/N0 (02/07/18)!
        rx_gain = 0.8
        txsps = 2
        rxsps = 4
        alpha = 1e-3
        beta = 5
        basepath = "/home/wunsch/Documents/grcon18/figures/Messung_02-07-18_2/"

        num_packets = 1000
        time_per_packet = float((SF + 10) * (256 + 32)) / 1e6
        tx_time = num_packets * time_per_packet

        for idx, gain in enumerate(tx_gain_range):
            TX = tx(SF=SF, sps=txsps, gain=gain)
            TX.start()

            RX = rx(SF=SF,alpha=alpha, beta=beta, fname=basepath+"ebn0_"+str(EbN0_range[idx])+"dB.bin", sps=rxsps)
            RX.start()
            print("Measuring at Eb/N0=" + str(EbN0_range[idx]) + "dB. Sleep for 30 seconds")
            time.sleep(30)

            TX.stop()
            TX.wait()
            RX.stop()
            RX.wait()

