#pragma once
#include <string>
#include <vector>
#include <utility>

#include "C_UDPSocket.h"
#include "i_socketcreator.h"


namespace myTask {

class C_UPDSocketCreator : public I_SocketCreator {

public:

    C_UPDSocketCreator();

    ~C_UPDSocketCreator();

    // функция фабрики сокета
    I_Socket* FactoryMethod() override;


};


} // namespace myTask
