/*****************************************************************************

  C_CLient

  Предоставляет реализаци. сетевого клиента.


*****************************************************************************/
#include "C_Client.h"

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

/*****************************************************************************
 * Задание имени для сокета
 *
 * Назначает сокету определенное имя, для идентификации действий сокета при
 * выводе лога.
 *
 * @param
 *   [in]    a_sockName     - имя сокета
 */
bool C_Client::setSockName( std::string a_sockName )
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
    if ( setupRes = m_socket.setup( a_type, a_protocol, a_ipFamily ) ) {
        std::cout << m_socket.name() << ": Socket created \n";
    }
    else { std::cout << m_socket.name() << ": Socket crating has Failed.\n"; }

    // Настройка соединения
    if ( settingsRes = m_socket.setSettings( a_ipAddr, a_port, a_optFlag ) ) {
        std::cout << m_socket.name() << ": Socket applied settings.\n";
    }
    else { std::cout << m_socket.name() << ": Socket appling settings has Failed.\n"; }

    return ( settingsRes && setupRes );
}

/*****************************************************************************
 * Работа клиента
 *
 * Парметры a_messPerSec и a_workDuration передаваются в функцию communication и выбираются
 * в зависимости от необходимой длительности работы клиента.
 * По истечении времени a_workDuration клиент закрывает сокет и функция возвращает
 * true если обмен данными состоялся успешно и сокет успешно закрылся.
 *
 * @param
 *  [in] a_messPerSec - частота отправления запросов.
 *       a_workDuration - длительность работы клиента.
 *
 * @return
 *  true - workingSession успешно выполнена.
 *  false - ошибка при обмене данными либо закрытии сокета.
 *
 */
bool C_Client::workingSession( int a_messPerSec, int a_workDuration )
{
    bool commRes = false;
    bool discRes = false;


    std::cout << m_socket.name() << ": communication started.\n";

    // запуск сетевого взяимодействия со стороны клиента
    if ( commRes = communication( a_messPerSec, a_workDuration ) ) {
        std::cout << m_socket.name() << ": communication successfuly finished.\n";
    }
    // закрытие сокета
    if ( discRes = m_socket.flush() ) {
        std::cout << m_socket.name() << ": Socket closed.\n";
    }

    return commRes && discRes;

}

/*****************************************************************************
 * Обмен данными с клиентом
 *
 * Пока клиент не отработает время равное a_workDuration секунд, он продолжит отправлять
 * каждую секунду запрос на сервер. Полученный ответ клиент логирует в консоль.
 * По истечении выделенного времени communication заверншает работу.
 *
 * @param
 * [in] a_messPerSec   - количество запросов отправляемых на сервер в секунду
 *      a_workDuration - длительность работы клиента (длительность отправки
 *                       запрсов) в секундах.
 *
 * @return
 *  true - все отправки и все получения данных прошли успешно.
 *  false - ошибка при обмене данными либо закрытии сокета.
 *
 */
bool C_Client::communication(int a_messPerSec, int a_workDuration)
{
    bool sendRes = false;
    bool recvRes = false;

    char buffer[m_BufSize];
    char *strMessage    = "Give me a number!";
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

            sendRes = m_socket.send(seraddr, strMessage, strlen(strMessage), &recvSize );
            std::cout << m_socket.name() << ": Send message to " << inet_ntoa( m_socket.servAddr()->sin_addr )
                      << " : " << ntohs( m_socket.servAddr()->sin_port ) << '\n';

            recvRes = m_socket.recv(seraddr, buffer, messageLen, &sendSize );
            std::cout << m_socket.name() << ": Recived message from " << inet_ntoa( m_socket.servAddr()->sin_addr )
                      << " : " << ntohs( m_socket.servAddr()->sin_port ) << '\n';

            std::cout << m_socket.name() << ": Get number: " << static_cast<int>(*buffer) << '\n';

            end = std::chrono::steady_clock::now();
            curentWorkTime = end - start;

            Sleep( a_messPerSec * 1000 );
    }

    return sendRes && recvSize;
}


/*****************************************************************************
 * Конструктор
 */
C_Client::C_Client(){}

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

