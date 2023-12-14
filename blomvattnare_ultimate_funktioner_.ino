#include "U8glib.h" // inkludera bibloteket för oled displayen
#include <Wire.h> // inkludera biblotek för komunikation med i2c

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0 | U8G_I2C_OPT_NO_ACK | U8G_I2C_OPT_FAST); // Fast I2C / TWI

// definera knappar samt inputs och outputs för fuktighets sensorn
#define BUTTON_UP_PIN 12 
#define BUTTON_SELECT_PIN 8
#define BUTTON_DOWN_PIN 4
#define sensorPower 7
#define sensorPin A0

// variabler för menyn
int current_screen = 0;   // 0 = menu, 1 = screenshot, 2 = qr
int item_selected = 0;
int itme_sel_previous;
int item_sel_next;
const int NUM_ITEMS = 3; // hur många föremål menyn har

// only perform action when button is clicked, and wait until another press
int button_up_clicked = 0; 
int button_select_clicked = 0; // same as above
int button_down_clicked = 0; // same as above

int water_lvl = 80; // variabel som anger det initiala tröskelvärdet för att pumpen ska börja pumpa

const int relais_moteur = 2; // the relay is connected to pin 1 of the Adruino board

// Bitmap för 'ange icon', 16x16px
const unsigned char epd_bitmap_ange_icon [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x08, 0x88, 0x1c, 0x88, 0x1c, 0x88, 0x08, 0x88, 0x08, 0x9c, 0x08, 0x9c,
  0x08, 0x88, 0x08, 0x88, 0x09, 0xc8, 0x09, 0xc8, 0x08, 0x88, 0x08, 0x88, 0x00, 0x00, 0x00, 0x00
};
// Bitmap för 'eye icon', 16x16px
const unsigned char epd_bitmap_eye_icon [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xc0, 0x08, 0x30, 0x13, 0x88, 0x26, 0x44,
  0x4c, 0x62, 0x24, 0x44, 0x13, 0x88, 0x08, 0x30, 0x07, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// Bitmap för 'vatten icon', 16x16px
const unsigned char epd_bitmap_vatten_icon [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x02, 0x80, 0x02, 0x80, 0x04, 0x40, 0x08, 0x20,
  0x08, 0x20, 0x10, 0x10, 0x10, 0x10, 0x14, 0x10, 0x12, 0x10, 0x08, 0x20, 0x07, 0xc0, 0x00, 0x00
};

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 144)
const unsigned char* bitmap_icons[3] = {
  epd_bitmap_ange_icon,
  epd_bitmap_eye_icon,
  epd_bitmap_vatten_icon
};

// Bitmap för 'Scroll bar', 8x63px
const unsigned char epd_bitmap_Scroll_bar [] PROGMEM = {
  0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00,
  0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00,
  0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00,
  0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00
};
// Bitmap för 'select bar', 128x19px
const unsigned char epd_bitmap_select_bar [] PROGMEM = {
  0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0,
  0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80,
  0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00
};

char menu_item[NUM_ITEMS] [20] = {
  {"Set Level"},
  {"Overview"},
  {"Water Level"}
};

void setup() {y
  // så som jag har kopplat mina knappar är dom odefinerade om ingen ström tillförs. Jag kan motverka detta men den inbyggda funktionen 'INPUT_PULLUP'
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);

  // Anger text storlek och ljusstyrka
  u8g.setFont(u8g_font_tpssb);
  u8g.setColorIndex(1);

  // Fuktighetssensor delen
  pinMode(sensorPower, OUTPUT);
  Wire.begin();
  Serial.begin(9600);
  
  // Initially keep the sensor OFF
  digitalWrite(sensorPower, LOW);

  // pump delen
  pinMode(relais_moteur, OUTPUT);
}

void loop() {
  // Call the menu function to display the user interface
  oledMenu();

  // If the select button is pressed, trigger the pump function 
  if (!(digitalRead(BUTTON_SELECT_PIN) == LOW)) {
    Serial.println("vi är inne i if satsen");
    pump();
  }
}

// Function to update OLED display with text and values
void updateOled(String text, String number) {
  u8g.firstPage();
  do {
    u8g.drawStr(0, 11, text.c_str()); // Rita text på OLED-skärmen
    u8g.drawStr(0, 31, number.c_str());
  } while (u8g.nextPage());
}

// Function to set humidity level
int setHumidity() {
  if (current_screen == 1 && item_selected == 0) { // om vi är inne i rätt meny gör detta
    if ((digitalRead(BUTTON_DOWN_PIN) == LOW) && (button_down_clicked == 0)) { // om ner-knappen är tryckt kommer tröskelvärdet sänkas
      water_lvl -= 1;
    }
    else if ((digitalRead(BUTTON_UP_PIN) == LOW) && (button_up_clicked == 0)) { // om upp-knappen är tryckt kommer tröskelvärdet öka
      water_lvl += 1;
    }
  }
  return water_lvl;
}

int readSensor() {
  digitalWrite(sensorPower, HIGH);  // Turn the sensor ON
  delay(10);              // Allow power to settle
  int val = analogRead(sensorPin);  // Read the analog value form sensor
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
  if (current_screen == 0) { // MENU SCREEN

    // up and down buttons only work for the menu screen
    if ((digitalRead(BUTTON_UP_PIN) == LOW) && (button_up_clicked == 0)) { // up button clicked - jump to previous menu item
      item_selected = item_selected - 1; // select previous item
      button_up_clicked = 1; // set button to clicked to only perform the action once
      if (item_selected < 0) { // if first item was selected, jump to last item
        item_selected = NUM_ITEMS - 1;
      }
    }
    else if ((digitalRead(BUTTON_DOWN_PIN) == LOW) && (button_down_clicked == 0)) { // down button clicked - jump to next menu item
      item_selected = item_selected + 1; // select next item
      button_down_clicked = 1; // set button to clicked to only perform the action once
      if (item_selected >= NUM_ITEMS) { // last item was selected, jump to first menu item
        item_selected = 0;
      }
    }

    if ((digitalRead(BUTTON_UP_PIN) == HIGH) && (button_up_clicked == 1)) { // unclick
      button_up_clicked = 0;
    }
    if ((digitalRead(BUTTON_DOWN_PIN) == HIGH) && (button_down_clicked == 1)) { // unclick
      button_down_clicked = 0;
    }
  }

  if ((digitalRead(BUTTON_SELECT_PIN) == LOW) && (button_select_clicked == 0)) { // select button clicked, jump between screens
    button_select_clicked = 1; // set button to clicked to only perform the action once
    if (current_screen == 0) {
      current_screen = 1; // menu items screen --> selected menu
    }
    else if (current_screen == 1) {
      current_screen = 0; // selected menu --> menu items screen
    }
  }

  if ((digitalRead(BUTTON_SELECT_PIN) == HIGH) && (button_select_clicked == 1)) { // unclick
    button_select_clicked = 0;
  }

  // set correct values for the previous and next items
  itme_sel_previous = item_selected - 1;
  if (itme_sel_previous < 0) {
    itme_sel_previous = NUM_ITEMS - 1; // previous item would be below first = make it the last
  }
  item_sel_next = item_selected + 1;
  if (item_sel_next >= NUM_ITEMS) {
    item_sel_next = 0; // next item would be after last = make it the first
  }

  u8g.firstPage();
  do {

    if (current_screen == 0) { // MENU SCREEN
      // selected item background
      u8g.drawBitmapP(0, 22, 128 / 8, 21, epd_bitmap_select_bar);

      // draw previous item as icon + label
      u8g.setFont(u8g_font_7x14);
      u8g.drawStr(25, 15, menu_item[itme_sel_previous]);
      u8g.drawBitmapP( 4, 2, 16 / 8, 16, bitmap_icons[itme_sel_previous]);

      // draw selected item as icon + label in bold font
      u8g.setFont(u8g_font_7x14B);
      u8g.drawStr(25, 15 + 20 + 2, menu_item[item_selected]);
      u8g.drawBitmapP( 4, 24, 16 / 8, 16, bitmap_icons[item_selected]);

      // draw next item as icon + label
      u8g.setFont(u8g_font_7x14);
      u8g.drawStr(25, 15 + 20 + 20 + 2 + 2, menu_item[item_sel_next]);
      u8g.drawBitmapP( 4, 46, 16 / 8, 16, bitmap_icons[item_sel_next]);

      // draw scrollbar background
      u8g.drawBitmapP(128 - 8, 0, 8 / 8, 64, epd_bitmap_Scroll_bar);

      // draw scrollbar handle
      u8g.drawBox(125, 64 / NUM_ITEMS * item_selected, 3, 64 / NUM_ITEMS);

    } //set level delen
    else if (current_screen == 1 && item_selected == 0) {
      updateOled("State Level:", String(setHumidity()) + "%");
    } //water level delen
    else if (current_screen == 1 && item_selected == 2) {
      updateOled("Humidity Level:", String(water_lvl) + "%");
    } //overview delen
    else if (current_screen == 1 && item_selected == 1) {
      if (waterOrNot() == true) {
        updateOled("Plant Humidity:" + String(readSensor()) + "%", "Pland too dry, watering...");
      }
      else {
        updateOled("Plant Humidity:" + String(readSensor()) + "%", "Humidity below stated level");
      }
    }
  } while ( u8g.nextPage() );
}

// Function to control the water pump
void pump(){ 
    if (readSensor() >= water_lvl) {
    digitalWrite(relais_moteur, HIGH); // The motor starts to run
    while (readSensor() > 40) {
      //delay can be added here
    }
    digitalWrite(relais_moteur, LOW); // The motor stops running
  }
}
