#include "U8glib.h"
#include <Wire.h>

// Initialize the U8glib library for the OLED display
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0 | U8G_I2C_OPT_NO_ACK | U8G_I2C_OPT_FAST); // Fast I2C / TWI

// Define button pins and other constants
#define BUTTON_UP_PIN 12
#define BUTTON_SELECT_PIN 8
#define BUTTON_DOWN_PIN 4
#define sensorPower 7
#define sensorPin A0

int current_screen = 0;   // 0 = menu, 1 = screenshot, 2 = qr
int button_up_clicked = 0; // only perform action when button is clicked, and wait until another press
int button_select_clicked = 0; // same as above
int button_down_clicked = 0; // same as above

int item_selected = 0;
int itme_sel_previous;
int item_sel_next;

int water_lvl = 80;

const int NUM_ITEMS = 3;

const int relais_moteur = 2; // the relay is connected to pin 1 of the Arduino board

// Define bitmap images for icons
const unsigned char epd_bitmap_ange_icon [] PROGMEM = {...};  // 'ange icon'
const unsigned char epd_bitmap_eye_icon [] PROGMEM = {...};   // 'eye icon'
const unsigned char epd_bitmap_vatten_icon [] PROGMEM = {...}; // 'vatten icon'

// Array of all bitmaps for convenience
const unsigned char* bitmap_icons[3] = {
  epd_bitmap_ange_icon,
  epd_bitmap_eye_icon,
  epd_bitmap_vatten_icon
};

// 'Scroll bar' and 'select bar' bitmaps
const unsigned char epd_bitmap_Scroll_bar [] PROGMEM = {...};   // 'Scroll bar'
const unsigned char epd_bitmap_select_bar [] PROGMEM = {...};   // 'select bar'

// Menu item labels
char menu_item[NUM_ITEMS] [20] = {
  {"Set Level"},
  {"Overview"},
  {"Water Level"}
};

// Setup function
void setup() {
  // Set button pins as INPUT_PULLUP
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);

  // Initialize OLED display settings
  u8g.setFont(u8g_font_tpssb);
  u8g.setColorIndex(1);

  // Moisture sensor setup
  pinMode(sensorPower, OUTPUT);
  Wire.begin();
  Serial.begin(9600);
  digitalWrite(sensorPower, LOW);

  // Water pump setup
  pinMode(relais_moteur, OUTPUT);
}

// Main loop
void loop() {
  // Call the menu function to display the user interface
  oledMenu();

  // If the select button is pressed, trigger the pump function
  if (!(digitalRead(BUTTON_SELECT_PIN) == LOW)) {
    Serial.println("we are inside the if statement");
    pump();
  }
}

// Function to update OLED display with text and values
void updateOled(String text, String number) {
  u8g.firstPage();
  do {
    u8g.drawStr(0, 11, text.c_str()); // Draw text on the OLED screen
    u8g.drawStr(0, 31, number.c_str());
  } while (u8g.nextPage());
}

// Function to set humidity level
int setHumidity() {
  if (current_screen == 1 && item_selected == 0) {
    if ((digitalRead(BUTTON_DOWN_PIN) == LOW) && (button_down_clicked == 0)) {
      water_lvl -= 1;
    }
    else if ((digitalRead(BUTTON_UP_PIN) == LOW) && (button_up_clicked == 0)) {
      water_lvl += 1;
    }
  }
  return water_lvl;
}

// Function to read moisture sensor value
int readSensor() {
  digitalWrite(sensorPower, HIGH);  // Turn the sensor ON
  delay(10);              // Allow power to settle
  int val = analogRead(sensorPin);  // Read the analog value from the sensor
  digitalWrite(sensorPower, LOW);   // Turn the sensor OFF
  val = val / 10;
  return val;             // Return analog moisture value
}

// Function to determine if water is needed based on sensor reading and set level
int waterOrNot() {
  int result;
  if (readSensor() >= water_lvl) {
    result = true;
  }
  else {
    result = false;
  }
  return result;
}

// Function to handle OLED menu display
void oledMenu() {
  // ... (omitted for brevity, see the full code for the details)
}

// Function to control the water pump
void pump(){ 
  if (readSensor() >= water_lvl) {
    digitalWrite(relais_moteur, HIGH); // The motor starts to run
    while (readSensor() > 40) {
      // Delay or other actions during pumping
    }
    digitalWrite(relais_moteur, LOW); // The motor stops running
  }
}
