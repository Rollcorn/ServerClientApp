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
#include "server.h"

namespace server { // TODO

/*****************************************************************************
  Forward Declarations
*****************************************************************************/

/*****************************************************************************
  Types and Classes Definitions
*****************************************************************************/

void C_Server::working_session()
{
    char buffer[SER_BUFFER_SIZE];
    int socket_fd;
    struct sockaddr_in server;
    struct sockaddr_in si_other;
    int slen = sizeof(si_other);

    WSADATA wsadata;

    int init_check = initializingWinsock(wsadata);

    socket_fd = create_sock(socket_fd, &server);
/*
    u_long iMode = 0;
    int block_state = set_nonblock(socket_fd, iMode);
*/
    if( bind( socket_fd, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        std::cout << "Bind failed with error code : " << WSAGetLastError() <<'\n';
        exit(EXIT_FAILURE);
    }
    puts("Bind done");



    dg_echo( socket_fd, buffer, SER_BUFFER_SIZE, &si_other, slen );
    closesocket(socket_fd);
    WSACleanup();
}

void C_Server::dg_echo(int socket_fd, char *buff, int buff_len, struct sockaddr_in *si_other, socklen_t si_len)
{
    int recv_len;

    while(true)
        {
            std::cout << "Waiting for data...\n";
            fflush(stdout);

            //clear the buffer by filling null, it might have previously received data
            ZeroMemory(buff, buff_len);

            //try to receive some data, this is a blocking call
            if ((recv_len = recvfrom(socket_fd, buff, buff_len, 0,
                                      (struct sockaddr *)si_other, &si_len)) == SOCKET_ERROR)
            {
                std::cout << "recvfrom() server failed with error code : " << WSAGetLastError() << '\n';
                exit(EXIT_FAILURE);
            }

            //print details of the client/peer and the data received
            std::cout << "Received packet from " << inet_ntoa(si_other->sin_addr) << " : "
                      << ntohs(si_other->sin_port) << '\n';
            std::cout << "Data: " << buff << '\n';

            //now reply the client with the same data
            if (sendto(socket_fd, buff, recv_len, 0,  (struct sockaddr *)si_other, si_len) == SOCKET_ERROR)
            {
                std::cout << "sendto() server failed with error code : " << WSAGetLastError() << '\n';
                exit(EXIT_FAILURE);
            }
        }
}

// namespace
/*****************************************************************************
  Functions Prototypes
*****************************************************************************/

/*****************************************************************************
  Variables Definitions
*****************************************************************************/

/*****************************************************************************
  Functions Definitions
*****************************************************************************/


} // namespace foo TODO




