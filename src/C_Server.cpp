/*****************************************************************************

  C_Server

  Класс предоставляет функционал для работы сервера


  РЕАЛИЗАЦИЯ

  Функция communication генрерирует случайное число и отправляет его на запрос
  клиента. Интервал в котором генерируется число задается от lowBracket до topBracket и
  может быть изменен в этих переменных.


*****************************************************************************/
#pragma once

#include "C_Server.h"

/*****************************************************************************
  Macro Definitions
*****************************************************************************/

namespace myTask { // TODO

/*****************************************************************************
  Forward Declarations
*****************************************************************************/

/*****************************************************************************
  Types and Classes Definitions
*****************************************************************************/

/*****************************************************************************
 * Конструктор
 */
C_Server::C_Server(){
    creator = new I_SocketCreator();

    m_socket = creator->MakeSocket("UDP");
}



/*****************************************************************************
 * Закрытие сокетета
 *
 * Прекращение соединения сокета с сервером и его закрытие.
 *
 */
bool C_Server::flush(){
    bool discRes = false;
    bool flushRes = false;

    discRes = m_socket->close();
    if ( discRes ){
        std::cout << m_socket->name() << ": Socket diconnected successfully.\n";
    }
    else {
        std::cout << m_socket->name() << ": BAD Socket diconnect.\n";
    }

    flushRes = m_socket->flush();
    if ( discRes && flushRes ){
        std::cout << m_socket->name() << ": Socket closed successfully.\n";
    }
    else {
        std::cout << m_socket->name() << ": BAD Socket closed.\n";
    }

    return discRes && flushRes;
}

/*****************************************************************************
 * Запуск сокета сервера
 *
 * Функция запускает сокет - m_socket.setupSock,
 * настраивает подключение - m_socket.socketSettings,
 * открывает на установленном адресе соединие - m_socket.openSock.
 * При возникновении проблемы в одном из этапов метод возвращает false - информируя
 * о неудачном запуске.
 *
 * @param
 *   [in]    a_addr     - ip-адрес на котором открыт или необходимо открыть сокет сервера.
 *           a_protocol - протокол ip-соединения.
 *           a_type     - тип соединения.
 *           a_protocol - протокол соединения.
 *           a_ipFamily - тип домена сокета.
 *           a_optFlag  - флаговая переменная.
 *
 * @return
 *  true - setupConnect успешно выполнена.
 *  false - ошибка.
 *
 */
bool C_Server::setup( std::pair<std::string, short> a_conParam, int a_optFlag ){

    bool setupRes    = false;
    bool openRes     = false;

    // Инициализация сокета клиента
    setupRes = m_socket->setup( a_conParam, a_optFlag );
    if ( setupRes ){
        std::cout << m_socket->name() << ": Socket created \n";
    }
    else {
        std::cout << m_socket->name() << ": Socket crating has Failed.\n";
    }

    // Попытка открытия сокета клиента
    openRes = m_socket->open();
    if ( openRes ) {
        std::cout << m_socket->name() << ": Socket opened \n";
    }
    else { std::cout << m_socket->name() << ": Socket opening has Failed.\n"; }

    return setupRes && openRes;
}

/*****************************************************************************
 * Работа сервера
 *
 * Парметр передаваемый в функцию communication может быть выбран в зависимости
 * от необходимого размера буфера для получаемых данных. По завешению функция возвращает
 * true если обмен данными состоялся успешно и сокет успешно закрылся.
 *
 * @param
 *  [in] a_buffSize - размер буфера принимающего сообщение.
 *
 * @return
 *  true - workingSession успешно выполнена.
 *  false - ошибка при обмене данными либо закрытии сокета.
 *
 */
bool C_Server::workingSession()
{
    bool workRes = false;
    bool flushRes = false;

    std::cout << m_socket->name() << ": communication started.\n";

    // Запуск комуникации с сервером
    workRes = communication( m_servBufferSize );
    if( workRes ){
        std::cout << m_socket->name() << ": Start communacation successfully.\n";
    } else {
        std::cout << m_socket->name() << ": BAD Start communacation.\n";
    }

    flushRes = flush();


    return workRes && flushRes;
}


/*****************************************************************************
 * Обмен данными с клиентом
 *
 * Пока клиент не закроет соединение соервер отсылает на каждый запрос клиента
 * случайное число генерируемое в интервале от lowBracket до topBracket.
 *
 * @param
 * [in] a_buffSize - размер буфера в которйы будут передоваться полученные из сокета
 *                   данные.
 *
 * @return
 *  true - communication успешно выполнена.
 *  false - ошибка при обмене данными либо закрытии сокета.
 *
 */
bool C_Server::communication( int a_buffSize ) {
    std::cout << m_socket->name() << ": Start communication..." << '\n';

    char buffer[a_buffSize];    // Буфер для полученных данных



    std::random_device  rd;
    std::mt19937        gen(rd());
    std::uniform_int_distribution<int> dist( -100, 100 );

    // Очищение буфера
    ZeroMemory( &buffer, a_buffSize );

    while( true ) {

        std::cout << m_socket->name() << ": Waiting for data...\n";
        fflush(stdout);

        // Генерируется случайное число
        int num = dist(gen);
        int recvSize = 0;
        int sendSize = 0;
        bool recvRes = false;
        bool sendRes = false;

        // Получение запроса от клиента
        recvRes = m_socket->recv( buffer, sizeof(buffer), &recvSize );
        if ( recvRes ){
            std::cout << m_socket->name() << ": Recived message from " << '\n';
        }

        // Отправка ответа клиенту
        sendRes = m_socket->send( (char *)&num, sizeof(num), &sendSize );
        if ( sendRes ){
            std::cout << m_socket->name() << ": Sent message to " << '\n';
        }
    }

    return true;
}



/*****************************************************************************
  Functions Prototypes
*****************************************************************************/

/*****************************************************************************
  Variables Definitions
*****************************************************************************/

/*****************************************************************************
  Functions Definitions
*****************************************************************************/

} // namespace myTask




