#include "Arduino.h"

#include "Core.hpp"

GalaxOS_Class GalaxOS;

/*char WiFi_SSID[] = "Avrupa";
char WiFi_Password[] = "0749230994";*/

GalaxOS_Class::GalaxOS_Class() : Keyboard(2, 6) //builder
{
  //strcpy(WiFi_SSID, "Avrupa");
  //strcpy(WiFi_Password, "0749230994");

#if SD_MODE == 0
  Drive = &SD_MMC;
#else
  Drive = &SD;
#endif

  for (int i = 0; i < 4; i++)
  {
    Software_Pointer[i] = NULL;
  }

  Low_RAM_Threshold = 2000;

  C_MIDI = 60;

  C_Frequency = 262;

  memset(Current_Username, 0, sizeof(Current_Username));

  Tag = 0x00;

  memset(Software_Pointer, NULL, 6);
}

GalaxOS_Class::~GalaxOS_Class() // Detroyer
{
}

// Used to initialise the core

void GalaxOS_Class::Start()
{

  Serial.begin(921600); //PC Debug UART
  Remaining_Spaces = 0;
  Horizontal_Separator();
  Print_Line();
  Print_Line(" _____       ___   _           ___  __    __       _____   _____ ", STYLE_CENTER_ALIGNMENT);
  Print_Line("|  ___|     /   | | |         /   | | |  | |      |  _  | |  ___|", STYLE_CENTER_ALIGNMENT);
  Print_Line("| |        / /| | | |        / /| |  | || |       | | | | | |___ ", STYLE_CENTER_ALIGNMENT);
  Print_Line("| |  _    / /_| | | |       / /_| |   |  |        | | | | |___  |", STYLE_CENTER_ALIGNMENT);
  Print_Line("| |_| |  / ___  | | |___   / ___  |  | || |       | |_| |  ___| |", STYLE_CENTER_ALIGNMENT);
  Print_Line("|_____| /_/   |_| |_____| /_/   |_| |_|  |_|      |_____| |_____|", STYLE_CENTER_ALIGNMENT);
  Print_Line();
  Print_Line();
  Horizontal_Separator();
  //Print_Line("Flash : 1,310,720 Bytes - EEPROM : 512 Bytes - RAM : " + char(ESP.getFreeHeap()) + "/ 327680 Bytes");
  Print_Line(F("Galax OS Embedded Edition - Alix ANNERAUD - Alpha"));
  Horizontal_Separator();
  Print_Line(F("Starting Galax OS ..."), 0);
  // Initialize SD Card
  Print_Line(F("Mount The SD Card ..."), 0);

#if SD_MODE == 0
  pinMode(14, INPUT_PULLUP);
  //pinMode(15, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  pinMode(13, INPUT_PULLUP);
  delete &SD_MMC;
#else

  delete &SD;
#endif

  if (!Drive->begin() || Drive->cardType() == CARD_NONE)
  {
    Print_Line(F("> Warning : The SD Card isn't mouted."), STYLE_LEFT_ALIGNMENT);
  }
  else
  {
    Print_Line(F("> The SD Card is mounted."));
  }
  File Temporary_File;

  // Initialize Virtual Memory
  Virtual_Memory_Semaphore = xSemaphoreCreateMutex();

  if (Virtual_Memory_Semaphore == NULL)
  {
    //error handle
  }
  else
  {
    xSemaphoreGive(Virtual_Memory_Semaphore);
  }

  //Testing Virtual Memory

  Verbose_Print_Line(F("> Testing Virtual Memory ..."));
  uint64_t Test_Long_Long = 249548639455789;
  Set_Variable('T', Test_Long_Long);
  Test_Long_Long = 0;
  Get_Variable('T', Test_Long_Long);

  uint32_t Test_Float = 123456789;
  Set_Variable('T', Test_Float);
  Serial.println(Test_Float);
  Test_Float = 0;
  Get_Variable('T', Test_Float);
  Serial.println(Test_Float);

  uint16_t Test_Integer = 1543;
  Set_Variable('T', Test_Integer);
  Test_Integer = 0;
  Get_Variable('T', Test_Integer);
  Serial.println(Test_Integer);

  uint8_t Test_Byte = 128;
  Set_Variable('T', Test_Byte);
  Test_Byte = 0;
  Get_Variable('T', Test_Byte);
  Serial.println(Test_Byte);

  if (Test_Long_Long != 249548639455789 || Test_Float != 123456789 || Test_Integer != 1543 || Test_Byte != 128)
  {
    Verbose_Print("> Waring : Test failed for uint64_t");
  }

  // Load Task
  Verbose_Print("> Loading Task ...");

  //xTaskCreatePinnedToCore() //core class : used to execute api call in "root" mode

  // Check if the system state was saved
  Verbose_Print("> Check existing of file last state ...");
  if (Drive->exists("/GALAXOS/GOSCUSA.GSF"))
  {
    Restore_System_State();
  }

  // Load display configuration

  {
    Verbose_Print_Line("> Load display registry ...");
    Temporary_File = Drive->open(Display_Registry_Path);
    DynamicJsonDocument Display_Registry(256);
    deserializeJson(Display_Registry, Temporary_File);
    Display.Set_Backlight(Display_Registry["Backlight"] | 100, false);
  }

  // Load network configuration
  {
    Verbose_Print_Line("> Load network registry ...");
    Temporary_File = Drive->open(Network_Registry_Path);
    if (!Temporary_File)
    {
      //error handle
    }
    DynamicJsonDocument Network_Registry(256);
    deserializeJson(Network_Registry, Temporary_File);
    WiFi.setHostname(Network_Registry["Host Name"]);                   // Set hostname
    const uint8_t Number_WiFi_AP = Network_Registry["Number WiFi AP"]; // Check number of registred AP
    char SSID[32], Password[32];
    for (uint8_t i = 1; i < Number_WiFi_AP; i++)
    {
      strcpy(SSID, Network_Registry["SSID" + String(i)]);
      strcpy(Password, Network_Registry["Password" + String(i)]);
      WiFi.setAutoConnect(false);
      WiFi.begin(SSID, Password);
      while (WiFi.status() != WL_CONNECTED && i < 50)
      {
        vTaskDelay(pdMS_TO_TICKS(100));
        i++;
      }
      if (WiFi.status() == WL_CONNECTED)
      {
        break;
      }
    }
    Temporary_File.close();
  }

  // Set Regional Parameters
  {
    Verbose_Print_Line("> Load regional registry ...");
    Temporary_File = Drive->open(Regional_Registry_Path);
    if (!Temporary_File)
    {
      //error
    }
    Synchronise_Time();
    Temporary_File.close();
  }

  // Load software (including Shell UI)
  Verbose_Print_Line("> Load software registry ...");
  Temporary_File = Drive->open(Software_Registry_Path);
  if (!Temporary_File)
  {
    return;
  }
  DynamicJsonDocument Software_Registry(256);
  deserializeJson(Software_Registry, Temporary_File);

  // to do

  // When every thing is loaded : open shell (with privilège -> friend class) in order to login
  Display.Set_Time(F("LOAD_TIM"), 50);

  Open_Software("Shell");
}

void GalaxOS_Class::Save_System_State()
{
  if (Drive->exists("/GALAXOS/GOSCUSA.GSF"))
  {
    if (Drive->remove("/GALAXOS/GOSCUSA.GSF"))
    {
      //event handler
    }
  }
  DynamicJsonDocument System_Save_Registry(256);

  System_Save_Registry["Current Username"] = Current_Username;

  File Temporary_File = Drive->open("/GALAXOS/GOSCUSA.GSF");
  serializeJson(System_Save_Registry, Temporary_File);
  Temporary_File.close();
}

// Callback function for screen

void GalaxOS_Class::Incomming_String_Data_From_Display(String &Received_Data)
{
  uint8_t Temporary_Return_Code = Received_Data.charAt(0);
  Received_Data.remove(0, 1);
  switch (Temporary_Return_Code)
  {
  case CODE_COMMAND:
  case CODE_COMMAND_NEW:
    if (Received_Data == "Close")
    {
      Close_Software();
    }
    else if (Received_Data == "Minimize")
    {
      Software_Pointer[0]->Minimize();
    }
    else
    {
      Software_Pointer[0]->Execute(Received_Data.charAt(0), Received_Data.charAt(1));
    }
    break;
  case CODE_VARIABLE_STRING:
    Tag = (char)Received_Data.charAt(0);
    Received_Data.remove(0, 1);
    Set_Variable(Tag, Received_Data);
    Tag = '\0';
    break;
  case CODE_VARIABLE_LONG_LONG:
    Tag = Received_Data.charAt(0);
    break;
  default:
    //error handle
    break;
  }
}

void GalaxOS_Class::Incomming_Numeric_Data_From_Display(uint64_t const &Received_Data)
{
  if (Tag != NULL)
  {
    Set_Variable(Tag, Received_Data);
    Tag = NULL;
  }
  else
  {
    Serial.println(F("Error : Cannot Set Variable Because Tag is NULL."));
    //error handle
  }
}

// Open file handler
void GalaxOS_Class::Open_File(File &File_To_Open)
{
  if (!File_To_Open)
  {
    //error handle
    File_To_Open.close();
    return;
  }
  else if (File_To_Open.isDirectory())
  {
    //error : directory not a file
    File_To_Open.close();
    return;
  }
  char File_Name[14];
  strcpy(File_Name, File_To_Open.name());
  uint8_t i = 1;
  for (; i < 14; i++) //extract extension
  {
    if (File_Name[i] == '.')
    {
      break;
    }
  }
  char Extension[] = {File_Name[i + 1], File_Name[i + 2], File_Name[i + 3]};
  //pre-defined
  if (strcmp(Extension, "GEF")) // executable file : load it into FFS
  {
    if (File_To_Open.size() == 0)
    {
      //error : empty file
      File_To_Open.close();
      return;
    }
    if (Update.begin(File_To_Open.size()))
    {
      if (Update.writeStream(File_To_Open) != File_To_Open.size())
      {
        //error : update perform partialy
        File_To_Open.close();
        return;
      }
      if (Update.end() && Update.isFinished())
      {
        //info : update completed
      }
      else
      {
        //error : update went wrong
      }
    }
    else
    {
      //error : not enought space to perform OTA
    }
  }
  else if (strcmp(Extension, "BMP"))
  {
  }

  else if (strcmp(Extension, "GPF"))
  {
  }
  else if (strcmp(Extension, "FPF"))
  {
  }
  else
  {
    if (Drive->open(F("/GALAXOS/REGISTRY/EXTENSIO.GCF")))
    {
      DynamicJsonDocument Extension_Registry(256);
      File Temporary_File = Drive->open(Extension_Registry_Path);
      deserializeJson(Extension_Registry, Temporary_File);
      Software_Pointer[Get_Software_Pointer(Extension_Registry[Extension])]->Open_File(File_To_Open);
    }
    //error handle : unknow extension
  }
  File_To_Open.close();
}

// Software management

void GalaxOS_Class::Open_Software(const char *Software_Name)
{
  if (Get_Software_Handle_Pointer(Software_Name) != NULL)
  {
    uint8_t Software_Handle_Slot = Get_Software_Handle_Pointer(Software_Name);
    for (uint8_t i = 0; i < 6; i++)
    {
      if (Software_Pointer[i] == NULL)
      {
        Software_Pointer[i] = Software_Handle_Pointer[Software_Handle_Slot]->Load_Function_Pointer();
        Software_Pointer[i]->Handle_Pointer = Software_Handle_Pointer[Software_Handle_Slot];
      }
    }
  }
}

void GalaxOS_Class::Close_Software(const char *Software_Name)
{
  if (Software_Name == NULL && Software_Pointer[0] != NULL) //by default delete current running software
  {
    delete Software_Pointer[0];
    Software_Pointer[0] = NULL;
  }
  else
  {
    uint8_t Software_Pointer_Slot = Get_Software_Pointer(Software_Name);
    delete Software_Pointer[Software_Pointer_Slot];
    Software_Pointer[Software_Pointer_Slot] = NULL;
  }
}

uint8_t &GalaxOS_Class::Get_Software_Pointer(const char *Software_Name)
{
  for (uint8_t i = 0; 1 < 6; i++)
  {
    if (i > 6)
    {
      return 0; //nothing find
    }
    if (Software_Pointer[i]->Handle_Pointer == NULL)
    {
      continue;
    }
    else if (strcmp(Software_Pointer[i]->Handle_Pointer->Name, Software_Name))
    {
      return i;
    }
  }
}

uint8_t &GalaxOS_Class::Get_Software_Handle_Pointer(const char *Software_Name)
{
  for (byte i = 0; i <= MAXIMUM_SOFTWARE; i++)
  {
    if (i >= MAXIMUM_SOFTWARE)
    {
      return NULL;
    }
    if (Software_Handle_Pointer[i] == NULL)
    {
      continue;
    }
    if (strcmp(Software_Handle_Pointer[i]->Name, Software_Name))
    {
      return i;
    }
  }
}

void GalaxOS_Class::Set_Load_Function(const char *Software_Name, void (*Load_Function_To_Set)())
{
  Software_Handle_Pointer[Get_Software_Handle_Pointer(Software_Name)]->Load_Function_Pointer = Load_Function_To_Set;
}

// Serial communication with commputerà

void GalaxOS_Class::Horizontal_Separator()
{
  Serial.println(F("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"));
}

void GalaxOS_Class::Print_Line(const __FlashStringHelper *Text_To_Print, uint8_t const &Alignement)
{
  Serial.print(F("||"));
  Serial.print(F(" "));
  Serial.print(Text_To_Print);
  Serial.println(F("||"));
}

void GalaxOS_Class::Print_Line(const char *Text_To_Print, uint8_t const &Alignement)
{
  Serial.print(F("||"));

  if (sizeof(Text_To_Print) > 64)
  {
    //unsupported now
    return;
  }
  Remaining_Spaces = 64 - sizeof(Text_To_Print);
  switch (Alignement)
  {
  case 0: //left
    Serial.print(F(" "));
    Serial.print(Text_To_Print);
    break;
  case 1: //centered
    Serial.print(F(" "));
    for (uint8_t i = 0; i < Remaining_Spaces / 2; i++)
    {
      Serial.println(F(" "));
    }
    Serial.print(Text_To_Print);
    Remaining_Spaces /= 2;
  default:

    break;
  }
  for (uint8_t i = 0; i < Remaining_Spaces; i++)
  {
    Serial.println(F(" "));
  }
  Serial.println(F("||"));
}

void GalaxOS_Class::Print_Line()
{
  Serial.print(F("||                                                                            ||"));
}

// --------------------------

void GalaxOS_Class::Restore_System_State()
{
  Display.Set_Current_Page(PAGE_DESK);

  if (!Drive->exists("/GALAXOS/GOSCUSA.GSF"))
  {
    return;
  }
  File Temporary_File = Drive->open("/GALAXOS/GOSCUSA.GSF");
  DynamicJsonDocument Temporary_Json_Document(256);
  //ReadBufferingStream Buffering_Stream(Drive->open("/"), 64); dont work, i don't know why

  if (deserializeJson(Temporary_Json_Document, Temporary_File) == true)
  {
    return;
  }

  Current_Username = Temporary_Json_Document["Current Username"];

  Temporary_File.close();
  ESP.restart();
}

//---------------------------------------------------------------------------//
//                                Virtual Memory Management                  //
//---------------------------------------------------------------------------//

// Char array
void GalaxOS_Class::Set_Variable(uint8_t const &Tag, const char *String_To_Set, Software_Handle_Class *Software_Handle_Targeted = NULL)
{
  xSemaphoreTake(Virtual_Memory_Semaphore, portMAX_DELAY);
  if (Software_Handle_Targeted == NULL) // global scope
  {
    Virtual_Memory_File = Drive->open("/GALAXOS/MEMORY/GLOBAL/STRING/" + Tag, FILE_WRITE);
    Virtual_Memory_File.seek(0);
    if (Virtual_Memory_File)
    {
      Virtual_Memory_File.print(String_To_Set);
    }
  }
  else // local scope
  {
    Virtual_Memory_File = Drive->open("/GALAXOS/MEMORY/" + char(Software_Handle_Targeted->Name) + "/STRING/" + Tag, FILE_WRITE);
    Virtual_Memory_File.seek(0);
    if (Virtual_Memory_File)
    {
      Virtual_Memory_File.print(String_To_Set);
    }
  }
  Virtual_Memory_File.close();
  xSemaphoreGive(Virtual_Memory_Semaphore);
}

void GalaxOS_Class::Get_Variable(uint8_t const &Tag, char *String_To_Set, Software_Handle_Class *Software_Handle_Targeted = NULL)
{
  xSemaphoreTake(Virtual_Memory_Semaphore, portMAX_DELAY);
  if (Software_Handle_Targeted == NULL) // global scope
  {
    Virtual_Memory_File = Drive->open("/GALAXOS/MEMORY/GLOBAL/STRING/" + Tag, FILE_READ);
    Virtual_Memory_File.seek(0);
    if (Virtual_Memory_File)
    {
      Virtual_Memory_File.readBytes(String_To_Set, sizeof(String_To_Set));
    }
  }
  else // local scope
  {
    Virtual_Memory_File = Drive->open("/GALAXOS/MEMORY/" + char(Software_Handle_Targeted->Name) + "/STRING/" + Tag, FILE_READ);
    Virtual_Memory_File.seek(0);
    if (Virtual_Memory_File)
    {
      Virtual_Memory_File.readBytes(String_To_Set, sizeof(String_To_Set));
    }
  }
  Virtual_Memory_File.close();
  xSemaphoreGive(Virtual_Memory_Semaphore);
}

// String
void GalaxOS_Class::Set_Variable(uint8_t const &Tag, String const &String_To_Set, Software_Handle_Class *Software_Handle_Targeted = NULL)
{
  xSemaphoreTake(Virtual_Memory_Semaphore, portMAX_DELAY);
  if (Software_Handle_Targeted == NULL) // global scope
  {
    Virtual_Memory_File = Drive->open("/GALAXOS/MEMORY/GLOBAL/STRING/" + Tag, FILE_WRITE);
    Virtual_Memory_File.seek(0);
    if (Virtual_Memory_File)
    {
      Virtual_Memory_File.print(String_To_Set);
    }
  }
  else // local scope
  {
    Virtual_Memory_File = Drive->open("/GALAXOS/MEMORY/" + char(Software_Handle_Targeted->Name) + "/STRING/" + Tag, FILE_WRITE);
    Virtual_Memory_File.seek(0);
    if (Virtual_Memory_File)
    {
      Virtual_Memory_File.print(String_To_Set);
    }
  }
  Virtual_Memory_File.close();
  xSemaphoreGive(Virtual_Memory_Semaphore);
}

void GalaxOS_Class::Get_Variable(uint8_t const &Tag, String &String_To_Get, Software_Handle_Class *Software_Handle_Targeted = NULL)
{
  xSemaphoreTake(Virtual_Memory_Semaphore, portMAX_DELAY);
  if (Software_Handle_Targeted == NULL) // global scope
  {
    Virtual_Memory_File = Drive->open("/GALAXOS/MEMORY/GLOBAL/STRING/" + Tag, FILE_READ);
    Virtual_Memory_File.seek(0);
    if (Virtual_Memory_File)
    {
      String_To_Get = Virtual_Memory_File.readString();
    }
  }
  else // local scope
  {
    Virtual_Memory_File = Drive->open("/GALAXOS/MEMORY/" + char(Software_Handle_Targeted->Name) + "/STRING/" + Tag, FILE_READ);
    Virtual_Memory_File.seek(0);
    if (Virtual_Memory_File)
    {
      String_To_Get = Virtual_Memory_File.readString();
    }
  }
  Virtual_Memory_File.close();
  xSemaphoreGive(Virtual_Memory_Semaphore);
}

// 64-bit var
void GalaxOS_Class::Set_Variable(uint8_t const &Tag, uint64_t const &Number_To_Set, Software_Handle_Class *Software_Handle_Targeted = NULL)
{
  xSemaphoreTake(Virtual_Memory_Semaphore, portMAX_DELAY);
  Split_Number[0] = (uint8_t)Number_To_Set;
  Split_Number[1] = (uint8_t)Number_To_Set << 8;
  Split_Number[2] = (uint8_t)Number_To_Set << 16;
  Split_Number[3] = (uint8_t)Number_To_Set << 24;
  Split_Number[4] = (uint8_t)Number_To_Set << 32;
  Split_Number[5] = (uint8_t)Number_To_Set << 40;
  Split_Number[6] = (uint8_t)Number_To_Set << 48;
  Split_Number[7] = (uint8_t)Number_To_Set << 56;
  if (Software_Handle_Targeted == NULL)
  {
    Virtual_Memory_File = Drive->open(Virtual_Global_Memory_File, FILE_WRITE);
    Virtual_Memory_File.seek(Tag << 1);
    if (Virtual_Memory_File)
    {
      Virtual_Memory_File.write(Split_Number, 8);
    }
  }
  else
  {
    Virtual_Memory_File = Drive->open("/GALAXOS/MEMORY/" + char(Software_Handle_Targeted->Name) + "/VARIABLE.GSF", FILE_WRITE);
    Virtual_Memory_File.seek(Tag << 1);
    if (Virtual_Memory_File)
    {
      Virtual_Memory_File.write(Split_Number, 8);
    }
  }
  Virtual_Memory_File.close();
  xSemaphoreGive(Virtual_Memory_Semaphore);
}

void GalaxOS_Class::Get_Variable(uint8_t const &Tag, uint64_t &Number_To_Get, Software_Handle_Class *Software_Handle_Targeted = NULL)
{
  xSemaphoreTake(Virtual_Memory_Semaphore, portMAX_DELAY);
  if (Software_Handle_Targeted == NULL)
  {
    Virtual_Memory_File = Drive->open(Virtual_Global_Memory_File, FILE_READ);
    Virtual_Memory_File.seek(Tag << 1);
    if (Virtual_Memory_File)
    {
      Virtual_Memory_File.readBytes((char *)Split_Number, 8);
    }
  }
  else
  {
    Virtual_Memory_File = Drive->open("/GALAXOS/MEMORY/" + char(Software_Handle_Targeted->Name) + "/VARIABLE.GSF", FILE_READ);
    Virtual_Memory_File.seek(Tag << 1);
    if (Virtual_Memory_File)
    {
      Virtual_Memory_File.readBytes((char *)Split_Number, 8);
    }
  }
  Number_To_Get = (uint64_t)Split_Number << 56 | (uint64_t)Split_Number << 48 | (uint64_t)Split_Number << 40 | (uint64_t)Split_Number << 32 | (uint64_t)Split_Number << 24 | (uint64_t)Split_Number << 16 | (uint64_t)Split_Number << 8 | (uint64_t)Split_Number;
  Virtual_Memory_File.close();
  xSemaphoreGive(Virtual_Memory_Semaphore);
}

// 32-bit variable
void GalaxOS_Class::Set_Variable(uint8_t const &Tag, uint32_t const &Number_To_Set, Software_Handle_Class *Software_Handle_Targeted = NULL)
{
  xSemaphoreTake(Virtual_Memory_Semaphore, portMAX_DELAY);
  Split_Number[0] = (uint8_t)Number_To_Set;
  Split_Number[1] = (uint8_t)Number_To_Set << 8;
  Split_Number[2] = (uint8_t)Number_To_Set << 16;
  Split_Number[3] = (uint8_t)Number_To_Set << 24;
  if (Software_Handle_Targeted == NULL)
  {
    Virtual_Memory_File = Drive->open(Virtual_Global_Memory_File, FILE_WRITE);
    Virtual_Memory_File.seek(Tag << 1);
    if (Virtual_Memory_File)
    {
      Virtual_Memory_File.write(Split_Number, 4);
    }
  }
  else
  {
    Virtual_Memory_File = Drive->open("/GALAXOS/MEMORY/" + char(Software_Handle_Targeted->Name) + "/VARIABLE.GSF", FILE_WRITE);
    Virtual_Memory_File.seek(Tag << 1);
    if (Virtual_Memory_File)
    {
      Virtual_Memory_File.write(Split_Number, 4);
    }
  }
  Virtual_Memory_File.close();
  xSemaphoreGive(Virtual_Memory_Semaphore);
}

void GalaxOS_Class::Get_Variable(uint8_t const &Tag, uint32_t &Number_To_Get, Software_Handle_Class *Software_Handle_Targeted = NULL)
{
  xSemaphoreTake(Virtual_Memory_Semaphore, portMAX_DELAY);
  if (Software_Handle_Targeted == NULL)
  {
    Virtual_Memory_File = Drive->open(Virtual_Global_Memory_File, FILE_READ);
    Virtual_Memory_File.seek(Tag << 1);
    if (Virtual_Memory_File)
    {
      Number_To_Get = Virtual_Memory_File.parseFloat();
    }
  }
  else
  {
    Virtual_Memory_File = Drive->open("/GALAXOS/MEMORY/" + char(Software_Handle_Targeted) + "/VARIABLE.GSF", FILE_READ);
    Virtual_Memory_File.seek(Tag << 1);
    if (Virtual_Memory_File)
    {
      Number_To_Get = Virtual_Memory_File.parseFloat();
    }
  }
  Virtual_Memory_File.close();
  xSemaphoreGive(Virtual_Memory_Semaphore);
}

// 16 bit variable
void GalaxOS_Class::Set_Variable(uint8_t const &Tag, uint16_t const &Number_To_Set, Software_Handle_Class *Software_Handle_Targeted = NULL)
{
  xSemaphoreTake(Virtual_Memory_Semaphore, portMAX_DELAY);
  if (Software_Handle_Targeted == NULL)
  {
    Virtual_Memory_File = Drive->open(Virtual_Global_Memory_File, FILE_WRITE);
    if (Virtual_Memory_File)
    {
      Split_Number[0] = (char)Number_To_Set;
      Split_Number[1] = (char)Number_To_Set << 8;
      Virtual_Memory_File.write(Split_Number, 2);
    }
  }
  else
  {
    Virtual_Memory_File = Drive->open("/GALAXOS/MEMORY/" + char(Software_Handle_Targeted->Name) + "/VARIABLE.GSF", FILE_WRITE);
    if (Virtual_Memory_File)
    {
      Split_Number[0] = (char)Number_To_Set;
      Split_Number[1] = (char)Number_To_Set << 8;
      Virtual_Memory_File.write(Split_Number, 2);
    }
  }
  Virtual_Memory_File.close();
  xSemaphoreGive(Virtual_Memory_Semaphore);
}

void GalaxOS_Class::Get_Variable(uint8_t const &Tag, uint16_t &Number_To_Set, Software_Handle_Class *Software_Handle_Targeted = NULL)
{
  xSemaphoreTake(Virtual_Memory_Semaphore, portMAX_DELAY);
  if (Software_Handle_Targeted == NULL)
  {
    Virtual_Memory_File = Drive->open(Virtual_Global_Memory_File, FILE_WRITE);
    Virtual_Memory_File.seek(0);
    if (Virtual_Memory_File)
    {
      Number_To_Set = Virtual_Memory_File.parseInt();
    }
  }
  else
  {
    Virtual_Memory_File = Drive->open("/GALAXOS/MEMORY/" + char(Software_Handle_Targeted->Name) + "/VARIABLE.GSF", FILE_WRITE);
    Virtual_Memory_File.seek(0);
    if (Virtual_Memory_File)
    {
      Number_To_Set = Virtual_Memory_File.parseInt();
    }
  }
  Virtual_Memory_File.close();
  xSemaphoreGive(Virtual_Memory_Semaphore);
}

// 8 bit variable
void GalaxOS_Class::Set_Variable(uint8_t const &Tag, uint8_t const &Number_To_Set, Software_Handle_Class *Software_Handle_Targeted = NULL)
{
  xSemaphoreTake(Virtual_Memory_Semaphore, portMAX_DELAY);
  Virtual_Memory_File = Drive->open(Virtual_Global_Memory_File, FILE_WRITE);
  Virtual_Memory_File.seek(0);
  if (Software_Handle)
    if (Virtual_Memory_File)
    {
      Virtual_Memory_File.write(Number_To_Set);
    }
  Virtual_Memory_File.close();
  xSemaphoreGive(Virtual_Memory_Semaphore);
}

void GalaxOS_Class::Get_Variable(uint8_t const &Tag, uint8_t &Number_To_Set, Software_Handle_Class *Software_Handle_Targeted = NULL)
{
  xSemaphoreTake(Virtual_Memory_Semaphore, portMAX_DELAY);
  Virtual_Memory_File = Drive->open(Virtual_Global_Memory_File, FILE_READ);
  Virtual_Memory_File.seek(0);
  if (Virtual_Memory_File)
  {
    Number_To_Set = Virtual_Memory_File.read();
  }
  Virtual_Memory_File.close();
  xSemaphoreGive(Virtual_Memory_Semaphore);
}

void Ressource_Monitor(void *pvParameters)
{
  (void)pvParameters;
  for (;;)
  {
    if (ESP.getFreeHeap() < 2000)
    {
      Serial.println("Low Memory !");
    }
    vTaskDelay(1000);
  }
}

// Security
uint16_t GalaxOS_Class::Check_Credentials(const char *Username_To_Check, const char *Password_To_Check)
{
  File Temporary_File = Drive->open("/USERS/" + String(Username_To_Check) + "/STTNGS/PASSWORD.GSF", FILE_READ);
  char Temporary_Password[Temporary_File.size()];
  if (Temporary_File)
  {
    Temporary_File.readString().toCharArray(Temporary_Password, Temporary_File.size());
  }
  else
  {
    Serial.println(F("Wrong Username !"));
    return WARNING_WRONG_USERNAME;
  }
  Temporary_File.close();
  if (strcmp(Temporary_Password, Password_To_Check))
  {
    Serial.println(F("Good Password"));

    return INFORMATION_GOOD_CREDENTIALS;
  }
  else
  {
    Serial.println(F("Wrong Password !"));
    return WARNING_WRONG_PASSWORD;
  }
}

uint16_t GalaxOS_Class::Login(const char *Username_To_Check, const char *Password_To_Check)
{
  if (Check_Credentials(Username_To_Check, Password_To_Check) == INFORMATION_GOOD_CREDENTIALS)
  {
    strcpy(Current_Username, Username_To_Check);
    return INFORMATION_GOOD_CREDENTIALS;
  }
  else
  {
    return Check_Credentials(Username_To_Check, Password_To_Check);
  }
}

void GalaxOS_Class::Load_User_Files()
{
  Serial.print(F("Load User Files ..."));
  String Temporary_Username;
  String Temporary_Password;
  Get_Variable('U', Temporary_Username);
  Get_Variable('P', Temporary_Password);
  if (Check_Credentials(Temporary_Username, Temporary_Password) == INFORMATION_GOOD_CREDENTIALS)
  {
    Display.Set_Current_Page(F("Load_User"));
  }
  else
  {
    //error : wrong username or password
  }
}

void GalaxOS_Class::Login(String const &Username, String const &Password)
{
  if (Check_Credentials(Username, Password_To_Log))
}

uint8_t GalaxOS_Class::Event_Handler(uint8_t const &Type)
{
  /*  switch (Type)
  {
    Nextion_Serial_Transmit(F("Event"), COMMAND_PAGE_NAME, "", 0);
  case ERROR_FAILLED_TO_INTIALIZE_SD_CARD:
    Nextion_Serial.print(F("TITLE_TXT.txt="));
    Nextion_Serial.print(F("A"));
    Nextion_Serial_Transmit(F("TITLE_TXT"), ATTRIBUTE_TXT, F("Error !"), 0);
    Nextion_Serial_Transmit(F("MESSAGE_TXT"), ATTRIBUTE_TXT, F("Failled to initialize the SD Card !"), 0);
    break;
  case ERROR_SOME_SYSTEM_FILES_ARE_MISSING:
    Nextion_Serial_Transmit(F("TITLE_TXT"), ATTRIBUTE_TXT, F("Error !"), 0);
    Nextion_Serial_Transmit(F("MESSAGE_TXT"), ATTRIBUTE_TXT, F("Some system files are missing !"), 0);
    break;
  case ERROR_SOME_USER_SETTINGS_FILES_ARE_MISSING:
    Nextion_Serial_Transmit(F("TITLE_TXT"), ATTRIBUTE_TXT, F("Error !"), 0);
    Nextion_Serial_Transmit(F("MESSAGE_TXT"), ATTRIBUTE_TXT, F("Some settings files are missing !"), 0);
    break;
  case ERROR_SOME_USER_SETTINGS_FILES_ARE_CORRUPTED:
    Nextion_Serial_Transmit(F("TITLE_TXT"), ATTRIBUTE_TXT, F("Error !"), 0);
    Nextion_Serial_Transmit(F("MESSAGE_TXT"), ATTRIBUTE_TXT, F("Some user settings files are corrupted !"), 0);
    break;
  case ERROR_THE_FILE_DO_NOT_EXIST:
    Nextion_Serial_Transmit(F("TITLE_TXT"), ATTRIBUTE_TXT, F("Error !"), 0);
    Nextion_Serial_Transmit(F("MESSAGE_TXT"), ATTRIBUTE_TXT, F("The file doesn't exist !"), 0);
    break;
  case WARNING_WRONG_PASSWORD:
    break;
  case WARNING_WRONG_USERNAME:
    Nextion_Serial_Transmit(F("WRONG_TXT"), ATTRIBUTE_TXT, F("Wrong Username !"), 0);
    Serial.println(F("Wrong Username !"));
    break;
  case WARNING_DO_YO_REALLY_WANT_TO_DELETE_THIS_ITEM:
    break;
  default:
    break;
  }*/
}

uint8_t GalaxOS_Class::Event_Handler(uint16_t const &Type, String const &Extra_Informations = "")
{
#define EVENT_CANCEL 0
#define EVENT_YES 1

  Display.Set_Current_Page(F("Event"));

  DynamicJsonDocument Event_Registry(256);
  deserializeJson(Event_Registry, Drive->open("/GALAXOS/REGISTRY/ERROR.GRF"));

  Display.Set_Text(F("MESSAGE_TXT"), Event_Registry[Type]);
  byte i = 0;

  while (Event_Reply == 0 && i < 100)
  {
    vTaskDelay(pdMS_TO_TICKS(50));
    i++;
  }
  if (i > 100)
  {
    Display.Set_Current_Page(Last_Page);
    return 0;
  }
  return Event_Reply;
}

void GalaxOS_Class::Synchronise_Time()
{
  char NTP_Server[20] = "pool.ntp.org";
  const long GMT_Offset = 3600;
  const long Daylight_Offset = 3600;
  if (WiFi.status() != WL_CONNECTED)
  {
    //error handle
    return;
  }
  configTime(GMT_Offset, Daylight_Offset, NTP_Server);
  if (!getLocalTime(&Time))
  {
    //error handle
    Serial.println(F("Failed to get time"));
    return;
  }
  Serial.println(&Time, "%A, %B %d %Y %H:%M:%S");
  vTaskDelay(pdMS_TO_TICKS(1000));
  Serial.println(&Time, "%A, %B %d %Y %H:%M:%S");
}

void Pictviewer()
{
  /*Nextion_Serial.print(F("page Pictviewer\xFF\xFF\xFF"));
  Nextion_Serial.print(F("FILENAME_TXT.txt=\""));
  Nextion_Serial.print(Temporary_File_Name);
  Nextion_Serial.print(F("\"\xFF\xFF\xFF"));
  unsigned int Width;
  unsigned int Height;
  unsigned int Size;
  unsigned long Data_offset;
  unsigned int Encoding;
  unsigned int Array_Size;
  byte Red;
  byte Green;
  byte Blue;
  unsigned int Color;
  Serial.print(F("Open path :"));
  Serial.println(Temporary_File_Path);
  Temporary_File = Drive->open(Temporary_File_Path);
  if (Temporary_File)
  {
    //Read the header
    Temporary_File.seek(0);
    if (Temporary_File.read() == 66 && Temporary_File.read() == 77)
    {
      Serial.println(F("It's a Bitmap File"));
      Temporary_File.seek(2);
      Size = int(Temporary_File.read()); //in bytes
      Serial.print(F("Size :"));
      Serial.println(Size);
      Nextion_Serial.print(F("SIEZ_NUM.val="));
      Nextion_Serial.print(Size);
      Nextion_Serial.print(F("\xFF\xFF\xFF"));
      Temporary_File.seek(10);
      Data_offset = long(Temporary_File.read());
      Serial.print(F("Data Offset :"));
      Serial.println(Data_offset);
      Temporary_File.seek(18);
      Width = int(Temporary_File.read());
      Serial.print(F("Width :"));
      Serial.println(Width);
      Nextion_Serial.print(F("WIDTH_NUM.val="));
      Nextion_Serial.print(Width);
      Nextion_Serial.print(F("\xFF\xFF\xFF"));
      Temporary_File.seek(22);
      Height = long(Temporary_File.read());
      Nextion_Serial.print(F("HEIGH_NUM.val="));
      Nextion_Serial.print(Height);
      Nextion_Serial.print(F("\xFF\xFF\xFF"));
      Serial.print(F("Height :"));
      Serial.println(Height);
      Temporary_File.seek(28);
      Encoding = int(Temporary_File.read());
      Serial.print(F("Encoding :"));
      Serial.println(Encoding);
      Height += 24;
      Width += 10;
      Temporary_File.seek(Data_offset);
      Serial.println(Temporary_File.position());
      if (Encoding == 16)
      {
        //Draw picture
        Serial.println(F("16 bit encoding"));
        for (int y = Height; y > 24; y--)
        {
          for (int x = 10; x < Width; x++)
          {
            Serial.println(Temporary_File.peek());
            Blue = Temporary_File.read();
            Serial.println(Temporary_File.peek());
            Green = Temporary_File.read();
            Color = Blue;
            Color = Color << 8;
            Color = Color | Green;
            Serial.println(Color);
            Nextion_Serial.print(F("fill "));
            Nextion_Serial.print(x);
            Nextion_Serial.write(0x2c);
            Nextion_Serial.print(y);
            Nextion_Serial.write(0x2c);
            Nextion_Serial.print(1);
            Nextion_Serial.write(0x2c);
            Nextion_Serial.print(1);
            Nextion_Serial.write(0x2c);
            Nextion_Serial.print(Color);
            Nextion_Serial.write(0xff);
            Nextion_Serial.write(0xff);
            Nextion_Serial.write(0xff);
          }
        }
      }
      else if (Encoding == 24)
      {
        Serial.println(F("24 bit encoding"));
        for (int y = Height; y > 24; y--)
        {
          for (int x = 10; x < Width; x++)
          {
            Serial.println(Temporary_File.peek());
            Blue = Temporary_File.read();
            Serial.println(Temporary_File.peek());
            Green = Temporary_File.read();
            Serial.println(Temporary_File.peek());
            Red = Temporary_File.read();
            Color = Red << 6;
            Color = Color | Green;
            Color = Color << 5;
            Color = Color | Blue;
            Serial.println(Color);
            Nextion_Serial.print(F("fill "));
            Nextion_Serial.print(x);
            Nextion_Serial.write(0x2c);
            Nextion_Serial.print(y);
            Nextion_Serial.write(0x2c);
            Nextion_Serial.print(1);
            Nextion_Serial.write(0x2c);
            Nextion_Serial.print(1);
            Nextion_Serial.write(0x2c);
            Nextion_Serial.print(map(Color, 0, 16777215, 0, 65535));
            Nextion_Serial.write(0xff);
            Nextion_Serial.write(0xff);
            Nextion_Serial.write(0xff);
          }
          Temporary_File.read();
          Temporary_File.read();
        }
      }

      else
      {
        Serial.println(F("The 32 bit encoding isn't be supported now"));
      }
    }
    else
    {
      Serial.println(F("It isn't a bitmap file"));
    }
  }
  else
  {
    Serial.println(F("The File Doesn't Exist or isn't readable"));
  }*/
}