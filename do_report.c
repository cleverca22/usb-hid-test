#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

typedef struct {
//  uint8_t report_id;
  uint8_t x, y, z;
  uint8_t buttons[1];
} report;

report rep;

int main(int argc, char **argv) {
  struct timeval t;
  gettimeofday(&t, NULL);
  srand(t.tv_sec);
//  rep.report_id = 2;
  if (0) {
    rep.x = atoi(argv[1]);
    rep.y = atoi(argv[2]);
    rep.z = atoi(argv[3]);
  } else {
    //rep.x = rand();
    //rep.y = rand();
    //rep.z = rand();
  }
  for (int i=0; i<sizeof(rep.buttons); i++) {
    //rep.buttons[i] = rand();
  }
  int fd = open("/dev/hidg0", O_RDWR);
  for (int button = 0; button < 80; button++) {
    int byte = button/8;
    int bit = button % 8;
    rep.buttons[byte] = 1<<bit;
    write(fd, &rep, sizeof(rep));
    rep.buttons[byte] = 0;
    write(fd, &rep, sizeof(rep));
    sleep(1);
  }
  close(fd);
}
