/*****************************************************************************

  C_CLient

  Реализация сетевого клиента.

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
C_Client::C_Client()
{
}

/************************************************************************
 * Деструктор
 */
C_Client::~C_Client()
{
    delete m_socket;
}

/*****************************************************************************
 * Создание/запуск сокета клиента
 *
 * Осуществляет попытку создания объекта обмена данными - сокет, на указанном
 * адресе - ip + порт, и сообщает об об успешности этой завершения операции.
 *
 * @param
 *   [in]    a_conParam - пара значений: first - ip-адрес на котором открывать сокет,
 *                                       second - порт открыть сокет этого сокета.
 *           a_protocol - протокол ip-соединения.
 *
 * @return
 *  успешность создания сокета.
 */
bool C_Client::setup( std::map<std::string, std::string> a_conParam )
{
    bool setupRes = false; // Результат запуска сокета

    // Инстанцируем необходимый  сокет
    m_socket = CreateSocket("UDP");

    // Инициализация параметров соединения
    m_ownIp              = a_conParam.at("ownIp");
    m_ownPort            = a_conParam.at("ownPort");
    m_remIp              = a_conParam.at("remIp");
    m_remPort            = a_conParam.at("remPort");

    m_blocking = atoi( a_conParam.at("block").c_str() );

    // Попытка инициализации сокета клиента
    setupRes = m_socket->setup( m_ownIp, m_ownPort, m_blocking );
    if (setupRes) {
        std::cout << m_socket->name() << ": Client Socket creating SUCCESS" << std::endl;
    }
    else { std::cout << m_socket->name() << ": Client Socket crating FAILED." << std::endl; }

    return setupRes;
}

/*****************************************************************************
 * Работа клиента
 *
 * Парметры a_messPerSec и a_workDuration передаваются в функцию communication и
 * выбираются в зависимости от необходимой длительности работы клиента.
 * По истечении времени a_workDuration клиент закрывает сокет и функция возвращает
 * true если обмен данными состоялся успешно и сокет успешно закрылся.
 *
 * @param
 *  [in] a_messPerSec - частота отправления запросов.
 *       a_workDuration - длительность работы клиента.
 *
 * @return
 *  корректность провдения сетевого взаимодействия
 */
bool C_Client::workingSession( int a_messPerSec, int a_workDuration )
{
    bool commRes = false;
    bool discRes = false;

    std::cout << m_socket->name() << ": Client Communication started. " << std::endl;

    // Попытка запуска сетевого взяимодействия со стороны клиента
    commRes = communication( a_messPerSec, a_workDuration );
    if (commRes) {
        std::cout << m_socket->name() << ": Client Communication SUCCESS. " << std::endl;
    }
    else {
        std::cout << m_socket->name() << ": Client Communication FAILED. " << std::endl;
    }

    // Попытка закрытия сокета
    discRes = m_socket->flush();
    if (discRes) {
        std::cout << m_socket->name() << ": Client Socket closed SUCCESS. " << std::endl;
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
 *  корректность обмена данными с удаленным сокетом
 */
bool C_Client::communication( int a_messPerSec, int a_workDuration )
{
    bool  sendRes = true; // результат отправки данных
    bool  recvRes = true; // Результат получения данных

    char        buffer[m_BufSize];
    std::string strMessage = "Give me a number!"; // Запрос клиента
    int         messageLen = sizeof(int);

    int   sendSize   = 0; // Размер отправленных данных
    int   recvSize   = 0; // Размер принятых данных

    // Объявление таймера работы клиента
    auto start  = std::chrono::steady_clock::now();
    auto end    = std::chrono::steady_clock::now();
    std::chrono::duration<double> curentWorkTime = end - start;

    std::cout << m_socket->name() << ": Client Start communication...\n" << std::endl;

    while ( ( curentWorkTime.count() < a_workDuration ) && sendRes && recvRes ) {

        // Очищение буфера
        ZeroMemory( buffer, m_BufSize );

        // Попытка отправки запроса серверу
        sendRes = m_socket->send( m_remIp, m_remPort, &strMessage[0], &sendSize );
        if(sendRes){
            std::cout << m_socket->name() << ": Client Sent message size: " << sendSize << std::endl;
        }
        else {
            std::cout << m_socket->name() << ": BAD Send on Client" << std::endl;
        }

        // Попытка получения сообщения от сервера
        recvRes = m_socket->recv( m_remIp, m_remPort, buffer, &recvSize );
        if(recvRes) {
            std::cout << m_socket->name() << ": Client Recived message size: " << recvSize << std::endl;
            std::cout << m_socket->name() << ": Get number: " << static_cast<int>(*buffer) << std::endl;
        }
        else {
            std::cout << m_socket->name() << ": BAD Reciev on Client" << std::endl;
        }

        Sleep( a_messPerSec * 1000 ); // Выдержка в 1 секнду

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

} // namespace myTask

