#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

typedef struct {
  uint8_t x, y, z;
  uint8_t buttons[10];
} report;

report rep;

int main(int argc, char **argv) {
  struct timeval t;
  gettimeofday(&t, NULL);
  srand(t.tv_sec);
  rep.x = atoi(argv[1]);
  rep.y = atoi(argv[2]);
  rep.z = atoi(argv[3]);
  for (int i=0; i<sizeof(rep.buttons); i++) {
    rep.buttons[i] = rand();
  }
  int fd = open("/dev/hidg0", O_RDWR);
  write(fd, &rep, sizeof(rep));
  close(fd);
}
