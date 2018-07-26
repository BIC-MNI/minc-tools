# Fix tools tests to avoid modifications to source directory (bert).
# 
# Get path to minctools binaries.
GET_PROPERTY(mincstats_bin TARGET mincstats PROPERTY LOCATION)
GET_PROPERTY(mincinfo_bin TARGET mincinfo PROPERTY LOCATION)
GET_PROPERTY(mincextract_bin TARGET mincextract PROPERTY LOCATION)
GET_PROPERTY(mincaverage_bin TARGET mincaverage PROPERTY LOCATION)
GET_PROPERTY(mincresample_bin TARGET mincresample PROPERTY LOCATION)
GET_PROPERTY(minccalc_bin TARGET minccalc PROPERTY LOCATION)
GET_PROPERTY(mincreshape_bin TARGET mincreshape PROPERTY LOCATION)
GET_PROPERTY(mincmorph_bin TARGET mincmorph PROPERTY LOCATION)
GET_PROPERTY(minccmp_bin TARGET minccmp PROPERTY LOCATION)

# Copy files used by minccalc-test and other tests.
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/test-zero.mnc" "${CMAKE_CURRENT_BINARY_DIR}/test-zero.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/test-one.mnc"  "${CMAKE_CURRENT_BINARY_DIR}/test-one.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/test-two.mnc"  "${CMAKE_CURRENT_BINARY_DIR}/test-two.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/test-rnd.mnc"  "${CMAKE_CURRENT_BINARY_DIR}/test-rnd.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/test-4d.mnc"   "${CMAKE_CURRENT_BINARY_DIR}/test-4d.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/test-zero-byte.mnc" "${CMAKE_CURRENT_BINARY_DIR}/test-zero-byte.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/test-labels.mnc" "${CMAKE_CURRENT_BINARY_DIR}/test-labels.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/test-dimname.mnc"  "${CMAKE_CURRENT_BINARY_DIR}/test-dimname.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/test-float.mnc"   "${CMAKE_CURRENT_BINARY_DIR}/test-float.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/test-double.mnc" "${CMAKE_CURRENT_BINARY_DIR}/test-double.mnc" COPYONLY)

# Copy files used by mincmorph-test
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/mincmorph/test-erosion.mnc" "${CMAKE_CURRENT_BINARY_DIR}/mincmorph/test-erosion.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/mincmorph/test-dilation.mnc"  "${CMAKE_CURRENT_BINARY_DIR}/mincmorph/test-dilation.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/mincmorph/test-padded.mnc"  "${CMAKE_CURRENT_BINARY_DIR}/mincmorph/test-padded.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/mincmorph/test-distance.mnc"  "${CMAKE_CURRENT_BINARY_DIR}/mincmorph/test-distance.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/mincmorph/test-shell.mnc"   "${CMAKE_CURRENT_BINARY_DIR}/mincmorph/test-shell.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/mincmorph/test-two-shells.mnc" "${CMAKE_CURRENT_BINARY_DIR}/mincmorph/test-two-shells.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/mincmorph/test-mdilate.mnc" "${CMAKE_CURRENT_BINARY_DIR}/mincmorph/test-mdilate.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/mincmorph/test-group.mnc"  "${CMAKE_CURRENT_BINARY_DIR}/mincmorph/test-group.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/mincmorph/test-binarise.mnc"  "${CMAKE_CURRENT_BINARY_DIR}/mincmorph/test-binarise.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/mincmorph/test-clamp.mnc"  "${CMAKE_CURRENT_BINARY_DIR}/mincmorph/test-clamp.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/mincmorph/test-pad.mnc"  "${CMAKE_CURRENT_BINARY_DIR}/mincmorph/test-pad.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/mincmorph/test-convolve.mnc"  "${CMAKE_CURRENT_BINARY_DIR}/mincmorph/test-convolve.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/mincmorph/test-lowpass.mnc"  "${CMAKE_CURRENT_BINARY_DIR}/mincmorph/test-lowpass.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/mincmorph/test-open.mnc"  "${CMAKE_CURRENT_BINARY_DIR}/mincmorph/test-open.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/mincmorph/test-close.mnc"  "${CMAKE_CURRENT_BINARY_DIR}/mincmorph/test-close.mnc" COPYONLY)

IF(MINC_TEST_ENVIRONMENT)
    SET(MINCTOOLS_TEST_ENVIRONMENT ${MINC_TEST_ENVIRONMENT})
ELSE()
    SET(MINCTOOLS_TEST_ENVIRONMENT "")
ENDIF()

LIST(APPEND MINCTOOLS_TEST_ENVIRONMENT "MINCAVERAGE_BIN=${mincaverage_bin}")
LIST(APPEND MINCTOOLS_TEST_ENVIRONMENT "MINCSTATS_BIN=${mincstats_bin}")
LIST(APPEND MINCTOOLS_TEST_ENVIRONMENT "MINCINFO_BIN=${mincinfo_bin}")
LIST(APPEND MINCTOOLS_TEST_ENVIRONMENT "MINCRESAMPLE_BIN=${mincresample_bin}")
LIST(APPEND MINCTOOLS_TEST_ENVIRONMENT "MINCCALC_BIN=${minccalc_bin}")
LIST(APPEND MINCTOOLS_TEST_ENVIRONMENT "MINCRESHAPE_BIN=${mincreshape_bin}")
LIST(APPEND MINCTOOLS_TEST_ENVIRONMENT "MINCMORPH_BIN=${mincmorph_bin}")
LIST(APPEND MINCTOOLS_TEST_ENVIRONMENT "MINCCMP_BIN=${minccmp_bin}")

SET(MINCTOOLS_TEST_ENVIRONMENT1 ${MINCTOOLS_TEST_ENVIRONMENT})
SET(MINCTOOLS_TEST_ENVIRONMENT2 ${MINCTOOLS_TEST_ENVIRONMENT})

LIST(APPEND MINCTOOLS_TEST_ENVIRONMENT1 "MINC_FORCE_V2=0")
LIST(APPEND MINCTOOLS_TEST_ENVIRONMENT2 "MINC_FORCE_V2=1")

MACRO(ADD_TEST12 name cmd)
  # test in minc1 mode
  ADD_TEST( ${name}-1
      ${cmd} ${ARGV2} ${ARGV3} ${ARGV4} ${ARGV5} ${ARGV6} ${ARGV7} ${ARGV8} ${ARGV9} ${ARGV10} ${ARGV11}
        ${ARGV12} ${ARGV13} ${ARGV14} ${ARGV15} ${ARGV16} ${ARGV17} ${ARGV18} ${ARGV19} ${ARGV20} ${ARGV21} ${ARGV22}
        ${ARGV23} ${ARGV24} ${ARGV25} ${ARGV26}
   )
  set_tests_properties( ${name}-1 PROPERTIES ENVIRONMENT "${MINCTOOLS_TEST_ENVIRONMENT1}")
  
  # test in minc2 mode
  ADD_TEST( ${name}-2
      ${cmd} ${ARGV2} ${ARGV3} ${ARGV4} ${ARGV5} ${ARGV6} ${ARGV7} ${ARGV8} ${ARGV9} ${ARGV10} ${ARGV11}
        ${ARGV12} ${ARGV13} ${ARGV14} ${ARGV15} ${ARGV16} ${ARGV17} ${ARGV18} ${ARGV19} ${ARGV20} ${ARGV21} ${ARGV22}
        ${ARGV23} ${ARGV24} ${ARGV25} ${ARGV26}
   )
  set_tests_properties( ${name}-2 PROPERTIES ENVIRONMENT "${MINCTOOLS_TEST_ENVIRONMENT2}")
ENDMACRO(ADD_TEST12)


ADD_TEST12(mincinfo-test ${CMAKE_CURRENT_SOURCE_DIR}/mincinfo-test.pl)

ADD_TEST12(mincstats-test ${CMAKE_CURRENT_SOURCE_DIR}/mincstats-test.pl)

# Copy files used by mincaverage-test
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/mincaverage-in0.mnc" "${CMAKE_CURRENT_BINARY_DIR}/mincaverage-in0.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/mincaverage-in1.mnc" "${CMAKE_CURRENT_BINARY_DIR}/mincaverage-in1.mnc" COPYONLY)

ADD_TEST12(mincaverage-test ${CMAKE_CURRENT_SOURCE_DIR}/mincaverage-test.sh)

ADD_TEST12(minccalc-test ${CMAKE_CURRENT_SOURCE_DIR}/minccalc-test.sh)

ADD_TEST12(minccalc-labels-test ${CMAKE_CURRENT_SOURCE_DIR}/minccalc-labels-test.sh)

ADD_TEST12(mincresample-test ${CMAKE_CURRENT_SOURCE_DIR}/mincresample-test.sh)

# Get path to mincaverage binary.

ADD_TEST12(mincreshape-test ${CMAKE_CURRENT_SOURCE_DIR}/mincreshape-test.pl)

ADD_TEST12(mincmorph-test ${CMAKE_CURRENT_SOURCE_DIR}/mincmorph-test.sh)

ADD_TEST12(minccmp-test ${CMAKE_CURRENT_SOURCE_DIR}/minccmp-test.pl)
#TODO: add more tests?
