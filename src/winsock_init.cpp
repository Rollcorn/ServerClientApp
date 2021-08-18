#include "winsock_init.h"

int C_Winsock_Init::initializingWinsock(WSADATA &wsadata)
{
    int result_check = WSAStartup(MAKEWORD( 2, 2 ), &wsadata);
    if (result_check != 0)
        std::cout << "WSAStartup failed: " << result_check << '\n';

    return result_check;
}

int C_Winsock_Init::create_sock( int sock_fd, struct sockaddr_in *serv_addr )
{

    if((sock_fd = socket(AF_INET , SOCK_DGRAM , 0 )) == INVALID_SOCKET)
    {
        std::cout << "Could not create socket : " << WSAGetLastError() << '\n';
    }
    std::cout << "Socket created.\n";


    ZeroMemory(serv_addr, sizeof(serv_addr));

    serv_addr->sin_family = AF_INET;
    serv_addr->sin_addr.s_addr =INADDR_ANY;
    serv_addr->sin_port = htons(SERV_PORT);

    return sock_fd;
}

int C_Winsock_Init::set_nonblock(int socket_fd, u_long flags){
    int blocking_state = ioctlsocket(socket_fd, FIONBIO, &flags);
    if (blocking_state != NO_ERROR)
        std::cout << "ioctlsocket failed with error: " << blocking_state << '\n';
    return blocking_state;
}




