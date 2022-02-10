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
    bool initRes = false;
    int initState = false;
    WSADATA wsadata;

    // Попытка инициализации библиотек сокета
    initState = WSAStartup( MAKEWORD(2,2), &wsadata );
    if ( initState != 0 ) {
        std::cout << "Failed. Error Code : " << WSAGetLastError() << std::endl;
    }
    else {
        std::cout << "Successfull init winsock : " << WSAGetLastError() << std::endl;
        initRes = true;
    }

    m_wsadata = wsadata;
    return initRes;

}

/*****************************************************************************
 * Создание сокета
 */
bool C_UdpSocket::setup( std::string a_ipParam, std::string a_portParam,
                         int a_optFlag = optNonblock )
{
    // Инициализация ip-адреса и порта сокета
    m_ownIpAddr = a_ipParam;
    m_ownPort   = a_portParam;

    bool setupRes = false;
    bool ret = true;

    //Попытка иницициализации библеотеки Winsock2
    ret = initWinsock();


    // Попытка запуска сокета на переданных ip-адресе и порту
    m_sockFd = socket( m_ipFamily , m_type, m_protocol );
    if ( m_sockFd == SOCKET_ERROR ) {
        std::cout << "FAILED create socket : " << WSAGetLastError() << std::endl;
    }
    else {
        setupRes = true;
    }

    // Заполнение параметров буфера
    ZeroMemory( a_sockAddr, sizeof( *a_sockAddr ) );
    m_ownAddr->sin_family      = m_ipFamily;
    m_ownAddr->sin_port        = htons(m_ownPort);
    m_ownAddr->sin_addr.s_addr = inet_addr(m_ownIpAddr.c_str());

    // Установка, при необходимости, сокета в неблокирующий режим
    if ( a_optFlag & optNonblock ) {
        setNonblock();
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
    u_long flags = 0;
    bool   ret   = true;
    int    res;

    // Попытка установки блокирующего режима
    res = ioctlsocket( m_sockFd, FIONBIO, &flags );
    if (  res != NO_ERROR ) {
        std::cout << "ioctlsocket failed with error: " << res << std::endl;
        ret = false;
    }

    return ret;
}

/*****************************************************************************
 * Cвязывание сокет с локальным адресом протокола
 */
bool C_UdpSocket::open()
{
    bool ret = true;
    int  res;

    // Попытка открытия сокета
    res = bind( m_sockFd, (struct sockaddr *)m_ownAddr, sizeof(*m_ownAddr) );
    if ( res == SOCKET_ERROR ) {
        std::cout << "Bind failed with error code : " << WSAGetLastError() << std::endl;
        ret = false;
    }

    return ret;
}

/*****************************************************************************
 * Получение данных из сокета
 */
bool C_UdpSocket::recv( char *a_data, int a_dataLen, int *a_recvSize )
{
    bool recvRes = false;
    int  slen    = sizeof(*m_remoteAddr);

    //Попытка получения запроса
    *a_recvSize = recvfrom( m_sockFd, a_data, a_dataLen, 0,
                            (struct sockaddr *)m_remoteAddr, &slen );
    if ( *a_recvSize == SOCKET_ERROR) {
        std::cout << ": recvfrom() socket failed with error code : "
                  << WSAGetLastError() << '\n';
    }
    else {
        recvRes = true;
    }

    return recvRes;

}

/*****************************************************************************
 * Отправка данных
 */
bool C_UdpSocket::send( char *a_data, int a_dataLen, int *a_sendSize)
{
    bool sendRes = false;
    int slen = sizeof(*m_remoteAddr); // Определение размера длины адреса назначения

    // Попытка отправки данных
    *a_sendSize = sendto( m_sockFd,  a_data, a_dataLen, 0,
                          (struct sockaddr *)m_remoteAddr, slen );
    if ( *a_sendSize == SOCKET_ERROR ) {
        std::cout << "sendto() failed with error code : "
                  << WSAGetLastError() << '\n';
    }
    else {
        sendRes = true;
    }

    return sendRes;

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






