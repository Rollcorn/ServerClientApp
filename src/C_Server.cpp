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
C_Server::C_Server(){}


/*****************************************************************************
 * Деструктор
 */
C_Server::~C_Server(){
    delete m_socket;
}


/*****************************************************************************
 * Запуск сокета сервера
 *
 * Функция запускает сокет - m_socket.setup, открывает на установленном адресе
 * соединие - m_socket.open. При возникновении проблемы в одном из этапов
 * метод возвращает false.
 *
 * @param
 *   [in]    a_conParam - пара значений first - ip-адрес на котором необходимо
 *                        открыть сокет сервера, second - порт соединения.
 *           a_optFlag  - флаговая переменная опций сокета .
 *
 * @return
 *
 */
bool C_Server::setup( std::pair<std::string, short> a_conParam, int a_optFlag ){

    bool setupRes = false; // Результат запуска сокета
    bool openRes  = false; // Результат открытия соединения сокета

    // Инстанцируем необходимый  сокет
    m_socket = CreateSocket("UDP");

    std::cout << " Server setup " << '\n';
    // Инициализация сокета клиента
    setupRes = m_socket->setup( a_conParam, m_socket->ownSockAdr(), a_optFlag );
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

    bool recvRes = true;
    bool sendRes = true;

    int recvSize = 0;
    int sendSize = 0;

    char buffer[a_buffSize]; // Буфер для полученных данных

    std::random_device  rd;
    std::mt19937        gen(rd());
    std::uniform_int_distribution<int> dist( -100, 100 );


    std::cout << m_socket->name() << ": Server Start communication...\n" << std::endl;

    while( recvRes && sendRes ) {
        recvRes = false;
        sendRes = false;

        // Очищение буфера
        ZeroMemory( &buffer, a_buffSize );

        std::cout << m_socket->name() << ": Waiting for data...\n";
        fflush(stdout);

        // Генерируется случайное число
        int num = dist(gen);

        // Попытка получения запроса от клиента
        recvRes = m_socket->recv( buffer, a_buffSize, &recvSize );
        if ( recvRes ){
            std::cout << m_socket->name() << ": Recived message size: " << recvSize << '\n';
        } else {
            std::cout << m_socket->name() << ": BAD Reciev in Server" << std::endl;
        }

        // Попытка отправки ответа клиенту на его запрос
        if ( recvRes ) { sendRes = m_socket->send( (char *)&num, sizeof(num), &sendSize ); }
        if ( sendRes ) {
            std::cout << m_socket->name() << ": Sent message size: " << sendSize << '\n';
        } else {
            std::cout << m_socket->name() << ": BAD Send in Server" << std::endl;
        }
    }

    return true;
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

    // Попытка закрытия соединения
    discRes = m_socket->close();
    if ( discRes ){
        std::cout << m_socket->name() << ": Socket diconnected successfully.\n";
    }
    else {
        std::cout << m_socket->name() << ": BAD Socket diconnect.\n";
    }

    // Попытка освобождения ресурсов соединения
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
  Functions Prototypes
*****************************************************************************/


/*****************************************************************************
  Variables Definitions
*****************************************************************************/


/*****************************************************************************
  Functions Definitions
*****************************************************************************/

} // namespace myTask




