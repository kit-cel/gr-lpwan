import numpy as np


def calculate_phase_increments(samp_rate_hz, SF, sps, max_offset_hz, max_num_filters):
    filter_spacing_normalized = 2 * 0.1 * 1.0 / SF / sps  # 0.44 corresponds to about 3 dB maximum loss due to decorrelation for conventional IEEE LECIM with BPSK
    max_offset_normalized = float(max_offset_hz) / samp_rate_hz
    num_filters = int(np.ceil(max_offset_normalized * 2.0 / filter_spacing_normalized))
    if num_filters == 0:  # this occurs for max_offset_hz==0
        num_filters = 1
    print "Number of filters requested:", num_filters
    if num_filters > max_num_filters:
        if max_num_filters == 0:
            raise ValueError("At least one input filter must be allowed")
        print "WARNING:", num_filters, "required to cover the possible frequency offset, but only", max_num_filters, "possible"
        num_filters = max_num_filters

    achievable_max_offset_normalized = min(max_offset_normalized, num_filters * filter_spacing_normalized / 2)
    actual_filter_spacing_normalized = 2 * achievable_max_offset_normalized / num_filters

    delta_phi = [-achievable_max_offset_normalized + actual_filter_spacing_normalized/2 + i * actual_filter_spacing_normalized for i in range(num_filters)]
    print "Phase increments for each branch at sample rate / in Hz:", delta_phi, "/", np.array(delta_phi) * samp_rate_hz
    return delta_phi