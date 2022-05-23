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
#include "Config.h"


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
    conf::conf_t servParams { {"transProt", "UDP"},
                        {"ownIp", "127.0.0.1"}, {"ownPort", "5000"},
                        {"remIp", "127.0.0.1"}, {"remPort", "5005"},
                        {"block", "1"} };

    // Инициализация параметров соединия для клиента
    conf::conf_t cliParams { {"transProt", "UDP"},
                       {"ownIp", "127.0.0.1"}, {"ownPort", "5005"},
                       {"remIp", "127.0.0.1"}, {"remPort", "5000"},
                       {"block", "1"} };


    int messPerS     = 1;   // частота отправки запросов на сервевер (сообщений в секунду)
    int workDuration = 30;  // длительность работы клиента

    myTask::C_Server myServer;
    myTask::C_Client myClient;
    bool isServerSetup = myServer.setup(servParams);
    bool isClientSetup = myClient.setup(cliParams);

    std::thread server_thread( [&]{ if ( isServerSetup ){
                                         myServer.workingSession();
                                    } else { std::cout << "Server Setup ERROR!\n"; }
                                  } );

    std::thread client_thread( [&]{ if ( isClientSetup ){
                                         myClient.workingSession(messPerS, workDuration);
                                    } else { std::cout << "Client Setup ERROR!\n"; }
                                  } );

    server_thread.join();
    client_thread.join();

    return 0;
}



