///
/// @file Power.cpp
/// @author Alix ANNERAUD (alix.anneraud@outlook.fr)
/// @brief
/// @version 0.1
/// @date 28-03-2021
///
/// @copyright Copyright (c) 2021
///

#include "Core/Core.hpp"
#include "soc/rtc_wdt.h"

Xila_Class::Power_Class::Power_Class()
{
    Button_Mutex = portMUX_INITIALIZER_UNLOCKED;
    Button_Counter = false;
}

void IRAM_ATTR Xila_Class::Power_Class::Button_Handler()
{
    Verbose_Print_Line("Button triggered");
    vTaskEnterCritical(&Xila.Power.Button_Mutex);
    Xila.Power.Button_Counter = 1;

    vTaskExitCritical(&Xila.Power.Button_Mutex);
}

void Xila_Class::Power_Class::Check_Button()
{
    if (Button_Counter != 0)
    {
        Xila.Software.Execute_Shell(Software_Class::Dialog_Power);
        Xila.Software.Maximize(Shell_Handle);
        Button_Counter = 0;
    }
}
