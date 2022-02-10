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
    // Инициализация параметров соединия
    std::string addr = "127.0.0.1";
    short         port = 8080;
    std::pair<std::string, short> connectData{addr, port};
    std::cout << connectData.first << " " << connectData.second << '\n';

    int messPerS     = 1;   // частота отправки запросов на сервевер (сообщений в секунду)
    int workDuration = 30;  // длительность работы клиента

    myTask::C_Server myServer;
    myTask::C_Client myClient;
    std::cout << "test" << std::endl;

    std::thread server_thread( [&]{ if ( myServer.setup( connectData, 1) ){
                                         myServer.workingSession();
                                    } else { std::cout << "Server Setup ERROR!\n"; }
                                  } );

    std::thread client_thread( [&]{ if ( myClient.setup( connectData, 1 ) ){
                                         myClient.workingSession(messPerS, workDuration);
                                    } else { std::cout << "Client Setup ERROR!\n"; }
                                  } );

    server_thread.join();
    client_thread.join();

    return 0;
}



