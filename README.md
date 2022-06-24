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


3. Wiring the 1.8" TFT display to NodeMCU

    Display - NodeMCU
    
      Vcc   -   5V or 3.3V
      
      GND   -   GND
      
      CS    -   GPIO 0
      
     RESET  -   GPIO 2
     
      A0    -   GPIO 4
      SDA   -   GPIO 13
      
      SCK   -   GPIO 14
      
      LED   -   GPIO 5
        



