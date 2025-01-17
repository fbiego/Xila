///
/// @file Login.cpp
/// @author Alix ANNERAUD (alix.anneraud@outlook.fr)
/// @brief
/// @version 0.1.0
/// @date 13-01-2023
///
/// @copyright Copyright (c) 2023
///

#include "Software/Shell/Shell.hpp"

Shell_Class::Login_Class::Login_Class(Shell_Class *Shell_Pointer) : Shell_Pointer(Shell_Pointer)
{
    Dialog.Create();
    Dialog.Set_Title("Login");

    Name_Input.Create(Dialog);
    Name_Input.Set_Size(Percentage(80), 40);
    Name_Input.Set_Alignment(Alignment_Type::Top_Middle, 0, 10);
    Name_Input.Set_Placeholder_Text("Username");
    Name_Input.Set_One_Line(true);
    Name_Input.Set_Maximum_Length(Maximum_Username_Length);
    Name_Input.Add_Event(Shell_Pointer, Graphics_Type::Focused);
    Name_Input.Add_Event(Shell_Pointer, Graphics_Type::Defocused);

    Password_Input.Create(Dialog);
    Password_Input.Set_Size(Percentage(80), 40);
    Password_Input.Set_Alignment(Name_Input, Alignment_Type::Out_Bottom_Middle, 0, 20);
    Password_Input.Set_Placeholder_Text("Password");
    Password_Input.Set_Password_Mode(true);
    Password_Input.Set_One_Line(true);
    Password_Input.Set_Maximum_Length(Maximum_Password_Length);
    Password_Input.Add_Event(Shell_Pointer, Graphics_Type::Focused);
    Password_Input.Add_Event(Shell_Pointer, Graphics_Type::Defocused);

    Login_Button.Create(Dialog);
    Login_Button.Set_Size(Percentage(50), 40);
    Login_Button.Set_Alignment(Alignment_Type::Bottom_Middle, 0, -10);
    Login_Button.Add_Event(Shell_Pointer, Graphics_Type::Clicked);

    Login_Label.Create(Login_Button);
    Login_Label.Set_Text("Login");
    Login_Label.Set_Alignment(Alignment_Type::Center);

    // TODO : 
    Keyboard.Create();
    Keyboard.Add_Flag(Flag_Type::Hidden);
}

Shell_Class::Login_Class::~Login_Class()
{
    Dialog.Delete();
    Keyboard.Delete();
}

void Shell_Class::Login_Class::Open(Shell_Class *Shell_Pointer)
{
    Shell_Pointer->Login_Pointer = new Login_Class(Shell_Pointer);
}

void Shell_Class::Login_Class::Close(Shell_Class *Shell_Pointer)
{
    delete Shell_Pointer->Login_Pointer;
    Shell_Pointer->Login_Pointer = NULL;
}

bool Shell_Class::Login_Class::Is_Openned(Shell_Class *Shell_Pointer)
{
    if (Shell_Pointer->Login_Pointer != NULL)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Shell_Class::Login_Class::Execute_Instruction(Instruction_Type Instruction)
{
    if (Instruction.Get_Sender() == &Graphics)
    {
        switch (Instruction.Graphics.Get_Code())
        {
        case Graphics_Type::Focused:
            if (Instruction.Graphics.Get_Object() == Name_Input)
            {
                Keyboard.Clear_Flag(Flag_Type::Hidden);
                Keyboard.Set_Text_Area(Name_Input);
            }
            else if (Instruction.Graphics.Get_Object() == Password_Input)
            {
                Keyboard.Clear_Flag(Flag_Type::Hidden);
                Keyboard.Set_Text_Area(Password_Input);
            }
            break;
        case Graphics_Type::Defocused:
            if ((Instruction.Graphics.Get_Object() == Name_Input) || (Instruction.Graphics.Get_Object() == Password_Input))
            {
                Keyboard.Remove_Text_Area();
                Keyboard.Add_Flag(Flag_Type::Hidden);
            }
            break;
        case Graphics_Type::Clicked:
            if (Instruction.Graphics.Get_Object() == Login_Button)
            {
                if (Account.Login(Name_Input.Get_Text(), Password_Input.Get_Text()) == Result_Type::Success)
                {
                    Shell_Class::Desk_Class::Open(Shell_Pointer);
                    Close(Shell_Pointer);
                }
                else
                {
                    Dialog.Set_Title("Login failed");
                }
            }
        }
    }
}