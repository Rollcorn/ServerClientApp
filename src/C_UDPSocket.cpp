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
{
}

/*****************************************************************************
 * Деструктор
 */
C_UdpSocket::~C_UdpSocket()
{
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
bool C_UdpSocket::setup( ConnectionParams a_conParam )
{
    // Инициализация параметров соединения
    m_ownIp     = a_conParam.at("ownIp");
    m_ownPort   = (short) std::stoi( a_conParam.at("ownPort") );
    m_remIp     = a_conParam.at("remIp");
    m_remPort   = (short) std::stoi( a_conParam.at("remPort") );
    m_isBlocking =(bool) std::stoi( a_conParam.at("block") );

    bool setupRes = true;
    bool ret = true;

    //Попытка иницициализации библеотеки Winsock2
    ret = initWinsock();

    // Попытка запуска сокета
    m_sockFd = socket( m_ipFamily , m_type, m_protocol );
    if ( m_sockFd == INVALID_SOCKET ) {
        setupRes = false;
        std::cout << "FAILED create socket : " << WSAGetLastError() << std::endl;
    }

    // Заполнение параметров буфера
    ZeroMemory( &m_ownAddr, sizeof( m_ownAddr ) );
    m_ownAddr.sin_family      = m_ipFamily;
    m_ownAddr.sin_port        = htons(m_ownPort);
    m_ownAddr.sin_addr.s_addr = inet_addr( m_ownIp.c_str() );

    // Установка, при необходимости, сокета в неблокирующий режим
    if ( m_isBlocking & optNonblock ) {
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
    u_long flags = 1;
    bool   ret   = true;
    int    res;

    // Попытка установки блокирующего режима
    res = ioctlsocket( m_sockFd, FIONBIO, &flags );
    if (  res != NO_ERROR ) {
        ret = false;
        std::cout << "ioctlsocket failed with error: " << res << std::endl;
    }
    else {
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
    res = bind( m_sockFd, (sockaddr *)&m_ownAddr, sizeof(m_ownAddr) );
    if ( res == SOCKET_ERROR) {
        std::cout << "Bind failed with error code : " << WSAGetLastError() << std::endl;
        ret = false;
    }

    return ret;
}


/*****************************************************************************
 * Отправка данных
 */
bool C_UdpSocket::send( const std::vector<char> &a_data, const std::string &a_to )
{

    bool    sendRes = false;
    socklen_t tolen = sizeof(m_remoteAddr); // Определение размера длины адреса назначения

    int sepIndex = 0;
    while( (sepIndex < a_to.length()) && (a_to.c_str()[sepIndex] != ':') ){
        ++sepIndex;
    }

    std::string remoteIp = a_to.substr(0, sepIndex);
    std::string remotePort = a_to.substr(sepIndex + 1, a_to.length() );
    std::cout <<  "SEND remoteIp [" << remoteIp << "]" << std::endl;
    std::cout <<  "SEND remotePort [" << remotePort << "]" << std::endl;

    // Инициализация параметров соединения
    ZeroMemory( &m_remoteAddr, sizeof( m_remoteAddr ) );
    m_remoteAddr.sin_family      = m_ipFamily;
    m_remoteAddr.sin_port        = htons( (short)std::stoi(remotePort) );
    m_remoteAddr.sin_addr.s_addr = inet_addr( remoteIp.c_str() );

    int sendSize = -1;
    // Попытка отправки данных
    sendSize = sendto( m_sockFd, a_data.data(), a_data.size(), 0,
                          (struct sockaddr *)&m_remoteAddr, tolen );

    if ( sendSize == SOCKET_ERROR ) {
        std::cout << name() <<":\tsendto() failed with error code : "
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
bool C_UdpSocket::recv( std::vector<char> &a_buffer, std::string &a_from )
{
    bool        recvRes = false;
    socklen_t   fromlen = sizeof(m_remoteAddr);
    int recvSize = 0;

    // Очистка струтуры адреса
    // memset( &m_remoteAddr, 0, sizeof(m_remoteAddr) );
    ZeroMemory( &m_remoteAddr, sizeof( m_remoteAddr ) );

    a_buffer.resize(MAXLINE);
    std::fill( a_buffer.begin(), a_buffer.end(), '\0' );

    // Попытка получения запроса
    do{
    recvSize = recvfrom( m_sockFd, a_buffer.data(), a_buffer.size(), 0,
                            (sockaddr *)&m_remoteAddr, &fromlen);
    }  while(recvSize == -1 && WSAGetLastError() == 10035 );

    if ( recvSize == SOCKET_ERROR ) {
        std::cout << name() <<":\trecvfrom() socket failed with error code : "
                  << WSAGetLastError() << std::endl;
        return false;
    }
    else {
        a_buffer.resize(recvSize);
        a_buffer.push_back('\0');
        recvRes = true;
    }

    std::string remoteIp    = inet_ntoa( m_remoteAddr.sin_addr);
    std::string remotePort  = std::to_string( ntohs(m_remoteAddr.sin_port) );
    a_from = remoteIp + ":" + remotePort;

    std::cout << name() << ":\tRecieve from: " << a_from << " message {" << a_buffer.data()
              << "} Recv size=" << a_buffer.size() << std::endl;

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
    return  m_ownIp + ":" + std::to_string(m_ownPort);
}

/***************************************************************************
 * Имя удаленного сокета
 *
 * @return
 *  имя сокета в формате <ip-адрес>:<порт>
 */
std::string C_UdpSocket::remoteAddr()
{
    return  m_remIp + ":" + std::to_string(m_remPort);
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






