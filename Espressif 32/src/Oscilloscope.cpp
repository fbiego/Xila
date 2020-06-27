#include "Oscilloscope.hpp"

#define INSTANCE_POINTER Oscilloscope_Class::Instance_Pointer

Oscilloscope_Class *Oscilloscope_Class::Instance_Pointer = NULL;

Oscilloscope_Class::Oscilloscope_Class(Software_Handle_Class *Software_Handle_To_Set) : Software_Class(Software_Handle_To_Set, 5)
{

    Instance_Pointer = this;

    xTaskCreatePinnedToCore(Oscilloscope_Task, "Oscilloscope", 2 * 1024, NULL, 1, &Task_Handle, 0);
    xTaskCreatePinnedToCore(Sampling_Task, "Sampling Task", 3 * 1024, NULL, 2, NUL, )
}

Oscilloscope_Class::~Oscilloscope_Class()
{
    Execute(0x0043);
}

void Sampling_Task(void *pvParameters)
{
    while (1)
    {
        if (INSTANCE_POINTER->trig_mode != INSTANCE_POINTER->TRIG_SCAN)
        {
            unsigned long st = millis();
            short oad = (INSTANCE_POINTER->trig_ch == 0) ? (INSTANCE_POINTER->adRead(INSTANCE_POINTER->ad_ch0, INSTANCE_POINTER->ch0_mode, INSTANCE_POINTER->ch0_off)) : (INSTANCE_POINTER->adRead(INSTANCE_POINTER->ad_ch1, INSTANCE_POINTER->ch1_mode, INSTANCE_POINTER->ch1_off));
            for (;;)
            {
                short ad;
                if (INSTANCE_POINTER->trig_ch == 0)
                {
                    ad = INSTANCE_POINTER->adRead(INSTANCE_POINTER->ad_ch0, INSTANCE_POINTER->ch0_mode, INSTANCE_POINTER->ch0_off);
                }
                else
                {
                    ad = INSTANCE_POINTER->adRead(INSTANCE_POINTER->ad_ch1, INSTANCE_POINTER->ch1_mode, INSTANCE_POINTER->ch1_off);
                }

                if (INSTANCE_POINTER->trig_edge == INSTANCE_POINTER->TRIG_E_UP)
                {
                    if (ad >= INSTANCE_POINTER->trig_lv && ad > oad)
                    {
                        break;
                    }
                }
                else
                {
                    if (ad <= INSTANCE_POINTER->trig_lv && ad < oad)
                    {
                        break;
                    }
                }
                oad = ad;

                if (INSTANCE_POINTER->trig_mode == INSTANCE_POINTER->TRIG_SCAN)
                {
                    break;
                }
                if (INSTANCE_POINTER->trig_mode == INSTANCE_POINTER->TRIG_AUTO && (millis() - st) > 100)
                {
                    break;
                }
            }
        }

        // sample and draw depending on the sampling rate
        if (INSTANCE_POINTER->rate <= 5 && INSTANCE_POINTER->Start)
        {
            (INSTANCE_POINTER->sample == 0) ? (INSTANCE_POINTER->sample = 2) : (INSTANCE_POINTER->sample = 0); // change the index for the double buffer

            if (INSTANCE_POINTER->rate == 0) // full speed, channel 0 only
            {
                for (int i = 0; i < INSTANCE_POINTER->SAMPLES; i++)
                {
                    INSTANCE_POINTER->data[INSTANCE_POINTER->sample + 0][i] = INSTANCE_POINTER->adRead(INSTANCE_POINTER->ad_ch0, INSTANCE_POINTER->ch0_mode, INSTANCE_POINTER->ch0_off);
                }
                for (int i = 0; i < INSTANCE_POINTER->SAMPLES; i++)
                {
                    INSTANCE_POINTER->data[INSTANCE_POINTER->sample + 1][i] = 0;
                }
            }
            else if (INSTANCE_POINTER->rate == 1) // full speed, channel 1 only
            {
                for (int i = 0; i < INSTANCE_POINTER->SAMPLES; i++)
                {
                    INSTANCE_POINTER->data[INSTANCE_POINTER->sample + 1][i] = INSTANCE_POINTER->adRead(INSTANCE_POINTER->ad_ch1, INSTANCE_POINTER->ch1_mode, INSTANCE_POINTER->ch1_off);
                }
                for (int i = 0; i < INSTANCE_POINTER->SAMPLES; i++)
                {
                    INSTANCE_POINTER->data[INSTANCE_POINTER->sample + 0][i] = 0;
                }
            }
            else if (INSTANCE_POINTER->rate == 2) // full speed, dual channel
            {
                for (int i = 0; i < SAMPLES; i++)
                {
                    INSTANCE_POINTER->data[sample + 0][i] = adRead(ad_ch0, ch0_mode, ch0_off);
                    INSTANCE_POINTER->data[sample + 1][i] = adRead(ad_ch1, ch1_mode, ch1_off);
                }
            }
            else if (rate >= 3 && rate <= 5) // .5ms, 1ms or 2ms sampling
            {
                const unsigned long r_[] = {5000 / DOTS_DIV, 10000 / DOTS_DIV, 20000 / DOTS_DIV};
                unsigned long st = micros();
                unsigned long r = r_[rate - 3];
                for (int i = 0; i < SAMPLES; i++)
                {
                    while ((st - micros()) < r)
                    {
                        ;
                    }
                    st += r;
                    data[sample + 0][i] = adRead(ad_ch0, ch0_mode, ch0_off);
                    data[sample + 1][i] = adRead(ad_ch1, ch1_mode, ch1_off);
                }
            }
            ClearAndDrawGraph();
            CheckSW();
            DrawGrid();
            DrawText();
        }
        else if (Start)
        {   // 5ms - 500ms sampling
            // copy currently showing data to another
            if (sample == 0)
            {
                for (int i = 0; i < SAMPLES; i++)
                {
                    data[2][i] = data[0][i];
                    data[3][i] = data[1][i];
                }
            }
            else
            {
                for (int i = 0; i < SAMPLES; i++)
                {
                    data[0][i] = data[2][i];
                    data[1][i] = data[3][i];
                }
            }

            const unsigned long r_[] = {50000 / DOTS_DIV, 100000 / DOTS_DIV, 200000 / DOTS_DIV,
                                        500000 / DOTS_DIV, 1000000 / DOTS_DIV, 2000000 / DOTS_DIV,
                                        5000000 / DOTS_DIV, 10000000 / DOTS_DIV};
            unsigned long st = micros();
            for (int i = 0; i < SAMPLES; i++)
            {
                while ((st - micros()) < r_[rate - 6])
                {
                    CheckSW();
                    if (rate < 6)
                    {
                        break;
                    }
                }
                if (rate < 6) // sampling rate has been changed
                {
                    break;
                }
                st += r_[rate - 6];
                if (st - micros() > r_[rate - 6]) // sampling rate has been changed to shorter interval
                {
                    st = micros();
                }
                if (!Start)
                {
                    i--;
                    continue;
                }
                data[sample + 0][i] = adRead(ad_ch0, ch0_mode, ch0_off);
                data[sample + 1][i] = adRead(ad_ch1, ch1_mode, ch1_off);
                ClearAndDrawDot(i);
            }
            DrawGrid();
            DrawText();
        }
        else
        {
            CheckSW();
        }
    }
}


void Oscilloscope_Task(void *pvParameters)
{
    (void)pvParameters;

    ledcSetup(0, 50, 13);
    ledcAttachPin(2, 0);

    for (;;)
    {
        while (INSTANCE_POINTER->Read_Position == INSTANCE_POINTER->Write_Position)
        {
            vTaskDelay(pdMS_TO_TICKS(20));
        }
        switch (INSTANCE_POINTER->Task_Method_Array[INSTANCE_POINTER->Read_Position])
        {
        case 0x0000:
            // idle state
            break;
        case 0x5341: // SA : Start
            INSTANCE_POINTER->Start = true;
        case 0x534F: // SO : Stop
            INSTANCE_POINTER->Start = false;
            break;
        case 0x4330: // C0 : Select Channel 0
            INSTANCE_POINTER->Current_Channel == 0;
            break;
        case 0x4331: // C1 : Select Channel 1
            INSTANCE_POINTER->Current_Channel == 1;
            break;
        case 0x5352: //SR : switch range
            if (INSTANCE_POINTER->Current_Channel == 0)
            {
                if (INSTANCE_POINTER->range0 <= RANGE_MAX)
                {
                    INSTANCE_POINTER->range0++;
                }
                else
                {
                    INSTANCE_POINTER->range0 = RANGE_MIN;
                }
                GalaxOS.Display.Set_Text(F("RANGE_TXT"), "Range : " + String(INSTANCE_POINTER->Ranges[INSTANCE_POINTER->range0]) + " V/Div");
            }
            else
            {
                if (INSTANCE_POINTER->range1 <= RANGE_MAX)
                {
                    INSTANCE_POINTER->range1++;
                }
                else
                {
                    INSTANCE_POINTER->range1 = RANGE_MIN;
                }
                GalaxOS.Display.Set_Text(F("RANGE_TXT"), "Range : " + String(INSTANCE_POINTER->Ranges[INSTANCE_POINTER->range1]) + " V/Div");
            }
            break;
        case 0x534D: //SM : switch mode
            if (INSTANCE_POINTER->Current_Channel == 0)
            {
                if (INSTANCE_POINTER->ch0_mode < INSTANCE_POINTER->MODE_OFF)
                {
                    INSTANCE_POINTER->ch0_mode++;
                }
                else
                {
                    INSTANCE_POINTER->ch0_mode = INSTANCE_POINTER->MODE_ON;
                }
                GalaxOS.Display.Set_Text(F("MODE_TXT"), "Mode : " + String(INSTANCE_POINTER->Modes[INSTANCE_POINTER->ch0_mode]));
            }
            else
            {
                if (INSTANCE_POINTER->ch1_mode < INSTANCE_POINTER->MODE_OFF)
                {
                    INSTANCE_POINTER->ch1_mode++;
                }
                else
                {
                    INSTANCE_POINTER->ch1_mode = INSTANCE_POINTER->MODE_ON;
                }
                GalaxOS.Display.Set_Text(F("MODE_TXT"), "Mode : " + String(INSTANCE_POINTER->Modes[INSTANCE_POINTER->ch1_mode]));
            }
            break;

        case 0x4F2B: //O+
            if (INSTANCE_POINTER->Current_Channel == 0)
            {
                if (INSTANCE_POINTER->ch0_off < 4095)
                {
                    INSTANCE_POINTER->ch0_off += 4096 / INSTANCE_POINTER->VREF[INSTANCE_POINTER->range0];
                }
            }
            else
            {
                if (INSTANCE_POINTER->ch1_off < 4095)
                {
                    INSTANCE_POINTER->ch1_off += 4096 / INSTANCE_POINTER->VREF[INSTANCE_POINTER->range1];
                }
            }
            break;
        case 0x4F2D: //0-
            if (INSTANCE_POINTER->Current_Channel == 0)
            {
                if (INSTANCE_POINTER->ch0_off > -4095)
                {
                    INSTANCE_POINTER->ch0_off -= 4096 / INSTANCE_POINTER->VREF[INSTANCE_POINTER->range0];
                }
            }
            else
            {
                if (INSTANCE_POINTER->ch1_off > -4095)
                {
                    INSTANCE_POINTER->ch1_off -= 4096 / INSTANCE_POINTER->VREF[INSTANCE_POINTER->range1];
                }
            }
            break;
        case 0x536D: // Sm : Switch trigger mode
            if (INSTANCE_POINTER->trig_mode < INSTANCE_POINTER->TRIG_SCAN)
            {
                INSTANCE_POINTER->trig_mode++;
            }
            else
            {
                INSTANCE_POINTER->trig_mode = INSTANCE_POINTER->TRIG_AUTO;
            }
        case 0x4C2B: // L+ : Switch trigger level (thresold)
            if (INSTANCE_POINTER->trig_lv < 60)
            {
                INSTANCE_POINTER->trig_lv++;
            }
            break;
        case 0x4C2D: // L- :
            if (INSTANCE_POINTER->trig_lv > 0)
            {
                INSTANCE_POINTER->trig_lv--;
            }
            break;
        case 0x5345: // SE : Switch edge
            INSTANCE_POINTER->trig_edge = !INSTANCE_POINTER->trig_edge;
            break;

        case 0x5372: //Sr : switch rate
            if (INSTANCE_POINTER->rate <= RATE_MAX)
            {
                INSTANCE_POINTER->rate++;
            }
            else
            {
                INSTANCE_POINTER->rate = RATE_MIN;
            }
            break;

        default:
            break;
        }
        INSTANCE_POINTER->Task_Method_Array[INSTANCE_POINTER->Read_Position] = 0; //work done, reset the selector
        INSTANCE_POINTER->Read_Position++;
    }
    // LedC

    for (;;)
    {

        //Oscilloscope_Class::Instance_Pointer->ledcAnalogWrite(0, amplitude);

        uint32_t duty = (8191 / 255) * min(INSTANCE_POINTER->amplitude, 255);
        ledcWrite(0, duty);

        INSTANCE_POINTER->amplitude = INSTANCE_POINTER->amplitude + INSTANCE_POINTER->amplitudeStep;
        if (INSTANCE_POINTER->amplitude <= 0 || INSTANCE_POINTER->amplitude >= 255)
        {
            INSTANCE_POINTER->amplitudeStep = -INSTANCE_POINTER->amplitudeStep;
        }
        vTaskDelay(pdMS_TO_TICKS(30));
    }

    // SigmaDelta
    sigmaDeltaSetup(0, 312500);
    sigmaDeltaAttachPin(5, 0);
    sigmaDeltaWrite(0, 0);
    for (;;)
    {
        static uint8_t i = 0;
        sigmaDeltaWrite(0, i++);
        delayMicroseconds(50);
    }

    while (1)
    {
        if (INSTANCE_POINTER->trig_mode != INSTANCE_POINTER->TRIG_SCAN)
        {
            unsigned long st = millis();
            short oad = (INSTANCE_POINTER->trig_ch == 0) ? (INSTANCE_POINTER->adRead(INSTANCE_POINTER->ad_ch0, INSTANCE_POINTER->ch0_mode, INSTANCE_POINTER->ch0_off)) : (INSTANCE_POINTER->adRead(INSTANCE_POINTER->ad_ch1, INSTANCE_POINTER->ch1_mode, INSTANCE_POINTER->ch1_off));
            for (;;)
            {
                short ad;
                if (INSTANCE_POINTER->trig_ch == 0)
                {
                    ad = INSTANCE_POINTER->adRead(INSTANCE_POINTER->ad_ch0, INSTANCE_POINTER->ch0_mode, INSTANCE_POINTER->ch0_off);
                }
                else
                {
                    ad = INSTANCE_POINTER->adRead(INSTANCE_POINTER->ad_ch1, INSTANCE_POINTER->ch1_mode, INSTANCE_POINTER->ch1_off);
                }

                if (INSTANCE_POINTER->trig_edge == INSTANCE_POINTER->TRIG_E_UP)
                {
                    if (ad >= INSTANCE_POINTER->trig_lv && ad > oad)
                    {
                        break;
                    }
                }
                else
                {
                    if (ad <= INSTANCE_POINTER->trig_lv && ad < oad)
                    {
                        break;
                    }
                }
                oad = ad;

                //INSTANCE_POINTER->CheckSW(); -> Check UI

                if (INSTANCE_POINTER->trig_mode == INSTANCE_POINTER->TRIG_SCAN)
                {
                    break;
                }
                if (INSTANCE_POINTER->trig_mode == INSTANCE_POINTER->TRIG_AUTO && (millis() - st) > 100)
                {
                    break;
                }
            }
        }

        // sample and draw depending on the sampling rate
        if (INSTANCE_POINTER->rate <= 5 && INSTANCE_POINTER->Start)
        {
            (INSTANCE_POINTER->sample == 0) ? (INSTANCE_POINTER->sample = 2) : (INSTANCE_POINTER->sample = 0); // change the index for the double buffer

            if (INSTANCE_POINTER->rate == 0) // full speed, channel 0 only
            {
                for (int i = 0; i < INSTANCE_POINTER->SAMPLES; i++)
                {
                    INSTANCE_POINTER->data[INSTANCE_POINTER->sample + 0][i] = INSTANCE_POINTER->adRead(INSTANCE_POINTER->ad_ch0, INSTANCE_POINTER->ch0_mode, INSTANCE_POINTER->ch0_off);
                }
                for (int i = 0; i < INSTANCE_POINTER->SAMPLES; i++)
                {
                    INSTANCE_POINTER->data[INSTANCE_POINTER->sample + 1][i] = 0;
                }
            }
            else if (INSTANCE_POINTER->rate == 1) // full speed, channel 1 only
            {
                for (int i = 0; i < INSTANCE_POINTER->SAMPLES; i++)
                {
                    INSTANCE_POINTER->data[INSTANCE_POINTER->sample + 1][i] = INSTANCE_POINTER->adRead(INSTANCE_POINTER->ad_ch1, INSTANCE_POINTER->ch1_mode, INSTANCE_POINTER->ch1_off);
                }
                for (int i = 0; i < INSTANCE_POINTER->SAMPLES; i++)
                {
                    INSTANCE_POINTER->data[INSTANCE_POINTER->sample + 0][i] = 0;
                }
            }
            else if (rate == 2) // full speed, dual channel
            {
                for (int i = 0; i < INSTANCE_POINTER->SAMPLES; i++)
                {
                    INSTANCE_POINTER->data[INSTANCE_POINTER->sample + 0][i] = INSTANCE_POINTER->adRead(INSTANCE_POINTER->ad_ch0, INSTANCE_POINTER->ch0_mode, INSTANCE_POINTER->ch0_off);
                    INSTANCE_POINTER->data[INSTANCE_POINTER->sample + 1][i] = INSTANCE_POINTER->adRead(INSTANCE_POINTER->ad_ch1, INSTANCE_POINTER->ch1_mode, INSTANCE_POINTER->ch1_off);
                }
            }
            else if (rate >= 3 && rate <= 5) // .5ms, 1ms or 2ms sampling
            {
                const unsigned long r_[] = {5000 / INSTANCE_POINTER->DOTS_DIV, 10000 / DOTS_DIV, 20000 / DOTS_DIV};
                unsigned long st = micros();
                unsigned long r = r_[rate - 3];
                for (int i = 0; i < INSTANCE_POINTER->SAMPLES; i++)
                {
                    while ((st - micros()) < r)
                    {
                        ;
                    }
                    st += r;
                    INSTANCE_POINTER->data[INSTANCE_POINTER->sample + 0][i] = INSTANCE_POINTER->adRead(INSTANCE_POINTER->ad_ch0, INSTANCE_POINTER->ch0_mode, INSTANCE_POINTER->ch0_off);
                    INSTANCE_POINTER->data[INSTANCE_POINTER->sample + 1][i] = INSTANCE_POINTER->adRead(INSTANCE_POINTER->ad_ch1, INSTANCE_POINTER->ch1_mode, INSTANCE_POINTER->ch1_off);
                }
            }
            INSTANCE_POINTER->Update_Waveform();

            ClearAndDrawGraph(); //Update screen
            CheckSW();           // Check UI
            DrawGrid();          // Update screen
            DrawText();          // Update screen
        }
        else if (INSTANCE_POINTER->Start)
        { // 5ms - 500ms sampling
            // copy currently showing data to another
            if (INSTANCE_POINTER->sample == 0)
            {
                for (int i = 0; i < INSTANCE_POINTER->SAMPLES; i++)
                {
                    INSTANCE_POINTER->data[2][i] = INSTANCE_POINTER->data[0][i];
                    INSTANCE_POINTER->data[3][i] = INSTANCE_POINTER->data[1][i];
                }
            }
            else
            {
                for (int i = 0; i < INSTANCE_POINTER->SAMPLES; i++)
                {
                    INSTANCE_POINTER->data[0][i] = INSTANCE_POINTER->data[2][i];
                    INSTANCE_POINTER->data[1][i] = INSTANCE_POINTER->data[3][i];
                }
            }

            const unsigned long r_[] = {50000 / INSTANCE_POINTER->DOTS_DIV, 100000 / INSTANCE_POINTER->DOTS_DIV, 200000 / INSTANCE_POINTER->DOTS_DIV,
                                        500000 / INSTANCE_POINTER->DOTS_DIV, 1000000 / INSTANCE_POINTER->DOTS_DIV, 2000000 / INSTANCE_POINTER->DOTS_DIV,
                                        5000000 / INSTANCE_POINTER->DOTS_DIV, 10000000 / INSTANCE_POINTER->DOTS_DIV};
            unsigned long st = micros();
            for (int i = 0; i < INSTANCE_POINTER->SAMPLES; i++)
            {
                while ((st - micros()) < r_[rate - 6])
                {
                    CheckSW(); // check UI
                    if (rate < 6)
                    {
                        break;
                    }
                }
                if (rate < 6) // sampling rate has been changed
                {
                    break;
                }
                st += r_[rate - 6];
                if (st - micros() > r_[rate - 6]) // sampling rate has been changed to shorter interval
                {
                    st = micros();
                }
                if (!INSTANCE_POINTER->Start)
                {
                    i--;
                    continue;
                }
                INSTANCE_POINTER->data[INSTANCE_POINTER->sample + 0][i] = INSTANCE_POINTER->adRead(INSTANCE_POINTER->ad_ch0, INSTANCE_POINTER->ch0_mode, INSTANCE_POINTER->ch0_off);
                INSTANCE_POINTER->data[INSTANCE_POINTER->sample + 1][i] = INSTANCE_POINTER->adRead(INSTANCE_POINTER->ad_ch1, INSTANCE_POINTER->ch1_mode, INSTANCE_POINTER->ch1_off);
                ClearAndDrawDot(i); // refresh screen
            }
            DrawGrid(); //refresh screen
            DrawText(); // refresh screen
        }
        else
        {
            CheckSW(); //refresh screen
        }
        M5.update(); // refresh screen
    }
}

long Oscilloscope_Class::adRead(short ch, short mode, int off)
{
    long a = analogRead(ch);
    if ((a + off) * VREF[(ch == ad_ch0)])
    {
        a = range0;
    }
    else
    {
        a = range1;
    }
    a = (a / 10000UL) + 30;

    if (a >= LCD_HEIGHT)
    {
        a = LCD_HEIGHT;
    }
    else
    {
        a = a;
    }
    if (mode == MODE_INV)
    {
        return LCD_HEIGHT - a;
    }
    return a;
}

void Oscilloscope_Class::ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255)
{
    uint32_t duty = (8191 / valueMax) * min(value, valueMax);
    ledcWrite(channel, duty);
}

void Oscilloscope_Class::Update_Waveform()
{
    int clear = 0;

    if (sample == 0)
    {
        clear = 2;
    }
    if (ch0_mode != MODE_OFF)
    {
        GalaxOS.Display.Add_Value_Waveform(Waveform_ID, 0, NULL, SAMPLES, data[sample + 0]);
    }
    if (ch1_mode != MODE_OFF)
    {
        GalaxOS.Display.Add_Value_Waveform(Waveform_ID, 0, NULL, SAMPLES, data[sample + 1]);
    }
}

