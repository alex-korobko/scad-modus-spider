#include <arpa/inet.h>
#include <errno.h>
#include <hw/inout.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/neutrino.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <pthread.h>

#define WATCH_DOG_PORT 0x0043 //for Advantec 6145   WATCH_DOG_PORT 0x0043 for Advantec 6753   WATCH_DOG_PORT 0x0443 

void* WatchDogThread (void* arg)
{
ThreadCtl(_NTO_TCTL_IO, 0);
uintptr_t pport_map;
uint8_t data=0;
pport_map=mmap_device_io(1, (uint64_t) WATCH_DOG_PORT);

if (pport_map<=0) {
	printf("Error creating watch dog timer thread");
	} else {
	data=in8(pport_map);
	printf ("\nIn from port %d\n", data);
	};

return 0;
}


int main (int argc, const char *argv[])
{
pthread_t  w_dog_thread_pthid=NULL;

if (pthread_create(&w_dog_thread_pthid, NULL, &WatchDogThread, NULL) != EOK)
		{
		printf("fail to create watch dog thread [%s]", strerror(errno));
		return EXIT_FAILURE;
		};
	
	pthread_join(w_dog_thread_pthid, NULL);
	return EXIT_SUCCESS;
}