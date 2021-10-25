/*****************************************************************************

  SocketCreator

  Интерфейс фабрики создания сокета


  РЕАЛИЗАЦИЯ

  * Если в функцию передается идентификатор "UDP" - функция вернет указатеь на
    объект с интерфейсом I_Socket в который будент инстанцированна реализация
    сокета под UDP протокол.

  * При передаче других идентификаторов функция вернет nullptr.

*****************************************************************************/

#include "SocketCreator.h"


/*****************************************************************************
  Macro Definitions
*****************************************************************************/


namespace myTask {

/*****************************************************************************
  Forward Declarations
*****************************************************************************/

/*****************************************************************************
  Types and Classes Definitions
*****************************************************************************/

/*****************************************************************************
  Functions Prototypes
*****************************************************************************/

/****************************************************************************
 * Создание сокета соответствующего переданному ключу
 *
 * Метод получает имя протокола сокета и если имеется реализация конкретной
 * фабрики данного протокола, то возвращает
 *
 * @param
 *  [in] a_protoName - идентификатор сокета.
 *
 * @return
 *  Объект с интерфейсом I_Socket, реализованным в соответствии с заданным
 *  протоколом.
 */
I_Socket * CreateSocket( std::string a_protoName )
{
    I_Socket* mySock = nullptr;

    if ( a_protoName.compare("UDP") == 0 ) {
        mySock = new C_UdpSocket;
    }

    return mySock;
}

/*****************************************************************************
  Variables Deсlarations
*****************************************************************************/

/*****************************************************************************
  Inline Functions Definitions
*****************************************************************************/

} // namespace myTask

