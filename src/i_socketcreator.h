#pragma once
#include "i_socket.h";

namespace myTask{

class I_SocketCreator
{

public:
    virtual ~I_SocketCreator(){}

    virtual I_Socket* FactoryMethod() = 0;

};

} // namespace myTask
