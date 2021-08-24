/*****************************************************************************

  MODULE_OR_CLASS <TODO: заменить на имя модуля/класса>

  Модуль/Класс <TODO: краткое описание назначения в одном предложении> ... (должно быть идентично в .h и .cpp)


  ОПИСАНИЕ <TODO: описание функций и возможностей класса/модуля, форматов используемых данных и т.д., если не очевидно>

  * ...
  * ...
  * ...


  ИСПОЛЬЗОВАНИЕ <TODO: описания правил инициализации и использования, последовательности вызова интерфейсных функций,
                       ограничений и т.д., если не очевидно>

  * ...
  * ...
  * ...


  ПРИМЕЧАНИЯ <TODO: особенности и замечания, не вошедшие в предыдущие разделы (при необходимости)>

  * ...
  * ...

  <TODO: при использовании тематических подразделов в разделах ОПИСАНИЕ, ИСПОЛЬЗОВАНИЕ
         выполнять их нумерацию в соответствии с уровнями вложенности.
         Тематические подразделы вводятся для описания различных аспектов функционирования,
         форматов используемых данных и пр.>

*****************************************************************************/

/*****************************************************************************
  Macro Definitions
*****************************************************************************/
#ifndef SERVER_H
#define SERVER_H
#pragma once

#include <random>
#include <ctime>
#include <typeinfo>

#include "C_Socket.h"

namespace server { // TODO

/*****************************************************************************
  Forward Declarations
*****************************************************************************/

/*****************************************************************************
  Types and Classes Definitions
*****************************************************************************/

class C_Server
{
public:

    //Запуск работы сервера
    void workingSession();

    C_Server(std::string a_ipAddr, int a_port, std::string a_sockName);
    ~C_Server() = default;

private:
    mySocket::C_Socket  m_socket;   //объект обеспечивающий связь клиента с сервером
    const int           m_servBufferSize = 1024;  //Размер буфера сервера

    //Запуск сокета сервера
    void setupConnect(int addr, int protocol);

    //Обмен данными с клиентом
    void communication (int buffSize);



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
} // namespace server TODO

#endif // SERVER_H
