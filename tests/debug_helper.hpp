#ifndef DEBUG_HELPER_H
#define DEBUG_HELPER_H

/* DEBUG_HELPER
 *
 * this file is used to debug the functions with printf
 * it is not used in the final program
 *
 */

#include <stdio.h>

#define LOG(msg)                                                               \
  printf("in %s() at line %d: ", __func__, __LINE__);                          \
  printf(msg);                                                                 \
  printf("\n");

#define CRASH(msg)                                                             \
  LOG(msg);                                                                    \
  exit(1);

#endif /* DEBUG_HELPER_H */
