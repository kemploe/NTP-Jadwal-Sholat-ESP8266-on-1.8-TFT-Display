# NTP-Jadwal-Sholat-ESP8266-on-1.8-TFT-Display
NodeMCU ESP8266 - Jadwal Sholat berbasis NTP dengan 1.8" TFT SPI Display

![NTP Jadwal Sholat](https://github.com/kemploe/NTP-Jadwal-Sholat-ESP8266-on-1.8-TFT-Display/blob/main/images/photo_2022-07-02_06-09-52.jpg)

## 1. Introduction

This is basically a NTP clock running on NodeMCU ESP8266 with Adafruit compatible 1.8" TFT SPI display. Adding Jadwal Sholat (Salat Time) by requesting it from https://api.myquran.com on a daily basis and WiFiManager https://github.com/tzapu/WiFiManager to prevent hard coding SSID and Password. 

## 2. Bill of Material (BOM)
   - NodeMCU ESP8266        - https://bit.ly/3zTizP3 or equivalent. 
   - 1.8" TFT SPI Display   - https://bit.ly/3QDjysy or equivalent. 
   - Breadboard             - https://bit.ly/3xL2dFn or equivalent. 
   - Jumpers                - https://bit.ly/39KrPdJ or equivalent. 

## 3. Wiring the 1.8" TFT SPI display to NodeMCU
      Display <---> NodeMCU
          VCC <---> 5V or 3.3V
          GND <---> GND
           CS <---> GPIO 0
    RESET/RST <---> GPIO 2
        A0/DC <---> GPIO 4
     SDA/MOSI <---> GPIO 13
     SCK/SCLK <---> GPIO 14
       LED/BL <---> GPIO 5

![1.8" TFT SPI Display](https://github.com/kemploe/NTP-Jadwal-Sholat-ESP8266-on-1.8-TFT-Display/blob/main/images/2022-07-01_082841.png)

Note:
On some Adafruit compatible 1.8" TFT SPI display, RESET is RST, A0 is DC, SDA is MOSI, SCK is SCLK and LED is BL. So pay attention to your wiring, check it twice before powering up the device. 

## 4. How it works

Upon start up the WiFiManager kicks in. If a saved WiFi configuration found it will configure NodeMCU as a WiFi Station (STA) and connect to the WiFi Access Point that is configured in the saved configuration. Otherwise it will configure NodeMCU as an Access Point with SSID: "JamSholat" and spawn a web page that you can connect and configure using your Smartphone. Once the WiFi has been configured it will switch to STA mode and start the clock.

![Configure WiFi](https://github.com/kemploe/NTP-Jadwal-Sholat-ESP8266-on-1.8-TFT-Display/blob/main/images/2022-07-02_012612.png)

![Configure WiFi](https://github.com/kemploe/NTP-Jadwal-Sholat-ESP8266-on-1.8-TFT-Display/blob/main/images/2022-07-02_012636.png)

![Configure WiFi](https://github.com/kemploe/NTP-Jadwal-Sholat-ESP8266-on-1.8-TFT-Display/blob/main/images/2022-07-02_012656.png)

![Configure WiFi](https://github.com/kemploe/NTP-Jadwal-Sholat-ESP8266-on-1.8-TFT-Display/blob/main/images/2022-07-02_012714.png)

![Configure WiFi](https://github.com/kemploe/NTP-Jadwal-Sholat-ESP8266-on-1.8-TFT-Display/blob/main/images/2022-07-02_012730.png)

