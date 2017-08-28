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

# Copy files used by minccalc-test and other tests.
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/test-zero.mnc" "${CMAKE_CURRENT_BINARY_DIR}/test-zero.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/test-one.mnc" "${CMAKE_CURRENT_BINARY_DIR}/test-one.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/test-two.mnc" "${CMAKE_CURRENT_BINARY_DIR}/test-two.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/test-rnd.mnc" "${CMAKE_CURRENT_BINARY_DIR}/test-rnd.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/test-4d.mnc" "${CMAKE_CURRENT_BINARY_DIR}/test-4d.mnc" COPYONLY)


IF(MINC_TEST_ENVIRONMENT)
    SET(MINCTOOLS_TEST_ENVIRONMENT1 ${MINC_TEST_ENVIRONMENT})
    SET(MINCTOOLS_TEST_ENVIRONMENT2 ${MINC_TEST_ENVIRONMENT})
ELSE()
    SET(MINCTOOLS_TEST_ENVIRONMENT1 "")
    SET(MINCTOOLS_TEST_ENVIRONMENT2 "")
ENDIF()


LIST(APPEND MINCTOOLS_TEST_ENVIRONMENT1 "MINC_FORCE_V2=0")
LIST(APPEND MINCTOOLS_TEST_ENVIRONMENT2 "MINC_FORCE_V2=0")


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
  set_tests_properties( ${name}-2 PROPERTIES ENVIRONMENT "${MINCTOOLS_TEST_ENVIRONMENT1}")
   
   
ENDMACRO(ADD_TEST12)



ADD_TEST12(mincinfo-test ${CMAKE_CURRENT_SOURCE_DIR}/mincinfo-test.pl)
SET_TESTS_PROPERTIES(mincinfo-test-1
  PROPERTIES ENVIRONMENT "MINCINFO_BIN=${mincinfo_bin}")
SET_TESTS_PROPERTIES(mincinfo-test-2
  PROPERTIES ENVIRONMENT "MINCINFO_BIN=${mincinfo_bin}")

ADD_TEST12(mincstats-test ${CMAKE_CURRENT_SOURCE_DIR}/mincstats-test.pl)
SET_TESTS_PROPERTIES(mincstats-test-1
  PROPERTIES ENVIRONMENT "MINCSTATS_BIN=${mincstats_bin}")
SET_TESTS_PROPERTIES(mincstats-test-2
  PROPERTIES ENVIRONMENT "MINCSTATS_BIN=${mincstats_bin}")

# Copy files used by mincaverage-test
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/mincaverage-in0.mnc" "${CMAKE_CURRENT_BINARY_DIR}/mincaverage-in0.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/mincaverage-in1.mnc" "${CMAKE_CURRENT_BINARY_DIR}/mincaverage-in1.mnc" COPYONLY)

# Add the test itself.
ADD_TEST12(mincaverage-test ${CMAKE_CURRENT_SOURCE_DIR}/mincaverage-test.sh)

# Set its environment variables.
SET_TESTS_PROPERTIES(mincaverage-test-1
    PROPERTIES ENVIRONMENT "MINCAVERAGE_BIN=${mincaverage_bin};MINCSTATS_BIN=${mincstats_bin};MINCINFO_BIN=${mincinfo_bin}")
SET_TESTS_PROPERTIES(mincaverage-test-2
    PROPERTIES ENVIRONMENT "MINCAVERAGE_BIN=${mincaverage_bin};MINCSTATS_BIN=${mincstats_bin};MINCINFO_BIN=${mincinfo_bin}")

# Get path to the binary.
GET_PROPERTY(minccalc_bin TARGET minccalc PROPERTY LOCATION)

# Add the test.
ADD_TEST12(minccalc-test ${CMAKE_CURRENT_SOURCE_DIR}/minccalc-test.sh)

# Set the test's environment.
SET_TESTS_PROPERTIES(minccalc-test-1
    PROPERTIES ENVIRONMENT "MINCCALC_BIN=${minccalc_bin};MINCSTATS_BIN=${mincstats_bin}")
SET_TESTS_PROPERTIES(minccalc-test-2
    PROPERTIES ENVIRONMENT "MINCCALC_BIN=${minccalc_bin};MINCSTATS_BIN=${mincstats_bin}")

# Add the test itself.
ADD_TEST12(mincresample-test ${CMAKE_CURRENT_SOURCE_DIR}/mincresample-test.sh)

# Set its environment variables.
SET_TESTS_PROPERTIES(mincresample-test-1
    PROPERTIES ENVIRONMENT "MINCRESAMPLE_BIN=${mincresample_bin};MINCSTATS_BIN=${mincstats_bin}")
SET_TESTS_PROPERTIES(mincresample-test-2
    PROPERTIES ENVIRONMENT "MINCRESAMPLE_BIN=${mincresample_bin};MINCSTATS_BIN=${mincstats_bin}")

# Get path to mincaverage binary.
GET_PROPERTY(mincreshape_bin TARGET mincreshape PROPERTY LOCATION)

ADD_TEST12(mincreshape-test ${CMAKE_CURRENT_SOURCE_DIR}/mincreshape-test.pl)

SET_TESTS_PROPERTIES(mincreshape-test-1
  PROPERTIES ENVIRONMENT "MINCRESHAPE_BIN=${mincreshape_bin};MINCSTATS_BIN=${mincstats_bin};MINCINFO_BIN=${mincinfo_bin};MINCEXTRACT_BIN=${mincextract_bin}")
SET_TESTS_PROPERTIES(mincreshape-test-2
  PROPERTIES ENVIRONMENT "MINCRESHAPE_BIN=${mincreshape_bin};MINCSTATS_BIN=${mincstats_bin};MINCINFO_BIN=${mincinfo_bin};MINCEXTRACT_BIN=${mincextract_bin}")

#TODO: add more tests?
