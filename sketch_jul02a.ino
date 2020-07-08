/*#include <Preferences.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "esp_spiffs.h"
#include "esp_task_wdt.h"
#include <stdio.h>*/
#include "fabgl.h"
//#include <stdio.h>
//#include <stdlib.h>
#include <esp_int_wdt.h>
#include <esp_task_wdt.h>
//Preferences preferences;

fabgl::VGAController      VGA;
fabgl::PS2Controller      PS2;
fabgl::Canvas       GFX(&VGA);
SoundGenerator            SND;
/*SquareWaveformGenerator   SQ0;
SquareWaveformGenerator   SQ1;
TriangleWaveformGenerator TR0;
TriangleWaveformGenerator TR1;
SawtoothWaveformGenerator ST0;
SineWaveformGenerator     SW0;
NoiseWaveformGenerator    WN0;*/

unsigned long tsv;
//byte          vm;
//byte          fgc = 63;
//byte          bgc = 2;
byte          fgc = 60;
byte          bgc = 1;
int           tcx;
int           tcy;
bool          tcm;
int           chr;
int           tbp;
char          tbfr[30][40];
//char          pmem[1];
bool          ctlkey;
bool          altkey;
bool          sftkey;
//byte          sr;
//byte          hr;
long          pmp;
long          mas = 65536;
long          pms = 65536;
char          *pmem = (char *)malloc(mas);
char          *line = (char *)malloc(2048);
long          vmp = pms - 1;
bool          noscroll = false;
byte          sc;
//#define EBVER "0.0.0.0";
//#define EBREV "";

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
  //unsigned char pmem[16384];
/*char *line = (char *)malloc(2048);
  if (line == NULL) {sicon(0); printErr(6, F("")); do {} while (true);}
  mas = 163840;
  long acs = 98303;
  sicon(2);
  printString(F("Finding free memory...\n   Chunk size: ")); printString(String(acs)); printString(F(" bytes\n"));
  mrsv:
  pms = mas;
  Serial.println(mas);
  Serial.println(pms);
  char *pmem = (char *)malloc(mas);
  if (mas <= 0) {sicon(0); printErr(6, F("")); do {} while (true);}
  if (pmem == NULL) {mas = mas - (acs + 1); goto mrsv;}
  sicon(2);
  printString(F("Found ")); printString(String(mas)); printString(F(" byte(s)\n"));*/
  mas = pms;
  clrpmem();
  pmem[pms - 1] = 255;
  mkvar("VER", 5, 0, "0.0.0.9");
  mkvar("REV", 5, 0, "Alpha");
  Serial.println(F("Started ESPBASIC"));
  printString("ESPBASIC v" + getvar("VER") + " r" + getvar("REV") + "\n");
  //printString(getvar("E"));
  printString(String(pms, DEC) + F(" bytes total.\n"));
  printString(String(getFreePM(), DEC) + F(" bytes free.\n"));
  int lp = 0;
  int ttcx;
  int ttcy;
  int ltcx;
  int ltcy;
  bool exitBASIC = false;
  String lineString;
  String CMD;
  tcm = 1;
  do {
    printChar('>'); ltcx = tcx; ltcy = tcy; lp = 0; sc = 0;
    do {
      rfKB();
      drawCursor(tcx, tcy);
      if (chr == 8) {
        if (lp > 0) {
          lp--;
          for (int i = lp; i < 2047; i++) {line[i] = line[i + 1];}; line[2047] = 0;
          printChar(8);
          ttcx = tcx; ttcy = tcy; tcx = ltcx; tcy = ltcy; printString(line); 
          noscroll = true; printChar(32); tcx = ttcx; tcy = ttcy; noscroll = false;
        }
      }
      if (chr > 31 && chr < 127 && lp < 2048) {
        if (tcm) {line[lp]=chr; printChar(chr); lp++;} else {
          for (int i = 2047; i > lp; i--) {line[i] = line[i - 1];}; line[lp]=chr;
          ttcx = tcx; ttcy = tcy; tcx = ltcx; tcy = ltcy; printString(line); tcx = ttcx; tcy = ttcy - sc;
          ltcy = ltcy - sc; lp++; printChar(255); sc = 0;
        }
      }
    } while (chr != 13);
    tcx = ltcx; tcy = ltcy; printString(line);
    lineString = line; lineString.trim();
    printChar(13);
    CMD = lineString + ' '; CMD.toUpperCase();
    if (getFrontStr(CMD, 5) == "EXIT ") {exitBASIC = true;}
    for (long i = 0; i < 2047; i++) {
      line[i] = 0;
    }  
  } while (!exitBASIC);
  dummy();
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
    if ((chr > 31 && chr < 127) || chr == 8 || chr == 128 || chr == 129 || chr == 130 || chr == 131 || chr == 13) {printChar(chr);}
  } while (true);
}
String getFrontStr(String s, int c) {
  String ns;
  for (int i = 0; i < c; i++) {
    ns = ns + s.charAt(i);
  }
  return ns;
}
long getFreePM() {
  long fm = vmp;
  long fmt = fm;
  for (long i = 0; i < fmt; i++) {
    if (pmem[i] != 0) {fm--;}
  }
  return fm;
}
void clrpmem() {
  for (long i = 0; i < pms; i++) {
    pmem[i] = 0;
  }  
  for (long i = 0; i < 2047; i++) {
    line[i] = 0;
  }  
}
void mkvar(String vn, byte t, float vlng, String vstr) {
  Serial.println(findvar(vn, vmp + 1));
  if (findvar(vn, vmp + 1) != -1) {return;};
  if (t > 5) {t = 1;}
/* 1 == int
 * 2 == uint
 * 3 == byte
 * 4 == float
 * 5 == string
*/
  byte vl;
  switch (t) {
    case 1:
      vl = 2;
    break;
    case 2:
      vl = 2;
    break;
    case 3:
      vl = 1;
    break;
    case 4:
      vl = 4;
    break;
    case 5:
      vl = vstr.length();
    break;
  }
  long vsize = vn.length() + vl + 5;
  long srchofst;
  int za;
  char chold;
  pmp = vmp - vsize;
  vmp = pmp;
  for (long i = findCharRevPM(255, pms - 1); i < pms; i++) {
    srchofst = 0;
    do {
      chold = pmem[i] + srchofst;
      srchofst++;
    } while (chold == 0);
    if (srchofst > vsize) {pmp = i;}
  }
  Serial.print(F("srchofst: ")); Serial.println(String(srchofst, DEC)); Serial.print(F("vsize: ")); Serial.println(String(vsize, DEC)); 
  Serial.print(F("pmp: ")); Serial.println(String(pmp, DEC));
  pmWChr(255); pmWChr(0); pmWChr('!'); pmWStr(vn); pmWChr(0); pmWChr(t);
  if (t != 5) {
    //int e;
    for (byte i = vl - 1; i >= 0; i--) {
      pmWChr(getByte(vlng, i));
    }
  } else {pmWStr(vstr);}
  pmWChr(0);
  for (long i = vmp; i < pms; i++) {Serial.println(String(pmem[i], DEC));};
}
String getvar(String vn) {
  long vpos = findvar(vn, vmp + 1);
  if (vpos == -1) {return "";}
  byte vtype = getvart(vn);
  if (vtype == 0) {return "";}
  String vval;
  if (vtype == 5) {
    long vdp = vpos + vn.length() + 3;
    do {
      vdp++;
      if (pmem[vdp] != 0) {vval = vval + pmem[vdp];}
    } while (pmem[vdp] != 0);
  }
  return vval;
  //for (long i = findCharRevPM(255, pms); i < pms; i++) {Serial.println(String(pmem[i], DEC));};
  /*if (vpos > -1) {
    for (long i = vpos; i < pms; i++) {Serial.println(String(pmem[i], DEC));};
  } else {Serial.println("var not found");}*/
}
void delvar(String vn) {
  long vpos = findvar(vn, vmp + 1);
  if (vpos > -1) {
    long vdp = vpos + vn.length() + 3;
    do {
      vdp++;
      if (pmem[vdp] != 0) {pmem[vdp] = 0;}
    } while (pmem[vdp] != 0);
  }
}
byte getvart(String vn) {
  long vpos = findvar(vn, vmp + 1);
  if (vpos == -1) {return 0;}
  return pmem[vpos + vn.length() + 3];
}
byte getByte(long v, byte p) {
  p = p * 8;
  return bitRead(v,p+0)+(bitRead(v,p+1)*2)+(bitRead(v,p+2)*4)+(bitRead(v,p+3)*8)+(bitRead(v,p+4)*16)+(bitRead(v,p+5)*32)+(bitRead(v,p+6)*64)+(bitRead(v,p+7)*128);
}
/*byte setByte(long v, byte p) {
  p = p * 8;
  return bitRead(v,p+0)+(bitRead(v,p+1)*2)+(bitRead(v,p+2)*4)+(bitRead(v,p+3)*8)+(bitRead(v,p+4)*16)+(bitRead(v,p+5)*32)+(bitRead(v,p+6)*64)+(bitRead(v,p+7)*128);
}*/
long findCharPM(byte fc, long pos) {
  for (long i = pos; i < pms; i++) {
    if (pmem[i] == fc) {return i;}
  }
  return -1;
}
long findCharRevPM(char fc, long pos) {
  for (long i = pos; i >= 0; i--) {
    //Serial.print(F("b: ")); Serial.println(i); Serial.print("v: "); Serial.println(String(pmem[i], DEC));
    if (pmem[i] == fc) {return i;}
  }
  return -1;
}
long findvar(String fs, long pos) {
  long cp;
  int s;
  long t;
  fs = '!' + fs;
  pos--;
  retry:
  pos++;
  if (pos > pms - 2) {return -1;}
  cp = findCharPM(0, pos);
  if (cp > -1) {
    s = 1;
    for (long i = cp + 1; i <= fs.length() + cp; i++) {
      if (pmem[i] == fs.charAt(i - cp - 1)) {s++;}
      t = i;
    }
  }
  if (pmem[t + 1] == 0) {s++;}
  if (pmem[t + 2] == 0) {goto retry;}
  if (s == fs.length() + 2) {return pos;} else {goto retry;}
  return -1;
}
void pmWChr(byte wc) {
  pmem[pmp] = wc; pmp++;
}
void pmWStr(String s) {
  for (int i = 0; i < s.length(); i++) {
    pmWChr(s.charAt(i));
  }
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
  return round(millis() / 200);
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
/*void vMode(byte m) {
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
}*/
void printChar(char c) {
  //Serial.println(c);
  //Serial.println(tcx);
  //Serial.println(tcy);
  if (c == 13 || c == 10) { GFX.drawChar(tcx * 8, tcy * 8, getbfrc(tcx, tcy)); tcy++; tcx = 0; c = 0; }
  if (c == 8) { GFX.drawChar(tcx * 8, tcy * 8, getbfrc(tcx, tcy)); tcx--; setbfrc(tcx, tcy, 0); GFX.drawChar(tcx * 8, tcy * 8, 32); c = 0; }
  if (c == 127) { GFX.drawChar(tcx * 8, tcy * 8, 32); setbfrc(tcx, tcy, 0); c = 0; }
  if (c == 131) { GFX.drawChar(tcx * 8, tcy * 8, getbfrc(tcx, tcy)); tcx++; c = 0; }
  if (c == 130) { GFX.drawChar(tcx * 8, tcy * 8, getbfrc(tcx, tcy)); tcx--; c = 0; }
  if (c == 129) { GFX.drawChar(tcx * 8, tcy * 8, getbfrc(tcx, tcy)); tcy++; c = 0; }
  if (c == 128) { GFX.drawChar(tcx * 8, tcy * 8, getbfrc(tcx, tcy)); if (tcy > 0) {tcy--;} c = 0; }
  if (c == 255) { tcx++; c = 0; }
  if (c != 0) { GFX.drawChar(tcx * 8, tcy * 8, c); setbfrc(tcx, tcy, c); tcx++; }
  if (tcx < 0 && tcy > 0) { tcy--; tcx = tcx + VGA.getScreenWidth() / 8; }
  if (tcx < 0 && tcy <= 0) { tcx++; }
  if (tcx >= VGA.getScreenWidth() / 8)  { tcx = 0; tcy++; }
  if (tcy >= VGA.getScreenHeight() / 8 && !noscroll) { tcy--; GFX.scroll(0, -8); tbp++; cbfrcln(); sc++; }
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
    if (ctlkey && altkey && chr == 127) {esp_task_wdt_init(1,true); esp_task_wdt_add(NULL); while(true);}
    if (ctlkey && sftkey && chr == 27) {ESP.restart();}
    if (chr > 96 && chr < 123 && sftkey) {chr = chr - 32;}
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
