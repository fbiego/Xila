///
/// @file Object.hpp
/// @author Alix ANNERAUD (alix.anneraud@outlook.fr)
/// @brief
/// @version 0.1
/// @date 04-05-2022
///
/// @copyright Copyright (c) 2022
///

#include "Arduino.h"

#include "lvgl.h"

#include "Area.hpp"
#include "Animation.hpp"
#include "Style.hpp"

#include "Types.hpp"

#include "../Module/Module.hpp"

#ifndef Object_Hpp_Included
#define Object_Hpp_Included

#define Percentage(x) LV_PCT(x)

namespace Xila_Namespace
{

    /// @brief Object class.
    typedef class Object_Class : public Types_Class
    {
    public:
        // - Types

        /// @brief Point type.
        typedef lv_point_t Point_Type;

        /// @brief Style type.
        typedef Style_Class Style_Type;

        /// @brief Color type.
        typedef Color_Class Color_Type;

        /// @brief Color descriptor type.
        typedef Color_Filter_Descriptor_Class Color_Filter_Descriptor_Type;

        // -- Types and enumerations

     
        typedef lv_obj_class_t Class_Type;
        typedef lv_style_selector_t Style_Selector_Type;

        class Draw_Part
        {
        public:
            /// @brief Event enumeration.
            // TODO : Don't forget to convert the draw part to LVGL draw part (no auto-cast).
            typedef enum Enumeration
            {
                Rectangle = LV_OBJ_DRAW_PART_RECTANGLE,
                Border_Post = LV_OBJ_DRAW_PART_BORDER_POST,
                Scrollbar = LV_OBJ_DRAW_PART_SCROLLBAR
            } Type;
        };

        // -- Methods

        // -- Constructors

        Object_Class();
        Object_Class(lv_obj_t *Object_Pointer);
        Object_Class(const Object_Class &Object_To_Copy);

        // - - Operators

        bool operator==(const Object_Class &Other) const
        {
            if (this->Get_Pointer() == nullptr || Other.Get_Pointer() == nullptr)
            {
                return false;
            }
            return (this->Get_Pointer() == Other.Get_Pointer());
        }

        bool operator!=(const Object_Class &Other) const
        {
            if (this->Get_Pointer() == nullptr || Other.Get_Pointer() == nullptr)
            {
                return true;
            }
            return (this->Get_Pointer() != Other.Get_Pointer());
        }

        // - Management
        virtual void Create(Object_Class Parent_Object);
        void Delete();
        void Clean();
        // - - Flags
        void Add_Flag(Flag_Type Flag);
        void Clear_Flag(Flag_Type Flag);
        bool Has_Flag(Flag_Type Flag);
        bool Has_Any_Flag(Flag_Type Flag);
        // - - States
        void Add_State(Graphics_Types::State_Type State);
        void Clear_State(Graphics_Types::State_Type State);
        bool Has_State(Graphics_Types::State_Type State);
        // - - Swap
        void Swap(Object_Class Object_To_Swap_With);
        // - - Events
        void Add_Event(Module_Class *Module_Pointer, Graphics_Types::Event_Code_Type Event_Code);
        bool Remove_Event(Module_Class *Module_Pointer);
        bool Remove_All_Events();
        void Send_Event(Graphics_Types::Event_Code_Type Event);
        // - - Layer management
        void Move_Foreground();
        void Move_Background();

        void Allocate_Special_Data();

        bool Check_Type(const Class_Type *Class);
        bool Has_Class(const Class_Type *Class);

        bool Is_Valid();

        void Add_Style(Style_Type &Style, Style_Selector_Type Style_Selector);

        Coordinate_Type DPX(Coordinate_Type Coordinate);

        // - - Setters methods.
        void Set_Index(uint32_t Index);
        void Set_Parent(Object_Class Parent_Object);

        void Set_User_Data(void *User_Data);

        // - - - Position and size.
        void Set_Position_X(Coordinate_Type X);
        void Set_Position_Y(Coordinate_Type Y);
        void Set_Position(Coordinate_Type X, Coordinate_Type Y);
        void Set_Alignment(Alignment_Type Alignment);                                                                                        // -- Relative to parent object.
        void Set_Alignment(Alignment_Type Alignment, Coordinate_Type X_Offset, Coordinate_Type Y_Offset);                                    // -- Relative to parent object.
        void Set_Alignment(Object_Class Object_To_Align_With, Alignment_Type Alignment, Coordinate_Type X_Offset, Coordinate_Type Y_Offset); // -- Relative to parent object.
        void Set_Size(Coordinate_Type Width, Coordinate_Type Height);
        void Set_Width(Coordinate_Type Width);
        void Set_Height(Coordinate_Type Height);

        void Set_Content_Height(Coordinate_Type Content_Height);

        // - - - Flex
        void Set_Flex_Alignment(Flex_Alignment_Type Main_Place_Alignment, Flex_Alignment_Type Cross_Place_Alignment, Flex_Alignment_Type Track_Cross_Place);
        void Set_Flex_Flow(Flex_Flow_Type Flow);
        void Set_Flex_Grow(uint8_t Grow);

        // - - - Grid
        void Set_Grid_Alignment(Grid_Alignment_Type Column_Alignment, Grid_Alignment_Type Row_Alignment);
        void Set_Grid_Cell(Grid_Alignment_Type Column_Alignment, uint8_t Column_Position, uint8_t Column_Span, Grid_Alignment_Type Row_Alignment, uint8_t Row_Position, uint8_t Row_Span);
        void Set_Grid_Descriptor_Array(const Coordinate_Type *Column_Descriptor, const Coordinate_Type *Row_Descriptor);

        // - - - Style

        // - - - - Position
        void Set_Style_Alignment(Alignment_Type Alignment, Style_Selector_Type Style_Selector);

        // - - - - Animation
        void Set_Style_Animation_Speed(uint32_t Value, Style_Selector_Type Style_Selector);
        void Set_Style_Animation_Time(uint32_t Value, Style_Selector_Type Style_Selector);

        // - - - - Arc
        void Set_Style_Arc_Color(Color_Type Color, Style_Selector_Type Style_Selector);
        void Set_Style_Arc_Image_Source(const void *Image_Source, Style_Selector_Type Style_Selector);
        void Set_Style_Arc_Opacity(Opacity_Type Opacity, Style_Selector_Type Style_Selector);
        void Set_Style_Arc_Rounded(bool Rounded, Style_Selector_Type Style_Selector);
        void Set_Style_Arc_Width(Coordinate_Type Width, Style_Selector_Type Style_Selector);

        void Set_Style_Base_Direction(Direction_Type Direction, Style_Selector_Type Style_Selector);

        // - - - - Background
        void Set_Style_Background_Color(Color_Type Color, Style_Selector_Type Style_Selector);
        void Set_Style_Background_Dither_Mode(Dither_Mode::Type Dither_Mode, Style_Selector_Type Style_Selector);
        void Set_Style_Background_Gradient(const Gradient::Descriptor_Type *Gradient_Descriptor, Style_Selector_Type Style_Selector);
        void Set_Style_Background_Gradient_Color(Color_Type Color, Style_Selector_Type Style_Selector);
        void Set_Style_Background_Gradient_Direction(Gradient::Direction_Type Gradient_Direction, Style_Selector_Type Style_Selector);
        void Set_Style_Background_Gradient_Stop(Coordinate_Type Value, Style_Selector_Type Style_Selector);
        void Set_Style_Background_Image_Opacity(Opacity_Type Opacity, Style_Selector_Type Style_Selector);
        void Set_Style_Background_Image_Recolor(Color_Type Color, Style_Selector_Type Style_Selector);
        void Set_Style_Background_Image_Recolor_Opacity(Opacity_Type Opacity, Style_Selector_Type Style_Selector);
        void Set_Style_Background_Image_Source(const void *Image_Source, Style_Selector_Type Style_Selector);
        void Set_Style_Background_Image_Tiled(bool Tiled, Style_Selector_Type Style_Selector);
        void Set_Style_Background_Main_Stop(Coordinate_Type Value, Style_Selector_Type Style_Selector);
        void Set_Style_Background_Opacity(Opacity_Type Opacity, Style_Selector_Type Style_Selector);

        // - - - - Blend
        void Set_Style_Blend_Mode(Blend::Mode_Type Blend_Mode, Style_Selector_Type Style_Selector);

        // - - - - Border
        void Set_Style_Border_Color(Color_Type Color, Style_Selector_Type Style_Selector);
        void Set_Style_Border_Opacity(Opacity_Type Opacity, Style_Selector_Type Style_Selector);
        void Set_Style_Border_Post(bool Value, Style_Selector_Type Style_Selector);
        void Set_Style_Border_Side(Border_Side_Type Border_Side, Style_Selector_Type Style_Selector);
        void Set_Style_Border_Width(Coordinate_Type Width, Style_Selector_Type Style_Selector);

        void Set_Style_Clip_Corner(bool Value, Style_Selector_Type Style_Selector);

        // - - - - Color filter
        void Set_Style_Color_Filter_Descriptor(Color_Filter_Descriptor_Class &Color_Filter_Descriptor, Style_Selector_Type Style_Selector);
        void Set_Style_Color_Filter_Opacity(Opacity_Type Opacity, Style_Selector_Type Style_Selector);

        // - - - - Flex
        void Set_Style_Flex_Cross_Place(Flex_Alignment_Type Cross_Place, Style_Selector_Type Style_Selector);
        void Set_Style_Flex_Flow(Flex_Flow_Type Flow, Style_Selector_Type Style_Selector);
        void Set_Style_Flex_Grow(uint8_t Value, Style_Selector_Type Style_Selector);
        void Set_Style_Flex_Main_Place(Flex_Alignment_Type Main_Place, Style_Selector_Type Style_Selector);
        void Set_Style_Flex_Track_Place(Flex_Alignment_Type Track_Place, Style_Selector_Type Style_Selector);

        // - - - - Grid
        void Set_Style_Grid_Cell_Column_Position(Coordinate_Type Column_Position, Style_Selector_Type Style_Selector);
        void Set_Style_Grid_Cell_Column_Span(Coordinate_Type Column_Span, Style_Selector_Type Style_Selector);
        void Set_Style_Grid_Cell_Row_Position(Coordinate_Type Row_Position, Style_Selector_Type Style_Selector);
        void Set_Style_Grid_Cell_Row_Span(Coordinate_Type Row_Span, Style_Selector_Type Style_Selector);
        void Set_Style_Grid_Cell_X_Alignment(Coordinate_Type X_Alignment, Style_Selector_Type Style_Selector);
        void Set_Style_Grid_Cell_Y_Alignment(Coordinate_Type Y_Alignment, Style_Selector_Type Style_Selector);
        void Set_Style_Grid_Column_Alignment(Grid_Alignment_Type Column_Alignment, Style_Selector_Type Style_Selector);
        void Set_Style_Grid_Column_Descriptor_Array(const Coordinate_Type *Column_Descriptor_Array, Style_Selector_Type Style_Selector);
        void Set_Style_Grid_Row_Alignment(Grid_Alignment_Type Row_Alignment, Style_Selector_Type Style_Selector);
        void Set_Style_Grid_Row_Descriptor_Array(const Coordinate_Type *Row_Descriptor_Array, Style_Selector_Type Style_Selector);

        void Set_Style_Height(Coordinate_Type Height, Style_Selector_Type Style_Selector);

        // - - - - Image
        void Set_Style_Image_Opacity(Opacity_Type Opacity, Style_Selector_Type Style_Selector);
        void Set_Style_Image_Recolor(Color_Type Color, Style_Selector_Type Style_Selector);
        void Set_Style_Image_Recolor_Opacity(Opacity_Type Opacity, Style_Selector_Type Style_Selector);

        void Set_Style_Layout(uint16_t Value, Style_Selector_Type Style_Selector);

        // - - - - Line
        void Set_Style_Line_Color(Color_Type Color, Style_Selector_Type Style_Selector);
        void Set_Style_Line_Dash_Gap(Coordinate_Type Dash_Gap, Style_Selector_Type Style_Selector);
        void Set_Style_Line_Dash_Width(Coordinate_Type Dash_Width, Style_Selector_Type Style_Selector);
        void Set_Style_Line_Opacity(Opacity_Type Opacity, Style_Selector_Type Style_Selector);
        void Set_Style_Line_Rounded(bool Rounded, Style_Selector_Type Style_Selector);
        void Set_Style_Line_Width(Coordinate_Type Width, Style_Selector_Type Style_Selector);

        void Set_Style_Maximum_Height(Coordinate_Type Maximum_Height, Style_Selector_Type Style_Selector);
        void Set_Style_Maximum_Width(Coordinate_Type Maximum_Width, Style_Selector_Type Style_Selector);
        void Set_Style_Minimum_Height(Coordinate_Type Minimum_Height, Style_Selector_Type Style_Selector);
        void Set_Style_Minimum_Width(Coordinate_Type Minimum_Width, Style_Selector_Type Style_Selector);
        void Set_Style_Opacity(Opacity_Type Opacity, Style_Selector_Type Style_Selector);
        void Set_Style_Opacity(uint8_t Opacity, Style_Selector_Type Style_Selector);

        // - - - - Outline
        void Set_Style_Outline_Color(Color_Type Color, Style_Selector_Type Style_Selector);
        void Set_Style_Outline_Opacity(Opacity_Type Opacity, Style_Selector_Type Style_Selector);
        void Set_Style_Outline_Pad(Coordinate_Type Pad, Style_Selector_Type Style_Selector);
        void Set_Style_Outline_Width(Coordinate_Type Width, Style_Selector_Type Style_Selector);

        // - - - - Pad
        void Set_Style_Pad_Bottom(Coordinate_Type Pad_Bottom, Style_Selector_Type Style_Selector);
        void Set_Style_Pad_Left(Coordinate_Type Pad_Left, Style_Selector_Type Style_Selector);
        void Set_Style_Pad_Right(Coordinate_Type Pad_Right, Style_Selector_Type Style_Selector);
        void Set_Style_Pad_Top(Coordinate_Type Pad_Top, Style_Selector_Type Style_Selector);
        void Set_Style_Pad_Row(Coordinate_Type Pad_Row, Style_Selector_Type Style_Selector);
        void Set_Style_Pad_Column(Coordinate_Type Pad_Column, Style_Selector_Type Style_Selector);
        void Set_Style_Pad_All(Coordinate_Type Pad_All, Style_Selector_Type Style_Selector);

        void Set_Style_Radius(Coordinate_Type Radius, Style_Selector_Type Style_Selector);

        // - - - - Shadow
        void Set_Style_Shadow_Color(Color_Type Color, Style_Selector_Type Style_Selector);
        void Set_Style_Shadow_Offset_X(Coordinate_Type Offset_X, Style_Selector_Type Style_Selector);
        void Set_Style_Shadow_Offset_Y(Coordinate_Type Offset_Y, Style_Selector_Type Style_Selector);
        void Set_Style_Shadow_Opacity(Opacity_Type Opacity, Style_Selector_Type Style_Selector);
        void Set_Style_Shadow_Spread(Coordinate_Type Spread, Style_Selector_Type Style_Selector);
        void Set_Style_Shadow_Width(Coordinate_Type Width, Style_Selector_Type Style_Selector);

        // - - - - Text
        void Set_Style_Text_Alignment(Text::Alignment_Type Alignment, Style_Selector_Type Style_Selector);
        void Set_Style_Text_Color(Color_Type Color, Style_Selector_Type Style_Selector);
        void Set_Style_Text_Decor(Text::Decor_Type Decor, Style_Selector_Type Style_Selector);
        void Set_Style_Text_Font(Font_Type *Font, Style_Selector_Type Style_Selector);
        void Set_Style_Text_Letter_Space(Coordinate_Type Letter_Space, Style_Selector_Type Style_Selector);
        void Set_Style_Text_Line_Space(Coordinate_Type Line_Space, Style_Selector_Type Style_Selector);
        void Set_Style_Text_Opacity(Opacity_Type Opacity, Style_Selector_Type Style_Selector);

        // - - - - Transform
        void Set_Style_Transform_Angle(Coordinate_Type Angle, Style_Selector_Type Style_Selector);
        void Set_Style_Transform_Height(Coordinate_Type Height, Style_Selector_Type Style_Selector);
        void Set_Style_Transform_Width(Coordinate_Type Width, Style_Selector_Type Style_Selector);
        void Set_Style_Transform_Zoom(Coordinate_Type Zoom, Style_Selector_Type Style_Selector);

        void Set_Style_Transition(const Style_Class::Transition_Descriptor_Type *Transition_Descriptor, Style_Selector_Type Style_Selector);

        void Set_Style_Translate_X(Coordinate_Type Translate_X, Style_Selector_Type Style_Selector);
        void Set_Style_Translate_Y(Coordinate_Type Translate_Y, Style_Selector_Type Style_Selector);

        void Set_Style_Width(Coordinate_Type Width, Style_Selector_Type Style_Selector);

        void Set_Style_X(Coordinate_Type Style_X, Style_Selector_Type Style_Selector);
        void Set_Style_Y(Coordinate_Type Style_Y, Style_Selector_Type Style_Selector);

        void Set_Layout(uint32_t Layout);

        // - - - Tile
        void Set_Tile(Object_Class Tile, bool Animation);
        void Set_Tile_Identifier(uint16_t Column_Identifier, uint16_t Row_Identifier, bool Animation);

        // - - Getters values.

        const Class_Type *Get_Class();

        Graphics_Types::State_Type Get_State();

        void *Get_Group();
        void *Get_User_Data();

        bool Get_Object_Visibility();

        // Family relationship.
        Object_Class Get_Parent();
        uint32_t Get_Child_Count();
        Object_Class Get_Child(uint16_t Index);

        // - - Style

        Alignment_Type Get_Style_Alignment(Part_Type Part);
        // - - - Animation
        // const Animation_Class Get_Style_Animation(Part_Type Part);
        uint32_t Get_Style_Animation_Time(Part_Type Part);
        uint32_t Get_Style_Animation_Speed(Part_Type Part);
        // - - - Arc
        Color_Type Get_Style_Arc_Color(Part_Type Part);
        Color_Type Get_Style_Arc_Color_Filtered(Part_Type Part);
        const void *Get_Style_Arc_Image_Source(Part_Type Part);
        Opacity_Type Get_Style_Arc_Opacity(Part_Type Part);
        bool Get_Style_Arc_Rounded(Part_Type Part);
        Coordinate_Type Get_Style_Arc_Width(Part_Type Part);

        Base_Direction::Type Get_Style_Base_Direction(Part_Type Part);
        // - - - Background
        Color_Type Get_Style_Background_Color(Part_Type Part);
        Color_Type Get_Style_Background_Color_Filtered(Part_Type Part);
        Opacity_Type Get_Style_Background_Opacity(Part_Type Part);
        Color_Type Get_Style_Background_Gradient_Color(Part_Type Part);
        Color_Type Get_Style_Background_Gradient_Color_Filtered(Part_Type Part);
        Gradient::Direction_Type Get_Style_Background_Gradient_Direction(Part_Type Part);
        Coordinate_Type Get_Style_Background_Main_Stop(Part_Type Part);
        Coordinate_Type Get_Style_Background_Gradient_Stop(Part_Type Part);
        const Gradient::Descriptor_Type *Get_Style_Background_Gradient(Part_Type Part);
        Dither_Mode::Type Get_Style_Background_Dither_Mode(Part_Type Part);
        const void *Get_Style_Background_Image_Source(Part_Type Part);
        Opacity_Type Get_Style_Background_Image_Opacity(Part_Type Part);
        Color_Type Get_Style_Background_Image_Recolor(Part_Type Part);
        Color_Type Get_Style_Background_Image_Recolor_Filtered(Part_Type Part);
        Opacity_Type Get_Style_Background_Image_Recolor_Opacity(Part_Type Part);
        bool Get_Style_Background_Image_Tiled(Part_Type Part);

        Blend::Mode_Type Get_Style_Blend_Mode(Part_Type Part);

        // - - - Border
        Color_Type Get_Style_Border_Color(Part_Type Part);
        Color_Type Get_Style_Border_Color_Filtered(Part_Type Part);
        Opacity_Type Get_Style_Border_Opacity(Part_Type Part);
        Coordinate_Type Get_Style_Border_Width(Part_Type Part);
        Border_Side_Type Get_Style_Border_Side(Part_Type Part);
        bool Get_Style_Border_Post(Part_Type Part);

        bool Get_Style_Clip_Corner(Part_Type Part);
        const Color_Filter_Descriptor_Class Get_Style_Color_Filter_Descriptor(Part_Type Part);
        Opacity_Type Get_Style_Color_Filter_Opacity(Part_Type Part);

        // - - - Flex

        Flex_Flow_Type Get_Style_Flex_Flow(Part_Type Part);
        Flex_Alignment_Type Get_Style_Flex_Main_Place(Part_Type Part);
        Flex_Alignment_Type Get_Style_Flex_Cross_Place(Part_Type Part);
        Flex_Alignment_Type Get_Style_Flex_Track_Place(Part_Type Part);
        uint8_t Get_Style_Flex_Grow(Part_Type Part);

        // - - - Grid
        Coordinate_Type Get_Style_Grid_Cell_Column_Position(Part_Type Part);
        Coordinate_Type Get_Style_Grid_Cell_Column_Span(Part_Type Part);
        Coordinate_Type Get_Style_Grid_Cell_Row_Position(Part_Type Part);
        Coordinate_Type Get_Style_Grid_Cell_Row_Span(Part_Type Part);
        Coordinate_Type Get_Style_Grid_Cell_X_Alignment(Part_Type Part);
        Coordinate_Type Get_Style_Grid_Cell_Y_Alignment(Part_Type Part);
        Grid_Alignment_Type Get_Style_Grid_Column_Alignment(Part_Type Part);
        const Coordinate_Type *Get_Style_Grid_Column_Descriptor_Array(Part_Type Part);
        Grid_Alignment_Type Get_Style_Grid_Row_Alignment(Part_Type Part);
        const Coordinate_Type *Get_Style_Grid_Row_Descriptor_Array(Part_Type Part);

        Coordinate_Type Get_Style_Height(Part_Type Part);

        // - - - Image
        Opacity_Type Get_Style_Image_Opacity(Part_Type Part);
        Color_Type Get_Style_Image_Recolor(Part_Type Part);
        Color_Type Get_Style_Image_Recolor_Filtered(Part_Type Part);
        Opacity_Type Get_Style_Image_Recolor_Opacity(Part_Type Part);

        uint16_t Get_Style_Layout(Part_Type Part);

        // - - - Line
        Color_Type Get_Style_Line_Color(Part_Type Part);
        Color_Type Get_Style_Line_Color_Filtered(Part_Type Part);
        Coordinate_Type Get_Style_Line_Dash_Gap(Part_Type Part);
        Coordinate_Type Get_Style_Line_Dash_Width(Part_Type Part);
        Opacity_Type Get_Style_Line_Opacity(Part_Type Part);
        bool Get_Style_Line_Rounded(Part_Type Part);
        Coordinate_Type Get_Style_Line_Width(Part_Type Part);

        // - - - Size
        Coordinate_Type Get_Style_Maximum_Height(Part_Type Part);
        Coordinate_Type Get_Style_Maximum_Width(Part_Type Part);
        Coordinate_Type Get_Style_Minimum_Height(Part_Type Part);
        Coordinate_Type Get_Style_Minimum_Width(Part_Type Part);

        Opacity_Type Get_Style_Opacity(Part_Type Part);

        // - - - Outline
        Color_Type Get_Style_Outline_Color(Part_Type Part);
        Color_Type Get_Style_Outline_Color_Filtered(Part_Type Part);
        Opacity_Type Get_Style_Outline_Opacity(Part_Type Part);
        Coordinate_Type Get_Style_Outline_Pad(Part_Type Part);
        Coordinate_Type Get_Style_Outline_Width(Part_Type Part);

        // - - - Padding
        Coordinate_Type Get_Style_Pad_Bottom(Part_Type Part);
        Coordinate_Type Get_Style_Pad_Top(Part_Type Part);
        Coordinate_Type Get_Style_Pad_Left(Part_Type Part);
        Coordinate_Type Get_Style_Pad_Right(Part_Type Part);
        Coordinate_Type Get_Style_Pad_Row(Part_Type Part);
        Coordinate_Type Get_Style_Pad_Column(Part_Type Part);

        Coordinate_Type Get_Style_Radius(Part_Type Part);

        // - - - Shadow
        Color_Type Get_Style_Shadow_Color(Part_Type Part);
        Color_Type Get_Style_Shadow_Color_Filtered(Part_Type Part);
        Coordinate_Type Get_Style_Shadow_Offset_X(Part_Type Part);
        Coordinate_Type Get_Style_Shadow_Offset_Y(Part_Type Part);
        Opacity_Type Get_Style_Shadow_Opacity(Part_Type Part);
        Coordinate_Type Get_Style_Shadow_Spread(Part_Type Part);
        Coordinate_Type Get_Style_Shadow_Width(Part_Type Part);

        // - - - Text
        Text::Alignment_Type Get_Style_Text_Alignment(Part_Type Part);
        Color_Type Get_Style_Text_Color(Part_Type Part);
        Color_Type Get_Style_Text_Color_Filtered(Part_Type Part);
        Text::Decor_Type Get_Style_Text_Decor(Part_Type Part);
        const Font_Type *Get_Style_Text_Font(Part_Type Part);
        Coordinate_Type Get_Style_Text_Letter_Space(Part_Type Part);
        Coordinate_Type Get_Style_Text_Line_Space(Part_Type Part);
        Opacity_Type Get_Style_Text_Opacity(Part_Type Part);

        // - - - Transform
        Coordinate_Type Get_Style_Transform_Angle(Part_Type Part);
        Coordinate_Type Get_Style_Transform_Height(Part_Type Part);
        Coordinate_Type Get_Style_Transform_Width(Part_Type Part);
        Coordinate_Type Get_Style_Transform_Zoom(Part_Type Part);

        const Style_Class::Transition_Descriptor_Type *Get_Style_Transition(Part_Type Part);
        Coordinate_Type Get_Style_Translate_X(Part_Type Part);
        Coordinate_Type Get_Style_Translate_Y(Part_Type Part);
        Coordinate_Type Get_Style_Width(Part_Type Part);
        Coordinate_Type Get_Style_X(Part_Type Part);
        Coordinate_Type Get_Style_Y(Part_Type Part);

        // - - Operator
        operator bool();

        virtual bool Set_Pointer(lv_obj_t *LVGL_Object_Pointer);
        // TODO : See the encapsulation of these following methods.

        inline lv_obj_t *Get_Pointer() const
        {
            return LVGL_Object_Pointer;
        };

        void Clear_Pointer()
        {
            LVGL_Object_Pointer = NULL;
        };

        lv_obj_t *LVGL_Object_Pointer;

    protected:
    } Object_Type;

    Object_Type::Style_Selector_Type operator |(Part_Type Part, Graphics_Types::State_Type State)
    {
        return static_cast<lv_part_t>(Part) | static_cast<lv_state_t>(State);
    }

}

// -- Management -- //

#endif