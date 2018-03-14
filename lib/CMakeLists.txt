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

#####################################################################
# Setup library
#####################################################################
include(GrPlatform) #define LIB_SUFFIX
list(APPEND lpwan_sources
    dsss_interleaver_bb_impl.cc
    dsss_codes.cc
    dsss_diff_coding_bb_impl.cc
    dsss_spreading_bb_impl.cc
    dsss_deinterleaver_ff_impl.cc
    dsss_diff_decoding_ff_impl.cc
    dsss_despread_simple_cc_impl.cc
    dsss_preamble_demod_cc_impl.cc
    conj_multiply_delay_ccc_impl.cc
    dsss_preamble_detector_cc_impl.cc
    dsss_despread_cc_impl.cc
    dsss_normalize_ff_impl.cc
    dsss_snr_estimator_impl.cc
    mac_format.cc
    dsss_fragmentation_impl.cc
    mac_field.cc
    mac_field_frame_ctrl.cc
    mac_field_frag_header.cc
    mac_field_frag_status.cc
    mac_field_frame.cc
    mac_crc.cc
    mac_field_header_ie.cc
    mac_field_ie_fscd.cc
    mac_field_fragment.cc
    mac_field_frak.cc
    dsss_filter_crc_packets_impl.cc
    fddsm_mod_bcb_impl.cc
    fddsm_soft_demod_bpsk_cf_impl.cc
    fddsm_modulator_kernel.cc
    fddsm_demodulator_kernel.cc
    fddsm_preamble_detector_cc_impl.cc
    sliding_dotprod_32f_x2_32f.cc )

list(APPEND lpwan_libs
    ${Boost_LIBRARIES}
    ${GNURADIO_ALL_LIBRARIES}
    ${VOLK_LIBRARIES}
)

set(lpwan_sources "${lpwan_sources}" PARENT_SCOPE)
if(NOT lpwan_sources)
    MESSAGE(STATUS "No C++ sources... skipping lib/")
    return()
endif(NOT lpwan_sources)

add_library(gnuradio-lpwan SHARED ${lpwan_sources})
target_link_libraries(gnuradio-lpwan ${Boost_LIBRARIES} ${lpwan_libs})
set_target_properties(gnuradio-lpwan PROPERTIES DEFINE_SYMBOL "gnuradio_lpwan_EXPORTS")

#####################################################################
# Install built library files
#####################################################################
install(TARGETS gnuradio-lpwan
    LIBRARY DESTINATION lib${LIB_SUFFIX} # .so/.dylib file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)