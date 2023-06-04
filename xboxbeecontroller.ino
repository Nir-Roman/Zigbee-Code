/*
 Example sketch for the Xbox Wireless Reciver library - developed by Kristian Lauszus
 It supports up to four controllers wirelessly
 For more information see the blog post: http://blog.tkjelectronics.dk/2012/12/xbox-360-receiver-added-to-the-usb-host-library/ or
 send me an e-mail:  kristianl@tkjelectronics.com
 */
//RouterA 0x0013A20041C5542F
//CoordinatorA 0x0013A20041C53ECD


//R2_Base 0x0013A20041C53E6A
//R2_Remote 0x0013A200419B4D6F
#include <XBee.h>
#define XBEE_DEST_UPPER_ADDRESS 0x0013A200
#define XBEE_DEST_LOWER_ADDRESS 0x419B4D6F
//#define _WIRELESS_JOYSTICK_

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif

#ifdef _WIRELESS_JOYSTICK_
#include <XBOXRECV.h>
#else
#include <XBOXUSB.h>
#endif

// #define DEBUG_MODE
#include <SPI.h>

USB Usb;
#ifndef _WIRELESS_JOYSTICK_
XBOXUSB Xbox(&Usb);
#else
XBOXRECV Xbox(&Usb);
#endif

XBee xbee = XBee();
uint8_t payload[8] = { 0 };

XBeeAddress64 addr64 = XBeeAddress64(XBEE_DEST_UPPER_ADDRESS, XBEE_DEST_LOWER_ADDRESS);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();


int statusLed = 13;
int errorLed = 13;
unsigned long gCurr_time = 0;


// Button's bit position in the byte 0
#define B_X (7)
#define B_Y (6)
#define B_A (5)
#define B_B (4)
#define B_RB (3)
#define B_LB (2)
#define B_R3 (1)
#define B_L3 (0)

// byte 1
#define B_START (7)
#define B_BACK (6)
#define B_XBOX (5)
#define B_UP (3)
#define B_DOWN (2)
#define B_LEFT (1)
#define B_RIGHT (0)


// Bytes Positions in the packet
#define P_BUTTON1 (0)
#define P_BUTTON2 (1)
#define P_RHXL (2)
#define P_RHYL (3)
#define P_LHXL (4)
#define P_LHYL (5)
#define P_LT (6)
#define P_RT (7)

#define _BV(x) (1 << x)

void sendpacket();
void flashLed(int pin, int times, int wait) {

  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(wait);
    digitalWrite(pin, LOW);

    if (i + 1 < times) {
      delay(wait);
    }
  }
}

void getButtonStatus();

void setup() {
  Serial.begin(115200);
#if !defined(__MIPSEL__)
  while (!Serial)
    ;  // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1)
      ;  //halt
  }
  Serial.print(F("\r\nXbox Wireless Receiver Library Started"));
  pinMode(statusLed, OUTPUT);
  pinMode(errorLed, OUTPUT);
  Serial3.begin(9600);
  xbee.setSerial(Serial3);
  gCurr_time = millis();
}
uint8_t button_bytes[2] = { 0 };
uint8_t r_hatx = 0;
uint8_t r_haty = 0;
uint8_t l_hatx = 0;
uint8_t l_haty = 0;
uint8_t left_trigger = 0;
uint8_t right_trigger = 0;

void loop() {
  Usb.Task();
  button_bytes[0] = 0;
  button_bytes[1] = 0;
  r_hatx = 0;
  r_haty = 0;
  l_hatx = 0;
  l_haty = 0;
  left_trigger = 0;
  right_trigger = 0;

#ifdef _WIRELESS_JOYSTICK_

  if (Xbox.XboxReceiverConnected) {
    for (uint8_t i = 0; i < 4; i++) {
      if (Xbox.Xbox360Connected[i]) {

        if (Xbox.getButtonPress(LT, i) || Xbox.getButtonPress(RT, i)) {
          Serial.print("here");
          left_trigger = Xbox.getButtonPress(LT, i);
          right_trigger = Xbox.getButtonPress(RT, i);
#ifdef DEBUG_MODE
          Serial.print("LT: ");
          Serial.print(left_trigger);
          Serial.print("\tRT: ");
          Serial.println(right_trigger);
          Xbox.setRumbleOn(Xbox.getButtonPress(LT, i), Xbox.getButtonPress(RT, i), i);
#endif  // DEBUG_MODE
        }

        if (Xbox.getButtonPress(LB, i)) {
          button_bytes[0] |= _BV(B_LB);
#ifdef DEBUG_MODE
          Serial.print("LB");
          Serial.println();

#endif  // DEBUG_MODE
        }

        if (Xbox.getButtonPress(RB, i)) {
          button_bytes[0] |= _BV(B_RB);
#ifdef DEBUG_MODE
          Serial.print("RB");
          Serial.println();
#endif  // DEBUG_MODE
        }

        if (Xbox.getButtonPress(X, i)) {
          button_bytes[0] |= _BV(B_X);
#ifdef DEBUG_MODE
          Serial.print("X");
          Serial.println();
#endif  // DEBUG_MODE
        }

        if (Xbox.getButtonPress(Y, i)) {
          button_bytes[0] |= _BV(B_Y);
#ifdef DEBUG_MODE
          Serial.print("Y");
          Serial.println();
#endif  // DEBUG_MODE
        }

        if (Xbox.getButtonPress(A, i)) {
          button_bytes[0] |= _BV(B_A);
#ifdef DEBUG_MODE
          Serial.print("A");
          Serial.println();
#endif  // DEBUG_MODE
        }

        if (Xbox.getButtonPress(B, i)) {
          button_bytes[0] |= _BV(B_B);
#ifdef DEBUG_MODE
          Serial.print("B");
          Serial.println();
#endif  // DEBUG_MODE
        }

        if (Xbox.getButtonPress(UP, i)) {
          button_bytes[1] |= _BV(B_UP);
#ifdef DEBUG_MODE
          Serial.print("UP");
          Serial.println();
#endif  // DEBUG_MODE
        }

        if (Xbox.getButtonPress(DOWN, i)) {
          button_bytes[1] |= _BV(B_DOWN);
#ifdef DEBUG_MODE
          Serial.print("DOWN");
          Serial.println();
#endif  // DEBUG_MODE
        }

        if (Xbox.getButtonPress(LEFT, i)) {
          button_bytes[1] |= _BV(B_LEFT);
#ifdef DEBUG_MODE
          Serial.print("LEFT");
          Serial.println();
#endif  // DEBUG_MODE
        }

        if (Xbox.getButtonPress(RIGHT, i)) {
          button_bytes[1] |= _BV(B_RIGHT);
#ifdef DEBUG_MODE
          Serial.print("RIGHT");
          Serial.println();
#endif  // DEBUG_MODE
        }
        if (Xbox.getButtonPress(L3, i)) {
          button_bytes[0] |= _BV(B_L3);
#ifdef DEBUG_MODE
          Serial.print("L3");
          Serial.println();
#endif
        }

        if (Xbox.getButtonPress(R3, i)) {
          button_bytes[0] |= _BV(B_R3);
#ifdef DEBUG_MODE
          Serial.print("R3");
          Serial.println();
#endif
        }

        if (Xbox.getButtonPress(START, i)) {
          button_bytes[1] |= _BV(B_START);
#ifdef DEBUG_MODE
          Serial.print("START");
          Serial.println();
#endif  // DEBUG_MODE
        }

        if (Xbox.getButtonPress(BACK, i)) {
          button_bytes[1] |= _BV(B_BACK);
#ifdef DEBUG_MODE
          Serial.print("BACK");
          Serial.println();
#endif  // DEBUG_MODE
        }

        if (Xbox.getButtonPress(XBOX, i)) {
          button_bytes[1] |= _BV(B_XBOX);
#ifdef DEBUG_MODE
          Serial.print("XBOX");
          Serial.println();
#endif  // DEBUG_MODE
        }

        if (Xbox.getAnalogHat(LeftHatX, i) > 7500 || Xbox.getAnalogHat(LeftHatX, i) < -7500) {
          int8_t left_hatx = (int8_t)((float)Xbox.getAnalogHat(LeftHatX, i) / 256.0);
          // Lower byte in index 0
          l_hatx = (uint8_t)(left_hatx);
          // l_hatx[1] = (uint8_t)(left_hatx >> 8);
#ifdef DEBUG_MODE
          Serial.print("LeftHatX: ");
          Serial.println(left_hatx);
#endif  // DEBUG_MODE
        }

        if (Xbox.getAnalogHat(LeftHatY, i) > 7500 || Xbox.getAnalogHat(LeftHatY, i) < -7500) {
          int8_t left_haty = (int8_t)((float)Xbox.getAnalogHat(LeftHatY, i) / 256.0);
          // Lower byte in index 0
          l_haty = (uint8_t)(left_haty);
          // l_haty[1] = (uint8_t)(left_haty >> 8);
#ifdef DEBUG_MODE
          Serial.print("LeftHatY: ");
          Serial.println(left_haty);
#endif  // DEBUG_MODE
        }

        if (Xbox.getAnalogHat(RightHatX, i) > 7500 || Xbox.getAnalogHat(RightHatX, i) < -7500) {
          int8_t right_hatx = (int8_t)((float)Xbox.getAnalogHat(RightHatX, i) / 256.0);
          // Lower byte in index 0
          r_hatx = (uint8_t)(right_hatx);
          // r_hatx[1] = (uint8_t)(right_hatx >> 8);
#ifdef DEBUG_MODE
          Serial.print("RightHatX: ");
          Serial.println(right_hatx);
#endif  // DEBUG_MODE
        }

        if (Xbox.getAnalogHat(RightHatY, i) > 7500 || Xbox.getAnalogHat(RightHatY, i) < -7500) {
          int8_t right_haty = (int8_t)((float)Xbox.getAnalogHat(RightHatY, i) / 256.0);
          // Lower byte in index 0
          r_haty = (uint8_t)(right_haty);
          // r_haty[1] = (uint8_t)(right_haty >> 8);
        }
      }
    }
  }

#else  // USE_WIRED_JOYSTICK
  if (Xbox.Xbox360Connected) {

    if (Xbox.getButtonPress(L2)) {
      left_trigger = Xbox.getButtonPress(L2);
#ifdef DEBUG_MODE
      Serial.print("LT: ");
      Serial.println(left_trigger);
#endif  // DEBUG_MODE
    }

    if (Xbox.getButtonPress(R2)) {
      right_trigger = Xbox.getButtonPress(R2);
#ifdef DEBUG_MODE
      Serial.print("RT: ");
      Serial.println(right_trigger);
#endif  // DEBUG_MODE
    }

    if (Xbox.getButtonPress(L1)) {
      button_bytes[0] |= _BV(B_LB);
#ifdef DEBUG_MODE
      Serial.print("LB");
      Serial.println();

#endif  // DEBUG_MODE
    }

    if (Xbox.getButtonPress(R1)) {
      button_bytes[0] |= _BV(B_RB);
#ifdef DEBUG_MODE
      Serial.print("RB");
      Serial.println();
#endif  // DEBUG_MODE
    }

    if (Xbox.getButtonPress(X)) {
      button_bytes[0] |= _BV(B_X);
#ifdef DEBUG_MODE
      Serial.print("X");
      Serial.println();
#endif  // DEBUG_MODE
    }

    if (Xbox.getButtonPress(Y)) {
      button_bytes[0] |= _BV(B_Y);
#ifdef DEBUG_MODE
      Serial.print("Y");
      Serial.println();
#endif  // DEBUG_MODE
    }

    if (Xbox.getButtonPress(A)) {
      button_bytes[0] |= _BV(B_A);
#ifdef DEBUG_MODE
      Serial.print("A");
      Serial.println();
#endif  // DEBUG_MODE
    }

    if (Xbox.getButtonPress(B)) {
      button_bytes[0] |= _BV(B_B);
#ifdef DEBUG_MODE
      Serial.print("B");
      Serial.println();
#endif  // DEBUG_MODE
    }

    if (Xbox.getButtonPress(UP)) {
      button_bytes[1] |= _BV(B_UP);
#ifdef DEBUG_MODE
      Serial.print("UP");
      Serial.println();
#endif  // DEBUG_MODE
    }

    if (Xbox.getButtonPress(DOWN)) {
      button_bytes[1] |= _BV(B_DOWN);
#ifdef DEBUG_MODE
      Serial.print("DOWN");
      Serial.println();
#endif  // DEBUG_MODE
    }

    if (Xbox.getButtonPress(LEFT)) {
      button_bytes[1] |= _BV(B_LEFT);
#ifdef DEBUG_MODE
      Serial.print("LEFT");
      Serial.println();
#endif  // DEBUG_MODE
    }

    if (Xbox.getButtonPress(RIGHT)) {
      button_bytes[1] |= _BV(B_RIGHT);
#ifdef DEBUG_MODE
      Serial.print("RIGHT");
      Serial.println();
#endif  // DEBUG_MODE
    }
    if (Xbox.getButtonPress(L3)) {
      button_bytes[0] |= _BV(B_L3);
#ifdef DEBUG_MODE
      Serial.print("L3");
      Serial.println();
#endif
    }

    if (Xbox.getButtonPress(R3)) {
      button_bytes[0] |= _BV(B_R3);
#ifdef DEBUG_MODE
      Serial.print("R3");
      Serial.println();
#endif
    }

    if (Xbox.getButtonPress(START)) {
      button_bytes[1] |= _BV(B_START);
#ifdef DEBUG_MODE
      Serial.print("START");
      Serial.println();
#endif  // DEBUG_MODE
    }

    if (Xbox.getButtonPress(BACK)) {
      button_bytes[1] |= _BV(B_BACK);
#ifdef DEBUG_MODE
      Serial.print("BACK");
      Serial.println();
#endif  // DEBUG_MODE
    }

    if (Xbox.getButtonPress(XBOX)) {
      button_bytes[1] |= _BV(B_XBOX);
#ifdef DEBUG_MODE
      Serial.print("XBOX");
      Serial.println();
#endif  // DEBUG_MODE
    }

    if (Xbox.getAnalogHat(LeftHatX) > 7500 || Xbox.getAnalogHat(LeftHatX) < -7500) {
      int8_t left_hatx = (int8_t)((float)Xbox.getAnalogHat(LeftHatX) / 256.0);
      // Lower byte in index 0
      l_hatx = (uint8_t)(left_hatx);
      // l_hatx[1] = (uint8_t)(left_hatx >> 8);
#ifdef DEBUG_MODE
      Serial.print("LeftHatX: ");
      Serial.println(left_hatx);
#endif  // DEBUG_MODE
    }

    if (Xbox.getAnalogHat(LeftHatY) > 7500 || Xbox.getAnalogHat(LeftHatY) < -7500) {
      int8_t left_haty = (int8_t)((float)Xbox.getAnalogHat(LeftHatY) / 256.0);
      // Lower byte in index 0
      l_haty = (uint8_t)(left_haty);
      // l_haty[1] = (uint8_t)(left_haty >> 8);
#ifdef DEBUG_MODE
      Serial.print("LeftHatY: ");
      Serial.println(left_haty);
#endif  // DEBUG_MODE
    }

    if (Xbox.getAnalogHat(RightHatX) > 7500 || Xbox.getAnalogHat(RightHatX) < -7500) {
      int8_t right_hatx = (int8_t)((float)Xbox.getAnalogHat(RightHatX) / 256.0);
      // Lower byte in index 0
      r_hatx = (uint8_t)(right_hatx);
      // r_hatx[1] = (uint8_t)(right_hatx >> 8);
#ifdef DEBUG_MODE
      Serial.print("RightHatX: ");
      Serial.println(right_hatx);
#endif  // DEBUG_MODE
    }

    if (Xbox.getAnalogHat(RightHatY) > 7500 || Xbox.getAnalogHat(RightHatY) < -7500) {
      int8_t right_haty = (int8_t)((float)Xbox.getAnalogHat(RightHatY) / 256.0);
      // Lower byte in index 0
      r_haty = (uint8_t)(right_haty);
      // r_haty[1] = (uint8_t)(right_haty >> 8);
    }
  }
#endif  // _WIRELESS_JOYSTICK_

  payload[P_BUTTON1] = button_bytes[0];
  payload[P_BUTTON2] = button_bytes[1];
  payload[P_RHXL] = r_hatx;
  payload[P_LHXL] = l_hatx;
  payload[P_RHYL] = r_haty;
  payload[P_LHYL] = l_haty;
  payload[P_LT] = left_trigger;
  payload[P_RT] = right_trigger;
  if ((millis() - gCurr_time) > 50) {
    sendpacket();
    gCurr_time = millis();
  }

      // sendpacket();

}

void sendpacket() {
  xbee.send(zbTx);
//  if (xbee.readPacket(500)) {
//    // got a response!
//
//    // should be a znet tx status              
//    if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
//      xbee.getResponse().getZBTxStatusResponse(txStatus);
//
//      // get the delivery status, the fifth byte
//      if (txStatus.getDeliveryStatus() == SUCCESS) {
//        // success.  time to celebrate
//        flashLed(statusLed, 5, 50);
//      } else {
//        // the remote XBee did not receive our packet. is it powered on?
//        flashLed(errorLed, 3, 500);
//      }
//    }
//  } else if (xbee.getResponse().isError()) {
//    //nss.print("Error reading packet.  Error code: ");  
//    //nss.println(xbee.getResponse().getErrorCode());
//  } else {
//    // local XBee did not provide a timely TX Status Response -- should not happen
//    flashLed(errorLed, 2, 50);
//  }
//  delay(500);
}
