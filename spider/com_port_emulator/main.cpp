using namespace std;

#include <sys/iofunc.h>
#include <sys/resmgr.h>
#include <sys/dispatch.h>
#include <sys/neutrino.h>
#include <sys/iomsg.h>
#include <termios.h>
#include <sys/dcmd_chr.h>
#include <devctl.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <inttypes.h>
#include <iostream.h>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>

#include "defines.h"
#include "spider_exception.h"
#include "program_settings.h"
#include "metro_device.h"
#include "comport_exception.h"

//Macro to set number of bytes in devctl reply message
#ifndef _IO_SET_DEVCTL_NBYTES
#define _IO_SET_DEVCTL_NBYTES(msg, nbytes) \
  do { \
   (msg)->size = (uint32_t)(nbytes); \
  } while(0)
  
#endif

// Max time io_read waits for the Modbus thread to queue a response before
// returning 0 bytes. Must stay short: the resource manager has ONE thread, so
// a blocked read also blocks every io_write/devctl from the client.
#define COM_EMU_READ_WAIT_MS 300

// Emulated device turnaround time (ms) before the reply starts; overridable
// with the optional 3rd command line argument.
static int g_reply_turnaround_ms = 10;

// Line speed as a plain number, independent of how the B<rate> constants are
// defined on the target.
static int baud_to_bps(int b) {
    if (b == B9600)   return 9600;
    if (b == B19200)  return 19200;
    if (b == B38400)  return 38400;
    if (b == B57600)  return 57600;
    if (b == B115200) return 115200;
    return 9600;
}

// Custom devctl: release device so another process can open (call from current opener, then close fd)
#define COM_EMU_DCMD_RELEASE  0x4001

// Resource Manager State Structure
typedef struct {
    // Resource manager components
    dispatch_t *dpp;
    resmgr_connect_funcs_t connect_funcs;
    resmgr_io_funcs_t io_funcs;
    iofunc_attr_t attr;
    iofunc_mount_t mount;
    
    // Serial port configuration
    struct termios termios_settings;
    int baud_rate;
    byte parity;
    byte data_bits;
    byte stop_bits;
    int line_status;   // emulated modem line status (DCMD_CHR_SERCTL/LINESTATUS)
    
    // Thread synchronization
    pthread_mutex_t request_mutex;
    pthread_mutex_t response_mutex;
    pthread_cond_t request_ready;
    pthread_cond_t response_ready;

    // Startup handshake: RM thread reports resmgr_attach() result back to main
    pthread_mutex_t init_mutex;
    pthread_cond_t  init_cond;
    bool init_done;
    int  init_status;       // 0 = attach ok, -1 = attach failed
    int  com_port_number;   // /dev/ser<com_port_number> to register

    // Data buffers
    metro_device::command_data request_buffer;
    metro_device::command_data response_buffer;
    bool request_pending;
    bool response_pending;
    
    // Open connection tracking
    iofunc_ocb_t *open_ocb;
    bool device_open;
    bool shutdown;  // true only when process is exiting (so modbus thread can exit)
    
    // Statistics
    uint64_t bytes_received;
    uint64_t bytes_sent;
    uint64_t packets_received;
    uint64_t packets_sent;
} com_port_rm_state_t;

static com_port_rm_state_t rm_state;

// Directory that contains the response-file tree: <dir>/<device>/<command>.txt.
// Resolved once at startup to the executable's own directory, so the emulator
// finds its response files no matter which working directory it is launched
// from. Empty string => fall back to the legacy cwd-relative "gcc_ntox86/".
static string g_response_base_dir;

// Forward declarations
int io_open_handler(resmgr_context_t *ctp, io_open_t *msg, RESMGR_HANDLE_T *handle, void *extra);
int io_close_handler(resmgr_context_t *ctp, void *reserved, iofunc_ocb_t *ocb);
int io_read_handler(resmgr_context_t *ctp, io_read_t *msg, iofunc_ocb_t *ocb);
int io_write_handler(resmgr_context_t *ctp, io_write_t *msg, iofunc_ocb_t *ocb);
int io_devctl_handler(resmgr_context_t *ctp, io_devctl_t *msg, iofunc_ocb_t *ocb);
//TODO confirm no need to use notification
//int io_notify_handler(resmgr_context_t *ctp, io_notify_t *msg, iofunc_ocb_t *ocb);
void* resource_manager_thread(void *arg);
void* modbus_handler_thread(void *arg);
int init_resource_manager(int com_port_number, int baud_rate, byte parity, byte data_bits, byte stop_bits);
void cleanup_resource_manager();



// Resolve the directory that holds the executable. Device directories are
// expected to sit right next to the binary (<exe_dir>/<device>/<command>.txt),
// so this lets the emulator be launched from any working directory. Tries the
// QNX procfs entry first, then falls back to argv[0]. Returns an empty string
// if the directory can't be determined.
static string resolve_exe_dir(const char *argv0) {
    char buf[1024];

    // 1) QNX Neutrino procfs: reading /proc/<pid>/exefile yields the full path
    //    of the running executable.
    {
        ostringstream proc_path;
        proc_path << "/proc/" << getpid() << "/exefile";
        int fd = open(proc_path.str().c_str(), O_RDONLY);
        if (fd != -1) {
            int n = read(fd, buf, sizeof(buf) - 1);
            close(fd);
            if (n > 0) {
                buf[n] = '\0';
                string p(buf);
                string::size_type nl = p.find_first_of("\r\n");
                if (nl != string::npos) p.erase(nl);
                // Only trust an absolute path; otherwise fall through to argv[0].
                if (!p.empty() && p[0] == '/') {
                    string::size_type slash = p.find_last_of('/');
                    if (slash != string::npos) return p.substr(0, slash);
                }
            }
        }
    }

    // 2) Fall back to argv[0], made absolute with the cwd captured now (the
    //    emulator never chdir()s, so this stays valid).
    if (argv0 != NULL && argv0[0] != '\0') {
        string a(argv0);
        string abs_path;
        if (a[0] == '/') {
            abs_path = a;                          // already absolute
        } else if (a.find('/') != string::npos) {  // relative, but has a path
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                abs_path = string(cwd) + "/" + a;
            }
        }
        // (argv0 with no '/' came from a PATH search: can't resolve here.)
        if (!abs_path.empty()) {
            string::size_type slash = abs_path.find_last_of('/');
            if (slash != string::npos) return abs_path.substr(0, slash);
        }
    }

    return string();  // unknown -> caller uses legacy cwd-relative path
}

// Private function to read response data from file
static metro_device::command_data read_response_from_file(byte device_number, byte command_code) {
    metro_device::command_data response_data;

    try {
        // Build the response file path. Device directories live next to the
        // executable, so root the path at the executable's own directory:
        //   <exe_dir>/<device>/<command>.txt
        // If the executable directory couldn't be resolved, fall back to the
        // legacy cwd-relative "gcc_ntox86/" prefix (same location when launched
        // from the directory above gcc_ntox86/).
        ostringstream file_path;
        if (!g_response_base_dir.empty()) {
            file_path << g_response_base_dir << "/";
        } else {
            file_path << "gcc_ntox86/";
        }
        vector<char> tmp_chars(20);
        itoa(device_number, &tmp_chars[0], 10);
        file_path << &tmp_chars[0] << "/";
        itoa(command_code, &tmp_chars[0], 10);
        file_path << &tmp_chars[0] << ".txt";
        
        // Open file for reading
        ifstream response_file(file_path.str().c_str());
        if (!response_file.is_open()) {
            // File missing or (more often) wrong working directory: the path is
            // relative to the emulator's cwd, so log both to make it obvious.
            int open_errno = errno;
            char cwd_buf[1024];
            ostringstream diag;
            diag << "Cannot open response file '" << file_path.str() << "'"
                 << " : " << strerror(open_errno);
            // For a relative path (legacy fallback), also show where it resolved.
            if (!file_path.str().empty() && file_path.str()[0] != '/'
                && getcwd(cwd_buf, sizeof(cwd_buf)) != NULL) {
                diag << " (cwd=" << cwd_buf << " -> '" << cwd_buf << "/"
                     << file_path.str() << "')";
            }
            program_settings::get_instance()->sys_message(
                program_settings::ERROR_MSG, diag.str());
            return response_data; // Return empty vector if file doesn't exist
        }
        
        // Read file content - each line contains one hexadecimal value
        string line;
        while (getline(response_file, line)) {
            // Trim whitespace from line
            size_t start = line.find_first_not_of(" \t\r\n");
            if (start == string::npos) {
                continue; // Skip empty lines
            }
            size_t end = line.find_last_not_of(" \t\r\n");
            string trimmed_line = line.substr(start, end - start + 1);
            
            // Skip empty lines after trimming
            if (trimmed_line.empty()) {
                continue;
            }
            
            // Skip lines that start with # (comments)
            if (trimmed_line[0] == '#') {
                continue;
            }
            
            // Parse hex value (format: 0xNN or 0XNN)
            unsigned int byte_value = 0;
            bool parsed = false;
            
            if (trimmed_line.length() >= 2 && (trimmed_line.substr(0, 2) == "0x" || trimmed_line.substr(0, 2) == "0X")) {
                // Format: 0xNN
                istringstream hex_stream(trimmed_line);
                if (hex_stream >> hex >> byte_value) {
                    parsed = true;
                }
            } else {
                // Try to parse as hex without 0x prefix
                istringstream hex_stream(trimmed_line);
                if (hex_stream >> hex >> byte_value) {
                    parsed = true;
                }
            }
            
            if (parsed && byte_value <= 0xFF) {
                response_data.push_back(static_cast<byte>(byte_value));
            }
        }
        
        response_file.close();

        // The file opened but yielded no usable bytes. This is reported the
        // same way upstream as a missing file ("No response file found"), so
        // call it out distinctly here.
        if (response_data.empty()) {
            ostringstream diag;
            diag << "Response file '" << file_path.str()
                 << "' opened but contained no usable byte values"
                    " (only comments/blank lines, or values > 0xFF / not hex)";
            program_settings::get_instance()->sys_message(
                program_settings::ERROR_MSG, diag.str());
        }
    } catch (...) {
        // Return empty vector on any error
        response_data.clear();
    }
    
    return response_data;
}

// Resource Manager Initialization
int init_resource_manager(int com_port_number, int baud_rate,
                          byte parity, byte data_bits, byte stop_bits) {
    // NOTE: the dispatch handle, resmgr_attach() and dispatch_context_alloc()
    // are performed in resource_manager_thread() so that the channel is created
    // AND received on by the same thread. Doing them in different threads led to
    // ESRCH ("No such process") on the first dispatch_block().

    // Remember which port this RM will register (/dev/ser<N>)
    rm_state.com_port_number = com_port_number;

    {
        ostringstream m;
        m << "init_resource_manager: port=" << com_port_number
          << " baud=" << baud_rate
          << " parity=" << static_cast<int>(parity)
          << " data_bits=" << static_cast<int>(data_bits)
          << " stop_bits=" << static_cast<int>(stop_bits);
        program_settings::get_instance()->sys_message(program_settings::INFO_MSG, m.str());
    }

    // Initialize mount structure
    iofunc_mount_t *mount = &rm_state.mount;
    mount->conf =IOFUNC_PC_SYNC_IO;
    mount->flags = IOFUNC_MOUNT_32BIT;
    
    // Initialize attribute structure (character device)
    iofunc_attr_init(&rm_state.attr, S_IFCHR | 0666, NULL, NULL);
    rm_state.attr.mount = mount;
    rm_state.attr.nbytes = 0;  // Character device, no file size
    
    // Initialize function tables
    iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &rm_state.connect_funcs,
                     _RESMGR_IO_NFUNCS, &rm_state.io_funcs);
    
    // Set up connect functions
    rm_state.connect_funcs.open = io_open_handler;
    
    // Set up I/O functions
    rm_state.io_funcs.read = io_read_handler;
    rm_state.io_funcs.write = io_write_handler;
    rm_state.io_funcs.devctl = io_devctl_handler;
//TODO confirm no need to use notification
//    rm_state.io_funcs.notify = io_notify_handler;
    rm_state.io_funcs.close_ocb =  io_close_handler;
    
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
    rm_state.line_status = 0;
    
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
    if (parity == 1) {  // PARITY_ODD
        rm_state.termios_settings.c_cflag |= PARENB | PARODD;
    } else if (parity == 2) {  // PARITY_EVEN
        rm_state.termios_settings.c_cflag |= PARENB;
    }
    
    // Apply stop bits
    if (stop_bits == 2) {
        rm_state.termios_settings.c_cflag |= CSTOPB;
    } else {
        rm_state.termios_settings.c_cflag &= ~CSTOPB;
    }
    
    // Initialize synchronization primitives
    pthread_mutex_init(&rm_state.request_mutex, NULL);
    pthread_mutex_init(&rm_state.response_mutex, NULL);
    pthread_cond_init(&rm_state.request_ready, NULL);
    pthread_cond_init(&rm_state.response_ready, NULL);
    pthread_mutex_init(&rm_state.init_mutex, NULL);
    pthread_cond_init(&rm_state.init_cond, NULL);

    // Initialize state
    rm_state.init_done = false;
    rm_state.init_status = -1;
    rm_state.device_open = false;
    rm_state.shutdown = false;
    rm_state.request_pending = false;
    rm_state.response_pending = false;
    rm_state.open_ocb = NULL;
    rm_state.bytes_received = 0;
    rm_state.bytes_sent = 0;
    rm_state.packets_received = 0;
    rm_state.packets_sent = 0;
    
    // Dispatch handle / channel is created in resource_manager_thread()
    rm_state.dpp = NULL;

    return 0;  // Success (channel is created and attached in the RM thread)
}

// I/O Handler: Open
int io_open_handler(resmgr_context_t *ctp, io_open_t *msg, 
                    RESMGR_HANDLE_T *handle, void *extra) {
    iofunc_ocb_t *ocb;
    int status;
    
    // Only one client at a time; EBUSY = another fd has it open (same or other process).
    // Check who has it: pidin fds | grep ser
    if (rm_state.device_open) {
        return EBUSY;  // Resource busy
    }
    
    // Allocate our own OCB. We MUST bind this exact OCB to the connection
    // ourselves so that every later io_read / io_write / io_devctl handler is
    // handed the same pointer we store in rm_state.open_ocb.
    //
    // Do NOT use iofunc_open_default() here: per the QNX docs it internally
    // calls iofunc_ocb_calloc() to allocate its OWN ocb and iofunc_ocb_attach()
    // to bind THAT one to the connection. rm_state.open_ocb would then point at
    // a different, unbound ocb, so every subsequent handler fails the
    // "ocb != rm_state.open_ocb" check and returns EBADF (client sees
    // tcgetattr/read/write fail with "Bad file descriptor").
    ocb = iofunc_ocb_calloc( ctp, &(rm_state.attr));
    if (ocb == NULL) {
        return ENOMEM;
    }

    // Set up open control block
    ocb->ioflag = msg->connect.ioflag;
    ocb->sflag = msg->connect.sflag;
    ocb->offset = 0;

    // Validate the client's open mode against the device attributes.
    status = iofunc_open(ctp, msg, &rm_state.attr, NULL, NULL);
    if (status != EOK) {
        iofunc_ocb_free(ocb);
        return status;
    }

    // Bind OUR ocb to this connection (calls resmgr_open_bind() internally).
    status = iofunc_ocb_attach(ctp, msg, ocb, &rm_state.attr, NULL);
    if (status != EOK) {
        iofunc_ocb_free(ocb);
        return status;
    }

    // Open succeeded and our ocb is now the one bound to the connection.
    rm_state.open_ocb = ocb;
    pthread_mutex_lock(&rm_state.request_mutex);
    rm_state.device_open = true;
    rm_state.request_buffer.clear();
    rm_state.response_buffer.clear();
    rm_state.request_pending = false;
    rm_state.response_pending = false;
    pthread_cond_broadcast(&rm_state.request_ready);  // wake Modbus thread if waiting for client
    pthread_mutex_unlock(&rm_state.request_mutex);

    return EOK;
}

// I/O Handler: Read (sends Modbus responses to client)
int io_read_handler(resmgr_context_t *ctp, io_read_t *msg,
                    iofunc_ocb_t *ocb) {
    int nbytes;
    int status;
    // static: the reply IOV points into this buffer and is sent AFTER this
    // handler returns, so it must outlive the call. Safe because the single
    // resource manager thread is the only caller.
    static metro_device::command_data data_to_send;
    
    // Verify read access permissions
    status = iofunc_read_verify(ctp, msg, ocb, NULL);
    if (status != EOK) {
        return status;
    }
    
    // Check if device is open
    if (!rm_state.device_open || ocb != rm_state.open_ocb) {
        return EBADF;
    }
    
    // Lock response mutex
    pthread_mutex_lock(&rm_state.response_mutex);
    
    // Wait for response data, but only for a bounded time (see
    // COM_EMU_READ_WAIT_MS) so the single RM thread is never blocked forever.
    {
        struct timespec deadline;
        clock_gettime(CLOCK_REALTIME, &deadline);
        deadline.tv_nsec += (long)COM_EMU_READ_WAIT_MS * 1000000L;
        deadline.tv_sec  += deadline.tv_nsec / 1000000000L;
        deadline.tv_nsec %= 1000000000L;

        while (!rm_state.response_pending && rm_state.device_open) {
            if (pthread_cond_timedwait(&rm_state.response_ready,
                                       &rm_state.response_mutex,
                                       &deadline) == ETIMEDOUT) {
                break;
            }
        }
    }
    
    if (!rm_state.device_open) {
        pthread_mutex_unlock(&rm_state.response_mutex);
        return EBADF;
    }

    if (!rm_state.response_pending || rm_state.response_buffer.empty()) {
        pthread_mutex_unlock(&rm_state.response_mutex);
        _IO_SET_READ_NBYTES(ctp, 0);
        return _RESMGR_NPARTS(0);
    }
    
    // Take only what the client asked for; keep the remainder queued so the
    // next read (modbus_srv reads a reply in several pieces) gets the rest.
    nbytes = min(static_cast<int>(rm_state.response_buffer.size()),
                 static_cast<int>(msg->i.nbytes));
    data_to_send.assign(rm_state.response_buffer.begin(),
                        rm_state.response_buffer.begin() + nbytes);
    rm_state.response_buffer.erase(rm_state.response_buffer.begin(),
                                   rm_state.response_buffer.begin() + nbytes);
    rm_state.response_pending = !rm_state.response_buffer.empty();
    
    pthread_mutex_unlock(&rm_state.response_mutex);

    // Set up reply message with IOV
    SETIOV(ctp->iov, &data_to_send[0], nbytes);

    // Report the number of bytes read back to the client. As with write, the
    // client's read() return value comes from the reply status (ctp->status),
    // which _IO_SET_READ_NBYTES sets; assigning msg->i.nbytes does NOT affect
    // it, so the client would otherwise see read() return 0 despite the data.
    _IO_SET_READ_NBYTES(ctp, nbytes);

    // Update statistics
    rm_state.bytes_sent += nbytes;
    rm_state.packets_sent++;

    // Return number of IOV parts
    return _RESMGR_NPARTS(1);
}

// I/O Handler: Write (receives Modbus requests from client)
int io_write_handler(resmgr_context_t *ctp, io_write_t *msg,
                     iofunc_ocb_t *ocb) {
    int nbytes;
    int status;
    metro_device::command_data received_data;
    
    // Verify write access permissions
    status = iofunc_write_verify(ctp, msg, ocb, NULL);
    if (status != EOK) {
        return status;
    }
    
    // Check if device is open
    if (!rm_state.device_open || ocb != rm_state.open_ocb) {
        return EBADF;
    }
    
    nbytes = msg->i.nbytes;
    if (nbytes <= 0) {
        // 0-byte write: report 0 bytes written to the client.
        _IO_SET_WRITE_NBYTES(ctp, 0);
        return _RESMGR_NPARTS(0);
    }
    
    // Read data from the message using MsgRead
    // Data follows the io_write_t structure in the message
    received_data.resize(nbytes);
    int bytes_read = MsgRead(ctp->rcvid, &received_data[0], nbytes, 
                            sizeof(io_write_t));
    
    if (bytes_read < 0) {
        // Error reading message
        msg->i.nbytes = 0;
        return errno;
    }
    
    if (bytes_read < nbytes) {
        // Partial read - resize to actual bytes read
        received_data.resize(bytes_read);
        nbytes = bytes_read;
    }
    
    // A new request makes any unread part of the previous response stale.
    pthread_mutex_lock(&rm_state.response_mutex);
    rm_state.response_buffer.clear();
    rm_state.response_pending = false;
    pthread_mutex_unlock(&rm_state.response_mutex);
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

    // Report the number of bytes written back to the client. The client's
    // write() return value comes from the reply status (ctp->status), which
    // _IO_SET_WRITE_NBYTES sets - assigning msg->i.nbytes does NOT affect it,
    // so the client would otherwise see write() return 0.
    _IO_SET_WRITE_NBYTES(ctp, nbytes);
    return _RESMGR_NPARTS(0);
}

// I/O Handler: Device Control
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
            // Get terminal attributes: copy termios into the reply payload and
            // tell the framework how many data bytes follow the reply header.
            if (nbytes >= static_cast<int>(sizeof(struct termios)) && data_ptr != NULL) {
                memset(&msg->o, 0, sizeof(msg->o));
                msg->o.ret_val = EOK;
                msg->o.nbytes  = sizeof(struct termios);
                memcpy(_DEVCTL_DATA(msg->o), &rm_state.termios_settings,
                       sizeof(struct termios));
                SETIOV(ctp->iov, &msg->o, sizeof(msg->o) + sizeof(struct termios));
                return _RESMGR_NPARTS(1);
            }
            return EINVAL;
            
        case DCMD_CHR_TCSETATTR:
            // Set terminal attributes
            if (nbytes >= static_cast<int>(sizeof(struct termios)) && data_ptr != NULL) {
                struct termios *new_termios = 
                    static_cast<struct termios*>(data_ptr);
                
                // Update internal settings
                memcpy(&rm_state.termios_settings, new_termios, 
                       sizeof(struct termios));
                
                // Extract baud rate
                speed_t ispeed = cfgetispeed(new_termios);
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
                        rm_state.parity = 1;  // PARITY_ODD
                    } else {
                        rm_state.parity = 2;  // PARITY_EVEN
                    }
                } else {
                    rm_state.parity = 0;  // PARITY_DISABLE
                }
                
                // Extract stop bits
                if (new_termios->c_cflag & CSTOPB) {
                    rm_state.stop_bits = 2;
                } else {
                    rm_state.stop_bits = 1;
                }
                
                memset(&msg->o, 0, sizeof(msg->o));
                msg->o.ret_val = EOK;
                msg->o.nbytes  = 0;
                SETIOV(ctp->iov, &msg->o, sizeof(msg->o));
                return _RESMGR_NPARTS(1);
            } else {
                return EINVAL;
            }
            
        case DCMD_CHR_TCFLUSH:
            // Flush input/output buffers
            // Deliberately a no-op. The emulator answers within ~1 ms, so a
            // client flush (modbus_srv flushes before recv) races with the
            // response being queued and used to erase it, leaving the client
            // waiting forever. Stale data is discarded in io_write instead.

            memset(&msg->o, 0, sizeof(msg->o));
            msg->o.ret_val = EOK;
            msg->o.nbytes  = 0;
            SETIOV(ctp->iov, &msg->o, sizeof(msg->o));
            return _RESMGR_NPARTS(1);
            
        case COM_EMU_DCMD_RELEASE:
            // Release device so another process can open. Caller must be current opener; then close(fd).
            pthread_mutex_lock(&rm_state.request_mutex);
            if (ocb == rm_state.open_ocb) {
                rm_state.device_open = false;
                rm_state.open_ocb = NULL;
                rm_state.request_buffer.clear();
                rm_state.request_pending = false;
                pthread_cond_broadcast(&rm_state.request_ready);
            }
            pthread_mutex_unlock(&rm_state.request_mutex);
            pthread_mutex_lock(&rm_state.response_mutex);
            pthread_cond_broadcast(&rm_state.response_ready);
            pthread_mutex_unlock(&rm_state.response_mutex);
            memset(&msg->o, 0, sizeof(msg->o));
            msg->o.ret_val = EOK;
            msg->o.nbytes  = 0;
            SETIOV(ctp->iov, &msg->o, sizeof(msg->o));
            return _RESMGR_NPARTS(1);
            
        case DCMD_CHR_SERCTL:
            // Set modem control lines (e.g. RTS). No physical line exists in the
            // emulator, so just record the requested RTS state so a subsequent
            // DCMD_CHR_LINESTATUS reports it consistently, then acknowledge.
            if (nbytes >= static_cast<int>(sizeof(int)) && data_ptr != NULL) {
                int ctl = *static_cast<int*>(data_ptr);
                if (ctl & _CTL_RTS_CHG) {
                    if (ctl & _CTL_RTS) {
                        rm_state.line_status |= _LINESTATUS_SER_RTS;
                    } else {
                        rm_state.line_status &= ~_LINESTATUS_SER_RTS;
                    }
                }
            }
            memset(&msg->o, 0, sizeof(msg->o));
            msg->o.ret_val = EOK;
            msg->o.nbytes  = 0;
            SETIOV(ctp->iov, &msg->o, sizeof(msg->o));
            return _RESMGR_NPARTS(1);

        case DCMD_CHR_LINESTATUS:
            // Report emulated modem line status (RTS bit tracked via SERCTL).
            if (nbytes >= static_cast<int>(sizeof(int)) && data_ptr != NULL) {
                memset(&msg->o, 0, sizeof(msg->o));
                msg->o.ret_val = EOK;
                msg->o.nbytes  = sizeof(int);
                *static_cast<int*>(_DEVCTL_DATA(msg->o)) = rm_state.line_status;
                SETIOV(ctp->iov, &msg->o, sizeof(msg->o) + sizeof(int));
                return _RESMGR_NPARTS(1);
            }
            return EINVAL;

        default:
            // Unknown command, try default handler
            status = iofunc_devctl_default(ctp, msg, ocb);
            break;
    }
    
    return status;
}

// I/O Handler: Close
int io_close_handler(resmgr_context_t *ctp, void *reserved,
                     iofunc_ocb_t *ocb) {
    // Clear open state
    pthread_mutex_lock(&rm_state.request_mutex);
    
    // Only clear if this is the open OCB
    if (ocb == rm_state.open_ocb) {
        rm_state.device_open = false;
        rm_state.open_ocb = NULL;
        rm_state.request_buffer.clear();
        rm_state.request_pending = false;
        
        // Wake up any waiting threads
        pthread_cond_broadcast(&rm_state.request_ready);
    }
    
    pthread_mutex_unlock(&rm_state.request_mutex);
    
    // Also wake up response waiters
    pthread_mutex_lock(&rm_state.response_mutex);
    pthread_cond_broadcast(&rm_state.response_ready);
    pthread_mutex_unlock(&rm_state.response_mutex);
    
    // Call default close handler
    return iofunc_close_ocb_default(ctp, reserved, ocb);
}

//TODO confirm no need to use notification
// I/O Handler: Notify
// int io_notify_handler(resmgr_context_t *ctp, io_notify_t *msg,
//                      iofunc_ocb_t *ocb) {
//    // Handle notification events
//    return iofunc_notify_default(ctp, msg, ocb);
//}

// Resource Manager Thread
// Creates the dispatch/channel, attaches /dev/ser<N>, allocates the context and
// runs the message loop - all in this single thread so the channel is owned by
// the thread that receives on it (fixes ESRCH on the first dispatch_block()).
void* resource_manager_thread(void *arg) {
    program_settings* sett_obj = program_settings::get_instance();
    dispatch_context_t *ctp = NULL;
    int attach_ok = 0;

    sett_obj->sys_message(program_settings::INFO_MSG,
                         "RM thread: step 1 - dispatch_create()");

    // Create dispatch structure (channel) in this thread
    rm_state.dpp = dispatch_create();
    if (rm_state.dpp != NULL) {
        sett_obj->sys_message(program_settings::INFO_MSG,
                             "RM thread: step 1 ok - dispatch handle created");

        resmgr_attr_t resmgr_attr;
        memset(&resmgr_attr, 0, sizeof(resmgr_attr));
        resmgr_attr.nparts_max = 1;      // Number of IOV parts for replies
        resmgr_attr.msg_max_size = 2048; // Maximum message size
        resmgr_attr.flags = 0;

        ostringstream device_path;
        device_path << "/dev/ser" << rm_state.com_port_number;

        {
            ostringstream m;
            m << "RM thread: step 2 - resmgr_attach(" << device_path.str()
              << ") nparts_max=" << resmgr_attr.nparts_max
              << " msg_max_size=" << resmgr_attr.msg_max_size;
            sett_obj->sys_message(program_settings::INFO_MSG, m.str());
        }

        errno = 0;
        int resmgr_id = resmgr_attach(rm_state.dpp, &resmgr_attr,
                                      device_path.str().c_str(),
                                      _FTYPE_ANY, 0,
                                      &rm_state.connect_funcs,
                                      &rm_state.io_funcs,
                                      &rm_state.attr);
        int attach_errno = errno;

        {
            ostringstream m;
            m << "RM thread: step 2 result - resmgr_attach(" << device_path.str()
              << ") returned " << resmgr_id;
            if (resmgr_id == -1) {
                m << " errno=" << attach_errno << " (" << strerror(attach_errno) << ")";
            }
            sett_obj->sys_message(
                resmgr_id == -1 ? program_settings::ERROR_MSG : program_settings::INFO_MSG,
                m.str());
        }

        if (resmgr_id != -1) {
            sett_obj->sys_message(program_settings::INFO_MSG,
                                 "RM thread: step 3 - dispatch_context_alloc()");
            errno = 0;
            ctp = dispatch_context_alloc(rm_state.dpp);
            if (ctp != NULL) {
                attach_ok = 1;
                sett_obj->sys_message(program_settings::INFO_MSG,
                                     "RM thread: step 3 ok - dispatch context allocated");
            } else {
                int alloc_errno = errno;
                ostringstream m;
                m << "RM thread: step 3 FAILED - dispatch_context_alloc() returned NULL"
                  << " errno=" << alloc_errno << " (" << strerror(alloc_errno) << ")";
                sett_obj->sys_message(program_settings::ERROR_MSG, m.str());
            }
        }

        if (!attach_ok) {
            sett_obj->sys_message(program_settings::ERROR_MSG,
                                 "RM thread: attach/alloc failed - destroying dispatch handle");
            dispatch_destroy(rm_state.dpp);
            rm_state.dpp = NULL;
        }
    } else {
        int create_errno = errno;
        ostringstream m;
        m << "RM thread: step 1 FAILED - dispatch_create() returned NULL"
          << " errno=" << create_errno << " (" << strerror(create_errno) << ")";
        sett_obj->sys_message(program_settings::ERROR_MSG, m.str());
    }

    // Report attach result to main (handshake)
    sett_obj->sys_message(program_settings::INFO_MSG,
                         attach_ok ? "RM thread: signalling main - init OK"
                                   : "RM thread: signalling main - init FAILED");
    pthread_mutex_lock(&rm_state.init_mutex);
    rm_state.init_status = attach_ok ? 0 : -1;
    rm_state.init_done = true;
    pthread_cond_signal(&rm_state.init_cond);
    pthread_mutex_unlock(&rm_state.init_mutex);

    if (!attach_ok) {
        return NULL;  // main will observe init_status and exit
    }

    sett_obj->sys_message(program_settings::INFO_MSG,
                         "Resource manager thread started - entering dispatch_block() loop");

    // Main message loop - continue while dispatch context is valid
    while (ctp != NULL) {
        // Block waiting for messages
        ctp = dispatch_block(ctp);
        
        if (ctp == NULL) {
             int err = errno;
            // Error or shutdown
                sett_obj->sys_message(program_settings::INFO_MSG,
                         (string("Resource manager thread : ctp == NULL errno : ") + string(strerror(err))).c_str());
            // Diagnostic: channel no longer exists (ESRCH) - log context
            ostringstream diag;
            diag.str("");
            diag << "RM thread tid=" << static_cast<unsigned long>(pthread_self())
                 << " dpp=" << (rm_state.dpp ? "non-NULL" : "NULL")
                 << " device_open=" << (rm_state.device_open ? "yes" : "no");
            sett_obj->sys_message(program_settings::ERROR_MSG, diag.str());
            if (err == ESRCH) {
                sett_obj->sys_message(program_settings::ERROR_MSG,
                    "ESRCH: channel destroyed - check if another process registered the same path (/dev/serN) or path was unregistered");
            }
            break;
        }
        
        // Handle message
        dispatch_handler(ctp);
    }
    
    sett_obj->sys_message(program_settings::INFO_MSG,
                         "Resource manager thread exiting");
    
    // Cleanup
    if (ctp != NULL) {
        dispatch_context_free(ctp);
    }
    return NULL;
}

// Modbus Handler Thread
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
                // No client has device open - wait until one opens or process shuts down
                while (!rm_state.device_open && !rm_state.shutdown) {
                    pthread_cond_wait(&rm_state.request_ready, 
                                     &rm_state.request_mutex);
                }
                if (rm_state.shutdown) {
                    pthread_mutex_unlock(&rm_state.request_mutex);
                    break;  // Process exiting
                }
                continue;  // New client opened, go wait for request
            }
            
            // Copy request
            telemetr_request = rm_state.request_buffer;
            rm_state.request_pending = false;
            
            pthread_mutex_unlock(&rm_state.request_mutex);
            
            // Validate request size
            if (telemetr_request.size() < 
                static_cast<metro_device::command_data::size_type>(
                    program_settings::MODBUS_REQUEST_HEADER_SIZE)) {
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
            
            // Emulate a real device: turnaround time, then the time the reply
            // needs on the wire (10 bits per byte: start + 8 data + stop).
            // Done here, in the Modbus thread, so the resource manager thread
            // keeps serving io_write/io_read meanwhile.
            {
                long wire_us = (long)telemetr_answer.size() * 10L * 1000000L /
                               baud_to_bps(rm_state.baud_rate);
                long delay_us = (long)g_reply_turnaround_ms * 1000L + wire_us;
                usleep(delay_us);
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

// Cleanup Function
void cleanup_resource_manager() {
    program_settings::get_instance()->sys_message(program_settings::INFO_MSG,
        "cleanup_resource_manager called (dispatch/channel will be destroyed)");
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
    pthread_mutex_destroy(&rm_state.init_mutex);
    pthread_cond_destroy(&rm_state.init_cond);
    
    // Destroy dispatch structure
    if (rm_state.dpp != NULL) {
        dispatch_destroy(rm_state.dpp);
        rm_state.dpp = NULL;
    }
}

// Main Function
int main(int argc, const char *argv[]) {
    program_settings* sett_obj = program_settings::get_instance();

    // Resolve where response files live. Device directories are expected next
    // to the executable, so response files are read from
    // <exe_dir>/<device>/<command>.txt regardless of the current working
    // directory. If the executable directory can't be resolved, this stays
    // empty and read_response_from_file() falls back to the cwd-relative
    // "gcc_ntox86/" prefix.
    g_response_base_dir = resolve_exe_dir(argc > 0 ? argv[0] : NULL);
    {
        ostringstream m;
        if (!g_response_base_dir.empty()) {
            m << "Response file base directory (executable location): "
              << g_response_base_dir
              << "  (expected files: " << g_response_base_dir << "/<device>/<command>.txt)";
        } else {
            m << "Could not resolve executable directory; response files will be "
                 "read relative to the working directory (gcc_ntox86/<device>/<command>.txt)";
        }
        sett_obj->sys_message(program_settings::INFO_MSG, m.str());
    }
    pthread_t rm_thread, modbus_thread;
    pthread_attr_t attr;
    int pthread_creating_result;
    int com_port_number = 1;  // Default
    int baud_rate = B9600;  // Default baud rate
    
    // Parse command line arguments (simple version)
    if (argc > 1) {
        com_port_number = atoi(argv[1]);
        if (com_port_number < 1 || com_port_number > 4) {
            sett_obj->sys_message(program_settings::ERROR_MSG,
                                 "Invalid COM port number (must be 1-4)");
            return 1;
        }
    }
    if (argc > 2) {
        baud_rate = atoi(argv[2]);
        // Convert to termios baud rate constant
        switch (baud_rate) {
            case 9600: baud_rate = B9600; break;
            case 19200: baud_rate = B19200; break;
            case 38400: baud_rate = B38400; break;
            case 57600: baud_rate = B57600; break;
            case 115200: baud_rate = B115200; break;
            default:
                sett_obj->sys_message(program_settings::ERROR_MSG,
                                     "Unsupported baud rate, using 9600");
                baud_rate = B9600;
        }
    }
    
    if (argc > 3) {
        g_reply_turnaround_ms = atoi(argv[3]);
        if (g_reply_turnaround_ms < 0) g_reply_turnaround_ms = 0;
    }

    // Initialize pthread attributes
    if (pthread_attr_init(&attr) != EOK) {
        sett_obj->sys_message(program_settings::ERROR_MSG,
                             "Can't pthread_attr_init(&attr)");
        return 1;
    }
    
    // Use joinable threads so main blocks until threads actually exit before cleanup
    if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE) != EOK) {
        sett_obj->sys_message(program_settings::ERROR_MSG,
                             "Can't pthread_attr_setdetachstate");
        return 1;
    }
    
    // Initialize resource manager (non-channel setup: tables, termios, sync, state)
    if (init_resource_manager(com_port_number, baud_rate,
                             0,  // PARITY_DISABLE
                             8,  // DATA_BITS_8
                             1) != 0) {  // STOP_BITS_1
        sett_obj->sys_message(program_settings::ERROR_MSG,
                             "Failed to initialize resource manager");
        return 1;
    }

    // Start resource manager thread (it creates the channel and attaches /dev/serN)
    pthread_creating_result = pthread_create(&rm_thread, &attr,
                                            resource_manager_thread, NULL);
    if (pthread_creating_result != EOK) {
        string message("Failed to create resource manager thread: ");
        message += strerror(pthread_creating_result);
        sett_obj->sys_message(program_settings::ERROR_MSG, message);
        cleanup_resource_manager();
        return 1;
    }

    // Wait for the RM thread to report whether resmgr_attach() succeeded
    sett_obj->sys_message(program_settings::INFO_MSG,
                         "main: waiting for RM thread attach handshake");
    pthread_mutex_lock(&rm_state.init_mutex);
    while (!rm_state.init_done) {
        pthread_cond_wait(&rm_state.init_cond, &rm_state.init_mutex);
    }
    int rm_init_status = rm_state.init_status;
    pthread_mutex_unlock(&rm_state.init_mutex);

    {
        ostringstream m;
        m << "main: RM attach handshake received, init_status=" << rm_init_status;
        sett_obj->sys_message(program_settings::INFO_MSG, m.str());
    }

    if (rm_init_status != 0) {
        sett_obj->sys_message(program_settings::ERROR_MSG,
                             "Resource manager failed to attach /dev/ser - exiting");
        pthread_join(rm_thread, NULL);
        cleanup_resource_manager();
        return 1;
    }

    ostringstream info_msg;
    info_msg << "Resource manager initialized for /dev/ser" << com_port_number;
    sett_obj->sys_message(program_settings::INFO_MSG, info_msg.str());

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
    
    // Main loop - wait for threads (joinable so we don't run cleanup while RM is in dispatch_block)
    sett_obj->sys_message(program_settings::INFO_MSG,
                         "main: waiting for resource manager thread to exit");
    pthread_join(rm_thread, NULL);
    sett_obj->sys_message(program_settings::INFO_MSG,
                         "main: RM thread exited, signalling Modbus handler to exit");
    // Wake modbus thread so it can exit (shutdown flag + device_open false)
    pthread_mutex_lock(&rm_state.request_mutex);
    rm_state.shutdown = true;
    rm_state.device_open = false;
    pthread_cond_broadcast(&rm_state.request_ready);
    pthread_mutex_unlock(&rm_state.request_mutex);
    pthread_mutex_lock(&rm_state.response_mutex);
    pthread_cond_broadcast(&rm_state.response_ready);
    pthread_mutex_unlock(&rm_state.response_mutex);
    sett_obj->sys_message(program_settings::INFO_MSG,
                         "main: waiting for Modbus handler thread");
    pthread_join(modbus_thread, NULL);
    sett_obj->sys_message(program_settings::INFO_MSG,
                         "main: both threads exited, calling cleanup");
    // Cleanup (channel still exists; destroy dispatch and sync objects)
    cleanup_resource_manager();
    
    return 0;
}

