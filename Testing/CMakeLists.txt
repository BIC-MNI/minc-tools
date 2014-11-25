# Fix tools tests to avoid modifications to source directory (bert).
# 

# Get path to mincstats binary.
GET_PROPERTY(mincstats_bin TARGET mincstats PROPERTY LOCATION)

# Get path to mincaverage binary.
GET_PROPERTY(mincaverage_bin TARGET mincaverage PROPERTY LOCATION)

# Copy files used by mincaverage-test
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/mincaverage-in0.mnc" "${CMAKE_CURRENT_BINARY_DIR}/mincaverage-in0.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/mincaverage-in1.mnc" "${CMAKE_CURRENT_BINARY_DIR}/mincaverage-in1.mnc" COPYONLY)

# Add the test itself.
ADD_TEST(mincaverage-test ${CMAKE_CURRENT_SOURCE_DIR}/mincaverage-test.sh)

# Set its environment variables.
SET_TESTS_PROPERTIES(mincaverage-test
    PROPERTIES ENVIRONMENT "MINCAVERAGE_BIN=${mincaverage_bin};MINCSTATS_BIN=${mincstats_bin}")

# Copy files used by minccalc-test
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/test-zero.mnc" "${CMAKE_CURRENT_BINARY_DIR}/test-zero.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/test-one.mnc" "${CMAKE_CURRENT_BINARY_DIR}/test-one.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/test-two.mnc" "${CMAKE_CURRENT_BINARY_DIR}/test-two.mnc" COPYONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/test-rnd.mnc" "${CMAKE_CURRENT_BINARY_DIR}/test-rnd.mnc" COPYONLY)

# Get path to the binary.
GET_PROPERTY(minccalc_bin TARGET minccalc PROPERTY LOCATION)

# Add the test.
ADD_TEST(minccalc-test ${CMAKE_CURRENT_SOURCE_DIR}/minccalc-test.sh)

# Set the test's environment.
SET_TESTS_PROPERTIES(minccalc-test
    PROPERTIES ENVIRONMENT "MINCCALC_BIN=${minccalc_bin};MINCSTATS_BIN=${mincstats_bin}")

#TODO: add more tests?
