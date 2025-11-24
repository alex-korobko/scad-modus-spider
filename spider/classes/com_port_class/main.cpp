using namespace std;

#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <fcntl.h>

#include <iostream.h>

#include <vector>
#include <string>
#include <map>
#include <functional>
#include <sstream>

#include  "test_cond.h"
#include  "defines.h"

#include "comport_exception.h"
#include "comport.h"

#ifdef __UNIT_TESTING__
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>

#include "test_comport.h"
#include "all_tests.h"
#endif

byte g_CRCHi[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40
};

byte g_CRCLo[] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
0x40
};


word crc(vector<byte> buffer)
{
	byte highCRC = 0xFF;
	byte lowCRC = 0xFF;
	word	 index;
	
	if ( buffer.empty()) return(0);
	
	for (vector<byte>::iterator iter= buffer.begin();
			iter!= buffer.end();
			iter++)
	{
		index = highCRC ^ *iter;
		highCRC = lowCRC ^ g_CRCHi[index];
		lowCRC = g_CRCLo[index];
	}

	return (highCRC << 8 | lowCRC);
};

// splitting and merging per bytes -  very dangerous templates using
template<typename T> 
     vector<byte> bytes_of_type (T value_to_splitting) {
					vector<byte> ret_val(sizeof(T));

					//moving bytes from types
						vector<byte>::size_type size = ret_val.size();
						for(int i =0; i<size; i++){
						ret_val[i]=((value_to_splitting & (0x00FF<<i*8))>>i*8);					
					};

					//deleting left side zeros
					while (ret_val.size()>1 &&
								ret_val.back()==0) {
								ret_val.pop_back();
					};

				return  ret_val;
	};

template<typename T>
    T type_from_bytes (vector<byte> bytes_to_merging){
    T ret_val=0;
    vector<byte>::size_type count=__min(bytes_to_merging.size(), sizeof(T));
    for (vector<byte>::size_type i=0; i<count; i++){
        ret_val |=(bytes_to_merging[i] << (i*8) );
    };
    return  ret_val;
};
																    

vector<vector<byte> > load_commands(string file_name) throw (comport_exception) {
FILE*		file;
int lines_count, integer_value;
vector<char> file_line(255), byte_of_command;
vector<byte> command;
vector<vector<byte> > commands;
string line_of_file, command_parameter("[command]");
ostringstream exception_message;	     
	
file = fopen(file_name.c_str(), "r");
if (file==NULL){
	exception_message<<"Can`t open file "<<file_name.c_str()<<" : "<<strerror(errno);
	 throw comport_exception(exception_message.str());
}

lines_count=0;
while (fgets	(&file_line[0], file_line.size(), file)!=NULL){
		lines_count++;
		if (file_line.empty()) continue;	

		line_of_file=&file_line[0];

		if (line_of_file.empty()) {
				continue;
			} else if (line_of_file[line_of_file.size()-1]=='\n') {
							line_of_file.erase(--line_of_file.end());
						} else { //if (line_of_file[line_of_file
							string mock_string;
							 while (fgets	(&file_line[0], file_line.size(), file)!=NULL){
							 		if (file_line.empty()) continue;	
									mock_string=&file_line[0];
									if (mock_string.empty()) {
										break;
										} else if (mock_string[mock_string.size()-1]=='\n') {
											break;
										};
							 };	 //while (fgets	(&file_line
						}; // else if (line_of_file[line_of_file
		
		if (line_of_file.compare(command_parameter)==0) {
                 while (fgets	(&file_line[0], file_line.size(), file)!=NULL){
                      lines_count++;
                      if (file_line.empty()) continue;	
                      line_of_file=&file_line[0];

                      if (!line_of_file.empty() &&
                      line_of_file[line_of_file.size()-1]=='\n') line_of_file.erase(--line_of_file.end());
                      
                      if (line_of_file.empty()) continue;
					   if (line_of_file.compare(command_parameter)==0) {
                           commands.push_back(command);
                           command.clear();
                           continue;
                       } else {
                          for (int i=0; i<line_of_file.size(); i++){	
                             if (line_of_file[i]=='#') {
                                 break;
                             } else if (line_of_file[i]==' ' ||
                                           line_of_file[i]=='\t') {
                                 continue;
                             } else if ( isdigit(line_of_file[i])!=0 ) {
                                 byte_of_command.push_back(line_of_file[i]);
                             }else {
                                 exception_message<<"In file "<<file_name<<" unrecognized sumbol \""<<line_of_file[i]<<"\" on line "<<lines_count;
                                 if (fclose(file)!=0)
                                 exception_message<<" Can`t close file "<<file_name<<" : "<<strerror(errno);
                                 throw comport_exception(exception_message.str());
                            		};
                            }; // for (int i=0; i<line_of_file.size(); i++)
					
					  if (byte_of_command.empty())  continue;
                      integer_value = atoi(&byte_of_command[0]);

					 if (integer_value<0 ||
					 	integer_value>255) {
                                 exception_message<<"In file "<<file_name<<" uncasted to byte value 0x"<<&byte_of_command[0]<<" on line "<<lines_count;
                                 if (fclose(file)!=0)
                                   exception_message<<" Can`t close file "<<file_name<<" : "<<strerror(errno);
                                 throw comport_exception(exception_message.str());
					 	}; //if (integer_value<0 ||
					  byte_of_command.clear();
                      command.push_back(static_cast<byte>(integer_value));
					};  //if (line_of_file.compare(command_parameter)==0)
				}; //while (fgets	(&file_line[0], file_line.size(), file)!=NULL)
				
					  if (!byte_of_command.empty()){
                               integer_value = atoi(&byte_of_command[0]);

                             if (integer_value<0 ||
                                  integer_value>255) {
                                     exception_message<<"In file "<<file_name<<" uncasted to byte value 0x"<<&byte_of_command[0]<<" on line "<<lines_count;
                                      if (fclose(file)!=0)
                                         exception_message<<" Can`t close file "<<file_name<<" : "<<strerror(errno);
                                      throw comport_exception(exception_message.str());
                                    };
                              byte_of_command.clear();
                              command.push_back(static_cast<byte>(integer_value));
					}; //if (!byte_of_command.empty())
				
                  commands.push_back(command);
                   command.clear();

				
		} else {
			for (int i=0; i<line_of_file.size(); i++){	
				if (line_of_file[i]=='#') {
						break;
					} else if (line_of_file[i]==' ' ||
								  line_of_file[i]=='\t') {
						  continue;
					} else {
						exception_message<<"In file "<<file_name<<" unrecognized sumbol \""<<line_of_file[i]<<"\" on line "<<lines_count;
						if (fclose(file)!=0)
							exception_message<<" Can`t close file "<<file_name<<" : "<<strerror(errno);
						 throw comport_exception(exception_message.str());
					};
			};
		};
}

if (fclose(file)!=0) {
	exception_message<<"Can`t close file "<<file_name<<" : "<<strerror(errno);
	throw comport_exception(exception_message.str());
};

return commands;

};

void usage(char *program_name) {
cout<<"Program "<<program_name<<" to com port testing v1.5.3"<<endl;
cout<< "  Usage :"<<endl;
cout<< "  #"<<program_name<<" commands_file_name\n\t\t[port_number]\n\t\t[baud_rate]\n\t\t[delay_send]\n\t\t[delay_devices]\n\t\t[repeats_count]"<<endl;
cout<< "    where :"<<endl;
cout<< "        commands_file_name name of file with stored commands descriptions"<<endl;
cout<< "        [port_number] (optional) com port number from 1 to 4 (by default 2)"<<endl;
cout<< "        [baud_rate] (optional) baud rate of comport"<<endl;
cout<< "        [delay_send] (optional) delay after send from 0 to 500 000 mkS"<<endl;
cout<< "                        (by default or if 0 - calculated in program)"<<endl;
cout<< "        [delay_devices] (optional) delay between requests to devices"<<endl;
cout<< "                         from 1 to 900 000 mkS (by default 200 000)"<<endl;
cout<< "        [repeats_count] (optional) count of requests to devices"<<endl;
cout<< "                         from 1 to 1000 000 (by default 1)"<<endl;
};

int main (int argc, char *argv[]) {
#ifdef __UNIT_TESTING__
all_tests::run_tests();
#else

int port_number=2;
int delay_send_useconds = 0;
int baud_rate;
int delay_devices_useconds = 200000;
int repeats_count=1;
if (argc<2||
	argc>7) {
	usage(argv[0]);
	return 0;
};

if (argc>2) {
	if (strlen(argv[2])!=1)  {
		usage(argv[0]);
		return 0;
		};

	if (isdigit(argv[2][0])==0)  {
		usage(argv[0]);
		return 0;
		};
	
	port_number=atoi(argv[2]);
	if (port_number<1 ||
		port_number>4 )	 {
			usage(argv[0]);
			return 0;
		};
    };

if (argc>3) {
	if (atoi (argv[3])>=0 &&
		atoi (argv[3])<=115200) {
        baud_rate=atoi (argv[3]);
        if (baud_rate!=comport::BAUD_9600 &&
             baud_rate!=comport::BAUD_19200 &&
             baud_rate!=comport::BAUD_115200) {
            cout<<"wrong baud rate "<<baud_rate<<endl;
   			usage(argv[0]);
			return 0;
           };
	} else {
			usage(argv[0]);
			return 0;
	};
};


if (argc>4) {
	if (atoi (argv[4])>=0 &&
		atoi (argv[4])<=500000) {
		delay_send_useconds = atoi (argv[4]);
	} else {
			usage(argv[0]);
			return 0;
	};
};


if (argc>5) {
	if (atoi (argv[5])>0 &&
		atoi (argv[5])<=900000) {
		delay_devices_useconds = atoi (argv[5]);
	} else {
			usage(argv[0]);
			return 0;
	};
};

if (argc>6) {
	if (atoi (argv[6])>0 &&
		atoi (argv[6])<=1000000) {
		repeats_count = atoi (argv[6]);
	} else {
			usage(argv[0]);
			return 0;
	};
};


string file_name(argv[1]);
vector<vector<byte> > commands;
vector<byte> crc_bytes, received_message_header, received_message_body;
vector<char> tmp_bytes(20);
word crc_value;
int printed_bytes_cou, printed_bytes_per_row=10,
     received_message_header_size=3, received_message_body_size,
     received_crc_size = 2, registers_count_index=4;

try {
	comport  comport_device(port_number,
                                       baud_rate,
                                       comport::PARITY_DISABLE,
                                       comport::DATA_BITS_8,
                                       comport::STOP_BITS_1,
                                       delay_send_useconds);

     commands = load_commands(file_name);
     
	for (int commands_cou  = 0;
 	      commands_cou<commands.size();
  	     commands_cou++) {
  			    crc_value  = crc(commands[commands_cou]);
			    crc_bytes = bytes_of_type (crc_value);
		       commands[commands_cou].push_back(crc_bytes[1]);
		       commands[commands_cou].push_back(crc_bytes[0]);
	};

      int repeats_counter=0;
      while (repeats_counter<repeats_count) {

	for (int commands_cou  = 0;
           commands_cou<commands.size();
           commands_cou++) {

			cout << "Sended packet (size "<<commands[commands_cou].size()<<") :"<<endl;
			for (printed_bytes_cou=0; 
			   printed_bytes_cou<commands[commands_cou].size();
			   printed_bytes_cou++) {
                                 itoa(commands[commands_cou][printed_bytes_cou],
				   		&tmp_bytes[0],
				   		16);
				   	cout<<"  0x"<<&tmp_bytes[0];
				   	if (printed_bytes_cou!=0 &&
				   		printed_bytes_cou%printed_bytes_per_row==0)  cout<<endl;
				   };
		   	cout<<endl;

			comport_device.set_rts_state(comport::RTS_SET_1);	
		 	usleep(50);
		 	comport_device.send(commands[commands_cou]);	
			comport_device.set_rts_state(comport::RTS_SET_0);	
		 	usleep(15);

			try{
			received_message_header=comport_device.receive
			                       (received_message_header_size, 
			                        true);
			} catch (comport_exception cpt_exc) {
				cout<<"\n"<<cpt_exc.get_description()<<endl;
                usleep(delay_devices_useconds);
				continue; 
			};

  		    cout <<"\nreceived header"<<endl;
			for (printed_bytes_cou=0; 
				   printed_bytes_cou<received_message_header.size();
				   printed_bytes_cou++) {
				   itoa(received_message_header[printed_bytes_cou],
				   		&tmp_bytes[0],
				   		16);
				   	cout<<"  0x"<<&tmp_bytes[0];
				   	if (printed_bytes_cou!=0 &&
				   		printed_bytes_cou%printed_bytes_per_row==0)  cout<<endl;
				   };

            if  (received_message_header[received_message_header_size-2]==
                  (received_message_header[received_message_header_size-2] | 0x80))  { // function code
                  received_message_body_size=0; //recived_crc_size will be added later
             } else {
                  received_message_body_size=received_message_header[received_message_header_size-1];
             };

			if (received_message_body_size<0) {
					cout << "Wrong message body size "<<	received_message_body_size<<endl;
					cout << "Recieved packet size("<<(received_message_header.size())<<") :"<<endl;			
					for (printed_bytes_cou=0; 
						   printed_bytes_cou<received_message_header.size();
						   printed_bytes_cou++) {
				  		 itoa(received_message_header[printed_bytes_cou],
						   		&tmp_bytes[0],
						   		16);
					   	cout<<"  0x"<<&tmp_bytes[0];
					   	if (printed_bytes_cou!=0 &&
					   		printed_bytes_cou%printed_bytes_per_row==0)  cout<<endl;
				   };
				   	cout<<endl;

					return 0;	
				};
			try{
			received_message_body=comport_device.receive(received_message_body_size+received_crc_size);
			} catch (comport_exception cpt_exc) {
				cout<<"\n"<<cpt_exc.get_description()<<endl;
                usleep(delay_devices_useconds);
				continue; 
			};

			cout <<"\nreceived body (body size "<<received_message_body_size<<"+"<<received_crc_size<<"):"<<endl;
		
			for (printed_bytes_cou=0; 
				   printed_bytes_cou<received_message_body.size();
				   printed_bytes_cou++) {
				   itoa(received_message_body[printed_bytes_cou],
				   		&tmp_bytes[0],
				   		16);
				   	cout<<"  0x"<<&tmp_bytes[0];
				   	if (printed_bytes_cou!=0 &&
				   		(printed_bytes_cou+received_message_header_size)%printed_bytes_per_row==0)  cout<<endl;
				   };
		   	cout<<endl;

//ATTENTION!!! Its implement only for command 4 or similar
            if  (received_message_header[received_message_header_size-2]==
                  (received_message_header[received_message_header_size-2] | 0x80))  { // function error code
                 if (received_message_header.size()+received_crc_size !=
                      received_message_header.size()+
                      received_message_body.size()) {
                            cout <<"Count of excpected bytes in error not equal to received after "<<repeats_counter<<" repeats"<<endl;
                            cout<<"Expected "<< (received_message_header.size()+received_crc_size)<<endl;
                            cout<<"Received "<<(received_message_header.size()+received_message_body.size()) <<endl;
                            return 1;
                    }; // if (received_message_header.size()+received_crc_size !=

            } else { // if  (received_message_header[received_message_header_size-2]!=
            vector<byte>::iterator iter_beg;
            iter_beg=commands[commands_cou].begin();
            advance(iter_beg, registers_count_index+1);
            vector<byte> rec_count_vector;
            rec_count_vector.push_back(*iter_beg);
            rec_count_vector.push_back(*(--iter_beg));
            int send_command_registers_count=type_from_bytes<int>(rec_count_vector);

			if (  (received_message_header.size()+
                   send_command_registers_count*2+
			       received_crc_size)!=
			     (received_message_header.size()+
			      received_message_body.size())
			    ) {
			     cout <<"Count of excpected bytes not equal to received after "<<repeats_counter<<" repeats"<<endl;
			     cout<<"Expected "<< (received_message_header.size()+send_command_registers_count*2+received_crc_size)<<endl;
			     cout<<"Received "<<(received_message_header.size()+received_message_body.size()) <<endl;
			    return 1;
			     }; // if (  (received_message_header.size()+
           }; // if  (received_message_header[received_message_header_size-2]!=
			usleep(delay_devices_useconds);
			}; //for (int commands_cou  = 0;
		repeats_counter++;
		}; //while (repeats_counter<repeats_count)
} catch (comport_exception cmp_exc) {
	cout << cmp_exc.get_description()<<endl;
	return 0;
};

return 1;
#endif
};
