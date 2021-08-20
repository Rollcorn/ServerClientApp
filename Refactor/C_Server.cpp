/*****************************************************************************

  MODULE_OR_CLASS <TODO: заменить на имя модуля/класса>

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
#include "C_Server.h"

namespace server { // TODO

/*****************************************************************************
  Forward Declarations
*****************************************************************************/

/*****************************************************************************
  Types and Classes Definitions
*****************************************************************************/

/*****************************************************************************
* Создает и инициализирует сокет и выполняет коммуникацию с сокетом сервера
*
* Функция для запуска в потоке работы клиента. Выполняет инициализацию
* Winsock, создает и инициализирует сокет клиента. коммуникация с клиентом
* происходит исходя из функции communication(). Работа продолжается пока
* клиент не закроет соединение.
*
* @param
*
* @return
*
*******************************************************************************/
void C_Server::workingSession()
{
    WSADATA wsadata;

    int socket_fd;
    struct sockaddr_in server;
    struct sockaddr_in siOther;
    int slen = sizeof(siOther);

    initializingWinsock(wsadata);

    socket_fd = createSock(&server);


    int block_state = setNonblock(socket_fd);

    if( bind( socket_fd, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        std::cout << "Server: Bind failed with error code : " << WSAGetLastError() <<'\n';
        exit(EXIT_FAILURE);
    }
    puts("Server: Bind done");

    puts("Server: Communication start...");
    communication( socket_fd, s_servBufferSize, &siOther, slen );
}

/*****************************************************************************
 * Создает сокет инициализируя параметры сетевого соединения в переданной
 * структуры.
 *
 * @param
 *  [in]  siOther     - указатель на структуру сетвых параметров сокета для
 *                      взаимодействия с сервером.
 *
 * @return
 *  sockFd - файловый дескриптор созданного сокета.
 */
int C_Server::createSock( struct sockaddr_in *a_servaddr )
{
    int sockFd = INVALID_SOCKET;

    if ( ( sockFd = socket(AF_INET , SOCK_DGRAM , IP) ) == INVALID_SOCKET )
    {
        std::cout << "Server: Could not create socket : " << WSAGetLastError() << '\n';
    }
    std::cout << "Server: Socket created.\n";

    ZeroMemory(a_servaddr, sizeof(a_servaddr));

    a_servaddr->sin_family      = AF_INET;
    a_servaddr->sin_port        = htons(s_servPort);
    a_servaddr->sin_addr.s_addr = INADDR_ANY;

    return sockFd;
}

/*****************************************************************************
* Принимает запросы от сервера и отправляет в ответ случайное число
* от -100 до 100
*
* Пока клиент не закроет соединение соервер отсылает на каждый запрос клиента
* случайное число
*
*
* @param
* [in] sockfd    - файловый дескриптор сокета клиента;
* [in] buffSize - размер буфера клиента;
* [in] siOther  - указатель на структуру сетевых параметров сокета;
* [in] slen        - размер
*
* @return
*
********************************************************************************/
void C_Server::communication( int a_socketFd, int a_buffSize,struct sockaddr_in *a_siOther, socklen_t a_siLen)
{
    char buffer[a_buffSize];
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(-100,100);

    while(true)
    {
        std::cout << "Server: Waiting for data...\n";
        fflush(stdout);

        ZeroMemory(buffer, a_buffSize);

        //Получение запроса
        if ( ( recvfrom(a_socketFd, buffer, a_buffSize, 0,
                                      (struct sockaddr *)a_siOther, &siLen ) ) == SOCKET_ERROR)
        {
            std::cout << "recvfrom() server failed with error code : " << WSAGetLastError() << '\n';
            exit(EXIT_FAILURE);
        }

       std::cout << "Server: Received packet from " << inet_ntoa(a_siOther->sin_addr) << " : "
                      << ntohs(a_siOther->sin_port) << '\n';
       std::cout << "Server: Data: " << buffer << '\n';

       int num = dist(gen);

       //Отправка ответа клиенту
       if ( ( sendto(a_socketFd,  (char *)&num, sizeof(num), 0,
                             (struct sockaddr *)a_siOther, siLen ) ) == SOCKET_ERROR )
       {
            std::cout << "sendto() server failed with error code : " << WSAGetLastError() << '\n';
            exit(EXIT_FAILURE);
        }

    }
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

} // namespace server TODO




