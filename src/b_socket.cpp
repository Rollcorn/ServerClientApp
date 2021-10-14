#include "b_socket.h"

namespace myTask {


I_Socket* Socket::createSock(std::string protoType, std::vector< std::pair<std::string, int> > conectData)
{
    Socket* socket = new Socket();
    if( protoType.compare("UDP") ) {
        socket->m_ipFamily = T_SockTransProt.UDP.a_ipFamily;
        socket->m_type     = T_SockTransProt.UDP.a_type;
        socket->m_protocol = T_SockTransProt.UDP.a_protocol;
    }

    return socket;
}

Socket::Socket()
{

}

} // myTask
