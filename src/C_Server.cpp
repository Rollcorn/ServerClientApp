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
 * Запускает сокет - m_socket.setup, открывает на установленном адресе
 * соединие - m_socket.open. При возникновении проблемы в одном из этапов
 * метод возвращает false.
 *
 * @param
 *   [in] a_conParam - контейнер и данными конфигурации соединения.
 *
 * @return
 *  успешность запуска
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
    setupRes = m_socket->setup( a_conParam );
    if (setupRes) {
        std::cout << m_socket->name() << ":\tServer Socket CREATING SUCCESS." << std::endl;
    } else {
        std::cout << m_socket->name() << ":\tServer Socket CREATING FAILED." << std::endl;
    }

    // Попытка открытия сокета клиента
    openRes = m_socket->open();
    if (openRes) {
        std::cout << m_socket->name() << ":\tServer Socket OPENING SUCCESS." << std::endl;
    } else {
        std::cout << m_socket->name() << ":\tServer Socket OPENING FAILED." << std::endl;
    }
    return setupRes && openRes;
}

/*****************************************************************************
 * Работа сервера
 *
 * Парметр передаваемый в функцию communication может быть выбран в зависимости
 * от необходимого размера буфера для получаемых данных. По завешению функция возвращает
 * true если обмен данными состоялся успешно и сокет успешно закрылся.
 *
 * @return
 *  успешность проведения сетевого взаимодействия
 */
bool C_Server::workingSession()
{
    bool okWork = false;
    bool flushRes = false;

    std::cout << m_socket->name() << ":\tSERVER COMUNICATION STARTED.\n";

    // Попытка запуска комуникации с сервером
    okWork = communication();
    if( okWork ) {
        std::cout << m_socket->name() << ":\tServer Start communacation SUCCESS. " << std::endl;
    } else {
        std::cout << m_socket->name() << ":\tServer communacation FAILED. "
                  << strerror(errno) << std::endl;
    }

    // Попытка закрыть соединение и освободить ресурсы соединения
    flushRes = flush();

    return okWork && flushRes;
}

/*****************************************************************************
 * Обмен данными с клиентом
 *
 * Пока клиент не закроет соединение соервер отсылает на каждый запрос клиента
 * случайное число генерируемое в интервале от lowBracket до topBracket.
 *
 * @return
 *  успешность проведенного обмена данными
 */
bool C_Server::communication()
{
    bool recvRes = true;
    bool sendRes = true;

    std::vector<char> buffer(s_bufferLen); // Буфер для полученных данных

    // Генерация случайного числа
    std::random_device  rd;
    std::mt19937        gen( rd() );
    int minRandNum = -100;
    int maxRandNum = 100;
    std::uniform_int_distribution<int> dist( minRandNum, maxRandNum );
    std::cout << "===========================================================" << std::endl;

    while ( !m_isEndConnSignal ) {
        recvRes = false;
        sendRes = false;

        // Очищение буфера
        std::fill( buffer.begin(), buffer.end(), '\0' );

        fflush(stdout);

        // Попытка получения запроса от клиента
        std::string fromAddr;
        recvRes = recv( buffer, fromAddr);

        m_isEndConnSignal = (buffer.data() == s_endConnMessage);

        // Попытка отправки ответа клиенту на его запрос
        std::vector<char> messageVec;

        if (!m_isEndConnSignal) {
            // Генерируется случайное число
            int num = dist(gen);

            // Подготовка сообщения
            std::string messageStr = std::to_string(num);
            messageVec = {messageStr.begin(), messageStr.end() };
            messageVec.push_back('\0');
            messageVec.resize(messageStr.length());

            sendRes = m_socket->send( messageVec, fromAddr );
        } else {
            sendRes = true;
        }

        if (sendRes && !m_isEndConnSignal) {
            std::cout << m_socket->name() << ":\tServer Sent message ["
                      << messageVec.data() <<  "] Send size: " << messageVec.size() << " "
                      << std::endl;
        }
        std::cout << "==========================================================="
                  << std::endl;
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
 * Отправка сообщения
 *
 * [in]
 *
 * @return
 *  успешность закрытия соединения
 */
bool C_Server::recv( std::vector<char> &a_buffer, std::string &a_fromAddr)
{
    bool okRecv= true;

    std::fill( a_buffer.begin(), a_buffer.end(), '\0' ); // Очищение буфера

    // Попытка получения запроса от клиента
    do {
        okRecv = m_socket->recv( a_buffer, a_fromAddr );
    } while( !(okRecv && errno != ERR_BUFEMPT) );

    if (okRecv) {
        std::cout << m_socket->name() << ":\tServer Recived message ["
                  << a_buffer.data() << "], size: ["
                  << a_buffer.size() << "], from [" << a_fromAddr << "]" << std::endl;
    } else {

        if ( errno == ERR_BADRECV ){
            std::cout << m_socket->name() << ":\tBAD Recieve on Server Side."
                      << std::endl;
        } else {
            std::cout << m_socket->name() << ":\tUnexpectred Error"
                      << ":\tUnexpectred Error: " << strerror(errno)<< std::endl;
        }
    }

    return okRecv;
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




