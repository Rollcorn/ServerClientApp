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

    socket.setup( ... );

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

  * Функции send() и recv() могут заканчиваться статус успешного/не усепшного выполения приема/отправки
    данных, но причин для неудачного статуса больше одной. Для обработки причины неудачи используется метод
    needToReapet() в котором реализуются обработчики некоторых ошибок, не являющиеся критическими, и в случае
    если программа может попробовать вызвать метод, вернувший ошибку, еще раз возвращает true, если же ошибка
    критическая, выводит в лог код ошибки и её код.

  * Параметры UDP соединие приведены в структуре T_SockTransProt.UDP

  * Обьекты Flags передаются в setup() как дополнительные опция сокета

    - optNo       - сокет не использует дополнительные опции;
    - optNonblock - опция сокета неблокирующего поток.

  ДАННЫЕ

  * Конфигурация соединения создается на основе передданого в метод setup() словаря a_conParam. Словарь строится
    по принципу ключ-значение. Где и ключ и значения являются строковыми литералами.

  * Для коректного парсинга словаря необходимо присутствие минимально необходимых для конфигурации параметров,
    таких как:
    - "ownIp" - это ключ для собственного ip-адреса сокета в качестве значения ожидается строка в формате
      "xxx.xxx.xxx.xxx". Например "127.0.0.1";
    - "ownPort" - это ключ для собственного порта сокета в качестве значения ожидается строка в формате 4-x значное
      число в формате строки "XXXX". Например "5000";
    - "remIp" - это ключ ip-адреса удаленного сокета, он строится аналогично "ownIp";
    - "remPort" - это ключ порта удаленного сокета, он строится аналогично "ownPort";
    - "block" - дополнительный параметр указывающих на необходимые опции сокета. Например значение "1" воспринимается
      переключает сокет в неблокирующий режим.


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

#include <iostream>
//#include <thread>
//#include <chrono>
#include <vector>
#include <utility>
#include <map>
#include <cerrno>

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#include "I_Connection.h"
#include "Config.h"
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

class C_UdpSocket : public I_Connection
{

public:

    C_UdpSocket();

    ~C_UdpSocket();

    // Запуск сокета
    bool setup( const conf_t& a_conParam ) override;

    // Cвязывание сокет с локальным адресом протокола
    bool open() override;

    // Получение данных
    bool recv( std::vector<char> &a_buffer, std::string &a_from ) override;

    // Отправка данных
    bool send( const std::vector<char> &a_data, const std::string &a_to ) override;

    // Закрытие соединеия
    bool close() override;

    // Закрытие сокета
    bool flush() override;

    // Имя сокета (ip - port)
    std::string name() override;

    // Обработка ошибок сокета
    bool needToRepeat() override;

    // Адрес удаленного сокета
    std::string remoteAddr() override;

private:
    int m_lastErrCod;

    // Инициализация библиотеки WinSock2
    bool initWinsock();

    // Установка сокета в неблокирующий режим
    bool setNonblock();

    /**
     * Параметры протокола
     */
    int m_ipFamily = AF_INET;       // IP протокол соединения
    int m_type     = SOCK_DGRAM;    // тип соединения сокета
    int m_protocol = IPPROTO_UDP;   // протокол соединения сокета
    const int MAXLINE = 512;

    /**
     * Параметры соединения
     */
    std::string m_ownIp;        // собственный IP адресс
    short       m_ownPort;      // собственный Порт
    std::string m_remIp;        // IP адресс удаленного сокета
    short       m_remPort;      // Порт удаленного сокета
    bool        m_isBlocking;   // Статус блокироки потока сокета
    /**
     * Данные сокета
     */
    WSADATA m_wsadata;                  // Объект библиотеки winsock2
    SOCKET  m_sockFd = INVALID_SOCKET;  // дескриптор сокета
    struct sockaddr_in m_ownAddr;       // структура адреса собственного сокета IPv4
    struct sockaddr_in m_remoteAddr;    // структура адреса удаленного сокета IPv4

protected:
    // Параметры состояния сокета блокировки при передачи данных
    enum Flags {
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

} // namespace myTask


