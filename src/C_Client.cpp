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
    delete m_socket;
}


/*****************************************************************************
 * Запуск сокета сервера
 *
 * Функция последовательно вызывает необходимые методы объекта m_socket предоставляющего
 * интрефейс для работы сокета
 *
 * @param
 *   [in]    a_conParam - пара значений: first - ip-адрес на котором открывать сокет,
 *                                       second - порт открыть сокет этого сокета.
 *           a_protocol - протокол ip-соединения.
 *
 *
 */
bool C_Client::setup( std::pair<std::string, short> a_conParam, int a_optFlag ) {

    bool setupRes = false; // Результат запуска сокета

    // Инстанцируем необходимый  сокет
    m_socket = CreateSocket("UDP");

    // Попытка инициализации сокета клиента
    setupRes = m_socket->setup( a_conParam, m_socket->remoteSockAdr(), a_optFlag );
    if (setupRes) {
        std::cout << m_socket->name() << ": Socket created" << std::endl;
    }
    else { std::cout << m_socket->name() << ": Socket crating has Failed." << std::endl; }

    return setupRes;
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
    bool sendRes = true; // результат отправки данных
    bool recvRes = true; // Результат получения данных

    char buffer[m_BufSize];
    char *strMessage = "Give me a number!"; // Запрос клиента
    int   messageLen = sizeof(int);

    int   sendSize   = 0; // Размер отправленных данных
    int   recvSize   = 0; // Размер принятых данных

    // Объявление таймера работы клиента
    auto start  = std::chrono::steady_clock::now();
    auto end    = std::chrono::steady_clock::now();
    std::chrono::duration<double> curentWorkTime = end - start;

    std::cout << m_socket->name() << ": Client Start communication...\n" << std::endl;

    while( ( curentWorkTime.count() < a_workDuration ) && sendRes && recvRes ) {

        // Очищение буфера
        ZeroMemory( &buffer, sizeof(*buffer) );
        std::cout << std::endl;

        // Попытка отправки запроса серверу
        sendRes = m_socket->send( strMessage, strlen(strMessage), &sendSize );
        if( sendRes ){
            std::cout << m_socket->name() << ": Sent message size: " << sendSize << std::endl;
        } else {
            std::cout << m_socket->name() << ": BAD Send in Client" << std::endl;
        }

        // Попытка получения сообщения от сервера
        recvRes = m_socket->recv( buffer, messageLen, &recvSize );
        if( recvRes ) {
            std::cout << m_socket->name() << ": Recived message size: " << recvSize << std::endl;
            std::cout << m_socket->name() << ": Get number: " << static_cast<int>(*buffer) << std::endl;
        } else {
            std::cout << m_socket->name() << ": BAD Reciev in Client" << std::endl;
        }

        // Выдержка в 1с
        Sleep( a_messPerSec * 1000 );
        // Обновление таймера
        end = std::chrono::steady_clock::now();
        curentWorkTime = end - start;
    }

    return sendRes && recvSize;
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

} // namespace client

