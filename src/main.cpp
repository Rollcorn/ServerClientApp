/*****************************************************************************

  main

  Реализация сетевого взяаимодействи клент-сервер.


  ОПИСАНИЕ

  * Описание протокола взаимодействияя см. Protocol.h.

*****************************************************************************/

#include <iostream>
#include <thread>
#include <utility>

#include "C_Server.h"
#include "C_Client.h"


/*****************************************************************************
  Macro Definitions
*****************************************************************************/


/*****************************************************************************
  Forward Declarations
*****************************************************************************/


/*****************************************************************************
  Types and Classes Definitions
*****************************************************************************/


/*****************************************************************************
  Functions Prototypes
*****************************************************************************/


/*****************************************************************************
  Variables Definitions
*****************************************************************************/


/*****************************************************************************
  Functions Definitions
*****************************************************************************/

int main()
{
    // Инициализация параметров соединия для сервера
    std::map<std::string, std::string> servParams { {"ownIp", "127.0.0.1"}, {"ownPort", "8080"},
                                                    {"remIp", "127.0.0.1"}, {"remPort", "8081"},
                                                    {"block", "1"} };

    // Инициализация параметров соединия для клиента
    std::map<std::string, std::string> cliParams { {"ownIp", "127.0.0.1"}, {"ownPort", "8081"},
                                                   {"remIp", "127.0.0.1"}, {"remPort", "8080"},
                                                   {"block", "1"} };

    std::cout << "server" << " " << servParams.at("ownIp") << ":" << servParams.at("ownPort") << std::endl;
    std::cout << "server" << " " << servParams.at("remIp") << ":" << servParams.at("remPort") << std::endl;
    std::cout << "client" << " " << cliParams.at("ownIp") << ":" << cliParams.at("ownPort") << std::endl;
    std::cout << "client" << " " << cliParams.at("remIp") << ":" << cliParams.at("remPort") << std::endl;

    int messPerS     = 1;   // частота отправки запросов на сервевер (сообщений в секунду)
    int workDuration = 30;  // длительность работы клиента

    myTask::C_Server myServer;
    myTask::C_Client myClient;

    std::thread server_thread( [&]{ if ( myServer.setup(servParams) ){
                                         myServer.workingSession();
                                    } else { std::cout << "Server Setup ERROR!\n"; }
                                  } );

    std::thread client_thread( [&]{ if ( myClient.setup(cliParams) ){
                                         myClient.workingSession(messPerS, workDuration);
                                    } else { std::cout << "Client Setup ERROR!\n"; }
                                  } );

    server_thread.join();
    client_thread.join();

    return 0;
}



