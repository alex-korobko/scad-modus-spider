using namespace std;

#include <errno.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>

//for usleep
#include <unistd.h>

//for devctl
#include <devctl.h>
#include <sys/dcmd_chr.h>

#include <iostream.h>

//stl headers
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <sstream>

//local headers
#include "../system_settings_classes/defines.h"
#include "comport_exception.h"
#include "comport.h"

comport::comport (
    		int  port_number,
        int baud_rate,
	    byte parity,
	    byte data_bits,
	    byte stop_bits,
	    int delay_useconds=0,
        float comport_koeff=1.0)  throw (comport_exception)  :
	    		m_port_handle(-1),
	    	    m_dev_port("/dev/ser"),
	    	    m_baud_rate(baud_rate),
		    m_parity(parity),
			m_data_bits(data_bits),
		    m_stop_bits(stop_bits),
		    m_delay_useconds(delay_useconds),
            m_comport_koeff(comport_koeff) {

	struct termios termios_param;		  
	ostringstream exception_message;	     

	exception_message<<port_number;
	m_dev_port+=exception_message.str();

	exception_message.str("");
	if (port_number < 1 || port_number > 4) {
	 exception_message<<"Wrong com port number "<<m_dev_port;
	 throw comport_exception(exception_message.str());
	}

	if (baud_rate!=BAUD_9600 &&
	baud_rate!=BAUD_19200 &&
    baud_rate!=BAUD_115200 )  {
		exception_message<<"Wrong baud rate value "<<baud_rate;
	    throw comport_exception(exception_message.str());
    }

	if ((m_port_handle = open(m_dev_port.c_str(), 
										O_RDWR))<0) {
	exception_message<<"Can`t open com port "<<m_dev_port<<" : "<<strerror(errno);
	 throw comport_exception(exception_message.str());
	 }

	if (tcgetattr(m_port_handle, &termios_param)!=0) {
	 exception_message<<"Can`t get the current terminal control settings for "<<m_dev_port<<" : "<<strerror(errno);
	 if (close(m_port_handle)!=0)
	 	 exception_message<<"  Can`t close "<<m_dev_port<<" : "<<strerror(errno);
	 m_port_handle=-1;
	 throw comport_exception(exception_message.str());
	}

	if (cfsetispeed(&termios_param, baud_rate)!= 0){
	 exception_message<<"Can`t set input speed "<<m_dev_port<<" : "<<strerror(errno);
	 if (close(m_port_handle)!=0)
	 	 exception_message<<"  Can`t close "<<m_dev_port<<" : "<<strerror(errno);
	 m_port_handle=-1;
	 throw comport_exception(exception_message.str());
	}

	if (cfsetospeed(&termios_param, baud_rate) != 0){
	 exception_message<<"Can`t set output speed "<<m_dev_port<<" : "<<strerror(errno);
	 if (close(m_port_handle)!=0)
	 	 exception_message<<"  Can`t close "<<m_dev_port<<" : "<<strerror(errno);
	 m_port_handle=-1;
	 throw comport_exception(exception_message.str());
	}

    termios_param.c_lflag &= ~( ECHO|ECHOE|ECHOK|ECHOKE|ECHOCTL|ECHONL|ICANON|ISIG);
	termios_param.c_cflag &= ~CLOCAL;
	termios_param.c_iflag &= ~IMAXBEL;
	termios_param.c_oflag &= ~ONLCR;
	
	if (data_bits==DATA_BITS_5) {
			termios_param.c_cflag |= CS5;
		} else if ( data_bits==DATA_BITS_6) {
			termios_param.c_cflag |= CS6;	
		} else if (data_bits==DATA_BITS_7) {
			termios_param.c_cflag |=CS7;
		} else if (data_bits==DATA_BITS_8) {
			termios_param.c_cflag |= CS8;
		}else  {
		exception_message<<"Wrong data bits count "<<static_cast<int>(data_bits);
	 	 if (close(m_port_handle)!=0)
		 	 exception_message<<"  Can`t close "<<m_dev_port<<" : "<<strerror(errno);
		 m_port_handle=-1;
	    throw comport_exception(exception_message.str());
    }

	if (stop_bits==STOP_BITS_1) {
			termios_param.c_cflag &= ~CSTOPB;
  	    } else if (stop_bits==STOP_BITS_2 ) {
			termios_param.c_cflag |= CSTOPB;
  	    } else {
		exception_message<<"Wrong stop bits count "<<stop_bits;
	 	 if (close(m_port_handle)!=0)
		 	 exception_message<<"  Can`t close "<<m_dev_port<<" : "<<strerror(errno);
		 m_port_handle=-1;
	    throw comport_exception(exception_message.str());
    }

	if (parity==PARITY_DISABLE) {
			termios_param.c_cflag &=~PARENB;
  	    } else if (parity==PARITY_ODD) {
 			termios_param.c_cflag |=(PARENB | PARODD);
  	    } else if (parity==PARITY_EVEN) {
		  	termios_param.c_cflag |=(PARENB | ~PARODD);
  	    } else {
		exception_message<<"Wrong stop parity value "<<parity;
 		 if (close(m_port_handle)!=0)
	 		 exception_message<<"  Can`t close "<<m_dev_port<<" : "<<strerror(errno);
		 m_port_handle=-1;
	    throw comport_exception(exception_message.str());
	  }
	
	if (tcsetattr(m_port_handle, TCSANOW , &termios_param)!=0){
	 exception_message<<"Can`t set the current terminal control settings for "<<m_dev_port<<" : "<<strerror(errno);
	 if (close(m_port_handle)!=0)
	 	 exception_message<<"  Can`t close "<<m_dev_port<<" : "<<strerror(errno);
	 m_port_handle=-1;
	 throw comport_exception(exception_message.str());
	}

    if (	tcflush(m_port_handle, TCIOFLUSH)<0) {
		exception_message<<"Can`t flush io streams "<<strerror(errno);
		 if (close(m_port_handle)!=0)
	 		 exception_message<<"  Can`t close "<<m_dev_port<<" : "<<strerror(errno);
		m_port_handle =-1;		
		throw comport_exception(exception_message.str());
    };
};

comport::~comport() throw (comport_exception){
	ostringstream exception_message;	     
		if (m_port_handle>0) {
			if (close(m_port_handle)<0) {
			exception_message<<"Can`t close "<<m_dev_port<<" : "<<strerror(errno);
			throw comport_exception(exception_message.str());
			};
		}
};

void comport::send(comport_data_block data_to_send) throw (comport_exception){
	ostringstream exception_message;
	int count_of_sended_bytes;
	const byte parity_bits_delay_koeff=3; //delay for parity bits, so on $-)
	useconds_t echo_interval;

	//impossible
	if (m_port_handle < 0) {
		exception_message<<"Port "<<m_dev_port<<" not initialized";
		throw comport_exception(exception_message.str());
	};

	count_of_sended_bytes=write(m_port_handle,
                                                     &data_to_send[0],
                                                     data_to_send.size());

	if (static_cast<comport_data_block::size_type >(count_of_sended_bytes)!=data_to_send.size()) {
		exception_message<<"Can`t  write "<<data_to_send.size()<<" bytes to "<<m_dev_port;
		if (count_of_sended_bytes<0) 
				exception_message<<" : "<<strerror(errno);
		throw comport_exception(exception_message.str());
	};

	if (m_delay_useconds==0) {
	   echo_interval=  static_cast<useconds_t> (
                                  (m_data_bits+m_stop_bits+parity_bits_delay_koeff)*1000000
                                  *data_to_send.size()*m_comport_koeff/m_baud_rate
                                   );
	} else {
	echo_interval=m_delay_useconds;
	};

	usleep(echo_interval);
};
/*
void comport::flush_input_buffer() throw (comport_exception){

    if (tcflush(m_port_handle, TCIFLUSH)<0) {
                ostringstream exception_message;
		exception_message<<"Can`t flush input stream "<<strerror(errno);
		 if (close(m_port_handle)!=0)
	 		 exception_message<<"  Can`t close "<<m_dev_port<<" : "<<strerror(errno);
		m_port_handle =-1;		
		throw comport_exception(exception_message.str());
    };
};

void comport::flush_output_buffer() throw (comport_exception){
    if (tcflush(m_port_handle, TCOFLUSH)<0) {
                ostringstream exception_message;
		exception_message<<"Can`t flush output stream "<<strerror(errno);
		 if (close(m_port_handle)!=0)
	 		 exception_message<<"  Can`t close "<<m_dev_port<<" : "<<strerror(errno);
		m_port_handle =-1;		
		throw comport_exception(exception_message.str());
    };
};

void comport::flush_input_output_buffer() throw (comport_exception){
    if (tcflush(m_port_handle, TCIOFLUSH)<0) {
                ostringstream exception_message;
		exception_message<<"Can`t flush input and output streams "<<strerror(errno);
		 if (close(m_port_handle)!=0)
	 		 exception_message<<"  Can`t close "<<m_dev_port<<" : "<<strerror(errno);
		m_port_handle =-1;		
		throw comport_exception(exception_message.str());
    };
};
*/


    int  comport::recv(comport::comport_data_block &buffer_to_recieve,
                     int bytes_count,
	                 bool flush_io_buffers=false) 
                      throw (comport_exception){
    comport_data_block recived_data(bytes_count), all_recieved_data;
   comport_data_block::iterator iter_on_received_data;
	ostringstream exception_message;	     
	int  bytes_readed, bytes_readed_summary=0;

	//impossible
	if (m_port_handle < 0) {
		exception_message<<"Port "<<m_dev_port<<" not initialized";
		throw comport_exception(exception_message.str());
	};
    if (flush_io_buffers)
     if (tcflush(m_port_handle, TCIOFLUSH)<0) {
	exception_message<<"Can`t flush input and output streams "<<strerror(errno);
	 if (close(m_port_handle)!=0)
 		exception_message<<"  Can`t close "<<m_dev_port<<" : "<<strerror(errno);
                m_port_handle =-1;		
		throw comport_exception(exception_message.str());
    };


  while(bytes_readed_summary<bytes_count) {
    bytes_readed = readcond(m_port_handle,
                            &recived_data[0],
                            bytes_count,
                            bytes_count,
                            3, //timeout  if 3*1/10 sec (300 mks)
                            6); //expire if 6*1/10 sec (600 mks)

    	if (bytes_readed<0) {
 		exception_message<<"Can`t  read from "<<m_dev_port<<" : "<<strerror(errno);
		throw comport_exception(exception_message.str());
    	};
       if (bytes_readed==0) break;

         bytes_readed_summary+=bytes_readed;
         iter_on_received_data=recived_data.begin();
         advance(iter_on_received_data, bytes_readed);

         buffer_to_recieve.insert(buffer_to_recieve.end(), recived_data.begin() , iter_on_received_data);
    };

     if (bytes_readed_summary!=bytes_count) {
 		exception_message<<"Too many bytes recieved: must be "<<bytes_count<<" but "<<bytes_readed_summary;
		throw comport_exception(exception_message.str());
     };

    return bytes_readed_summary;
};    


    
void comport::set_rts_state (byte rts_state) throw (comport_exception) {
	ostringstream exception_message;	     
	int data, ret_val;

	//impossible
	if (m_port_handle < 0) {
		exception_message<<"Port "<<m_dev_port<<" not initialized";
		throw comport_exception(exception_message.str());
	};

	if (rts_state==RTS_SET_1) {
		data = _CTL_RTS_CHG  | _CTL_RTS;
	} else if (rts_state==RTS_SET_0) {
		data = _CTL_RTS_CHG  | 0;
	} else {
		exception_message<<"Wrong RTS state "<<static_cast<int>(rts_state)<<" for "<<m_dev_port<<" must be com_port::RTS_SET_1 or com_port::RTS_SET_0";
		throw comport_exception(exception_message.str());
	};

	ret_val	 = devctl (m_port_handle, 
					DCMD_CHR_SERCTL,
					&data,
					sizeof(data),
					NULL);
					
	if (ret_val!=EOK) {
		exception_message<<"Can`t set RTS by devctl : "<<strerror(ret_val);
		throw comport_exception(exception_message.str());
	};
	
	};


	byte comport::get_rts_state () throw (comport_exception){
	ostringstream exception_message;	     
	int data = 0, ret_val;

	//impossible
	if (m_port_handle < 0) {
		exception_message<<"Port "<<m_dev_port<<" not initialized";
		throw comport_exception(exception_message.str());
	};
	
	ret_val	 = devctl (m_port_handle, 
					DCMD_CHR_LINESTATUS,
					&data,
					sizeof(data),
					NULL);
					
	if (ret_val!=EOK) {
		exception_message<<"Can`t get DCMD_CHR_LINESTATUS by devctl : "<<strerror(ret_val);
		throw comport_exception(exception_message.str());
	};

	if ((data & _LINESTATUS_SER_RTS)!=0) {
		return RTS_SET_1;
	 } else  {
		return RTS_SET_0;
	  };
	};

