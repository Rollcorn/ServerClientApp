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

#include "client.h"
#include <ws2tcpip.h>

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


void C_Client::working_session()
{
    struct sockaddr_in si_other;

    int socket_fd;
    int socket_len = sizeof(si_other);
    char message[m_client_buf_size];
    char buffer[m_client_buf_size];

    WSADATA wsadata;

    std::cout << "\nInitialising Winsock..." << '\n';
    if (WSAStartup(MAKEWORD(2,2),&wsadata) != 0)
    {
        std::cout << "Client Failed. Error Code : " << WSAGetLastError() << '\n';
        exit(EXIT_FAILURE);
    }
    std::cout <<"Initialised." << '\n';

    //create socket
    socket_fd = make_sock(socket_fd, &si_other);
    u_long iMode = 0;
    int block_state= set_nonblock(socket_fd, iMode);

    //start communication
    dg_client(socket_fd, message, buffer, (struct sockaddr *)&si_other, socket_len);

    closesocket(socket_fd);
    WSACleanup();
}

int C_Client::make_sock(int socket_fd, struct sockaddr_in *si_other)
{

    if ( (socket_fd=socket(AF_INET, SOCK_DGRAM, 0)) == SOCKET_ERROR)
    {
        std::cout <<("socket() client failed with error code : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    //setup address structure
    ZeroMemory(si_other, sizeof (si_other));

    si_other->sin_family = AF_INET;
    si_other->sin_port = htons(SERV_PORT);
    si_other->sin_addr.s_addr = inet_addr(SERVER_ADDRESS);

    return socket_fd;
}

void C_Client::dg_client( int sockfd, char *message, char * buff, struct sockaddr *si_other,
                                                                                                    socklen_t slen )
{
    while(1)
        {
            std::cout << "Enter message: " << '\n';
            gets(message);

            //send the message
            if (sendto(sockfd, message, strlen(message) , 0 , si_other, slen) == SOCKET_ERROR)
            {
                std::cout <<"sendto() client failed with error code: " << WSAGetLastError() << '\n';
                exit(EXIT_FAILURE);
            }

            //receive a reply and print it
            //clear the buffer by filling null, it might have previously received data
            ZeroMemory(buff, sizeof (*buff));
            //try to receive some data, this is a blocking call
            if (recvfrom(sockfd, buff, m_client_buf_size, 0, si_other, &slen) == SOCKET_ERROR)
            {
                std::cout <<"recvfrom() client failed with error code : " << WSAGetLastError() << '\n';
                exit(EXIT_FAILURE);
            }

            puts(buff);
        }


/*
    int n;

    while( fgets( send_buffer, m_CLI_BUFF_SIZE, fp ) != nullptr )
    {
        sendto( sockfd, send_buffer, strlen(send_buffer), 0, pservaddr, servlen );

        n = recvfrom( sockfd, recv_buffer, m_CLI_BUFF_SIZE , 0, nullptr, nullptr );

        recv_buffer[n] = 0;
        fputs(recv_buffer, stdout);
    }
*/
}

namespace {

} // namespace
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

