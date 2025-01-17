///
/// @file Modules.hpp
/// @author Alix ANNERAUD (alix.anneraud@outlook.fr)
/// @brief
/// @version 0.1.0
/// @date 20-07-2022
///
/// @copyright Copyright (c) 2022
///

#ifndef Modules_Hpp_Included
#define Modules_Hpp_Included

#include <vector>
#include "Arduino.h"
#include "ArduinoJson.h"
#include "FS.h"
#include "ArduinoTrace.h"

// - - Base types

#include "Base_Types.hpp"

// - - Other types

#include "../Communication/IP_Address.hpp"
#include "Task.hpp"
#include "Instruction.hpp"
#include "Semaphore.hpp"
#include "Queue.hpp"
#include "String.hpp"
#include "Date.hpp"
#include "Time.hpp"

#include "../Configuration/Configuration.hpp"
#include "../Configuration/Path.hpp"

namespace Xila_Namespace
{
    typedef class Module_Class
    {
    public:  
        // - Methods

        Module_Class(Size_Type Queue_Size = Default_Instruction_Queue_Size);
        ~Module_Class();

        void Send_Instruction(const Instruction_Type &Instruction);

        Instruction_Type Get_Instruction();
        Size_Type Instruction_Available();

    private:
        QueueHandle_t Instruction_Queue_Handle;

        static std::vector<Module_Class *> List; // - Module lists.

    } Module_Type;

#define Instruction_Macro()

}

#endif
