/*****************************************************************************

  C_UDPSocket

  Класс реализации UDP сокета.


  ОПИСАНИЕ

  * Реализация интерфейса сокета для работы по протоколу UDP.


  ИСПОЛЬЗОВАНИЕ

  * Cм. i_socket.h


  ПРИМИЧАНИЕ

  * Перед использованием методов для работы с сокетом необходимо выполнить initWinsock
    для инициализации необходимых библиотек в системе. Метод initWinsock вызывается в конструкторе класса;

  * создать сокет createSock;

  * setNonblock и makeBind методы выполняются в произвольном порядке;

  * sendData и reciveData используются руководствуясь логикой создаваемой коммуникации;

  * servAddr и clientAddr - геттеры соответствующих структур адресов сервера и клиента, соответственно.
  ipAddress - геттер IP-адреса сервера.

  * При создании экземпляра конструктор создает 2 структуры sockaddr_in для адреса сервера и клиента,
    соответственно.


*****************************************************************************/

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN

#endif


#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <utility>

#include "i_socket.h"


/*****************************************************************************
  Macro Definitions
*****************************************************************************/


namespace myTask {
/*****************************************************************************
  Forward Declarations
*****************************************************************************/

/*****************************************************************************
  Types and Classes Definitions
*****************************************************************************/

class C_UDPSocket : I_Socket {

public:

    C_UDPSocket();

    virtual ~C_UDPSocket();

    // Запуск сокета
    virtual bool setup( std::pair<std::string, short> a_conParam,
                        int a_optFlag );

    // Cвязывание сокет с локальным адресом протокола
    bool open();

    // Получение данных
    virtual bool recv( char *a_data, int a_dataLen, int *a_recvSize );

    // Отправка данных
    virtual bool send( char *a_data, int a_dataLen, int *a_sendSize );

    // Закрытие соединеия
    bool close();

    // Закрытие сокета
    bool flush();

    // Имя сокета (ip - port)
    std::string name();

    // Получение структуры адреса источника
    struct sockaddr_in * ownAddr();

    // Получение структуры адреса клиента
    struct sockaddr_in * remoteAddr();

    // Получение ip адреса сервера
    std::string ipAddress();


private:

    // Инициализация библиотеки WinSock2
    bool initWinsock();

    // Установка сокета в неблокирующий режим
    bool setNonblock();

    // Параметры протокола
    int m_ipFamily = T_SockTransProt.UDP.a_ipFamily; // IP протокол соединения
    int m_type     = T_SockTransProt.UDP.a_type;     // тип соединения сокета
    int m_protocol = T_SockTransProt.UDP.a_protocol; // протокол соединения сокета

    // Параметры соединения
    std::string m_servIpAddr;   // IP адресс сервера
    short       m_servPort;     // Порт сервера
    std::string m_sockName;     // Имя сокета отображаемое в логе

    struct sockaddr_in *m_ownAddr;    // Параметры соединения сервера
    struct sockaddr_in *m_remoteAddr; // Параметры соединения клиента

    // Данные сокета
    WSADATA m_wsadata;
    int     m_sockFd = INVALID_SOCKET;  // дескриптор сокета

protected:

    struct T_SockTransProt{
        struct {
            int a_protocol = IPPROTO_UDP;
            int a_type     = SOCK_DGRAM;
            int a_ipFamily = AF_INET;
        } UDP;
    } T_SockTransProt;

    // Параметры состояния сокета блокировки при передачи данных
    enum Flags{
        optNo       = 0x00,
        optNonblock = 0x01,
    };

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
}//mySocket


