///
/// @file WiFi.hpp
/// @author Alix ANNERAUD (alix.anneraud@outlook.fr)
/// @brief
/// @version 0.1.0
/// @date 20-07-2022
///
/// @copyright Copyright (c) 2022
///

#ifndef WiFi_Hpp_Included
#define WiFi_Hpp_Included

#include "../Module/Module.hpp"

//==============================================================================//
///
/// @brief WiFi class
///

namespace Xila_Namespace
{

    /// @brief WiFi types
    namespace WiFi_Types
    {
        /// @brief WiFi status type
        enum class Status_Type
        {
            No_Shield,
            Idle,
            No_SSID_Available,
            Scan_Completed,
            Connected,
            Connection_Failed,
            Connection_Lost,
            Disconnected
        };

        enum class Mode_Type
        {
            None,
            Station,
            Access_Point,
            Station_Access_Point
        };

        enum class Authentification_Mode_Type
        {
            Open,
            WEP,
            WPA_PSK,         /**< authenticate mode : WPA_PSK */
            WPA2_PSK,        /**< authenticate mode : WPA2_PSK */
            WPA_WPA2_PSK,    /**< authenticate mode : WPA_WPA2_PSK */
            WPA2_ENTERPRISE, /**< authenticate mode : WPA2_ENTERPRISE */
            WPA3_PSK,        /**< authenticate mode : WPA3_PSK */
            WPA2_WPA3_PSK,   /**< authenticate mode : WPA2_WPA3_PSK */
            WAPI_PSK,        /**< authenticate mode : WAPI_PSK */
            MAX
        };

        enum class WPA2_Authentification_Method_Type
        {
            TLS,
            PEAP,
            TTLS
        };

    };

    /// @brief WiFi class
    typedef class WiFi_Class : public Module_Class
    {
    public:

        // - Constructors / Destructors

        WiFi_Class();
        ~WiFi_Class();

        // - Methods

        // - - Managment

        void Turn_Off();

        // - - Getters

        int32_t Get_Channel();
        WiFi_Types::Mode_Type Get_Mode();
        int16_t Get_Transmission_Power();

        const char *Get_Host_Name();

        // - - Setters

        void Set_Long_Range(bool Enable);
        Result_Type Set_Transmission_Power(int16_t Power);

        Result_Type *Set_Host_Name(const char *Host_Name);

        // Result_Type Set_Antennas(uint8_t GPIO_1, uint8_t GPIO_2);

        // - - Registry

        Result_Type Create_Registry();
        Result_Type Load_Registry();
        Result_Type Save_Registry();
        Result_Type Add_Access_Point(const char *SSID, const char *Password = NULL, int32_t Channel = 0, const uint8_t *BSSID = NULL);
        Result_Type Remove_Access_Point(const char *SSID);

        // - -


        // - Sub class

        /// @brief Station class
        class Station_Class
        {
        public:

            Station_Class();

            // From saved SSID
            Result_Type Connect(const char *SSID);

            // From unknown SSID
            void Connect(const char *SSID, const char *Password = NULL, int32_t Channel = 0, const uint8_t *BSSID = NULL);

            void Connect(char *SSID, char *Password = NULL, int32_t Channel = 0, const uint8_t *BSSID = NULL);

            // Result_Type Connect(const char* WPA2_SSID, WiFi_Types::WPA2_Authentification_Method_Type, const char* Identity = NULL, const char* Username = NULL, const char* Password = NULL, const char* CA_PEM = NULL, const char* Client_CRT = NULL, const char* Client_Key = NULL, int32_t Channel = 0, const uint8_t* BSSID = 0);

            Result_Type Disconnect();

            // - Registry

            Result_Type Remove(const char *SSID, int32_t Channel = 0);
            Result_Type Add(const char *SSID, const char *Password = NULL, int32_t Channel = 0, const uint8_t *BSSID = NULL);

            // - - Getters

            bool Get_Automatic_Reconnection();
            WiFi_Types::Status_Type Get_Status(bool Wait = false, uint32_t Timeout = 30000);

            // - - - Station

            const char *Get_SSID();
            const char *Get_Pre_Shared_Key();
            uint8_t *Get_BSSID();
            int8_t Get_RSSI();

            // - - - Network configuration

            uint8_t *Get_MAC_Address(uint8_t *MAC_Address);

            IP_Address_Type Get_IP_Address(bool IPv6 = false);
            IP_Address_Type Get_Subnet_Mask();
            IP_Address_Type Get_Gateway_IP_Address();
            IP_Address_Type Get_DNS_IP_Address(uint8_t Index);

            IP_Address_Type Get_Broadcast_IP_Address();
            IP_Address_Type Get_Network_ID();
            uint8_t Get_Subnet_CIDR();

            // - - Setters

            // - - - Network configuration

            Result_Type Set_Configuration(IP_Address_Type IP_Address, IP_Address_Type Subnet_Mask, IP_Address_Type Gateway, IP_Address_Type DNS_1 = static_cast<uint32_t>(0x00000000), IP_Address_Type DNS_2 = static_cast<uint32_t>(0x00000000));

            bool Set_Automatic_Reconnection(bool Enable);
            void Set_Scan_Method(bool Fast);
            void Set_Sort_Method(bool Signal);

        private:
            bool IP_v6;

            IP_Address_Type IP_Address, Subnet_Mask, Gateway, DNS_1, DNS_2;

            friend class WiFi_Class;

        } Station;

        class Access_Point_Class
        {
        public:
            Access_Point_Class();

            // - Methods

            // - - Managment

            Result_Type Create(const char *SSID, const char *Password = NULL, int32_t Channel = 1, int SSID_Hidden = 0, int Maximum_Connection = 4, bool FTM_Responder = false);

            // - - Getters

            const char *Get_SSID();

            uint8_t Get_Station_Number();

            IP_Address_Type Get_IP_Address();
            IP_Address_Type Get_Broadcast_IP_Address();
            IP_Address_Type Get_Network_ID();
            uint8_t Get_Subnet_CIDR();

            // - - Setters

            Result_Type Set_IPv6(bool Enable = true);
            uint8_t *Get_MAC_Address(uint8_t *MAC_Address);
            Result_Type Set_Configuration(IP_Address_Type Local_IP, IP_Address_Type Gateway, IP_Address_Type Subnet, IP_Address_Type DHCP_Lease_Start = static_cast<uint32_t>(0));

        private:
            bool IP_v6;
            char Password[64];
            int32_t Channel;
            bool Hidden;
            uint8_t Maximum_Stations;


            IP_Address_Type IP_Address, Subnet_Mask, Gateway_IP_Address, DHCP_Lease_Start_IP_Address;

            friend class WiFi_Class;

        } Access_Point;

        class Scan_Class
        {
        public:

            Scan_Class();

            int16_t Start(bool Async = false, bool Show_Hidden = false, bool Passive = false, uint32_t Max_MS_Per_Channel = 300, uint8_t Channel = 0, const char *SSID = NULL, const uint8_t *BSSID = NULL);

            Result_Type Get_Result(uint16_t Result);
            void Delete();

            // - - Getters
            bool Get_Informations(uint8_t Index, char *SSID, uint8_t &Encryption_Type, int32_t &RSSI, uint8_t *&BSSID, int32_t &Channel);

            const char *Get_SSID(uint8_t Index);
            Authentification_Mode_Type Get_Encryption(uint8_t Index);
            int32_t RSSI(uint8_t Index);
            uint8_t *Get_BSSID(uint8_t Index);
            int32_t Get_Channel(uint8_t Index);

        } Scan;

     
    private:

        bool Long_Range;


    } WiFi_Type;

    extern WiFi_Type WiFi;

}

#endif