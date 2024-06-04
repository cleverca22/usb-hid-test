#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

enum {
  HID_USAGE_PAGE_DESKTOP = 0x1,
  HID_USAGE_PAGE_BUTTON  = 0x9,
};

enum {
  HID_COLLECTION_PHYSICAL = 0,
  HID_COLLECTION_APPLICATION,
  HID_COLLECTION_LOGICAL,
};

enum {
  HID_USAGE_DESKTOP_X = 0x30,
  HID_USAGE_DESKTOP_Y = 0x31,
  HID_USAGE_DESKTOP_Z = 0x32,
};

enum {
  HID_USAGE_DESKTOP_JOYSTICK = 4,
  HID_USAGE_DESKTOP_GAMEPAD = 5,
};

#define HID_USAGE_PAGE(x) 0x5, x
#define HID_USAGE(x) 0x9, x
#define HID_COLLECTION(x) 0xa1, x
#define HID_COLLECTION_END 0xc0
#define HID_REPORT_COUNT(x) 0x95, x
#define HID_REPORT_SIZE(x) 0x75, x
#define HID_LOGICAL_MIN(x) 0x15, x
#define HID_LOGICAL_MAX(x) 0x26, (x & 0xff), (x >> 8)
#define HID_PHYSICAL_MIN(x) 0x35, x
#define HID_PHYSICAL_MAX(x) 0x46, (x & 0xff), (x >> 8)
#define HID_INPUT(x) 0x81, x
#define HID_PUSH 0xa4
#define HID_POP 0xb4
#define HID_USAGE_MIN(x) 0x19, x
#define HID_USAGE_MAX(x) 0x29, x
#define HID_REPORT_ID(x) 0x85, x

#define HID_VARIABLE (1<<1)


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
  write(fd, buffer, size);
  close(fd);
  for (int i=0; i<size; i++) {
    printf("0x%x,", buffer[i]);
  }
  puts("");
  return 0;
}
