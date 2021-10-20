/*****************************************************************************

  C_Socket

  Предоставляет реализацию сокета протокола UDP.

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
C_UDPSocket::C_UDPSocket() : m_ownAddr( new struct sockaddr_in),
    m_remoteAddr(new struct sockaddr_in ){
    initWinsock();
}

/*****************************************************************************
 * Деструктор
 */
C_UDPSocket::~C_UDPSocket(){
    delete m_ownAddr;
    delete m_remoteAddr;
}

/*****************************************************************************
 * Инициализация библиотеки WinSock2
 *
 * Для использования функций библиотеки WInsock, использующих DLL библиотеки,
 * выполняется инициация использования WS2_32.dll.
 *
 */
bool C_UDPSocket::initWinsock(){
    bool initRes = false;
    int initState = false;
    WSADATA wsadata;

    // Попытка инициализации библиотек сокета
    initState = WSAStartup( MAKEWORD(2,2), &wsadata );
    if ( initState != 0){
        std::cout << m_sockName << ": Failed. Error Code : " << WSAGetLastError() << '\n';
    }
    else {
        std::cout << m_sockName << ": Successfull init winsock : " << WSAGetLastError() << '\n';

        initRes = true;
    }

    m_wsadata = wsadata;

    return initRes;

}

/*****************************************************************************
 * Создание/запуск сокета
 *
 * Метод устанавливает сокету ip-адрес и порт, флаговая переменная
 * переводит сокет в режим неблокирующего ввода/вывода.
 *
 * @param
 *  [in] conParam - пара значений first - IP-адрес, second - порт
 *       a_optFlag  - флаговая переменная, при 1 метод вызовет функцию setNonblock.
 *
 */
bool C_UDPSocket::setup( std::pair<std::string, short> a_conParam,
                         int a_optFlag = optNonblock ){
    std::cout << m_sockName << "Start setup" << '\n';

    // Инициализация ip-адреса и порта сокета
    m_servIpAddr = a_conParam.first;
    m_servPort   = a_conParam.second;

    bool setupRes = false;

    // Попытка запуска сокета на переданных ip-адресе и порту
    m_sockFd = socket( m_ipFamily , m_type, m_protocol );
    if ( m_sockFd == SOCKET_ERROR ) {
        std::cout << m_sockName << ": Could not create socket : "
                  << WSAGetLastError() << '\n';
    }
    else { setupRes = true; }

    // Обнуление буфера
    ZeroMemory( m_ownAddr, sizeof( *m_ownAddr ) );

    m_ownAddr->sin_family      = m_ipFamily;
    m_ownAddr->sin_port        = htons(m_servPort);
    m_ownAddr->sin_addr.s_addr = inet_addr(m_servIpAddr.c_str());

    // Установка, при необходимости, сокета в неблокирующий режим
    if( a_optFlag & optNonblock ){ setNonblock(); }

    return setupRes;
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
bool C_UDPSocket::open(){
    bool openRes = false;
    int tryBind;

    // Попытка открытия сокета
    tryBind = bind( m_sockFd, (struct sockaddr *)m_ownAddr, sizeof(*m_ownAddr) );
    if( tryBind == SOCKET_ERROR ) {
        std::cout << m_sockName << ": Bind failed with error code : "
                  << WSAGetLastError() <<'\n';
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
bool C_UDPSocket::setNonblock(){
    u_long flags = 0;
    bool setupRes = false;
    int blockingState;

    // Попытка установки блокирующего режима
    blockingState = ( ioctlsocket( m_sockFd, FIONBIO, &flags ) );
    if (  blockingState != NO_ERROR ){
        std::cout << m_sockName << ": ioctlsocket failed with error: "
                  << blockingState << '\n';
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
bool C_UDPSocket::recv( char *a_data, int a_dataLen, int *a_recvSize ){

    bool recvRes = false;
    int srcAddrLen = sizeof(*m_remoteAddr);

    //Попытка получения запроса
    *a_recvSize = recvfrom( m_sockFd, a_data, a_dataLen, 0,
                            (struct sockaddr *)m_remoteAddr, &srcAddrLen );
    if ( *a_recvSize == SOCKET_ERROR) {
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
bool C_UDPSocket::send( char *a_data, int a_dataLen, int *a_sendSize ){
    bool sendRes = false;
    int slen = sizeof(*m_remoteAddr); // Определение размера длины адреса назначения

    // Попытка отправки данных
    *a_sendSize = sendto( m_sockFd,  a_data, a_dataLen, 0,
                          ( struct sockaddr *)m_remoteAddr, slen );
    if ( *a_sendSize == SOCKET_ERROR ) {
         std::cout << m_sockName << ": sendto() failed with error code : "
                   << WSAGetLastError() << '\n';
    }
    else { sendRes = true; }

    return sendRes;

}


/******************************************************************************
 * Закрытие соединения
 *
 * Выполняет отключение созданного сокета
 *
 */
bool C_UDPSocket::close() {
    closesocket( m_sockFd );
    return true;

}

/*****************************************************************************
 * Освобождение ресурсов
 *
 */
bool C_UDPSocket::flush() {
    WSACleanup();
    return true;
}

/***************************************************************************
 * Имя сокета
 *
 */
std::__cxx11::string C_UDPSocket::name()
{
    return m_servIpAddr;
}

/*****************************************************************************
 * Получение собственной структуры адреса
 *
 * @return
 *   m_myAddr - собственноая структура адреса сокета.
 *
 */
struct sockaddr_in * C_UDPSocket::ownAddr(){
    return m_ownAddr;
}

/*****************************************************************************
 * Получение структуры адреса клиента
 *
 * @return
 *   m_otherAddr - структура адреса сокета клиента.
 *
 */
struct sockaddr_in * C_UDPSocket::remoteAddr(){
    return m_remoteAddr;
}

/* Получение ip-адреса сокета
 *
 * @return
 *   m_servIpAddr - ip-адрес сокета сервера.
 *
 */
std::string C_UDPSocket::ipAddress(){
    return m_servIpAddr;
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






