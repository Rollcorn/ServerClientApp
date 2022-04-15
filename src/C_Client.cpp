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
{}

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
 *   [in]   a_conParam - словарь содержащий парамметры конфигурации соединения.
 *
 * @return
 *  успешность создания сокета.
 */
bool C_Client::setup( ConParams a_conParam )
{
    bool setupRes = false; // Результат запуска клиента
    std::string protocol = a_conParam.at("transProt");
    // Инстанцируем необходимый  сокет
    m_socket = CreateSocket(protocol);
//    m_blocking = atoi( a_conParam.at("block").c_str() );

    // Попытка инициализации сокета клиента
    setupRes = m_socket->setup( a_conParam );
    if (setupRes) {
        std::cout << m_socket->name() << ":\tClient Socket CREATING SUCCESS"
                  << std::endl;
    }
    else {
        std::cout << m_socket->name() << ":\tClient Socket creating FAILED. Errno ["
                  << strerror(errno) << "]" << std::endl;
    }

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
 *  [in] a_workDuration - длительность работы клиента.
 *
 * @return
 *  корректность провдения сетевого взаимодействия
 */
bool C_Client::workingSession( int a_messPerSec, int a_workDuration )
{
    bool okComm = false;
    bool okDisc = false;

    std::cout << m_socket->name() << ":\tCLIENT COMUNICATION STARTED. " << std::endl;

    // Попытка запуска сетевого взяимодействия со стороны клиента
    okComm = communication( a_messPerSec, a_workDuration );
    if (okComm) {
        std::cout << m_socket->name() << ":\tClient Communication SUCCESS. " << std::endl;
    }
    else {
        std::cout << m_socket->name() << ":\tClient Communication FAILED. " << std::endl;
    }

    // Попытка закрытия сокета
    okDisc = m_socket->flush();
    if (okDisc) {
        std::cout << m_socket->name() << ":\tCLIENT SOCK CLOSED SUCCESS. " << std::endl;
    }

    return okComm && okDisc;
}

/*******************************************************************************
 * Попытка отправки сообщения
 *
 * @param
 *  [in] a_strMessage - данные для отправки.
 *
 * @return
 *  успешность отправки
 */
bool C_Client::send( std::string a_strMessage )
{
    bool okSend= true; // результат отправки данных

    std::vector<char> message = {a_strMessage.begin(), a_strMessage.end()};
    message.push_back( '\0' );
    // Вызов resize() важен чтобы не получить мусора в данных
    message.resize(a_strMessage.length() );

    okSend = m_socket->send( message, m_socket->remoteAddr());
    if(okSend){
        std::cout << m_socket->name() << ":\tClient Sent message ["
                  << message.data() << "], size: [" << message.size() << "]" << std::endl;
    } else {
        std::cout << m_socket->name() << ":\tBAD Send on Client. Errno ["
                  << strerror(errno) << "]" << std::endl;
        // TODO обработка errno
    }

    return okSend;
}

/*******************************************************************************
 * Попытка получения сообщения
 *
 */
bool C_Client::recv( std::vector<char> &a_buffer, std::string &a_fromAddr)
{
    bool okRecv = false;

    // Вызов fill() важен чтобы не получить мусора в данных
    std::fill( a_buffer.begin(), a_buffer.end(), '\0' );

    // Попытка получения запроса от клиента
    do {
        okRecv = m_socket->recv( a_buffer, a_fromAddr );
    } while( !okRecv || errno == ERR_BUFEMPT );

    if( okRecv ) {
        std::cout << m_socket->name() << ":\tClient Recived message ["
                  << a_buffer.data() << "], size: ["
                  << a_buffer.size() << "], from [" << a_fromAddr << "]" << std::endl;
    } else {
        if ( errno == ERR_BADRECV ){
            std::cout << m_socket->name() << ":\tBAD Reciev on Client Side." << std::endl;
        } else {
            std::cout << m_socket->name() << ":\tUnexpectred Error: " << strerror(errno) << std::endl;
        }
    }

    return okRecv;
}


/*****************************************************************************
 * Обмен данными с клиентом
 *
 * Пока клиент не отработает время равное a_workDuration секунд, он продолжит отправлять
 * каждую секунду запрос на сервер. Полученный ответ клиент логирует в консоль.
 * По истечении выделенного времени communication заверншает работу.
 *
 * @param
 *  [in] a_messPerSec   - количество запросов отправляемых на сервер в секунду
 *  [in] a_workDuration - длительность работы клиента (длительность отправки
 *                        запрсов) в секундах.
 *
 * @return
 *  корректность обмена данными с удаленным сокетом
 */
bool C_Client::communication( int a_messPerSec, int a_workDuration )
{
    bool    okSend = false; // результат отправки данных
    bool    okRecv = false; // Результат получения данных

    std::vector<char> buffer( s_bufferLen );  // Буфер для полученных данных

    // Определение таймера работы клиента
    auto start  = std::chrono::steady_clock::now();
    auto end    = start;
    std::chrono::duration<double> curentWorkTime = end - start;

    auto lastSendTime  = start - s_sendTimout;

    do
    {
         auto currTime = std::chrono::steady_clock::now();

        // Попытка отправки запроса серверу
        if (currTime - lastSendTime  >= std::chrono::milliseconds(s_sendTimout) ){
            okSend = send(s_getNumMessage);
            lastSendTime  = std::chrono::steady_clock::now();
        } else {
            end = std::chrono::steady_clock::now();
            curentWorkTime = end - start;
            continue;
        }


        // Попытка получения сообщения от сервера
        std::string fromAddr;

        if (okSend){
            okRecv = recv( buffer, fromAddr );

            Sleep( a_messPerSec * 1000 ); // Выдержка в 1 секнду

            // Обновление таймера
            end = std::chrono::steady_clock::now();
            curentWorkTime = end - start;
        }

    } while ( okSend && okRecv && (curentWorkTime.count() <= a_workDuration) );

    bool okEndConn = send(s_endConnMessage);

    return okSend && okRecv && okEndConn;
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

