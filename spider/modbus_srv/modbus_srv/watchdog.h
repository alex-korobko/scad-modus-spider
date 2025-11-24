#ifndef _WATCHDOG_TIMER_
#define _WATCHDOG_TIMER_

class watchdog_timer {
friend class ghost_class_only_for_compiler_happening ;
private:
//unused metods
watchdog_timer(const watchdog_timer&) {};
watchdog_timer& operator= (const watchdog_timer& w) {return const_cast<watchdog_timer&>(w);};
~watchdog_timer(){};

byte reset_interval;
uint64_t port_number;
bool watchdog_enabled;

watchdog_timer(): reset_interval(0), port_number(0), watchdog_enabled(false){};
public:
bool is_watchdog_enabled() {return watchdog_enabled;};

void loop() throw (metro_device::metro_device_exception);

//in modbus_srv now unised, see load in main.cpp
void load(string file_name) throw (metro_device::metro_device_exception);
void load_watchdog() throw (metro_device::metro_device_exception);

static watchdog_timer& get_instance(){ static watchdog_timer wtimer; return wtimer;};
};

#endif
