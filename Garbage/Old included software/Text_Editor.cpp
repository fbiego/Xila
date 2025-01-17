#include "Software/Text_Editor.hpp"

Text_Editor_Class *Text_Editor_Class::Instance_Pointer = NULL;

Text_Editor_Class::Text_Editor_Class() : Xila_Class::Software(Text_Editor_Handle)
{
    Xila.Task.Create(Main_Task, "Text Editor Task", Memory_Chunk(3), NULL, &Task_Handle);
    Send_Instruction(Xila.Open);
}

Text_Editor_Class::~Text_Editor_Class()
{
    if (Instance_Pointer != this)
    {
        delete Instance_Pointer;
    }
    Instance_Pointer = NULL;
}

Xila_Class::Software *Text_Editor_Class::Load()
{
    if (Instance_Pointer != NULL)
    {
        delete Instance_Pointer;
    }
    Instance_Pointer = new Text_Editor_Class();
    return Instance_Pointer;
}

void Text_Editor_Class::Set_Variable(Xila_Class::Address Address, uint8_t Type, const void *Variable)
{

    if (Address == Address('O', 'f') && Type == Xila.Display.Variable_Long)
    {
        Offset = *(uint8_t *)Variable;
        Offset = (Offset * File_To_Edit.size()) / 176;
        Send_Instruction('R', 'T');
    }
}

Xila_Class::Task_Function Text_Editor_Class::Main_Task(void *pvParameters)
{
    while (1)
    {
        switch (Instance_Pointer->Get_Instruction())
        {
        case Xila.Idle:
            // IDLE : nothing to do
            if (Xila.Software_Management.Get_State(Text_Editor_Handle) == Xila.Minimized)
            {
                Xila.Task.Delay(90);
            }
            Xila.Task.Delay(20);
            break;

        case Xila.Close:
            delete Instance_Pointer;
            vTaskDelete(NULL);
            break;
        case Xila.Maximize:
            Xila.Display.Set_Current_Page(F("Text_Editor"));
            Instance_Pointer->Refresh_Interface();
            break;
        case Xila.Open:
            Xila.Display.Set_Current_Page(F("Text_Editor"));
            break;
        case Xila.Minimize:
            break;
        case Xila.Hibernate:
        case Xila.Shutdown:
        case Xila.Restart:
        case Instruction('C', 'l'):
            Xila.Software_Management.Close(Text_Editor_Handle);
            break;
        case Instruction('M', 'i'):
            Xila.Software_Management.Minimize(Text_Editor_Handle);
            break;
        case Instruction('R', 'T'): // RT : refresh text
            Instance_Pointer->Refresh_Interface();
            break;
        case Instruction('O', 'F'): // OF : open file
            Instance_Pointer->File_To_Edit.close();
            if (Xila.Dialog.Open_File(Instance_Pointer->File_To_Edit) == Xila.Default_Yes)
            {
                Instance_Pointer->Scan();
                Instance_Pointer->Refresh_Interface();
            }
            break;

        case Instruction('S', 'F'): // SF : Scan file
            Instance_Pointer->Scan();
            break;
        default:
            break;
        }
    }
}

void Text_Editor_Class::Scan()
{
    if (!File_To_Edit)
    {
        Xila.Dialog.Event(F("Failed to read file."), Xila.Error);
        return;
    }

    Offset = 0;
    if (File_To_Edit.find(0x0D))
    {
        if (File_To_Edit.read() == 0x0A)
        {
            Mode = Windows;
        }
        else
        {
            Mode = MacOS;
        }
    }
    else if (File_To_Edit.find(0x0A))
    {
        Mode = Unix;
    }
    else
    {
        Mode = Undefined;
    }
}

void Text_Editor_Class::Refresh_Interface()
{
    char Line_Name[11] = "LINE _TXT";
    uint8_t Line_Number = 0;

    if (!File_To_Edit)
    {
        Xila.Display.Set_Value(F("SCROLLBAR_SLI"), 0);
        Xila.Display.Set_Text(F("FILENAME_TXT"), "");
        for (; Line_Number < 14; Line_Number++)
        {
            Line_Name[4] = '0' + Line_Number;
            Xila.Display.Set_Text(Line_Name, "");
        }
        return;
    }

    File_To_Edit.seek(Offset);

    char Temporary_Character;

    char Temporary_Char_Array[67]; // double to allow

    memset(Temporary_Char_Array, '\0', sizeof(Temporary_Char_Array));

    Xila.Drive.Get_Name(File_To_Edit, Temporary_Char_Array, sizeof(Temporary_Char_Array));
    Xila.Display.Set_Text(F("FILENAME_TXT"), Temporary_Char_Array);

    memset(Temporary_Char_Array, '\0', sizeof(Temporary_Char_Array));

    uint8_t Column_Number = 0;
    char Line_Ending = 0x0D;

    switch (Mode)
    {
    case MacOS:
    case Windows:
        Line_Ending = 0x0D;
        break;
    case Unix:
        Line_Ending = 0x0A;
        break;
    }

    while (File_To_Edit.available())
    {
        Temporary_Character = File_To_Edit.read();

        if (Line_Number > 14)
        {
            break;
        }

        if (Temporary_Character == Line_Ending)
        {

            if (Mode == Windows) // skip 0x0A char
            {
                File_To_Edit.read();
            }
            Xila.Task.Delay(5);

            Line_Name[4] = 'A' + Line_Number;
            Xila.Display.Set_Text(Line_Name, Temporary_Char_Array);
            memset(Temporary_Char_Array, '\0', sizeof(Temporary_Char_Array));
            Line_Number++;
            Column_Number = 0;
        }
        else if (Column_Number >= 67)
        {
            Xila.Task.Delay(5);
            Line_Name[4] = 'A' + Line_Number;
            Xila.Display.Set_Text(Line_Name, Temporary_Char_Array);
            memset(Temporary_Char_Array, '\0', sizeof(Temporary_Char_Array));
            Line_Number++;
            Column_Number = 0;
        }
        else
        {
            if (isPrintable(Temporary_Character))
            {
                Temporary_Char_Array[Column_Number++] = Temporary_Character;
            }
        }
    }
    for (; Line_Number <= 14; Line_Number++)
    {
        Line_Name[4] = 'A' + Line_Number;
        Xila.Display.Set_Text(Line_Name, "");
    }
}
