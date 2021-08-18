#include "winsock_init.h"

int C_Winsock_Init::initializingWinsock(WSADATA &wsadata)
{
    int init_state;
    std::cout << "\nInitialising Winsock..." << '\n';
    if ((init_state = WSAStartup(MAKEWORD(2,2),&wsadata)) != 0)
    {
        std::cout << "Client Failed. Error Code : " << WSAGetLastError() << '\n';
        exit(EXIT_FAILURE);
    }
    std::cout <<"Initialised.\n";
    return init_state;

}


int C_Winsock_Init::set_nonblock(int socket_fd, u_long flags){
    int blocking_state = ioctlsocket(socket_fd, FIONBIO, &flags);
    if (blocking_state != NO_ERROR)
        std::cout << "ioctlsocket failed with error: " << blocking_state << '\n';
    return blocking_state;
}




