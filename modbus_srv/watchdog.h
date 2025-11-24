#include <stdio.h>
#include <hw/inout.h>

#include "types.h"

class WatchdogTimer {
public:
explicit WatchdogTimer(uintptr_t port, const  byte interv=1);
void SetInterval (const byte interv);
byte GetInterval ();
void Loop();
private:
byte interval;
uintptr_t port_nmap;
}; 

WatchdogTimer::WatchdogTimer (uintptr_t port,  const  byte interv=1): interval(interv), port_nmap(port){};

void WatchdogTimer::SetInterval  (const byte interv)
{
interval=interv;
};

byte WatchdogTimer::GetInterval (){
return interval;
}

void WatchdogTimer::Loop (){
useconds_t sleep_interval;

if (interval>1 || interval==0) {sleep_interval=900000;};
if (interval==1) {sleep_interval=200000;};

while(1){
	out8(port_nmap, (uint8_t) interval);
//	printf ("Out to port %d\n", (uint8_t) interval);
	usleep (sleep_interval);
	}
}