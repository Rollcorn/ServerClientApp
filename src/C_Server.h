/********************************************************************************************************

  C_Server

  Класс предоставляет функционал для работы сервера


  ОПИСАНИЕ

  * Класс позволяет запустить сокет для обмена данными с клиентами, провести сетевую сессию
    и закрыть сокет.


  ИСПОЛЬЗОВАНИЕ

    1) Для запуска сервера необходимо создать экземпляр класса C_Server.

         C_Server server = new C_Server;

    2) Запустить сокет сервера на полученном адресе и с полученными опциями.

         server.setup(...);

    3) Начать процесс сетевого взаимодействия клиента.

         server.workingSession()

       Метод communication(...) инкапсулирует в workingSession() логику обработкии
       полученных данных и логику отправки ответа.

    4) Закрыть соединение с сокетом и освободить все ресурсы.

         server.flush();


  ПРИМИЧАНИЕ

  * Больше информации об протоколе работы сервера см. "Protocol.h".

*********************************************************************************************************/

#pragma once

#include <random>
#include <ctime>
#include <typeinfo>
#include <map>

#include "I_Socket.h"
#include "SocketCreator.h"
#include "C_UdpSocket.h"

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

class C_Server
{
public:

    C_Server();

    ~C_Server();

    // Запуск сервера
    bool setup( std::map<std::string, std::string> a_conParam, int a_optFlag = 1 );

    // Запуск работы сервера
    bool workingSession();

    // Закрытие сокета клиента
    bool flush();

private:

    std::string m_ipParam;
    std::string m_portParam;

    // Обмен данными с клиентом
    bool communication(const int a_bufSize);    

    // Объект обеспечивающий связь клиента с сервером
    I_Socket*  m_socket = nullptr;

    // Размер буфера сервера
    const int m_servBufferSize = 1024;

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

