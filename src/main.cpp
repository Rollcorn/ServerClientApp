#include <iostream>
#include <thread>
#include "server.h"
#include "client.h"




int main(){
server::C_Server server_ex;
    client::C_Client client_ex;

//    client_ex.working_session();

    std::thread t1([&server_ex]{server_ex.working_session();});
    std::thread t2([&client_ex]{client_ex.working_session();});
    t1.join();
    t2.join();

    return 0;
}

