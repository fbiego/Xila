///
/// @file Power.hpp
/// @author Alix ANNERAUD (alix.anneraud@outlook.fr)
/// @brief
/// @version 0.1.0
/// @date 21-07-2022
///
/// @copyright Copyright (c) 2022
///

#ifndef Power_Hpp_Included
#define Power_Hpp_Included

#include "../Module/Module.hpp"

#include "Battery_Library.hpp"

//==============================================================================//
///
/// @brief Power management module.
///

namespace Xila_Namespace
{
    class Power_Class : public Module_Class, public Battery_Class
    {

    public:
        // -- Constructors / Destructors
        Power_Class();

        // -- Methods

        Result_Type Save_Registry();
        Result_Type Load_Registry();

        uint8_t Get_Battery_Charge_Level();
        uint16_t Get_Battery_Voltage();

        // -- Attributes
        ///
        /// @brief Button press counter.
        ///
        volatile uint8_t Button_Counter;

        ///
        /// @brief Button press timer, used to differentiate a short press from a long press.
        ///
        volatile uint32_t Button_Timer;

        ///
        /// @brief Button interrupt mutex.
        ///
        portMUX_TYPE Button_Mutex;

        // -- Methods -- //
        void static IRAM_ATTR Button_Interrupt_Handler();

        void Check_Button();
        void Deep_Sleep();
    } Power;
}

#endif
