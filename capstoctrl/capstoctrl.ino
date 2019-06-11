#include <hidboot.h>

#include <usbhub.h>

# define MAX_RESET 7 //MAX3421E pin 12
# define MAX_GPX 8 //MAX3421E pin 17

class KbdRptParser: public KeyboardReportParser {
  protected: void OnKeyDown(uint8_t mod, uint8_t key);
  void OnKeyUp(uint8_t mod, uint8_t key);
};

uint8_t buf[8] = {
  0
};

int ctrllatch = 0;

void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key) {
  if (key == 0x39) // this is capslock - make it control
    ctrllatch = 1;
  else {
    if (ctrllatch == 1)
      mod = mod ^ 0x01;
    buf[0] = mod;
    buf[2] = key;
    Serial.write(buf, 8);
  }
}

void KbdRptParser::OnKeyUp(uint8_t mod, uint8_t key) {
  if (key == 0x39) { // this is capslock - made it control
    ctrllatch = 0;
  } else {
    buf[0] = 0;
    buf[2] = 0;
    Serial.write(buf, 8);
  }
}

USB Usb;
HIDBoot < USB_HID_PROTOCOL_KEYBOARD > HidKeyboard( & Usb);

KbdRptParser Prs;

void setup() {
  Serial.begin(9600);
  pinMode(MAX_GPX, INPUT);
  pinMode(MAX_RESET, OUTPUT);
  digitalWrite(MAX_RESET, LOW);
  delay(20); //wait 20ms 
  digitalWrite(MAX_RESET, HIGH);
  delay(20); //wait 20ms
  if (Usb.Init() == -1)
    Serial.println("OSC did not start.");
  HidKeyboard.SetReportParser(0, & Prs);
}

void loop() {
  Usb.Task();
}
