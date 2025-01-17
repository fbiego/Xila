****************
Clipboard Module
****************

Here you will find a full description of the clipboard API.

This module is responsible to allow copy / paste feature.
It only offers an API, the graphical implementation remains the responsibility of the software.

API Reference
=============

.. doxygenclass::   Xila_Class::Clipboard_Class
    :members:

Example
=======

.. code-block:: C

    void Function_A()
    {
        char String_To_Copy[] = "String to copy";
        Xila.Clipboard.Copy(String_To_Copy, sizeof(String_To_Copy));    // -- Copy a string into the clipboard.
    }

    void Function_B()
    {
        char String_To_Paste[32];
        Xila.Clipboard.Paste(String_To_Paste, sizeof(String_To_Paste)); // -- String to paste from the clipboard.
        Xila.Clipboard.Clear(); // -- Clear the clipboard.
    }
    
