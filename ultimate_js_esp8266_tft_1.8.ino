// NodeMCU ESP8266 - NTP Jadwal Sholat 1.8" TFT SPI

#include <Adafruit_ST7735.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ezTime.h>
#include <NTPClient.h>
#include <WiFiClientSecure.h>
#include <WiFiManager.h>
#include <WiFiUdp.h>

// Adjustable parameters
const char   * t_zone = "Asia/Jakarta";     // See https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
const char * ntp_pool = "id.pool.ntp.org";  // NTP Server pool address
const int     id_kota = 1301;               // See https://api.myquran.com/v1/sholat/kota/semua
const int  duty_cycle = 72;                 // TFT brightness using PWM duty cycle (0-255)
const int display_ori = 1;                  // TFT display orientation
String   new_hostname = "JamSholat";

// Buffers for JSON payload, string to character conversion
char b_imsak[10], b_subuh[10], b_terbit[10], b_dhuha[10], b_dzuhur[10], b_ashar[10], b_maghrib[10], b_isya[10];

// day of the week (dow) and it's corresponding absolute x position array
char dow_matrix[7][10] = {"Ahad", "Senin", "Selasa", "Rabu", "Kamis", "Jum'at", "Sabtu"};
byte dow_x_pos[7] = {65, 59, 53, 65, 59, 53, 59};
static byte previous_dow = 0;

Timezone my_tz;                              // local timezone variable
time_t loc_time;                             // current & displayed local time

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

#define SYNC_MARGINAL 3600                          // yellow status if no sync for 1 hour
#define SYNC_LOST     86400                         // red status if no sync for 1 day

// STATIC DISPLAY FUNCTION
void SDF()
{
  tft.fillScreen(BLACK);                            // blanking display
  // draw rectangle frames on display
  tft.drawRect(0, 0, 16, 70, WHITE);                // draw rectangle (x, y, w, h, color)
  tft.drawRect(0, 73, 160, 55, WHITE);              // draw rectangle (x, y, w, h, color)
  
  tft.setTextSize(1);                               // text size = 1
  tft.setTextColor(CYAN, BLACK);                    // set text color to cyan and black background
  tft.setCursor(8, 78);                             // move cursor to position (8, 78) pixel
  tft.print( "Imsak" );
  tft.setCursor(43, 78);                            // move cursor to position (43, 78) pixel
  tft.print( "Subuh" );
  tft.setCursor(79, 78);                            // move cursor to position (79, 78) pixel
  tft.print( "Terbit" );
  tft.setCursor(120, 78);                           // move cursor to position (120, 78) pixel
  tft.print( "Dhuha" );

  tft.setCursor(6, 104);                            // move cursor to position (6, 104) pixel
  tft.print( "Dzuhur" );
  tft.setCursor(45, 104);                           // move cursor to position (45, 104) pixel
  tft.print( "Ashar" );
  tft.setCursor(79, 104);                           // move cursor to position (79, 104) pixel
  tft.print( "Maghrib" );
  tft.setCursor(124, 104);                          // move cursor to position (124, 104) pixel
  tft.print( "Isya'" );
}

// CLOCK AND SALAT TIME FUNCTION
void CSTF()
{
  loc_time = my_tz.now();
// print date
  tft.setTextSize(2);                               // text size = 2
  tft.setCursor(28, 21);                            // move cursor to position (22, 21) pixel
  tft.setTextColor(LIGHTGREY, BLACK);               // set text color to yellow and black background
  tft.printf( "%02u/%02u/%04u", day(loc_time), month(loc_time), year(loc_time) );

// print time
  tft.setTextSize(3);                               // text size = 3
  tft.setCursor(28, 42);                            // move cursor to position (20, 42) pixel
  tft.setTextColor(LIGHTGREY, BLACK);               // set text color to lightgrey and black background
  tft.printf( "%02u:%02u", hour(loc_time), minute(loc_time) );

// print seconds
  tft.setTextSize(2);                               // text size = 2
  tft.setCursor(114, 49);                           // move cursor to position (108, 49) pixel
  tft.printf( ":%02u", second(loc_time) );

// print day of the week (dow)
  if( previous_dow != weekday(loc_time) )
  {
    previous_dow = weekday(loc_time);
    tft.fillRect(17, 0, 140, 16, BLACK);            // fill rectangle (x,y,w,h,color) (erase day from the display)
    tft.setTextSize(2);                             // text size = 2
    tft.setTextColor(CYAN, BLACK);                  // set text color to cyan and black background
    tft.setCursor( dow_x_pos[previous_dow-1], 0 );  // set cursor to position (dow_x_pos, 0) pixel
    tft.print( dow_matrix[previous_dow-1] );

    JSF();                                          // requesting Jadwal Sholat once every day at 00:00:00

// print jadwal sholat upper row
    tft.fillRect(5, 89, 150, 11, BLACK);            // fill rectangle (x,y,w,h,color)
    tft.setTextSize(1);                             // text size = 1
    tft.setTextColor(YELLOW, BLACK);                // set text color to yellow and black background
    tft.setCursor(8, 90);                           // move cursor to position (8, 90) pixel
    tft.print( b_imsak );
    tft.setCursor(43, 90);                          // move cursor to position (43, 90) pixel
    tft.print( b_subuh );
    tft.setCursor(83, 90);                          // move cursor to position (83, 90) pixel
    tft.print( b_terbit );
    tft.setCursor(120, 90);                         // move cursor to position (120, 90) pixel
    tft.print( b_dhuha );

// print jadwal sholat lower row
    tft.fillRect(5, 115, 150, 11, BLACK);           // fill rectangle (x,y,w,h,color)
    tft.setCursor(8, 116);                          // move cursor to position (8, 116) pixel
    tft.print( b_dzuhur );
    tft.setCursor(44, 116);                         // move cursor to position (44, 116) pixel
    tft.print( b_ashar );
    tft.setCursor(84, 116);                         // move cursor to position (84, 116) pixel
    tft.print( b_maghrib );
    tft.setCursor(120, 116);                        // move cursor to position (120, 116) pixel
    tft.print( b_isya );
  }
}

// JADWAL SHOLAT FUNCTION
void JSF()
{
  WiFiClientSecure client;
  client.setInsecure();                             // use with caution
  client.connect ( "api.myquran.com", 80 );

// url request construct
  HTTPClient https;
  String url = "https://api.myquran.com/v1/sholat/jadwal/";
  url = url + id_kota + "/" + year(loc_time) + "/" + month(loc_time) + "/" + day(loc_time);

  // requesting the table
  Serial.println ( url );
  https.begin ( client, url );
  int httpCode = https.GET();
  String payload = https.getString();
  Serial.print ( payload );
  Serial.print ( "\r\n\r\n" );

// deserialize JSON payload
  DynamicJsonDocument doc (1024);
  DeserializationError error = deserializeJson(doc, payload);
  JsonObject results = doc [ "data" ][ "jadwal" ];
  String imsak    = results[ "imsak" ];
  String subuh    = results[ "subuh" ];
  String terbit   = results[ "terbit" ];
  String dhuha    = results[ "dhuha" ];
  String dzuhur   = results[ "dzuhur" ];
  String ashar    = results[ "ashar" ];
  String maghrib  = results[ "maghrib" ];
  String isya     = results[ "isya" ];

// convert string to character
  imsak.toCharArray   (b_imsak,   10);
  subuh.toCharArray   (b_subuh,   10);
  terbit.toCharArray  (b_terbit,  10);
  dhuha.toCharArray   (b_dhuha,   10);
  dzuhur.toCharArray  (b_dzuhur,  10);
  ashar.toCharArray   (b_ashar,   10);
  maghrib.toCharArray (b_maghrib, 10);
  isya.toCharArray    (b_isya,    10);

// on JSON deserialization error
  if ( error ) {
    Serial.print (F( "deserializeJson() failed: " ));
    Serial.println ( error.c_str() );
    return;
  }
}

// NTP Clock Status Function - inspired by W8BH - Bruce E. Hall - https://github.com/bhall66/NTP-clock
void NCSF()
{
  int sync_result, sync_age;

  if ( second()%30 ) return;                        // update every 30 seconds
  sync_age = now() - lastNtpUpdateTime();           // how long since last sync?
  if ( sync_age < SYNC_MARGINAL )                   // time is good & in sync
    sync_result = GREEN;
  else 
  if ( sync_age < SYNC_LOST )                       // sync is 1-24 hours old
    sync_result = YELLOW;
  else sync_result = RED;                           // time is stale!

  // display sync result
  tft.setTextSize(1);                               // text size = 1
  tft.setTextColor(sync_result, BLACK);             // set text color to 'sync_result' and black background
  tft.setCursor(6, 15);                             // move cursor to position (6, 15) pixel
  tft.print( "N" );
  tft.setCursor(6, 25);                             // move cursor to position (6, 25) pixel
  tft.print( "T" );
  tft.setCursor(6, 35);                             // move cursor to position (6, 35) pixel
  tft.print( "P" );
  tft.fillRoundRect( 5, 50, 7, 7, 0, sync_result ); // show clock status color as 'sync_result'
}

// SETUP
void setup()
{
// initializing 1.8" TFT display
  analogWrite(led_pin, duty_cycle);                 // set display brightness
  tft.initR(INITR_BLACKTAB);                        // initialize TFT display with ST7735 chip
  tft.setRotation(display_ori);                               // set display orientation
  tft.fillScreen(BLACK);                            // blanking display

// initializing Serial Port
  Serial.begin(115200);                             // set serial port speed
  delay (3000);                                     // 3 seconds delay

  // WiFiManager, Local initialization. 
  // Once its business is done, there is no need to keep it around
  WiFiManager wfm;

    // Supress Debug information
    wfm.setDebugOutput(true);

    // reset settings - wipe stored credentials for testing
    // these are stored by the esp library
//    wfm.resetSettings();

    Serial.println("WiFi connecting");
    tft.setCursor(38, 20);                          // move cursor to position (38, 20) pixel
    tft.print("WiFi connecting");

    if (!wfm.autoConnect( "JamSholat" )) {
      // Did not connect, print error message
      Serial.println("failed to connect and hit timeout");
      tft.setCursor(40, 10);                        // move cursor to position (40, 10) pixel
      tft.print("failed to connect");
   
      // Reset and try again
      ESP.restart();
      delay(1000);
    }
 
    // Connected!
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    tft.setCursor(40, 40);                          // move cursor to position (40, 40) pixel
    tft.print("WiFi connected");
    tft.setCursor(40, 60);                          // move cursor to position (40, 60) pixel
    tft.print(WiFi.localIP());
    delay(3000);

  // set hostname
  WiFi.setHostname(new_hostname.c_str());

  // priming eztime library
  setServer(ntp_pool);                              // set NTP server
  while (timeStatus()!=timeSet)                     // wait until time synced
  {
    events();                                       // allow ezTime to sync
    delay(1000);
  }
  my_tz.setLocation(t_zone);
  loc_time = my_tz.now();

  JSF();                                            // initialize Jam Sholat Function
  delay(500);
  SDF();                                            // requesting Static Display Function
}

// MAIN LOOP
void loop()
{
  CSTF();                                          // requesting Clock and Salat Time
  NCSF();                                          // requesting NTP Clock Status
  events();                                        // update ntp
}

// PROGRAM END
