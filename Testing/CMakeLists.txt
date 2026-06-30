# Fix tools tests to avoid modifications to source directory (bert).
# Use separate working directories for MINC1 (-1) and MINC2 (-2) test
# variants so that parallel ctest runs don't collide on temp files.

SET(TEST_INPUT_FILES
  test-zero.mnc
  test-one.mnc
  test-two.mnc
  test-rnd.mnc
  test-4d.mnc
  test-zero-byte.mnc
  test-labels.mnc
  test-dimname.mnc
  test-float.mnc
  test-double.mnc
  test-ratio.mnc
  mincaverage-in0.mnc
  mincaverage-in1.mnc
)

SET(MINCMORPH_INPUT_FILES
  mincmorph/test-erosion.mnc
  mincmorph/test-dilation.mnc
  mincmorph/test-padded.mnc
  mincmorph/test-distance.mnc
  mincmorph/test-shell.mnc
  mincmorph/test-two-shells.mnc
  mincmorph/test-mdilate.mnc
  mincmorph/test-group.mnc
  mincmorph/test-binarise.mnc
  mincmorph/test-clamp.mnc
  mincmorph/test-pad.mnc
  mincmorph/test-convolve.mnc
  mincmorph/test-lowpass.mnc
  mincmorph/test-open.mnc
  mincmorph/test-close.mnc
)

# Create per-variant working directories and copy test data into each
FOREACH(_variant v1 v2)
  FILE(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${_variant}")
  FILE(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${_variant}/mincmorph")
  FOREACH(_f ${TEST_INPUT_FILES})
    CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/${_f}"
                   "${CMAKE_CURRENT_BINARY_DIR}/${_variant}/${_f}" COPYONLY)
  ENDFOREACH()
  FOREACH(_f ${MINCMORPH_INPUT_FILES})
    CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/${_f}"
                   "${CMAKE_CURRENT_BINARY_DIR}/${_variant}/${_f}" COPYONLY)
  ENDFOREACH()
ENDFOREACH()

IF(MINC_TEST_ENVIRONMENT)
    SET(MINCTOOLS_TEST_ENVIRONMENT ${MINC_TEST_ENVIRONMENT})
ELSE()
    SET(MINCTOOLS_TEST_ENVIRONMENT "")
ENDIF()

SET(MINCTOOLS_TEST_ENVIRONMENT1 ${MINCTOOLS_TEST_ENVIRONMENT})
SET(MINCTOOLS_TEST_ENVIRONMENT2 ${MINCTOOLS_TEST_ENVIRONMENT})

LIST(APPEND MINCTOOLS_TEST_ENVIRONMENT1 "MINC_FORCE_V2=0")
LIST(APPEND MINCTOOLS_TEST_ENVIRONMENT2 "MINC_FORCE_V2=1")

MACRO(ADD_TEST12 name cmd)
  # test in minc1 mode — runs in v1/ directory
  ADD_TEST( ${name}-1
      ${cmd} ${ARGV2} ${ARGV3} ${ARGV4} ${ARGV5} ${ARGV6} ${ARGV7} ${ARGV8} ${ARGV9} ${ARGV10} ${ARGV11}
        ${ARGV12} ${ARGV13} ${ARGV14} ${ARGV15} ${ARGV16} ${ARGV17} ${ARGV18} ${ARGV19} ${ARGV20} ${ARGV21} ${ARGV22}
        ${ARGV23} ${ARGV24} ${ARGV25} ${ARGV26}
   )
  set_tests_properties( ${name}-1 PROPERTIES
    ENVIRONMENT "${MINCTOOLS_TEST_ENVIRONMENT1}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/v1")
  
  # test in minc2 mode — runs in v2/ directory
  ADD_TEST( ${name}-2
      ${cmd} ${ARGV2} ${ARGV3} ${ARGV4} ${ARGV5} ${ARGV6} ${ARGV7} ${ARGV8} ${ARGV9} ${ARGV10} ${ARGV11}
        ${ARGV12} ${ARGV13} ${ARGV14} ${ARGV15} ${ARGV16} ${ARGV17} ${ARGV18} ${ARGV19} ${ARGV20} ${ARGV21} ${ARGV22}
        ${ARGV23} ${ARGV24} ${ARGV25} ${ARGV26}
   )
  set_tests_properties( ${name}-2 PROPERTIES
    ENVIRONMENT "${MINCTOOLS_TEST_ENVIRONMENT2}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/v2")
ENDMACRO(ADD_TEST12)


ADD_TEST12(mincinfo-test ${CMAKE_CURRENT_SOURCE_DIR}/mincinfo-test.pl)

ADD_TEST12(mincstats-test ${CMAKE_CURRENT_SOURCE_DIR}/mincstats-test.pl)

ADD_TEST12(mincaverage-test ${CMAKE_CURRENT_SOURCE_DIR}/mincaverage-test.sh)

ADD_TEST12(minccalc-test ${CMAKE_CURRENT_SOURCE_DIR}/minccalc-test.sh)

ADD_TEST12(minccalc-labels-test ${CMAKE_CURRENT_SOURCE_DIR}/minccalc-labels-test.sh)

ADD_TEST12(minclookup-labels-test ${CMAKE_CURRENT_SOURCE_DIR}/minclookup-labels-test.sh)

ADD_TEST12(mincresample-test ${CMAKE_CURRENT_SOURCE_DIR}/mincresample-test.sh)

ADD_TEST12(mincreshape-test ${CMAKE_CURRENT_SOURCE_DIR}/mincreshape-test.pl)

ADD_TEST12(mincmorph-test ${CMAKE_CURRENT_SOURCE_DIR}/mincmorph-test.sh)

ADD_TEST12(mincmorph-fillholes-test ${CMAKE_CURRENT_SOURCE_DIR}/mincmorph-fillholes-test.sh)

ADD_TEST12(minccmp-test ${CMAKE_CURRENT_SOURCE_DIR}/minccmp-test.pl)

ADD_TEST12(mnc2nii-compress-test ${CMAKE_CURRENT_SOURCE_DIR}/mnc2nii-compress-test.sh)

# ------------ nii2mnc dimension-naming regression tests ----------------------
# Self-contained: tiny committed NIfTI fixtures (nii2mnc_data/, built by
# make_nii_fixtures.py) exercise the file-axis -> MINC spatial-axis assignment.
# The *_zeroslice fixtures reproduce a degenerate s-form (slice column all zero)
# that used to make nii2mnc emit "xspace" twice and drop "zspace".
SET(_nii2mnc_bin ${CMAKE_CURRENT_SOURCE_DIR}/nii2mnc-dimnames-test.sh)
SET(_nii2mnc_dir ${CMAKE_CURRENT_SOURCE_DIR}/nii2mnc_data)

MACRO(ADD_NII2MNC_TEST _case)
  ADD_TEST(NAME nii2mnc-dimnames-${_case}
    COMMAND ${_nii2mnc_bin} $<TARGET_FILE:nii2mnc> $<TARGET_FILE:mincinfo>
            ${_nii2mnc_dir}/${_case}.nii ${ARGN}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  set_tests_properties(nii2mnc-dimnames-${_case} PROPERTIES
    ENVIRONMENT "${MINCTOOLS_TEST_ENVIRONMENT}" LABELS "nii2mnc")
ENDMACRO(ADD_NII2MNC_TEST)

ADD_NII2MNC_TEST(nii_3d_ras       xspace yspace zspace)
ADD_NII2MNC_TEST(nii_3d_permuted  xspace yspace zspace)
ADD_NII2MNC_TEST(nii_4d_normal    time xspace yspace zspace)
ADD_NII2MNC_TEST(nii_3d_zeroslice xspace yspace zspace)
ADD_NII2MNC_TEST(nii_4d_zeroslice time xspace yspace zspace)

# ------------ dcm2mnc integration tests (opt-in, data fetched at build time) -

if(MT_BUILD_DCM2MNC_TESTS)

  include(ExternalProject)

  set(_dcm2mnc      $<TARGET_FILE:dcm2mnc>)
  set(_mincinfo     $<TARGET_FILE:mincinfo>)
  set(_mincstats    $<TARGET_FILE:mincstats>)
  set(_cmp          ${CMAKE_CURRENT_SOURCE_DIR}/dcm2mnc-compare-test.sh)
  set(_expdir       ${CMAKE_CURRENT_SOURCE_DIR}/dcm2mnc_expected)
  set(_outbase      ${CMAKE_CURRENT_BINARY_DIR}/dcm2mnc_out)
  set(_datadir      ${CMAKE_CURRENT_BINARY_DIR}/dcm2niix_data)

  # Download-only ExternalProjects — configure/build/install are no-ops.
  # DOWNLOAD_EXTRACT_TIMESTAMP TRUE suppresses the CMP0135 warning on CMake >= 3.24.
  # ExternalProject flattens the single <repo>-<sha>/ top-level dir that GitHub
  # puts inside archive tarballs, so In/... resolves directly under SOURCE_DIR.
  ExternalProject_Add(dcm_qa_data
    URL        "https://github.com/neurolabusc/dcm_qa/archive/4f85d067c8b61d12cec1689f8516cc30cd583d65.tar.gz"
    SOURCE_DIR "${_datadir}/dcm_qa"
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    CONFIGURE_COMMAND "" BUILD_COMMAND "" INSTALL_COMMAND "")

  ExternalProject_Add(dcm_qa_nih_data
    URL        "https://github.com/neurolabusc/dcm_qa_nih/archive/d302d6e241dda747b79ee1724ada3200179755a2.tar.gz"
    SOURCE_DIR "${_datadir}/dcm_qa_nih"
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    CONFIGURE_COMMAND "" BUILD_COMMAND "" INSTALL_COMMAND "")

  ExternalProject_Add(dcm_qa_uih_data
    URL        "https://github.com/neurolabusc/dcm_qa_uih/archive/d729bdd785d37ad5f1ed479c305a76f370d8011b.tar.gz"
    SOURCE_DIR "${_datadir}/dcm_qa_uih"
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    CONFIGURE_COMMAND "" BUILD_COMMAND "" INSTALL_COMMAND "")

  # Each test converts one DICOM series and checks the output geometry +
  # statistics against a committed manifest (see dcm2mnc_expected/). No
  # reference volumes are stored; the manifests are machine-generated by
  # dcm2mnc_expected/regenerate.sh and pin dcm2mnc's own output (centre of
  # mass cross-validated against the dcm2niix reference NIfTI via nii2mnc).
  #
  # The test set is data-driven: series.list holds one "name|repo|relpath"
  # row per convertible series (JPEG-compressed and crashing series are
  # excluded by the generator). To add coverage, rebuild and re-run the
  # generator, then commit the refreshed manifests + series.list.

  file(STRINGS ${_expdir}/series.list _dcm2mnc_series)
  foreach(_row ${_dcm2mnc_series})
    if(_row MATCHES "^#" OR _row STREQUAL "")
      continue()
    endif()
    # row = name|repo|relpath
    string(REPLACE "|" ";" _parts "${_row}")
    list(GET _parts 0 _name)
    list(GET _parts 1 _repo)
    list(GET _parts 2 _rel)
    add_test(NAME dcm2mnc-test-${_name}
      COMMAND ${_cmp} ${_dcm2mnc} ${_mincinfo} ${_mincstats}
        ${_outbase}/${_name}
        "${_datadir}/${_repo}/${_rel}"
        ${_expdir}/${_name}.manifest)
    set_tests_properties(dcm2mnc-test-${_name} PROPERTIES
      ENVIRONMENT "${MINCTOOLS_TEST_ENVIRONMENT}" LABELS "dcm2mnc")
  endforeach()

endif() # MT_BUILD_DCM2MNC_TESTS

# ------------ dcm2mnc tests on LOCAL (non-redistributable) DICOM data --------
#
# Some regression cases rely on DICOM exports that cannot be downloaded by CI
# (e.g. clinical data). Point MT_DCM2MNC_LOCAL_DATA at a directory holding such
# data; these tests then run against manifests stored WITH that data, under
# ${MT_DCM2MNC_LOCAL_DATA}/dcm2mnc_manifests/. Nothing local lives in this repo:
# the only knob here is the MT_DCM2MNC_LOCAL_DATA path. The manifests, and the
# generator that produces them, travel with the dataset (they are derived from,
# and only meaningful with, the private data). When the variable is empty or the
# path does not exist, no tests are registered.
#
# The manifests are produced by a regenerate-local.sh kept at the data root
# (same dcm2niix ground-truth pipeline as the downloaded set). Each manifest is
# self-describing: a "series" line gives its path relative to the data root, so
# tests are discovered by globbing the manifest dir -- there is no index file.
# Unlike the downloaded tests these need no internet, only the local data path.

set(MT_DCM2MNC_LOCAL_DATA "" CACHE PATH
    "Path to local (non-downloadable) DICOM data root for extra dcm2mnc regression tests")

if(MT_DCM2MNC_LOCAL_DATA AND EXISTS "${MT_DCM2MNC_LOCAL_DATA}")

  set(_ldcm2mnc   $<TARGET_FILE:dcm2mnc>)
  set(_lmincinfo  $<TARGET_FILE:mincinfo>)
  set(_lmincstats $<TARGET_FILE:mincstats>)
  set(_lcmp       ${CMAKE_CURRENT_SOURCE_DIR}/dcm2mnc-compare-test.sh)
  set(_lmandir    "${MT_DCM2MNC_LOCAL_DATA}/dcm2mnc_manifests")
  set(_loutbase   ${CMAKE_CURRENT_BINARY_DIR}/dcm2mnc_local_out)

  # Discover tests by globbing the manifests stored with the data. Each manifest
  # carries its series relpath in a "series" line. (file(GLOB) runs at configure
  # time, so regenerating manifests needs a re-configure, like the downloaded set.)
  file(GLOB _local_manifests "${_lmandir}/*.manifest")
  foreach(_man ${_local_manifests})
    get_filename_component(_name ${_man} NAME_WLE)
    file(STRINGS ${_man} _sline REGEX "^series[ \t]+")
    if(NOT _sline)
      continue()
    endif()
    list(GET _sline 0 _sline0)
    string(REGEX REPLACE "^series[ \t]+" "" _rel "${_sline0}")
    string(STRIP "${_rel}" _rel)
    if(EXISTS "${MT_DCM2MNC_LOCAL_DATA}/${_rel}")
      add_test(NAME dcm2mnc-local-${_name}
        COMMAND ${_lcmp} ${_ldcm2mnc} ${_lmincinfo} ${_lmincstats}
          ${_loutbase}/${_name}
          "${MT_DCM2MNC_LOCAL_DATA}/${_rel}"
          ${_man})
      set_tests_properties(dcm2mnc-local-${_name} PROPERTIES
        ENVIRONMENT "${MINCTOOLS_TEST_ENVIRONMENT}" LABELS "dcm2mnc;dcm2mnc-local")
    endif()
  endforeach()

endif() # MT_DCM2MNC_LOCAL_DATA
