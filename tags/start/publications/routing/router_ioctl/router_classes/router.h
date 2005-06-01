#ifndef _ROUTER_H_	
#define _ROUTER_H_

class router   {
friend class frinend_class_only_for_compiler_happening ;
public:
enum {ROUTING, ROUTE, SECTIONS_COUNT};
enum {ECHO_TCP_PORT=7,  //echo service port on tested host
          CONNECT_TO_DEVICE_TIMEOUT=12, //connection established timeout in sec
          RECIEVE_SEND_TO_DEVICE_TIMEOUT=10}; //recieve/send timeout in sec
struct route {
    typedef vector<in_addr> test_hosts_container;
    typedef test_hosts_container::iterator test_hosts_iterator;
    typedef test_hosts_container::size_type test_hosts_size_type;
    in_addr destination;	
    in_addr mask;	
    in_addr current_gateway;
    test_hosts_container test_hosts;
 };

private:
 typedef vector<route> routes_container;
 typedef vector<route>::iterator routes_iterator;
 typedef vector<route>::size_type routes_size_type;

 typedef vector<in_addr> gateways_container;
 typedef vector<in_addr>::iterator gateways_iterator;
 typedef vector<in_addr>::size_type gateways_size_type;

routes_container routes;
gateways_container gateways;

bool routing_enabled;
int sock;

 router() throw (runtime_error);
 ~router();

//hide unused operations
router(const router& ){};
router& operator=(const router& r) { return const_cast<router&>(r);};

public:

void push_back_routes(route route_entr) {routes.push_back(route_entr);};
bool empty_routes() {return routes.empty();};
routes_size_type size_routes() {return routes.size();};
routes_iterator begin_routes() {return routes.begin();};
routes_iterator end_routes() {return routes.end();};

void push_back_gateways(in_addr gateway_entr) {gateways.push_back(gateway_entr);};
bool empty_gateways() {return gateways.empty();};
gateways_size_type size_gateway() {return gateways.size();};
gateways_iterator begin_gateway() {return gateways.begin();};
gateways_iterator end_gateway() {return gateways.end();};

bool is_routing_enabled() {return routing_enabled;};
bool test_connection_to_test_host(in_addr test_host);

void add_route ( struct in_addr destination, 
                         struct in_addr mask,
                         struct in_addr gateway,
                         int flags);
void delete_route ( struct in_addr destination, 
                         struct in_addr mask,
                         struct in_addr gateway,
                         int flags);

void load_route() throw (runtime_error);
void load_routing() throw (runtime_error);
void load (string file_name) 
                  throw (runtime_error);

static  router& get_instance() throw (runtime_error) {
     static router router_instance;
     return router_instance;
};

};

#endif

