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

typedef struct {
  int report_size;
  int report_count;
  int usage_page;
  int usage[256];
  int next_usage;
  int usage_min;
} hid_state;

hid_state stack[16];
int sp = 0;
int bits = 0;

void addbyte(uint8_t **p, uint8_t byte) {
  *((*p)++) = byte;
}

void addshort(uint8_t **p, uint16_t d) {
  *((*p)++) = d & 0xff;
  *((*p)++) = d >> 8;
}

void hid_usage_page(uint8_t **p, uint8_t page) {
  addbyte(p, 0x5);
  addbyte(p, page);
  stack[sp].usage_page = page;
}

void hid_usage(uint8_t **p, uint8_t usage) {
  addbyte(p, 0x9);
  addbyte(p, usage);
  stack[sp].usage[stack[sp].next_usage++] = usage;
}

void hid_collection(uint8_t **p, uint8_t collection) {
  addbyte(p, 0xa1);
  addbyte(p, collection);
}

void hid_collection_end(uint8_t **p) {
  addbyte(p, 0xc0);
}

int get_bytes(uint16_t d) {
  if ((d >= 0) && (d <= UINT8_MAX)) return 1;
  else if ((d >= 0) && (d <= UINT16_MAX)) return 2;
}

void add_variable(uint8_t **p, uint16_t d, int bytes) {
  switch (bytes) {
  case 1:
    addbyte(p, d);
    break;
  case 2:
    addshort(p, d);
    break;
  }
}

void hid_report_size(uint8_t **p, uint8_t size) {
  addbyte(p, 0x75);
  addbyte(p, size);
  stack[sp].report_size = size;
}

void hid_report_count(uint8_t **p, uint16_t count) {
  int bytes = get_bytes(count);
  addbyte(p, 0x94 | bytes);
  add_variable(p, count, bytes);
  stack[sp].report_count = count;
}

void hid_logical_min(uint8_t **p, uint8_t min) {
  addbyte(p, 0x15);
  addbyte(p, min);
}

void hid_logical_max(uint8_t **p, uint16_t max) {
  int bytes = get_bytes(max);

  addbyte(p, 0x24 | bytes);
  add_variable(p, max, bytes);
}

void hid_physical_min(uint8_t **p, uint16_t min) {
  int bytes = get_bytes(min);

  addbyte(p, 0x34 | bytes);
  add_variable(p, min, bytes);
}

void hid_physical_max(uint8_t **p, uint16_t max) {
  int bytes = get_bytes(max);

  addbyte(p, 0x44 | bytes);
  add_variable(p, max, bytes);
}

void hid_input(uint8_t **p, uint8_t input) {
  addbyte(p, 0x81);
  addbyte(p, input);

  printf("INPUT with %d * %d bits\n", stack[sp].report_count, stack[sp].report_size);
  for (int i=stack[sp].next_usage - stack[sp].report_count; i<stack[sp].next_usage; i++) {
    printf("usage#%d == 0x%x%04x bits %d + %d\n", i, stack[sp].usage_page, stack[sp].usage[i], bits, stack[sp].report_size);
    bits += stack[sp].report_size;
  }
  stack[sp].next_usage = 0;
}

void hid_push(uint8_t **p) {
  addbyte(p, 0xa4);
}

void hid_pop(uint8_t **p) {
  addbyte(p, 0xb4);
}

void hid_usage_min(uint8_t **p, uint8_t min) {
  addbyte(p, 0x19);
  addbyte(p, min);
  stack[sp].usage_min = min;
}

void hid_usage_max(uint8_t **p, uint8_t max) {
  addbyte(p, 0x29);
  addbyte(p, max);
  for (int n=stack[sp].usage_min; n<= max; n++) {
    stack[sp].usage[stack[sp].next_usage++] = n;
  }
}

uint8_t *make_descriptor(int bits_per_axis, int button_max, int *size) {
  uint8_t *buffer = malloc(4096);
  uint8_t *p = buffer;

  hid_usage_page(&p, HID_USAGE_PAGE_DESKTOP);
  hid_usage(&p, HID_USAGE_DESKTOP_JOYSTICK);
  hid_collection(&p, HID_COLLECTION_APPLICATION);
    hid_collection(&p, HID_COLLECTION_LOGICAL);
      hid_report_size(&p, bits_per_axis);
      hid_logical_min(&p, 0);
      hid_logical_max(&p, 2047);
      hid_physical_min(&p, 0);
      hid_physical_max(&p, 2047);
      hid_usage(&p, HID_USAGE_DESKTOP_X);
      hid_usage(&p, HID_USAGE_DESKTOP_Y);
      hid_usage(&p, HID_USAGE_DESKTOP_Z);
      hid_report_count(&p, 3);
      hid_input(&p, HID_VARIABLE);
      hid_push(&p);
        hid_report_count(&p, button_max);
        hid_report_size(&p, 1);
        hid_logical_max(&p, 1);
        hid_physical_max(&p, 1);
        hid_usage_page(&p, HID_USAGE_PAGE_BUTTON);
        hid_usage_min(&p, 1);
        hid_usage_max(&p, button_max);
        hid_input(&p, HID_VARIABLE);
      hid_pop(&p);
    hid_collection_end(&p);
  hid_collection_end(&p);

  *size = p - buffer;
  return buffer;

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
  buffer = malloc(sizeof(hid_descriptor));
  memcpy(buffer, hid_descriptor, sizeof(hid_descriptor));
  *size = sizeof(hid_descriptor);
  return buffer;
}

uint8_t hid_descriptor2[] = {
  5,1,9,4,161,1,133,1,5,1,9,48,9,49,9,50,9,51,9,52,9,53,9,54,9,55,21,0,38,0,4,149,8,117,11,129,2,5,1,9,57,9,57,21,1,37,8,53,0,70,59,1,149,2,117,4,129,2,5,9,25,1,41,64,21,0,37,1,149,64,117,1,129,2,192
};

int main(int argc, char **argv) {
  int size;
  uint8_t *buffer = make_descriptor(8, 16, &size);
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
