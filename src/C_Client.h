/*****************************************************************************

  C_Client

  Предоставляет реализацию сетевого клиента.


  ОПИСАНИЕ

  * Класс имеет 3 публичных метода которые позволяют:

    - запустить сетевой клиент реализующий совой обмен с помощью сокета по
      протоколу UDP;
    - провести сессии сетевого взаимодействия с другим сокетом;
    - завершить соединение и освободить ресурсы;


  ИСПОЛЬЗОВАНИЕ

  1) Для запуска сервера необходимо создать экземпляр класса C_Client.

        C_Client client = new Client();

  2) Настроить/запустить сокет сервера:

        client.setup(...);

  3) Начать процесс сетевого взаимодействия.

        client.workingSession();

  4) Закрыть соединение и освободить все ресурсы.

        client.flush();


  ПРИМИЧАНИЕ

  * Больше информации об протоколе работы клиента см. "Protocol.h"

*****************************************************************************/

#pragma once

#include <chrono>
#include <map>
#include <cerrno>

#include "I_Socket.h"
#include "SocketCreator.h"
#include "C_UdpSocket.h"
#include "Config.h"

namespace myTask {

/*****************************************************************************
  Macro Definitions
*****************************************************************************/

/*****************************************************************************
  Forward Declarations
*****************************************************************************/

/*****************************************************************************
  Types and Classes Definitions
*****************************************************************************/

class C_Client
{
public:

    C_Client();

    ~C_Client();

    // Создание/запуск сокета клиента
    bool setup( ConnectionParams a_conParam);

    // Работа клиента
    bool workingSession( int a_messPerSec, int a_workDuration );

    // Закрытие сокета клиента
    bool flush();

    bool recv( std::vector<char> &buffer, std::string &fromAddr);

private:

    bool send( std::string message );


    const std::chrono::milliseconds   s_sendTimout = std::chrono::milliseconds(1000);
    const std::string s_getNumMessage= "Give me a number!";   // Запрос клиента
    const std::string s_endConnMessage= "Stop Connection";   // Запрос клиента

    std::string m_ownIp;
    std::string m_ownPort;
    std::string m_remIp;
    std::string m_remPort;
    int         m_blocking;


    // Сокет клиента
    I_Socket* m_socket = nullptr;

    // Размер буфера сообщений клиента
    const int m_BUF_SIZE = 512;

    // Запуск сетевого взаимодействия с сервером
    bool communication( int messPerSec, int workDuration );
};

/*****************************************************************************
  Functions Prototypes
*****************************************************************************/

/*****************************************************************************
  Variables Deсlarations
*****************************************************************************/

/*****************************************************************************
  Inline Functions Definitions
*****************************************************************************/

} // namespace myTask

