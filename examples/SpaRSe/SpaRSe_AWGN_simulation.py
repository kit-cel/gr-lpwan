import numpy as np
import os
import time
import datetime
import matplotlib.pyplot as plt


if __name__ == "__main__":
    print "PID:", os.getpid()

    SF = 256
    max_time = 30
    num_packets = 100
    phi = 0
    perfect_sync = False
    beta = 5
    EbN0_range = np.arange(0, 16, 1)

    # calculate SNR from Eb/N0
    bpcu = 1.0
    R = 1.0/2
    SNR_range = EbN0_range + 10*np.log10(bpcu*R/SF)
    print "SNR range:", SNR_range

    # set file name
    fname = "SpaRSe_simulation_results_AWGN_"+str(int(time.time()))

    if not perfect_sync:
        from SpaRSe_loopback_noGUI import SpaRSe_loopback_noGUI as SpaRSe_AWGN
    else:
        from SpaRSe_loopback_noGUI_noSync import SpaRSe_loopback_noGUI_noSync as SpaRSe_AWGN

    PER = []
    unique_packets = []
    for snr in SNR_range:
        t0 = time.time()
        print "SNR=", snr, "dB"
        sim = SpaRSe_AWGN(SF=SF)
        sim.set_snr_db(snr)
        sim.set_num_packets(num_packets)
        sim.set_phi(phi)
        sim.set_beta(beta)  # this has no impact for perfect sync
        sim.lpwan_error_rate_calculation_0.start_counting_packets()
        sim.start()
        while time.time() - t0 < max_time and sim.lpwan_error_rate_calculation_0.get_unique_packets() < num_packets:
            time.sleep(1)
        print "Stop and wait for flow graph"
        sim.stop()
        sim.wait()
        sim.lpwan_error_rate_calculation_0.stop_counting_packets()
        print "PER:", sim.lpwan_error_rate_calculation_0.get_overall_PER()
        print "unique packets:", sim.lpwan_error_rate_calculation_0.get_unique_packets()
        unique_packets.append(sim.lpwan_error_rate_calculation_0.get_unique_packets())
        PER.append(sim.lpwan_error_rate_calculation_0.get_overall_PER())
    result = zip(EbN0_range, SNR_range, unique_packets, PER)
    print "final result:", result
    with open(fname+".txt", 'w') as f:
        f.write("Settings: \nSNR_range={}\nSF={}\nnum_packets={}\tphi={}\tbeta={}\tperfect sync={}\n\n".format(SNR_range, SF, num_packets, phi, beta, perfect_sync))
        f.write("Results: \n{}".format(result))

    plt.figure()
    plt.plot(EbN0_range, PER)
    plt.grid()
    plt.title("PER")
    plt.savefig(fname+".pdf")
    
