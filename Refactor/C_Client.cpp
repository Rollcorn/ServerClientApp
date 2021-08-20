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
#include "C_Client.h"

namespace client { // TODO

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
* Создает и инициализирует сокет и выполняет коммуникацию с сокетом сервера
*
* Функция для запуска в потоке работы клиента. Выполняет инициализацию
* Winsock, создает и инициализирует сокет сервера. Клиент работает 30с, а
* затем выполняется отключение сокета
*
* @param
*
* @return
*
*******************************************************************************/
void C_Client::workingSession()
{
    struct sockaddr_in siOther;

    WSADATA wsadata;


    int init_state = initializingWinsock(wsadata);
    int socketLen  = sizeof(siOther);
    int socketFd   = createSock(&siOther);
    int blockState = setNonblock(socketFd);

    communication( socketFd, s_cliBufferSize, (struct sockaddr *)&siOther,
                   socketLen, timer );

    timerThread.join();

    closesocket(socketFd);
    WSACleanup();
    std::cout << " \n Connection closed \n";
}

/*****************************************************************************
*Создает сокет инициализируя параметры сетевого соединения в переданной
* структуры.
*
* @param
* [in] siOther     - указатель на структуру сетвых параметров сокета для
* взаимодействия с сервером.
*
* @return
* sockFd - файловый дескриптор созданного сокета.
*/
int C_Client::createSock(struct sockaddr_in *a_siOther)
{
    int socketFd;

    if ( (socketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
    {
        std::cout <<( "Client: socket() failed with error code : %d" , WSAGetLastError( ) );
        exit(EXIT_FAILURE);
    }

    //setup address structure
    ZeroMemory(a_siOther, sizeof (a_siOther));

    a_siOther->sin_family      = AF_INET;
    a_siOther->sin_port        = htons(s_servPort);
    a_siOther->sin_addr.s_addr = inet_addr(s_servAddr);

    return socketFd;
}

/*****************************************************************************
* В течении 30с отправляет запросы к серверу и выводит полученные ответы
*
* Запросы к серверу отправляются до тех пор пока значение значение параметра
* timer не будет установлено в 1. При Sleep(1000) запросы отправляются раз в секунду
*
* @param
* [in] sockfd    - файловый дескриптор сокета клиента;
* [in] buffSize - размер буфера клиента;
* [in] siOther  - указатель на структуру сетевых параметров сокета;
* [in] slen        - размер
* [in] timer      - параметр для завершения комуникации клиента при переключении
* его в значение 1.
*
* @return
*
*/
void C_Client::communication( int a_sockfd, int a_buffSize, struct sockaddr *a_siOther, socklen_t a_slen)
{
    int timer = 0;
    /*
     * Таймер для прекращения работы функции communication() при переклюяении
     * в значение 1, функция завершается и сокет закрывается.
     */
    std::thread timerThread( [&timer]{ Sleep(30000); timer = 1; } );

    char buffer[a_buffSize];

    while(timer != 1)
    {
            ZeroMemory(buffer, a_buffSize);

            const char *message = "Client: Give me a number!";

            std::cout << "Client: Sent message to " << inet_ntoa( ((struct sockaddr_in *)a_siOther)->sin_addr) <<
                         " : " << ntohs ( ( (struct sockaddr_in *)a_siOther )->sin_port) << '\n';

            if ( sendto ( a_sockfd, message, strlen(message) , 0 , a_siOther, a_slen ) == SOCKET_ERROR )
            {
                std::cout <<"Client: sendto() failed with error code: " << WSAGetLastError() << '\n';
                exit( EXIT_FAILURE );
            }

            if ( ( recvfrom ( a_sockfd, buffer, sizeof(int), 0, a_siOther, &a_slen ) ) == SOCKET_ERROR)
            {
                std::cout <<"Client: recvfrom() failed with error code : " << WSAGetLastError() << '\n';
                exit(EXIT_FAILURE);
            }

            std::cout << "Client: Get number: " << static_cast<int >(*buffer) << '\n';
            Sleep(1000);
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

} // namespace client TODO

