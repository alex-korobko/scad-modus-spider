#ifndef __ROUTER_H__
#define __ROUTER_H__

class net
{
protected:
	int id;
	in_addr_t	ip;
	int  curr_gateway_id;
	int		broken;
	int skip_count;
	
	net();
public:
	net(
		int new_id,
		in_addr_t	new_ip,
		int new_curr_gateway_id,
		int	 new_broken,
		int new_skip_count=0
		) : id (new_id), ip(new_ip), curr_gateway_id(new_curr_gateway_id),
		broken(new_broken), skip_count(new_skip_count)
		{};
/*
	set_ and get_ methods for private data members
*/
	int get_id() {return(id);}
	in_addr_t get_ip() {return (ip);};
		
	int get_broken() {return(broken);};
	void set_broken(int new_broken) {broken=new_broken;};

	int get_skip_count() {return(skip_count);};
	void set_skip_count (int new_skip_count) {skip_count=new_skip_count;};

	int  get_curr_gateway_id() {return(curr_gateway_id);};
	void set_curr_gateway_id(int new_curr_gateway_id) {curr_gateway_id=new_curr_gateway_id;};

};

class gateway 
{
private:
int id;
in_addr_t ip;
string name; // direction
gateway();
public:
gateway( int  new_id,
				in_addr_t new_ip,
				string new_name
				) : id(new_id), ip (new_ip), name(new_name) 
{};

int get_id() {return(id);};
string get_name() {return(name);};
in_addr_t get_ip() {return(ip);};

};

class router
{
protected:
struct ltint
	{
		  bool operator() (const int i1, const int i2) const
 			 {
   			 return (i1<i2) ;
  			}
	};
	typedef  map <int,  net, ltint> router_nets;	
	typedef  map <int,  gateway, ltint> router_gateways;	
	router_nets nets;
	router_gateways gateways;

	pthread_t		routerTID;

	bool get_gateway_parameters();
	bool get_net_parameters();

public:
	typedef router_nets::iterator iterator_nets;
	typedef router_gateways::iterator iterator_gateways;
	
	~router();

	enum {
				ADD_ROUTE=0,
				DELETE_ROUTE=1,

				NO_BROKEN=0,
				BROKEN=1,

				SKIP_TEST_COUNT=4,
				ECHO_PORT=7
				};

	bool load_routing(const string file_name);

	int start();
	void loop();
};
	
#endif
