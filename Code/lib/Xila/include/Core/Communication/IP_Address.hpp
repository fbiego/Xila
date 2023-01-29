#ifndef IP_Address_Hpp_Included
#define IP_Address_Hpp_Included

#include "Arduino.h"

namespace Xila_Namespace
{
    typedef class IP_Address_Class
    {
    public:
        // - Methods

        // - - Constructors / Destructors

        IP_Address_Class(bool Is_IPv4 = true);
        IP_Address_Class(uint32_t Address);
        IP_Address_Class(const uint8_t *Address, bool Is_IPv4 = true);
        IP_Address_Class(uint8_t Byte_1, uint8_t Byte_2, uint8_t Byte_3, uint8_t Byte_4);

        // - - Operators

        operator uint32_t() const;
        bool operator==(const IP_Address_Class &IP_Address) const;
        bool operator==(const uint8_t *Address) const;
        uint8_t operator[](uint8_t Index) const;
        uint8_t& operator[](uint8_t Index);
        IP_Address_Class& operator=(const uint8_t* Address);
        IP_Address_Class& operator=(uint32_t Address);

        // - - Getters
        bool Get_IP_Version() const;

        // - - Setters
        void Set_IP_Version(bool Is_IPv4);


    private:
        bool Is_IPv4;

        union
        {
            uint32_t DWord;
            uint8_t Bytes[16];
        } Address;

    } IP_Address_Type;
}

#endif