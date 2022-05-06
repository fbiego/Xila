///
/// @file Core.hpp
/// @author Alix ANNERAUD (alix.anneraud@outlook.fr)
/// @brief Xila's core header file.
/// @version 0.1
/// @date 28-03-2021
///
/// @copyright Copyright (c) 2021
///
/// @details Gather all the parts used by Xila core.
///
/// @section MIT License
///
/// Copyright (c) 2021 Alix ANNERAUD
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
/// The above notice and this permission notice shall be included in all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
///
#ifndef CORE_HPP_INCLUDED
#define CORE_HPP_INCLUDED

#ifdef __cplusplus

#else

#error Xila requires a C++ complier, pleasr change file extension to .cpp

#endif

//============================================================================//
//                          Include required libraries                        //
//============================================================================//

// -- C++ standard library
#include <stdint.h>

// -- Arduino framework
#include "Arduino.h"

// -- File system library
#include "FS.h"

// -- SPI library
#include "SPI.h"

// -- Time library
#include "time.h"

// -- Registry management library
#include "ArduinoJson.h" //used to store registries
#include "StreamUtils.h"

// -- WiFi libraries
#include "Print.h"
#include "IPAddress.h"
#include "IPv6Address.h"
#include "WiFiType.h"
#include "WiFiSTA.h"
#include "WiFiAP.h"
#include "WiFiScan.h"
#include "WiFiGeneric.h"
#include "WiFiClient.h"
#include "WiFiServer.h"
#include "WiFiUdp.h"

// -- Debug library
#include <ArduinoTrace.h>

// -- Driver headers
#include "lvgl.h"
#include "Nextion_Library.hpp"

//----------------------------------------------------------------------------//
//                          Include all project file                          //
//----------------------------------------------------------------------------//

// -- Configuration file (at compile time)
#include "Configuration.hpp" // default values
#include "Modules/Log.hpp"
#include "Modules/Path.hpp" // Path list

//----------------------------------------------------------------------------//
//                                Define Const                                //
//----------------------------------------------------------------------------//

///
/// @brief Xila time class
///

//----------------------------------------------------------------------------//
//

///
/// @brief String to 16 bits encoded address conversion macro.
///
#define Address(x, y) (x * 256 + y)

///
/// @brief String to 16 bits encoded instruction conversion macro.
///
#define Instruction(x, y) (x * 256 + y)

///
/// @brief Memory chunk macro.
///
#define Memory_Chunk(x) (x * 1024)

//----------------------------------------------------------------------------//
//                         Define Xila Core API                               //
//----------------------------------------------------------------------------//

class Shell_Class;

///
/// @class Xila_Class
///
/// @brief Core class.
///
/// @details Contain all core A.P.I. and modules to make the kernel working.
///
class Xila_Class
{
public:
    //==============================================================================//
    //                                 Constructors                                 //
    //==============================================================================//

    Xila_Class();
    ~Xila_Class();

    //==============================================================================//
    //                              Enumerations & Type definition                  //
    //==============================================================================//

    /// @brief Xila Address type.
    typedef uint16_t Address;

    /// @brief Image type.
    typedef uint8_t Image;

    /// @brief Xila instruction type.
    typedef uint32_t Instruction;

    /// @brief Page type.
    typedef uint8_t Page;

    /// @brief Task type
    typedef void Task_Function;

    /// @brief Task handle type
    typedef void *Task_Handle;

    /// @brief Instructions used by the core (with the prefix "#").
    enum : Xila_Class::Instruction
    {
        // -- General instructions
        Idle = 0,
        // -- Software state instructions
        Open = 'O',
        Close = 'C',
        Maximize = 'M',
        Minimize = 'm',
        // -- System state instructions
        Shutdown = 200,
        Restart,
        Hibernate = 'H',
        Watchdog = 'W',
        // -- Shell specials instructions
        Desk = 'D',                // Open desk
        Dialog_Open_File = 'f',    // Open open file dialog
        Dialog_Open_Folder = 'F',  // Open open folder dialog
        Dialog_Save_File = 'e',    // Open save file dialog
        Dialog_Keyboard = 'K',     // Open keyboard dialog
        Dialog_Keypad = 'k',       // Open keypad dialog
        Dialog_Color_Picker = 'c', // Open color picker dialog
        Dialog_Power = 'P',        // Open power dialog
        Dialog_Event = 'E',        // Open event dialog
        Dialog_Login = 'L',        // Open login dialog
        Dialog_Load = 'l'          // Open load dialog
    };

    ///
    /// @brief Variable type enumeration
    ///
    enum Types : uint8_t
    {
        Variable_Long = 'l',   ///< Long variable (64 bits)
        Variable_String = 's', ///< String variable (char array)
        Pointer = 'p',         ///< Point variable (pointer)
        Other = 'o'            ///< Other data types
    };

    ///
    /// @brief Software states
    ///
    typedef enum
    {
        Minimized, ///< Maximized state.
        Maximized  ///< Minimized state.
    } State;

    ///
    /// @brief Xila event type.
    ///
    typedef enum : uint8_t
    {
        Success = true,            ///< Success event.
        Error = false,             ///< Error event.
        Warning,                   ///< Warning event.
        Information,               ///< Information event.
        Question,                  ///< Question event.
        Button_1 = 0x31,           ///< Button 1 reply (equivalent to Yes reply).
        Button_2 = 0x32,           ///< Button 2 reply (equivalent to No reply).
        Button_3 = 0x33,           ///< Button 3 reply (equivalent to Cancel reply).
        Default_Yes = Button_1,    ///< Yes reply (equivalent to Button 1).
        Default_No = Button_2,     ///< No reply (equivalent to Button 2).
        Default_Cancel = Button_3, ///< Cancel reply (equivalent to Button 3).
        None
    } Event;

    ///
    /// @brief Color union.
    ///
    union Color
    {
        uint32_t Color;
        uint8_t Component;
    };

    class Software_Handle;

    //==============================================================================//
    ///
    /// @brief Software class.
    ///
    /// @details This API is used by core in order to communicate with the software.
    ///
    class Software
    {
    protected:
        // -- Constructor / Destructor
        Software(Xila_Class::Software_Handle &Software_Handle, uint8_t Queue_Size = Default_Instruction_Queue_Size);
        virtual ~Software();

        // -- Methods
        void Send_Instruction(Xila_Class::Instruction Intruction);

        ///
        /// @brief Convert 2 byte char instruction into Xila Instruction and send it.
        ///
        /// @param Instruction_Char_1 Instruction first byte
        /// @param Instruction_Char_2 Instruction second byte
        inline void Send_Instruction(char Instruction_Char_1, char Instruction_Char_2)
        {
            Send_Instruction(((uint16_t)Instruction_Char_1 << 8) | (uint16_t)Instruction_Char_2);
        }

        virtual void Set_Variable(Xila_Class::Address Address, uint8_t Type, const void *Variable);

        Xila_Class::Instruction Get_Instruction();

        void Set_Watchdog_Timeout(uint16_t Watchdog_Timeout = Default_Watchdog_Timeout);

        // -- Attributes

        ////
        /// @brief Software task handle.
        ///
        Xila_Class::Task_Handle Task_Handle;

        ///
        /// @brief Software handle pointer.
        ///
        Xila_Class::Software_Handle *Handle;

        friend class Xila_Class;
        friend class Shell_Class;

    private:
        ///
        /// @brief Temporary variable to receive current instruction from queue.
        ///
        Xila_Class::Instruction Current_Instruction;

        ///
        /// @brief Queue handle.
        ///
        QueueHandle_t Instruction_Queue_Handle;

        ///
        /// @brief Last software watchdog feed.
        ///
        uint32_t Last_Watchdog_Feed;

        ///
        /// @brief Watchdog defined timeout.
        ///
        uint16_t Watchdog_Timeout;
    };

    //==============================================================================//
    ///
    /// @class Software_Handle
    ///
    /// @brief Software handle used by Xila to manage each software.
    ///
    class Software_Handle
    {
    public:
        // -- Constructors / Destructor
        Software_Handle();
        Software_Handle(const char *Software_Name, uint8_t Icon_ID, Xila_Class::Software *(*Load_Function_Pointer)(), void (*Startup_Function_Pointer)() = NULL);
        ~Software_Handle();

        /// -- Methods -- //
        bool Is_Equal(Xila_Class::Software_Handle const &Software_Handle_To_Compare) const;

    protected:
        ///
        /// @brief Software string name.
        ///
        char Name[Default_Software_Name_Length]; // used to identify the software,

        ///
        /// @brief Software icon.
        ///
        uint8_t Icon;

        ///
        /// @brief Function pointer called by Xila to load software.
        /// @details Function allocate memory and return allocated software memory pointer and then send an "Open" instruction in the queue.
        ///
        Xila_Class::Software *(*Load_Function_Pointer)();

        ///
        /// @brief Function pointer that is called by Xila at startup.
        /// @details That is usefull to start a background task, or launch your application at the startup.
        ///
        void (*Startup_Function_Pointer)();

        // -- Friendship
        friend class Xila_Class;
        friend class Xila_Class::Software;
        friend class Shell_Class;
        friend class Unit_Test_Class;
    };

    //==============================================================================//
    //                                  Modules                                     //
    //==============================================================================//

    ///
    /// @brief Account management class
    ///
    class Account_Class
    {
    public:
        // -- Constructor
        Account_Class();

        ///
        /// @brief Session state type
        ///
        typedef enum : uint8_t
        {
            Disconnected,
            Logged,
            Locked
        } Session_State;

        Xila_Class::Event Check_Credentials(const char *, const char *);

        // -- Getter
        const char *Get_Current_Username();
        uint8_t Get_State();

        friend class Xila_Class;
        friend class Shell_Class;
        friend class Unit_Test_Class;

    protected:
        ///
        /// @brief Loaded username.
        ///
        char Current_Username[9];

        ///
        /// @brief Session state.
        ///
        Session_State State;

        // -- Methods

        Xila_Class::Event Add(const char *Username, const char *Password);
        Xila_Class::Event Delete(const char *);
        Xila_Class::Event Change_Password(const char *, const char *);
        Xila_Class::Event Change_Username(const char *, const char *);
        Xila_Class::Event Set_Autologin(bool Enable);
        Xila_Class::Event Login(const char *Username_To_Check = NULL, const char *Password_To_Check = NULL);
        Xila_Class::Event Logout();
        Xila_Class::Event Lock();

        // -- Setter

        void Set_Current_Username(const char *Username);
        void Set_State(Session_State State);

        Xila_Class::Event Load_Registry();

    } Account;

    //==============================================================================//
    ///
    /// @brief Clipboard module
    ///
    class Clipboard_Class
    {
    public:
        Xila_Class::Event Clear();

        Xila_Class::Event Copy(uint64_t const &Value_To_Copy);
        Xila_Class::Event Copy(const char *Char_Array_To_Copy, size_t Char_Array_Length = 0);
        Xila_Class::Event Copy(String const &String_To_Copy); // deprecated : only for compatibility purpose

        Xila_Class::Event Paste(uint64_t &Value_To_Paste);
        Xila_Class::Event Paste(char *Char_Array_To_Paste, size_t Char_Array_Length);
        Xila_Class::Event Paste(String &String_To_Paste);

        friend class Xila_Class;
        friend class Shell_Class;
        friend class Unit_Test_Class;

    protected:
        ///
        /// @brief Clipboard file.
        ///
        File Clipboard_File;

        ///
        /// @brief Split number array.
        ///
        uint8_t Split_Number[8];
    } Clipboard;

    //==============================================================================//
    ///
    /// @brief Dialog class
    ///
    class Dialog_Class
    {
    public:
        Xila_Class::Event Login();
        Xila_Class::Event Event(const __FlashStringHelper *Message, uint8_t Type, const __FlashStringHelper *Button_Text_1 = NULL, const __FlashStringHelper *Button_Text_2 = NULL, const __FlashStringHelper *Button_Text_3 = NULL);
        Xila_Class::Event Event(const char *Message, uint8_t Type, const char *Button_Text_1, const char *Button_Text_2, const char *Button_Text_3);
        Xila_Class::Event Color_Picker(uint16_t &Color);
        Xila_Class::Event Open_File(File &File_To_Open);
        Xila_Class::Event Open_Folder(File &Folder_To_Open);
        Xila_Class::Event Save_File(File &File_To_Save);
        Xila_Class::Event Keyboard(char *String, size_t Size = 189, bool Masked_Input = false);
        Xila_Class::Event Keypad(float &Number);
        void Power();
        void Load(const __FlashStringHelper *Header, const __FlashStringHelper *Message, uint32_t Duration = 0);
        void Load(const char *Header, const char *Message, uint32_t Duration = 0);
        void Close_Load();

        Dialog_Class();

        friend class Xila_Class;
        friend class Shell_Class;
        friend class Unit_Test_Class;

    protected:
        // -- Attributes

        ///
        /// @brief Dialogs state
        ///
        Xila_Class::Event Power_State;
        Xila_Class::Event Keyboard_State;
        Xila_Class::Event Keypad_State;
        Xila_Class::Event Color_Picker_State;
        Xila_Class::Event Event_State;
        Xila_Class::Event File_Manager_State;
        Xila_Class::Event Credentials_State;
        Xila_Class::Event Login_State;

    } Dialog;

    //==============================================================================//

    ///
    /// @brief Display class
    ///
    class Display_Class : public Nextion_Class
    {
    public:
        Display_Class();
        ~Display_Class();

        // -- Enumerations

        ///
        /// @brief Prefix used to distinguish exchanged data between display, core and software
        ///
        enum Prefix
        {
            Instruction = '#',
            Variable_String = 's',
            Variable_Long = 'l',
        };

        ///
        /// @brief Default system colors.
        ///
        enum Color
        {
            Black,
            White = 65535,
            Light_Grey = 33808,
            Dark_Grey = 16904,
            Red = 57344,
            Blue = 1300,
            Green = 34308,
            Yellow = 64896
        };

        ///
        /// @brief Xila_32 special characters list.
        ///
        enum Font_32
        {
            Exclamation_Mark = 127,
            Question_Mark,
            Cross
        };

        ///
        /// @brief Font list
        ///
        enum Font
        {
            Regular_16 = 0, ///< Roboto Regular 16 px (+ special character), main font used almost everywhere.
            Mono_16,        ///< Robot Mono 16 px, used for terminals etc.
            Regular_24,     ///< Robot Regular 24 px.
            Regular_32,     ///< Roboto Regular 32 px .
            Regular_48      ///< Roboto Regular 48 px.
        };

        static void Incoming_String_Data_From_Display(const char *, uint8_t);
        static void Incoming_Numeric_Data_From_Display(uint32_t);
        static void Incoming_Event_From_Display(uint8_t);

        uint8_t Get_State();

        friend class Xila_Class;
        friend class Shell_Class;
        friend class Unit_Test_Class;

        enum
        {
            Keep = -32768,
        };

        enum Alignment
        {
            Left = LV_ALIGN_LEFT,
            Center = LV_ALIGN_CENTER,
            Right = LV_ALIGN_RIGHT,
            In_Between = LV_ALIGN_IN_BETWEEN,
            In_Between_Hor = LV_ALIGN_IN_BETWEEN_HOR,
            In_Between_Ver = LV_ALIGN_IN_BETWEEN_VER
        };

        /// @brief Animations enumeration
        enum Animation
        {
            Enable = LV_ANIM_ON,
            Disable = LV_ANIM_OFF
        };

        void Initialise();
        bool Initialisation_State();

        /// @brief Object class.
        class Object_Class
        {
        public:
            // -- Types
            typedef lv_state_t State_Type;
            typedef lv_part_t Part_Type;
            typedef lv_obj_flag_t Object_Flag_Type;
            typedef lv_event_t Event_Type;
            typedef lv_obj_t LVGL_Object_Type;

            typedef enum
            {
                All = LV_EVENT_ALL,
                // -- Interaction related events
                Pressed = LV_EVENT_PRESSED,
                Pressing = LV_EVENT_PRESSING,
                Press_Lost = LV_EVENT_PRESS_LOST,
                
                Short_Clicked = LV_EVENT_SHORT_CLICKED,
                Long_Pressed = LV_EVENT_LONG_PRESSED,
                Long_Pressed_Repeat = LV_EVENT_LONG_PRESSED_REPEAT,
                
                Clicked = LV_EVENT_CLICKED,
                Released = LV_EVENT_RELEASED,
                Scroll_Begin = LV_EVENT_SCROLL_BEGIN,
                Scroll_End = LV_EVENT_SCROLL_END,
                Scroll = LV_EVENT_SCROLL,
                Gesture = LV_EVENT_GESTURE,
                Key = LV_EVENT_KEY,
                Focused = LV_EVENT_FOCUSED,
                Defocused = LV_EVENT_DEFOCUSED,
                Leave = LV_EVENT_LEAVE,
                Hit_Test = LV_EVENT_HIT_TEST,
                // -- Drawing context
                Cover_Check = LV_EVENT_COVER_CHECK,
                
                Extra_Draw_Size = LV_EVENT_REFR_EXT_DRAW_SIZE,
                Draw_Main_Begin = LV_EVENT_DRAW_MAIN_BEGIN,
                Draw_Main = LV_EVENT_DRAW_MAIN,
                Draw_Main_End = LV_EVENT_DRAW_MAIN_END,

                Draw_Post_Begin = LV_EVENT_DRAW_POST_BEGIN,
                Draw_Post = LV_EVENT_DRAW_POST,
                Draw_Post_End = LV_EVENT_DRAW_POST_END,
                Draw_Part_Begin = LV_EVENT_DRAW_PART_BEGIN,
                Draw_Part_End = LV_EVENT_DRAW_PART_END,

                // -- Special events
                Value_Changed = LV_EVENT_VALUE_CHANGED,
                Insert = LV_EVENT_INSERT,
                Refresh = LV_EVENT_REFRESH,
                Ready = LV_EVENT_READY,
                Cancel = LV_EVENT_CANCEL,

                // -- State events
                Delete = LV_EVENT_DELETE,
                Child_Changed = LV_EVENT_CHILD_CHANGED,
                Child_Created = LV_EVENT_CHILD_CREATED,
                Child_Deleted = LV_EVENT_CHILD_DELETED,
                
                // -- Screen events
                Screen_Unload_Start = LV_EVENT_SCREEN_UNLOAD_START,
                Screen_Load_Start = LV_EVENT_SCREEN_LOAD_START,
                Screen_Loaded = LV_EVENT_SCREEN_LOADED,
                Screen_Unloaded = LV_EVENT_SCREEN_UNLOADED,

                // -- Appearance events
                Size_Changed = LV_EVENT_SIZE_CHANGED,
                Style_Changed = LV_EVENT_STYLE_CHANGED,
                Layout_Changed = LV_EVENT_LAYOUT_CHANGED,
                Get_Self_Size = LV_EVENT_GET_SELF_SIZE,

                Last_Event = _LV_EVENT_LAST
            } Event_Code_Type;

            // -- Enumerations
            enum
            {
                Default = LV_STATE_DEFAULT,
                Checked = LV_STATE_CHECKED,
                Focused = LV_STATE_FOCUSED,
                Focus_Key = LV_STATE_FOCUS_KEY,
                Edited = LV_STATE_EDITED,
                Hovered = LV_STATE_HOVERED,
                Pressed = LV_STATE_PRESSED,
                Scrolled = LV_STATE_SCROLLED,
                Disabled = LV_STATE_DISABLED,
                Custom_1 = LV_STATE_USER_1,
                Custom_2 = LV_STATE_USER_2,
                Custom_3 = LV_STATE_USER_3,
                Custom_4 = LV_STATE_USER_4,
                Any = LV_STATE_ANY
            };

            enum
            {
                Main = LV_PART_MAIN,
                Scrollbar = LV_PART_SCROLLBAR,
                Indicator = LV_PART_INDICATOR,
                Knob = LV_PART_KNOB,
                Selected = LV_PART_SELECTED,
                Items = LV_PART_ITEMS,
                Ticks = LV_PART_TICKS,
                Cursor = LV_PART_CURSOR,
                Custom_First = LV_PART_CUSTOM_FIRST,
                Any = LV_PART_ANY
            };

            // -- Methods

            // -- Constructors

            Object_Class(Object_Class& Parent_Object);
            ~Object_Class();

            //  -- Management
            void Delete();
            void Clean();
            // -- -- Flags
            void Add_Flag(Object_Flag_Type Flag);
            void Clear_Flag();
            bool Has_Flag(Object_Flag_Type Flag);
            // -- -- States
            void Add_State(State_Type State);
            void Clear_State(State_Type State);
            // -- -- Swap
            void Swap(Object_Class Object_To_Swap_With);
            // -- -- Events
            void Add_Event(Event_Type Event);
            void Send_Event(Event_Type Event);
            // -- -- Layer management
            void Move_Foreground();
            void Move_Background();

            // -- Set attributes values.
            void Set_Index(uint32_t Index);
            void Set_Parent(Object_Type Object, Object_Type Parent_Object);

            // -- Get attributes values.
            void Set_Position(Xila_Class::Object_Type, Coordinates X = Keep, Coordinates Y = Keep);
            void Set_Size(Xila_Class::Object_Type, Coordinates Width = Keep, Coordinates Height = Keep, char Fit = 'N');
            void Set_Alignment(Xila_Class::Object_Type, Xila_Class::Alignment, Xila_Class::Alignment = Xila_Class::Keep);

            // -- Get attributes values.
            State Get_Object_Visibility(Xila_Class::Object_Type);
            void Get_Identifier(Object_Type Object);

            void Set_Parent(Object_Type Parent_Object);

            uint16_t Get_Child_Count(Object_Type Parent_Object);
            Object_Type Get_Child(Object_Type Object, uint16_t Index);

            // -- Operator
            LVGL_Object_Type &operator=(const Object_Class &Object_Type);

        protected:
            LVGL_Object_Class Get_Parent();

            void Set_Pointer(LVGL_Object_Type* Object);
            LVGL_Object_Type* Get_Pointer();

            void Set_User_Data(Object_Type Object, void *User_Data);
            void *Get_User_Data(const Object_Type Object);

            static void Event_Handler(Event_Type Event);

        private:
            LVGL_Object_Type *LVGL_Object_Pointer;
        };

        class Screen_Class : public Object_Class
        {
        public:
            inline const Object Get() // TODO : Maybe move into an header file, integrate directly into display_class.
            {
                return Screen;
            }

        private:
            Object Create();

        protected:
            // -- Root component for all element displayed
            Object Screen;

        }

        class Arc_Class : public Object_Class
        {
            typedef enum
            {
                Normal = LV_ARC_MODE_NORMAL,
                Symmetrical = LV_ARC_MODE_SYMMETRICAL,
                Reversed = LV_ARC_MODE_REVERSED
            } Mode_Type;

            typedef enum
            {
                Background = LV_ARC_DRAW_PART_BACKGROUND,
                Foreground = LV_ARC_DRAW_PART_FOREGROUND,
                Knob = LV_ARC_DRAW_PART_KNOB
            } Part_Type;

            // -- Managment
            Object Create(Object Parent, const Object Copy = NULL);
            void Align_To_Angle(const Object Object, Object Object_To_Align, Coordinates Radius_Offset);
            void Rotate_To_Angle(const Object Object, Object Object_To_Rotate, Coordinates Radius_Offset);

            // -- Set attributes values.
            void Set_Angles(Object Object, uint16_t Start = 0xFFFF, uint16_t End = 0xFFFF);
            void Set_Background_Angles(Object Object, uint16_t Start = 0xFFFF, unt16_t 0xFFFF);
            void Set_Range(Object Object, int16_t Minimum, int16_t Maximum);
            void Set_Change_Rate(Object Object, uint16_t Rate);
            void Set_Rotation(Object Object, uint16_t Rotation);
            void Set_Mode(Object Object, Mode_Type Mode);
            void Set_Value(Object Object, int16_t Value);

            // -- Get attributes values.
            void Get_Angles(Object Object, uint16_t *Start, uint16_t *End);
            void Get_Background_Angles(Object Object, uint16_t *Start, uint16_t *End);
            void Get_Value(const Object Object) void Get_Range(Object Object, int16_t *Minimum, int16_t *Maximum);
            Mode_Type Get_Mode(Object Object);
        };

        class Bar_Class : public Object_Class
        {
        public:
            typedef enum
            {
                Normal = LV_BAR_MODE_NORMAL,
                Symmetrical = LV_BAR_MODE_SYMMETRICAL,
                Custom = LV_BAR_MODE_RANGE
            } Mode_Type;

            typedef enum
            {
                Indicator = LV_BAR_DRAW_PART_INDICATOR
            } Part_Type;

            // -- Managment
            Object Create(Object Parent, const Object Copy = NULL);

            // -- Set attributes values.
            void Set_Value(Object Bar, int32_t Value, Animation Animation_State = Enable);
            void Set_Start_Value(Object Bar, int32_t Value, Animation Animation_State = Enable);
            void Set_Range(Object Bar, int32_t Minimum_Value, int32_t Maximum_Value);
            void Set_Mode(Object Bar, Mode_Type Mode);
            void Set_Type(Object Bar, Type_Enum Type);

            // -- Get attributes values.
            int32_t Get_Value(Object Bar);
            int32_t Get_Start_Value(Object Bar);
            void Get_Range(Object Bar, int32_t *Minimum_Value, int32_t *Maximum_Value);
            Mode_Type Get_Mode(Object Bar);
        };

        class Button_Class : public Object_Class
        {
        public:
            Object Create(Object Parent);
        };

        class Windows_Class
        {
        public:
            typedef Xila_Class::Object_Type Object_Type;

            Object_Type Load(File);

            Object_Type Create();

            void Set_Title(Object_Type Window, const char *Title);

        protected:
            Object_Type Add_Button(Object_Type Parent_Window);

        private:
            Object_Type Title_Label;
            Object_Type Clock_Label;

            Object_Type Network_Button;
            Object_Type Battery_Button;
            Object_Type Sound_Button;

            Object_Type Close_Button;
            Object_Type Minimize_Button;

        }

        static void
        Task(void *);

        Xila_Class::Task_Handle Task_Handle;

        Xila_Class::Semaphore_Handle_Type Mutex_Semaphore_Handle;

        // -- Display state
        uint16_t Get_Horizontal_Definition();
        uint16_t Get_Vertical_Definition();

        Xila_Class::Event Compile_Page(File);
        Xila_Class::Event Load_Page(File, Object *, uint16_t);

    protected:
        void Set_State(uint8_t State);

        uint8_t State;

        uint8_t Brightness, Receive_Pin, Standby_Time, Transmit_Pin;

        Xila_Class::Address Current_Address;

        Xila_Class::Event Load_Registry();
        Xila_Class::Event Save_Registry();

    } Display;

    //==============================================================================//
    ///
    /// @brief Drive class
    ///
    class Drive_Class
    {
    public:
        ///
        /// @brief Drive type.
        ///
        typedef enum
        {
            None,  ///< None (nothing connected).
            MMC,   ///< MMC type.
            SD_SC, ///< SD type.
            SD_HC, ///< SD HC type.
            Unknow ///< Unknow type.
        } Sd_Card_Type;

        uint64_t Size();
        Sd_Card_Type Type();

        bool Exists(const char *Path);
        bool Exists(const String &Path);

        bool Make_Directory(const char *Path);
        bool Make_Directory(const String &Path);

        bool Remove_Directory(const char *Path);
        bool Remove_Directory(const String &Path);

        File Open(const char *Path, const char *Mode = "r");
        File Open(const String &Path, const char *Mode = "r");

        bool Remove(const char *Path);
        bool Remove(const String &Path);

        bool Rename(const char *Path_From, const char *Path_To);
        bool Rename(const String &Path_From, const String &Path_To);

        uint64_t Total_Bytes();
        uint64_t Used_Bytes();

        // Custom

        Xila_Class::Event Copy(File &Origin_File, File &Destination_File);
        Xila_Class::Event Get_Name(File const &File, char *File_Name_Buffer, size_t Size);
        uint16_t Count_Items(File &Folder);

        // -- Constructor
        Drive_Class();

        // -- Friendship
        friend class Xila_Class;
        friend class Shell_Class;
        friend class Unit_Test_Class;

    protected:
        void End();

        bool Begin(uint8_t Slave_Select_Pin = SS, SPIClass &spi = SPI, uint32_t Frequency = 4000000, const char *Mount_Point = "/sd", uint8_t Maximum_Files = 5);

    } Drive;

    ///=============================================================================//
    ///
    /// @brief Flash class
    ///
    class Flash_Class
    {
    public:
        // -- Methods
        uint32_t Get_Size();
        uint32_t Get_Speed();
        FlashMode_t Get_Mode();

        Xila_Class::Event Read(uint32_t Offset, uint32_t *Data, size_t Size);

        Xila_Class::Event Partition_Read(const esp_partition_t *Partition, uint32_t Offset, uint32_t *Data, size_t Size);

        uint32_t Get_Sketch_Size();
        String Get_Sketch_MD5();
        uint32_t Get_Sketch_Free_Space();

    private:
        // -- Methods
        Xila_Class::Event Set_Boot_Partition(const uint8_t Partition_Number);

        Xila_Class::Event Erase_Sector(uint32_t Sector);
        Xila_Class::Event Write(uint32_t Offset, uint32_t *Data, size_t Size);

        Xila_Class::Event Partition_Write(const esp_partition_t *Partition, uint32_t Offset_, uint32_t *Data, size_t Size);
        Xila_Class::Event Partition_Erase_Range(const esp_partition_t *Partition, uint32_t Offset, size_t Size);

        static uint32_t Sketch_Size(sketchSize_t Response);
        uint32_t Magic_Size(uint8_t Byte);
        uint32_t Magic_Speed(uint8_t Byte);
        FlashMode_t Magic_Mode(uint8_t Byte);

        friend class Xila_Class;
        friend class Shell_Class;
        friend class Unit_Test_Class;
    } Flash;

    //==============================================================================//
    ///
    /// @brief GPIO management class
    ///
    class GPIO_Class
    {
    public:
        GPIO_Class();

        // -- Pin mode
        void Set_Mode(uint8_t Pin, uint8_t Mode);

        Xila_Class::Event Valid_Output_Pin(uint8_t Pin);

        // -- Digital
        void Digital_Write(uint8_t Pin, uint8_t State);
        int16_t Digital_Read(uint8_t Pin);

        Xila_Class::Event Valid_Digital_Pin(uint8_t Pin);

        // -- Analog
        uint16_t Analog_Read(uint8_t Pin);
        uint32_t Analog_Read_Milli_Volts(uint8_t Pin);

        void Set_Voltage_Reference_Pin(uint8_t Pin);
        void Set_Read_Resolutions(uint8_t Bits_Resolution);
        void Set_Width(uint8_t Bits_Width);
        void Set_Clock_Divider(uint8_t Clock_Divider);

        void Set_Attenuation(uint8_t Attenuation);
        void Set_Attenuation(uint8_t Pin, uint8_t Attenuation);

        // -- Interrupts
        void Attach_Interrupt(uint8_t Pin, void (*Function_Pointer)(void), int16_t Mode);
        void Attach_Interrupt_Argument(uint8_t Pin, void (*Function_Pointer)(void *), void *Argument, int16_t Mode);
        void Detach_Interrupt(uint8_t Pin);

        ///
        /// @brief Digital IOs states.
        ///
        enum Digital_States : uint8_t
        {
            Low = LOW,   ///< Low state.
            High = HIGH, ///< High state.

        };

        ///
        /// @brief IOs modes.
        ///
        enum Modes : uint8_t
        {
            Input = 0x01,             ///< Input
            Output = 0x02,            ///< Output
            Pull_Up = 0x04,           ///< Pull up resistor.
            Input_Pull_Up = 0x05,     ///< Input with a pull up resistor.
            Pull_Down = 0x08,         ///< Pull down resistor.
            Input_Pull_Down = 0x09,   ///< Input with a pull down resistor.
            Open_Drain = 0x10,        ///< Open drain (nothing connected).
            Output_Open_Drain = 0x12, ///< Output with open drain by default.
            Special = 0xF0,           ///< Special
            Function_1 = 0x00,        ///< Function 1
            Function_2 = 0x20,        ///< Function 2
            Function_3 = 0x40,        ///< Function 3
            Function_4 = 0x60,        ///< Function 4
            Function_5 = 0x80,        ///< Function 5
            Function_6 = 0xA0,        ///< Function 6
            Analog = 0xC0             ///< Analog
        };

        ///
        /// @brief Interrupt modes.
        ///
        enum Interrupt_Modes : uint8_t
        {
            Disabled = 0x00,  ///< No interrupt.
            Rising = 0x01,    ///< Triggered when signal rise.
            Falling = 0x02,   ///< Triggered when signal fall.
            Change = 0x03,    ///< Triggered when signal fall or rise.
            On_Low = 0x04,    ///< Triggered on low state.
            On_High = 0x05,   ///< Triggered on high state.
            On_Low_WE = 0x0C, ///< Triggered on low state.
            On_High_WE = 0x0D ///< Triggered on high state.
        };

        friend class Xila_Class;
        friend class Shell_Class;
        friend class Unit_Test_Class;
    } GPIO;

    //==============================================================================//
    ///
    /// @brief Keyboard class
    ///
    class Keyboard_Class
    {
    public:
        // -- Keyboard constructor / destructor

        Keyboard_Class();

        ///
        /// @brief Keyboard available layout.
        ///
        enum Layouts
        {
            American, ///< American QWERTY layout.
            German,   ///< German QWERTY layout.
            French,   ///< French AZERTY layout.
            Spanish,  ///< Spanish QWERTY layout.
            Italian,  ///< Italian QWERTY layout.
            English   ///< English QWERTY layout.
        };

        ///
        /// @brief Special keys enumeration.
        ///
        enum Special_Keys
        {
            Tab = 9,
            Enter = 13,
            Backspace = 127,
            Escape = 27,
            Insert = 0,
            Delete = 127,
            Home = 0,
            End = 0,
            Page_Up = 25,
            Page_Down = 26,
            Arrow_Up = 11,
            Arrow_Down = 10,
            Arrow_Left = 8,
            Arrow_Right = 21,
            F1 = 0,
            F2 = 0,
            F3 = 0,
            F4 = 0,
            F5 = 0,
            F6 = 0,
            F7 = 0,
            F8 = 0,
            F9 = 0,
            F10 = 0,
            F11 = 0,
            F12 = 0,
            Scroll = 0,
            Euro_Sign = 0,
            // Custom regional keys
            Inverted_Exclamation = 161,
            Cent_Sign,
            Pound_Sign,
            Currency_Sign,
            Yen_Sign,
            Broken_Bar,
            Secontion_Sign,
            Diaeresis,
            Copyright_Sign,
            Feminine_Ordinal,
            Left_Double_Angle_Quote,
            Not_Sign,
            Hyphen,
            Registered_Sign,
            Macron,
            Degree_Sign,
            Plus_Minus_Sign,
            Superscript_Two,
            Superscript_Three,
            Acute_Accent,
            Micro_Sign,
            Pilcrow_Sign,
            Middle_Dot,
            Cedilla,
            Superscript_One,
            Masculine_Ordinal,
            Right_Double_Angle_Quote,
            Fraction_One_Quarter,
            Fraction_One_Half,
            Fraction_Three_Quarters,
            Inverted_Question_Mark,
            A_Grave,
            A_Acute,
            A_Circumflex,
            A_Tilde,
            A_Diaeresis,
            A_Ring_Above,
            AE,
            C_Cedilla,
            E_Grave,
            E_Acute,
            E_Circumflex,
            E_Diaeresis,
            I_Grave,
            I_Acute,
            I_Circumflex,
            I_Diaeresis,
            Eth,
            N_Tilde,
            O_Grave,
            O_Acute,
            O_Circumflex,
            O_Tilde,
            O_Diaeresis,
            Multiplication,
            O_Stroke,
            U_Grave,
            U_Acute,
            U_Circumflex,
            U_Diaeresis,
            Y_Acute,
            Thorn,
            Sharp_S,
            a_Grave,
            a_Acute,
            a_Circumflex,
            a_Tilde,
            a_Diaeresis,
            a_Ring_Above,
            ae,
            c_Cedilla,
            e_Grave,
            e_Acute,
            e_Circumflex,
            e_Diaeresis,
            i_Grave,
            i_Acute,
            i_Circumflex,
            i_Diaeresis,
            eth,
            n_Tilde,
            o_Grave,
            o_Acute,
            o_Circumflex,
            o_Tilde,
            o_Diaeresis,
            Division,
            o_Stroke,
            u_Grave,
            u_Acute,
            u_Circumflex,
            u_Diaeresis,
            y_Acute,
            thorn,
            y_Diaeresis
        };

        // -- Methods
        uint8_t Read();
        uint8_t Read_Raw();

        uint8_t Available();

        void Clear();

        // -- Friendship
        friend class Xila_Class;
        friend class Shell_Class;
        friend class Unit_Test_Class;

    protected:
        // -- Methods
        Xila_Class::Event Load_Registry();
        Xila_Class::Event Save_Registry();

        uint8_t Layout;

        uint8_t Data_Pin;
        uint8_t Clock_Pin;

        void Begin();

    } Keyboard;

    //==============================================================================//
    ///
    /// @brief Mathematics class
    ///

    class Mathematics_Class
    {
    public:
        ///
        /// @brief Archimedes constant.
        ///
        const double Pi = 3.1415926535897932384626433832795;

        ///
        /// @brief Euler constant.
        ///
        const double Euler = 2.718281828459045235360287471352;

        ///
        /// @brief Degrees to radian coefficient.
        ///
        const double Degrees_To_Radians = 0.017453292519943295769236907684886;

        ///
        /// @brief Radian to degrees coefficient.
        ///
        const double Radians_To_Degrees = 57.295779513082320876798154814105;

        // -- Methods -- //
        double String_To_Float(const char *String); // --
        char *Float_To_String(double Number, int8_t Width, uint8_t Precision, char *String);

        bool Is_NAN(double Number);
        bool Is_Infinite(double Number);

        double Copy_Sign(double Number_1, double Number_2);
        double Floor(double Number);
        double Modulo(double Number_1, double Number_2);
        double Greatest_Common_Divisor(double Number_1, double Number_2);
        double Least_Common_Multiple(double Number_1, double Number_2);

        double Radians(double Degrees);
        double Degrees(double Radians);

        double Factorial(double Number);
        double Absolute(double Number);
        double Inverse(double Number);

        double Combination(double n, double k);
        double Permutation(double n, double k);

        double Sine(double Number);
        double Cosine(double Number);
        double Tangent(double Number);
        double Secant(double Number);
        double Cosecant(double Number);
        double Cotangent(double Number);

        double Arc_Sine(double Number);
        double Arc_Cosine(double Number);
        double Arc_Tangent(double Number);
        double Arc_Secant(double Number);
        double Arc_Cosecant(double Number);
        double Arc_Cotangent(double Number);

        double Hyperbolic_Sine(double Number);
        double Hyperbolic_Cosine(double Number);
        double Hyperbolic_Tangent(double Number);
        double Hyperbolic_Secant(double Number);
        double Hyperbolic_Cosecant(double Number);
        double Hyperbolic_Cotangent(double Number);

        double Arc_Hyperbolic_Sine(double Number);
        double Arc_Hyperbolic_Cosine(double Number);
        double Arc_Hyperbolic_Tangent(double Number);
        double Arc_Hyperbolic_Secant(double Number);
        double Arc_Hyperbolic_Cosecant(double Number);
        double Arc_Hyperbolic_Cotangent(double Number);

        double Power(double Base, double Exponent);
        double Square(double Number);
        double Cube(double Number);

        double Root(double Base, double Exponent);
        double Square_Root(double Number);
        double Cubic_Root(double Number);

        double Exponential(double Number);

        double Logarithm(double Base, double Number);
        double Binary_Logarithm(double Number);
        double Natural_Logarithm(double Number);
        double Decimal_Logarithm(double Number);

        uint32_t Random() const;
        uint32_t Random(uint32_t Upper_Bound) const;
        uint32_t Random(uint32_t Low_Bound, uint32_t Upper_Bound) const;

    } Mathematics;

    //==============================================================================//
    ///
    /// @brief Memory management class.
    ///
    class Memory_Class
    {
    public:
        // -- Methods

        uint32_t Get_Heap_Size();
        uint32_t Get_Free_Heap();
        uint32_t Get_Minimum_Free_Heap();
        uint32_t Get_Maximum_Allocated_Heap();

        uint32_t Get_PSRAM_Size();
        uint32_t Get_Free_PSRAM();
        uint32_t Get_Minimum_Free_PSRAM();
        uint32_t Get_Maximum_Allocated_PSRAM();
    } Memory;

    //==============================================================================//
    ///
    /// @brief Power management module.
    ///
    class Power_Class : public Battery_Class
    {

    protected:
        // -- Constructors / Destructors
        Power_Class();

        // -- Friendship
        friend class Xila_Class;
        friend class Shell_Class;
        friend class Unit_Test_Class;

        // -- Methods

        Xila_Class::Event Save_Registry();
        Xila_Class::Event Load_Registry();

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

    //==============================================================================//
    ///
    /// @brief Software management module.
    ///
    class Software_Management_Class
    {
    public:
        Software_Management_Class();

        Xila_Class::State Get_State(Xila_Class::Software_Handle const &Software_Handle);

        Xila_Class::Event Open(Xila_Class::Software_Handle const &Software_Handle);
        void Minimize(Xila_Class::Software_Handle const &Software_Handle);
        Xila_Class::Event Maximize(Xila_Class::Software_Handle const &);
        void Close(Xila_Class::Software_Handle const &Software_Handle);

        void Feed_Watchdog(Xila_Class::Software_Handle const &Software_Handle);

        // -- Friendship -- //
        friend class Xila_Class;
        friend class Shell_Class;
        friend class Unit_Test_Class;

    protected:
        // -- Attributes -- //

        uint32_t Watchdog_Timer;
        uint8_t Watchdog_State;

        ///
        /// @brief Openned software pointer array
        ///
        /// Openned[0] : Maximized software
        /// Openned[1 - 7] : All openned software (Slot 1 is for Shell)
        ///
        Xila_Class::Software *Openned[8] = {NULL};

        ///
        /// @brief All software handle pointers.
        ///
        Xila_Class::Software_Handle *Handle[Maximum_Software] = {NULL};

        // -- Methods -- //

        void Defrag();

        void Check_Watchdog();

        void Shell_Maximize();
        void Shell_Send_Instruction(Xila_Class::Instruction);
        void Shell_Set_Variable(Xila_Class::Address, uint8_t, const void *);

        uint8_t Seek_Open_Software_Handle(Xila_Class::Software_Handle const &Software_Handle);

        uint8_t Get_Software_Handle(Xila_Class::Software_Handle const &);
        void Add_Handle(Xila_Class::Software_Handle &);

        Xila_Class::Event Force_Close(Xila_Class::Software_Handle const &Software_Handle);

    } Software_Management;

    //==============================================================================//

    ///
    /// @brief Sound class API
    ///
    class Sound_Class
    {
    public:
        Sound_Class();
        ~Sound_Class();

        void Set_Channels(uint8_t Channels);

        void Set_Volume(uint8_t Volume);
        uint8_t Get_Volume();

        uint8_t Play(File &File_To_Play);
        uint8_t Play(const char *File_Path_Or_Host, const char *User = "", const char *Password = "");

        void Set_Loop(bool Loop);

        uint32_t Get_File_Size();
        uint32_t Get_File_Position();
        bool Set_File_Position(uint32_t Position);
        bool Set_File_Seek(const float Speed);

        uint32_t Get_Sample_Rate();
        uint8_t Get_Bit_Resolution();
        uint8_t Get_Channels();
        uint32_t Get_Bit_Rate();

        void Set_Tone(int8_t Gain_Low_Pass, int8_t Gain_Band_Pass, int8_t Gain_High_Pass);

        void Set_Output(uint8_t Output);

        ///
        /// @brief Output DAC.
        ///
        enum : uint8_t
        {
            Internal_DAC = 0, ///< Internal DAC.
            External_DAC      ///< External DAC.
        };

        void Resume();
        void Pause();
        void Mute();
        void Stop();

        void Set_Current_Time(uint16_t Time);
        uint32_t Get_Current_Time();
        uint32_t Get_Duration();

        bool Set_Pinout(uint8_t Bit_Clock_Pin, uint8_t Frame_Clock_Pin, uint8_t Data_Out_Pin, uint8_t Data_In_Pin = (-1));
        void Set_Balance(int8_t Balance = 0);

        uint32_t Get_Total_Time();

        void Set_Time_Offset(int16_t Time);
        uint8_t Get_State();

        void Tone(uint16_t Frequency, uint32_t Duration = 0, uint8_t Pin = 0xFF);
        void No_Tone(uint8_t Pin = 0xFF); // no tone (0xFF default pins)

        static void Task(void *);

        ///
        /// @brief Sound class event type.
        ///
        enum Event
        {
            Failed_To_Get_RTC_Period,   ///< Failed to get ULP processor frequency.
            Failed_To_Open_File,        ///< Failed to open file.
            Failed_To_Get_Metadata,     ///< Failed to read file metadata (corrupted).
            Failed_To_Load_ULP_Program, ///< Failed to load program into ULP.
            Failed_To_Create_Task,      ///< Failed to create sound task.
            Not_RIFF_Compliant,         ///< Invalid metadata (RIFF).
            Not_WAVE_Compliant,         ///< Invalid metadata (WAVE).
            Incompatible_Block_ID,      ///< Incompatible bloc ID.
            Not_A_PCM_File,             ///< Failed to read a non PCM file.
            Unsupported_Bit_Depth,      ///< Unsupported bit depth.
            Unsupported_Sampling_Rate,  ///< Unsupported sampling rate.
            Unsupported_Channel_Number, ///< Unsupported channel number.
        };

        ///
        /// @brief Playing state.
        ///
        enum State
        {
            Stopped, ///< Stopped (no file loaded, no task running)
            Playing, ///< Currently playing.
            Paused   ///< Paused play.
        };

        friend void audio_eof_mp3(const char *);

        // -- Friendship
        friend class Xila_Class;
        friend class Shell_Class;
        friend class Unit_Test_Class;

        ///
        /// @brief Channel enumeration.
        ///
        enum Channels
        {
            Left_Channel = 0,  ///< Left channel only.
            Right_Channel = 1, ///< Right channel only.
            Custom_Channel = 2 ///< Custom channel.
        };

    protected:
        // -- Attributes -- //

        ///
        /// @brief Sound task handle.
        ///
        Xila_Class::Task_Handle Task_Handle;

        ///
        /// @brief Custom pin to tone.
        ///
        uint8_t Custom_Pin;

        ///
        /// @brief Music file.
        ///
        File Music_File;

        ///
        /// @brief Custom data pin.
        ///
        uint8_t Data_Pin;

        ///
        /// @brief Custom word select pin.
        ///
        uint8_t Word_Select_Pin;

        ///
        /// @brief Custom clock pin.
        ///
        uint8_t Clock_Pin;

        uint8_t Output;

        // -- Methods -- //
        Xila_Class::Event Save_Registry();
        Xila_Class::Event Load_Registry();

        void Begin();

    } Sound;

    //==============================================================================//

    ///
    /// @brief System module API
    ///
    class System_Class
    {
    public:
        // -- Constructors
        System_Class();
        ~System_Class();

        // -- Methods
        const char *Get_Device_Name();
        uint64_t Get_eFuse_MAC();
        uint8_t Get_Chip_Revision();
        const char *Get_Chip_Model();
        uint32_t Get_Chip_Cores();
        uint32_t Get_CPU_Frequency();
        const char *Get_SDK_Version();

        void Start();
        void Start(Xila_Class::Software_Handle **Software_Package, uint8_t Size);

        void Shutdown();
        void Restart();
        void Hibernate();

        void Refresh_Header();

        static void Task(void *);

        // -- Friendship
        friend class Xila_Class;
        friend class Shell_Class;
        friend class Unit_Test_Class;

    protected:
        // System's task :
        Xila_Class::Task_Handle Task_Handle;

        ///
        /// @brief Device name used as Network hostname ...
        ///
        char Device_Name[25];

        // -- Methods

        Xila_Class::Event Load_Registry();
        Xila_Class::Event Save_Registry();

        Xila_Class::Event Save_Dump();
        Xila_Class::Event Load_Dump();

        Xila_Class::Event Load_Executable(File Executable_File);

        void Second_Sleep_Routine();

        ///
        /// @brief Panic codes used by the panic handler ("Grey screen").
        ///
        enum Panic_Code
        {
            Missing_System_Files,     ///< Missing system files (registries).
            Damaged_System_Registry,  ///< Damaged system registry.
            Installation_Conflict,    ///< Installation conflict (between MCU and Display or Drive).
            System_Drive_Failure,     ///< System drive failure (disconnected).
            Failed_To_Update_Display, ///< Failed to update display.
            Low_Memory,               ///< Low memory (fragmentation, too much software openned).
            Memory_Corruption,        ///< Memory corruption.
        };

        void Panic_Handler(Panic_Code Panic_Code);

        void First_Start_Routine();
        void Second_Start_Routine();

        void Execute_Startup_Function();
    } System;

    //==============================================================================//

    ///
    /// @brief Task management module
    ///
    class Task_Class
    {

    public:
        typedef TickType_t Tick_Type;

        // -- Task management -- //
        static Xila_Class::Event Create(Xila_Class::Task_Function (*Task_Function)(void *), const char *Task_Name, size_t Stack_Size, void *pvParameters = NULL, Xila_Class::Task_Handle *Task_Handle = NULL) const;
        static void Suspend(Xila_Class::Task_Handle Task_To_Suspend = NULL) const;
        static void Resume(Xila_Class::Task_Handle Task_To_Resume) const;
        static void Delete(Xila_Class::Task_Handle Task_To_Delete = NULL) const;

        static void Delay(uint32_t Delay_In_Millisecond) const;

        // -- Communication between tasks

        typedef SemaphoreHandle_t Semaphore_Handle_Type;
        typedef StaticSemaphore_t Static_Semaphore_Type;
        ///
        /// @brief
        ///
        class Semaphore_Class
        {
        public:
            typedef TickType_t Tick_Type;

            enum
            {
                Binary,
                Counting,
                Mutex,
                Recursive_Mutex
            };

            static Semaphore_Handle_Type Create(uint8_t Type, UBaseType_t Initial_Count, UBaseType_t Maximum_Count);
            static Semaphore_Handle_Type Create_Static(uint8_t Type, Static_Semaphore_Type *Semaphore_Buffer, UBaseType_t Initial_Count = 0, UBaseType_t Maximum_Count = 0);
            static void Delete(Semaphore_Handle_Type Semaphore_Handle);
            static void Take(Semaphore_Handle_Type Semaphore_Handle);
            static void Give(Semaphore_Handle_Type Semaphore_Handle);
            static void Take_From_ISR(Semaphore_Type Semaphore_Handle, signed UBaseType_t *Higher_Priority_Task_Woken);
            static void Give_From_ISR(Semaphore_Type Semaphore_Handle, signed UBaseType_t *Higher_Priority_Task_Woken);
            static void Take_Recursive(Semaphore_Handle_Type Semaphore_Handle, Tick_Type Ticks_To_Wait);
            static void Give_Recursive(Semaphore_Handle_Type Semaphore_Handle);
            static Xila_Class::Task_Handle Get_Mutex_Holder(Semaphore_Handle_Type Semaphore_Handle);
            static UBaseType_t Get_Count(Semaphore_Handle_Type Semaphore);
        };

        // -- Friendship
        friend Xila_Class;
        friend class Shell_Class;
        friend class Unit_Test_Class;

    protected:
        ///
        /// @brief Tasks priorities.
        ///
        enum Priorities
        {
            Idle_Task = 0, ///< Idle task (only for statistics purpose).
            Software_Task, ///< Software task (lowest priority).
            System_Task,   ///< System task.
            Driver_Task    ///< Driver task (highest priority).
        };

        Xila_Class::Event Create(Xila_Class::Task_Function (*Task_Function)(void *), const char *Task_Name, size_t Stack_Size, void *pvParameters, uint16_t Priority, Xila_Class::Task_Handle *Task_Handle);

    } Task;

    //==============================================================================//

    ///
    /// @brief
    ///
    class Time_Class
    {

    public:
        // -- Methods
        tm Get_Time();
        void Synchronize();

        uint32_t Get_Cycle_Count();
        uint32_t Milliseconds() const;
        int64_t Microseconds() const;

        // -- Time constructor / destructor

        Time_Class();

        // -- Friendship
        friend class Xila_Class;
        friend class Shell_Class;
        friend class Unit_Test_Class;

    protected:
        // -- Attributes

        ///
        /// @brief GMT offset in seconds.
        ///
        int32_t GMT_Offset;

        ///
        /// @brief Daylight offset in seconds.
        ///
        int16_t Daylight_Offset;

        ///
        /// @brief Current time structure.
        ///
        tm Current_Time;

        ///
        /// @brief Current unix time in seconds.
        ///
        time_t Now;

        ///
        /// @brief NTP server name.
        ///
        char NTP_Server[32];

        // -- Methods
        Xila_Class::Event Load_Registry();
        Xila_Class::Event Save_Registry();
    } Time;

    //==============================================================================//
    ///
    /// @brief WiFi class
    ///
    class WiFi_Class : public WiFiGenericClass, public WiFiSTAClass, public WiFiScanClass, public WiFiAPClass
    {

    public:
        void printDiag(Print &dest);

        void enableProv(bool status);
        bool isProvEnabled();

        WiFi_Class();

        using WiFiGenericClass::channel;

        using WiFiSTAClass::BSSID;
        using WiFiSTAClass::BSSIDstr;
        using WiFiSTAClass::RSSI;
        using WiFiSTAClass::SSID;

        using WiFiScanClass::BSSID;
        using WiFiScanClass::BSSIDstr;
        using WiFiScanClass::channel;
        using WiFiScanClass::encryptionType;
        using WiFiScanClass::RSSI;
        using WiFiScanClass::SSID;

        void Set_Credentials(const char *Name, const char *Password);

        Xila_Class::Event Load_Registry();
        Xila_Class::Event Save_Registry();

        friend class WiFiClient;
        friend class WiFiServer;
        friend class WiFiUDP;

        friend Xila_Class;
        friend class Shell_Class;
        friend class Unit_Test_Class;

    protected:
        bool prov_enable;

        ///
        /// @brief Access point password.
        ///
        char Password[82];

    } WiFi;

private:
    //==============================================================================//
    //                                    Attributes                                //
    //==============================================================================//

    ///
    /// @brief Current instance pointer. Help to prevent from corruption.
    ///
    static Xila_Class *Instance_Pointer;
};

bool operator==(Xila_Class::Software_Handle const &a, Xila_Class::Software_Handle const &b);

#include "Core/Modules/Task.hpp"
#include "Core/Modules/Time.hpp"
#include "Core/Modules/System.hpp"
#include "Core/Modules/Mathematics.hpp"
#include "Core/Modules/Software_Management.hpp"

#include "Core/Abstraction/Display.hpp"
#include "Core/Abstraction/GPIO.hpp"
#include "Core/Abstraction/Memory.hpp"
#include "Core/Abstraction/Flash.hpp"

extern Xila_Class Xila;

#endif