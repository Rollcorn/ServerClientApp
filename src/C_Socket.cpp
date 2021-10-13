/*****************************************************************************

  C_Socket

  Предоставляет методы для создания сокета и работы с ними.

*****************************************************************************/

/*****************************************************************************
  Macro Definitions
*****************************************************************************/

#include "C_Socket.h"

namespace mySocket{
/*****************************************************************************
  Forward Declarations
*****************************************************************************/

/*****************************************************************************
  Types and Classes Definitions
*****************************************************************************/

/*****************************************************************************
 * Инициализация библиотеки WinSock2
 *
 * Для использования функций библиотеки WInsock, использующих DLL библиотеки,
 * выполняется инициация использования WS2_32.dll.
 *
 */
bool C_Socket::initWinsock()
{
    bool initRes = false;
    int initState;
    WSADATA wsadata;

    std::cout << m_sockName << ": Initialising Winsock...\n";

    if ( ( initState = WSAStartup( MAKEWORD(2,2), &wsadata ) ) != 0 ){
        std::cout << m_sockName << ": Failed. Error Code : " << WSAGetLastError() << '\n';
    }
    else { initRes = true;}

    m_wsadata = wsadata;

    std::cout << m_sockName << ": Initialised.\n";

    return initRes;

}

/*****************************************************************************
 * Создание/запуск сокета
 *
 * @param
 *  [in] a_type - тип сокета.
 *       a_protocol - аргументустанавливаемый в соответствии с используемым протоколом.
 *       a_ipFamily - семество протоколов сокетов.
 *
 */
bool C_Socket::setupSock( int a_type, int a_protocol, int a_ipFamily )
{
    m_ipFamily = a_ipFamily;
    m_type     = a_type;
    m_protocol = a_protocol;

    bool setupRes = false;

    if ( ( m_sockFd = socket( m_ipFamily , m_type, m_protocol ) ) == SOCKET_ERROR ){
        std::cout << m_sockName << ": Could not create socket : "
                  << WSAGetLastError() << '\n';
    }
    else { setupRes = true; }

    return setupRes;
}

/*****************************************************************************
 * Настройка соединения сокета
 *
 * Метод устанавливает сокету ip-адрес и порт, флаговая переменная
 * переводит сокет в режим неблокирующего ввода/вывода.
 *
 * @param
 *  [in] a_ipAddr   - ip-адрес
 *       a_port     - порт
 *       a_optFlag  - флаговая переменная, при 1 метод вызовет функцию setNonblock.
 *
 */
bool C_Socket::socketSettings( std::string a_ipAddr, int a_port, int a_optFlag )
{
    m_servIpAddr = a_ipAddr;
    m_servPort   = a_port;

    ZeroMemory( m_myAddr, sizeof(*m_myAddr) );

    m_myAddr->sin_family      = m_ipFamily;
    m_myAddr->sin_port        = htons(m_servPort);
    m_myAddr->sin_addr.s_addr = inet_addr(m_servIpAddr.c_str());

    if(a_optFlag & optNonblock) setNonblock();

    return true;

}

/*****************************************************************************
 * Cвязывание сокет с локальным адресом протокола
 *
 * Метод выполняется только для сокета сервера для привязки его к определенному локальному адресу
 * метод производит логирование результата выполнения привязки.
 *
 * @param
 *  [in] a_myAddr - структура собственного адреса сокета.
 *
 */
bool C_Socket::openSock()
{
    bool openRes = false;

    if( bind( m_sockFd, (struct sockaddr *)m_myAddr, sizeof(*m_myAddr) ) == SOCKET_ERROR) {
        std::cout << m_sockName << ": Bind failed with error code : " << WSAGetLastError()
                  <<'\n';
    }
    else { openRes = true; }

    return openRes;
}

/*****************************************************************************
 * Установка сокета в неблокирующий режим
 *
 * Изменяет флаг режим работы сокета на неблокирующий.
 *
 */
bool C_Socket::setNonblock(){
    u_long flags = 0;
    bool setupRes = false;
    int blockingState;

    if ( blockingState = ( ioctlsocket( m_sockFd, FIONBIO, &flags ) ) != NO_ERROR ){
        std::cout << m_sockName << ": ioctlsocket failed with error: " << blockingState
                  << '\n';
    }
    else { setupRes = true; }

    return setupRes;
}

/*****************************************************************************
 * Получение данных из сокета
 *
 * Производит получение данных из сокета. Сокет источника определяется структурой адреса
 * при получении данных от сервера используется - m_myAddr, при получении данных от
 * клиента передается  - m_otherAddr.
 * Полученные данные записываются в переданный буфер a_dataStore.
 *
 * @param
 *  [in] a_srcAddr  - структура адреса сокета источника данных.
 *       a_data     - буфер в который необходимо записать данные полученные из бефера.
 *       a_dataLen  - размер переданного буфера.
 *       a_recvSize - переменная для хранения размера полученных данных.
 *
 */
bool C_Socket::reciveData( struct sockaddr_in *a_srcAddr, char *a_data,
                           int a_dataLen, int *a_recvSize )
{

    bool recvRes = false;
    int srcAddrLen = sizeof(*a_srcAddr);

    //Получение запроса
    if ( ( *a_recvSize = recvfrom( m_sockFd, a_data, a_dataLen, 0,
                                  (struct sockaddr *)a_srcAddr, &srcAddrLen ) ) == SOCKET_ERROR) {
        std::cout << ": recvfrom() server failed with error code : "
                  << WSAGetLastError() << '\n';
    }
    else { recvRes = true; }

    return recvRes;

}

/*****************************************************************************
 * Отправка данных
 *
 * Выполняет отправку данных в сокет с логированием результата. Сокет назначения
 * определяется структурой адреса a_distAddr. Данны отправляются в сокет из переданного
 * буфера a_dataStore.
 *
 * @param
 * [in] a_distAddr       - структура адреса сокета назначения данных.
 *      a_data     - буфер из которого данные отправляются в сокет.
 *      a_dataLen - размер передаваемых данных.
 *      a_sendSize
 *
 * @return
 *  NO_ERROR (0) - при успешной установке неблокирующего режима;
 *  иначе - ошибка.
 *
 */
bool C_Socket::sendData( struct sockaddr_in *a_distAddr, char *a_data,
                         int a_dataLen, int *a_sendSize )
{
    bool sendRes = false;
    int slen = sizeof(*a_distAddr); // Определение размера длины адреса назначения

    if ( ( *a_sendSize = sendto( m_sockFd,  a_data, a_dataLen, 0,
                                (struct sockaddr *)a_distAddr, slen) ) == SOCKET_ERROR ) {
         std::cout << m_sockName << ": sendto() failed with error code : " << WSAGetLastError() << '\n';
    }
    else { sendRes = true; }

    return sendRes;

}

/*****************************************************************************
 * Закрытие сокета
 *
 * Выполняет отключение созданного сокета closesocket(m_sockFd) и очистку WSACleanup.
 *
 */
bool C_Socket::flushSock()
{
    closesocket( m_sockFd );
    WSACleanup();

    return true;
}

/*****************************************************************************
 * Задание имени сокета
 *
 * Назначает сокету имя a_name для использования в логах.
 *
 * @param
 *
 */
bool C_Socket::setName(std::__cxx11::string a_name)
{
    m_sockName = a_name;
    return true;
}


/*****************************************************************************
 * Получение имени сокета
 *
 * @return
 *   m_sockName - имя назначенное сокету при создании экземпляра класса.
 *
 */
std::string C_Socket::name()
{
    return m_sockName;
}

/*****************************************************************************
 * Получение структуры адреса источника(сервера)
 *
 *
 * @return
 *   m_myAddr - структура собственного адреса сокета.
 *
 */
struct sockaddr_in *C_Socket::servAddr()
{
    return m_myAddr;
}

/*****************************************************************************
 * Получение структуры адреса клиента
 *
 * @return
 *   m_otherAddr - структура адреса сокета клиента.
 *
 */
struct sockaddr_in *C_Socket::clientAddr()
{
    return m_otherAddr;
}

/* Получение ip-адреса сокета
 *
 * @return
 *   m_servIpAddr - ip-адрес сокета сервера.
 *
 */
std::string C_Socket::ipAddress()
{
    return m_servIpAddr;
}

/*****************************************************************************
 * Конструктор
 */
C_Socket::C_Socket() :
    m_myAddr( new struct sockaddr_in), m_otherAddr(new struct sockaddr_in )
{
    initWinsock();
}

/*****************************************************************************
 * Деструктор
 */
C_Socket::~C_Socket()
{
    delete m_myAddr;
    delete m_otherAddr;
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
}//namespace mySocket






