/*****************************************************************************

  CLASS C_Server

  Класс предоставляет функционал для работы сервера


  РЕАЛИЗАЦИЯ фнкция communication генрерирует случайное число и отправляет его на запрос
  клиента. Интервал в котором генерируется число задается от lowBracket до topBracket и
  может быть изменен в этих переменных.


*****************************************************************************/

/*****************************************************************************
  Macro Definitions
*****************************************************************************/
#include "C_Server.h"

namespace server { // TODO

/*****************************************************************************
  Forward Declarations
*****************************************************************************/

/*****************************************************************************
  Types and Classes Definitions
*****************************************************************************/

/*****************************************************************************
 * Задание имени для сокета
 *
 * Назначает сокету определенное имя, для идентификации действий сокета при
 * выводе лога.
 *
 * @param
 *   [in]    a_sockName     - имя сокета
 */
bool C_Server::setSockName(std::__cxx11::string a_sockName)
{
    m_socket.setName( a_sockName );
}

/*****************************************************************************
 * Запуск сокета сервера
 *
 * Функция последовательно вызывает необходимые методы объекта m_socket предоставляющего
 * интрефейс для работы сокета
 *
 * @param
 *   [in]    a_addr     - ip-адрес на котором открыт или необходимо открыть сокет сервера.
 *           a_protocol - протокол ip-соединения.
 */
bool C_Server::setupConnect( std::string a_ipAddr, int a_port, int a_type,
                             int a_protocol, int a_ipFamily, int a_optFlag )
{
    bool setupRes    = false;
    bool settingsRes = false;
    bool openRes     = false;

    // Инициализация сокета клиента
    if ( setupRes = m_socket.setupSock( a_type, a_protocol, a_ipFamily ) )
        std::cout << m_socket.name() << ": Socket created \n";
    else std::cout << m_socket.name() << ": Socket crating has Failed.\n";

    // Настройка сокета
    if ( settingsRes = m_socket.socketSettings( a_ipAddr, a_port, a_optFlag ) )
         std::cout << m_socket.name() << ": Socket applied settings.\n";
    else std::cout << m_socket.name() << ": Socket appling settings has Failed.\n";

        // Cвязывание сокет с локальным адресом протокола
    if ( openRes = m_socket.openSock() )
         std::cout << m_socket.name() << ": Bind done\n";
    else std::cout << m_socket.name() << ": Socket binding has Failed.\n";

    return setupRes && settingsRes && openRes;
}

/*****************************************************************************
 * Запуск работы сервера
 *
 * В функции задаетются параметры protocol - ip-протокоо и addr - адрес соединия с
 * сервером необходимые функции setupConnect. Парметр передаваемый в функцию communication
 * может быть выбран в зависимости от необходимого размера буфера для получаемых данных.
 *
 * @param
 *  [in] a_buffSize - размер буфера принимающего сообщение.
 */
bool C_Server::workingSession(int a_buffSize)
{
    bool workRes = false;
    std::cout << m_socket.name() << ": communication started.\n";

    // Запуск комуникации с сервером
    if( workRes = communication( a_buffSize ) );

    //закрытие сокета
    if ( m_socket.disconnect())
        std::cout << m_socket.name() << ": Socket closed.\n";

    return workRes;
}


/*****************************************************************************
 * Обмен данными с клиентом
 *
 * Пока клиент не закроет соединение соервер отсылает на каждый запрос клиента
 * случайное число генерируемое в интервале от lowBracket до topBracket.
 *
 * @param
 * [in] a_buffSize - размер буфера в которйы будут передоваться полученные из сокета
 *                      данные.
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
    std::uniform_int_distribution<int> dist(-100, 100);

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




