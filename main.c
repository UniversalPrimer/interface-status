#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // uint8_t, uint16_t, etc.
#include <string.h> // strlen
#include <sys/ioctl.h>
#include <net/if.h>
#include "interfaces.h"


int main() {
  struct interface** ifs;
  int i;

  ifs = get_interfaces();

  i = 0;
  for(;;) {
    if(ifs[i] == NULL) {
      return 0;
    }

    print_interface_info(ifs[i]);

    i++;
  }


  return 0;
}

