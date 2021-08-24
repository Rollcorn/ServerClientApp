/*****************************************************************************

  CLASS C_Socket

  Класс для создания и работы с сокетами средствами библиотеки WinSock2

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
* Для использования функций библиотеки WInsock использующих DLL библиотеки
* выполняется инициация использования WS2_32.dll.
*
*/
void C_Socket::initWinsock()
{
    int initState;
    WSADATA wsadata;
    std::cout << m_sockName << " Initialising Winsock...\n";
    if ( ( initState = WSAStartup( MAKEWORD(2,2), &wsadata ) ) != 0 )
    {
        std::cout << m_sockName << " Client Failed. Error Code : " << WSAGetLastError() << '\n';
        exit(EXIT_FAILURE);
    }
    m_wsadata = wsadata;
    std::cout << m_sockName << " Initialised.\n";


}

/*****************************************************************************
* Создание/запуск сокета
*
* Метод принимает структура адреса в которой будут инициализирова структура собственного
* адреса сокета. Создание происходит с проверкой успешности и логированием в случае ошибки.
*
* @param
*  [in] a_myAddr - структура адреса сокета с параметрами соединения: sin_family, sin_port,
*                   sin_addr.s_addr;
*       addr     - IP адрес в вормате беззнакового целого числа. Для клиента он форматируется
*                   функцией inet_addr() из си-строки ip-адреса, для сервера он задается как
*                   INADDR_ANY;
*       protocol - протокол соединения для сервера он задан как 0, для клиента IPPROTO_UDP.

*/
void C_Socket::createSock(struct sockaddr_in *a_myAddr, unsigned long addr, int protocol)
{

    if ( ( m_sockFd = socket( m_ipFamily , m_type, protocol ) ) == SOCKET_ERROR )
    {
        std::cout << m_sockName << "Could not create socket : "
                  << WSAGetLastError() << '\n';
    }

    ZeroMemory(a_myAddr, sizeof(*a_myAddr));

    a_myAddr->sin_family      = m_ipFamily;
    a_myAddr->sin_port        = htons(m_servPort);
    a_myAddr->sin_addr.s_addr = addr;

    std::cout << m_sockName << " Socket created.\n";

    m_myAddr = a_myAddr;
}


/*****************************************************************************
* Установка сокета в неблокирующий режим
*
* Изменяет флаг параметров сокета в неблокирующий режим. Так как работа происходит
* с кокретным экземляром (сокетом) то метод знает к какому дескриптору обращаться -
* m_sockFd.
*
*/
void C_Socket::setNonblock(){
    u_long flags = 0;

    int blockingState = ioctlsocket( m_sockFd, FIONBIO, &flags );

    if (blockingState != NO_ERROR)
        std::cout << m_sockName << " ioctlsocket failed with error: " << blockingState << '\n';
}

/*****************************************************************************
* Cвязывание сокет с локальным адресом протокола
*
* Метод выполняется только для сокета сервера для привязки его к определенному локальному адресу
* метод производит логирование результата выполнения привязки.
*
* @param
*  [in] a_myAddr - структура собственного адреса сокета.
*/
void C_Socket::makeBind(struct sockaddr_in *a_myAddr)
{
    if( bind( m_sockFd, (struct sockaddr *)a_myAddr, sizeof(*a_myAddr) ) == SOCKET_ERROR)
    {
        std::cout << m_sockName << " Bind failed with error code : " << WSAGetLastError() <<'\n';
        exit(EXIT_FAILURE);
    }
    std::cout << m_sockName << " Bind done\n";
}

/*****************************************************************************
* Получение данныйх
*
* Производит получение данных из сокета с логированием результата. Сокет источника
* определяется структурой a_srcAddr. Полученные данные записываются в переданный буфер
* a_dataStore.
*
* @param
*  [in] a_srcAddr       - структура адреса сокета источника данных.
*       a_dataStore     - буфер в который необходимо записать данные полученные из бефера.
*       a_dataStoreSize - размер переданного буфера.
*/
void C_Socket::reciveData(struct sockaddr_in *a_srcAddr, char *a_dataStore, int a_dataStoreSize)
{
    int srcaddrlen = sizeof(*a_srcAddr);
    //Получение запроса
    if ( ( recvfrom(m_sockFd, a_dataStore, a_dataStoreSize, 0,
                   (struct sockaddr *)a_srcAddr, &srcaddrlen ) ) == SOCKET_ERROR)
    {
        std::cout << "recvfrom() server failed with error code : "
                  << WSAGetLastError() << '\n';
        exit(EXIT_FAILURE);
    }

    std::cout << m_sockName << " Received packet from " << inet_ntoa(a_srcAddr->sin_addr)
              << " : " << ntohs(a_srcAddr->sin_port) << '\n';
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
*      a_dataStore     - буфер из которого данные отправляются в сокет.
*      a_dataStoreSize - размер передаваемых данных.
*
* @return
* NO_ERROR (0) - при успешной установке неблокирующего режима;
* иначе - ошибка.
*/
void C_Socket::sendData(struct sockaddr_in *a_distAddr, char *a_dataStore, int a_dataStorSize)
{

    std::cout << m_sockName << " Sent message to " << inet_ntoa(a_distAddr->sin_addr)
              << " : " << ntohs(a_distAddr->sin_port) << '\n';

    socklen_t slen = sizeof(*a_distAddr); // Определение размера длины адреса назначения

    if ( ( sendto( m_sockFd,  a_dataStore, a_dataStorSize, 0,
                  (struct sockaddr *)a_distAddr, slen) ) == SOCKET_ERROR )
    {
         std::cout << m_sockName << " sendto() failed with error code : " << WSAGetLastError() << '\n';
         exit(EXIT_FAILURE);
     }

}

/*****************************************************************************
* Закрытие сокета
*
* Выполняет отключение созданного сокета closesocket(m_sockFd) и очистку WSACleanup.
*
*/
void C_Socket::disconnect()
{
    closesocket(m_sockFd);
    WSACleanup();
    std::cout << m_sockName <<" closed the connection  \n";
}

/*****************************************************************************
* Получить имя сокета
*
* @return
*   m_sockName - имя назначенное сокету при создании экземпляра класса;
*/
std::string C_Socket::name()
{
    return m_sockName;
}

/*****************************************************************************
* Получить структуру соединия источника
*
*
* @return
*   m_myAddr - структура собственного адреса сокета;
*/
struct sockaddr_in *C_Socket::servAddr()
{
    return m_myAddr;
}

/*****************************************************************************
* Получить имя сокета
*
* @return
*   m_otherAddr - структура адреса сокета клиента;
*/
struct sockaddr_in *C_Socket::clientAddr()
{
    return m_otherAddr;
}

/* Получить имя сокета
*
* @return
*   m_servIpAddr - ip-адрес сокета сервера;
*/
std::string C_Socket::ipAddress()
{
    return m_servIpAddr;
}

/*****************************************************************************
* Конструктор
*/
C_Socket::C_Socket(std::string a_ipAddr, int a_port, std::string a_sockName) :
    m_servIpAddr(a_ipAddr), m_servPort(a_port), m_sockName(a_sockName),
    m_myAddr(new struct sockaddr_in), m_otherAddr(new struct sockaddr_in)
{}

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






