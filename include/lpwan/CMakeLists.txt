#
# Copyright 2013 Free Software Foundation, Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
install(FILES
    dsss_interleaver_bb.h
    dsss_codes.h
    dsss_diff_coding_bb.h
    dsss_spreading_bb.h
    dsss_deinterleaver_ff.h
    dsss_diff_decoding_ff.h
    dsss_despread_simple_cc.h
    dsss_preamble_demod_cc.h
    conj_multiply_delay_ccc.h
    dsss_preamble_detector_cc.h
    dsss_despread_cc.h
    dsss_normalize_ff.h
    dsss_snr_estimator.h
    dsss_fragmentation.h
    mac_format.h
    mac_field.h
    mac_field_frame_ctrl.h
    mac_field_frag_header.h
    mac_field_frag_status.h
    mac_field_frame.h
        mac_crc.h
    mac_field_header_ie.h
    mac_field_ie_fscd.h
    mac_field_fragment.h
    mac_field_frak.h
    dsss_filter_crc_packets.h
    fddsm_mod_bcb.h
    fddsm_soft_demod_bpsk_cf.h
    fddsm_modulator_kernel.h
    fddsm_demodulator_kernel.h DESTINATION ${GR_INCLUDE_DIR}/gnuradio/lpwan
    COMPONENT "lpwan_devel"
)
