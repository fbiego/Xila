*************************
Display Abstraction Layer
*************************

Here you will find a full description of the display abstraction layer.

This abstraction layer is responsible to drive the display.
In addition, this API supports resource protection to avoid data collisions when sending information.
This API rest upon the Nextion Library. For the full API, please visit the library page.

.. warning::
    Because Nextion displays use a serial interface (U.A.R.T.) to communicate, the exchange and processing of information is slow.
    To avoid saturation of this interface, it is recommended to deport as many commands as possible directly on the display.

API Reference
=============

.. doxygenclass::   Xila_Class::Display_Class
    :members:

Example
=======

.. code-block:: c

    Xila.Display.Begin(921600, 16, 17); // -- Initialize communication with the screen to 921600 baud at pin 16 and 17.
    Xila.Display.Set_Current_Page(39);  // -- Change page to 39.
    Xila.Display.Draw_Rectangle(20, 40, 100, 200, 65535, false);    // -- Draws a filled rectangle at coordinates (20, 40) with a width of 100 and a length of 200.
    Xila.Display.Set_Text("Text", "Hello World");   // -- Set "Hello World" text in Text object.
    Xila.Display.Set_Brightness(50, false); // -- Set the brightness to half without saving it (display EEPROM).
    Xila.Display.Sleep();   // -- Turn off the display.
