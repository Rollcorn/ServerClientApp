/*****************************************************************************

  C_UDPSocket

  Класс реализации UDP сокета.


  ОПИСАНИЕ

  * Реализация интерфейса сокета для работы по протоколу UDP.


  ИСПОЛЬЗОВАНИЕ

  * Необходимыми для начала работы являеются следующие параметры:

    - IP-адрес сервера;
    - Номер порта для открытия сокета;


  * Базовый порядок применения функций класса для запуска сокета:

    1. Создание сокета

    I_Socket socket;

    2. Настройка сокета

    socket.setup(...);

    3. Открытие сокета
    socket.open();

    4. Использование сокета - обмен (в необходимом порядке)

    4.1. Отправка данных
        socket.send( ... );
    4.2. Получение данных
        socket.recv( ... );

    5. Закрытие соединения
    socket.close();

    6. Закрытие сокета
    socket.flush()

    В случае новго открытия сокета повторить шаги начиная с п. 2.

  * Параметры UDP соединие приведены в структуре T_SockTransProt.UDP

  * Обьекты Flags передаются в setup() как дополнительные опция сокета

    - optNo       - сокет не использует дополнительные опции;
    - optNonblock - опция сокета неблокирующего поток.


  ПРИМИЧАНИЕ

  * Перед использованием методов для работы с сокетом необходимо выполнить initWinsock
    для инициализации необходимых библиотек в системе. Метод initWinsock вызывается в конструкторе класса;

  * send и recv используются руководствуясь логикой создаваемой коммуникации;

  * remoteAddr и ownAddr - геттер соответствующих структур адресов удаленного и собственного, соответственно.

  * при создании экземпляра конструктор создает 2 структуры sockaddr_in для удаленного и собственного адреса.


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

class C_UDPSocket : public I_Socket {

public:

    C_UDPSocket();

    virtual ~C_UDPSocket();

    // Запуск сокета
    virtual bool setup( std::pair<std::string, short> a_conParam,
                        int a_optFlag );

    // Cвязывание сокет с локальным адресом протокола
    virtual bool open();

    // Получение данных
    virtual bool recv( char *a_data, int a_dataLen, int *a_recvSize );

    // Отправка данных
    virtual bool send( char *a_data, int a_dataLen, int *a_sendSize );

    // Закрытие соединеия
    virtual bool close();

    // Закрытие сокета
    virtual bool flush();

    // Имя сокета (ip - port)
    virtual std::string name();

    // Получение структуры собственного адреса
    struct sockaddr_in * ownAddr();

    // Получение структуры удаленного адреса
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


