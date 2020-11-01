#include "Music_Player.hpp"

#define INSTANCE_POINTER Music_Player_Class::Instance_Pointer

Music_Player_Class *Music_Player_Class::Instance_Pointer = NULL;

Software_Class* Music_Player_Class::Load()
{
    if (Instance_Pointer == NULL)
    {
        Instance_Pointer = new Music_Player_Class();
    }
    return Instance_Pointer;
}

Music_Player_Class::Music_Player_Class() : Software_Class(6)
{

}

Music_Player_Class::~Music_Player_Class()
{
    
}

void Music_Player_Task(void *pvParameters)
{
    (void)pvParameters;
    while (1)
    {
        switch (INSTANCE_POINTER->Get_Command())
        {
        case 0:

            //idle state
            vTaskDelay(pdMS_TO_TICKS(20));
            break;
        case 0xFF:
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void Music_Player_Class::Pause()
{
    
}

#undef INSTANCE_POINTER