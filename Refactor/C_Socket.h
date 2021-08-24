/*****************************************************************************

  CLASS C_Socket

  Класс для создания и работы с сокетами средствами библиотеки WinSock2

  ОПИСАНИЕ Предоставляет интерфейсы для работы с сокетами по протоколу UDP

  ИСПОЛЬЗОВАНИЕ Необходимимы для начала работы являеются следующие параметры:
  * IP-адрес сервера;
  * Номер порта для открытия сервера;
  * Имя сокета, используемое в выводимых логах.
    Порядок использования
  * Перед использованием методов класса для работы с сокетом необходимо выполнить initWinsock
    для инициализации необходимых библиотек в системе.
  * создать сокет createSock;
  * setNonblock и makeBind методы выполняются в произвольном порядке;
  * sendData и reciveData используются руководствуясь логикой создаваемой коммуникации;
  * servAddr и clientAddr - геттеры соответствующих структур адресов сервера и клиента, соответственно.
  ipAddress - геттер IP-адреса сервера.

  ПРИМИЧАНИЕ
  * При создании экземпляра конструктор создает 2 структуры sockaddr_in для адреса сервера и клиента,
    соответственно.

*****************************************************************************/

/*****************************************************************************
  Macro Definitions
*****************************************************************************/

#ifndef WINSOCK_INIT_H
#define WINSOCK_INIT_H

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

namespace mySocket {
/*****************************************************************************
  Forward Declarations
*****************************************************************************/
/*****************************************************************************
  Types and Classes Definitions
*****************************************************************************/
struct trans_prot{
    // TODO
};

class C_Socket
{

public:
    // Инициализация библиотеки WinSock2
    void initWinsock();

    // Создание/запуск сокета
    void createSock(struct sockaddr_in *a_myAddr, unsigned long addr, int protocol);

    // Установка сокета в неблокирующий режим
    void setNonblock();

    // Cвязывание сокет с локальным адресом протокола
    void makeBind(struct sockaddr_in *a_myAddr);

    // Установление соединения(для ТСР) TODO
    void makeConnect();

    // Установка в слушающий режим(для ТСР) TODO
    void makeListen(int a_backlog);

    // Получение данныйх
    void reciveData(struct sockaddr_in *a_srcAddr, char *a_dataStore, int a_dataStoreSize);

    // Отправка данных
    void sendData(struct sockaddr_in *a_distAddr, char *a_dataStore, int a_dataStorSize);

    // Закрытие сокета
    void disconnect();

    // Получить имя сокета
    std::string name();

    // Получить структуру соединия источника
    struct sockaddr_in* servAddr();

    // Получить структуру соединия назначения
    struct sockaddr_in* clientAddr();

    // Получить ip адресс сервера
    std::string ipAddress();

    C_Socket( std::string a_ipAddr, int a_port, std::string a_sockName);

    ~C_Socket();

private:
    // Параметры протокола
    int m_ipFamily  = AF_INET;      // IP протокол соединения
    int m_type      = SOCK_DGRAM;   // тип соединения сокета
    int m_protocol  = 0;            // протокол соединения сокета

    // Параметры соединения
    std::string         m_sockName;     // Имя сокета отображаемое в логе
    int                 m_servPort;     // Порт сервера
    std::string         m_servIpAddr;   // IP адресс сервера

    struct sockaddr_in *m_myAddr;               // Параметры соединения сервера
    struct sockaddr_in *m_otherAddr;            // Параметры соединения клиента


    //Данные сокета
    WSADATA m_wsadata;
    int     m_sockFd = INVALID_SOCKET;  // дескриптор сокета

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
#endif // WINSOCK_INIT_H


