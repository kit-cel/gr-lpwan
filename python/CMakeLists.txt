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
# Setup python install
#####################################################################
include(GrPython)
if(NOT PYTHONINTERP_FOUND)
    return()
endif()

list(APPEND python_sources
    __init__.py
    dsss_const.py
    dsss_phy.py
    dsss_mod.py
)
GR_PYTHON_INSTALL(
    FILES ${python_sources}
    dsss_preamble_search_cc.py
    message_counter.py DESTINATION ${GR_PYTHON_DIR}/lpwan
    COMPONENT "lpwan_python"
)

#####################################################################
# Handle the unit tests
#####################################################################
include(GrTest)

set(GR_TEST_TARGET_DEPS gnuradio-lpwan)
set(GR_TEST_PYTHON_DIRS ${CMAKE_BINARY_DIR}/swig)
GR_ADD_TEST(qa_dsss_interleaver_bb ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_dsss_interleaver_bb.py)
GR_ADD_TEST(qa_dsss_diff_coding_bb ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_dsss_diff_coding_bb.py)
GR_ADD_TEST(qa_dsss_spreading_bb ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_dsss_spreading_bb.py)
GR_ADD_TEST(qa_dsss_deinterleaver_ff ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_dsss_deinterleaver_ff.py)
GR_ADD_TEST(qa_dsss_diff_decoding_ff ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_dsss_diff_decoding_ff.py)
GR_ADD_TEST(qa_dsss_despread_simple_cc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_dsss_despread_simple_cc.py)
GR_ADD_TEST(qa_dsss_preamble_search_cc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_dsss_preamble_search_cc.py)
GR_ADD_TEST(qa_dsss_preamble_demod_cc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_dsss_preamble_demod_cc.py)
GR_ADD_TEST(qa_conj_multiply_delay_ccc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_conj_multiply_delay_ccc.py)
GR_ADD_TEST(qa_dsss_preamble_detector_cc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_dsss_preamble_detector_cc.py)
GR_ADD_TEST(qa_dsss_despread_cc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_dsss_despread_cc.py)
GR_ADD_TEST(qa_dsss_normalize_ff ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_dsss_normalize_ff.py)
GR_ADD_TEST(qa_dsss_fragmentation ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_dsss_fragmentation.py)
GR_ADD_TEST(qa_dsss_snr_estimator ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_dsss_snr_estimator.py)
GR_ADD_TEST(qa_fddsm_mod_bcb ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_fddsm_mod_bcb.py)
GR_ADD_TEST(qa_fddsm_soft_demod_bpsk_cf ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_fddsm_soft_demod_bpsk_cf.py)
GR_ADD_TEST(qa_fddsm_preamble_detector_cc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_fddsm_preamble_detector_cc.py)
GR_ADD_TEST(qa_packet_demux_cc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_packet_demux_cc.py)
