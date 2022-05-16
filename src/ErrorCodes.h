#pragma once
#include <winsock2.h>

namespace myTask {

    enum E_ConectCodes {
        // не критические коды bind()
        ERR_BINDPROGRESS = WSAEINPROGRESS,
        ERR_ALREADYBOUND = WSAEINVAL,
        
        // не критические коды send()
        ERR_BUFEMPT              = WSAEWOULDBLOCK,
        CONNECT_HAS_BEEN_BROCKEN = WSAENETRESET,
        MESS_LARGER_THAN_MAX     = WSAEMSGSIZE,
        NET_NOT_REACHED          = WSAENETUNREACH,
        CONN_HAS_BEEN_DROPPED    = WSAETIMEDOUT,
    } ;
}
