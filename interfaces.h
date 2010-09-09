
#define PROC_DEVICE_PATH "/proc/net/dev"
#define MAX_NUM_DEVICES (64) // this is arbitrary


struct interface {
  char* name; // the device name
  struct sockaddr_in* ip_addr; // the ip address (if any)
  char is_up; // 1 if the interface is up, 0 if down
};

char** get_device_names();
struct interface** get_interfaces();
void print_interface_info(struct interface* iface);
void free_device_names(char** dev_names);
