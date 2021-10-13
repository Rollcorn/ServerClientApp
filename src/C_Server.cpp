/*****************************************************************************

  C_Server

  Класс предоставляет функционал для работы сервера


  РЕАЛИЗАЦИЯ

  Функция communication генрерирует случайное число и отправляет его на запрос
  клиента. Интервал в котором генерируется число задается от lowBracket до topBracket и
  может быть изменен в этих переменных.


*****************************************************************************/

#include "C_Server.h"

/*****************************************************************************
  Macro Definitions
*****************************************************************************/

namespace server { // TODO

/*****************************************************************************
  Forward Declarations
*****************************************************************************/

/*****************************************************************************
  Types and Classes Definitions
*****************************************************************************/

/*****************************************************************************
 * Задание имени сокета
 *
 * Назначает сокету определенное имя, для идентификации действий сокета при
 * создании лога.
 *
 * @param
 *   [in]    a_sockName     - имя сокета
 *
 * @return
 *  true - setSockName успешно выполнена.
 *  false - ошибка.
 *
 */
bool C_Server::setSockName( std::__cxx11::string a_sockName )
{
    m_socket.setName( a_sockName );
    return true;
}

/*****************************************************************************
 * Закрытие сокетета
 *
 * Прекращение соединения сокета с сервером и его закрытие.
 *
 */
bool C_Server::flushConnect()
{
    return m_socket.flushSock();
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
bool C_Server::setupConnect( std::string a_ipAddr, int a_port, int a_type,
                             int a_protocol, int a_ipFamily, int a_optFlag )
{
    bool setupRes    = false;
    bool settingsRes = false;
    bool openRes     = false;

    // Инициализация сокета клиента
    if ( setupRes = m_socket.setupSock( a_type, a_protocol, a_ipFamily ) ){
        std::cout << m_socket.name() << ": Socket created \n";
    }
    else {
        std::cout << m_socket.name() << ": Socket crating has Failed.\n";
    }

    // Настройка сокета
    if ( settingsRes = m_socket.socketSettings( a_ipAddr, a_port, a_optFlag ) ) {
         std::cout << m_socket.name() << ": Socket applied settings.\n";
    }
    else {
        std::cout << m_socket.name() << ": Socket appling settings has Failed.\n";
    }

    // Cвязывание сокет с локальным адресом протокола
    if ( openRes = m_socket.openSock() ) {
        std::cout << m_socket.name() << ": Bind done\n";
    }
    else {
        std::cout << m_socket.name() << ": Socket binding has Failed.\n";
    }

    return setupRes && settingsRes && openRes;
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
bool C_Server::workingSession( int a_buffSize )
{
    bool workRes = false;
    bool discRes = false;
    bool closeRes = false;
    std::cout << m_socket.name() << ": communication started.\n";

    // Запуск комуникации с сервером
    workRes = communication( a_buffSize );
    if( workRes ){
        std::cout << m_socket.name() << ": Start communacation successfully.\n";
    } else {
        std::cout << m_socket.name() << ": BAD Start communacation.\n";
    }

    closeRes = flushConnect();
    if ( closeRes ){
        std::cout << m_socket.name() << ": Socket closed successfully.\n";
    }
    else {
        std::cout << m_socket.name() << ": BAD Socket closed.\n";
    }

    return workRes && discRes && closeRes;
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
bool C_Server::communication( int a_buffSize )
{
    std::cout << m_socket.name() << ": Start communication..." << '\n';

    char buffer[a_buffSize];    // Буфер для полученных данных

    // Инициализация генератора случайных чисел
    int lowBracket = -100;
    int topBracket = 100;

    std::random_device  rd;
    std::mt19937        gen(rd());
    std::uniform_int_distribution<int> dist( -100, 100 );

    // Очищение буфера
    ZeroMemory( &buffer, a_buffSize );

    while( true )
    {

        std::cout << m_socket.name() << ": Waiting for data...\n";
        fflush(stdout);

        // Генерируется случайное число
        int num = dist(gen);
        int recvSize = 0;
        int sendSize = 0;

        // Получение запроса от клиента
        if ( m_socket.reciveData( m_socket.clientAddr(), buffer, sizeof(buffer), &recvSize ) )
            std::cout << m_socket.name() << ": Recived message from " << inet_ntoa( m_socket.clientAddr()->sin_addr )
                      << " : " << ntohs( m_socket.clientAddr()->sin_port ) << '\n';

        // Отправка ответа клиенту
        if ( m_socket.sendData( m_socket.clientAddr(), (char *)&num, sizeof(num), &sendSize ) )
            std::cout << m_socket.name() << ": Sent message to " << inet_ntoa( m_socket.clientAddr()->sin_addr )
                      << " : " << ntohs( m_socket.clientAddr()->sin_port ) << '\n';

    }

    return true;
}

/*****************************************************************************
 * Конструктор
 */
C_Server::C_Server()
{}

/*****************************************************************************
  Functions Prototypes
*****************************************************************************/

/*****************************************************************************
  Variables Definitions
*****************************************************************************/

/*****************************************************************************
  Functions Definitions
*****************************************************************************/

} // namespace server TODO




