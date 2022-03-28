/*****************************************************************************

  C_Server

  Реализация сетвеого сервера


  РЕАЛИЗАЦИЯ

  * Функция communication генрерирует случайное число и отправляет его на запрос
    клиента. Интервал в котором генерируется число задается от lowBracket до
    topBracket и может быть изменен в этих переменных.

*****************************************************************************/

#include "C_Server.h"

/*****************************************************************************
  Macro Definitions
*****************************************************************************/

namespace myTask {
/*****************************************************************************
  Forward Declarations
*****************************************************************************/

/*****************************************************************************
  Types and Classes Definitions
*****************************************************************************/

/*****************************************************************************
 * Конструктор
 */
C_Server::C_Server()
{
}

/*****************************************************************************
 * Деструктор
 */
C_Server::~C_Server()
{
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
 *   [in]    a_conParam - вектор пар значений.
 *           a_optFlag  - флаговая переменная опций сокета .
 *
 * @return
 *
 */
bool C_Server::setup( std::map<std::string, std::string> a_conParam )
{
    bool setupRes = false; // Результат запуска сокета
    bool openRes  = false; // Результат открытия соединения сокета

    // Инстанцируем необходимый  сокет
    m_socket = CreateSocket("UDP");

    // Инициализация параметров соединения
    m_ownIp   = a_conParam.at("ownIp");
    m_ownPort = a_conParam.at("ownPort");
    m_remIp   = a_conParam.at("remIp");
    m_remPort = a_conParam.at("remPort");

    m_blocking = std::stoi( a_conParam.at("block") );

    // Инициализация сокета клиента
    setupRes = m_socket->setup( m_ownIp, m_ownPort, 1 );
    if (setupRes) {
        std::cout << m_socket->name() << ":\tServer Socket CREATING SUCCESS." << std::endl;
    }
    else {
        std::cout << m_socket->name() << ":\tServer Socket CREATING FAILED." << std::endl;
    }

    // Попытка открытия сокета клиента
    openRes = m_socket->open();
    if (openRes) {
        std::cout << m_socket->name() << ":\tServer Socket OPENING SUCCESS." << std::endl;
    }
    else {
        std::cout << m_socket->name() << ":\tServer Socket OPENING FAILED." << std::endl;
    }

    std::cout << "SERVER: own-" << m_ownIp << ":" << m_ownPort << " rem-"
              << m_remIp << ":" << m_remPort << std::endl;

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
 *  успешность проведения сетевого взаимодействия
 */
bool C_Server::workingSession()
{
    bool workRes = false;
    bool flushRes = false;

    std::cout << m_socket->name() << ":\tSERVER COMUNICATION STARTED.\n";

    // Попытка запуска комуникации с сервером
    workRes = communication();
    if( workRes ) {
        std::cout << m_socket->name() << ":\tServer Start communacation SUCCESS. " << std::endl;
    } else {
        std::cout << m_socket->name() << ":\tServer communacation FAILED. " << std::endl;
    }

    // Попытка закрыть соединение и освободить ресурсы соединения
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
 *  корректность обмена данными с удаленным сокетом
 */
bool C_Server::communication()
{
    bool recvRes = true;
    bool sendRes = true;

    int recvSize = 0;
    int sendSize = 0;

    std::vector<char> buffer(m_bufferSize); // Буфер для полученных данных

    std::random_device  rd;
    std::mt19937        gen( rd() );
    int a = -100, b = 100;
    std::uniform_int_distribution<int> dist( a, b );

    while ( sendRes && recvRes ) {
        recvRes = false;
        sendRes = false;

        // Очищение буфера
        std::fill( buffer.begin(), buffer.end(), '\0' );

        std::cout << m_socket->name() << ":\tServer Waiting for data... "
                  << std::endl;
        fflush(stdout);

        // Попытка получения запроса от клиента
        recvRes = m_socket->recv( m_ownIp, m_ownPort, buffer, recvSize );
        if (recvRes) {
            std::cout << m_socket->name() << ":\tServer Recived message: {"
                      << buffer.data()
                      << "} size: " << recvSize << " " << std::endl;
        }
        else {
            std::cout << m_socket->name() << ":\tBAD Reciev on Server "
                      << std::endl;
        }

        // Генерируется случайное число
        int num = dist(gen);

        std::string message = std::to_string(num);
        std::vector<char> messageVec = {message.begin(), message.end() };
        messageVec.push_back('\0');
        messageVec.resize(message.size() + 1);
//      Попытка отправки ответа клиенту на его запрос
        if (recvRes) {
            sendRes = m_socket->send( m_remIp, m_remPort, messageVec, sendSize );
        }
        if (sendRes) {
            std::cout << m_socket->name() << ":\tServer Sent message {"
                      << messageVec.data() <<  "} Send size: " << sendSize << " "
                      << std::endl;
        }
        else {
            std::cout << m_socket->name() << ":\tBAD Send on Server " << std::endl;
        }
    }

    return recvRes && sendRes;
}

/*****************************************************************************
 * Закрытие сокетета
 *
 * Прекращение соединения сокета с сервером и его закрытие.
 *
 * @return
 *  успешность закрытия соединения
 */
bool C_Server::flush()
{
    bool discRes = false;
    bool flushRes = false;

    // Попытка закрытия соединения
    discRes = m_socket->close();
    if (discRes) {
        std::cout << m_socket->name()
                  << ":\tServer Socket diconnected successfully. " << std::endl;
    }
    else {
        std::cout << m_socket->name() << ":\tBAD Socket diconnect. " << std::endl;
    }

    // Попытка освобождения ресурсов соединения
    flushRes = m_socket->flush();
    if ( discRes && flushRes ) {
        std::cout << m_socket->name() << ":\tSERVER SOCK CLOSED SUCCESS. " << std::endl;
    }
    else {
        std::cout << m_socket->name() << ":\tBAD Socket closed on Server. " << std::endl;
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




