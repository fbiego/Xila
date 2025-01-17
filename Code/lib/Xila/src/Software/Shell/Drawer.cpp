///
/// @file Drawer.cpp
/// @author Alix ANNERAUD (alix.anneraud@outlook.fr)
/// @brief
/// @version 0.1.0
/// @date 12-09-2022
///
/// @copyright Copyright (c) 2022
///

#include "Software/Shell/Shell.hpp"

Shell_Class::Drawer_Class::Drawer_Class(Shell_Class *Shell_Pointer) : Shell_Pointer(Shell_Pointer)
{
    Window.Create();
    Window.Set_Title("Drawer");

    // - Set flex container
    Window.Get_Body().Set_Flex_Flow(Flex_Flow_Type::Column_Wrap);
    Window.Get_Body().Set_Style_Pad_Row(5, 0);
    Window.Get_Body().Set_Style_Pad_Column(5, 0);
     

    {
        Object_Type Container;
        Button_Type Icon;
        Label_Type Label;

        for (auto &Software_Handle_Pointer : Software_Handle_Class::List)
        {
            if (*Software_Handle_Pointer != Shell_Class::Handle)
            {
                Container.Create(Window.Get_Body());
                Container.Set_Size(10 * 8, 9 * 8);
                Container.Set_Style_Background_Opacity(Opacity_Type::Transparent, 0);
                Container.Set_Style_Pad_All(0, 0);
                Container.Add_Event(Shell_Pointer, Graphics_Type::Event_Code_Type::Clicked);

                Icon.Create(Container);
                Icon.Set_Size(32, 32);
                Icon.Set_Alignment(Alignment_Type::Top_Middle);
                Icon.Add_Flag(Flag_Type::Event_Bubble);
                Icon.Add_Event(Shell_Pointer, Graphics_Type::Event_Code_Type::Clicked);

                Label.Create(Container);
                Label.Set_Size(Percentage(100), 32);
                Label.Set_Alignment(Alignment_Type::Bottom_Middle);
                Label.Set_Long_Mode(Label_Type::Long_Dot);
                Label.Add_Flag(Flag_Type::Event_Bubble);
                Label.Set_Text(Software_Handle_Pointer->Get_Name());
                Label.Add_Event(Shell_Pointer, Graphics_Type::Event_Code_Type::Clicked);

                Container.Clear_Pointer();
                Icon.Clear_Pointer();
                Label.Clear_Pointer();
            }
        }
    }
}

Shell_Class::Drawer_Class::~Drawer_Class()
{
    Window.Delete();
}

void Shell_Class::Drawer_Class::Execute_Instruction(Instruction_Type Instruction)
{
    if (Instruction.Get_Sender() == &Graphics)
    {
        switch (Instruction.Graphics.Get_Code())
        {
        case Graphics_Type::Pressed :
            for (uint8_t i = 0; i < Window.Get_Body().Get_Child_Count(); i++)
            {
                if (Instruction.Graphics.Get_Object() == Window.Get_Body().Get_Child(i))
                {
                    Software_Handle_Class::List[i]->Create_Instance();
                    break;
                }
            }
            break;
        default:
            break;
        }
    }
}


void Shell_Class::Drawer_Class::Open(Shell_Class *Shell_Pointer)
{
    Shell_Pointer->Drawer_Pointer = new Drawer_Class(Shell_Pointer);
}

void Shell_Class::Drawer_Class::Close(Shell_Class *Shell_Pointer)
{
    delete Shell_Pointer->Drawer_Pointer;
    Shell_Pointer->Drawer_Pointer = NULL;
}

bool Shell_Class::Drawer_Class::Is_Openned(Shell_Class *Shell_Pointer)
{
    if (Shell_Pointer->Drawer_Pointer == NULL)
    {
        return true;
    }
    return false;
}