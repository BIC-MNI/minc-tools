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
#TODO: add more tests?
