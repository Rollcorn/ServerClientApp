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

       Метод communication(...) включает в workingSession() логику обработкии
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

#include "I_Connection.h"
#include "SocketCreator.h"
#include "C_UdpSocket.h"
#include "ErrorCodes.h"


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
    bool setup( const conf_t& a_conParam );

    // Запуск работы сервера
    bool workingSession();

    // Закрытие сокета клиента
    bool flush();


private:
    // Отправка ответа клиенту
    bool recv( std::vector<char> &a_buffer, std::string &a_fromAddr );

    // Данные соединения
    std::string m_ownIp;
    std::string m_ownPort;
    std::string m_remIp;
    std::string m_remPort;
    int         m_blocking;

    // Обмен данными с клиентом
    bool communication( );
    bool m_isEndConnSignal = false;
    const std::string s_endConnMessage= "Stop Connection";   // Запрос клиента

    // Объект обеспечивающий связь клиента с сервером
    I_Connection*  m_socket = nullptr;

    // Размер буфера сервера
    const size_t s_bufferLen = 1024;

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

