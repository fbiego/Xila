///
/// @file Shell.hpp
/// @author Alix ANNERAUD (alix.anneraud@outlook.fr)
/// @brief Xila's user interface header file.
/// @version 0.1.0
/// @date 21-05-2020
/// @details Xila user interface software.
///
/// @section License
///
/// Copyright (c) 2020 Alix ANNERAUD
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
/// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
///
/// @copyright Copyright (c) 2021
///

#ifndef Shell_Hpp_Included
#define Shell_Hpp_Included

#include "Xila.hpp"

using namespace Xila;

/// @brief Shell class
///
/// @details Main inter
class Shell_Class : public Software_Type
{
public:
    // - Types

    static Software_Handle_Type Handle;

private:
    static void Create_Instance()
    {
        new Shell_Class();
    }

    // -- Methods

    Shell_Class();
    ~Shell_Class();

    // - Attributes

    // - Types

    /// @brief Desk class
    class Desk_Class
    {
    public:
        // - Methods
        void Execute_Instruction(const Instruction_Type &Instruction);
        void Refresh_Overlay();

        // - -  Getters

        Color_Type Get_Background_Color() const;
        Color_Type Get_Foreground_Color() const;

        // - - Setters

        void Set_Background_Color(Color_Type Color);
        void Set_Foreground_Color(Color_Type Color);

        // - - Constructors / destructor
        Desk_Class(Shell_Class *Shell_Pointer);
        ~Desk_Class();

    private:
        // - Methods

        void Refresh();

        // - Attributes
        // - - Overlay
        Object_Type Overlay;
        Button_Type Battery_Button;
        Image_Type Battery_Image;
        Button_Type WiFi_Button;
        Image_Type WiFi_Image;
        Button_Type Sound_Button;
        Image_Type Sound_Image;
        Label_Type Clock_Label;

        // - - Parent window
        Window_Type Window;
        Object_Type Desk_Grid;
        Object_Type Dock;
        Object_Type Menu_Button;
        Object_Type Dock_List;

        Color_Type Background_Color;
        Color_Type Foreground_Color;

        // - - Shell pointer
        Shell_Class *Shell_Pointer;
    } Desk;

    class Drawer_Class
    {
    public:
        static void Open(Shell_Class *Shell_Pointer);
        static void Close(Shell_Class *Shell_Pointer);
        static bool Is_Openned(Shell_Class *Shell_Pointer);

        void Execute_Instruction(Instruction_Type Instruction);

    private:
        // - Attributes
        Window_Type Window;

        Drawer_Class(Shell_Class *Shell_Pointer);
        ~Drawer_Class();

        static const Instruction_Type Click_On_Item;

        Shell_Class *Shell_Pointer;

        friend class Shell_Class;

    } *Drawer_Pointer;

    class Login_Class
    {
    public:
        static void Open(Shell_Class *Shell_Pointer);
        static bool Is_Openned(Shell_Class *Shell_Pointer);
        static void Close(Shell_Class *Shell_Pointer);

    private:
        Login_Class(Shell_Class *Shell_Pointer);
        ~Login_Class();

        void Execute_Instruction(Instruction_Type Instruction);

        Dialog_Type Dialog;

        Text_Area_Type Name_Input;
        Text_Area_Type Password_Input;
        Button_Type Login_Button;
        Label_Type Login_Label;
        Keyboard_Type Keyboard;

        Shell_Class *Shell_Pointer;
    } *Login_Pointer;

    ///
    ///
    class File_Manager_Class
    {
    public:
        static void Open(Shell_Class *Shell_Pointer);
        static bool Is_Openned(Shell_Class *Shell_Pointer);
        static void Close(Shell_Class *Shell_Pointer);
        void Execute_Instruction(Instruction_Type Instruction);

    private:
        // -- Attributes
        char Buffer[256];

        bool Cut;

        // -- Methods

        File_Manager_Class(Shell_Class *Shell_Pointer);
        ~File_Manager_Class();

        void Refresh();

        void Enable_Selection_Mode(bool Multiple);
        void Disable_Selection_Mode();
        uint8_t Count_Selected_Items();


        void Paste();
        void Details();
        void Go_Parent_Folder();
        void Click();
        void Rename();
        void Delete();
        void Create_File();
        void Create_Folder();

        Window_Type Window;

        Object_Type Grid;
        Object_Type Flexbox;
        Text_Area_Type Path_Text_Area;
        List_Type List;
        Button_Type Parent_Folder_Button;
        Button_Type Home_Folder_Button;
        Button_Type Root_Folder_Button;
        Button_Type Refresh_Button;
        Button_Type New_Folder_Button;
        Button_Type New_File_Button;
        Button_Type Select_Button;
        Button_Type Deselect_Button;
        Button_Type Delete_Button;
        Button_Type Rename_Button;
        Button_Type Copy_Button;
        Button_Type Cut_Button;
        Button_Type Paste_Button;
        Button_Type Details_Button;

        Dialog_Type Details_Dialog;
        Table_Type Details_Table;

        Shell_Class *Shell_Pointer;

    } *File_Manager_Pointer;

    /// @brief Preferences class
    class Preferences_Class
    {
    public:
        // - Types

        enum class Event_Code_Type : uint16_t
        {
            Refresh_Personal,
            Refresh_Softwares,
            Refresh_Wireless,
            Refresh_Users,
            Refresh_Hardware,
            Refresh_System
        };

        // - Methods

        static void Open(Shell_Class *Shell_Pointer);
        static bool Is_Openned(Shell_Class *Shell_Pointer);
        static void Close(Shell_Class *Shell_Pointer);
        void Execute_Instruction(Instruction_Type Instruction);

    private:
        // - Methods

        // - - Constructor

        Preferences_Class(Shell_Class *Shell_Pointer);
        ~Preferences_Class();

        void Draw_Personal();
        void Draw_Softwares();
        void Draw_Wireless();
        void Draw_Users();
        void Draw_Hardware();
        void Draw_System();

        void Refresh_Personal();
        void Refresh_Softwares();
        void Refresh_Wireless();
        void Refresh_Users();
        void Refresh_Hardware();
        void Refresh_System();

        void Execute_Personal_Instruction(const Instruction_Type &Instruction);
        void Execute_Softwares_Instruction(const Instruction_Type &Instruction);
        void Execute_Wireless_Instruction(const Instruction_Type &Instruction);
        void Execute_Users_Instruction(const Instruction_Type &Instruction);
        void Execute_Hardware_Instruction(const Instruction_Type &Instruction);
        void Execute_System_Instruction(const Instruction_Type &Instruction);

        // - Attributes

        Window_Type Window;
        Tabs_Type Tabs;
        Object_Type Personnal_Tab, Softwares_Tab, Hardware_Tab, Network_Tab, Users_Tab, System_Tab;

        Keyboard_Type Keyboard;

        // - - Personnal

        Button_Type Personnal_Style_Apply_Button;
        Button_Type Personnal_Style_Background_Button;
        Button_Type Personnal_Style_Foreground_Button;

        // - - Wireless

        Switch_Type Wireless_WiFi_Switch;
        Button_Type Wireless_WiFi_Refresh_Button;
        Button_Type Wireless_WiFi_Informations_Button;
        Button_Type Wireless_WiFi_Connect_Button;
        Roller_Type Wireless_WiFi_Access_Point_Roller;
        Text_Area_Type Wireless_WiFi_Password_Text_Area;

        Button_Type Wireless_Network_Apply_Button;
        Checkbox_Type Wireless_Network_DHCP_Checkbox;
        Text_Area_Type Wireless_Network_Local_IP_Text_Area;
        Text_Area_Type Wireless_Network_Gateway_IP_Text_Area;
        Text_Area_Type Wireless_Network_Subnet_Mask_Text_Area;
        Text_Area_Type Wireless_Network_DNS_1_Text_Area;
        Text_Area_Type Wireless_Network_DNS_2_Text_Area;

        // - - Hardware

        Button_Type Hardware_Display_Calibrate_Button;
        Slider_Type Hardware_Display_Brightness_Slider;
        Slider_Type Hardware_Sound_Volume_Slider;
        Label_Type Hardware_Battery_Level_Label;
        Label_Type Hardware_Drive_Informations_Label;
        Roller_Type Hardware_Energy_Standby_Roller;
        Button_Type Hardware_Energy_Apply_Button;

        // - - Softwares

        Roller_Type Softwares_Roller;
        Button_Type Softwares_Delete_Button;

        // - - System

        Button_Type System_Device_Apply_Button;
        Text_Area_Type System_Device_Name_Text_Area;
        Button_Type System_Update_Button;
        Button_Type System_Reboot_Loader_Button;

        Button_Type System_Time_Apply_Button;
        Button_Type System_Time_Plus_Button;
        Button_Type System_Time_Minus_Button;
        Spinbox_Type System_Time_Daylight_Offset_Spinbox;
        Text_Area_Type System_Time_NTP_Server_Text_Area;
        Roller_Type System_Time_Zone_Roller;

        Shell_Class *Shell_Pointer;

        friend class Shell_Class;

    } *Preferences_Pointer;

    class Installer_Class
    {
    public:
        // - Methods

        static void Open(Shell_Class *Shell_Pointer);
        static bool Is_Openned(Shell_Class *Shell_Pointer);
        static void Close(Shell_Class *Shell_Pointer);

    private:
        // - Methods

        // - - Constructor / destructor
        Installer_Class(Shell_Class* Shell_Pointer);
        ~Installer_Class();

        



    } *Installer_Pointer;

    // -- Attributes

    // -- Methods

    void Execute_Instruction(Instruction_Type Instruction);
    uint32_t Next_Refresh;
    char Temporary_Char_Array[6];

    Result_Type Save_Registry();
    Result_Type Load_Registry();

    void Main_Task_Function() override;

    Software_Type *Open_Shell();
};

#endif