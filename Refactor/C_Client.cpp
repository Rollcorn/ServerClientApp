/*****************************************************************************

  C_CLient

  Класс предоставляет функционал сетевого клиента.




*****************************************************************************/
#include "C_Client.h"

namespace client {

/*****************************************************************************
  Macro Definitions
*****************************************************************************/

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
bool client::C_Client::setSockName( std::string a_sockName )
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
 *
 */
bool C_Client::setupConnect( std::string a_ipAddr, int a_port, int a_type,
                             int a_protocol, int a_ipFamily, int a_optFlag )
{

    bool setupRes = false;
    bool settingsRes =  false;

    // Инициализация сокета клиента
    if (setupRes = m_socket.setupSock(a_type, a_protocol, a_ipFamily ))
        std::cout << m_socket.name() << ": Socket created \n";
    else std::cout << m_socket.name() << ": Socket crating has Failed.\n";

    // Настройка соединения
    if (settingsRes = m_socket.socketSettings(a_ipAddr, a_port, a_optFlag ))
        std::cout << m_socket.name() << ": Socket applied settings.\n";
    else std::cout << m_socket.name() << ": Socket appling settings has Failed.\n";

    return (settingsRes && setupRes);
}


/*****************************************************************************
 * Работы сервера
 *
 * В функции задаетются параметры protocol - ip-протокоо и addr - адрес соединия с
 * сервером необходимые функции setupConnect. inet_addr преобразует ip-адрес переданный
 * в кострукторе строковым литералом в целочисленный тип. Парметр передаваемый в функцию
 * communication может быть выбран в зависимости от необходимого размера буфера для
 * получаемых данных.
 *
 *
 */
bool C_Client::workingSession()
{
    bool commRes = false;
    bool discRes = false;
    int messPerS     = 1;   // частота отправки запросов на сервевер (сообщений в секунду)
    int workDuration = 30;  // длительность работы клиента

    std::cout << m_socket.name() << ": communication started.\n";

    // запуск сетевого взяимодействия со стороны клиента
    if ( commRes = communication( messPerS, workDuration ) )
        std::cout << m_socket.name() << ": communication successfuly finished.\n";

    // закрытие сокета
    if ( discRes = m_socket.disconnect() )
        std::cout << m_socket.name() << ": Socket closed.\n";

    return commRes && discRes;

}

/*****************************************************************************
 * Обмен данными с клиентом
 *
 * Пока клиент не закроет соединение соервер отсылает на каждый запрос клиента
 * случайное число генерируемое в интервале от lowBracket до topBracket. В
 * функции reciveData явно прередан параметр messageLen являющийся размером
 * типа int так как в данной реализации ожидается получение от сервера целочисленного
 * значения.
 *
 * @param
 * [in] a_messPerSec   - количество запросов отправляемых на сервер в секунду
 *      a_workDuration - длительность работы клиента (длительность отправки
 *                       запрсов) в секундах.
 *
 */
bool C_Client::communication(int a_messPerSec, int a_workDuration)
{
    char buffer[m_BufSize];

    char *message    = "Give me a number!";
    int   messageLen = sizeof(int);
    int   sendSize   = 0;
    int   recvSize   = 0;

    auto        start    = std::chrono::steady_clock::now();
    auto        end      = std::chrono::steady_clock::now();
    sockaddr_in *seraddr = m_socket.servAddr();
    std::chrono::duration<double> curentWorkTime = end - start;

    while(curentWorkTime.count() < a_workDuration)
    {
            ZeroMemory(&buffer, sizeof(buffer));

            m_socket.sendData(seraddr, message, strlen(message), &recvSize );
            std::cout << m_socket.name() << ": Send message to " << inet_ntoa( m_socket.servAddr()->sin_addr )
                      << " : " << ntohs( m_socket.servAddr()->sin_port ) << '\n';

            m_socket.reciveData(seraddr, buffer, messageLen, &sendSize );
            std::cout << m_socket.name() << ": Recived message from " << inet_ntoa( m_socket.servAddr()->sin_addr )
                      << " : " << ntohs( m_socket.servAddr()->sin_port ) << '\n';

            std::cout << m_socket.name() << ": Get number: " << static_cast<int>(*buffer) << '\n';

            end = std::chrono::steady_clock::now();
            curentWorkTime = end - start;

            Sleep(a_messPerSec*1000);
    }

    return true;
}

/*****************************************************************************
 * Конструктор
 */
C_Client::C_Client( ){}

/*****************************************************************************
  Functions Prototypes
*****************************************************************************/

/*****************************************************************************
  Variables Definitions
*****************************************************************************/

static const int s_servPort = 8080;
static const char* s_servIpAddr = "127.0.0.1";

/*****************************************************************************
  Functions Definitions
*****************************************************************************/

} // namespace client

