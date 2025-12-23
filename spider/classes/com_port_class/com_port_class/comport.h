#ifndef __COMPORT__
#define __COMPORT__

class comport {
private:
    typedef vector<byte> comport_data_block;

    int m_port_handle;
    string m_dev_port;
    int m_baud_rate;
    byte m_parity;
    byte m_data_bits;
    byte m_stop_bits;
    int m_delay_useconds;
    float m_comport_koeff;
public:
    comport (
            int port_number,
            int baud_rate,
	    byte parity,
	    byte data_bits,
	    byte stop_bits,
	    int delay_useconds=0,
        float comport_koeff=1.0)  throw (comport_exception);

    virtual ~comport() throw (comport_exception);

    void send(comport_data_block data_to_send , 
					bool flush_io_buffers_after_send = false,
					unsigned int rts_delay = 5000) throw (comport_exception);
    int recv( comport::comport_data_block &buffer_to_recieve,
                   int bytes_count,
                   bool flush_io_buffers=false,
				   unsigned int recieve_timeout = 10) throw (comport_exception);

    void set_rts_state (byte rts_state) throw (comport_exception);
    byte get_rts_state () throw (comport_exception);
/*
    void flush_input_buffer() throw (comport_exception);
    void flush_output_buffer() throw (comport_exception);
    void flush_input_output_buffer() throw (comport_exception);
*/	
        enum{ BAUD_9600=9600,
              BAUD_19200=19200,
              BAUD_115200=115200};

        enum { STOP_BITS_1=1,
               STOP_BITS_2,
               DATA_BITS_5=5,
               DATA_BITS_6,
               DATA_BITS_7, 
               DATA_BITS_8,
               PARITY_DISABLE, 
               PARITY_ODD,
               PARITY_EVEN,
               RTS_SET_1,
               RTS_SET_0};
};
#endif
