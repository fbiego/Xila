///
/// @file String.cpp
/// @author Alix ANNERAUD (alix@anneraud.fr)
/// @brief
/// @version 0.1.0
/// @date 19-02-2023
///
/// @copyright Copyright (c) 2023
///

#include "Core/Module/String.hpp"

using namespace Xila_Namespace;

String_Class::String_Class() : Characters_Pointer(NULL), Size(0)
{
}

String_Class::String_Class(const String_Class &String) : Characters_Pointer(NULL), Size(0)
{
    Copy(String, true);
}

String_Class::String_Class(const char *String) : Characters_Pointer(NULL), Size(0)
{
    Copy(String, 0, true);
}

String_Class::String_Class(const char *String, Size_Type Size) : Characters_Pointer(NULL), Size(0)
{
    Copy(String, Size, true);
}

String_Class::~String_Class()
{
    Set_Size(0);
}

Size_Type String_Class::Get_Length() const
{
    if (Is_Valid())
    {
        return strnlen(Characters_Pointer, Get_Size());
    }
    else
    {
        return 0;
    }
}

Size_Type String_Class::Get_Size() const
{
    if (Is_Valid())
    {
        return Size;
    }
    return 0;
}

Size_Type String_Class::Get_Capacity() const
{
    return Get_Size() - 1;
}

/// @brief
/// @param Position
/// @return
char String_Class::Get_Character(Size_Type Position) const
{
    if (Position < Size && Is_Valid())
    {
        return Characters_Pointer[Position];
    }
    else
    {
        return '\0';
    }
}

Result_Type String_Class::Set_Size(Size_Type Size)
{
    if (Is_Valid())
    {
        if (Size == 0)
        {
            free(Characters_Pointer);
            Characters_Pointer = NULL;
        }
        else if (Size != Get_Size())
        {
            char *New_Buffer = (char *)realloc(Characters_Pointer, Size);
            if (New_Buffer == NULL)
            {
                return Result_Type::Error;
            }
            else
            {
                Characters_Pointer = New_Buffer;
            }
        }
    }
    else
    {
        if (Size == 0)
        {
            Characters_Pointer = NULL;
        }
        else
        {
            Characters_Pointer = (char *)malloc(Size);
            if (Characters_Pointer == NULL)
            {
                this->Size = 0;
                return Result_Type::Error;
            }
            Clear();
        }
    }

    this->Size = Size;
    return Result_Type::Success;
}

/// @brief
/// @param Capacity
/// @return
Result_Type String_Class::Set_Capacity(Size_Type Capacity)
{
    return Set_Size(Capacity + 1);
}

/// @brief Set custom buffer.
/// @param Buffer
/// @param Size
void String_Class::Set_Buffer(char *Buffer, Size_Type Size)
{
    if (Is_Valid())
    {
        Set_Size(0);
    }
    Characters_Pointer = Buffer;
    this->Size = Size;
}

/// @brief
/// @param Character
/// @param Position
void String_Class::Set_Character(char Character, Size_Type Position)
{
    if (Position < Size && Is_Valid())
    {
        Characters_Pointer[Position] = Character;
    }
}

void String_Class::Clear()
{
    if (Is_Valid())
    {
        memset(Characters_Pointer, '\0', Get_Size());
    }
}

Result_Type String_Class::Fit()
{
    if (Is_Valid())
    {
        return Set_Capacity(Get_Length());
    }
    return Result_Type::Error;
}

bool String_Class::Is_Empty() const
{
    if (Is_Valid())
    {
        return Characters_Pointer[0] == '\0';
    }
    else
    {
        return true;
    }
}

/// @brief Return if the string is valid (allocated).
/// @return true if the string is valid, false otherwise.
bool String_Class::Is_Valid() const
{
    return (Characters_Pointer != NULL) && (Size > 0);
}

/// @brief A function that copy a string into the current string.
/// @param String The string to copy.
/// @param Size The size of the string to copy. If 0, the size will be calculated according to the length of the text (must be null terminated).
/// @param Change_Size Does the string size must be changed to fit the new string.
/// @return Result_Type::Success if the string was copied, Result_Type::Error otherwise.
Result_Type String_Class::Copy(const char *String, Size_Type Size, bool Change_Size)
{
    if (String == NULL || !Is_Valid())
    {
        return Result_Type::Error;
    }

    if (Size == 0)
    {
        Size = strlen(String) + 1;
    }

    if (Change_Size)
    {
        if (Set_Size(Size) != Result_Type::Success)
        {
            return Result_Type::Error;
        }

        if (Get_Size() <= Size)
        {
            strlcpy(Characters_Pointer, String, Get_Size());
        }
        else
        {
            strlcpy(Characters_Pointer, String, Size);
        }
    }
    Characters_Pointer[Get_Size()] = '\0';
    return Result_Type::Success;
}

Result_Type String_Class::Copy(const String_Class &String, bool Change_Size)
{
    return Copy(String.Characters_Pointer, String.Size, Change_Size);
}

Result_Type String_Class::Copy(const String& String, bool Change_Size)
{
    return Copy(String.c_str(), String.length() + 1, Change_Size);
}

Result_Type String_Class::Copy_Format(const char *Format, ...)
{
    if (Format == NULL || !Is_Valid())
    {
        return Result_Type::Error;
    }

    va_list Arguments;
    va_start(Arguments, Format);
    vsnprintf(Characters_Pointer, Get_Size(), Format, Arguments);
    va_end(Arguments);
    
}


/// @brief
/// @param String
/// @param Size
/// @param Increase_Size
/// @return
Result_Type String_Class::Concatenate(const char *String, Size_Type Size, bool Increase_Size)
{
    if (String == NULL || !Is_Valid())
    {
        return Result_Type::Error;
    }

    if (Size == 0)
    {
        Size = strlen(String);
    }
    else
    {
        Size = strnlen(String, Size);
    }
    Size += Get_Length() + 1;

    if (Increase_Size && (Get_Size() < Size))
    {
        if (Set_Size(Size) != Result_Type::Success)
        {
            return Result_Type::Error;
        }
    }

    strlcat(Characters_Pointer, String, Get_Size());
    Characters_Pointer[Get_Size()] = '\0';

    return Result_Type::Success;
}

Result_Type String_Class::Concatenate(char Character, bool Increase_Size)
{
    if (!Is_Valid())
    {
        return Result_Type::Error;
    }

    Size_Type Size = Get_Length() + 2;

    if (Increase_Size && (Get_Size() < Size))
    {
        if (Set_Size(Size) != Result_Type::Success)
        {
            return Result_Type::Error;
        }
    }

    Characters_Pointer[Get_Length()] = Character;
    Characters_Pointer[Get_Size()] = '\0';

    return Result_Type::Success;
}

bool String_Class::Equals(const char *String, Size_Type Size) const
{
    if (String != NULL || Is_Valid())
    {
        if (Compare(String, Size) == 0)
        {
            return true;
        }
    }
    return false;
}

bool String_Class::Equals(const String_Class &String) const
{
    if (String && Is_Valid())
    {
        if (Compare(String) == 0)
        {
            return true;
        }
    }
    return false;
}

int8_t String_Class::Compare(const char *String, Size_Type Size) const
{
    if (String == NULL || !Is_Valid())
    {
        return 0;
    }

    if (Size == 0)
    {
        Size = strlen(String);
    }
    else
    {
        Size = strnlen(String, Size);
    }

    return strncmp(Characters_Pointer, String, Size);
}

int8_t String_Class::Compare(const String_Class &String) const
{
    return Compare(String.Characters_Pointer, String.Size);
}

Result_Type String_Class::Remove(Size_Type Position, Size_Type Size)
{
    if (Position >= Get_Size() || !Is_Valid())
    {
        return Result_Type::Error;
    }

    if (Size == 0)
    {
        return Result_Type::Invalid_Argument;
    }

    if (Position + Size > Get_Length())
    {
        Size = Get_Length() - Position;
    }

    memmove(Characters_Pointer + Position, Characters_Pointer + Position + Size, Get_Length() - Position - Size + 1);

    Characters_Pointer[Get_Size()] = '\0';

    return Result_Type::Success;
}

Result_Type String_Class::To_Char_Array(char *Array, Size_Type Size, Size_Type Offset_Position) const
{
    if (Array == NULL || Size == 0 || !Is_Valid())
    {
        return Result_Type::Error;
    }

    strlcpy(Array, Characters_Pointer, Size);
    return Result_Type::Success;
}

Result_Type String_Class::To_Upper_Case()
{
    if (!Is_Valid())
    {
        return Result_Type::Error;
    }

    for (Size_Type i = 0; i < Get_Length(); i++)
    {
        Characters_Pointer[i] = toupper(Characters_Pointer[i]);
    }
    return Result_Type::Success;
}

Result_Type String_Class::To_Lower_Case()
{
    if (!Is_Valid())
    {
        return Result_Type::Error;
    }

    for (Size_Type i = 0; i < Get_Length(); i++)
    {
        Characters_Pointer[i] = tolower(Characters_Pointer[i]);
    }
    return Result_Type::Success;
}

String_Class::operator bool() const
{
    return Is_Valid();
}

String_Class::operator const char *() const
{
    return Characters_Pointer;
}

String_Class &String_Class::operator=(const char *String)
{
    Copy(String);
    return *this;
}

String_Class &String_Class::operator=(const String_Class &String)
{
    Copy(String);
    return *this;
}

String_Class& String_Class::operator=(const String& String)
{
    Copy(String);
    return *this;
}

String_Class &String_Class::operator+=(const char *String)
{
    Concatenate(String);
    return *this;
}

String_Class &String_Class::operator+=(const String_Class &String)
{
    Concatenate(String);
    return *this;
}

String_Class &String_Class::operator+=(char Character)
{
    Concatenate(Character);
    return *this;
}

bool String_Class::operator==(const char *String) const
{

    return Equals(String);
}

bool String_Class::operator==(const String_Class &String) const
{
    return Equals(String);
}

bool String_Class::operator!=(const char *String) const
{
    return !Equals(String);
}

bool String_Class::operator!=(const String_Class &String) const
{
    return !Equals(String);
}

bool String_Class::operator<(const char *String) const
{
    return (Compare(String) < 0);
}

bool String_Class::operator<(const String_Class &String) const
{
    return (Compare(String) < 0);
}

bool String_Class::operator<=(const char *String) const
{
    return (Compare(String) <= 0);
}

bool String_Class::operator<=(const String_Class &String) const
{
    return (Compare(String) <= 0);
}

bool String_Class::operator>(const char *String) const
{
    return (Compare(String) > 0);
}

bool String_Class::operator>(const String_Class &String) const
{
    return (Compare(String) > 0);
}

bool String_Class::operator>=(const char *String) const
{
    return (Compare(String) >= 0);
}

bool String_Class::operator>=(const String_Class &String) const
{
    return (Compare(String) >= 0);
}

char String_Class::operator[](Size_Type Index) const
{
    return Get_Character(Index);
}

char &String_Class::operator[](Size_Type Index)
{
    if (Index >= Get_Size())
    {
        return Characters_Pointer[0];
    }
    return Characters_Pointer[Index];
}
