# NTP-Jadwal-Sholat-ESP8266-on-1.8-TFT-Display
NodeMCU ESP8266 - Jadwal Sholat berbasis NTP dengan 1.8" TFT SPI Display

NodeMCU ESP8266 - NTP driven islamic prayer times with 1.8" TFT SPI Display

![NTP Jadwal Sholat](https://github.com/kemploe/NTP-Jadwal-Sholat-ESP8266-on-1.8-TFT-Display/blob/main/images/photo_2022-07-02_06-09-52.jpg)

## 1. Introduction

This is basically a NTP clock running on NodeMCU ESP8266 with Adafruit compatible 1.8" TFT SPI display. Adding WiFiManager https://github.com/tzapu/WiFiManager to prevent hard coding SSID and Password. Adding Jadwal Sholat (Islamic Prayer Times) by requesting it on a daily basis from https://api.myquran.com which on the other hand had the data scrapped from Direktorat Jenderal Bimas Islam Kementerian Agama Republik Indonesia https://bimasislam.kemenag.go.id/jadwalshalat

For more information regarding the validity of the data, please visit the respective web site from the above given links.

## 2. Bill of Material (BOM)
   - NodeMCU ESP8266        - https://bit.ly/3zTizP3 or equivalent. 
   - 1.8" TFT SPI Display   - https://bit.ly/3QDjysy or equivalent. 
   - Breadboard             - https://bit.ly/3xL2dFn or equivalent. 
   - Jumpers                - https://bit.ly/39KrPdJ or equivalent. 

## 3. Cabling/Wiring the 1.8" TFT SPI display to NodeMCU
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
On some Adafruit compatible 1.8" TFT SPI display, RESET is RST, A0 is DC, SDA is MOSI, SCK is SCLK and LED is BL. So pay attention to your wiring, double check before powering up the device. 

## 4. Operation

Upon start up the WiFiManager kicks in. If a saved WiFi configuration found it will configure NodeMCU as a WiFi Station (STA) and connect to the WiFi Access Point that is configured in the saved configuration. Otherwise it will configure NodeMCU as an Access Point with SSID: "JamSholat" and spawn a web page (WiFiManager portal) that you can connect to and configure using your Smartphone. Once the WiFi has been configured it will switch to STA mode and start the clock.


![Configure WiFi](https://github.com/kemploe/NTP-Jadwal-Sholat-ESP8266-on-1.8-TFT-Display/blob/main/images/2022-07-02_012612.png)

1. In "WiFi" tap on "JamSholat", wait for the "WiFiManager" portal.



![Configure WiFi](https://github.com/kemploe/NTP-Jadwal-Sholat-ESP8266-on-1.8-TFT-Display/blob/main/images/2022-07-02_012636.png)

2. In "WiFiManager" portal, tap on "Configure WiFi".


![Configure WiFi](https://github.com/kemploe/NTP-Jadwal-Sholat-ESP8266-on-1.8-TFT-Display/blob/main/images/2022-07-02_012656.png)

3. In "Configure WiFi", tap on your WiFi SSID.


![Configure WiFi](https://github.com/kemploe/NTP-Jadwal-Sholat-ESP8266-on-1.8-TFT-Display/blob/main/images/2022-07-02_012714.png)

4. In "Configure WiFi", in "Password", provide the password for your WiFi then tap on "Save"

![Configure WiFi](https://github.com/kemploe/NTP-Jadwal-Sholat-ESP8266-on-1.8-TFT-Display/blob/main/images/2022-07-02_012730.png)

Once it is connected to the internet, it will start to synchronize and fetch the unix epoch from the configured NTP server. You do not need to adjust the date and time manually.

NTP sync status is displayed on the upper left side display inside the vertical rectangle. NTP is displayed in green means it is synced, NTP is displayed in yellow means it is synced in the last 1 - 24 hour, NTP is displayed in red means it is out of sync.
