#include <Adafruit_ST7735.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ezTime.h>
#include <NTPClient.h>
#include <WiFiClientSecure.h>
#include <WiFiManager.h>
#include <WiFiUdp.h>

// Fixed parameters
const int   time_zone = +7;                 // WIB (UTC + 7)
const char * ntp_pool = "id.pool.ntp.org";  // NTP Server pool address
const long ntp_update = 60000;              // NTP Client update interval in millisecond (ms)
const int     id_kota = 1301;               // See https://api.myquran.com/v1/sholat/kota/semua
const int  duty_cycle = 72;                 // TFT brightness using PWM duty cycle (0-255)
String   new_hostname = "JamSholat";

// Buffers for JSON payload string to character conversion
char b_imsak[10];
char b_subuh[10];
char b_terbit[10];
char b_dhuha[10];
char b_dzuhur[10];
char b_ashar[10];
char b_maghrib[10];
char b_isya[10];

// Elapsed time since 1 Jan 1970 in seconds
unsigned long unix_epoch;

// Pin assignment for PWM output to set TFT backlight brightness
uint8_t led_pin = 5;           // TFT LED/BL    pin is connected to NodeMCU pin D1 (GPIO 5)

// Pin assignment for 1.8" TFT display with ST7735
#define TFT_A0    4            // TFT DC/A0     pin is connected to NodeMCU pin D2 (GPIO 4)
#define TFT_CS    0            // TFT CS        pin is connected to NodeMCU pin D3 (GPIO 0)
#define TFT_RST   2            // TFT RST/RESET pin is connected to NodeMCU pin D4 (GPIO 2)
#define TFT_SCK   14           // TFT SCK/SCLK  pin is connected to NodeMCU pin D5 (GPIO 14)
#define TFT_SDA   13           // TFT SDA/MOSI  pin is connected to NodeMCU pin D7 (GPIO 13)

// 1.8" TFT display with ST7735
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_A0, TFT_RST);

// Convert RGB colors
#define RGB(r, g, b) ((( r & 0xF8 ) << 8)|(( g & 0xFC ) << 3 )|( b >> 3 ))

// TFT display colors
#define BLACK       RGB(  0,   0,   0)
#define AQUAMARINE  RGB(127, 255, 212)
#define GREY        RGB(128, 128, 128)
#define DARKGREY    RGB(169, 169, 169)
#define LIGHTGREY   RGB(211, 211, 211)
#define TURQUOISE   RGB( 64, 224, 208)
#define PINK        RGB(255, 128, 192)
#define OLIVE       RGB(128, 128,   0)
#define PURPLE      RGB(128,   0, 128)
#define AZURE       RGB(  0, 128, 255)
#define ORANGE      RGB(255, 128,  64)
#define CYAN        RGB(  0, 255, 255)
#define DARKCYAN    RGB(  0, 139, 139)
#define RED         RGB(255,   0,   0)
#define YELLOW      RGB(255, 255,   0)
#define WHITE       RGB(255, 255, 255)
#define BLUE        RGB(  0,   0, 255)
#define GREEN       RGB(  0, 255,   0)

#define SYNC_MARGINAL 3600             // orange status if no sync for 1 hour
#define SYNC_LOST 86400                // red status if no sync for 1 day

// NTP client setup, this must be done before setup
  int utc_offset = ( time_zone * 3600 );
  WiFiUDP ntpUDP;
  NTPClient timeClient( ntpUDP, ntp_pool, utc_offset, ntp_update );

// SETUP
void setup()
{
  // initializing 1.8" TFT display
  analogWrite(led_pin, duty_cycle);   // set display brightness
  tft.initR(INITR_BLACKTAB);          // initialize TFT display with ST7735 chip
  tft.setRotation(1);                 // set display orientation
  tft.fillScreen(BLACK);              // blanking display

  // initializing Serial Port
  Serial.begin(115200);               // set serial port speed
  delay (3000);                       // 3 seconds delay

  // WiFiManager, Local initialization. 
  // Once its business is done, there is no need to keep it around
  WiFiManager wfm;

    // Supress Debug information
    wfm.setDebugOutput(true);

    // reset settings - wipe stored credentials for testing
    // these are stored by the esp library
    // wfm.resetSettings();

    // Automatically connect using saved credentials,
    // if connection fails, it starts an access point with the specified name ( "AutoConnectAP" ),
    // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
    // then goes into a blocking loop awaiting configuration and will return success result

    Serial.println("WiFi connecting");
    tft.setCursor(38, 20);                    // move cursor to position (38, 20) pixel
    tft.print("WiFi connecting");

    if (!wfm.autoConnect( "JamSholat" )) {
      // Did not connect, print error message
      Serial.println("failed to connect and hit timeout");
      tft.setCursor(40, 30);                  // move cursor to position (40, 30) pixel
      tft.print("failed to connect and hit timeout");
   
      // Reset and try again
      ESP.restart();
      delay(1000);
    }
 
    // Connected!
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    tft.setCursor(40, 40);                   // move cursor to position (40, 40) pixel
    tft.print("WiFi connected");
    tft.setCursor(40, 60);                   // move cursor to position (40, 60) pixel
    tft.print(WiFi.localIP());
    delay(3000);

  // set hostname
  WiFi.hostname(new_hostname.c_str());

// Initializing NTP client
  timeClient.begin();
  delay (500);

  while ( unix_epoch == 0 ) {
    timeClient.update();                    // requesting time from NTP server
    unix_epoch = timeClient.getEpochTime(); // get UNIX epoch time from NTP server
  delay (500);
  }
  Serial.print ( "Clock initialized\r\n");

  tft.fillScreen(BLACK);                    // blanking display

  tft.drawRect(0, 73, tft.width(), 55, WHITE);  // draw rectangle (x, y, w, h, color)
  
  tft.setTextSize(1);                       // text size = 1
  tft.setTextColor(CYAN, BLACK);            // set text color to cyan and black background
  tft.setCursor(8, 78);                     // move cursor to position (8, 78) pixel
  tft.print( "Imsak" );
  tft.setCursor(43, 78);                    // move cursor to position (43, 78) pixel
  tft.print( "Subuh" );
  tft.setCursor(79, 78);                    // move cursor to position (79, 78) pixel
  tft.print( "Terbit" );
  tft.setCursor(120, 78);                   // move cursor to position (120, 78) pixel
  tft.print( "Dhuha" );

  tft.setCursor(6, 104);                    // move cursor to position (6, 104) pixel
  tft.print( "Dzuhur" );
  tft.setCursor(45, 104);                   // move cursor to position (45, 104) pixel
  tft.print( "Ashar" );
  tft.setCursor(79, 104);                   // move cursor to position (79, 104) pixel
  tft.print( "Maghrib" );
  tft.setCursor(124, 104);                  // move cursor to position (124, 104) pixel
  tft.print( "Isya'" );
}

// MAIN LOOP
void loop()
{
  timeClient.update();                      // requesting time from NTP server
  unix_epoch = timeClient.getEpochTime();   // get UNIX Epoch time from NTP server

  events();
  CST();                                    // requesting Clock and Salat Time
  delay(200);                               // wait 200ms
  CStat();
}

// CLOCK AND SALAT TIME FUNCTION
void CST()
{
  // creating day of week (dow) and it's absolute x position array
  char dow_matrix[7][10] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jum'at", "Sabtu"};
  byte x_pos[7] = {47, 53, 47, 59, 53, 47, 53};
  static byte previous_dow = 0;

  // print day of the week (dow)
  if( previous_dow != weekday(unix_epoch) )
  {
    previous_dow = weekday(unix_epoch);
    tft.fillRect(10, 0, 140, 16, BLACK);      // fill rectangle (x,y,w,h,color) (erase day from the display)
    tft.setTextSize(2);                       // text size = 2
    tft.setTextColor(CYAN, BLACK);            // set text color to cyan and black background
    tft.setCursor(x_pos[previous_dow-1], 0);  // set cursor to position (dow, 0) pixel
    tft.print( dow_matrix[previous_dow-1] );

    JS();                                     // requesting jadwal sholat

    tft.fillRect(5, 89, 150, 11, BLACK);      // fill rectangle (x,y,w,h,color)
    tft.setTextSize(1);                       // text size = 1
    tft.setTextColor(YELLOW, BLACK);             // set text color to red and black background
    tft.setCursor(8, 90);                     // move cursor to position (8, 90) pixel
    tft.print( b_imsak );
    tft.setCursor(43, 90);                    // move cursor to position (43, 90) pixel
    tft.print( b_subuh );
    tft.setCursor(83, 90);                    // move cursor to position (83, 90) pixel
    tft.print( b_terbit );
    tft.setCursor(120, 90);                   // move cursor to position (120, 90) pixel
    tft.print( b_dhuha );

    tft.fillRect(5, 115, 150, 11, BLACK);     // fill rectangle (x,y,w,h,color)
    tft.setCursor(8, 116);                    // move cursor to position (8, 116) pixel
    tft.print( b_dzuhur );
    tft.setCursor(44, 116);                   // move cursor to position (44, 116) pixel
    tft.print( b_ashar );
    tft.setCursor(84, 116);                   // move cursor to position (84, 116) pixel
    tft.print( b_maghrib );
    tft.setCursor(120, 116);                  // move cursor to position (120, 116) pixel
    tft.print( b_isya );
  }

  // print date
  tft.setTextSize(2);                         // text size = 2
  tft.setCursor(22, 21);                      // move cursor to position (22, 21) pixel
  tft.setTextColor(LIGHTGREY, BLACK);         // set text color to yellow and black background
  tft.printf( "%02u/%02u/%04u", day(unix_epoch), month(unix_epoch), year(unix_epoch) );

  // print time
  tft.setTextSize(3);                         // text size = 3
  tft.setCursor(20, 42);                      // move cursor to position (20, 42) pixel
  tft.setTextColor(LIGHTGREY, BLACK);         // set text color to lightgrey and black background
  tft.printf( "%02u:%02u", hour(unix_epoch), minute(unix_epoch) );
  // print seconds
  tft.setTextSize(2);                         // text size = 2
  tft.setCursor(108, 49);                     // move cursor to position (108, 49) pixel
  tft.printf( ":%02u", second(unix_epoch) );
}

// JADWAL SHOLAT FUNCTION
void JS()
{
  WiFiClientSecure client;
  client.setInsecure();                       // use with caution
  client.connect ( "api.myquran.com", 80 );

  // url request construct
  HTTPClient https;
  String url = "https://api.myquran.com/v1/sholat/jadwal/";
  url = url + id_kota + "/" + year(unix_epoch) + "/" + month(unix_epoch) + "/" + day(unix_epoch);

  Serial.println ( url );
  https.begin ( client, url );
  int httpCode = https.GET();
  String payload = https.getString();
  Serial.print ( payload );
  Serial.print ( "\r\n\r\n" );

  DynamicJsonDocument doc (1024);
  DeserializationError error = deserializeJson(doc, payload);
  JsonObject results = doc [ "data" ][ "jadwal" ];
  String imsak   = results[ "imsak" ];
  String subuh   = results[ "subuh" ];
  String terbit  = results[ "terbit" ];
  String dhuha   = results[ "dhuha" ];
  String dzuhur  = results[ "dzuhur" ];
  String ashar   = results[ "ashar" ];
  String maghrib = results[ "maghrib" ];
  String isya    = results[ "isya" ];

  // convert string to character
  imsak.toCharArray   (b_imsak,   10);
  subuh.toCharArray   (b_subuh,   10);
  terbit.toCharArray  (b_terbit,  10);
  dhuha.toCharArray   (b_dhuha,   10);
  dzuhur.toCharArray  (b_dzuhur,  10);
  ashar.toCharArray   (b_ashar,   10);
  maghrib.toCharArray (b_maghrib, 10);
  isya.toCharArray    (b_isya,    10);

  if ( error ) {
    Serial.print (F( "deserializeJson() failed: " ));
    Serial.println ( error.c_str() );
    return;
  }
}

// Clock Status Function - inspired by W8BH - Bruce Hall - https://github.com/bhall66/NTP-clock
void CStat()
{
  int color;
  if (second()%10) return;                      // update every 10 seconds 
  int syncAge = now()-lastNtpUpdateTime();      // how long since last sync?
  if (syncAge < SYNC_MARGINAL)                  // time is good & in sync
    color = GREEN;
  else if (syncAge < SYNC_LOST)                 // sync is 1-24 hours old
    color = YELLOW;
  else color = RED;                             // time is stale!
  tft.fillRoundRect(145, 3, 10, 10, 10, color); // show clock status as a color
}

// PROGRAM END
