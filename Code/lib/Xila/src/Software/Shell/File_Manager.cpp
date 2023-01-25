///
/// @file File_Manager.cpp
/// @author Alix ANNERAUD (alix.anneraud@outlook.fr)
/// @brief File manager of Shell
/// @version 0.2
/// @date 29-06-2021
///
/// @copyright Copyright (c) 2021
///

#include "Software/Shell/Shell.hpp"
#include "Software/Shell/Translation.hpp"

Shell_Class::File_Manager_Class::File_Manager_Class(Shell_Class* Shell_Pointer)
: Shell_Pointer(Shell_Pointer)
{
    Offset = 0;
    Operation = Browse;
    Selected_Item = Drive.Open(Xila_Directory_Path);
    Selected_Item.close();
    Operation_Item = Drive.Open(Xila_Directory_Path);
    Operation_Item.close();
}

Shell_Class::File_Manager_Class::~File_Manager_Class()
{
    Selected_Item.close();
    Operation_Item.close();
}

// -- File manager state -- //

void Shell_Class::File_Manager_Class::Open(uint8_t Mode)
{
    DUMP("open file manager");
    DUMP(Mode);
    if (!State())
    {
        FILE_MANAGER = new File_Manager_Class();
    }

    FILE_MANAGER->Mode = Mode;
    Display.Set_Current_Page(SHELL->Pages.File_Manager_Main);
    memset(FILE_MANAGER->Current_Path, '\0', sizeof(FILE_MANAGER->Current_Path));
    strlcpy(FILE_MANAGER->Current_Path, "/", sizeof(FILE_MANAGER->Current_Path));

    FILE_MANAGER->Refresh();
}

void Shell_Class::File_Manager_Class::Close()
{
    DUMP("close file manager");
    if (State())
    {
        delete FILE_MANAGER;
        FILE_MANAGER = NULL;
    }
}

void Shell_Class::File_Manager_Class::Execute_Instruction(Xila_Class::Instruction Instruction)
{
    switch (Instruction)
    {
    case Instruction('R', 'e'): // -- Refresh file manager
        Refresh();
        break;
    case Instruction('C', 'l'):
        if (Get_Mode() == Idle)    // -- If open in default mode,
        {
            SHELL->Send_Instruction('O', 'D');  // -- Open back the desk.
        }
        else    // -- If open in dialog mode,
        {
            Dialog.File_Manager_State = Default_Cancel;   // -- Set dialog state.
        }
        Close();    // -- Close file manager.
        break;
    case Instruction('C', 'o'): // copy file
        Copy_Item();
        break;
    case Instruction('C', 'u'):
        Cut_Item();
        break;
    case Instruction('D', 'e'):
        Set_Operation(Browse);
        Refresh();
        Set_Operation(Delete);

        break;
    case Instruction('R', 'E'):
        Set_Operation(Browse);
        Refresh();
        Set_Operation(Rename);

        break;
    case Instruction('N', 'P'):
        Offset += 29;
        Refresh();
        break;
    case Instruction('P', 'P'):
        Offset -= 29;
        Refresh();
        break;
    case Instruction('V', 'a'):
        Validate();
        break;
    case Instruction('R', 'D'): // open root directory
        Operation = Browse;
        memset(Current_Path, '\0', sizeof(Current_Path));
        Current_Path[0] = '/';
        Refresh();
        break;
    case Instruction('H', 'D'): // open home directory
        Operation = Browse;
        memset(Current_Path, '\0', sizeof(Current_Path));
        strlcpy(Current_Path, Users_Directory_Path "/", sizeof(Current_Path));
        strlcat(Current_Path, Account.Current_Username, sizeof(Current_Path));
        Refresh();
        break;
    case Instruction('N', 'f'): // -- Create a new file -- //
        Create_File();
        break;
    case Instruction('N', 'F'): // -- Create a new directory --//
        Create_Folder();
        break;
    case Instruction('K', 'P'): // -- Open keyboard to input the path -- //
        DIALOG.Keyboard(Current_Path, sizeof(Current_Path));
        Refresh();
        break;
    case Instruction('B', 'a'):
        Operation = Browse;
        Display.Set_Current_Page(Shell_Class::Pages.File_Manager_Main);
        Refresh();
        break;
    case Instruction('G', 'D'): // -- Get details on selected item
        Set_Operation(Browse);
        Refresh();
        Set_Operation(Detail);
        break;
    case Instruction('G', 'P'):
        Set_Operation(Browse);
        Go_Parent();
        SHELL->Send_Instruction('R', 'e');
        break;

    default:
        SHELL->Execute_Instruction(Instruction);
        break;
    }
}

void Shell_Class::File_Manager_Class::Validate()
{
    switch (Get_Mode())
    {
    case Dialog_Open_File:
        if (!Selected_Item.isDirectory())
        {
            Item_Pointer = &Selected_Item;
            Dialog.File_Manager_State = Default_Yes;
        }
        break;

    case Dialog_Open_Folder:
        if (Selected_Item.isDirectory())
        {
            Item_Pointer = &Selected_Item;
            Dialog.File_Manager_State = Default_Yes;
        }
        break;
    case Dialog_Save_File:
        if (!Selected_Item.isDirectory())
        {
            Item_Pointer = &Selected_Item;
            Dialog.File_Manager_State = Default_Yes;
        }
        break;
    default:
        break;
    }
}

void Shell_Class::File_Manager_Class::Cut_Item()
{
    if (Get_Operation() == Paste_Cut) //paste
    {
        if (Selected_Item.isDirectory())
        {
            DIALOG.Event(F("Cannot cut folder."), Error);
            SHELL->Send_Instruction('R', 'e');
        }
        else
        {
            char Temporary_File_Name[13] = "/";
            strlcat(Current_Path, Temporary_File_Name, sizeof(Current_Path));
            File Destination_File = Drive.Open(Current_Path, FILE_WRITE);
            SHELL->Set_Watchdog_Timeout(30000);
            if (Drive.Copy(Operation_Item, Destination_File) == Success)
            {
                DIALOG.Event(F("File was cut successfully."), Error);
            }
            else
            {
                DIALOG.Event(F("Failed to cut file."), Error);
            }
            SHELL->Set_Watchdog_Timeout();
            Destination_File.close();
            Operation_Item.close();
            SHELL->Send_Instruction('G', 'P');
        }
        Display.Set_Picture(F("CUT_BUT"), Images.Cut_24);
        Set_Operation(Browse);
    }
    else
    {
        Set_Operation(Browse);
        Refresh();
        Set_Operation(Cut);
    }
}

void Shell_Class::File_Manager_Class::Copy_Item()
{
    if (Get_Operation() == Paste_Copy) //paste
    {
        if (Operation_Item.isDirectory())
        {
            DIALOG.Event(F("Cannot copy folder."), Error);
            SHELL->Send_Instruction('R', 'e');
        }
        else
        {
            char Temporary_File_Name[13] = "/";
            strlcat(Current_Path, Temporary_File_Name, sizeof(Current_Path));
            File Destination_File = Drive.Open(Current_Path, FILE_WRITE);
            SHELL->Set_Watchdog_Timeout(30000);

            if (Drive.Copy(Operation_Item, Destination_File) == Success)
            {
                DIALOG.Event(F("File was copied successfully."), Information);
            }
            else
            {
                DIALOG.Event(F("Failed to copy file."), Error);
            }
            SHELL->Set_Watchdog_Timeout();
            Operation_Item.close();
            SHELL->Send_Instruction('G', 'P');
        }
        Operation_Item.close();
        Display.Set_Picture(F("COPY_BUT"), Shell_Class::Images.Copy_24);
        Set_Operation(Browse);
    }
    else
    {
        Set_Operation(Browse);
        Refresh();
        Set_Operation(Copy);
    }
}

void Shell_Class::File_Manager_Class::Create_File()
{
    strlcpy(Current_Item_Name, "NEWFILE .TXT", sizeof(Current_Item_Name));
    for (uint8_t i = 0; i <= 10; i++)
    {
        if (i < 10)
        {
            Current_Item_Name[7] = i + '0';
            if (!Drive.Exists(String(Current_Path) + String("/") + String(Current_Item_Name)))
            {
                Drive.Open(String(Current_Path) + String("/") + String(Current_Item_Name), FILE_WRITE).close();
                break;
            }
        }
        else
        {
            DIALOG.Event(F("Failed to create file."), Result_Type::Error);
            break;
        }
    }
    Refresh();
}

void Shell_Class::File_Manager_Class::Create_Folder()
{
    strlcpy(Current_Item_Name, "NEWDIRE ", sizeof(Current_Item_Name));
    for (uint8_t i = 0; i <= 10; i++)
    {
        if (i < 10)
        {
            Current_Item_Name[7] = i + '0';
            if (!Drive.Exists(String(Current_Path) + "/" + String(Current_Item_Name)))
            {
                if (!Drive.Make_Directory(String(Current_Path) + "/" + String(Current_Item_Name)))
                {
                    DIALOG.Event(F("Failed to create folder."), Result_Type::Error);
                }
                break;
            }
        }
        else
        {
            DIALOG.Event(F("Failed to create folder."), Result_Type::Error);
            break;
        }
    }
    Refresh();
}

void Shell_Class::File_Manager_Class::Refresh()
{
    File Temporary_Item = Drive.Open(Current_Path);

    if (Temporary_Item)
    {
        File Item;
        switch (Operation)
        {
        case Copy:
            Operation_Item = Temporary_Item;
            Operation = Paste_Copy;
            Display.Set_Picture(F("COPY_BUT"), Images.Paste_24);
            SHELL->Send_Instruction('G', 'P');
            break;
        case Cut:
            Operation_Item = Temporary_Item;
            Operation = Paste_Cut;
            Display.Set_Picture(F("CUT_BUT"), Images.Paste_24);
            SHELL->Send_Instruction('G', 'P');
            break;
        case Delete:
            if (DIALOG.Event(F("Are you sure to delete this item."), Question) == Button_1)
            {
                if (Temporary_Item.isDirectory())
                {
                    if (Drive.Remove_Directory(Current_Path) != true)
                    {
                        DIALOG.Event(F("Failed to delete directory."), Error);
                    }
                }
                else
                {
                    if (Drive.Remove(Current_Path) != true)
                    {
                        DIALOG.Event(F("Failed to delete file."), Error);
                    }
                }
            }
            Go_Parent();
            Operation = Browse;
            SHELL->Send_Instruction('R', 'e');
            break;
        case Rename:
            if (Get_Mode() == Idle)
            {
                Go_Parent();
                char Temporary_Input[14];
                memset(Temporary_Input, '\0', sizeof(Temporary_Input));
                Drive.Get_Name(Temporary_Item, Temporary_Input, sizeof(Temporary_Input));
                DIALOG.Keyboard(Temporary_Input, sizeof(Temporary_Input));
                Drive.Rename(Temporary_Item.name(), String(Current_Path) + String(Temporary_Input));
                Operation = Browse;
            }
            Operation = Browse;
            SHELL->Send_Instruction('R', 'e');

            break;
        case Detail:
            Open_Detail();
            Go_Parent();
            break;
        case Paste_Cut:
        case Paste_Copy:
        case Browse:
        default:
            if (Temporary_Item.isDirectory())
            {
                Display.Set_Text(F("PATH_TXT"), Current_Path);
                Temporary_Item.rewindDirectory();

                // -- Apply offset
                for (uint8_t i = 0; i < Offset; i++)
                {
                    Temporary_Item.openNextFile().close();
                }

                char Temporary_Item_Name[13] = "ITEM _TXT";
                char Temporary_Item_Picture[13] = "ITEM _PIC";
                char Temporary_File_Name[13] = "";

                for (uint8_t i = 1; i <= 30; i++)
                {

                    Item = Temporary_Item.openNextFile();

                    // -- Check if offset is correct, if not, reset offset and rewind directory
                    if (i == 1)
                    {
                        if (!Item)
                        {
                            Offset = 0;
                            Item.close();
                            Temporary_Item.rewindDirectory();
                            Item = Temporary_Item.openNextFile();
                        }
                    }

                    if (i < 10)
                    {

                        Temporary_Item_Name[4] = i + '0';
                        Temporary_Item_Picture[4] = i + '0';
                    }
                    else if (i < 20)
                    {
                        if (i == 10)
                        {
                            strlcpy(Temporary_Item_Name, "ITEM  _TXT", sizeof(Temporary_Item_Name));
                            strlcpy(Temporary_Item_Picture, "ITEM  _PIC", sizeof(Temporary_Item_Picture));
                        }

                        Temporary_Item_Name[4] = '1';
                        Temporary_Item_Name[5] = (i - 10) + '0';

                        Temporary_Item_Picture[4] = '1';
                        Temporary_Item_Picture[5] = Temporary_Item_Name[5];
                    }
                    else if (i < 30)
                    {
                        Temporary_Item_Name[4] = '2';
                        Temporary_Item_Name[5] = (i - 20) + '0';

                        Temporary_Item_Picture[4] = '2';
                        Temporary_Item_Picture[5] = Temporary_Item_Name[5];
                    }
                    else
                    {
                        Temporary_Item_Name[4] = '3';
                        Temporary_Item_Name[5] = '0';

                        Temporary_Item_Picture[4] = '3';
                        Temporary_Item_Picture[5] = '0';
                    }

                    if (Item)
                    {
                        Drive.Get_Name(Item, Temporary_File_Name, sizeof(Temporary_File_Name));

                        Display.Set_Text(Temporary_Item_Name, Temporary_File_Name);

                        if (Item.isDirectory())
                        {
                            Display.Set_Picture(Temporary_Item_Picture, Images.Folder_16);
                        }
                        else
                        {
                            Display.Set_Picture(Temporary_Item_Picture, Images.File_16);
                        }
                    }
                    else
                    {
                        Display.Set_Text(Temporary_Item_Name, "");

                        Display.Set_Picture(Temporary_Item_Picture, Images.Empty_16);
                    }

                    Item.close();
                    Task_Type::Delay_Static(5);
                }

                if (Get_Mode() == Dialog_Open_Folder)
                {
                    Selected_Item = Temporary_Item;
                }
                else
                {
                    Temporary_Item.close();
                }
            }
            else // if it is a file
            {

                if (Get_Mode() == Dialog_Save_File)
                {
                    Selected_Item = Temporary_Item;
                }
                else if (Get_Mode() == Dialog_Open_File)
                {
                    Selected_Item = Temporary_Item;
                }
                else
                {
                    Temporary_Item.close();
                }
                SHELL->Send_Instruction('G', 'P');
            }
        }
    }
    else
    {
        DIALOG.Event(F("Failed to open path."), Error);
        SHELL->Send_Instruction('R', 'D');
    }
    Refresh_Footerbar();
}

void Shell_Class::File_Manager_Class::Refresh_Footerbar()
{

    switch (Get_Mode())
    {
    case Idle:
        Display.Click(F("FOOTERBAR_HOT"), 0);
        return;
        break;
    case Dialog_Save_File:
        Display.Click(F("FOOTERBAR_HOT"), 1);
        Display.Set_Text(F("VALIDATE_BUT"), F("Save"));
        break;
    case Dialog_Open_Folder:
    case Dialog_Open_File:
        Display.Click(F("FOOTERBAR_HOT"), 1);
        Display.Set_Text(F("VALIDATE_BUT"), F("Open"));
        break;
    default:
        break;
    }

    if (Selected_Item)
    {
        char Temporary_File_Name[13];
        Drive.Get_Name(Selected_Item, Temporary_File_Name, sizeof(Temporary_File_Name));
        Display.Set_Text(F("FILENAME_TXT"), Temporary_File_Name);
    }
    else
    {
        Display.Set_Text(F("FILENAME_TXT"), F(""));
    }
}

void Shell_Class::File_Manager_Class::Go_Parent()
{
    for (uint8_t i = sizeof(Current_Path) - 1; i >= 0; i--)
    {

        if (Current_Path[i] == '/')
        {
            if (i != 0)
            {
                Current_Path[i] = '\0';
            }
            return;
        }
        else
        {
            Current_Path[i] = '\0';
        }
    }
}

// -- File Manager Detail -- //

void Shell_Class::File_Manager_Class::Open_Detail()
{
    Operation = Browse;
    Display.Set_Current_Page(Pages.File_Manager_Detail);
    Refresh_Detail();
}

void Shell_Class::File_Manager_Class::Refresh_Detail()
{
    File Temporary_Item = Drive.Open(Current_Path);

    if (!Temporary_Item)
    {
            DIALOG.Event(F("Failed to get file informations."), Error);
        Open(Mode);
    }

    Display.Set_Text(F("PATHVAL_TXT"), Current_Path);

    char Temporary_Char_Array[25];

    Drive.Get_Name(Temporary_Item, Temporary_Char_Array, sizeof(Temporary_Char_Array));

    Display.Set_Text(F("NAMEVAL_TXT"), Temporary_Char_Array);

    if (Temporary_Item.isDirectory())
    {
        Display.Set_Text(F("TYPEVAL_TXT"), F("Directory"));
    }
    else
    {
        Display.Set_Text(F("TYPEVAL_TXT"), F("File"));
    }

    sprintf(Temporary_Char_Array, "%di Bytes\n", Temporary_Item.size());
    dtostrf(Temporary_Item.size(), (sizeof(Temporary_Char_Array) - 6), 0, Temporary_Char_Array);

    Display.Set_Text(F("SIZEVAL_TXT"), Temporary_Char_Array);

    time_t File_Last_Write_Time = Temporary_Item.getLastWrite();
    tm *File_Last_Write = localtime(&File_Last_Write_Time);
    sprintf(Temporary_Char_Array, "%02d/%02d/%d %02d:%02d:%02d\n", File_Last_Write->tm_mday, (File_Last_Write->tm_mon) + 1, (File_Last_Write->tm_year) + 1900, File_Last_Write->tm_hour, File_Last_Write->tm_min, File_Last_Write->tm_sec);

    Display.Set_Text(F("LASTWVAL_TXT"), Temporary_Char_Array);
}