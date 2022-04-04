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
    m_ownIp      = a_conParam.at("ownIp");
    m_ownPort    = (short) std::stoi( a_conParam.at("ownPort") );
    m_remIp      = a_conParam.at("remIp");
    m_remPort    = (short) std::stoi( a_conParam.at("remPort") );
    m_isBlocking = (bool) std::stoi( a_conParam.at("block") );

    bool okSetup = false;
    bool okSetOpt = false;
    bool okRet = false;

    // Попытка иницициализации библеотеки Winsock2
    okRet = initWinsock();

    // Попытка запуска сокета
    if(okRet){
        m_sockFd = socket( m_ipFamily , m_type, m_protocol );
        if ( m_sockFd != INVALID_SOCKET ) {
            okSetup = true;
        } else {
            errno = ERR_BADSETUP;
        }
    } else {
        errno = ERR_BADINIT;
    }

    // Заполнение параметров буфера
    ZeroMemory( &m_ownAddr, sizeof( m_ownAddr ) );
    m_ownAddr.sin_family      = m_ipFamily;
    m_ownAddr.sin_port        = htons(m_ownPort);
    m_ownAddr.sin_addr.s_addr = inet_addr( m_ownIp.c_str() );

    // Установка, при необходимости, сокета в неблокирующий режим
    if ( okSetup && (m_isBlocking & optNonblock) ) {
        okSetOpt = setNonblock();
        if ( !okSetOpt ){
            errno = ERR_BADSETOPT;
        }
    }

    return okRet & okSetup & okSetOpt;
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
    bool   okSetOpt   = true;

    // Попытка установки блокирующего режима
    int setOptRes = ioctlsocket( m_sockFd, FIONBIO, &flags );
    if (  setOptRes != NO_ERROR ) {
        errno = ERR_BADSETOPT;
        okSetOpt = false;
    }

    return okSetOpt;
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
        errno = ERR_BADBIND;
        ret = false;
    }

    return ret;
}


/*****************************************************************************
 * Отправка данных
 */
bool C_UdpSocket::send( const std::vector<char> &a_data, const std::string &a_to )
{
    bool    okSend = false;                 // Результат отправки
    socklen_t tolen = sizeof(m_remoteAddr); //  Размера адреса назначения

    // Поиск разделителя ip и порта в строке a_to
    u_int sepIndex = 0;
    while( (sepIndex < a_to.length()) && (a_to.c_str()[sepIndex] != ':') ){
        ++sepIndex;
    }
    std::string remoteIp = a_to.substr(0, sepIndex);
    std::string remotePort = a_to.substr(sepIndex + 1, a_to.length() );

    // Инициализация параметров соединения
    ZeroMemory( &m_remoteAddr, sizeof( m_remoteAddr ) );
    m_remoteAddr.sin_family      = m_ipFamily;
    m_remoteAddr.sin_port        = htons( (short)std::stoi(remotePort) );
    m_remoteAddr.sin_addr.s_addr = inet_addr( remoteIp.c_str() );

    // Попытка отправки данных
    int sendSize = sendto( m_sockFd, a_data.data(), a_data.size(), 0,
                          (struct sockaddr *)&m_remoteAddr, tolen );

    if ( sendSize == SOCKET_ERROR ) {
        errno = ERR_BADSEND;
    }
    else {
        okSend = true;
    }

    return okSend;
}

/*****************************************************************************
 * Получение данных из сокета
 */
bool C_UdpSocket::recv( std::vector<char> &a_buffer, std::string &a_from )
{
    bool        okRecv  = false;
    socklen_t   fromlen = sizeof(m_remoteAddr);
    int recvSize = 0;

    // Очистка струтуры адреса
    ZeroMemory( &m_remoteAddr, sizeof( m_remoteAddr ) );

    a_buffer.resize(MAXLINE);
    std::fill( a_buffer.begin(), a_buffer.end(), '\0' );

    // Попытка получения запроса
    recvSize = recvfrom( m_sockFd, a_buffer.data(), a_buffer.size(), 0,
                            (sockaddr *)&m_remoteAddr, &fromlen);

    if ( recvSize == SOCKET_ERROR ) {
        if (a_buffer.size() == 0 ){
            errno = ERR_BUFEMPT;
        }
    }
    else {
        a_buffer.resize(recvSize);
        okRecv = true;
    }

    // Формирование строки адреса отправителя
    if (okRecv) {
        std::string remoteIp    = inet_ntoa( m_remoteAddr.sin_addr);
        std::string remotePort  = std::to_string( ntohs(m_remoteAddr.sin_port) );
        a_from = remoteIp + ":" + remotePort;
    }

    return okRecv;
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






