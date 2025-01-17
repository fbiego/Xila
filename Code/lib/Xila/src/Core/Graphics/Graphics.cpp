///
/// @file Graphics.cpp
/// @author Alix ANNERAUD (alix.anneraud@outlook.fr)
/// @brief
/// @version 0.1.0
/// @date 20-07-2022
///
/// @copyright Copyright (c) 2022
///

#include "Core/Graphics/Graphics.hpp"
#include "Core/Display/Display.hpp"

#include "Core/Core.hpp"

#include "lvgl.h"

using namespace Xila_Namespace;
using namespace Xila_Namespace::Graphics_Types;

Graphics_Type Graphics();

Graphics_Class::Graphics_Class() : Task(this)
{
}

Graphics_Class::~Graphics_Class()
{
    lv_deinit();
}

Result_Type Graphics_Class::Start()
{
    lv_init();

    while (!lv_is_initialized())
    {
        Task_Type::Delay_Static(10);
    }

    // - Set draw buffer
    lv_disp_draw_buf_init(&Draw_Buffer_Descriptor, Draw_Buffer, NULL, Display_Horizontal_Definition);

    // - Set screen driver interface
    lv_disp_drv_init(&Screen_Driver_Interface);

    Screen_Driver_Interface.hor_res = Display_Horizontal_Definition;
    Screen_Driver_Interface.ver_res = Display_Vertical_Definition;
    Screen_Driver_Interface.flush_cb = Display_Class::Output_Flush;
    Screen_Driver_Interface.draw_buf = &Draw_Buffer_Descriptor;

    lv_disp_drv_register(&Screen_Driver_Interface);

    // - Set touch pad driver interface
    lv_indev_drv_init(&Input_Device_Driver_Interface);

    Input_Device_Driver_Interface.type = LV_INDEV_TYPE_POINTER;
    Input_Device_Driver_Interface.read_cb = Display_Class::Input_Read;

    lv_indev_drv_register(&Input_Device_Driver_Interface);

    // - Set theme
    Theme.Initialize();

    // - Set file system driver
    Task.Create(Task_Start_Function, "Graphics", 2048, NULL, Task_Type::Priority_Type::System);

    return Result_Type::Success;
}

void Graphics_Class::Task_Start_Function(void *Instance_Pointer)
{
    ((Graphics_Class *)Instance_Pointer)->Task_Function();
}

void Graphics_Class::Task_Function()
{
    while (true)
    {
        lv_tick_inc(6);
        lv_timer_handler();
        Task_Type::Delay_Static(6);
    }
}

void Graphics_Class::Event_Handler(lv_event_t *Event)
{
    static Instruction_Type Instruction;
    static Object_Type Object;

    Instruction.Set_Sender(&Graphics);
    Instruction.Set_Receiver((Module_Class *)lv_event_get_user_data(Event));

    Instruction.Graphics.Set_Code(static_cast<Event_Code_Type>(lv_event_get_code(Event)));

    Object.Clear_Pointer();
    Object.Set_Pointer(lv_event_get_target(Event));

    Instruction.Graphics.Set_Object(Object);

    Instruction.Get_Receiver()->Send_Instruction(Instruction);
}

Object_Type Graphics_Type::Get_Top_Layer()
{
    return Object_Type(lv_layer_top());
}

Object_Type Graphics_Type::Get_Screen()
{
    return Object_Type(lv_scr_act());
}

/*

// -- File system callbacks

void *Display_Class::File_System_Open(lv_fs_drv_t *Driver, const char *Path, lv_fs_mode_t Mode)
{
    File File_To_Open;
    if (Mode == LV_FS_MODE_WR)
    {
        File_To_Open = Drive.Open(Path, FILE_WRITE);
    }
    else
    {
        File_To_Open = Drive.Open(Path, FILE_WRITE);
    }

    if (!File_To_Open)
    {
        return NULL;
    }

    return File_To_Open;
}

static lv_fs_res_t Display_Class::File_System_Close(lv_fs_drv_t *Driver, void *File_Pointer)
{
    *(File *)File_Pointer.Close();
    return LV_FS_RES_OK;
}

static lv_fs_res_t Display_Class::File_System_Read(lv_fs_drv_t *Driver, void *File_Pointer, void *Buffer, uint32_t Bytes_To_Read, uint32_t *Bytes_Read)
{
    File File_To_Read = *(File *)File_Pointer;

    if (!File_To_Read)
    {
        return LV_FS_RES_FS_ERR;
    }

    *Bytes_Read = File_To_Read.read((uint8_t *)Buffer, Bytes_To_Read);
    return LV_FS_RES_OK;
}

static lv_fs_res_t Display_Class::File_System_Write(lv_fs_drv_t *Driver, void *File_Pointer, const void *Buffer, uint32_t Bytes_To_Write, uint32_t *Bytes_Written)
{
    File File_To_Write = *(File *)File_Pointer;

    if (!File_To_Write)
    {
        return LV_FS_RES_FS_ERR;
    }

    *Bytes_Written = File_To_Write.write((uint8_t *)Buffer, Bytes_To_Write);
    return LV_FS_RES_OK;
}

static lv_fs_res_t Display_Class::File_System_Set_Position(lv_fs_drv_t *Driver, void *File_Pointer, uint32_t Position, lv_fs_whence_t Whence)
{
    File File_To_Seek = *(File *)File_Pointer;

    if (!File_To_Seek)
    {
        return LV_FS_RES_FS_ERR;
    }

    switch (Whence)
    {
    case LV_FS_SEEK_SET:
        File_To_Seek.Seek(Position, SeekSet);
        break;

    case LV_FS_SEEK_CUR:
        File_To_Seek.Seek(Position, SeekCur);
        break;

    case LV_FS_SEEK_END:
        File_To_Seek.Seek(Position, SeekEnd);
        break;

    default:
        break;
    }

    return LV_FS_RES_OK;
}

static lv_fs_res_t Display_Class::File_System_Get_Position(lv_fs_drv_t *Driver, void *File_Pointer, uint32_t *Position)
{
    File File_To_Get_Position = *(File *)File_Pointer;

    if (!File_To_Get_Position)
    {
        return LV_FS_RES_FS_ERR;
    }

    *Position = File_To_Get_Position.position();
    return LV_FS_RES_OK;
}

static void *Display_Class::File_System_Open_Directory(lv_fs_drv_t *Driver, const char *Path)
{
    File File_To_Open = Drive.Open(Path);
    if (!File_To_Open)
    {
        return LV_FS_RES_FS_ERR;
    }
    else if (!File_To_Open.isDirectory())
    {
        return LV_FS_RES_NOT_EX;
    }
    return &File_To_Open;
}

static lv_fs_res_t Display_Class::File_System_Directory_Read(lv_fs_drv_t *Driver, void *Directory_Pointer, char *File_Name)
{
    return LV_FS_RES_NOT_IMP;
}

static lv_fs_res_t Display_Class::File_System_Close_Directory(lv_fs_drv_t *Driver, void *Directory_Pointer)
{
    File File_Pointer = *(File *)Directory_Pointer;
    File_Pointer.Close();
    return LV_FS_RES_OK;
}*/