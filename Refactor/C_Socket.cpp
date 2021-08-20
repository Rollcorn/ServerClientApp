/*****************************************************************************

  MODULE_OR_CLASS <C_Class>

  Модуль/Класс <TODO: краткое описание назначения в одном предложении> ... (должно быть идентично в .h и .cpp)


  РЕАЛИЗАЦИЯ <TODO: описание особенностей реализации и используемых алгоритмов, если они не тривиальны и отсутствует документация>

  * ...
  * ...
  * ...


  ПРИМЕЧАНИЯ <TODO: особенности и замечания, не вошедшие в предыдущие разделы (при необходимости)>

  * ...
  * ...

  <TODO: при использовании тематических подразделов в разделе РЕАЛИЗАЦИЯ
         выполнять их нумерацию в соответствии с уровнями вложенности.
         Тематические подразделы вводятся для описания отдельных алгоритмов,
         форматов используемых данных и пр.>

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
* Выполняет иницициализацию библиотек Winsock.
*
* Для использования функций библиотеки WInsock использующих DLL библиотеки
* выполняется инициация использования WS2_32.dll.
*
* @param
* [in] wsadata     - объект WSADATA
*
* @return
* 0                         - при успешной инициализации библиотек winsock;
* EXIT_FAILURE  - при неудаче.
*/
int C_Socket::initializingWinsock()
{
    int initState;
    std::cout << "Initialising Winsock...\n";
    if ( ( initState = WSAStartup( MAKEWORD(2,2), &m_wsadata ) ) != 0 )
    {
        std::cout << "Client Failed. Error Code : " << WSAGetLastError() << '\n';
        exit(EXIT_FAILURE);
    }

    std::cout <<"Initialised.\n";
    return initState;

}

C_Socket::createSock(int a_family, int a_type, int a_ipProto, )
{
    m_type      = a_type;
    m_protocol  = a_ipProto;

    if ( ( m_sockFd = socket(a_family , a_type, a_ipProto) ) == INVALID_SOCKET )
    {
        std::cout << "Could not create socket : " << WSAGetLastError() << '\n';
    }

    ZeroMemory(m_servaddr, sizeof(m_servaddr));

    m_servaddr->sin_family      = a_family;
    m_servaddr->sin_port        = htons(m_servPort);
    m_servaddr->sin_addr.s_addr = INADDR_ANY;

    std::cout << "Socket created.\n";
}

void C_Socket::socketSettings(int a_family, int a_type, int a_ipProto)
{
    m_type      = a_type;
    m_protocol  = a_ipProto;

    ZeroMemory(m_servaddr, sizeof(m_servaddr));

    m_servaddr->sin_family = a_family;
    m_servaddr->sin_port = htons(m_servPort);
    m_servaddr->sin_addr.s_addr = INADDR_ANY;

}

/*****************************************************************************
* Выполняет установку сокета в неблокирующий режим.
*
* Выполняет установку сокета в режим неблокирующего ввода/вывода.
*
* @param
* [in] socketFd - файловый дескриптор сокета передаваемого для установки в
* неблокирующий режим.
*
* @return
* NO_ERROR (0) - при успешной установке неблокирующего режима;
* иначе - ошибка.
*/
int C_Socket::setNonblock( int socketFd){
    u_long flags = 0;
    int blockingState = ioctlsocket( socketFd, FIONBIO, &flags );

    if (blockingState != NO_ERROR)
        std::cout << "ioctlsocket failed with error: " << blockingState << '\n';

    return blockingState;
}

void C_Socket::makeBind()
{
    if( bind( socket_fd, (struct sockaddr *)&m_servaddr, sizeof(server)) == SOCKET_ERROR)
    {
        std::cout << "Server: Bind failed with error code : " << WSAGetLastError() <<'\n';
        exit(EXIT_FAILURE);
    }
}




C_Socket::C_Socket(std::string a_pAddr, std::string a_port) : m_servIpAddr(a_pAddr), m_servPort(a_port)
{}



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






