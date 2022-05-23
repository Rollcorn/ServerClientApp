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
    delete m_connector;
}

/*****************************************************************************
 * Запуск сокета сервера
 *
 * Формат файла конфигурации (# - необязательный параметр, * - множественный
 * параметр, [] - необязательная часть)::
 *
 * + конфигурация I_Connection, см. I_Connection::setup() конкретных производных
 *   классов.
 *
 * transProt   = {строка}
 *               Тип используемого соединения.
 *               Перечень типов см. SocketCreator::CreateSocket()
 *
 * C_Server извлекает параметр transProt, указанного в конфигурации, и
 * осуществляет создания объекта типа I_Connection с помощью фабрики CreateSocket.
 * И передает этот файл конфигурации в метод setup() построенного
 * объекта I_Connection m_connector который инициализурется согласно параметрам
 * a_conParam.
 *
 * @param
 *  [in] a_conParam - конфигурация.
 *
 * @return
 *  успешность запуска
 */
bool C_Server::setup( const conf_t& a_conParam )
{
    bool setupRes = false; // Результат запуска сокета
    bool openRes  = false; // Результат открытия соединения сокета

    std::string protocol = findValByName( a_conParam, "transProt");
    // Инстанцируем необходимый  сокет
    m_connector = createSocket(protocol);

    // Инициализация сокета клиента
    setupRes = m_connector->setup( a_conParam );
    if (setupRes) {
        std::cout << m_connector->name() << ":\tServer Socket CREATING SUCCESS." << std::endl;
    } else {
        std::cout << m_connector->name() << ":\tServer Socket CREATING FAILED." << std::endl;
    }

    // Попытка открытия сокета клиента
    openRes = m_connector->open();
    if (openRes) {
        std::cout << m_connector->name() << ":\tServer Socket OPENING SUCCESS." << std::endl;
    } else {
        std::cout << m_connector->name() << ":\tServer Socket OPENING FAILED." << std::endl;
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

    std::cout << m_connector->name() << ":\tSERVER COMUNICATION STARTED." << std::endl ;

    // Попытка запуска комуникации с сервером
    okWork = communication();
    if( okWork ) {
        std::cout << m_connector->name() << ":\tServer Start communacation SUCCESS. " << std::endl;
    } else {
        std::cout << m_connector->name() << ":\tServer communacation FAILED. "
                  << strerror(errno) << std::endl;
    }

    // Попытка закрыть соединение и освободить ресурсы соединения
    flushRes = flush();

    return okWork && flushRes;
}

/****************************************************************************
 * Генерация случайного числа
 *
 * [in] a_min - нижняя граница генерирования
 * [in] a_max - верхняя граница генерирования
 *
 * @return
 */
std::string C_Server::genRandNumMess(const int a_min, const int a_max)
{
    static std::random_device  s_rd;
    static std::mt19937        s_gen( s_rd() );

    std::uniform_int_distribution<int> dist( a_min, a_max );

    // Генерируется случайное число
    int num = dist(s_gen);

    // Подготовка сообщения
    std::string messageStr = std::to_string(num);

    return messageStr;
}


/****************************************************************************
 * Отправка сообщения
 *
 * [in] a_message - отправляемое сообщение
 * [in] a_fromAddr
 *
 * @return
 *  Успешность отправки сообщения
 */
bool C_Server::send(const std::string& a_message, const std::string& a_toAddr)
{
    bool okSend= true; // результат отправки данных

    std::vector<char> messageVec = {a_message.begin(), a_message.end() };

    okSend = m_connector->send( messageVec, a_toAddr );

    return okSend;
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

        m_isEndConnSignal = ( s_endConnMessage == std::string(buffer.data() ) );

        // Попытка отправки ответа клиенту на его запрос
        const int s_min = -100;
        const int s_max = 100;
        std::string messageStr = genRandNumMess( s_min, s_max );

        if (!m_isEndConnSignal) {
            sendRes = send(messageStr,fromAddr);
            if (sendRes) {
                // ??????????????
                std::cout << m_connector->name() << ":\tServer Sent message ["
                          << messageStr <<  "] Send size: " << messageStr.length() << " "
                          << std::endl;
            }
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
    discRes = m_connector->close();
    if (discRes) {
        std::cout << m_connector->name()
                  << ":\tServer Socket diconnected successfully. " << std::endl;
    }
    else {
        std::cout << m_connector->name() << ":\tBAD Socket diconnect. " << std::endl;
    }

    // Попытка освобождения ресурсов соединения
    flushRes = m_connector->flush();
    if ( discRes && flushRes ) {
        std::cout << m_connector->name() << ":\tSERVER SOCK CLOSED SUCCESS. " << std::endl;
    }
    else {
        std::cout << m_connector->name() << ":\tBAD Socket closed on Server. " << std::endl;
    }

    return discRes && flushRes;
}

/*****************************************************************************
 * Прием сообщения
 *
 * [out] a_buffer - буфер для записи полученных данных.
 * [out] a_fromAddr - адрес отправителя данных.
 *
 * @return
 *  успешность получения данных
 */
bool C_Server::recv( std::vector<char> &a_buffer, std::string &a_fromAddr)
{
    bool okRecv= true;

    // Попытка получения запроса от клиента
    do {
        if (a_buffer.size() != s_bufferLen ){
            a_buffer.resize(s_bufferLen);
        }
        okRecv = m_connector->recv( a_buffer, a_fromAddr );
    } while( !okRecv && errno == 0 );

    if (okRecv) {
        std::cout << m_connector->name() << ":\tServer Recived message ["
                  << a_buffer.data() << "], size: ["
                  << a_buffer.size() << "], from [" << a_fromAddr << "]" << std::endl;
    } else {
            std::cout << m_connector->name() << ":\tBAD Recieve on Server Side."
                      << std::endl;
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




