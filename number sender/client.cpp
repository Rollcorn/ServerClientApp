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

    WSADATA wsadata;

    int init_state  = initializingWinsock(wsadata);

    //create socket
    socket_fd = create_sock(&si_other);
    u_long iMode = 0;
    int block_state= set_nonblock(socket_fd, iMode);

    int timer = 0;
    std::thread m_thread( [&timer]{ Sleep(30000);
                                                            timer = 1; } );
    //start communication
    dg_client(socket_fd, m_client_buf_size,
                        (struct sockaddr *)&si_other, socket_len, timer);

    m_thread.join();

    closesocket(socket_fd);
    WSACleanup();

    std::cout << " \nConnection closed \n";

}

int C_Client::create_sock(struct sockaddr_in *si_other)
{
    int socket_fd;

    if ( (socket_fd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
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

void C_Client::dg_client( int sockfd, int buff_size,
                                            struct sockaddr *si_other, socklen_t slen , int &timer)
{
    char buffer[buff_size];

    while(timer != 1)
    {
            int recv_len;
            ZeroMemory(buffer, buff_size);

            Sleep(1000);
            const char *message = "Give me a number!";

            //send the message
            if (sendto(sockfd, message, strlen(message) , 0 , si_other, slen) == SOCKET_ERROR)
            {
                std::cout <<"sendto() client failed with error code: " << WSAGetLastError() << '\n';
                exit(EXIT_FAILURE);
            }

            //receive a reply and print it
            //clear the buffer by filling null, it might have previously received data
            //try to receive some data, this is a blocking call
            if ((recv_len = recvfrom(sockfd, buffer, sizeof(int), 0,
                                    si_other, &slen)) == SOCKET_ERROR)
            {
                std::cout <<"recvfrom() client failed with error code : " << WSAGetLastError() << '\n';
                exit(EXIT_FAILURE);
            }
//            std::cout << "Recv length: " << recv_len << '\n';
            std::cout << "Get number: " << static_cast<int >(*buffer) << '\n';
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

