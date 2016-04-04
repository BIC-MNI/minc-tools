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

ADD_TEST(mincinfo-test ${CMAKE_CURRENT_SOURCE_DIR}/mincinfo-test.pl)
SET_TESTS_PROPERTIES(mincinfo-test
  PROPERTIES ENVIRONMENT "MINCINFO_BIN=${mincinfo_bin}")

ADD_TEST(mincstats-test ${CMAKE_CURRENT_SOURCE_DIR}/mincstats-test.pl)
SET_TESTS_PROPERTIES(mincstats-test
  PROPERTIES ENVIRONMENT "MINCSTATS_BIN=${mincstats_bin}")

# Copy files used by mincaverage-test
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/mincaverage-in0.mnc" "${CMAKE_CURRENT_BINARY_DIR}/mincaverage-in0.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/mincaverage-in1.mnc" "${CMAKE_CURRENT_BINARY_DIR}/mincaverage-in1.mnc" COPYONLY)

# Add the test itself.
ADD_TEST(mincaverage-test ${CMAKE_CURRENT_SOURCE_DIR}/mincaverage-test.sh)

# Set its environment variables.
SET_TESTS_PROPERTIES(mincaverage-test
    PROPERTIES ENVIRONMENT "MINCAVERAGE_BIN=${mincaverage_bin};MINCSTATS_BIN=${mincstats_bin};MINCINFO_BIN=${mincinfo_bin}")

# Get path to the binary.
GET_PROPERTY(minccalc_bin TARGET minccalc PROPERTY LOCATION)

# Add the test.
ADD_TEST(minccalc-test ${CMAKE_CURRENT_SOURCE_DIR}/minccalc-test.sh)

# Set the test's environment.
SET_TESTS_PROPERTIES(minccalc-test
    PROPERTIES ENVIRONMENT "MINCCALC_BIN=${minccalc_bin};MINCSTATS_BIN=${mincstats_bin}")

# Add the test itself.
ADD_TEST(mincresample-test ${CMAKE_CURRENT_SOURCE_DIR}/mincresample-test.sh)

# Set its environment variables.
SET_TESTS_PROPERTIES(mincresample-test
    PROPERTIES ENVIRONMENT "MINCRESAMPLE_BIN=${mincresample_bin};MINCSTATS_BIN=${mincstats_bin}")

# Get path to mincaverage binary.
GET_PROPERTY(mincreshape_bin TARGET mincreshape PROPERTY LOCATION)

ADD_TEST(mincreshape-test ${CMAKE_CURRENT_SOURCE_DIR}/mincreshape-test.pl)

SET_TESTS_PROPERTIES(mincreshape-test
  PROPERTIES ENVIRONMENT "MINCRESHAPE_BIN=${mincreshape_bin};MINCSTATS_BIN=${mincstats_bin};MINCINFO_BIN=${mincinfo_bin};MINCEXTRACT_BIN=${mincextract_bin}")

#TODO: add more tests?
