#include "Software/Piano.hpp"

Piano_Class *Piano_Class::Instance_Pointer = NULL;

Piano_Class::Piano_Class() : Xila_Class::Software(Piano_Handle),
                             Offset(0),
                             Duration(200),
                             MIDI_Output(false)
{
    Xila.Task.Create(Main_Task, "Piano Task", Memory_Chunk(2), NULL, &Task_Handle);
}

Piano_Class::~Piano_Class()
{
    if (Instance_Pointer != this)
    {
        delete Instance_Pointer;
    }
    Instance_Pointer = NULL;
}

Xila_Class::Software *Piano_Class::Load()
{
    if (Instance_Pointer != NULL)
    {
        delete Instance_Pointer;
    }
    Instance_Pointer = new Piano_Class();
    return Instance_Pointer;
}

Xila_Class::Task_Function Piano_Class::Main_Task(void *pvParameters)
{
    (void)pvParameters;
    while (1)
    {
        switch (Instance_Pointer->Get_Instruction())
        {
        case Xila.Idle: //idle state
            if (Xila.Software_Management.Get_State(Piano_Handle) == Xila.Minimized)
            {
                Xila.Task.Delay(90);
            }
            Xila.Task.Delay(10);
            break;
        case Xila.Open:
            Xila.Display.Set_Current_Page(F("Piano"));
            break;
        case Xila.Maximize:
            Xila.Display.Set_Current_Page(F("Piano"));
            Instance_Pointer->Refresh_Interface();
            break;
        case Xila.Minimize:
            break;

        case Xila.Close:
            delete Instance_Pointer;
            Xila.Task.Delete();
            break;
        		case Xila.Hibernate:
		case Xila.Shutdown:
		case Xila.Restart:
        case Instruction('C', 'l'):
            Xila.Software_Management.Close(Piano_Handle);
            break;
        case Instruction('M', 'i'):
            Xila.Software_Management.Minimize(Piano_Handle);
            break;
        case Instruction('C', ' '): //C
            Instance_Pointer->Press_Key(0);
            break;
        case 0x4323: //C#
            Instance_Pointer->Press_Key(1);
            break;
        case 0x4420: //D
            Instance_Pointer->Press_Key(2);
            break;
        case 0x4423: //D#
            Instance_Pointer->Press_Key(3);
            break;
        case 0x4520: //E
            Instance_Pointer->Press_Key(4);
            break;

        case 0x4620: //F
            Instance_Pointer->Press_Key(5);
            break;

        case 0x4623: //F#
            Instance_Pointer->Press_Key(6);
            break;

        case 0x4720: //G
            Instance_Pointer->Press_Key(7);
            break;

        case 0x4723: //G#
            Instance_Pointer->Press_Key(8);
            break;

        case 0x4120: //A
            Instance_Pointer->Press_Key(9);
            break;

        case 0x4123: //A#
            Instance_Pointer->Press_Key(10);
            break;

        case 0x4220: //B
            Instance_Pointer->Press_Key(11);
            break;

        case 0x6320: //c
            Instance_Pointer->Press_Key(12);
            break;

        case 0x6323: //c#
            Instance_Pointer->Press_Key(13);
            break;

        case 0x6420: //d
            Instance_Pointer->Press_Key(14);
            break;

        case 0x6423: //d#
            Instance_Pointer->Press_Key(15);
            break;

        case 0x6520: //e
            Instance_Pointer->Press_Key(16);
            break;

        case 0x6620: //f
            Instance_Pointer->Press_Key(17);
            break;

        case 0x6623: //f#
            Instance_Pointer->Press_Key(18);
            break;

        case 0x6720: //g
            Instance_Pointer->Press_Key(19);
            break;

        case 0x6723: //g#
            Instance_Pointer->Press_Key(20);
            break;

        case 0x6120: //a
            Instance_Pointer->Press_Key(21);
            break;

        case 0x6123: //a#
            Instance_Pointer->Press_Key(22);
            break;

        case 0x6220: //b
            Instance_Pointer->Press_Key(23);
            break;
        case Instruction('R', 'e'):
            Instance_Pointer->Release_Key();
            break;
        default:
            break;
        }
        Xila.Task.Delay(10);
    }
}

void Piano_Class::Refresh_Interface()
{
    if (Note_ID == 0xFF)
    {
        Xila.Display.Set_Text(F("FREQVAL_TXT"), "");
    }
    else
    {
        snprintf(Temporary_String, sizeof(Temporary_String), "%i Hz", (Note_Frequency[Note_ID] + Offset));
        Xila.Display.Set_Text(F("FREQVAL_TXT"), Temporary_String);
    }
}

void Piano_Class::Load_Registry()
{
    File Temporary_File = Xila.Drive.Open(Piano_File("Registry"));
    DynamicJsonDocument Piano_Registry(256);
    if (deserializeJson(Piano_Registry, Temporary_File) != DeserializationError::Ok)
    {
        Temporary_File.close();
        Save_Registry();
        return;
    }
    Temporary_File.close();
    Offset = Piano_Registry["Offset"] | 0;
}

void Piano_Class::Save_Registry()
{
    DynamicJsonDocument Piano_Registry(256);
    Piano_Registry["Offset"] = Offset;
    File Temporary_File = Xila.Drive.Open(Piano_File("Registry"), FILE_WRITE);
    if (serializeJson(Piano_Registry, Temporary_File) == 0)
    {
        Temporary_File.close();
        Xila.Dialog.Event(F("Failed to save registry."), Xila.Error);
    }
    Temporary_File.close();
}

void Piano_Class::Press_Key(uint8_t Note)
{
    Note_ID = Note;
    Current_Note = Note_Frequency[Note_ID] + Offset;
    Xila.Sound.Tone(Current_Note);
    Refresh_Interface();
}
void Piano_Class::Release_Key()
{
    Note_ID = 0xFF;
    Current_Note = 0xFFFF;
    Xila.Sound.No_Tone();
    Refresh_Interface();
}