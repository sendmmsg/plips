#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
int convert_to_integer(char *str, int64_t *dst) {
  char *p = str;
  errno = 0;
  *dst = 0;
  int64_t val = strtoul(str, &p, 10);
  if (errno != 0) {
    // conversion failed (EINVAL, ERANGE)
    goto hex;
  }
  if (str == p) {
    // conversion failed (no characters consumed)
    goto hex;
  }
  if (*p != 0) {
    // conversion failed (trailing data)
    goto hex;
  }
  *dst = val;
  return 1;
hex:
  errno = 0;
  val = strtoul(str, &p, 16);
  if (errno != 0) {
    // conversion failed (EINVAL, ERANGE)
    return 0;
  }
  if (str == p) {
    // conversion failed (no characters consumed)
    return 0;
  }
  if (*p != 0) {
    // conversion failed (trailing data)
    return 0;
  }
  *dst = val;
  return 1;
}

int convert_to_double(char *str, double *dst) {
  char *p = str;
  errno = 0;
  *dst = 0;
  double val = strtod(str, &p);
  if (errno != 0) {
    // conversion failed (EINVAL, ERANGE)
    return 0;
  }
  if (str == p) {
    // conversion failed (no characters consumed)
    return 0;
  }
  if (*p != 0) {
    // conversion failed (trailing data)
    return 0;
  }
  *dst = val;
  return 1;
}

int main() {
  int64_t data;
  double dd;
  char *str[] = {"3213132",     "34234234213132", "32131.2342", "adsf234234",
                 "3213132sadf", "0x434242",       "0xff.ff"};

  for (int i = 0; i < 7; i++) {
    if (convert_to_integer(str[i], &data)) {
      printf("c2i - successful: %ld\n", data);
    } else {
      printf("c2i -  %s failed\n", str[i]);
    }
    if (convert_to_double(str[i], &dd)) {
      printf("c2d - successful: %f\n", dd);
    } else {
      printf("c2d -  %s failed\n", str[i]);
    }
  }
}
