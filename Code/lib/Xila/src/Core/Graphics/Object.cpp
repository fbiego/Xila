///
/// @file Object.cpp
/// @author Alix ANNERAUD (alix.anneraud@outlook.fr)
/// @brief
/// @version 0.1.0
/// @date 08-07-2022
///
/// @copyright Copyright (c) 2022
///

#include "Core/Graphics/Graphics.hpp"
#include "Core/Graphics/Object.hpp"
#include "Core/Graphics/Animation.hpp"
#include "Core/Core.hpp"

using namespace Xila_Namespace;
using namespace Xila_Namespace::Graphics_Types;

bool operator==(const Object_Type &Object_1, const Object_Type &Object_2)
{
    return Object_1.Get_Pointer() == Object_2.Get_Pointer();
}

// ------------------------------------------------------------------------- //
//
//                                  Constructor
//
// ------------------------------------------------------------------------- //

Object_Class::Object_Class() : LVGL_Object_Pointer(NULL)
{
}

Object_Class::Object_Class(lv_obj_t* Object_Pointer) : LVGL_Object_Pointer(Object_Pointer)
{
}

Object_Class::Object_Class(const Object_Class &Object_To_Copy)
{
    this->Set_Pointer(Object_To_Copy.Get_Pointer());
}

// ------------------------------------------------------------------------- //
//
//                                  Operators
//
// ------------------------------------------------------------------------- //

Object_Class::operator bool()
{
    return Is_Valid();
}

// ------------------------------------------------------------------------- //
//
//                                  Management
//
// ------------------------------------------------------------------------- //

void Object_Class::Create(Object_Class Parent_Object)
{
    // Parent object is required (windows, containers, etc.)
    if (Parent_Object)
    {
        Set_Pointer(lv_obj_create(Parent_Object.Get_Pointer()));
    }
}

void Object_Class::Delete()
{
    lv_obj_del_async(Get_Pointer());
    Set_Pointer(NULL);
}

void Object_Class::Clean()
{
    lv_obj_clean(Get_Pointer());
}

void Object_Class::Add_Flag(Flag_Type Flag)
{
    lv_obj_add_flag(Get_Pointer(), static_cast<lv_obj_flag_t>(Flag));
}

void Object_Class::Clear_Flag(Flag_Type Flag)
{
    lv_obj_clear_flag(Get_Pointer(), static_cast<lv_obj_flag_t>(Flag));
}

bool Object_Class::Has_Flag(Flag_Type Flag)
{
    return lv_obj_has_flag(Get_Pointer(), static_cast<lv_obj_flag_t>(Flag));
}

bool Object_Class::Has_Any_Flag(Flag_Type Flag)
{
    return lv_obj_has_flag_any(Get_Pointer(), static_cast<lv_obj_flag_t>(Flag));
}

void Object_Class::Add_State(State_Type State)
{
    lv_obj_add_state(Get_Pointer(), static_cast<lv_state_t>(State));
}

void Object_Class::Clear_State(State_Type State)
{
    lv_obj_clear_state(Get_Pointer(), static_cast<lv_state_t>(State));
}

bool Object_Class::Has_State(State_Type State)
{
    return lv_obj_has_state(Get_Pointer(), static_cast<lv_state_t>(State));
}

void Object_Class::Allocate_Special_Data()
{
    lv_obj_allocate_spec_attr(Get_Pointer());
}

void Object_Class::Swap(Object_Class Object_To_Swap_With)
{
    lv_obj_swap(Get_Pointer(), Object_To_Swap_With.Get_Pointer());
}

void Object_Class::Add_Event(Module_Class *Module_Pointer, Event_Code_Type Event_Code)
{
    lv_obj_add_event_cb(Get_Pointer(), Graphics_Class::Event_Handler, (lv_event_code_t)Event_Code, (void *)Module_Pointer); // Use user data pointer to store argument of the event.
}

bool Object_Class::Remove_Event(Module_Class *Module_Pointer)
{
    return lv_obj_remove_event_cb_with_user_data(Get_Pointer(), Graphics_Class::Event_Handler, Module_Pointer);
}

bool Object_Class::Remove_All_Events()
{

    uint16_t i = 0;
    while (lv_obj_remove_event_cb(Get_Pointer(), Graphics_Class::Event_Handler) == true)
    {
    }

    if (i == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void Object_Class::Send_Event(Event_Code_Type Event_Code)
{
    lv_event_send(Get_Pointer(), (lv_event_code_t)Event_Code, NULL);
}

void Object_Class::Move_Foreground()
{
    lv_obj_move_foreground(Get_Pointer());
}

void Object_Class::Move_Background()
{
    lv_obj_move_background(Get_Pointer());
}

bool Object_Class::Check_Type(const Class_Type *Class_To_Check)
{
    return lv_obj_check_type(Get_Pointer(), Class_To_Check);
}

bool Object_Class::Has_Class(const Class_Type *Class_To_Check)
{
    return lv_obj_has_class(Get_Pointer(), Class_To_Check);
}

bool Object_Class::Is_Valid()
{
    if (Get_Pointer() != NULL)
    {
        return false;
    }
    return lv_obj_is_valid(Get_Pointer());
}

Coordinate_Type Object_Class::DPX(Coordinate_Type Pixels_To_Scale)
{
    return lv_obj_dpx(Get_Pointer(), Pixels_To_Scale);
}

void Object_Class::Add_Style(Style_Type &Style, Style_Selector_Type Style_Selector)
{
    lv_obj_add_style(Get_Pointer(), Style.Get_Pointer(), Style_Selector);
}

// ------------------------------------------------------------------------- //
//
//                                    Setters
//
// ------------------------------------------------------------------------- //

void Object_Class::Set_User_Data(void *User_Data)
{
    lv_obj_set_user_data(Get_Pointer(), User_Data);
}

void Object_Class::Set_Parent(Object_Class Parent_Object)
{
    lv_obj_set_parent(Get_Pointer(), Parent_Object.Get_Pointer());
}

// -- Position and size.

void Object_Class::Set_Alignment(Alignment_Type Alignment)
{
    lv_obj_set_align(Get_Pointer(), (lv_align_t)Alignment);
}

void Object_Class::Set_Alignment(Alignment_Type Alignment, Coordinate_Type X_Offset, Coordinate_Type Y_Offset)
{
    lv_obj_align(Get_Pointer(), (lv_align_t)Alignment, X_Offset, Y_Offset);
}

void Object_Class::Set_Alignment(Object_Class Object_To_Align_With, Alignment_Type Alignment, Coordinate_Type X_Offset, Coordinate_Type Y_Offset)
{

    lv_obj_align_to(Get_Pointer(), Object_To_Align_With.Get_Pointer(), (lv_align_t)Alignment, X_Offset, Y_Offset);
}

void Object_Class::Set_Position_X(Coordinate_Type X)
{
    lv_obj_set_x(Get_Pointer(), X);
}

void Object_Class::Set_Position_Y(Coordinate_Type Y)
{
    lv_obj_set_y(Get_Pointer(), Y);
}

void Object_Class::Set_Position(Coordinate_Type X, Coordinate_Type Y)
{
    lv_obj_set_pos(Get_Pointer(), X, Y);
}

void Object_Class::Set_Size(Coordinate_Type Width, Coordinate_Type Height)
{
    lv_obj_set_size(Get_Pointer(), Width, Height);
}

void Object_Class::Set_Width(Coordinate_Type Width)
{
    lv_obj_set_width(Get_Pointer(), Width);
}

void Object_Class::Set_Height(Coordinate_Type Height)
{
    lv_obj_set_height(Get_Pointer(), Height);
}

void Object_Class::Set_Content_Height(Coordinate_Type Content_Height)
{
    lv_obj_set_content_height(Get_Pointer(), Content_Height);
}

bool Object_Class::Set_Pointer(lv_obj_t *LVGL_Object_Pointer)
{
    if (Get_Pointer() != NULL)
    {
        return false;
    }
    this->LVGL_Object_Pointer = LVGL_Object_Pointer;
    return true;
}

void Object_Class::Set_Index(uint32_t Index)
{
    lv_obj_move_to_index(Get_Pointer(), Index);
}

void Object_Class::Set_Flex_Alignment(Flex_Alignment_Type Main_Place_Alignment, Flex_Alignment_Type Cross_Place_Alignment, Flex_Alignment_Type Track_Cross_Place)
{
    lv_obj_set_flex_align(Get_Pointer(), (lv_flex_align_t)Main_Place_Alignment, (lv_flex_align_t)Cross_Place_Alignment, (lv_flex_align_t)Track_Cross_Place);
}

void Object_Class::Set_Flex_Flow(Flex_Flow_Type Flow)
{
    lv_obj_set_flex_flow(Get_Pointer(), (lv_flex_flow_t)Flow);
}

void Object_Class::Set_Flex_Grow(uint8_t Grow)
{
    lv_obj_set_flex_grow(Get_Pointer(), Grow);
}

void Object_Class::Set_Grid_Alignment(Grid_Alignment_Type Column_Alignment, Grid_Alignment_Type Row_Alignment)
{
    lv_obj_set_grid_align(Get_Pointer(), (lv_grid_align_t)Column_Alignment, (lv_grid_align_t)Row_Alignment);
}

void Object_Class::Set_Grid_Cell(Grid_Alignment_Type Column_Alignment, uint8_t Column_Position, uint8_t Column_Span, Grid_Alignment_Type Row_Alignment, uint8_t Row_Position, uint8_t Row_Span)
{
    lv_obj_set_grid_cell(Get_Pointer(), (lv_grid_align_t)Column_Alignment, Column_Position, Column_Span, (lv_grid_align_t)Row_Alignment, Row_Position, Row_Span);
}

void Object_Class::Set_Grid_Descriptor_Array(const Coordinate_Type *Column_Descriptor, const Coordinate_Type *Row_Descriptor)
{
    lv_obj_set_grid_dsc_array(Get_Pointer(), Column_Descriptor, Row_Descriptor);
}

void Object_Class::Set_Style_Alignment(Alignment_Type Alignment, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_align(Get_Pointer(), (lv_align_t)Alignment, Style_Selector);
}

void Object_Class::Set_Style_Animation_Speed(uint32_t Value, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_anim_speed(Get_Pointer(), Value, Style_Selector);
}

void Object_Class::Set_Style_Animation_Time(uint32_t Value, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_anim_time(Get_Pointer(), Value, Style_Selector);
}

void Object_Class::Set_Style_Arc_Color(Color_Type Color, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_arc_color(Get_Pointer(), Color.Get_LVGL_Color(), Style_Selector);
}

void Object_Class::Set_Style_Arc_Image_Source(const void *Image_Source, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_arc_img_src(Get_Pointer(), Image_Source, Style_Selector);
}

void Object_Class::Set_Style_Arc_Opacity(Opacity_Type Opacity, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_arc_opa(Get_Pointer(), static_cast<lv_opa_t>(Opacity), Style_Selector);
}

void Object_Class::Set_Style_Arc_Rounded(bool Rounded, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_arc_rounded(Get_Pointer(), Rounded, Style_Selector);
}

void Object_Class::Set_Style_Arc_Width(Coordinate_Type Width, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_arc_width(Get_Pointer(), Width, Style_Selector);
}

void Object_Class::Set_Style_Base_Direction(Direction_Type Direction, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_base_dir(Get_Pointer(), static_cast<lv_dir_t>(Direction), Style_Selector);
}

void Object_Class::Set_Style_Background_Color(Color_Type Color, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_bg_color(Get_Pointer(), Color.Get_LVGL_Color(), Style_Selector);
}

void Object_Class::Set_Style_Background_Dither_Mode(Dither_Mode::Type Dither_Mode, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_bg_dither_mode(Get_Pointer(), Dither_Mode, Style_Selector);
}

void Object_Class::Set_Style_Background_Gradient(const Gradient::Descriptor_Type *Gradient_Descriptor, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_bg_grad(Get_Pointer(), Gradient_Descriptor, Style_Selector);
}

void Object_Class::Set_Style_Background_Gradient_Color(Color_Type Color, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_bg_grad_color(Get_Pointer(), Color.Get_LVGL_Color(), Style_Selector);
}

void Object_Class::Set_Style_Background_Gradient_Direction(Gradient::Direction_Type Gradient_Direction, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_bg_grad_dir(Get_Pointer(), Gradient_Direction, Style_Selector);
}

void Object_Class::Set_Style_Background_Gradient_Stop(Coordinate_Type Value, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_bg_grad_stop(Get_Pointer(), Value, Style_Selector);
}

void Object_Class::Set_Style_Background_Image_Opacity(Opacity_Type Opacity, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_bg_img_opa(Get_Pointer(), static_cast<lv_opa_t>(Opacity), Style_Selector);
}

void Object_Class::Set_Style_Background_Image_Recolor(Color_Type Color, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_bg_img_recolor(Get_Pointer(), Color.Get_LVGL_Color(), Style_Selector);
}

void Object_Class::Set_Style_Background_Image_Recolor_Opacity(Opacity_Type Opacity, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_bg_img_recolor_opa(Get_Pointer(), static_cast<lv_opa_t>(Opacity), Style_Selector);
}

void Object_Class::Set_Style_Background_Image_Source(const void *Image_Source, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_bg_img_src(Get_Pointer(), Image_Source, Style_Selector);
}

void Object_Class::Set_Style_Background_Image_Tiled(bool Tiled, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_bg_img_tiled(Get_Pointer(), Tiled, Style_Selector);
}

void Object_Class::Set_Style_Background_Main_Stop(Coordinate_Type Value, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_bg_main_stop(Get_Pointer(), Value, Style_Selector);
}

void Object_Class::Set_Style_Background_Opacity(Opacity_Type Opacity, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_bg_opa(Get_Pointer(), static_cast<lv_opa_t>(Opacity), Style_Selector);
}

void Object_Class::Set_Style_Blend_Mode(Blend::Mode_Type Blend_Mode, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_blend_mode(Get_Pointer(), Blend_Mode, Style_Selector);
}

void Object_Class::Set_Style_Border_Color(Color_Type Color, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_border_color(Get_Pointer(), Color.Get_LVGL_Color(), Style_Selector);
}

void Object_Class::Set_Style_Border_Opacity(Opacity_Type Opacity, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_border_opa(Get_Pointer(), static_cast<lv_opa_t>(Opacity), Style_Selector);
}

void Object_Class::Set_Style_Border_Post(bool Value, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_border_post(Get_Pointer(), Value, Style_Selector);
}

void Object_Class::Set_Style_Border_Side(Border_Side_Type Border_Side, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_border_side(Get_Pointer(), static_cast<lv_border_side_t>(Border_Side), Style_Selector);
}

void Object_Class::Set_Style_Border_Width(Coordinate_Type Width, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_border_width(Get_Pointer(), Width, Style_Selector);
}

void Object_Class::Set_Style_Clip_Corner(bool Value, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_clip_corner(Get_Pointer(), Value, Style_Selector);
}

void Object_Class::Set_Style_Color_Filter_Descriptor(Color_Filter_Descriptor_Type &Color_Filter_Descriptor, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_color_filter_dsc(Get_Pointer(), Color_Filter_Descriptor.Get_Pointer(), Style_Selector);
}

void Object_Class::Set_Style_Color_Filter_Opacity(Opacity_Type Opacity, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_color_filter_opa(Get_Pointer(), static_cast<lv_opa_t>(Opacity), Style_Selector);
}

void Object_Class::Set_Style_Flex_Cross_Place(Flex_Alignment_Type Cross_Place, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_flex_cross_place(Get_Pointer(), (lv_flex_align_t)Cross_Place, Style_Selector);
}

void Object_Class::Set_Style_Flex_Flow(Flex_Flow_Type Flow, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_flex_flow(Get_Pointer(), (lv_flex_flow_t)Flow, Style_Selector);
}

void Object_Class::Set_Style_Flex_Grow(uint8_t Value, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_flex_grow(Get_Pointer(), Value, Style_Selector);
}

void Object_Class::Set_Style_Flex_Main_Place(Flex_Alignment_Type Main_Place, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_flex_main_place(Get_Pointer(), (lv_flex_align_t)Main_Place, Style_Selector);
}

void Object_Class::Set_Style_Flex_Track_Place(Flex_Alignment_Type Track_Place, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_flex_track_place(Get_Pointer(), (lv_flex_align_t)Track_Place, Style_Selector);
}

void Object_Class::Set_Style_Grid_Cell_Column_Position(Coordinate_Type Column_Position, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_grid_cell_column_pos(Get_Pointer(), Column_Position, Style_Selector);
}

void Object_Class::Set_Style_Grid_Cell_Column_Span(Coordinate_Type Column_Span, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_grid_cell_column_span(Get_Pointer(), Column_Span, Style_Selector);
}

void Object_Class::Set_Style_Grid_Cell_Row_Position(Coordinate_Type Row_Position, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_grid_cell_row_pos(Get_Pointer(), Row_Position, Style_Selector);
}

void Object_Class::Set_Style_Grid_Cell_Row_Span(Coordinate_Type Row_Span, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_grid_cell_row_span(Get_Pointer(), Row_Span, Style_Selector);
}

void Object_Class::Set_Style_Grid_Cell_X_Alignment(Coordinate_Type X_Alignment, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_grid_cell_x_align(Get_Pointer(), X_Alignment, Style_Selector);
}

void Object_Class::Set_Style_Grid_Cell_Y_Alignment(Coordinate_Type Y_Alignment, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_grid_cell_y_align(Get_Pointer(), Y_Alignment, Style_Selector);
}

void Object_Class::Set_Style_Grid_Column_Alignment(Grid_Alignment_Type Column_Alignment, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_grid_column_align(Get_Pointer(), (lv_grid_align_t)Column_Alignment, Style_Selector);
}

void Object_Class::Set_Style_Grid_Column_Descriptor_Array(const Coordinate_Type *Column_Descriptor_Array, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_grid_column_dsc_array(Get_Pointer(), Column_Descriptor_Array, Style_Selector);
}

void Object_Class::Set_Style_Grid_Row_Alignment(Grid_Alignment_Type Row_Alignment, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_grid_row_align(Get_Pointer(), (lv_grid_align_t)Row_Alignment, Style_Selector);
}

void Object_Class::Set_Style_Grid_Row_Descriptor_Array(const Coordinate_Type *Row_Descriptor_Array, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_grid_row_dsc_array(Get_Pointer(), Row_Descriptor_Array, Style_Selector);
}

void Object_Class::Set_Style_Height(Coordinate_Type Height, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_height(Get_Pointer(), Height, Style_Selector);
}

void Object_Class::Set_Style_Image_Opacity(Opacity_Type Opacity, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_img_opa(Get_Pointer(), static_cast<lv_opa_t>(Opacity), Style_Selector);
}

void Object_Class::Set_Style_Image_Recolor(Color_Type Color, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_img_recolor(Get_Pointer(), Color.Get_LVGL_Color(), Style_Selector);
}

void Object_Class::Set_Style_Image_Recolor_Opacity(Opacity_Type Opacity, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_img_recolor_opa(Get_Pointer(), static_cast<lv_opa_t>(Opacity), Style_Selector);
}

void Object_Class::Set_Style_Layout(uint16_t Value, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_layout(Get_Pointer(), Value, Style_Selector);
}

void Object_Class::Set_Style_Line_Color(Color_Type Color, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_line_color(Get_Pointer(), Color.Get_LVGL_Color(), Style_Selector);
}

void Object_Class::Set_Style_Line_Dash_Gap(Coordinate_Type Dash_Gap, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_line_dash_gap(Get_Pointer(), Dash_Gap, Style_Selector);
}

void Object_Class::Set_Style_Line_Dash_Width(Coordinate_Type Dash_Width, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_line_dash_width(Get_Pointer(), Dash_Width, Style_Selector);
}

void Object_Class::Set_Style_Line_Opacity(Opacity_Type Opacity, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_line_opa(Get_Pointer(), static_cast<lv_opa_t>(Opacity), Style_Selector);
}

void Object_Class::Set_Style_Line_Rounded(bool Rounded, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_line_rounded(Get_Pointer(), Rounded, Style_Selector);
}

void Object_Class::Set_Style_Line_Width(Coordinate_Type Width, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_line_width(Get_Pointer(), Width, Style_Selector);
}

void Object_Class::Set_Style_Maximum_Height(Coordinate_Type Maximum_Height, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_max_height(Get_Pointer(), Maximum_Height, Style_Selector);
}

void Object_Class::Set_Style_Maximum_Width(Coordinate_Type Maximum_Width, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_max_width(Get_Pointer(), Maximum_Width, Style_Selector);
}

void Object_Class::Set_Style_Minimum_Height(Coordinate_Type Minimum_Height, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_min_height(Get_Pointer(), Minimum_Height, Style_Selector);
}

void Object_Class::Set_Style_Minimum_Width(Coordinate_Type Minimum_Width, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_min_width(Get_Pointer(), Minimum_Width, Style_Selector);
}

void Object_Class::Set_Style_Opacity(Opacity_Type Opacity, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_opa(Get_Pointer(), static_cast<lv_opa_t>(Opacity), Style_Selector);
}

void Object_Class::Set_Style_Opacity(uint8_t Opacity, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_opa(Get_Pointer(), Opacity, Style_Selector);
}

void Object_Class::Set_Style_Outline_Color(Color_Type Color, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_outline_color(Get_Pointer(), Color.Get_LVGL_Color(), Style_Selector);
}

void Object_Class::Set_Style_Outline_Opacity(Opacity_Type Opacity, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_outline_opa(Get_Pointer(), static_cast<lv_opa_t>(Opacity), Style_Selector);
}

void Object_Class::Set_Style_Outline_Pad(Coordinate_Type Pad, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_outline_pad(Get_Pointer(), Pad, Style_Selector);
}

void Object_Class::Set_Style_Outline_Width(Coordinate_Type Width, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_outline_width(Get_Pointer(), Width, Style_Selector);
}

void Object_Class::Set_Style_Pad_Bottom(Coordinate_Type Pad_Bottom, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_pad_bottom(Get_Pointer(), Pad_Bottom, Style_Selector);
}

void Object_Class::Set_Style_Pad_Left(Coordinate_Type Pad_Left, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_pad_left(Get_Pointer(), Pad_Left, Style_Selector);
}

void Object_Class::Set_Style_Pad_Right(Coordinate_Type Pad_Right, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_pad_right(Get_Pointer(), Pad_Right, Style_Selector);
}

void Object_Class::Set_Style_Pad_Top(Coordinate_Type Pad_Top, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_pad_top(Get_Pointer(), Pad_Top, Style_Selector);
}

void Object_Class::Set_Style_Pad_Row(Coordinate_Type Pad_Row, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_pad_row(Get_Pointer(), Pad_Row, Style_Selector);
}

void Object_Class::Set_Style_Pad_Column(Coordinate_Type Pad_Column, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_pad_column(Get_Pointer(), Pad_Column, Style_Selector);
}

void Object_Class::Set_Style_Pad_All(Coordinate_Type Pad_All, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_pad_all(Get_Pointer(), Pad_All, Style_Selector);
}

void Object_Class::Set_Style_Radius(Coordinate_Type Radius, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_radius(Get_Pointer(), Radius, Style_Selector);
}

void Object_Class::Set_Style_Shadow_Color(Color_Type Color, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_shadow_color(Get_Pointer(), Color.Get_LVGL_Color(), Style_Selector);
}

void Object_Class::Set_Style_Shadow_Offset_X(Coordinate_Type Offset_X, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_shadow_ofs_x(Get_Pointer(), Offset_X, Style_Selector);
}

void Object_Class::Set_Style_Shadow_Offset_Y(Coordinate_Type Offset_Y, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_shadow_ofs_y(Get_Pointer(), Offset_Y, Style_Selector);
}

void Object_Class::Set_Style_Shadow_Opacity(Opacity_Type Opacity, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_shadow_opa(Get_Pointer(), static_cast<lv_opa_t>(Opacity), Style_Selector);
}

void Object_Class::Set_Style_Shadow_Spread(Coordinate_Type Spread, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_shadow_spread(Get_Pointer(), Spread, Style_Selector);
}

void Object_Class::Set_Style_Shadow_Width(Coordinate_Type Width, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_shadow_width(Get_Pointer(), Width, Style_Selector);
}

void Object_Class::Set_Style_Text_Alignment(Text::Alignment_Type Text_Alignment, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_text_align(Get_Pointer(), Text_Alignment, Style_Selector);
}

void Object_Class::Set_Style_Text_Color(Color_Type Color, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_text_color(Get_Pointer(), Color.Get_LVGL_Color(), Style_Selector);
}

void Object_Class::Set_Style_Text_Decor(Text::Decor_Type Text_Decor, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_text_decor(Get_Pointer(), Text_Decor, Style_Selector);
}

void Object_Class::Set_Style_Text_Font(Font_Type *Font, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_text_font(Get_Pointer(), Font, Style_Selector);
}

void Object_Class::Set_Style_Text_Letter_Space(Coordinate_Type Letter_Space, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_text_letter_space(Get_Pointer(), Letter_Space, Style_Selector);
}

void Object_Class::Set_Style_Text_Line_Space(Coordinate_Type Line_Space, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_text_line_space(Get_Pointer(), Line_Space, Style_Selector);
}

void Object_Class::Set_Style_Text_Opacity(Opacity_Type Opacity, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_text_opa(Get_Pointer(), static_cast<lv_opa_t>(Opacity), Style_Selector);
}

void Object_Class::Set_Style_Transform_Angle(Coordinate_Type Angle, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_transform_angle(Get_Pointer(), Angle, Style_Selector);
}

void Object_Class::Set_Style_Transform_Height(Coordinate_Type Height, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_transform_height(Get_Pointer(), Height, Style_Selector);
}

void Object_Class::Set_Style_Transform_Width(Coordinate_Type Width, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_transform_width(Get_Pointer(), Width, Style_Selector);
}

void Object_Class::Set_Style_Transform_Zoom(Coordinate_Type Zoom, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_transform_zoom(Get_Pointer(), Zoom, Style_Selector);
}

void Object_Class::Set_Style_Transition(const Style_Class::Transition_Descriptor_Type *Transition_Descriptor, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_transition(Get_Pointer(), Transition_Descriptor, Style_Selector);
}

void Object_Class::Set_Style_Translate_X(Coordinate_Type Translate_X, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_translate_x(Get_Pointer(), Translate_X, Style_Selector);
}

void Object_Class::Set_Style_Translate_Y(Coordinate_Type Translate_Y, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_translate_y(Get_Pointer(), Translate_Y, Style_Selector);
}

void Object_Class::Set_Style_Width(Coordinate_Type Width, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_width(Get_Pointer(), Width, Style_Selector);
}

void Object_Class::Set_Style_X(Coordinate_Type X, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_x(Get_Pointer(), X, Style_Selector);
}

void Object_Class::Set_Style_Y(Coordinate_Type Y, Style_Selector_Type Style_Selector)
{
    lv_obj_set_style_y(Get_Pointer(), Y, Style_Selector);
}

void Object_Class::Set_Layout(uint32_t Layout)
{
    lv_obj_set_layout(Get_Pointer(), Layout);
}

void Object_Class::Set_Tile(Object_Class Tile, bool Animation)
{
    if (Animation)
    {
        lv_obj_set_tile(Get_Pointer(), Tile.Get_Pointer(), LV_ANIM_ON);
    }
    else
    {
        lv_obj_set_tile(Get_Pointer(), Tile.Get_Pointer(), LV_ANIM_OFF);
    }
}

void Object_Class::Set_Tile_Identifier(uint16_t Column_Identifier, uint16_t Row_Identifier, bool Animation)
{
    if (Animation)
    {
        lv_obj_set_tile_id(Get_Pointer(), Column_Identifier, Row_Identifier, LV_ANIM_ON);
    }
    else
    {
        lv_obj_set_tile_id(Get_Pointer(), Column_Identifier, Row_Identifier, LV_ANIM_OFF);
    }
}

// ------------------------------------------------------------------------- //
//
//                                    Getters
//
// ------------------------------------------------------------------------- //

void *Object_Class::Get_User_Data()
{
    return lv_obj_get_user_data(Get_Pointer());
}

State_Type Object_Class::Get_State()
{
    return static_cast<State_Type>(lv_obj_get_state(Get_Pointer()));
}

Object_Class Object_Class::Get_Child(uint16_t Index)
{
    Object_Class Child_Object;
    Child_Object.Set_Pointer(lv_obj_get_child(Get_Pointer(), Index));
    return Child_Object;
}

/// @brief Get the number of children objects of an object.
/// @return The number of children objects.
uint32_t Object_Class::Get_Child_Count()
{
    return lv_obj_get_child_cnt(Get_Pointer());
}

Object_Class Object_Class::Get_Parent()
{
    Object_Class Parent_Object;
    Parent_Object.Set_Pointer(lv_obj_get_parent(Get_Pointer()));
    return Parent_Object;
}

void *Object_Class::Get_Group()
{
    return lv_obj_get_group(Get_Pointer());
}

const Object_Class::Class_Type *Object_Class::Get_Class()
{
    return lv_obj_get_class(Get_Pointer());
}

inline bool Object_Class::Get_Object_Visibility()
{
    return lv_obj_is_visible(Get_Pointer());
}

// -- Style

Alignment_Type Object_Class::Get_Style_Alignment(Part_Type Part)
{
    return (Alignment_Type)lv_obj_get_style_align(Get_Pointer(), static_cast<uint32_t>(Part));
}

/*
const Animation_Class Object_Class::Get_Style_Animation(Part_Type Part)
{
    Animation_Class Animation;
    Animation.Set_Pointer(lv_obj_get_style_anim(Get_Pointer(), Part));
    return Animation;

}*/

uint32_t Object_Class::Get_Style_Animation_Time(Part_Type Part)
{
    return lv_obj_get_style_anim_time(Get_Pointer(), static_cast<uint32_t>(Part));
}

uint32_t Object_Class::Get_Style_Animation_Speed(Part_Type Part)
{
    return lv_obj_get_style_anim_speed(Get_Pointer(), static_cast<uint32_t>(Part));
}

Object_Class::Color_Type Object_Class::Get_Style_Arc_Color(Part_Type Part)
{
    Color_Type Color(lv_obj_get_style_arc_color(Get_Pointer(), static_cast<uint32_t>(Part)));
    return Color;
}

Object_Class::Color_Type Object_Class::Get_Style_Arc_Color_Filtered(Part_Type Part)
{
    Color_Type Color(lv_obj_get_style_arc_color_filtered(Get_Pointer(), static_cast<uint32_t>(Part)));
    return Color;
}

const void *Object_Class::Get_Style_Arc_Image_Source(Part_Type Part)
{
    return lv_obj_get_style_arc_img_src(Get_Pointer(), static_cast<uint32_t>(Part));
}

Opacity_Type Object_Class::Get_Style_Arc_Opacity(Part_Type Part)
{
    return static_cast<Opacity_Type>(lv_obj_get_style_arc_opa(Get_Pointer(), static_cast<uint32_t>(Part)));
}

bool Object_Class::Get_Style_Arc_Rounded(Part_Type Part)
{
    return lv_obj_get_style_arc_rounded(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Arc_Width(Part_Type Part)
{
    return lv_obj_get_style_arc_width(Get_Pointer(), static_cast<uint32_t>(Part));
}

Object_Class::Base_Direction::Type Object_Class::Get_Style_Base_Direction(Part_Type Part)
{
    return lv_obj_get_style_base_dir(Get_Pointer(), static_cast<uint32_t>(Part));
}

Object_Class::Color_Type Object_Class::Get_Style_Background_Color(Part_Type Part)
{
    return lv_obj_get_style_bg_color(Get_Pointer(), static_cast<uint32_t>(Part));
}

Object_Class::Color_Type Object_Class::Get_Style_Background_Color_Filtered(Part_Type Part)
{
    return lv_obj_get_style_bg_color_filtered(Get_Pointer(), static_cast<uint32_t>(Part));
}

Opacity_Type Object_Class::Get_Style_Background_Opacity(Part_Type Part)
{
    return (Opacity_Type)lv_obj_get_style_bg_opa(Get_Pointer(), static_cast<uint32_t>(Part));
}

Object_Class::Color_Type Object_Class::Get_Style_Background_Gradient_Color(Part_Type Part)
{
    return lv_obj_get_style_bg_grad_color(Get_Pointer(), static_cast<uint32_t>(Part));
}

Object_Class::Color_Type Object_Class::Get_Style_Background_Gradient_Color_Filtered(Part_Type Part)
{
    return lv_obj_get_style_bg_grad_color_filtered(Get_Pointer(), static_cast<uint32_t>(Part));
}

Object_Class::Gradient::Direction_Type Object_Class::Get_Style_Background_Gradient_Direction(Part_Type Part)
{
    return lv_obj_get_style_bg_grad_dir(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Background_Main_Stop(Part_Type Part)
{
    return lv_obj_get_style_bg_main_stop(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Background_Gradient_Stop(Part_Type Part)
{
    return lv_obj_get_style_bg_grad_stop(Get_Pointer(), static_cast<uint32_t>(Part));
}

const Object_Class::Gradient::Descriptor_Type *Object_Class::Get_Style_Background_Gradient(Part_Type Part)
{
    return lv_obj_get_style_bg_grad(Get_Pointer(), static_cast<uint32_t>(Part));
}

Object_Class::Dither_Mode::Type Object_Class::Get_Style_Background_Dither_Mode(Part_Type Part)
{
    return lv_obj_get_style_bg_dither_mode(Get_Pointer(), static_cast<uint32_t>(Part));
}

const void *Object_Class::Get_Style_Background_Image_Source(Part_Type Part)
{
    return lv_obj_get_style_bg_img_src(Get_Pointer(), static_cast<uint32_t>(Part));
}

Opacity_Type Object_Class::Get_Style_Background_Image_Opacity(Part_Type Part)
{
    return static_cast<Opacity_Type>(lv_obj_get_style_bg_img_opa(Get_Pointer(), static_cast<uint32_t>(Part)));
}

Object_Class::Color_Type Object_Class::Get_Style_Background_Image_Recolor(Part_Type Part)
{
    return lv_obj_get_style_bg_img_recolor(Get_Pointer(), static_cast<uint32_t>(Part));
}

Object_Class::Color_Type Object_Class::Get_Style_Background_Image_Recolor_Filtered(Part_Type Part)
{
    return lv_obj_get_style_bg_img_recolor_filtered(Get_Pointer(), static_cast<uint32_t>(Part));
}

Opacity_Type Object_Class::Get_Style_Background_Image_Recolor_Opacity(Part_Type Part)
{
    return static_cast<Opacity_Type>(lv_obj_get_style_bg_img_recolor_opa(Get_Pointer(), static_cast<uint32_t>(Part)));
}

bool Object_Class::Get_Style_Background_Image_Tiled(Part_Type Part)
{
    return lv_obj_get_style_bg_img_tiled(Get_Pointer(), static_cast<uint32_t>(Part));
}

Object_Class::Blend::Mode_Type Object_Class::Get_Style_Blend_Mode(Part_Type Part)
{
    return lv_obj_get_style_blend_mode(Get_Pointer(), static_cast<uint32_t>(Part));
}

Object_Class::Color_Type Object_Class::Get_Style_Border_Color(Part_Type Part)
{
    return lv_obj_get_style_border_color(Get_Pointer(), static_cast<uint32_t>(Part));
}

Object_Class::Color_Type Object_Class::Get_Style_Border_Color_Filtered(Part_Type Part)
{
    return lv_obj_get_style_border_color_filtered(Get_Pointer(), static_cast<uint32_t>(Part));
}

Opacity_Type Object_Class::Get_Style_Border_Opacity(Part_Type Part)
{
    return static_cast<Opacity_Type>(lv_obj_get_style_border_opa(Get_Pointer(), static_cast<uint32_t>(Part)));
}

Coordinate_Type Object_Class::Get_Style_Border_Width(Part_Type Part)
{
    return lv_obj_get_style_border_width(Get_Pointer(), static_cast<uint32_t>(Part));
}

Border_Side_Type Object_Class::Get_Style_Border_Side(Part_Type Part)
{
    return static_cast<Border_Side_Type>(lv_obj_get_style_border_side(Get_Pointer(), static_cast<uint32_t>(Part)));
}

bool Object_Class::Get_Style_Border_Post(Part_Type Part)
{
    return lv_obj_get_style_border_post(Get_Pointer(), static_cast<uint32_t>(Part));
}

bool Object_Class::Get_Style_Clip_Corner(Part_Type Part)
{
    return lv_obj_get_style_clip_corner(Get_Pointer(), static_cast<uint32_t>(Part));
}

const Object_Class::Color_Filter_Descriptor_Type Object_Class::Get_Style_Color_Filter_Descriptor(Part_Type Part)
{
    Color_Filter_Descriptor_Type Color_Filter_Descriptor;
    *Color_Filter_Descriptor.Get_Pointer() = *lv_obj_get_style_color_filter_dsc(Get_Pointer(), static_cast<uint32_t>(Part));
    return Color_Filter_Descriptor;
}

Opacity_Type Object_Class::Get_Style_Color_Filter_Opacity(Part_Type Part)
{
    return static_cast<Opacity_Type>(lv_obj_get_style_color_filter_opa(Get_Pointer(), static_cast<uint32_t>(Part)));
}

Flex_Flow_Type Object_Class::Get_Style_Flex_Flow(Part_Type Part)
{
    return (Flex_Flow_Type)lv_obj_get_style_flex_flow(Get_Pointer(), static_cast<uint32_t>(Part));
}

Flex_Alignment_Type Object_Class::Get_Style_Flex_Main_Place(Part_Type Part)
{
    return (Flex_Alignment_Type)lv_obj_get_style_flex_main_place(Get_Pointer(), static_cast<uint32_t>(Part));
}

Flex_Alignment_Type Object_Class::Get_Style_Flex_Cross_Place(Part_Type Part)
{
    return (Flex_Alignment_Type)lv_obj_get_style_flex_cross_place(Get_Pointer(), static_cast<uint32_t>(Part));
}

Flex_Alignment_Type Object_Class::Get_Style_Flex_Track_Place(Part_Type Part)
{
    return (Flex_Alignment_Type)lv_obj_get_style_flex_track_place(Get_Pointer(), static_cast<uint32_t>(Part));
}

uint8_t Object_Class::Get_Style_Flex_Grow(Part_Type Part)
{
    return lv_obj_get_style_flex_grow(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Grid_Cell_Column_Position(Part_Type Part)
{
    return lv_obj_get_style_grid_cell_column_pos(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Grid_Cell_Column_Span(Part_Type Part)
{
    return lv_obj_get_style_grid_cell_column_span(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Grid_Cell_Row_Position(Part_Type Part)
{
    return lv_obj_get_style_grid_cell_row_pos(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Grid_Cell_Row_Span(Part_Type Part)
{
    return lv_obj_get_style_grid_cell_row_span(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Grid_Cell_X_Alignment(Part_Type Part)
{
    return lv_obj_get_style_grid_cell_x_align(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Grid_Cell_Y_Alignment(Part_Type Part)
{
    return lv_obj_get_style_grid_cell_y_align(Get_Pointer(), static_cast<uint32_t>(Part));
}

Grid_Alignment_Type Object_Class::Get_Style_Grid_Column_Alignment(Part_Type Part)
{
    return (Grid_Alignment_Type)lv_obj_get_style_grid_column_align(Get_Pointer(), static_cast<uint32_t>(Part));
}

const Coordinate_Type *Object_Class::Get_Style_Grid_Column_Descriptor_Array(Part_Type Part)
{
    return lv_obj_get_style_grid_column_dsc_array(Get_Pointer(), static_cast<uint32_t>(Part));
}

Grid_Alignment_Type Object_Class::Get_Style_Grid_Row_Alignment(Part_Type Part)
{
    return (Grid_Alignment_Type)lv_obj_get_style_grid_row_align(Get_Pointer(), static_cast<uint32_t>(Part));
}

const Coordinate_Type *Object_Class::Get_Style_Grid_Row_Descriptor_Array(Part_Type Part)
{
    return lv_obj_get_style_grid_row_dsc_array(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Height(Part_Type Part)
{
    return lv_obj_get_style_height(Get_Pointer(), static_cast<uint32_t>(Part));
}

Opacity_Type Object_Class::Get_Style_Image_Opacity(Part_Type Part)
{
    return static_cast<Opacity_Type>(lv_obj_get_style_img_opa(Get_Pointer(), static_cast<uint32_t>(Part)));
}

Object_Class::Color_Type Object_Class::Get_Style_Image_Recolor(Part_Type Part)
{
    return lv_obj_get_style_img_recolor(Get_Pointer(), static_cast<uint32_t>(Part));
}

Object_Class::Color_Type Object_Class::Get_Style_Image_Recolor_Filtered(Part_Type Part)
{
    return lv_obj_get_style_img_recolor_filtered(Get_Pointer(), static_cast<uint32_t>(Part));
}

Opacity_Type Object_Class::Get_Style_Image_Recolor_Opacity(Part_Type Part)
{
    return static_cast<Opacity_Type>(lv_obj_get_style_img_recolor_opa(Get_Pointer(), static_cast<uint32_t>(Part)));
}

uint16_t Object_Class::Get_Style_Layout(Part_Type Part)
{
    return lv_obj_get_style_layout(Get_Pointer(), static_cast<uint32_t>(Part));
}

// - - - Line

Object_Class::Color_Type Object_Class::Get_Style_Line_Color(Part_Type Part)
{
    return lv_obj_get_style_line_color(Get_Pointer(), static_cast<uint32_t>(Part));
}

Object_Class::Color_Type Object_Class::Get_Style_Line_Color_Filtered(Part_Type Part)
{
    return lv_obj_get_style_line_color_filtered(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Line_Dash_Gap(Part_Type Part)
{
    return lv_obj_get_style_line_dash_gap(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Line_Dash_Width(Part_Type Part)
{
    return lv_obj_get_style_line_dash_width(Get_Pointer(), static_cast<uint32_t>(Part));
}

Opacity_Type Object_Class::Get_Style_Line_Opacity(Part_Type Part)
{
    return static_cast<Opacity_Type>(lv_obj_get_style_line_opa(Get_Pointer(), static_cast<uint32_t>(Part)));
}

bool Object_Class::Get_Style_Line_Rounded(Part_Type Part)
{
    return lv_obj_get_style_line_rounded(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Line_Width(Part_Type Part)
{
    return lv_obj_get_style_line_width(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Maximum_Height(Part_Type Part)
{
    return lv_obj_get_style_max_height(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Maximum_Width(Part_Type Part)
{
    return lv_obj_get_style_max_width(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Minimum_Height(Part_Type Part)
{
    return lv_obj_get_style_min_height(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Minimum_Width(Part_Type Part)
{
    return lv_obj_get_style_min_width(Get_Pointer(), static_cast<uint32_t>(Part));
}

Object_Class::Color_Type Object_Class::Get_Style_Outline_Color(Part_Type Part)
{
    return lv_obj_get_style_outline_color(Get_Pointer(), static_cast<uint32_t>(Part));
}

Object_Class::Color_Type Object_Class::Get_Style_Outline_Color_Filtered(Part_Type Part)
{
    return lv_obj_get_style_outline_color_filtered(Get_Pointer(), static_cast<uint32_t>(Part));
}

Opacity_Type Object_Class::Get_Style_Outline_Opacity(Part_Type Part)
{
    return static_cast<Opacity_Type>(lv_obj_get_style_outline_opa(Get_Pointer(), static_cast<uint32_t>(Part)));
}

Coordinate_Type Object_Class::Get_Style_Outline_Pad(Part_Type Part)
{
    return lv_obj_get_style_outline_pad(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Outline_Width(Part_Type Part)
{
    return lv_obj_get_style_outline_width(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Pad_Bottom(Part_Type Part)
{
    return lv_obj_get_style_pad_bottom(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Pad_Top(Part_Type Part)
{
    return lv_obj_get_style_pad_top(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Pad_Left(Part_Type Part)
{
    return lv_obj_get_style_pad_left(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Pad_Right(Part_Type Part)
{
    return lv_obj_get_style_pad_right(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Pad_Row(Part_Type Part)
{
    return lv_obj_get_style_pad_row(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Pad_Column(Part_Type Part)
{
    return lv_obj_get_style_pad_column(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Radius(Part_Type Part)
{
    return lv_obj_get_style_radius(Get_Pointer(), static_cast<uint32_t>(Part));
}

Object_Class::Color_Type Object_Class::Get_Style_Shadow_Color(Part_Type Part)
{
    return lv_obj_get_style_shadow_color(Get_Pointer(), static_cast<uint32_t>(Part));
}

Object_Class::Color_Type Object_Class::Get_Style_Shadow_Color_Filtered(Part_Type Part)
{
    return lv_obj_get_style_shadow_color_filtered(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Shadow_Offset_X(Part_Type Part)
{
    return lv_obj_get_style_shadow_ofs_x(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Shadow_Offset_Y(Part_Type Part)
{
    return lv_obj_get_style_shadow_ofs_y(Get_Pointer(), static_cast<uint32_t>(Part));
}

Opacity_Type Object_Class::Get_Style_Shadow_Opacity(Part_Type Part)
{
    return static_cast<Opacity_Type>(lv_obj_get_style_shadow_opa(Get_Pointer(), static_cast<uint32_t>(Part)));
}

Coordinate_Type Object_Class::Get_Style_Shadow_Spread(Part_Type Part)
{
    return lv_obj_get_style_shadow_spread(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Shadow_Width(Part_Type Part)
{
    return lv_obj_get_style_shadow_width(Get_Pointer(), static_cast<uint32_t>(Part));
}

Object_Class::Text::Alignment_Type Object_Class::Get_Style_Text_Alignment(Part_Type Part)
{
    return lv_obj_get_style_text_align(Get_Pointer(), static_cast<uint32_t>(Part));
}

Object_Class::Color_Type Object_Class::Get_Style_Text_Color(Part_Type Part)
{
    return lv_obj_get_style_text_color(Get_Pointer(), static_cast<uint32_t>(Part));
}

Object_Class::Color_Type Object_Class::Get_Style_Text_Color_Filtered(Part_Type Part)
{
    return lv_obj_get_style_text_color_filtered(Get_Pointer(), static_cast<uint32_t>(Part));
}

Object_Class::Text::Decor_Type Object_Class::Get_Style_Text_Decor(Part_Type Part)
{
    return lv_obj_get_style_text_decor(Get_Pointer(), static_cast<uint32_t>(Part));
}

const Object_Class::Font_Type *Object_Class::Get_Style_Text_Font(Part_Type Part)
{
    return lv_obj_get_style_text_font(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Text_Letter_Space(Part_Type Part)
{
    return lv_obj_get_style_text_letter_space(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Text_Line_Space(Part_Type Part)
{
    return lv_obj_get_style_text_line_space(Get_Pointer(), static_cast<uint32_t>(Part));
}

Opacity_Type Object_Class::Get_Style_Text_Opacity(Part_Type Part)
{
    return static_cast<Opacity_Type>(lv_obj_get_style_text_opa(Get_Pointer(), static_cast<uint32_t>(Part)));
}

Coordinate_Type Object_Class::Get_Style_Transform_Angle(Part_Type Part)
{
    return lv_obj_get_style_transform_angle(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Transform_Height(Part_Type Part)
{
    return lv_obj_get_style_transform_height(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Transform_Width(Part_Type Part)
{
    return lv_obj_get_style_transform_width(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Transform_Zoom(Part_Type Part)
{
    return lv_obj_get_style_transform_zoom(Get_Pointer(), static_cast<uint32_t>(Part));
}

const Style_Class::Transition_Descriptor_Type *Object_Class::Get_Style_Transition(Part_Type Part)
{
    return lv_obj_get_style_transition(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Translate_X(Part_Type Part)
{
    return lv_obj_get_style_translate_x(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Translate_Y(Part_Type Part)
{
    return lv_obj_get_style_translate_y(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Width(Part_Type Part)
{
    return lv_obj_get_style_width(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_X(Part_Type Part)
{
    return lv_obj_get_style_x(Get_Pointer(), static_cast<uint32_t>(Part));
}

Coordinate_Type Object_Class::Get_Style_Y(Part_Type Part)
{
    return lv_obj_get_style_y(Get_Pointer(), static_cast<uint32_t>(Part));
}