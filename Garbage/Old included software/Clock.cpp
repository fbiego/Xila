#include "Software/Clock.hpp"

Clock_Class *Clock_Class::Instance_Pointer = NULL;
uint32_t Clock_Class::Next_Alarm = 0;

Clock_Class::Clock_Class()
    : Xila_Class::Software(Clock_Handle),
      Chronometer_State(Stopped),
      Chronometer_Inital_Time(0),
      Chronometer_Paused_Time(0),
      Timer_State(Stopped),
      Timer_Threshold_Time(0),
      Timer_Paused_Time(0),
      Temporary_Time(0),
      Selected_Alarm(0)
{
    memset(Alarm_Title, '\0', sizeof(Alarm_Title));
    Xila.Task.Create(Main_Task, "Clock Task", Memory_Chunk(4), NULL, &Task_Handle);
}

Clock_Class::~Clock_Class()
{
    if (Instance_Pointer != this)
    {
        delete Instance_Pointer;
    }
    Instance_Pointer = NULL;
}

Xila_Class::Software *Clock_Class::Load()
{
    if (Instance_Pointer != NULL)
    {
        delete Instance_Pointer;
    }
    Instance_Pointer = new Clock_Class();
    return Instance_Pointer;
}

void Clock_Class::Startup_Function()
{
    Xila.Task.Create(Background_Task, "Clock Task", Memory_Chunk(2));
}

Xila_Class::Task_Function Clock_Class::Main_Task(void *pvParameters)
{
    while (1)
    {
        switch (Xila.Display.Get_Current_Page())
        {
        case Clock:
            Instance_Pointer->Clock_Instructions();
            break;
        case Alarm:
            Instance_Pointer->Alarm_Instructions();
            break;
        case Chronometer:
            Instance_Pointer->Chronometer_Instructions();
            break;
        case Timer:
            Instance_Pointer->Timer_Instructions();
            break;
        default:
            Instance_Pointer->Current_Instruction = Instance_Pointer->Get_Instruction();
            Instance_Pointer->Main_Instructions();
            break;
        }
    }
}

void Clock_Class::Background_Task(void *pvParameters)
{
    while (1)
    {
        if (Next_Alarm == 0)
        {
            Xila.Task.Delay(5000); // wait 30 seconds
        }
        else
        {
            if (Next_Alarm < Xila.Time.Milliseconds())
            {
                Xila.Software_Management.Open(Clock_Handle);
                Instance_Pointer->Send_Instruction('R', 'A');
            }
            Xila.Task.Delay(1000);
        }
    }
}

void Clock_Class::Check_Timer()
{
    if (Timer_State == Running && Xila.Time.Milliseconds() >= Timer_Threshold_Time)
    {
        if (Xila.Software_Management.Get_State(Clock_Handle) == Xila.Minimized)
        {
            Xila.Software_Management.Maximize(Clock_Handle);
        }
        Timer_State = Stopped;
        Xila.Sound.Play(Clock_File("Ringtone.wav")); // play something
        Xila.Dialog.Event(F("Time over."), Xila.Information);
        Xila.Sound.Stop();
        Send_Instruction('R', 'e');
    }
}

void Clock_Class::Load_Registry()
{
    memset(Alarm_Title, '\0', sizeof(Alarm_Title));

    for (uint8_t i = 0; i < 6; i++)
    {
        Alarm_Hour[i] = 0;
        Alarm_Minute[i] = 0;
        Alarm_State[i] = Disabled;
    }

    File Temporary_File = Xila.Drive.Open(Clock_File("Registry.xrf"));
    DynamicJsonDocument Clock_Registry(1024);

    if (deserializeJson(Clock_Registry, Temporary_File) != DeserializationError::Ok)
    {
        Send_Instruction('S', 'R');
        Temporary_File.close();
        return;
    }
    Temporary_File.close();
    if (strcmp(Clock_Registry["Registry"] | "", "Clock") != 0)
    {
        Send_Instruction('S', 'R');
        return;
    }
    uint8_t i = 0;
    for (JsonObject Alarm : Clock_Registry["Alarms"].as<JsonArray>())
    {
        strlcpy(Alarm_Title[i], Alarm["Title"] | "", sizeof(Alarm_Title[i]));

        Alarm_Hour[i] = Alarm["Hour"];
        Alarm_Minute[i] = Alarm["Minute"];
        Alarm_State[i] = Alarm["State"];
        i++;
        if (i > 5)
        {
            break;
        }
    }
}

void Clock_Class::Save_Registry()
{
    DynamicJsonDocument Clock_Registry(1024);
    Clock_Registry["Registry"] = "Clock";
    JsonArray Alarms = Clock_Registry.createNestedArray("Alarms");
    JsonObject Alarms_0 = Alarms.createNestedObject();
    Alarms_0["Title"] = Alarm_Title[0];
    Alarms_0["Hour"] = Alarm_Hour[0];
    Alarms_0["Minute"] = Alarm_Minute[0];
    Alarms_0["State"] = Alarm_State[0];

    JsonObject Alarms_1 = Alarms.createNestedObject();
    Alarms_1["Title"] = Alarm_Title[1];
    Alarms_1["Hour"] = Alarm_Hour[1];
    Alarms_1["Minute"] = Alarm_Minute[1];
    Alarms_1["State"] = Alarm_State[1];

    JsonObject Alarms_2 = Alarms.createNestedObject();
    Alarms_2["Title"] = Alarm_Title[2];
    Alarms_2["Hour"] = Alarm_Hour[2];
    Alarms_2["Minute"] = Alarm_Minute[2];
    Alarms_2["State"] = Alarm_State[2];

    JsonObject Alarms_3 = Alarms.createNestedObject();
    Alarms_3["Title"] = Alarm_Title[3];
    Alarms_3["Hour"] = Alarm_Hour[3];
    Alarms_3["Minute"] = Alarm_Minute[3];
    Alarms_3["State"] = Alarm_State[3];

    JsonObject Alarms_4 = Alarms.createNestedObject();
    Alarms_4["Title"] = Alarm_Title[4];
    Alarms_4["Hour"] = Alarm_Hour[4];
    Alarms_4["Minute"] = Alarm_Minute[4];
    Alarms_4["State"] = Alarm_State[4];

    JsonObject Alarms_5 = Alarms.createNestedObject();
    Alarms_5["Title"] = Alarm_Title[5];
    Alarms_5["Hour"] = Alarm_Hour[5];
    Alarms_5["Minute"] = Alarm_Minute[5];
    Alarms_5["State"] = Alarm_State[5];

    if (!Xila.Drive.Exists(Clock_Directory_Path))
    {
        Xila.Drive.Make_Directory(Clock_Directory_Path);
    }

    File Temporary_File = Xila.Drive.Open(Clock_File("Registry.xrf"), FILE_WRITE);
    if (serializeJson(Clock_Registry, Temporary_File) == 0)
    {
        Xila.Dialog.Event(F("Failed to save clock registry."), Xila.Error);
    }
    Temporary_File.close();
}

void Clock_Class::Refresh_Timer()
{
    switch (Timer_State)
    {
    case Stopped:
        Xila.Display.Set_Text(F("START_BUT"), F("Start"));
        Xila.Display.Click(F("SETBUTTONS_BUT"), 1);
        break;
    case Running:
        Xila.Display.Set_Text(F("START_BUT"), F("Pause"));
        Temporary_Time = Timer_Threshold_Time - Xila.Time.Milliseconds();
        Temporary_Time /= 1000; //get rid of milliseconds
        Xila.Display.Set_Value(F("SECONDS_NUM"), Temporary_Time % 60);
        Temporary_Time /= 60;
        Xila.Display.Set_Value(F("MINUTES_NUM"), Temporary_Time % 60);
        Xila.Display.Set_Value(F("HOURS_NUM"), Temporary_Time / 60);
        Xila.Display.Click(F("SETBUTTONS_BUT"), 0);
        break;
    case Paused:
        Xila.Display.Set_Text(F("START_BUT"), F("Start"));
        Temporary_Time = Timer_Threshold_Time - Timer_Paused_Time;
        Temporary_Time /= 1000; //get rid of milliseconds
        Xila.Display.Set_Value(F("SECONDS_NUM"), Temporary_Time % 60);
        Temporary_Time /= 60;
        Xila.Display.Set_Value(F("MINUTES_NUM"), Temporary_Time % 60);
        Xila.Display.Set_Value(F("HOURS_NUM"), Temporary_Time / 60);
        Xila.Display.Click(F("SETBUTTONS_BUT"), 0);
        break;

    default:
        break;
    }
}

void Clock_Class::Refresh_Chronometer()
{
    switch (Chronometer_State)
    {
    case Stopped:
        Xila.Display.Set_Trigger(F("TIMER_TIM"), false);
        Xila.Display.Set_Text(F("START_BUT"), F("Start"));
        Xila.Display.Set_Value(F("MILLIS_NUM"), 0);
        Xila.Display.Set_Value(F("SECOND_NUM"), 0);
        Xila.Display.Set_Value(F("MINUTE_NUM"), 0);
        break;
    case Running:
        Xila.Display.Set_Trigger(F("TIMER_TIM"), true);
        Xila.Display.Set_Text(F("START_BUT"), F("Pause"));
        Temporary_Time = Xila.Time.Milliseconds() - Chronometer_Inital_Time;
        Xila.Display.Set_Value(F("MILLIS_NUM"), (Temporary_Time % 1000) / 10);
        Temporary_Time /= 1000;
        Xila.Display.Set_Value(F("SECOND_NUM"), Temporary_Time % 60);
        Xila.Display.Set_Value(F("MINUTE_NUM"), Temporary_Time / 60);
        break;
    case Paused:
        Xila.Display.Set_Trigger(F("TIMER_TIM"), false);
        Xila.Display.Set_Text(F("START_BUT"), F("Start"));
        Temporary_Time = Chronometer_Paused_Time - Chronometer_Inital_Time;
        Xila.Display.Set_Value(F("MILLIS_NUM"), Temporary_Time % 1000);
        Temporary_Time /= 1000;
        Xila.Display.Set_Value(F("SECOND_NUM"), Temporary_Time % 60);
        Xila.Display.Set_Value(F("MINUTE_NUM"), Temporary_Time / 60);
        break;
    default:
        break;
    }
}

void Clock_Class::Set_Variable(Xila_Class::Address Address, uint8_t Type, const void *Data)
{
    switch (Xila.Display.Get_Current_Page())
    {
    case Alarm:
        if (Selected_Alarm < 6)
        {
            switch (Address)
            {
            case Address('H', 'o'):
                Alarm_Hour[Selected_Alarm] = *(uint8_t *)Data;
                break;
            case Address('M', 'i'):
                Alarm_Minute[Selected_Alarm] = *(uint8_t *)Data;
                break;

            default:
                break;
            }
        }
        break;
    case Timer:
        switch (Address)
        {
        case Address('H', 'o'):
            Hours = *(uint8_t *)Data;
            break;
        case Address('M', 'i'):

            Minutes = *(uint8_t *)Data;
            break;
        case Address('S', 'e'):
            Seconds = *(uint8_t *)Data;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

void Clock_Class::Refresh_Next_Alarm()
{

    uint16_t Alarm_Delta[8]; // alarms + current time + minimum delta
    Alarm_Delta[6] = Time.tm_hour * 24 + Time.tm_min;
    Alarm_Delta[7] = 0xFFFF;
    uint8_t Next_Alarm_Slot = 0;

    for (uint8_t i = 0; i < 6; i++)
    {
        if (Alarm_Title[i] == '\0' || Alarm_State[i] == Disabled)
        {
            Alarm_Delta[i] = 0xFFFF;
        }
        else
        {
            Alarm_Delta[i] = Alarm_Hour[i] * 24 + Alarm_Minute[i];
        }
    }

    for (uint8_t i = 0; i < 6; i++)
    {
        if (Alarm_Delta[i] == 0xFFFF)
        {
        }
        else
        {
            if (Alarm_Delta[i] <= Alarm_Delta[6])
            {
                Alarm_Delta[i] = ((24 * 60) - Alarm_Delta[6]) + Alarm_Delta[i];
            }
            else
            {
                Alarm_Delta[i] -= Alarm_Delta[6];
            }
        }
    }

    for (uint8_t i = 0; i < 6; i++)
    {
        if (Alarm_Delta[i] < Alarm_Delta[7] && Alarm_Delta[i] > 1)
        {
            Alarm_Delta[7] = Alarm_Delta[i];
            Next_Alarm_Slot = i;
        }
    }

    if (Alarm_Delta[7] != 0xFFFF)
    {
        Next_Alarm = (Alarm_Delta[Next_Alarm_Slot] * 60) * 1000; // next alarm in milliseconds
    }
    else
    {
        Next_Alarm = 0;
    }
}

void Clock_Class::Refresh_Alarms()
{
    char Object_Name[11];
    char Object_Text[25];
    for (uint8_t i = 0; i < 6; i++)
    {
        if (Alarm_Title[i][0] == '\0')
        {
            strlcpy(Object_Name, "ALARM _TXT", sizeof(Object_Name));
            Object_Name[5] = i + '0';
            Xila.Display.Set_Text(Object_Name, "");
            strlcpy(Object_Name, "ALARM _RAD", sizeof(Object_Name));
            Object_Name[5] = i + '0';
            Xila.Display.Set_Value(Object_Name, 0);
            Xila.Display.Hide(Object_Name);
        }
        else
        {
            if (Alarm_State[i] == true)
            {
                sprintf(Object_Text, "E - %02i:%02i - %s", Alarm_Hour[i], Alarm_Minute[i], Alarm_Title[i]);
            }
            else
            {
                sprintf(Object_Text, "D - %02i:%02i - %s", Alarm_Hour[i], Alarm_Minute[i], Alarm_Title[i]);
            }

            strlcpy(Object_Name, "ALARM _TXT", sizeof(Object_Name));
            Object_Name[5] = i + '0';
            Xila.Display.Set_Text(Object_Name, Object_Text);
            Xila.Display.Show(Object_Name);
            strlcpy(Object_Name, "ALARM _RAD", sizeof(Object_Name));
            Object_Name[5] = i + '0';
            if (Selected_Alarm == i)
            {
                Xila.Display.Set_Value(Object_Name, 1);
            }
            else
            {
                Xila.Display.Set_Value(Object_Name, 0);
            }
            Xila.Display.Show(Object_Name);
        }
    }
}

void Clock_Class::Refresh_Alarm()
{
    if (Alarm_Title[Selected_Alarm][0] == '\0')
    {
        Xila.Display.Set_Text(F("TITLEVAL_TXT"), "");
        Xila.Display.Set_Value(F("HOURS_NUM"), 0);
        Xila.Display.Set_Value(F("MINUTES_NUM"), 0);
    }
    else
    {
        Xila.Display.Set_Text(F("TITLEVAL_TXT"), Alarm_Title[Selected_Alarm]);
        Xila.Display.Set_Value(F("HOURS_NUM"), Alarm_Hour[Selected_Alarm]);
        Xila.Display.Set_Value(F("MINUTES_NUM"), Alarm_Minute[Selected_Alarm]);
        if (Alarm_State[Selected_Alarm] == true)
        {
            Xila.Display.Set_Text(F("STATE_BUT"), F("Disable"));
        }
        else
        {
            Xila.Display.Set_Text(F("STATE_BUT"), F("Enable"));
        }
    }
}

void Clock_Class::Add_Alarm()
{
    for (uint8_t i = 0; i < 6; i++)
    {
        if (Alarm_Title[i][0] == '\0')
        {
            Alarm_Hour[i] = 0;
            Alarm_Minute[i] = 0;
            Alarm_State[i] = false;
            strlcpy(Alarm_Title[i], "Untitled", sizeof(Alarm_Title[i]));
            Selected_Alarm = i;
            Send_Instruction('R', 'e');
            return;
        }
    }
}

//

void Clock_Class::Refresh_Clock()
{
    Time = Xila.Time.Get_Time();
    // Set time
    memset(Temporary_Char_Array, '\0', sizeof(Temporary_Char_Array));
    sprintf(Temporary_Char_Array, "%02i:%02i:%02i", Time.tm_hour, Time.tm_min, Time.tm_sec);
    Xila.Display.Set_Text(F("TIME_TXT"), Temporary_Char_Array);
    // Set date
    memset(Temporary_Char_Array, '\0', sizeof(Temporary_Char_Array));
    if (Time.tm_mday == 1)
    {
        sprintf(Temporary_Char_Array, "%s %ist %s %i", Days[Time.tm_wday], Time.tm_mday, Months[Time.tm_mon], Time.tm_year + 1900);
    }
    else if (Time.tm_mday == 2)
    {
        sprintf(Temporary_Char_Array, "%s %ind %s %i", Days[Time.tm_wday], Time.tm_mday, Months[Time.tm_mon], Time.tm_year + 1900);
    }
    else if (Time.tm_mday == 3)
    {
        sprintf(Temporary_Char_Array, "%s %ird %s %i", Days[Time.tm_wday], Time.tm_mday, Months[Time.tm_mon], Time.tm_year + 1900);
    }
    else
    {
        sprintf(Temporary_Char_Array, "%s %ith %s %i", Days[Time.tm_wday], Time.tm_mday, Months[Time.tm_mon], Time.tm_year + 1900);
    }
    Xila.Display.Set_Text(F("DATE_TXT"), Temporary_Char_Array);
}

void Clock_Class::Main_Instructions()
{
    switch (Current_Instruction)
    {
    case Xila.Idle:
        if (Xila.Software_Management.Get_State(Clock_Handle) == Xila.Minimized)
        {
            Xila.Task.Delay(200);
        }
        Xila.Task.Delay(20);
        Send_Instruction('C', 'T');
        break;
    case Xila.Open:
        Load_Registry();
        Refresh_Next_Alarm();
        Xila.Display.Set_Current_Page(Clock);
        break;
    case Xila.Minimize:
        break;
    case Xila.Close:
        Save_Registry();
        delete Instance_Pointer;
        Xila.Task.Delete();
        break;
    case Xila.Maximize:
        Xila.Display.Set_Current_Page(Clock);
        break;
    case Instruction('M', 'i'):
        Save_Registry();
        Xila.Software_Management.Minimize(Clock_Handle);
        break;
    case Xila.Hibernate:
    case Xila.Shutdown:
    case Xila.Restart:
    case Instruction('C', 'l'):
        Xila.Software_Management.Close(Clock_Handle);
        break;
    case Instruction('C', 'T'):
        Check_Timer();
        break;
    case Instruction('O', 'A'):
        Xila.Display.Set_Current_Page(Alarm);
        Instance_Pointer->Refresh_Alarm();
        Instance_Pointer->Refresh_Alarms();
        break;
    case Instruction('O', 'T'):
        Xila.Display.Set_Current_Page(Timer);
        Instance_Pointer->Refresh_Timer();
        break;
    case Instruction('O', 'c'): // Switch page Chronometer
        Xila.Display.Set_Current_Page(Chronometer);
        Instance_Pointer->Refresh_Chronometer();
        break;
    case Instruction('O', 'C'):
        Xila.Display.Set_Current_Page(Clock);
        Instance_Pointer->Refresh_Chronometer();
        break;
    case Instruction('R', 'A'):
        Xila.Sound.Play(Clock_File("Ringtone.wav")); // play something
        Next_Alarm = 0;
        snprintf(Temporary_Char_Array, sizeof(Temporary_Char_Array), "Alarm : %s", Next_Alarm_Title);
        Xila.Dialog.Event(F("Time over."), Xila.Information);
        Xila.Sound.Stop();
        Send_Instruction('R', 'e');
        break;
    case Instruction('S', 'R'):
        Instance_Pointer->Save_Registry();
        break;
    default:
        break;
    }
}

void Clock_Class::
    Clock_Instructions()
{
    Current_Instruction = Instance_Pointer->Get_Instruction();

    switch (Current_Instruction)
    {
    case Instruction('R', 'e'):
        Instance_Pointer->Refresh_Clock();
        Refresh_Next_Alarm();
        break;
    default:
        Instance_Pointer->Main_Instructions();
        break;
    }
}

void Clock_Class::Select_Alarm(uint8_t Alarm_To_Select)
{
    if (Alarm_Title[Alarm_To_Select][0] != '\0')
    {
        Selected_Alarm = Alarm_To_Select;
    }
    Send_Instruction('R', 'e');
}

void Clock_Class::Delete_Alarm()
{
    if (Selected_Alarm < 6)
    {
        Alarm_State[Selected_Alarm] = Disabled;
        memset(Alarm_Title[Selected_Alarm], '\0', sizeof(Alarm_Title[Selected_Alarm]));
        Alarm_Hour[Selected_Alarm] = 0;
        Alarm_Minute[Selected_Alarm] = 0;
    }
}

void Clock_Class::Alarm_Instructions()
{
    Current_Instruction = Instance_Pointer->Get_Instruction();

    switch (Current_Instruction)
    {
    case Instruction('R', 'e'):
        Refresh_Alarm();
        Refresh_Alarms();
        Refresh_Next_Alarm();
        break;
    case Instruction('K', 'T'):
        DUMP("TEST");
        DUMP(Selected_Alarm);
        Xila.Task.Delay(10);
        if (Selected_Alarm < 6 && Alarm_Title[Selected_Alarm][0] != '\0')
        {
            Xila.Dialog.Keyboard(Alarm_Title[Selected_Alarm], sizeof(Alarm_Title[Selected_Alarm]));
        }
        Send_Instruction('R', 'e');
        break;
    case Instruction('A', '0'): //Ax : Select alarm
        Select_Alarm(0);
        break;
    case Instruction('A', '1'):
        Select_Alarm(1);
        break;
    case Instruction('A', '2'):
        Select_Alarm(2);
        break;
    case Instruction('A', '3'):
        Select_Alarm(3);
        break;
    case Instruction('A', '4'):
        Select_Alarm(4);
        break;
    case Instruction('A', '5'):
        Select_Alarm(5);
        break;
    case Instruction('A', 'A'): //Add alarm
        Add_Alarm();
        break;
    case Instruction('D', 'A'): //Delete alarm
        Delete_Alarm();
        Send_Instruction('R', 'e');
        break;
    case Instruction('S', 'S'): //SS : switch alarm state
        if (Alarm_State[Selected_Alarm] == Enabled)
        {
            Alarm_State[Selected_Alarm] = Disabled;
        }
        else
        {
            Alarm_State[Selected_Alarm] = Enabled;
        }

        Send_Instruction('R', 'e');
        break;
    default:
        Instance_Pointer->Main_Instructions();
        break;
    }
}

void Clock_Class::Chronometer_Instructions()
{
    Current_Instruction = Get_Instruction();
    switch (Current_Instruction)
    {
    case Instruction('R', 'e'):
        Refresh_Chronometer();
        Refresh_Next_Alarm();
        break;
    case Instruction('S', 'P'): // SP : Start / Pause
        switch (Chronometer_State)
        {
        case Stopped: // Stoped -> Running
            Xila.Display.Set_Trigger(F("TIMER_TIM"), true);
            Chronometer_State = Running;
            Chronometer_Inital_Time = Xila.Time.Milliseconds();
            break;
        case Paused: // Paused -> Running
            Chronometer_Inital_Time += (Xila.Time.Milliseconds() - Chronometer_Paused_Time);
            Chronometer_State = Running;
            Xila.Display.Set_Trigger(F("TIMER_TIM"), true);
            Xila.Display.Set_Text(F("START_BUT"), F("Pause"));
            break;
        case Running: // Running -> Pause
            Chronometer_Paused_Time = Xila.Time.Milliseconds();
            Chronometer_State = Paused;
            Xila.Display.Set_Trigger(F("TIMER_TIM"), false);
            Xila.Display.Set_Text(F("START_BUT"), F("Start"));
            break;
        default:
            break;
        }

        Send_Instruction('R', 'e');
        break;
    case Instruction('L', 'R'): //LR : Lap / Reset
        Chronometer_Inital_Time = 0;
        Chronometer_Paused_Time = 0;
        Xila.Display.Set_Text(F("START_BUT"), F("Start"));
        Xila.Display.Set_Trigger(F("TIMER_TIM"), false);
        Chronometer_State = Stopped;
        Send_Instruction('R', 'e');
        break;
    default:
        Instance_Pointer->Main_Instructions();
        break;
    }
}

void Clock_Class::Timer_Instructions()
{
    Current_Instruction = Get_Instruction();
    switch (Current_Instruction)
    {
    case Instruction('R', 'e'):
        Refresh_Timer();
        Refresh_Next_Alarm();
        break;
    case Instruction('S', 'P'): // SP : Start / Pause
        switch (Timer_State)
        {
        case Stopped: //Stopped -> Start
            Timer_State = Running;
            Timer_Threshold_Time = Xila.Time.Milliseconds();
            Timer_Threshold_Time += (((((Hours * 60) + Minutes) * 60) + Seconds) * 1000);
            Xila.Display.Click(F("SETBUTTONS_BUT"), 0);
            Xila.Display.Set_Text(F("START_BUT"), F("Pause"));
            break;
        case Running: // Running -> Pause
            Timer_Paused_Time = Xila.Time.Milliseconds();
            Timer_State = Paused;
            Xila.Display.Set_Text(F("START_BUT"), F("Start"));
            break;
        case Paused: // Paused -> Start
            Timer_State = Running;
            Timer_Threshold_Time += (Xila.Time.Milliseconds() - Timer_Paused_Time);
            Xila.Display.Set_Text(F("START_BUT"), F("Pause"));
            break;
        default:
            break;
        }
        Send_Instruction('R', 'e');
        break;
    case Instruction('C', 'L'): // CL : clear
        Timer_State = Stopped;
        Hours = 0;
        Minutes = 0;
        Seconds = 0;
        Xila.Display.Set_Value(F("HOURS_NUM"), 0);
        Xila.Display.Set_Value(F("MINUTES_NUM"), 0);
        Xila.Display.Set_Value(F("SECONDS_NUM"), 0);

        Xila.Display.Click(F("SETBUTTONS_BUT"), 1);
        Xila.Display.Set_Text(F("START_BUT"), F("Start"));
        Send_Instruction('R', 'e');
        break;
    default:
        Instance_Pointer->Main_Instructions();
        break;
    }
}
