# NTP-Jadwal-Sholat-ESP8266-on-TFT-1.8-Display
NodeMCU ESP8266 - Jadwal Sholat berbasis NTP dengan 1.8" TFT SPI Display

1. Introduction

    This is basically a NTP clock running on NodeMCU ESP8266 with Adafruit compatible 1.8" TFT SPI display. 
    
    Adding Jadwal Sholat (Salat Time) by requesting it from https://api.myquran.com on a daily basis.


2. Bill of Material (BOM)
   - NodeMCU ESP8266        - https://bit.ly/3zTizP3 or equivalent. 
   - 1.8" TFT SPI Display   - https://bit.ly/3QDjysy or equivalent. 
   - Breadboard             - https://bit.ly/3xL2dFn or equivalent. 
   - Jumpers                - https://bit.ly/39KrPdJ or equivalent. 


3. Wiring the 1.8" TFT SPI display to NodeMCU

            Display <---> NodeMCU

                VCC <---> 5V or 3.3V

                GND <---> GND

                 CS <---> GPIO 0
        
          RESET/RST <---> GPIO 2

              A0/DC <---> GPIO 4

           SDA/MOSI <---> GPIO 13

           SCK/SCLK <---> GPIO 14

             BL/LED <---> GPIO 5
        
Note:

On some Adafruit compatible 1.8" TFT SPI display, RESET is RST, A0 is DC, SDA is MOSI, SCK is SCLK and LED is BL. So pay attention to your wiring, check it twice before powering up the device. 

4. Operating the Clock




