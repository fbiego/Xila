///
/// @file Checkbox.hpp
/// @author Alix ANNERAUD (alix.anneraud@outlook.fr)
/// @brief
/// @version 0.1.0
/// @date 08-05-2022
///
/// @copyright Copyright (c) 2022
///

#ifndef Checkbox_Hpp_Included
#define Checkbox_Hpp_Included

#include "Object.hpp"

namespace Xila_Namespace
{
    typedef class Checkbox_Class : public Object_Class
    {
    public:
        // - Types

        typedef enum Draw_Part_Enumeration
        {
            Box = LV_CHECKBOX_DRAW_PART_BOX,
        } Draw_Part_Type;

        // - Methods

        Checkbox_Class();
        Checkbox_Class(const Object_Class &Object_To_Copy);

        void Create(Object_Class Parent_Object) override;

        Checkbox_Class& operator=(const Object_Class& Object);
        // - - Setters.

        bool Set_Pointer(lv_obj_t *LVGL_Object_Pointer);

        void Set_Text(const char *Text);
        void Set_Text_Static(const char *Text);

        // - - Getters.
        const char *Get_Text();
    } Checkbox_Type;
}

#endif
