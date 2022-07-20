///
 /// @file Task.hpp
 /// @author Alix ANNERAUD (alix.anneraud@outlook.fr)
 /// @brief 
 /// @version 0.1
 /// @date 29-03-2021
 /// 
 /// @copyright Copyright (c) 2021
 /// 

 #ifndef TASK_HPP_INCLUDED
 #define TASK_HPP_INCLUDED

 #include "Arduino.h"

 #define Class Xila_Class::Task_Class

inline void Class::Resume(Xila_Class::Task_Handle Task_To_Resume) const
{
  vTaskResume(Task_To_Resume);
}

inline void Class::Suspend(Xila_Class::Task_Handle Task_To_Suspend) const
{
  vTaskSuspend(Task_To_Suspend);
}

inline Result_Type Class::Create(Xila_Class::Task_Function (*Task_Function)(void *), const char *Task_Name, size_t Stack_Size, void *pvParameters, Xila_Class::Task_Handle *Task_Handle) const
{
  if (xTaskCreatePinnedToCore(Task_Function, Task_Name, Stack_Size, pvParameters, Software_Task, Task_Handle, tskNO_AFFINITY) != pdPASS)
  {
    return Error;
  }
  return Success;
}

inline Result_Type Class::Create(Xila_Class::Task_Function (*Task_Function)(void *), const char *Task_Name, size_t Stack_Size, void *pvParameters, uint16_t Priority, Xila_Class::Task_Handle *Task_Handle)
{
  if (xTaskCreatePinnedToCore(Task_Function, Task_Name, Stack_Size, pvParameters, Priority, Task_Handle, tskNO_AFFINITY) != pdPASS)
  {
    return Error;
  }
  return Success;
}

inline void Class::Delete(Xila_Class::Task_Handle Task_To_Delete) const
{
  vTaskDelete(Task_To_Delete);
}

///
/// @brief A delay function.
/// @param Delay_In_Millisecond
/// @details A delay function that behave exactly like delay()
///
inline void Class::Delay(uint32_t Delay_In_Millisecond) const
{
  vTaskDelay(pdMS_TO_TICKS(Delay_In_Millisecond));
}

 #endif