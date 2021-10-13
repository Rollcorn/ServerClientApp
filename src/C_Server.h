/********************************************************************************************************

  C_Server

  Класс предоставляет функционал для работы сервера


  ОПИСАНИЕ

  * Класс позволяет запустить сокет для обмена данными с клиентами, проводить логированиие выполняемых операций
    и производить ожидание запросов клиентов.

  * Пользователю класса представляется интерфейс из 3 методов:

    1) setupConnect - создает и инициализирует сокет с необходимыми параметрами.
       Первый и второй параметр метода - ip-адрес и порт, необходимо задать вручную остальные
       параметры можно оставить по умолчанию. По умолчанию передаются параметры для создани\
       UDP сокета.

    2) setSockName - назначает сокету серевера имя если необходимо при выведении логов.

    3) workingSession - получает размер буфера в который будет сохранять полученные из сокета
    данные. Ожидает запроса от клиента, чтобы отправить ему случайно сгенерированное число.


  ИСПОЛЬЗОВАНИЕ

  1) Для запуска сервера необходимо создать экземпляр класса C_Server

        C_Server server;

  2) Настроить/запустить сокет сервера:

        server.setupConnect( <ip-адрес>, <порт>, <тип соединени>, <Тип ip-протокола>, <тип домена>,
                             <флаговая переменная> )

    <ip-адрес> и <порт> определяются фактической информацией об адресе клиента/сервера;

    <тип соединени> - тип сокета. Может принимать следющие сначения:
        * SOCK_STREAM - потоковый сокет;
        * SOCK_DGRAM     - сокет дейтаграмм;
        * SOCK_SEQPACKET - сокет последовательных пакетов;
        * SOCK_RAW       - символьный (неструктурированный) сокет.

    <Тип ip-протокола> - возможные значения параметра protocol:
        * IPPROTO_TCP  - транспортный протокол TCP;
        * IPPROTO_UDP  - транспортный протокол UDP;
        * IPPROTO_SCTP - транспортный протокол SCTP;

    <тип домена> - семейство сокетов (family):
        * AF_INET  - Протоколы IPv4;
        * AF_INET6 - Протоколы IPv6;
        * AF_LOCAL - Протоколы доменных сокетов Unix;
        * AF_ROUTE - Маршрутизирующие сокеты
        * AF_KEY   - Сокет управления ключами

    <флаговая переменная> - примнимает значение 0 и 1, если задана 1, то запущенный сокет будет настроен
                            в неблокирующий режим, в обратном случае сокет будет работать в блокирующем
                            режиме. По умолчанию передается значение 1.

  3) При необходимости - назначить сокету имя:

        server.setServName( <Имя сокета> );

  4) вызвать метод workingSession( <Размер буфера> ). Метод setupConnect возвращает значение bool, поэтому
     желательно выполнить проверку того факта что сокет создался и запустился и только в этом случае имеет
     смысл запускать функцию обмена

        if ( server.setupConnect(...) ) {
            server.workingSession();
        }
        else {
            // Обработка ошибки
            ...
        }

*********************************************************************************************************/

/*****************************************************************************
  Macro Definitions
*****************************************************************************/

#pragma once

#include <random>
#include <ctime>
#include <typeinfo>

#include "C_Socket.h"

namespace server {

/*****************************************************************************
  Forward Declarations
*****************************************************************************/

/*****************************************************************************
  Types and Classes Definitions
*****************************************************************************/

class C_Server
{
public:

    // Запуск сокета сервера
    bool setupConnect( std::string a_ipAddr, int a_port, int a_type = SOCK_DGRAM,
                       int a_protocol = IPPROTO_UDP, int a_ipFamily = AF_INET ,
                       int a_optFlag = 1 );

    // Установка имени сокета
    bool setSockName( std::string a_sockName );

    // Закрытие сокета клиента
    bool flushConnect();

    // Запуск работы сервера
    bool workingSession( int a_buffSize );

    C_Server();
    ~C_Server() = default;

private:
    // Объект обеспечивающий связь клиента с сервером
    mySocket::C_Socket  m_socket;
    // Размер буфера сервера
    const int           m_servBufferSize = 1024;

    // Обмен данными с клиентом
    bool communication( int a_buffSize );

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
} // namespace server

