#pragma once
#include <string>
#include <vector>
#include <utility>

#include "i_socket.h"


namespace myTask {

class Socket : public I_Socket {

public:

    // Запуск сокета
    bool setup( int a_protocol = IPPROTO_UDP, int a_type = SOCK_DGRAM,
                    int a_ipFamily = AF_INET );

    // Настройки сокета
    Socket* createSock(std::string protoType, std::vector< std::pair<std::string, int> > conectData);

    // Cвязывание сокет с локальным адресом протокола
    bool open();

    // Получение данных
    bool recv( struct sockaddr_in *a_srcAddr, char *a_data,
               int a_dataLen, int *a_recvSize );

    // Отправка данных
    bool send( struct sockaddr_in *a_distAddr, char *a_data,
               int a_dataLen, int *a_sendSize );

    // Закрытие сокета
    bool flush();

    // Получение структуры адреса источника
    struct sockaddr_in * servAddr();

    // Получение структуры адреса клиента
    struct sockaddr_in * clientAddr();

    // Получение ip адреса сервера
    std::string ipAddress();


    ~Socket();

private:

    Socket();

    // Инициализация библиотеки WinSock2
    bool initWinsock();

    // Параметры протокола
    int m_ipFamily; // IP протокол соединения
    int m_type;     // тип соединения сокета
    int m_protocol; // протокол соединения сокета

    // Параметры соединения
    std::string m_servIpAddr;   // IP адресс сервера
    short       m_servPort;     // Порт сервера
    std::string m_sockName;     // Имя сокета отображаемое в логе

    struct sockaddr_in *m_myAddr;    // Параметры соединения сервера
    struct sockaddr_in *m_otherAddr; // Параметры соединения клиента


    // Данные сокета
    WSADATA m_wsadata;
    int     m_sockFd = INVALID_SOCKET;  // дескриптор сокета

protected:

    struct T_SockTransProt{
        struct {
            int a_protocol = IPPROTO_UDP;
            int a_type = SOCK_DGRAM;
            int a_ipFamily = AF_INET;
        } UDP;
    }T_SockTransProt;

    // Параметры состояния сокета блокировки при передачи данных
    enum Flags{
    optNo       = 0x00,
    optNonblock = 0x01,
    };
};
}
