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

#include "C_UdpSocket.h"


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
 * @param
 *  [in] a_type - идентификатор типа интерфейса
 *
 * @return
 *  nullptr - ошибка создания (неизвестный ид. типа)
 *  иначе   - объект реализации I_Connection заданного типа
 */
I_Connection * createSocket( const std::string &a_type )
{
    I_Connection *mySock = nullptr;

    if ( a_type.compare("UDP") == 0 ) {
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

