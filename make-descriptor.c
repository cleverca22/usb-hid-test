#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "hid.h"

uint8_t *make_descriptor(int bits_per_axis, int button_max, int *size) {
  uint8_t hid_descriptor[] = {
    HID_USAGE_PAGE(HID_USAGE_PAGE_DESKTOP),
    HID_USAGE(HID_USAGE_DESKTOP_JOYSTICK),
    HID_COLLECTION(HID_COLLECTION_APPLICATION),
      HID_COLLECTION(HID_COLLECTION_LOGICAL),
//        HID_REPORT_ID(1),
        HID_REPORT_SIZE(bits_per_axis),
        HID_LOGICAL_MIN(0),
        HID_LOGICAL_MAX(2047),
        HID_PHYSICAL_MIN(0),
        HID_PHYSICAL_MAX(2047),
        HID_USAGE(HID_USAGE_DESKTOP_X),
        HID_USAGE(HID_USAGE_DESKTOP_Y),
        HID_USAGE(HID_USAGE_DESKTOP_Z),
        HID_REPORT_COUNT(3),
        HID_INPUT(HID_VARIABLE),
        HID_PUSH,
          HID_REPORT_COUNT(button_max),
          HID_REPORT_SIZE(8),
          HID_LOGICAL_MAX(255),  // each button can return a value between 0 and 1
          HID_PHYSICAL_MAX(255),
          HID_USAGE_PAGE(HID_USAGE_PAGE_BUTTON),
          HID_USAGE_MIN(1),   // this is decribing buttons 1 thru button_max
          HID_USAGE_MAX(button_max),
          HID_INPUT(HID_VARIABLE),
        HID_POP,
      HID_COLLECTION_END,
    HID_COLLECTION_END,
  };
  uint8_t *buffer = malloc(sizeof(hid_descriptor));
  memcpy(buffer, hid_descriptor, sizeof(hid_descriptor));
  *size = sizeof(hid_descriptor);
  return buffer;
}

uint8_t hid_descriptor2[] = {
  5,1,9,4,161,1,133,1,5,1,9,48,9,49,9,50,9,51,9,52,9,53,9,54,9,55,21,0,38,0,4,149,8,117,11,129,2,5,1,9,57,9,57,21,1,37,8,53,0,70,59,1,149,2,117,4,129,2,5,9,25,1,41,64,21,0,37,1,149,64,117,1,129,2,192
};

int main(int argc, char **argv) {
  int size;
  uint8_t *buffer = make_descriptor(8, 1, &size);
  printf("%d\n", size);
  int fd = open("out.bin", O_RDWR | O_CREAT | O_TRUNC, 0666);
  int ret = write(fd, buffer, size);
  assert(ret == size);
  close(fd);
  for (int i=0; i<size; i++) {
    printf("0x%x,", buffer[i]);
  }
  puts("");
  return 0;
}
