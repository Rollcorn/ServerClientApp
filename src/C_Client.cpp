/*****************************************************************************

  C_CLient

  Предоставляет реализацию сетевого клиента.


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
 * Конструктор
 */
C_Client::C_Client() {}


/************************************************************************
 * Деструктор
 */
C_Client::~C_Client() {
    delete m_creator;
    delete m_socket;
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
bool C_Client::setup( std::pair<std::string, short> a_conParam,
                            int a_optFlag ) {

    bool setupRes = false; // Результат запуска сокета
    bool openRes  = false; // Результат открытия соединения сокета

    // Попытка инициализации сокета клиента
    setupRes = m_socket->setup( a_conParam, a_optFlag );
    if (setupRes) {
        std::cout << m_socket->name() << ": Socket created \n";
    }
    else { std::cout << m_socket->name() << ": Socket crating has Failed.\n"; }

    // Попытка открытия сокета клиента
    openRes = m_socket->open();
    if (openRes) {
        std::cout << m_socket->name() << ": Socket opened \n";
    }
    else { std::cout << m_socket->name() << ": Socket opening has Failed.\n"; }

    return setupRes && openRes;
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
bool C_Client::workingSession( int a_messPerSec, int a_workDuration ) {
    bool commRes = false;
    bool discRes = false;


    std::cout << m_socket->name() << ": communication started.\n";

    // запуск сетевого взяимодействия со стороны клиента
    if ( commRes = communication( a_messPerSec, a_workDuration ) ) {
        std::cout << m_socket->name() << ": communication successfuly finished.\n";
    }
    // закрытие сокета
    if ( discRes = m_socket->flush() ) {
        std::cout << m_socket->name() << ": Socket closed.\n";
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
bool C_Client::communication( int a_messPerSec, int a_workDuration ) {
    bool sendRes = false;
    bool recvRes = false;

    char buffer[m_BufSize];
    char *strMessage = "Give me a number!";
    int   messageLen = sizeof(int);
    int   sendSize   = 0;
    int   recvSize   = 0;

    auto        start    = std::chrono::steady_clock::now();
    auto        end      = std::chrono::steady_clock::now();
    std::chrono::duration<double> curentWorkTime = end - start;

    while( curentWorkTime.count() < a_workDuration ) {
            ZeroMemory(&buffer, sizeof(buffer));

            sendRes = m_socket->send( strMessage, strlen(strMessage), &recvSize );
            std::cout << m_socket->name() << ": Send message to " << '\n';

            recvRes = m_socket->recv( buffer, messageLen, &sendSize );
            std::cout << m_socket->name() << ": Recived message from " << '\n';

            std::cout << m_socket->name() << ": Get number: " << static_cast<int>(*buffer) << '\n';

            end = std::chrono::steady_clock::now();
            curentWorkTime = end - start;

            Sleep( a_messPerSec * 1000 );
    }

    return sendRes && recvSize;
}


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

