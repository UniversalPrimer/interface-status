#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // uint8_t, uint16_t, etc.
#include <string.h> // strlen
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include "interfaces.h"
#include "error.h"
#include "wrappers.h"


char** get_device_names() {

  FILE* f;
  char b;
  char** dev_names;
  char* dev_name;
  int dev_count;
  int char_count;
  int i;

  dev_names = (char**) wmalloc(sizeof(char*) * MAX_NUM_DEVICES);

  f = fopen(PROC_DEVICE_PATH, "r");

  if(f == NULL) {
    dies("Couldn't open proc device: %s", PROC_DEVICE_PATH);
  }

  // discard two first lines (not relevant
  for(i=0; i < 2; i++) {
    do {
      if(wfread(&b, 1, 1, f) == 0) {
        return NULL;
      }
    } while(b != '\n');
  }


  // read all device names
  dev_count = 0;
  while(dev_count < MAX_NUM_DEVICES) {
    dev_name = (char*) wmalloc(IFNAMSIZ); 
    memset(dev_name, 0, IFNAMSIZ);

    char_count = 0;
    // read until first colon and save data (this is the name part)
    for(;;) {
      if(wfread(&b, 1, 1, f) == 0) {
        dev_names[dev_count] = NULL;
        fclose(f);
        return dev_names;
      }
      if(b == ' ') {
        continue;
      }
      if(b == ':') {
        break;
      }
      dev_name[char_count++] = b;
      if(char_count >= IFNAMSIZ) {
        die("device name of more than max length detected");
      }
    } 

    dev_names[dev_count] = dev_name;

    // read until next line and discard data
    do {
      if(wfread(&b, 1, 1, f) == 0) {
        dev_names[dev_count + 1] = NULL;
        fclose(f);
        return dev_names;
      }
    } while(b != '\n');

    dev_count++;

  }
  return NULL;
  
}

void print_interface_info(struct interface* iface) {
  char* ip;

  ip = wmalloc(INET_ADDRSTRLEN); // enough room for an ipv4 address in string form

  printf("Device name: %s\n", iface->name); // XXX can we be sure the name is NULL-terminated?
  printf("  Device is: ");
  if(iface->is_up) {
    printf("Up\n");
    if(iface->ip_addr) {
      ip = inet_ntoa(iface->ip_addr->sin_addr);
      printf("  IP Address: %s\n", ip);
    }
  } else {
    printf("Down\n");
  }

  

}

void free_device_names(char** dev_names) {
  int i;

  if(dev_names == NULL) {
    return;
  }
  for(i=0; i < MAX_NUM_DEVICES; i++) {
    if(dev_names[i] == NULL) {
      return;
    }
    free(dev_names[i]);
  }
  free(dev_names);
}

// Get the IP addreses 
struct interface** get_interfaces() {

  struct interface** ifs; // the interface list
  struct interface* iface;
  int if_count;
  char** dev_names;
  int i;
  int sock;
  int err;
  int has_ip;
  struct ifreq ifr;

  dev_names = get_device_names();

  ifs = wmalloc(sizeof(struct interface*) * MAX_NUM_DEVICES + 1); // allocate and make room for terminating NULL pointer

  if_count = 0;

  for(i=0; i < MAX_NUM_DEVICES; i++) {

    if(dev_names[i] == NULL) {
      goto end;
    }

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    strncpy(ifr.ifr_name, dev_names[i], IFNAMSIZ); 
    ifr.ifr_addr.sa_family = AF_INET;

    err = ioctl(sock, SIOCGIFFLAGS, &ifr);
    if(err < 0) {
      dies("Error getting device info");
    }
    
    // if the device is not a loopback device
    if(!(ifr.ifr_flags & IFF_LOOPBACK)) {
      
      iface = wmalloc(sizeof(struct interface));
      
      iface->name = wmalloc(IFNAMSIZ);
      strncpy(iface->name, dev_names[i], IFNAMSIZ); 

      // is the interface up?
      if(ifr.ifr_flags & IFF_UP) {

        iface->is_up = 1;

        has_ip = ioctl(sock, SIOCGIFADDR, &ifr);
        if(has_ip < 0) {
          if(!(errno == EADDRNOTAVAIL || errno == EAFNOSUPPORT)) {
            dies("error finding out if device has an IP");
          }
          iface->ip_addr = NULL;
        } else {
          iface->ip_addr = (struct sockaddr_in *) wmalloc(sizeof(struct sockaddr_in));
          memcpy(iface->ip_addr, &ifr.ifr_addr, sizeof(struct sockaddr_in));
          //          inet_aton("10.0.0.42", (struct in_addr*) iface->ip_addr);
          //iface->ip_addr = (struct sockaddr_in *) &ifr.ifr_addr;
        }
                
      } else { // interface is not up
        iface->is_up = 0;
      }
      ifs[if_count++] = iface;
    }

  }

 end:

  //  free_device_names(dev_names);

  ifs[if_count] = NULL;

  return ifs;

}
