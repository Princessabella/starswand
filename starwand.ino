#include <Wire.h>
#include <Adafruit_NeoPixel.h>

//isabella bane
//csc235 finite state machine DA
//9/21/2023

//Neopixel
Adafruit_NeoPixel strip(22, 10);
uint32_t pink = strip.Color(255, 100, 100);
uint32_t green = strip.Color(0, 255, 0);
uint32_t off = strip.Color(0, 0, 0);
uint32_t purple = strip.Color(255, 0, 255);
uint32_t blue = strip.Color(100, 100, 255);
uint32_t white = strip.Color(255, 255, 255);
//state var
int state = 0;
//butt pins
int buttPin1 = 8;
boolean currentState1 = false;
boolean previousState1 = false;

//included from Michael Schoeffler
const int MPU_ADDR = 0x68;  // I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.

int16_t accelerometer_x, accelerometer_y, accelerometer_z;  // variables for accelerometer raw data
int16_t gyro_x, gyro_y, gyro_z;                             // variables for gyro raw data
int16_t temperature;                                        // variables for temperature data

char tmp_str[7];  // temporary variable used in convert function

char* convert_int16_to_str(int16_t i) {  // converts int16 to string. Moreover, resulting strings will have the same length in the debug monitor.
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}
/////////////////////

void setup() {
  Serial.begin(9600);
  strip.clear();
  strip.begin();
  strip.show();
  pinMode(buttPin1, INPUT);
  ///MS
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);  // Begins a transmission to the I2C slave (GY-521 board)
  Wire.write(0x6B);                  // PWR_MGMT_1 register
  Wire.write(0);                     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  /////
}

void loop() {
  //last frame
  previousState1 = currentState1;
  //get the current butt state :)
  currentState1 = debounce(buttPin1, previousState1);
  if (currentState1 == 1 && previousState1 == 0) {
    state++;
    if (state >= 5) {
      state = 0;
    }
  }

  switch (state) {
    case 0:
      strip.clear();
      strip.show();
      threeColorSweep(off, off, purple, 100);
      strip.show();
      break;
    case 1:
      strip.clear();
      strip.show();
      brightShift(green, 5000);
      strip.show();
      break;
    case 2:
      strip.clear();
      strip.show();
      pongLight(purple, 10);
      strip.show();
      break;
    case 3:
      strip.clear();
      strip.show();
      bulletTrain(pink, 30);
      strip.show();
      break;
    case 4:
      strip.clear();
      strip.show();
      threeColorSweep(purple, white, white, 100);
      strip.show();
  }
  //MS
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);                         // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission(false);              // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
  Wire.requestFrom(MPU_ADDR, 7 * 2, true);  // request a total of 7*2=14 registers

  // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
  int prev_x = accelerometer_x;
  accelerometer_x = Wire.read() << 8 | Wire.read();  // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
  accelerometer_y = Wire.read() << 8 | Wire.read();  // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
  accelerometer_z = Wire.read() << 8 | Wire.read();  // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
  temperature = Wire.read() << 8 | Wire.read();      // reading registers: 0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L)
  int prev_gx = gyro_x;
  gyro_x = Wire.read() << 8 | Wire.read();  // reading registers: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
  gyro_y = Wire.read() << 8 | Wire.read();  // reading registers: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
  gyro_z = Wire.read() << 8 | Wire.read();  // reading registers: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)

  // // print out data
  // Serial.print("aX = ");
  // Serial.print(convert_int16_to_str(accelerometer_x));
  // Serial.print(" | aY = ");
  // Serial.print(convert_int16_to_str(accelerometer_y));
  // Serial.print(" | aZ = ");
  // Serial.print(convert_int16_to_str(accelerometer_z));
  // // the following equation was taken from the documentation [MPU-6000/MPU-6050 Register Map and Description, p.30]
  // Serial.print(" | tmp = ");
  // Serial.print(temperature / 340.00 + 36.53);
  // Serial.print(" | gX = ");
  // Serial.print(convert_int16_to_str(gyro_x));
  // Serial.print(" | gY = ");
  // Serial.print(convert_int16_to_str(gyro_y));
  // Serial.print(" | gZ = ");
  // Serial.print(convert_int16_to_str(gyro_z));
  // Serial.println();

  ////////////////

  int threshhold = 10000;


  if (gyro_x - prev_gx >= threshhold) {
    state++;
strip.clear();
strip.show();
    delay(2000);
    if (state == 5) {
      state = 0;
    }
    Serial.print("state: ");
    Serial.println(state);
  }
}


boolean debounce(int aButton, boolean prevState) {
  // track the button's current state
  boolean aButtonState = digitalRead(aButton);

  // if you pressed the button this frame, then
  // wait so you don't read the button again
  // while it is bouncing
  if (aButtonState == HIGH && prevState == LOW) {
    delay(15);
  }
  // return the button's current state
  return aButtonState;
}

void bulletTrain(uint32_t aColor, int wait) {
  static unsigned long startTime = millis();
  unsigned long currentTime = millis();
  static int index = 0;
  if (currentTime - startTime >= wait) {
    strip.setPixelColor(index, aColor);
    strip.setPixelColor(index - 1, 0, 0, 0);
    index++;
    strip.show();

    //reset timer
    startTime = millis();
    //reset moving color when reaches end
    if (index >= strip.numPixels()) {
      index = 0;
      strip.clear();
    }
  }
}


void pongLight(uint32_t aColor, int wait) {
  static boolean backforth = 0;
  static unsigned long startTime = millis();
  unsigned long currentTime = millis();
  static int index = 0;
  if (currentTime - startTime >= wait) {
    if (backforth == 0) {
      strip.clear();
      strip.setPixelColor(index, aColor);
      // strip.show();
      strip.setPixelColor(index - 1, 0, 0, 0);
      index++;
    } else {
      strip.setPixelColor(index, aColor);
      strip.setPixelColor(index + 1, 0, 0, 0);
      index--;
    }
    strip.show();
    if (index > strip.numPixels()) {
      backforth = 1;
    }
    if (index < 0) {
      backforth = 0;
      // strip.clear();
    }
    // Serial.println(index);
    startTime = millis();
  }
}

void brightShift(uint32_t aColor, int wait) {
  static boolean backforth = 0;
  static unsigned long startTime = millis();
  unsigned long currentTime = millis();
  static int i = 0;
  static int brightness = 0;
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, aColor);
    strip.setBrightness(brightness);
  }
  if (backforth == 0) {
    brightness++;
  } else {
    brightness--;
  }
  if (brightness <= 0) {
    backforth = 0;
    startTime = millis();
  }
  if (brightness >= 255) {
    backforth = 1;
    startTime = millis();
  }
  strip.show();
}
void threeColorSweep(uint32_t c1, uint32_t c2, uint32_t c3, int interval) {
  static unsigned long startTime = millis();
  unsigned long currentTime = millis();
  static int state = 0;
  if (currentTime - startTime >= interval) {
    state++;
    startTime = millis();
  }
  if (state == 0) {
    for (int i = 0; i < strip.numPixels(); i++) {
      if (i % 3 == 0) {
        strip.setPixelColor(i, c1);
      } else if (i % 3 == 1) {
        strip.setPixelColor(i, c2);
      } else if (i % 3 == 2) {
        strip.setPixelColor(i, c3);
      }
    }
    strip.show();
  }
  if (state == 1) {
    for (int i = 0; i < strip.numPixels(); i++) {
      if (i % 3 == 0) {
        strip.setPixelColor(i, c2);
      } else if (i % 3 == 1) {
        strip.setPixelColor(i, c3);
      } else if (i % 3 == 2) {
        strip.setPixelColor(i, c1);
      }
    }
    strip.show();
  }
  if (state == 2) {
    for (int i = 0; i < strip.numPixels(); i++) {
      if (i % 3 == 0) {
        strip.setPixelColor(i, c3);
      } else if (i % 3 == 1) {
        strip.setPixelColor(i, c1);
      } else if (i % 3 == 2) {
        strip.setPixelColor(i, c2);
      }
    }
    strip.show();
    state = 0;
  }
}