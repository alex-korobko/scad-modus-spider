# Plan for COM Port Emulator Project

## Overview
Create a `com_port_emulator` subproject that emulates a COM port device in QNX 6.2, responding to Modbus commands using data from files, similar to `modbus_srv_emulator` but using serial communication instead of TCP/IP sockets.

## Project Structure

### Directory Layout
```
com_port_emulator/
├── main.cpp                    # Main entry point (similar to modbus_srv_emulator)
├── program_settings.cpp        # Configuration settings (reuse or adapt)
├── program_settings.h          # Configuration header (reuse or adapt)
├── Makefile                    # Build configuration
├── test_cond.h                 # Test condition header
├── com_port_class/             # COM port handling classes (symlink or copy)
│   ├── comport.h
│   ├── comport.cpp
│   └── comport_exception.h
├── containers_classes/         # Container classes (symlink)
├── metro_devices_classes/      # Metro device classes (symlink)
├── socket_classes/             # Not needed (COM port instead)
├── system_settings_classes/    # System settings (symlink)
├── gcc_ntox86/                 # Build output directory
│   ├── Makefile
│   └── [device_number]/       # Response files per device
│       ├── 4.txt              # Function 4 responses
│       ├── 5.txt              # Function 5 responses
│       ├── 6.txt              # Function 6 responses
│       └── 16.txt             # Function 16 responses
├── indHfiles                   # Include paths
├── indIpath
├── indLfiles
├── indLpath
├── indOfiles
├── indPlatform
├── indTestMode
└── indVpath
```

## Key Differences from modbus_srv_emulator

### 1. Communication Layer
- **modbus_srv_emulator**: Uses TCP/IP sockets (`server_socket`, `client_socket`)
- **com_port_emulator**: Uses serial COM port (`comport` class)

### 2. Connection Model
- **modbus_srv_emulator**: Server accepts multiple client connections (one thread per connection)
- **com_port_emulator**: Single COM port connection (one thread handles all requests)

### 3. Virtual Device Creation
- **modbus_srv_emulator**: Listens on TCP port (no virtual device needed)
- **com_port_emulator**: Creates/uses virtual COM port device in QNX (`/dev/ser1`, `/dev/ser2`, etc.)

## Implementation Steps

### Phase 1: Project Setup
1. **Create directory structure**
   - Create `com_port_emulator/` directory next to `modbus_srv_emulator/`
   - Create subdirectories: `gcc_ntox86/`, `com_port_class/`
   - Create symlinks to shared classes:
     - `containers_classes` → `../classes/metro_devices/containers_classes`
     - `metro_devices_classes` → `../classes/metro_devices/metro_devices_classes`
     - `system_settings_classes` → `../classes/system_settings/system_settings_classes`

2. **Copy and adapt files**
   - Copy `modbus_srv_emulator/main.cpp` → `com_port_emulator/main.cpp`
   - Copy `modbus_srv_emulator/program_settings.*` → `com_port_emulator/`
   - Copy `modbus_srv_emulator/Makefile` → `com_port_emulator/Makefile`
   - Copy `modbus_srv_emulator/test_cond.h` → `com_port_emulator/test_cond.h`
   - Copy COM port classes from `classes/com_port_class/` or create symlink

3. **Create build configuration files**
   - Copy `ind*` files from `modbus_srv_emulator/` and adapt paths
   - Create `gcc_ntox86/Makefile` based on modbus_srv_emulator structure

### Phase 2: Main Implementation Changes

#### 2.1 Replace Socket Communication with COM Port

**In main.cpp:**

1. **Remove socket includes, add COM port includes:**
   ```cpp
   // Remove:
   #include "socket_exception.h"
   #include "generic_socket.h"
   #include "server_socket.h"
   #include "client_socket.h"
   
   // Add:
   #include "comport_exception.h"
   #include "comport.h"
   ```

2. **Replace socket thread function with COM port thread function:**
   - Function name: `socket_thread_function` → `comport_thread_function`
   - Parameter: `generic_socket*` → `comport*`
   - Replace `socket_device->recv()` with `comport_device->recv()`
   - Replace `socket_device->send()` with `comport_device->send()`

3. **Update main() function:**
   ```cpp
   // Replace:
   server_socket *socket_device;
   socket_device = new server_socket(...);
   socket_device->initialize();
   accepted_socket = socket_device->accept();
   
   // With:
   comport *comport_device;
   comport_device = new comport(
       com_port_number,        // From config or command line
       comport::BAUD_9600,     // Or from config
       comport::PARITY_DISABLE,
       comport::DATA_BITS_8,
       comport::STOP_BITS_1,
       delay_value,            // Optional delay
       comport_delay_koeff     // Optional coefficient
   );
   pthread_create(..., &comport_thread_function, comport_device);
   ```

#### 2.2 Adapt Packet Reading Logic

**Key differences in packet reading:**
- COM port is stream-based (no connection concept)
- Need to handle byte-by-byte reading with timeouts
- Same Modbus protocol structure applies
- CRC validation remains the same

**In comport_thread_function:**
```cpp
void* comport_thread_function(void* comport_ptr) {
    comport *comport_device = static_cast<comport*>(comport_ptr);
    metro_device::command_data telemetr_request;
    metro_device::command_data telemetr_answer;
    
    while(failures_count < MAX_FAILURES) {
        try {
            telemetr_request.clear();
            
            // Read Modbus packet header (same as socket version)
            int header_bytes = comport_device->recv(
                telemetr_request,
                program_settings::MODBUS_REQUEST_HEADER_SIZE,
                false,  // flush_io_buffers
                receive_timeout
            );
            
            // Rest of packet reading logic (same as socket version)
            // ... function code detection, CRC validation, etc.
            
            // Read response from file (same function)
            telemetr_answer = read_response_from_file(device_number, command_code);
            
            // Send response via COM port
            comport_device->send(telemetr_answer);
            
        } catch (comport_exception cmp_exc) {
            // Error handling
        }
    }
    
    delete comport_device;
    return 0;
}
```

#### 2.3 File Reading Function
- **Keep unchanged**: `read_response_from_file()` function works the same
- File structure: `gcc_ntox86/[device_number]/[function_code].txt`
- Format: One hex value per line (e.g., `0x3`, `0x4`, `0x26`)

### Phase 3: Configuration

#### 3.1 Program Settings
- Add COM port number configuration
- Add baud rate configuration (default: 9600)
- Add parity/data bits/stop bits configuration
- Add receive timeout configuration
- Add delay/coefficient settings for timing

#### 3.2 Command Line Arguments
```cpp
Usage: com_port_emulator [options]
Options:
  -p <port>     COM port number (1-4, default: 1)
  -b <baud>     Baud rate (9600, 19200, 115200, default: 9600)
  -c <config>  Configuration file path
```

### Phase 4: Virtual COM Port Setup (QNX 6.2) - Integrated Resource Manager

#### 4.1 QNX Resource Manager Overview

In QNX 6.2, an **integrated resource manager** must be created within the `com_port_emulator` process to provide a virtual COM port device. A resource manager is a process that manages a namespace entry (like `/dev/ser1`) and handles I/O operations (open, read, write, ioctl, etc.) using QNX's message-passing architecture.

**Key Concepts:**
- Resource managers register a pathname in the filesystem namespace using `resmgr_attach()`
- They handle POSIX I/O operations (open, read, write, close, ioctl) via message passing
- They use `MsgSend()`/`MsgReceive()` for synchronous communication with clients
- They run in the same process as the Modbus handler, sharing memory directly
- They use `dispatch_block()` to wait for and process incoming messages

**Architecture Benefits:**
- No inter-process communication overhead
- Direct memory sharing between resource manager and Modbus handler
- Simpler synchronization (can use mutexes, condition variables)
- Single process to manage and debug

#### 4.2 Required Headers and Libraries

**Include Files:**
```cpp
#include <sys/iofunc.h>        // I/O function helpers
#include <sys/resmgr.h>        // Resource manager functions
#include <sys/dispatch.h>      // Dispatch message handling
#include <sys/neutrino.h>      // QNX-specific functions
#include <sys/iomsg.h>         // I/O message structures
#include <sys/termios.h>       // Terminal I/O control
#include <sys/dcmd_chr.h>      // Character device commands
#include <devctl.h>            // Device control
#include <fcntl.h>             // File control
#include <errno.h>             // Error codes
#include <pthread.h>           // Threading
#include <string.h>            // String functions
#include <stdlib.h>            // Standard library
```

**Link Libraries:**
- No additional libraries needed (all functions in libc)

#### 4.3 Data Structures and Global State

**Resource Manager State Structure:**
```cpp
typedef struct {
    // Resource manager components
    dispatch_t *dpp;                    // Dispatch structure
    resmgr_connect_funcs_t connect_funcs; // Connect function table
    resmgr_io_funcs_t io_funcs;         // I/O function table
    iofunc_attr_t attr;                 // Attribute structure
    iofunc_mount_t mount;               // Mount structure
    
    // Serial port configuration
    struct termios termios_settings;    // Terminal I/O settings
    int baud_rate;                      // Current baud rate
    byte parity;                         // Parity setting
    byte data_bits;                      // Data bits (5-8)
    byte stop_bits;                      // Stop bits (1-2)
    
    // Thread synchronization
    pthread_mutex_t request_mutex;      // Protects request buffer
    pthread_mutex_t response_mutex;      // Protects response buffer
    pthread_cond_t request_ready;       // Signals new request
    pthread_cond_t response_ready;      // Signals new response
    
    // Data buffers
    metro_device::command_data request_buffer;  // Incoming Modbus request
    metro_device::command_data response_buffer; // Outgoing Modbus response
    bool request_pending;               // Flag for pending request
    bool response_pending;              // Flag for pending response
    
    // Open connection tracking
    iofunc_ocb_t *open_ocb;             // Open control block (only one client)
    bool device_open;                   // Device open status
    
    // Statistics
    uint64_t bytes_received;
    uint64_t bytes_sent;
    uint64_t packets_received;
    uint64_t packets_sent;
} com_port_rm_state_t;

static com_port_rm_state_t rm_state;
```

#### 4.4 Resource Manager Initialization

**Complete Initialization Code:**
```cpp
int init_resource_manager(int com_port_number, int baud_rate, 
                          byte parity, byte data_bits, byte stop_bits) {
    int resmgr_id;
    resmgr_attr_t resmgr_attr;
    ostringstream device_path;
    
    // Build device path: /dev/ser1, /dev/ser2, etc.
    device_path << "/dev/ser" << com_port_number;
    
    // Initialize mount structure
    iofunc_mount_t *mount = &rm_state.mount;
    mount->conf = _IOFUNC_MOUNT_CONF;
    mount->flags = _IOFUNC_MOUNT_FLAG_USE_ACCESS;
    
    // Initialize attribute structure
    iofunc_attr_init(&rm_state.attr, S_IFCHR | 0666, NULL, NULL);
    rm_state.attr.mount = mount;
    rm_state.attr.nbytes = 0;  // Character device, no file size
    
    // Initialize function tables
    iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &rm_state.connect_funcs,
                     _RESMGR_IO_NFUNCS, &rm_state.io_funcs);
    
    // Set up connect functions
    rm_state.connect_funcs.open = io_open_handler;
    rm_state.connect_funcs.close_ocb = io_close_handler;
    
    // Set up I/O functions
    rm_state.io_funcs.read = io_read_handler;
    rm_state.io_funcs.write = io_write_handler;
    rm_state.io_funcs.devctl = io_devctl_handler;
    rm_state.io_funcs.notify = io_notify_handler;
    
    // Initialize termios structure with default values
    memset(&rm_state.termios_settings, 0, sizeof(struct termios));
    rm_state.termios_settings.c_iflag = IGNBRK | IGNPAR;
    rm_state.termios_settings.c_oflag = 0;
    rm_state.termios_settings.c_cflag = CS8 | CLOCAL | CREAD;
    rm_state.termios_settings.c_lflag = 0;
    rm_state.termios_settings.c_cc[VMIN] = 1;   // Minimum characters to read
    rm_state.termios_settings.c_cc[VTIME] = 0;   // Timeout (0 = no timeout)
    
    // Set serial port parameters
    rm_state.baud_rate = baud_rate;
    rm_state.parity = parity;
    rm_state.data_bits = data_bits;
    rm_state.stop_bits = stop_bits;
    
    // Apply baud rate
    cfsetispeed(&rm_state.termios_settings, baud_rate);
    cfsetospeed(&rm_state.termios_settings, baud_rate);
    
    // Apply data bits
    rm_state.termios_settings.c_cflag &= ~CSIZE;
    switch (data_bits) {
        case 5: rm_state.termios_settings.c_cflag |= CS5; break;
        case 6: rm_state.termios_settings.c_cflag |= CS6; break;
        case 7: rm_state.termios_settings.c_cflag |= CS7; break;
        case 8: rm_state.termios_settings.c_cflag |= CS8; break;
    }
    
    // Apply parity
    rm_state.termios_settings.c_cflag &= ~(PARENB | PARODD);
    if (parity == PARITY_ODD) {
        rm_state.termios_settings.c_cflag |= PARENB | PARODD;
    } else if (parity == PARITY_EVEN) {
        rm_state.termios_settings.c_cflag |= PARENB;
    }
    
    // Apply stop bits
    if (stop_bits == STOP_BITS_2) {
        rm_state.termios_settings.c_cflag |= CSTOPB;
    } else {
        rm_state.termios_settings.c_cflag &= ~CSTOPB;
    }
    
    // Initialize synchronization primitives
    pthread_mutex_init(&rm_state.request_mutex, NULL);
    pthread_mutex_init(&rm_state.response_mutex, NULL);
    pthread_cond_init(&rm_state.request_ready, NULL);
    pthread_cond_init(&rm_state.response_ready, NULL);
    
    // Initialize state
    rm_state.device_open = false;
    rm_state.request_pending = false;
    rm_state.response_pending = false;
    rm_state.open_ocb = NULL;
    rm_state.bytes_received = 0;
    rm_state.bytes_sent = 0;
    rm_state.packets_received = 0;
    rm_state.packets_sent = 0;
    
    // Create dispatch structure
    rm_state.dpp = dispatch_create();
    if (rm_state.dpp == NULL) {
        return -1;  // Error creating dispatch
    }
    
    // Set up resource manager attributes
    memset(&resmgr_attr, 0, sizeof(resmgr_attr));
    resmgr_attr.nparts_max = 1;
    resmgr_attr.msg_max_size = 2048;  // Maximum message size
    
    // Attach resource manager to pathname
    resmgr_id = resmgr_attach(rm_state.dpp, &resmgr_attr,
                              device_path.str().c_str(),
                              _FTYPE_ANY, 0,
                              &rm_state.connect_funcs,
                              &rm_state.io_funcs,
                              &rm_state.attr);
    
    if (resmgr_id == -1) {
        dispatch_destroy(rm_state.dpp);
        return -1;  // Error attaching
    }
    
    return 0;  // Success
}
```

#### 4.4 Resource Manager Implementation Details

#### 4.5 I/O Handler Function Implementations

**1. Open Handler (`io_open_handler`):**
```cpp
int io_open_handler(resmgr_context_t *ctp, io_open_t *msg, 
                    RESMGR_HANDLE_T *handle, void *extra) {
    iofunc_ocb_t *ocb;
    
    // Check if device is already open (only allow one client)
    if (rm_state.device_open) {
        return EBUSY;  // Device busy
    }
    
    // Call default open handler
    if ((ocb = iofunc_ocb_calloc(handle, &rm_state.attr)) == NULL) {
        return ENOMEM;
    }
    
    // Set up open control block
    ocb->ioflag = msg->connect.ioflag;
    ocb->sflag = msg->connect.sflag;
    ocb->offset = 0;
    
    // Store OCB for later use
    rm_state.open_ocb = ocb;
    rm_state.device_open = true;
    
    // Initialize buffers
    rm_state.request_buffer.clear();
    rm_state.response_buffer.clear();
    rm_state.request_pending = false;
    rm_state.response_pending = false;
    
    // Return status
    return iofunc_open_default(ctp, msg, handle, ocb, extra);
}
```

**2. Read Handler (`io_read_handler`) - Receives Modbus Requests:**
```cpp
int io_read_handler(resmgr_context_t *ctp, io_read_t *msg,
                    iofunc_ocb_t *ocb) {
    int status;
    int nbytes;
    metro_device::command_data data_to_send;
    
    // Check if device is open
    if (!rm_state.device_open || ocb != rm_state.open_ocb) {
        return EBADF;
    }
    
    // Lock request mutex
    pthread_mutex_lock(&rm_state.request_mutex);
    
    // Wait for request data to be available
    while (!rm_state.request_pending && rm_state.device_open) {
        pthread_cond_wait(&rm_state.request_ready, &rm_state.request_mutex);
    }
    
    if (!rm_state.device_open) {
        pthread_mutex_unlock(&rm_state.request_mutex);
        return EBADF;
    }
    
    // Copy request data
    data_to_send = rm_state.request_buffer;
    rm_state.request_pending = false;
    
    pthread_mutex_unlock(&rm_state.request_mutex);
    
    // Calculate bytes to send
    nbytes = min(static_cast<int>(data_to_send.size()), 
                 msg->i.nbytes);
    
    if (nbytes == 0) {
        return 0;  // No data available
    }
    
    // Set up reply message
    SETIOV(ctp->iov, data_to_send.data(), nbytes);
    
    // Update statistics
    rm_state.bytes_sent += nbytes;
    rm_state.packets_sent++;
    
    // Send reply
    return _RESMGR_NPARTS(1);
}
```

**3. Write Handler (`io_write_handler`) - Receives Modbus Responses from Client:**
```cpp
int io_write_handler(resmgr_context_t *ctp, io_write_t *msg,
                     iofunc_ocb_t *ocb) {
    int nbytes;
    byte *data_ptr;
    metro_device::command_data received_data;
    
    // Check if device is open
    if (!rm_state.device_open || ocb != rm_state.open_ocb) {
        return EBADF;
    }
    
    // Get data pointer from message
    data_ptr = _DEVCTL_DATA(msg->i);
    nbytes = msg->i.nbytes;
    
    if (nbytes <= 0) {
        return 0;
    }
    
    // Copy received data
    received_data.assign(data_ptr, data_ptr + nbytes);
    
    // Lock request mutex and store request
    pthread_mutex_lock(&rm_state.request_mutex);
    
    rm_state.request_buffer = received_data;
    rm_state.request_pending = true;
    
    // Signal Modbus handler thread
    pthread_cond_signal(&rm_state.request_ready);
    
    pthread_mutex_unlock(&rm_state.request_mutex);
    
    // Update statistics
    rm_state.bytes_received += nbytes;
    rm_state.packets_received++;
    
    // Return number of bytes written
    _IO_SET_WRITE_NBYTES(ctp, nbytes);
    return _RESMGR_NPARTS(0);
}
```

**4. Device Control Handler (`io_devctl_handler`) - Serial Port Configuration:**
```cpp
int io_devctl_handler(resmgr_context_t *ctp, io_devctl_t *msg,
                      iofunc_ocb_t *ocb) {
    int status = EOK;
    void *data_ptr = _DEVCTL_DATA(msg->i);
    int nbytes = msg->i.nbytes;
    
    // Check if device is open
    if (!rm_state.device_open || ocb != rm_state.open_ocb) {
        return EBADF;
    }
    
    switch (msg->i.dcmd) {
        case DCMD_CHR_TCGETATTR:
            // Get terminal attributes
            if (nbytes >= sizeof(struct termios)) {
                memcpy(data_ptr, &rm_state.termios_settings, 
                       sizeof(struct termios));
                _IO_SET_DEVCTL_NBYTES(ctp, sizeof(struct termios));
            } else {
                status = EINVAL;
            }
            break;
            
        case DCMD_CHR_TCSETATTR:
            // Set terminal attributes
            if (nbytes >= sizeof(struct termios)) {
                struct termios *new_termios = 
                    static_cast<struct termios*>(data_ptr);
                
                // Update internal settings
                memcpy(&rm_state.termios_settings, new_termios, 
                       sizeof(struct termios));
                
                // Extract baud rate
                speed_t ispeed = cfgetispeed(new_termios);
                speed_t ospeed = cfgetospeed(new_termios);
                rm_state.baud_rate = ispeed;
                
                // Extract data bits
                int csize = new_termios->c_cflag & CSIZE;
                switch (csize) {
                    case CS5: rm_state.data_bits = 5; break;
                    case CS6: rm_state.data_bits = 6; break;
                    case CS7: rm_state.data_bits = 7; break;
                    case CS8: rm_state.data_bits = 8; break;
                }
                
                // Extract parity
                if (new_termios->c_cflag & PARENB) {
                    if (new_termios->c_cflag & PARODD) {
                        rm_state.parity = PARITY_ODD;
                    } else {
                        rm_state.parity = PARITY_EVEN;
                    }
                } else {
                    rm_state.parity = PARITY_DISABLE;
                }
                
                // Extract stop bits
                if (new_termios->c_cflag & CSTOPB) {
                    rm_state.stop_bits = STOP_BITS_2;
                } else {
                    rm_state.stop_bits = STOP_BITS_1;
                }
                
                _IO_SET_DEVCTL_NBYTES(ctp, 0);
            } else {
                status = EINVAL;
            }
            break;
            
        case DCMD_CHR_TCFLUSH:
            // Flush input/output buffers
            pthread_mutex_lock(&rm_state.request_mutex);
            rm_state.request_buffer.clear();
            rm_state.request_pending = false;
            pthread_mutex_unlock(&rm_state.request_mutex);
            
            pthread_mutex_lock(&rm_state.response_mutex);
            rm_state.response_buffer.clear();
            rm_state.response_pending = false;
            pthread_mutex_unlock(&rm_state.response_mutex);
            
            _IO_SET_DEVCTL_NBYTES(ctp, 0);
            break;
            
        default:
            // Unknown command, try default handler
            status = iofunc_devctl_default(ctp, msg, ocb);
            break;
    }
    
    return status;
}
```

**5. Close Handler (`io_close_handler`):**
```cpp
int io_close_handler(resmgr_context_t *ctp, void *reserved,
                     iofunc_ocb_t *ocb) {
    // Clear open state
    pthread_mutex_lock(&rm_state.request_mutex);
    rm_state.device_open = false;
    rm_state.open_ocb = NULL;
    rm_state.request_buffer.clear();
    rm_state.request_pending = false;
    
    // Wake up any waiting threads
    pthread_cond_broadcast(&rm_state.request_ready);
    pthread_mutex_unlock(&rm_state.request_mutex);
    
    // Call default close handler
    return iofunc_close_ocb_default(ctp, reserved, ocb);
}
```

**6. Notify Handler (`io_notify_handler`):**
```cpp
int io_notify_handler(resmgr_context_t *ctp, io_notify_t *msg,
                      iofunc_ocb_t *ocb) {
    // Handle notification events (e.g., data available)
    return iofunc_notify_default(ctp, msg, ocb);
}
```

#### 4.6 Resource Manager Thread Implementation

**Resource Manager Message Loop Thread:**
```cpp
void* resource_manager_thread(void *arg) {
    dispatch_context_t *ctp;
    program_settings* sett_obj = program_settings::get_instance();
    
    // Allocate dispatch context
    ctp = dispatch_context_alloc(rm_state.dpp);
    if (ctp == NULL) {
        sett_obj->sys_message(program_settings::ERROR_MSG,
                             "Failed to allocate dispatch context");
        return NULL;
    }
    
    sett_obj->sys_message(program_settings::INFO_MSG,
                         "Resource manager thread started");
    
    // Main message loop
    while (rm_state.device_open || ctp != NULL) {
        // Block waiting for messages
        ctp = dispatch_block(ctp);
        
        if (ctp == NULL) {
            // Error or shutdown
            break;
        }
        
        // Handle message
        dispatch_handler(ctp);
    }
    
    sett_obj->sys_message(program_settings::INFO_MSG,
                         "Resource manager thread exiting");
    
    // Cleanup
    dispatch_context_free(ctp);
    return NULL;
}
```

#### 4.7 Modbus Handler Thread Implementation

**Modbus Handler Thread - Processes Requests and Generates Responses:**
```cpp
void* modbus_handler_thread(void *arg) {
    program_settings* sett_obj = program_settings::get_instance();
    metro_device::command_data telemetr_request;
    metro_device::command_data telemetr_answer;
    metro_device::command_data::iterator tmp_iter;
    program_settings::bytes telemetr_crc;
    word crc_value;
    int failures_count = 0;
    
    sett_obj->sys_message(program_settings::INFO_MSG,
                         "Modbus handler thread started");
    
    while (failures_count < program_settings::SOCKET_FAILURES_COUNT) {
        try {
            telemetr_request.clear();
            
            // Wait for request from resource manager
            pthread_mutex_lock(&rm_state.request_mutex);
            
            while (!rm_state.request_pending && rm_state.device_open) {
                pthread_cond_wait(&rm_state.request_ready, 
                                 &rm_state.request_mutex);
            }
            
            if (!rm_state.device_open) {
                pthread_mutex_unlock(&rm_state.request_mutex);
                break;  // Device closed, exit thread
            }
            
            // Copy request
            telemetr_request = rm_state.request_buffer;
            rm_state.request_pending = false;
            
            pthread_mutex_unlock(&rm_state.request_mutex);
            
            // Validate request size
            if (telemetr_request.size() < 
                program_settings::MODBUS_REQUEST_HEADER_SIZE) {
                throw comport_exception("Request too short");
            }
            
            // Determine function code and required sizes
            byte function_code = telemetr_request[
                program_settings::MODBUS_FUNCTION_CODE_INDEX];
            int function_partial_size = 0;
            
            switch (function_code) {
                case 4: 
                    function_partial_size = 
                        program_settings::MODBUS_FUNCTION_4_REQUEST_PARTIAL_SIZE; 
                    break;
                case 5: 
                    function_partial_size = 
                        program_settings::MODBUS_FUNCTION_5_REQUEST_PARTIAL_SIZE; 
                    break;
                case 6: 
                    function_partial_size = 
                        program_settings::MODBUS_FUNCTION_6_REQUEST_PARTIAL_SIZE; 
                    break;
                case 16: 
                    function_partial_size = 
                        program_settings::MODBUS_FUNCTION_16_REQUEST_PARTIAL_SIZE; 
                    break;
                default:
                    throw comport_exception("Unsupported function code");
            }
            
            // Validate complete packet size
            int total_expected_size = 
                program_settings::MODBUS_REQUEST_HEADER_SIZE + 
                function_partial_size + 
                ((function_code == 16) ? 
                 telemetr_request[program_settings::MODBUS_REQUEST_HEADER_SIZE + 
                                 function_partial_size - 1] : 0) +
                program_settings::MODBUS_CRC_SIZE;
            
            if (static_cast<int>(telemetr_request.size()) < 
                total_expected_size) {
                throw comport_exception("Incomplete packet");
            }
            
            // Verify CRC
            tmp_iter = telemetr_request.begin();
            advance(tmp_iter, telemetr_request.size() - 
                   program_settings::MODBUS_CRC_SIZE);
            crc_value = program_settings::crc(
                metro_device::command_data(telemetr_request.begin(), tmp_iter));
            telemetr_crc = program_settings::bytes_of_type<word>(crc_value);
            
            if (telemetr_crc[0] != telemetr_request[telemetr_request.size()-2] ||
                telemetr_crc[1] != telemetr_request[telemetr_request.size()-1]) {
                throw comport_exception("Bad packet CRC");
            }
            
            // Print request to stdout
            cout << "Request to device: ";
            vector<char> tmp_chars(32);
            for (metro_device::command_data::size_type i = 0; 
                 i < telemetr_request.size(); i++) {
                itoa(telemetr_request[i], &tmp_chars[0], 16);
                cout << "0x" << &tmp_chars[0];
                if (i < telemetr_request.size() - 1) cout << " ";
            }
            cout << endl;
            
            // Read response from file
            byte device_number = telemetr_request[
                program_settings::MODBUS_DEVICE_NUMBER_INDEX];
            telemetr_answer = read_response_from_file(device_number, function_code);
            
            // If file response is empty, generate error response
            if (telemetr_answer.empty()) {
                ostringstream exception_description;
                exception_description << "No response file found for device " <<
                    static_cast<int>(device_number) << " command " <<
                    static_cast<int>(function_code);
                sett_obj->sys_message(program_settings::ERROR_MSG,
                                     exception_description.str());
                
                telemetr_answer = metro_device::generate_error_answer_for_function(
                    device_number, function_code,
                    program_settings::MODBUS_EXCEPTION_FAILURE_IN_ASSOCIATED_DEVICE);
            } else {
                // Calculate and append CRC
                crc_value = program_settings::crc(telemetr_answer);
                telemetr_crc = program_settings::bytes_of_type<word>(crc_value);
                telemetr_answer.insert(telemetr_answer.end(),
                                      telemetr_crc.begin(), telemetr_crc.end());
            }
            
            // Store response for resource manager to send
            pthread_mutex_lock(&rm_state.response_mutex);
            rm_state.response_buffer = telemetr_answer;
            rm_state.response_pending = true;
            pthread_cond_signal(&rm_state.response_ready);
            pthread_mutex_unlock(&rm_state.response_mutex);
            
            failures_count = 0;
            
        } catch (comport_exception cmp_exc) {
            string message("Modbus handler thread error: ");
            message += cmp_exc.get_description();
            sett_obj->sys_message(program_settings::ERROR_MSG, message);
            failures_count++;
            continue;
        }
    }
    
    sett_obj->sys_message(program_settings::INFO_MSG,
                         "Modbus handler thread exiting");
    return NULL;
}
```

#### 4.8 Main Function Integration

**Complete Main Function:**
```cpp
int main(int argc, const char *argv[]) {
    program_settings* sett_obj = program_settings::get_instance();
    pthread_t rm_thread, modbus_thread;
    pthread_attr_t attr;
    int pthread_creating_result;
    int com_port_number = 1;  // Default
    int baud_rate = comport::BAUD_9600;
    
    // Parse command line arguments
    // ... (argument parsing code)
    
    // Initialize pthread attributes
    if (pthread_attr_init(&attr) != EOK) {
        sett_obj->sys_message(program_settings::ERROR_MSG,
                             "Can't pthread_attr_init(&attr)");
        return 1;
    }
    
    if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != EOK) {
        sett_obj->sys_message(program_settings::ERROR_MSG,
                             "Can't pthread_attr_setdetachstate");
        return 1;
    }
    
    // Initialize resource manager
    if (init_resource_manager(com_port_number, baud_rate,
                             comport::PARITY_DISABLE,
                             comport::DATA_BITS_8,
                             comport::STOP_BITS_1) != 0) {
        sett_obj->sys_message(program_settings::ERROR_MSG,
                             "Failed to initialize resource manager");
        return 1;
    }
    
    sett_obj->sys_message(program_settings::INFO_MSG,
                         "Resource manager initialized for /dev/ser" + 
                         to_string(com_port_number));
    
    // Start resource manager thread
    pthread_creating_result = pthread_create(&rm_thread, &attr,
                                            resource_manager_thread, NULL);
    if (pthread_creating_result != EOK) {
        string message("Failed to create resource manager thread: ");
        message += strerror(pthread_creating_result);
        sett_obj->sys_message(program_settings::ERROR_MSG, message);
        cleanup_resource_manager();
        return 1;
    }
    
    // Start Modbus handler thread
    pthread_creating_result = pthread_create(&modbus_thread, &attr,
                                            modbus_handler_thread, NULL);
    if (pthread_creating_result != EOK) {
        string message("Failed to create Modbus handler thread: ");
        message += strerror(pthread_creating_result);
        sett_obj->sys_message(program_settings::ERROR_MSG, message);
        cleanup_resource_manager();
        return 1;
    }
    
    sett_obj->sys_message(program_settings::INFO_MSG,
                         "COM port emulator started successfully");
    
    // Main loop - wait for threads
    pthread_join(rm_thread, NULL);
    pthread_join(modbus_thread, NULL);
    
    // Cleanup
    cleanup_resource_manager();
    
    return 0;
}
```

#### 4.9 Cleanup Function

**Resource Manager Cleanup:**
```cpp
void cleanup_resource_manager() {
    // Close device if open
    if (rm_state.device_open) {
        rm_state.device_open = false;
        pthread_cond_broadcast(&rm_state.request_ready);
        pthread_cond_broadcast(&rm_state.response_ready);
    }
    
    // Destroy synchronization primitives
    pthread_mutex_destroy(&rm_state.request_mutex);
    pthread_mutex_destroy(&rm_state.response_mutex);
    pthread_cond_destroy(&rm_state.request_ready);
    pthread_cond_destroy(&rm_state.response_ready);
    
    // Destroy dispatch structure
    if (rm_state.dpp != NULL) {
        dispatch_destroy(rm_state.dpp);
        rm_state.dpp = NULL;
    }
}
```

#### 4.10 Thread Synchronization Details

**Synchronization Pattern:**
- **Request Flow**: Client writes → `io_write_handler` → stores in `request_buffer` → signals `request_ready` → Modbus handler processes
- **Response Flow**: Modbus handler generates → stores in `response_buffer` → signals `response_ready` → `io_read_handler` sends to client

**Critical Sections:**
- `request_buffer` and `request_pending` protected by `request_mutex`
- `response_buffer` and `response_pending` protected by `response_mutex`
- `device_open` and `open_ocb` protected by `request_mutex`

**Condition Variables:**
- `request_ready`: Signaled when new request arrives from client
- `response_ready`: Signaled when new response is ready from Modbus handler

#### 4.11 Termios Configuration Details

**Termios Structure Fields:**
```cpp
struct termios {
    tcflag_t c_iflag;  // Input modes
    tcflag_t c_oflag;  // Output modes
    tcflag_t c_cflag;  // Control modes
    tcflag_t c_lflag; // Local modes
    cc_t c_cc[NCCS];   // Control characters
};
```

**Key Flags for Serial Communication:**
- `c_cflag`: CS8 (8 data bits), CLOCAL (local line), CREAD (enable receiver), PARENB (parity enable), CSTOPB (2 stop bits)
- `c_iflag`: IGNBRK (ignore break), IGNPAR (ignore parity errors)
- `c_lflag`: Should be 0 for raw mode (no line editing)
- `c_cc[VMIN]`: Minimum characters to read before returning
- `c_cc[VTIME]`: Timeout in deciseconds (0 = no timeout)

**Baud Rate Constants:**
```cpp
B0, B50, B75, B110, B134, B150, B200, B300, B600, B1200, B1800,
B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400
```

#### 4.12 Message Passing Details

**QNX Message Structure:**
- Messages are synchronous (blocking)
- Client calls `read()` → sends message → blocks until response
- Resource manager receives message → processes → sends reply → client unblocks
- Maximum message size: 2048 bytes (configurable in `resmgr_attr.msg_max_size`)

**I/O Message Types:**
- `_IO_READ`: Read operation
- `_IO_WRITE`: Write operation
- `_IO_OPEN`: Open operation
- `_IO_CLOSE`: Close operation
- `_IO_DEVCTL`: Device control operation

**Reply Mechanism:**
- Use `SETIOV()` to set up I/O vector for data
- Return `_RESMGR_NPARTS(n)` where n is number of I/O vectors
- Use `_IO_SET_WRITE_NBYTES()` to set bytes written
- Use `_IO_SET_DEVCTL_NBYTES()` to set device control response size

#### 4.13 Error Handling

**Common Error Codes:**
- `EBUSY`: Device already open (only one client allowed)
- `EBADF`: Bad file descriptor (device not open)
- `EINVAL`: Invalid parameter
- `ENOMEM`: Out of memory
- `EIO`: I/O error
- `ETIMEDOUT`: Operation timed out

**Error Handling Strategy:**
- Validate all inputs in handlers
- Check device state before operations
- Return appropriate errno values
- Log errors via `program_settings::sys_message()`
- Handle client disconnections gracefully

#### 4.14 Resource Manager Implementation Checklist

- [ ] Include required headers (`<sys/resmgr.h>`, `<sys/iofunc.h>`, `<sys/dispatch.h>`)
- [ ] Initialize dispatch structure
- [ ] Set up connect and I/O function tables
- [ ] Implement `io_open()` handler
- [ ] Implement `io_read()` handler (for Modbus requests)
- [ ] Implement `io_write()` handler (for Modbus responses)
- [ ] Implement `io_devctl()` handler (for serial port configuration)
- [ ] Implement `io_close()` handler
- [ ] Register pathname (`/dev/ser1`, `/dev/ser2`, etc.)
- [ ] Start dispatch message loop
- [ ] Handle thread synchronization between RM and Modbus handler
- [ ] Test with Modbus master client

#### 4.15 Additional Technical Considerations

**Permissions and Path Registration:**
- Resource manager must run with appropriate permissions to create `/dev/ser*`
- May need to run as root or use `setuid` if creating system device nodes
- Alternative: Use `/tmp/ser*` or user-specific paths for testing
- Path registration requires write access to parent directory

**Thread Safety:**
- All shared data structures must be protected by mutexes
- Condition variables used for thread synchronization
- Only one client connection allowed (enforced in `io_open_handler`)
- OCB (Open Control Block) tracks per-connection state

**Performance Considerations:**
- Message passing is synchronous but efficient in QNX
- No data copying needed between threads (shared memory)
- Buffer sizes should accommodate largest Modbus packet
- Consider timeout values for blocking operations

**Terminal Attributes Management:**
- `termios` structure maintained per device
- Changes via `DCMD_CHR_TCSETATTR` update internal state
- Raw mode recommended for Modbus (no line editing)
- Flow control (RTS/CTS) can be simulated if needed

**Device Control Commands:**
- `DCMD_CHR_TCGETATTR`: Get terminal attributes
- `DCMD_CHR_TCSETATTR`: Set terminal attributes
- `DCMD_CHR_TCFLUSH`: Flush input/output buffers
- Custom commands can be added for emulator-specific features

**Statistics and Monitoring:**
- Track bytes sent/received
- Track packets sent/received
- Track errors and failures
- Can be exposed via additional devctl commands

**Shutdown and Cleanup:**
- Graceful shutdown when device closed
- Wake up all waiting threads
- Free all allocated resources
- Unregister pathname (automatic on process exit)

**References:**
- QNX 6.2/6.3 Resource Manager Programming Guide
- QNX Neutrino Programmer's Guide - Resource Managers
- `resmgr_attach()` man page
- `iofunc_*` function documentation
- `dispatch_*` function documentation
- Example resource managers in QNX SDK
- QNX Character Device Resource Manager examples

### Phase 5: Testing and Validation

#### 5.1 Test Setup
1. Create test response files in `gcc_ntox86/[device_number]/`
2. Test with Modbus master connecting via COM port
3. Verify packet reading/writing
4. Verify CRC calculation
5. Test error handling (timeouts, invalid packets)

#### 5.2 Test Scenarios
- Function 4 (Read Input Registers)
- Function 5 (Write Single Coil)
- Function 6 (Write Single Register)
- Function 16 (Write Multiple Registers)
- Error responses (file not found, invalid CRC, etc.)
- Multiple sequential requests
- Timeout handling

### Phase 6: Documentation

#### 6.1 README.md
- Project description
- Build instructions
- Configuration guide
- Virtual COM port setup instructions
- Usage examples
- File format documentation

#### 6.2 Code Comments
- Document COM port-specific differences
- Document configuration options
- Document thread safety considerations

## File Modifications Summary

### Files to Create/Modify

1. **main.cpp**
   - Replace socket code with COM port code
   - Replace `socket_thread_function` with `comport_thread_function`
   - Update main() to create COM port instead of server socket
   - Keep `read_response_from_file()` unchanged

2. **program_settings.h/cpp**
   - Add COM port configuration constants
   - Add baud rate, parity, data bits, stop bits settings
   - Add COM port number setting
   - Add receive timeout setting

3. **Makefile**
   - Update include paths
   - Update library dependencies (remove socket libs if any)
   - Add COM port class compilation

4. **gcc_ntox86/Makefile**
   - Adapt from modbus_srv_emulator version
   - Update paths and output directory

## Dependencies

### Required Classes (via symlinks)
- `com_port_class/` - COM port handling
- `containers_classes/` - Data containers
- `metro_devices_classes/` - Metro device definitions
- `system_settings_classes/` - System settings

### Required Headers
- `defines.h`
- `spider_exception.h`
- `comport_exception.h`
- `program_settings.h`
- `metro_device.h`

## Build Process

1. Create directory structure
2. Set up symlinks to shared classes
3. Copy and adapt source files
4. Update Makefile paths
5. Build with QNX 6.2 toolchain
6. Test with virtual COM port

## Testing Strategy

1. **Unit Testing**
   - Test file reading function
   - Test CRC calculation
   - Test packet parsing

2. **Integration Testing**
   - Test with real Modbus master
   - Test with virtual COM port pair
   - Test error scenarios

3. **Performance Testing**
   - Measure response times
   - Test with high request rates
   - Test timeout handling

## Future Enhancements

1. Support for multiple COM ports (multiple threads)
2. Dynamic response file switching
3. Response file cycling (like `cycle_responses.sh`)
4. Logging to file
5. Configuration file support
6. Statistics collection

## Notes

- COM port communication is half-duplex (request-response)
- Need to handle timing carefully (delays between send/receive)
- QNX COM port devices: `/dev/ser1` through `/dev/ser4`
- Consider using `select()` or `poll()` for timeout handling if needed
- COM port may need RTS/CTS flow control configuration

