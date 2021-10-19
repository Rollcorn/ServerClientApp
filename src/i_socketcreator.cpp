/*****************************************************************************

  I_SocketCreator

  Интерфейс фабрики создания сокета

  ИСПОЛЬЗОВАНИЕ

  * Объявляет фабричный метод, возвращающий объект типа I_Socket.
    Creator может также определять реализацию по умолчанию фабричного
    метода, который возвращает объект ConcreteProduct;
  * может вызывать фабричный метод для создания объекта I_Socket.
  * I_Socket – определяет интерфейс объектов, создаваемых фабричным методом;
  * Определяет интерфейс для создания объекта, но оставляет подклассам
    решение о том, какой класс инстанцировать. Фабричный метод - FactoryMethod()
    позволяет классу делегировать инстанцирование подклассам.


*****************************************************************************/

#include "i_socketcreator.h"


/*****************************************************************************
  Macro Definitions
*****************************************************************************/


namespace myTask{
/*****************************************************************************
  Forward Declarations
*****************************************************************************/


/*****************************************************************************
  Types and Classes Definitions
*****************************************************************************/


/****************************************************************************
 * Создает и возвращает объект-сокет в сооветствии с переданным ключом
 *
 * Метод получает имя протокола сокета и если имеется реализация конкретной
 * фабрики данного протокола, то возвращает объект с интерфейсом I_Socket и
 * реализованным в соответствии с переданным протоколом.
 *
 * [in] protoName - имя протокола
 *
 * @return
 */
I_Socket* MakeSocket( std::string  a_protoName ){

    if( a_protoName == "UDP" ) {
        I_Socket* mySock = new C_UDPSocket();
        return mySock;
    }

    return nullptr;
}


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

