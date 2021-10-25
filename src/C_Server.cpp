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
 *   [in]    a_conParam - пара значений first - ip-адрес на котором необходимо
 *                        открыть сокет сервера, second - порт соединения.
 *           a_optFlag  - флаговая переменная опций сокета .
 *
 * @return
 *
 */
bool C_Server::setup( std::pair<std::string, short> a_conParam, int a_optFlag )
{
    bool setupRes = false; // Результат запуска сокета
    bool openRes  = false; // Результат открытия соединения сокета

    // Инстанцируем необходимый  сокет
    m_socket = CreateSocket("UDP");

    std::cout << " Server setup " << '\n';
    // Инициализация сокета клиента
    setupRes = m_socket->setup( a_conParam, m_socket->ownSockAdr(), a_optFlag );
    if (setupRes) {
        std::cout << m_socket->name() << ": Server Socket creating SUCCESS " << std::endl;
    }
    else {
        std::cout << m_socket->name() << ": Socket crating has FAILED." << std::endl;
    }

    // Попытка открытия сокета клиента
    openRes = m_socket->open();
    if (openRes) {
        std::cout << m_socket->name() << ": Server Socket opening SUCCESS " << std::endl;
    }
    else {
        std::cout << m_socket->name() << ": Server Socket opening has FAILED." << std::endl;
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

    std::cout << m_socket->name() << ": Server communication started.\n";

    // Попытка запуска комуникации с сервером
    workRes = communication( m_servBufferSize );
    if( workRes ) {
        std::cout << m_socket->name() << ": Server Start communacation SUCCESS. " << std::endl;
    } else {
        std::cout << m_socket->name() << ":  Server communacation FAILED. " << std::endl;
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
bool C_Server::communication( int a_buffSize )
{

    bool recvRes = true;
    bool sendRes = true;

    int recvSize = 0;
    int sendSize = 0;

    char buffer[a_buffSize]; // Буфер для полученных данных

    std::random_device  rd;
    std::mt19937        gen( rd() );
    int a = -100, b = 100;
    std::uniform_int_distribution<int> dist( a, b );


    std::cout << m_socket->name() << ": Server Start communication... " << std::endl;
    std::cout << std::endl;

    while ( recvRes && sendRes ) {
        recvRes = false;
        sendRes = false;

        // Очищение буфера
        ZeroMemory( &buffer, a_buffSize );

        std::cout << m_socket->name() << ": Server Waiting for data... " << std::endl;
        fflush(stdout);

        // Генерируется случайное число
        int num = dist(gen);

        // Попытка получения запроса от клиента
        recvRes = m_socket->recv( buffer, a_buffSize, &recvSize );
        if (recvRes){
            std::cout << m_socket->name() << ": Server Recived message size: " << recvSize << " " << std::endl;
        }
        else {
            std::cout << m_socket->name() << ": BAD Reciev on Server " << std::endl;
        }

        // Попытка отправки ответа клиенту на его запрос
        if (recvRes) { sendRes = m_socket->send( (char *)&num, sizeof(num), &sendSize ); }
        if (sendRes) {
            std::cout << m_socket->name() << ": Server Sent message size: " << sendSize << " " << std::endl;
        }
        else {
            std::cout << m_socket->name() << ": BAD Send on Server " << std::endl;
        }
    }

    return true;
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
        std::cout << m_socket->name() << ": Server Socket diconnected successfully. " << std::endl;
    }
    else {
        std::cout << m_socket->name() << ": BAD Socket diconnect. " << std::endl;
    }

    // Попытка освобождения ресурсов соединения
    flushRes = m_socket->flush();
    if ( discRes && flushRes ) {
        std::cout << m_socket->name() << ": Server Socket closed successfully. " << std::endl;
    }
    else {
        std::cout << m_socket->name() << ": BAD Socket closed on Server. " << std::endl;
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




