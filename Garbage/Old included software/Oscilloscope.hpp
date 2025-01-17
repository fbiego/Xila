///
 /// @file Oscilloscope.hpp
 /// @author Alix ANNERAUD (alix.anneraud@outlook.fr) - botofancalin
 /// @brief Oscilloscope software header file.
 /// @version 0.1
 /// @date 08-04-2021
 /// 
 /// @copyright Copyright (c) 2021
 /// 

#include "Xila.hpp"

#define Default_Channel_0_Pin 35
#define Default_Channel_1_Pin 36

class Oscilloscope_Class : private Xila_Class::Software
{
protected:
    static Oscilloscope_Class *Instance_Pointer;

    TaskHandle_t SigmaDelta_Handle;

    const uint8_t Waveform_ID = 4;

    uint8_t Current_Channel;

    char Temporary_Char_Array[16];

    // Constant

    const uint16_t LCD_WIDTH = 340;
    const uint16_t LCD_HEIGHT = 250;
    const uint16_t SAMPLES = 350;
    const uint16_t Dots_Per_Division = 75;

    uint8_t ad_ch0 = 35; // Analog 35 pin for channel 0
    uint8_t ad_ch1 = 36; // Analog 36 pin for channel 1

    const long VREF[5] = {250, 500, 1250, 2500, 5000};
    const int MILLIVOL_per_dot[5] = {33, 17, 6, 3, 2};

    // channel mode
    const int MODE_ON = 0;
    const int MODE_INV = 1;
    const int MODE_OFF = 2;

    const char *Modes[3] = {"Normal", "Inverted", "Off"};
    short ch0_mode = MODE_ON;
    short ch1_mode = MODE_OFF;

    // Triggering

    enum Trig_Modes
    {
        TRIG_AUTO = 0,
        TRIG_NORM = 1,
        TRIG_SCAN = 2
    };

    const char *TRIG_Modes[3] = {"Automa", "Normal", "Scan"};
    const int TRIG_E_UP = 0;
    const int TRIG_E_DN = 1;

    // Time rate
#define RATE_MIN 0
#define RATE_MAX 13

    const char *Rates[14] = {"F1-1", "F1-2", "F2", "5ms", "10ms", "20ms", "50ms", "0.1s", "0.2s", "0.5s", "1s", "2s", "5s", "10s"};
    short rate = 3;

    // Voltage range
#define RANGE_MIN 0
#define RANGE_MAX 4
    const char *Ranges[5] = {"1", "0.5", "0.2", "0.1", "0.05"};
    int range0 = RANGE_MIN;
    short range1 = RANGE_MIN;

    // Offset
    int ch0_off = 0;
    int ch1_off = 0;

    // Triggering mode
    short trig_mode = TRIG_AUTO;
    // Tringerring level
    int trig_lv = 40;
    // Trigger edge
    short trig_edge = TRIG_E_UP;
    short trig_ch = 0;
    bool Start;
    short menu = 19;

    uint8_t data[4][350]; // keep twice of the number of channels to make it a double buffer

    short sample = 0; // index for double buffer
    int amplitude = 0;
    int amplitudeStep = 5;

    ///////////////////////////////////////////////////////////////////////////////////////////////

    void Trigger();
    void Sampling();

    inline long adRead(uint8_t ch, short mode, int off)
    {
        long a = Xila.GPIO.Analog_Read(ch);
        a += off; // add offset
        if (ch == ad_ch0)
        {
            a = ((VREF[range0] * a) / 10000) + 30;
        }
        else
        {
            a = ((VREF[range1] * a) / 10000) + 30;
        }
        if (a > LCD_HEIGHT)
        {
            a = LCD_HEIGHT;
        }
        return a;
    }

    void Refresh_Waveform();

    void Refresh_Interface();

    void Loop();

    friend void Oscilloscope_Task(void *); //main task
    friend void SigmaDelta_Task(void *);   // used to generate sigmadelta signal

public:
    static Xila_Class::Task_Function Main_Task(void *);

    //static void SigmaDelta_Task(void*);

    static Xila_Class::Software *Load();

    enum Picture_ID
    {
        Icon_32 = Xila.Display.Oscilloscope_Images
    };

    Oscilloscope_Class();
    ~Oscilloscope_Class();
};

Xila_Class::Software_Handle Oscilloscope_Handle("Oscilloscope", Oscilloscope_Class::Icon_32, Oscilloscope_Class::Load);
