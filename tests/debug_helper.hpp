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

#define LOG_TILES(map)                                                         \
  for (int i = 0; i < map.size.y; i++) {                                       \
    for (int j = 0; j < map.size.x; j++) {                                     \
      if (map.tiles[i][j] == 0)                                                \
        printf(" ");                                                           \
      else if (map.tiles[i][j] == 1)                                           \
        printf("X");                                                           \
      else if (map.tiles[i][j] == 2)                                           \
        printf(".");                                                           \
    }                                                                          \
    printf("\n");                                                              \
  }

#endif /* DEBUG_HELPER_H */
