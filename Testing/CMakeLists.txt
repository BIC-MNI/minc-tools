# Fix tools tests to avoid modifications to source directory (bert).
# 
# Get path to mincstats binary.
GET_PROPERTY(mincstats_bin TARGET mincstats PROPERTY LOCATION)

# Get path to mincinfo binary.
GET_PROPERTY(mincinfo_bin TARGET mincinfo PROPERTY LOCATION)

# Get path to mincextract binary
GET_PROPERTY(mincextract_bin TARGET mincextract PROPERTY LOCATION)

# Get path to mincaverage binary.
GET_PROPERTY(mincaverage_bin TARGET mincaverage PROPERTY LOCATION)

# Get path to mincaverage binary.
GET_PROPERTY(mincresample_bin TARGET mincresample PROPERTY LOCATION)

# Get path to minccalc binary.
GET_PROPERTY(minccalc_bin TARGET minccalc PROPERTY LOCATION)

GET_PROPERTY(mincreshape_bin TARGET mincreshape PROPERTY LOCATION)

# Copy files used by minccalc-test and other tests.
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/test-zero.mnc" "${CMAKE_CURRENT_BINARY_DIR}/test-zero.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/test-one.mnc"  "${CMAKE_CURRENT_BINARY_DIR}/test-one.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/test-two.mnc"  "${CMAKE_CURRENT_BINARY_DIR}/test-two.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/test-rnd.mnc"  "${CMAKE_CURRENT_BINARY_DIR}/test-rnd.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/test-4d.mnc"   "${CMAKE_CURRENT_BINARY_DIR}/test-4d.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/test-zero-byte.mnc" "${CMAKE_CURRENT_BINARY_DIR}/test-zero-byte.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/test-labels.mnc" "${CMAKE_CURRENT_BINARY_DIR}/test-labels.mnc" COPYONLY)

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

#TODO: add more tests?
