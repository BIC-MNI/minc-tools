#include <stdio.h>
#include "minc_endian.h"

/**
 * Returns either MINC_BIG_ENDIAN on a big-endian CPU, or MINC_LITTLE_ENDIAN
 * on a little-endian CPU.
 */
static int minc_cpu_endian()
{
  union etest {
    short s;
    char b[sizeof(short)];
  } x;

  x.s = 1;

  if (x.b[0] == 1 && x.b[sizeof(short) - 1] == 0) {
    return MINC_LITTLE_ENDIAN;
  }
  if (x.b[0] == 0 && x.b[sizeof(short) - 1] == 1) {
    return MINC_BIG_ENDIAN;
  }

  fprintf(stderr, "CPU is neither-endian??\n");
  return MINC_NATIVE_ENDIAN;
}

/**
 * Function to swap 8-byte values in place.
 */
static void minc_swap_8bytes(char *p_data, size_t n_bytes)
{
  char *p_last;
  char tmp;

  for (p_last = p_data + n_bytes; p_data < p_last; p_data += 8) {
    tmp = p_data[0];
    p_data[0] = p_data[7];
    p_data[7] = tmp;
    
    tmp = p_data[5];
    p_data[5] = p_data[2];
    p_data[2] = tmp;

    tmp = p_data[1];
    p_data[1] = p_data[6];
    p_data[6] = tmp;

    tmp = p_data[3];
    p_data[3] = p_data[4];
    p_data[4] = tmp;
  }
}

/**
 * Function to swap 4-byte values in place.
 */
static void minc_swap_4bytes(char *p_data, size_t n_bytes)
{
  char *p_last;
  char tmp;

  for (p_last = p_data + n_bytes; p_data < p_last; p_data += 4) {
    tmp = p_data[0];
    p_data[0] = p_data[3];
    p_data[3] = tmp;

    tmp = p_data[2];
    p_data[2] = p_data[1];
    p_data[1] = tmp;
  }
}

/**
 * Function to swap 2-byte values in place.
 */
static void minc_swap_2bytes(char *p_data, size_t n_bytes)
{
  char *p_last;
  char tmp;

  for (p_last = p_data + n_bytes; p_data < p_last; p_data += 2) {
    tmp = p_data[0];
    p_data[0] = p_data[1];
    p_data[1] = tmp;
  }
}

minc_swap_fn_t minc_get_swap_function(int desired_format, int type_size)
{
  minc_swap_fn_t swap_fn = NULL;

  if (desired_format != MINC_NATIVE_ENDIAN &&
      desired_format != minc_cpu_endian()) {
    switch (type_size) {
    case 8:
      swap_fn = minc_swap_8bytes;
      break;
    case 4:
      swap_fn = minc_swap_4bytes;
      break;
    case 2:
      swap_fn = minc_swap_2bytes;
      break;
    default:
      (void) fprintf(stderr, "I can't change the endianness of data of length %d\n", type_size);
      swap_fn = NULL;
      break;
    }
  }
  return swap_fn;
}

