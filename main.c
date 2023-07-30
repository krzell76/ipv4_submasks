#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ip4_libs.h"

#define NUM_MASKS 5

// IP "xxx.xxx.xxx.xxx" --> int32
unsigned int ip_to_int(char *ip) {
  unsigned int a, b, c, d;
  sscanf(ip, "%u.%u.%u.%u", &a, &b, &c, &d);
  return (a << 24) | (b << 16) | (c << 8) | d;
}

// mask len str --> char
char mask_to_char(char *mask) {
  int length;
  if (sscanf(mask, "%d", &length) == 1) {
    if (length < 0 || length > 32) {
      printf("Bad length.\n");
      return -1;
    }
  }
  return (char)length;
}

void ip_str_to_ints(const char *subnet_mask_str, unsigned int *decimal_ip,
                   char *decimal_mask) {
  char *subnet_mask_copy = strdup(subnet_mask_str);
  char *ip_str = strtok(subnet_mask_copy, "/");
  char *mask_str = strtok(NULL, "/");

  if (ip_str == NULL || mask_str == NULL) {
    printf("Bad format. Expected'address_ip/mask'.\n");
    free(subnet_mask_copy);
  }
  *decimal_ip = ip_to_int(ip_str);
  *decimal_mask = mask_to_char(mask_str);
  printf("address_ip: %u.%u.%u.%u/%u\n", (*decimal_ip >> 24) & 0xFF,
         (*decimal_ip >> 16) & 0xFF, (*decimal_ip >> 8) & 0xFF,
         *decimal_ip & 0xFF, *decimal_mask);
  free(subnet_mask_copy);
}

int remove_subnet_mask(const char *subnet_mask_str) {
  unsigned int decimal_ip;
  char decimal_mask;

  ip_str_to_ints(subnet_mask_str, &decimal_ip, &decimal_mask);
  printf("remove_subnet_mask %x %x\n", decimal_ip, decimal_mask);
  return del(decimal_ip, decimal_mask);
}

int store_subnet_mask(const char *subnet_mask_str) {
  unsigned int decimal_ip;
  char decimal_mask;

  ip_str_to_ints(subnet_mask_str, &decimal_ip, &decimal_mask);
  return add(decimal_ip, decimal_mask);
}

int main() {
  const char *subnet_masks[NUM_MASKS] = {
      //"30.255.255.0/24",  //0011 0000
      "52.0.0.0/8",  // 0011 0100
      "80.0.0.0/5",  // 0101 0
      "160.0.0.0/4", // 1010
      //"60.255.0.0/16",   //0110 0000
      //"18.0.0.0/8",      //0001 1000
      "12.255.255.28/30", // 0000 1100
      "57.255.0.0/16"     // 0011 1001
  };
  int ret_value;

  for (int i = 0; i < NUM_MASKS; i++) {
    if (store_subnet_mask(subnet_masks[i]) != 0) {
      printf("cannot add\n");
      break;   
    }

  }
  dump();

  ret_value = check(0xaa000000);
  if (ret_value > 0) // 1110 1111
    printf("found - as expected %d\n", ret_value);
  else
    printf("bug\n");

  if (remove_subnet_mask(subnet_masks[2]) != 0) {
      printf("cannot add\n");
    }
  dump();
  
  if (check(0xa0000000) == -1)
    printf("not found - as expected\n");
  else
    printf("bug\n");
  return 0;
}
