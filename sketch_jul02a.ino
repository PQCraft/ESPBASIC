/*#include <Preferences.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "esp_spiffs.h"
#include "esp_task_wdt.h"
#include <stdio.h>*/
#include "fabgl.h"
#include <stdio.h> 
//#include <stdlib.h>
//Preferences preferences;

fabgl::VGAController      VGA;
fabgl::PS2Controller      PS2;
fabgl::Canvas       GFX(&VGA);
SoundGenerator            SND;
SquareWaveformGenerator   SQ0;
SquareWaveformGenerator   SQ1;
TriangleWaveformGenerator TR0;
TriangleWaveformGenerator TR1;
SawtoothWaveformGenerator ST0;
SineWaveformGenerator     SW0;
NoiseWaveformGenerator    WN0;

unsigned long tsv;
byte  vm;
byte fgc = 63;
byte bgc = 2;
int  tcx;
int  tcy;
bool tcm;
int  chr;
int  tbp;
char tbfr[30][40];
char pmem[1];
bool ctlkey;
bool altkey;
bool sftkey;
byte sr;
byte hr;
//goto spmi;

void setup() {
  Serial.begin(115200);
  PS2.begin(PS2Preset::KeyboardPort0);
  auto keyboard = PS2.keyboard();
  VGA.begin();
  VGA.setResolution(QVGA_320x240_60Hz);
  GFX.selectFont(&fabgl::FONT_8x8);
  GFX.setGlyphOptions(GlyphOptions().FillBackground(true));
  GFX.setScrollingRegion(0, 0, VGA.getScreenWidth() - 1, VGA.getScreenHeight() - 1);
  setFGColor(fgc);
  setBGColor(bgc);  
  //vMode(1);
  cls();
/*do {
    drawCursor(0, 0);
    delay(50);
  } while (sec() < 2);*/
}
void loop() {
  espbasic();
  dummy();
}
void espbasic() {
  String VER = "0.0.0.5";
  String REV = "Beta";
  printString(F("ESPBASIC "));
  printString(F("v"));
  printString(VER);
  printString(F(" r"));
  printString(REV);
  printString(F("\n"));
  //unsigned char pmem[16384];
  char *srsv = (char *)malloc(8192);
  if (srsv == NULL) {sicon(0); printErr(6, F("")); do {} while (true);}
  char *line = (char *)malloc(2048);
  if (line == NULL) {sicon(0); printErr(6, F("")); do {} while (true);}
  long mas = 163840;
  int acs = 16384;
  sicon(2);
  printString(F("Finding free memory...\n   Chunk size: ")); printString(String(acs)); printString(F(" bytes\n"));
  mrsv:
  char *pmem = (char *)malloc(mas);
  if (mas <= 0) {sicon(0); printErr(6, F("")); do {} while (true);}
  if (pmem == NULL) {mas = mas - acs; goto mrsv;}
  sicon(2);
  printString(F("Found ")); printString(String(mas)); printString(F(" bytes\n"));
  free(srsv);
  for (int i = 0; i < mas; i++) {
    pmem[i] = 0;
  }
  dummy();
}
void mkvar(String vn, byte t, String v) {
/* 0 == int
 * 1 == byte
 * 2 == long
 * 3 == string
*/
  //if t = 0 
}
void dummy() {
  tcm = 1;
  do {
    drawCursor(tcx, tcy);
    rfKB();
    if (chr == 137) {fgc++; setFGColor(fgc); chr = 0;}
    if (chr == 139) {bgc++; setBGColor(bgc); chr = 0;}
    if (chr == 136) {fgc--; setFGColor(fgc); chr = 0;}
    if (chr == 138) {bgc--; setBGColor(bgc); chr = 0;}
    if (chr == 140) {rfScrTxt(); chr = 0;}
    if (chr == 141) {clrTxtBfr(); rfScrTxt(); chr = 0;}
    if (chr > 0) {printChar(chr);}
  } while (true);
}
void rfScrTxt() {
  for (byte i = 0; i < 30; i++) {
    for (byte j = 0; j < 40; j++) {
      GFX.drawChar(j * 8, i * 8, getbfrc(j, i));
    }
  }
}
void clrTxtBfr() {
  for (byte i = 0; i < 30; i++) {
    for (byte j = 0; j < 40; j++) {
      setbfrc(j, i, 0);
    }
  }
}
void setFGColor(byte color) {
  fgc = color;
  GFX.setPenColor((bitRead(color, 5)*2+bitRead(color, 4))*64, (bitRead(color, 3)*2+bitRead(color, 2))*64, (bitRead(color, 1)*2+bitRead(color, 0))*64);
}
void setBGColor(byte color) {
  bgc = color;
  GFX.setBrushColor((bitRead(color, 5)*2+bitRead(color, 4))*64, (bitRead(color, 3)*2+bitRead(color, 2))*64, (bitRead(color, 1)*2+bitRead(color, 0))*64);
}
void drawCursor(int x, int y) {
  if (bitRead(cblink(), 0)) {
    if (tcm) {
      GFX.drawChar(x * 8, y * 8, 219);
    } else {
      GFX.drawChar(x * 8, y * 8, 95);
    }
  } else {
    GFX.drawChar(x * 8, y * 8, getbfrc(x, y));
  }
}
unsigned char getbfrc(byte xp, byte yp) {
  if (tbp > 29) {tbp = 0;}
  if (tbp < 0) {tbp = 29;}
  yp = yp + tbp;
  if (yp > 29) {yp = yp - 30;}
  return tbfr[yp][xp];
}
void setbfrc(byte xp, byte yp, char wchar) {
  if (tbp > 29) {tbp = 0;}
  if (tbp < 0) {tbp = 29;}
  yp = yp + tbp;
  if (yp > 29) {yp = yp - 30;}
  tbfr[yp][xp] = wchar;
}
void cbfrcln() {
  for (byte i = 0; i < 40; i++) {
    setbfrc(i, 29, 0);
  }
}
unsigned long sec() {
  return round(millis() / 1000);
}
unsigned long cblink() {
  return round(millis() / 250);
}
void cls() {
  GFX.fillRectangle(0, 0, VGA.getScreenWidth() - 1, VGA.getScreenHeight() - 1);
}
unsigned long timer() {
  return millis() - tsv;
}
void resetTimer(unsigned long v) {
  tsv = millis() - v;
}
void vMode(byte m) {
  m = bitRead(m, 1) * 2 + bitRead(m, 0);
//if (m == 0) { VGA.setResolution(VGA_256x384_60Hz); }
  if (m == 0) { VGA.setResolution(VGA_320x200_75Hz); }
  if (m == 1) { VGA.setResolution(QVGA_320x240_60Hz); }
  if (m == 2) { VGA.setResolution(VGA_512x384_60Hz); }
  if (m == 3) { VGA.setResolution(VGA_640x400_70Hz); }
//if (m == 3) { VGA.setResolution(VGA_512x384_60Hz); }
  GFX.selectFont(&fabgl::FONT_8x8);
  GFX.setGlyphOptions(GlyphOptions().FillBackground(true));
  GFX.setScrollingRegion(0, 0, VGA.getScreenWidth() - 1, VGA.getScreenHeight() - 1);
  setFGColor(fgc);
  setBGColor(bgc);
  //cls();
}
void printChar(byte c) {
  //Serial.println(c);
  //Serial.println(tcx);
  //Serial.println(tcy);
  if (c == 13 || c == 10) { GFX.drawChar(tcx * 8, tcy * 8, getbfrc(tcx, tcy)); tcy++; tcx = 0; c = 0; }
  if (c == 8) { GFX.drawChar(tcx * 8, tcy * 8, getbfrc(tcx, tcy)); tcx--; setbfrc(tcx, tcy, 0); GFX.drawChar(tcx * 8, tcy * 8, 32); c = 0; }
  if (c == 131) { GFX.drawChar(tcx * 8, tcy * 8, getbfrc(tcx, tcy)); tcx++; c = 0; }
  if (c == 130) { GFX.drawChar(tcx * 8, tcy * 8, getbfrc(tcx, tcy)); tcx--; c = 0; }
  if (c == 129) { GFX.drawChar(tcx * 8, tcy * 8, getbfrc(tcx, tcy)); tcy++; c = 0; }
  if (c == 128) { GFX.drawChar(tcx * 8, tcy * 8, getbfrc(tcx, tcy)); if (tcy > 0) {tcy--;} c = 0; }
  if (c == 255) { tcx++; }
  if (c != 0) { GFX.drawChar(tcx * 8, tcy * 8, c); setbfrc(tcx, tcy, c); tcx++; }
  if (tcx < 0 && tcy > 0) { tcy--; tcx = tcx + VGA.getScreenWidth() / 8; }
  if (tcx < 0 && tcy <= 0) { tcx++; }
  if (tcx >= VGA.getScreenWidth() / 8)  { tcx = 0; tcy++; }
  if (tcy >= VGA.getScreenHeight() / 8) { tcy--; GFX.scroll(0, -8); tbp++; cbfrcln(); }
}
void printString(String s) {
  //Serial.println(s.length());
  for (int i = 0; i < s.length(); i++) {
    printChar(s.charAt(i));
  }
}
void printErr(byte e, String etxt) {
  sicon(0); if (e > 6) {e = 0;}
  switch (e) {
    case 0:
      printString(F("UNKNOWN ERROR"));
      if (etxt != "") {
        printString(": " + etxt);
      }
    break;
    case 1:
      printString(F("FATAL SYSTEM ERROR"));
      if (etxt != "") {
        printString(": " + etxt);
      }
    break;
    case 2:
      printString(F("SYSTEM ERROR"));
      if (etxt != "") {
        printString(": " + etxt);
      }
    break;
    case 3:
      printString(F("NOT A COMMAND"));
      if (etxt != "") {
        printString(": " + etxt);
      }
    break;
    case 4:
      printString(F("SYNTAX ERROR"));
      if (etxt != "") {
        printString(": " + etxt);
      }
    break;
    case 5:
      printString(F("TYPE MISMATCH"));
      if (etxt != "") {
        printString(": " + etxt);
      }
    break;
    case 6:
      printString(F("OUT OF MEMORY"));
      if (etxt != "") {
        printString(": " + etxt);
      }
    break;
  }
}
void locate(int x, int y) {
  tcx = x;
  tcy = y;
}
void bas_ff() {
    
}
void rfKB() { 
  chr = 0;
  auto keyboard = PS2.keyboard();
  if (keyboard->virtualKeyAvailable()) {
    //if (Serial.peek() > -1) { printChar(Serial.read()); }
    bool down;
    auto inkey = keyboard->getNextVirtualKey(&down);
    if (down) {chr = keyboard->virtualKeyToASCII(inkey);}
    ctlkey = false;
    altkey = false;
    sftkey = false;
    if (keyboard->isVKDown(fabgl::VK_LCTRL) || keyboard->isVKDown(fabgl::VK_RCTRL)) {
      ctlkey = true;
    }
    if (keyboard->isVKDown(fabgl::VK_LALT) || keyboard->isVKDown(fabgl::VK_RALT)) {
      altkey = true;
    }
    if (keyboard->isVKDown(fabgl::VK_LSHIFT) || keyboard->isVKDown(fabgl::VK_RSHIFT)) {
      sftkey = true;
    }
    if (ctlkey && altkey && chr == 127) {ESP.restart();}
    if (chr < 0 && down) {
      switch (inkey) {
        case fabgl::VK_UP:
          chr = 128;
          break;
        case fabgl::VK_DOWN:
          chr = 129;
          break;
        case fabgl::VK_LEFT:
          chr = 130;
          break;
        case fabgl::VK_RIGHT:
          chr = 131;
          break;
        case fabgl::VK_KP_UP:
          chr = 128;
          break;
        case fabgl::VK_KP_DOWN:
          chr = 129;
          break;
        case fabgl::VK_KP_LEFT:
          chr = 130;
          break;
        case fabgl::VK_KP_RIGHT:
          chr = 131;
          break;
        case fabgl::VK_F1:
          chr = 132;
          break;
        case fabgl::VK_F2:
          chr = 133;
          break;
        case fabgl::VK_F3:
          chr = 134;
          break;
        case fabgl::VK_F4:
          chr = 135;
          break;
        case fabgl::VK_F5:
          chr = 136;
          break;
        case fabgl::VK_F6:
          chr = 137;
          break;
        case fabgl::VK_F7:
          chr = 138;
          break;
        case fabgl::VK_F8:
          chr = 139;
          break;
        case fabgl::VK_F9:
          chr = 140;
          break;
        case fabgl::VK_F10:
          chr = 141;
          break;
        case fabgl::VK_F11:
          chr = 142;
          break;
        case fabgl::VK_F12:
          chr = 143;
          break;
        case fabgl::VK_PAUSE:
          chr = 144;
          break;
        case fabgl::VK_INSERT:
          tcm = !tcm;
          break;
        case fabgl::VK_KP_INSERT:
          tcm = !tcm;
          break;
      }
    }
  }
}
void sicon (byte n) {
  switch (n) {
    case 0:
      GFX.setPenColor(192, 0, 0);
      printString("X");
      setFGColor(fgc);
      printString(": ");
      break;
    case 1:
      GFX.setPenColor(192, 192, 64);
      printString("!");
      setFGColor(fgc);
      printString(": ");
      break;
    case 2:
      GFX.setPenColor(64, 192, 192);
      printString("I");
      setFGColor(fgc);
      printString(": ");
      break;
    case 3:
      GFX.setPenColor(128, 128, 128);
      printString("?");
      setFGColor(fgc);
      printString(": ");
      break;
  }
}
