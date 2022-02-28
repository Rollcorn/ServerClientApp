/*****************************************************************************

  i_Socket

  Интерфес сокета


  ИСПОЛЬЗОВАНИЕ

  * Для реализации собственного класса сокета необходимо унаследоваться от данного интерфеса
    и реализовать все виртуальные методы.

  * Интерфейс позволяет реализовывать свои сокеты вне зависимости от используемых библиотек,
    платформ и с различной логикой реализации.

*****************************************************************************/

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN

#endif

#include "i_socket.h"

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <map>

namespace myTask {

/*****************************************************************************
  Macro Definitions
*****************************************************************************/

/*****************************************************************************
  Forward Declarations
*****************************************************************************/

/*****************************************************************************
  Types and Classes Definitions
*****************************************************************************/

class I_Socket
{

public:

    I_Socket(){}

    virtual ~I_Socket(){}

    /**************************************************************************
     * Создание сокета
     *
     * Выполняется инициализация ip-адреса и порта сокета, структуры адреса
     * сокета, выполнения дополнительных опций сокета
     *
     * @param
     *  conParam   - пара значений first - IP-адрес, second - порт;
     *        a_sockAddr - структура адреса сокетаl;
     *        a_optFlag  - флаговая переменная, дополнительных опций сокета;
     */
    virtual bool setup( std::string a_ipParam, std::string a_portParam,
                        int a_optFlag ) = 0;

    /*****************************************************************************
     * Cвязывание сокет с локальным адресом протокола
     *
     * Метод выполняется только для сокета сервера для привязки его к определенному
     * локальному адресу метод производит логирование результата выполнения привязки.
     *
     * @return
     *  успешность открытия сокета
     */
    virtual bool open() = 0;

    /*****************************************************************************
     * Получение данных из сокета
     *
     * Производит получение данных от удаленного сокета.Полученные данные записываются
     * в переданный буфер a_data. По переменной a_recvSize отслеживается размер полученного
     * сообщения.
     *
     * @param
     *  [in] a_data     - буфер в который необходимо записать данные полученные из бефера.
     *       a_dataLen  - размер переданного буфера.
     *       a_recvSize - переменная для хранения размера успешно полученных данных.
     *
     * @return
     *  корректность полученных от удаленного сокета данных
     */
    virtual bool recv( std::string a_remoteIp, std::string a_remotePort,
                       char *a_data, int *a_recvSize ) = 0;

    /*****************************************************************************
     * Отправка данных
     *
     * Выполняет отправку данных по адресу удаленного сокета. Данны отправляются в
     * сокет из переданного буфера a_data. По переменной a_sendSize отслеживается размер
     * полученного сообщения.
     *
     * @param
     * [in] a_data     - буфер из которого данные отправляются в сокет.
     *      a_dataLen  - размер передаваемых данных.
     *      a_sendSize - переменная для хранения размера успешно отправленны данных.
     *
     * @return
     *  успешность отправки данных в удаленный сокет
     */
    virtual bool send( std::string a_remoteIp, std::string a_remotePort,
                       std::string a_data, int *a_sendSize ) = 0;

    /******************************************************************************
     * Закрытие соединения
     *
     * Выполняет отключение созданного сокета
     */
    virtual bool close() = 0;

    /*****************************************************************************
     * Освобождение ресурсов соединения
     */
    virtual bool flush() = 0;

    /*****************************************************************************
     * Cтруктура адреса собственного сокета
     *
     * Возвращает структуру собственного адреса сокета. Необходимо для запуска сокета
     * и прявязке к определенном адресу.
     *
     * @return
     *  структура собственного адреса сокета.
     */
    virtual struct sockaddr_in * ownSockAdr() = 0;

    /*****************************************************************************
     * Cтруктура адреса собственного сокета
     *
     * Возвращает структуру собственного адреса сокета. Необходимо для запуска сокета
     * и прявязке к определенном адресу.
     *
     * @return
     *  структура удаленного адреса сокета.
     */
    virtual struct sockaddr_in * remoteSockAdr() = 0;

    /***************************************************************************
     * Имя сокета
     *
     * Возвращает имя сокета в удобном формате.
     *
     * @return
     *  имя объекта сокета.
     */
    virtual std::string name() = 0;

};

/*****************************************************************************
  Functions Prototypes
*****************************************************************************/

/*****************************************************************************
  Variables Deсlarations
*****************************************************************************/

/*****************************************************************************
  Inline Functions Definitions
*****************************************************************************/

} // namespace myTask
