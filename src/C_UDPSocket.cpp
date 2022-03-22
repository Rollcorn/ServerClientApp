/*****************************************************************************

  C_Socket

  Реализация сокета UDP протокола.

*****************************************************************************/

#include "C_UDPSocket.h"

namespace myTask{

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
C_UdpSocket::C_UdpSocket()
: m_ownAddr   ( new struct sockaddr_in ),
  m_remoteAddr( new struct sockaddr_in )
{
}

/*****************************************************************************
 * Деструктор
 */
C_UdpSocket::~C_UdpSocket()
{
    delete m_ownAddr;
    delete m_remoteAddr;
}

/*****************************************************************************
 * Инициализация библиотеки WinSock2
 *
 * Для использования функций библиотеки WInsock, использующих DLL библиотеки,
 * выполняется инициация использования WS2_32.dll.
 *
 * @return
 *  корректность инициализации библиотек Winsock2
 */
bool C_UdpSocket::initWinsock()
{
    bool initRes = true;
    int initState = false;
    WSADATA wsadata;

    // Попытка инициализации библиотек сокета
    initState = WSAStartup( MAKEWORD(2,2), &wsadata );
    if ( initState != 0 ) {
        std::cout << "Failed. Error Code : " << WSAGetLastError() << std::endl;
        initRes = false;
    }

    m_wsadata = wsadata;
    return initRes;
}

/*****************************************************************************
 * Создание сокета
 */
bool C_UdpSocket::setup( std::string a_ipParam, std::string a_portParam,
                         int a_optFlag )
{
    // Инициализация ip-адреса и порта сокета
    m_ownIpAddr = a_ipParam;
    m_ownPort   = (short) std::stoi(a_portParam);

    bool setupRes = true;
    bool ret = true;

    //Попытка иницициализации библеотеки Winsock2
    ret = initWinsock();

    // Попытка запуска сокета на переданных ip-адресе и порту
    m_sockFd = socket( m_ipFamily , m_type, m_protocol );
    if ( m_sockFd == INVALID_SOCKET ) {
        setupRes = false;
        std::cout << "FAILED create socket : " << WSAGetLastError() << std::endl;
    }

    // Заполнение параметров буфера
    ZeroMemory( m_ownAddr, sizeof( *m_ownAddr ) );
    m_ownAddr->sin_family      = m_ipFamily;
    m_ownAddr->sin_port        = htons(m_ownPort);
    m_remoteAddr->sin_addr.s_addr = inet_addr( a_ipParam.c_str() );

    // Установка, при необходимости, сокета в неблокирующий режим
    if ( a_optFlag & optNonblock ) {
        this->setNonblock();
    }

    return ret & setupRes;
}

/*****************************************************************************
 * Установка сокета в неблокирующий режим
 *
 * Устанавливает режим работы сокета на неблокирующий.
 *
 * @return
 *  Успешность установки опций сокета
 */
bool C_UdpSocket::setNonblock()
{
    u_long flags = 1;
    bool   ret   = true;
    int    res;

    // Попытка установки блокирующего режима
    res = ioctlsocket( m_sockFd, FIONBIO, &flags );
    if (  res != NO_ERROR ) {
        ret = false;
        std::cout << "ioctlsocket failed with error: " << res << std::endl;
    } else {
        std::cout << name() << ": NONBLOCKING success" << std::endl;

    }

    return ret;
}

/*****************************************************************************
 * Cвязывание сокет с локальным адресом протокола
 */
bool C_UdpSocket::open()
{
    bool ret = true;
    int  res = 0;

    // Попытка открытия сокета
    res = bind( m_sockFd, (sockaddr *)m_ownAddr, sizeof(*m_ownAddr) );
    if ( res == SOCKET_ERROR) {
        std::cout << "Bind failed with error code : " << WSAGetLastError() << std::endl;
        ret = false;
    }

    return ret;
}


/*****************************************************************************
 * Отправка данных
 */
bool C_UdpSocket::send( std::string a_remoteIp, std::string a_remotePort,
                        std::vector<char> a_data, int &a_sendSize )
{
    int     tolen    = sizeof(*m_remoteAddr); // Определение размера длины адреса назначения
    bool    sendRes = false;

    // Инициализация параметров соединения
    ZeroMemory( m_remoteAddr, sizeof( *m_remoteAddr ) );
    m_remoteAddr->sin_family      = m_ipFamily;
    m_remoteAddr->sin_port        = htons( (u_short)std::stoi(a_remotePort) );
    m_remoteAddr->sin_addr.s_addr = inet_addr( a_remoteIp.c_str() );
//inet_addr( a_remoteIp.c_str() )


    // Попытка отправки данных
    *a_sendSize = sendto( m_sockFd, a_data[0], strlen(a_data.c_str()), 0,
                          (struct sockaddr *)m_remoteAddr, tolen );
    if ( *a_sendSize == SOCKET_ERROR ) {
        std::cout << "sendto() failed with error code : "
                  << WSAGetLastError() << std::endl;
    }
    else {
        sendRes = true;
    }

    return sendRes;

}

/*****************************************************************************
 * Получение данных из сокета
 */
bool C_UdpSocket::recv( std::string a_remoteIp, std::string a_remotePort,
                        std::vector<char> &a_buff, int &a_recvSize )
{
    bool    recvRes = false;
    int     fromlen = sizeof(*m_remoteAddr);
    int n;

    // Инициализация параметров соединения
    std::fill( a_buff.begin(), a_buff.end(), '\0' );

    // Очистка струтуры адреса
    memset( m_remoteAddr, 0, sizeof(*m_remoteAddr) );

//    m_remoteAddr->sin_family      = m_ipFamily;
//    m_remoteAddr->sin_port        = htons( (u_short)std::stoi(a_remotePort) );
//    m_remoteAddr->sin_addr.s_addr = inet_addr( a_remoteIp.c_str() );

    // Попытка получения запроса
    n = recvfrom( m_sockFd, &a_buff[0], a_buff.size(), 0,
                            (sockaddr *)m_remoteAddr, &fromlen);
//    a_buff[n] = '\0';

    std::cout << this->name() << ": Get from "
              << ":" << m_remoteAddr->sin_port << " MESS {" << a_buff[0] << "} Recv size="
              << n << " fromlen=" << sizeof(a_buff) << std::endl;

    // Проверка коректности адреса клиента
    if ( inet_addr( a_remoteIp.c_str()) != m_remoteAddr->sin_addr.s_addr ){
        std::cout << "WRONG_IP: " << inet_addr( a_remoteIp.c_str()) <<
                  ": " << m_remoteAddr->sin_addr.s_addr << std::endl;
    }

    if ( htons( (u_short)std::stoi(a_remotePort) ) != m_remoteAddr->sin_port ){
        std::cout << "WRONG_PORT" << std::endl;
    }

    if ( n == SOCKET_ERROR ) {
        std::cout << " : recvfrom() socket failed with error code : "
                  << WSAGetLastError() << std::endl;
    }
    else {
        recvRes = true;
        a_recvSize = n;
    }

    return recvRes;
}

/******************************************************************************
 * Закрытие соединения
 */
bool C_UdpSocket::close()
{
    closesocket(m_sockFd);
    return true;
}

/*****************************************************************************
 * Освобождение ресурсов
 */
bool C_UdpSocket::flush()
{
    WSACleanup();
    return true;
}

/***************************************************************************
 * Имя сокета
 *
 * @return
 *  имя сокета в формате <ip-адрес>:<порт>
 */
std::string C_UdpSocket::name()
{
    return  m_ownIpAddr + ":" + std::to_string(m_ownPort);
}

/*****************************************************************************
 * Cтруктура адреса собственного сокета
 *
 * @return
 *   структура собственного адреса сокета.
 */
sockaddr_in * C_UdpSocket::ownSockAdr()
{
    return  m_ownAddr;
}

/*****************************************************************************
 * Получение адреса удаленного сокета
 *
 * @return
 *  структура удаленного адреса сокета.
 */
sockaddr_in * C_UdpSocket::remoteSockAdr()
{
    return m_remoteAddr;
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






