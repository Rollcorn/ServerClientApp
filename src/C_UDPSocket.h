/*****************************************************************************

  C_Socket

  Класс реализации сокета.


  ОПИСАНИЕ

  * Предоставляет интерфейс для работы с сокетами по протоколу UDP.


  ИСПОЛЬЗОВАНИЕ

  * Необходимыми для начала работы являеются следующие параметры:

        - IP-адрес сервера;
        - Номер порта для открытия сервера;
        - Имя сокета, используемое в выводимых логах.

  * Базовый порядок применения функций класса для запуска сокета:

    1. Создание сокета

        C_Socket socket;

    2. Настройка сокета

        socket.setupSock(...);

    3. Открытие сокета
        socket.openSock();

    4. Установка соединения (опционально для UDP)
        socket.connect();

    5. Использование сокета - обмен (в необходимом порядке)

        5.1. Отправка данных
            socket.send( ... );
        5.2. Получение данных
            socket.recv( ... );

    6. Закрытие соединения
        socket.disconnect();

    При необходимости соединения с новыми параметрами можно начать с п. 4, передав новые параметры.

    7. Закрытие сокета
        socket.close()

       В случае новго открытия сокета повторить шаги начиная с п. 2.


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

#include "i_socket.h"

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <iostream>
#include <thread>
#include <chrono>

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

class C_Socket
{
    // Параметры состояния сокета в
    enum Flags{
        optNo       = 0x00,
        optNonblock = 0x01,
    };

public:

    // Запуск сокета
    bool setup( int a_protocol = IPPROTO_UDP, int a_type = SOCK_DGRAM,
                    int a_ipFamily = AF_INET );

    // Настройки сокета
    bool setSettings( std::string a_ipAddr, int a_port, int a_optFlag = optNonblock );

    // Установка сокета в неблокирующий режим
    bool setNonblock();

    // Cвязывание сокет с локальным адресом протокола
    bool open();

    // Получение данных
    bool recv( struct sockaddr_in *a_srcAddr, char *a_data,
                     int a_dataLen, int *a_recvSize );

    // Отправка данных
    bool send(struct sockaddr_in *a_distAddr, char *a_data,
                   int a_dataLen, int *a_sendSize );

    // Закрытие сокета
    bool flush();

    // Задание имени сокета
    bool setName( std::string a_name );

    // Получение имени сокета
    std::string name();

    // Получение структуры адреса источника
    struct sockaddr_in * ownAddr();

    // Получение структуры адреса клиента
    struct sockaddr_in * remoteAddr();

    // Получение ip адреса сервера
    std::string ipAddress();

    C_Socket( );

    ~C_Socket();

private:

    // Инициализация библиотеки WinSock2
    bool initWinsock();

    // Параметры протокола
    int m_ipFamily; // IP протокол соединения
    int m_type;     // тип соединения сокета
    int m_protocol; // протокол соединения сокета

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
            int a_type = SOCK_DGRAM;
            int a_ipFamily = AF_INET;
        } UDP;
    }T_SockTransProt;

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


