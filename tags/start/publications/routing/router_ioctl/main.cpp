#include <arpa/inet.h>
#include <net/if.h>
#include <net/route.h>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include <vector>
#include <string>
#include <stdexcept>

using namespace std;

#include "router.h"

void* routing_thread (void* arg) {
   try {
    router& router_inst=router::get_instance();
    router::routes_iterator routes_iter;
    router::gateways_iterator gateways_iter;

    bool connect_to_test_host_established;
    if (router_inst.size_gateway()<2) {
             cout<<"routing_thread: router_inst.gateways_size()<2"<<endl;
             return NULL;
           };

    if (router_inst.empty_routes()) {
              cout<<"routing_thread: routes is empty"<<endl;
              return NULL;
           };

     	while (true) {
         routes_iter=router_inst.begin_routes();
         while (routes_iter!=router_inst.end_routes()){
                sleep(5);
                gateways_iter=router_inst.begin_gateway();
                while(gateways_iter!=router_inst.end_gateway() ) {
                            //test all connections in previos gateway
                            connect_to_test_host_established=false;
                             router::route::test_hosts_iterator test_hosts_iter=routes_iter->test_hosts.begin();
                             while (test_hosts_iter!=routes_iter->test_hosts.end()) {
                                  if (router_inst.test_connection_to_test_host(*test_hosts_iter)) {
                                          connect_to_test_host_established=true;
                                           break;
                                       };
                                  test_hosts_iter++;
                             }; //while (test_hosts_iter!=routes_iter->test_hosts.end())
                           if (connect_to_test_host_established) break;

                            router_inst.delete_route(routes_iter->destination, 
                                                                routes_iter->mask, 
                                                                routes_iter->current_gateway,
                                                                RTF_MASK);
                            routes_iter->current_gateway=*gateways_iter;
                            router_inst.add_route(routes_iter->destination,
                                                            routes_iter->mask, 
                                                            routes_iter->current_gateway,
                                                            RTF_MASK);

                            //test all connections in new gateway
                            connect_to_test_host_established=false;
                            test_hosts_iter= routes_iter->test_hosts.begin();
                             while (test_hosts_iter!=routes_iter->test_hosts.end()) {
                                  if (router_inst.test_connection_to_test_host(*test_hosts_iter)) {
                                          connect_to_test_host_established=true;
                                           break;
                                       };
                                  test_hosts_iter++;
                             }; //while (test_hosts_iter!=routes_iter->test_hosts.end())

                           if (connect_to_test_host_established) break;
                      gateways_iter++;
                  }; //while(gateways_iter!=router_inst.end_gateways() &&
                routes_iter++;

          };//while (routes_iter!=router_inst.end_routes()
     }; //while (true)
     } catch (runtime_error run_err) {
        cout<<"in routing_thread catched exception "<<run_err.what()<<endl;
        return NULL;
    };


};

void usage(string program_name) {
   if (program_name.empty()) program_name="router_pf_route";
   cout <<" Usage :\n"
          <<"    "<<program_name<<" routing_config_file.conf\n"
          <<"    where :\n"
          <<"       routing_config_file.conf file with routing configuration\n"<<endl;
};

int main (int argc, char* argv[]) {
int thread_creating_result;
pthread_t tid;

if (argc==0) {
    usage("");
    return 0;
};

if (argc<2) {
  usage(argv[0]);
  return 0;
};

try {
   router& router_instance=router::get_instance();
   router_instance.load(argv[1]);
   thread_creating_result=pthread_create(&tid,NULL,&routing_thread,NULL);
   if (thread_creating_result!=EOK) 
    throw runtime_error(string("Error creating thread:\n")+strerror(thread_creating_result));

   pthread_join(tid, NULL);
} catch (runtime_error run_err) {
   cout<<"In main catched runtime exception:\n"<<run_err.what()<<endl;
};

};
