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
#include "esp32-hal-cpu.h"
//Preferences preferences;

fabgl::VGAController      VGA;
fabgl::PS2Controller      PS2;
fabgl::Canvas       GFX(&VGA);
SoundGenerator            SND;
SquareWaveformGenerator   SQW[4];
TriangleWaveformGenerator TRI[4];
SawtoothWaveformGenerator SWT[4];
SineWaveformGenerator     SNW[4];
NoiseWaveformGenerator    NWV[4];
unsigned long            nDur[4];

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
char          tbfr[48][40];
//char          pmem[1];
bool          ctlkey;
bool          altkey;
bool          sftkey;
//byte          sr;
//byte          hr;
long          pmp;
long          mas = 65536;
long          pms = mas;
//char          pmem[65536];
char          *pmem = (char *)malloc(mas);
char          *line = (char *)malloc(2048);
long          vmp = pms;
bool          noscroll = false;
byte          sc;
byte          gve = 0;
byte          gvt = 0;
float         gvn = 0;
byte          gfe = 0;
byte          gft = 0;
byte          vlines = 0;
byte          vclmns = 0;
int ltcx;
int ltcy;
//#define EBVER "0.0.0.0";
//#define EBREV "";

void setup() {
  setCpuFrequencyMhz(240);
  Serial.begin(115200);
  PS2.begin(PS2Preset::KeyboardPort0);
  auto KB0 = PS2.keyboard();
  VGA.begin();
  //VGA.setResolution(QVGA_320x240_60Hz);
  //VGA.setResolution(VGA_400x300_60Hz);
  setvm(2, false);
  //vMode(1);
  cls();
  tcm = 1;
  //WaveformGenerator.enable(true);
  for (byte i = 0; i < 4; i++) {
    /*WaveformGenerator * &SQW[i] = nullptr;
      WaveformGenerator * &TRI[i] = nullptr;
      WaveformGenerator * &SWT[i] = nullptr;
      WaveformGenerator * &SNW[i] = nullptr;
      WaveformGenerator * &NWV[i] = nullptr;*/
    SND.attach(&SQW[i]);
    SND.attach(&TRI[i]);
    SND.attach(&SWT[i]);
    SND.attach(&SNW[i]);
    SND.attach(&NWV[i]);
  }
  SND.play(true);
  //  for (byte i = 0; i < 4; i++) {
  //    SQW[i].enable
  //  }
  //KB0->setLEDs(1, 0, 0);
  do {
    rfKB();
    if (chr == 132) {
      SND_setCHType(0, 0);
      SND_setCHType(1, 0);
      SND_setCHType(2, 0);
      SND_setCHType(3, 0);
      goto pbt;
    }
    if (chr == 133) {
      SND_setCHType(0, 1);
      SND_setCHType(1, 1);
      SND_setCHType(2, 1);
      SND_setCHType(3, 1);
      goto pbt;
    }
    if (chr == 134) {
      SND_setCHType(0, 2);
      SND_setCHType(1, 2);
      SND_setCHType(2, 2);
      SND_setCHType(3, 2);
      goto pbt;
    }
    if (chr == 135) {
      SND_setCHType(0, 3);
      SND_setCHType(1, 3);
      SND_setCHType(2, 3);
      SND_setCHType(3, 3);
      goto pbt;
    }
    if (chr == 136) {
      SND_setCHType(0, 4);
      SND_setCHType(1, 4);
      SND_setCHType(2, 4);
      SND_setCHType(3, 4);
      goto pbt;
    }
  } while (timer() < 1500);
  goto cwb;
pbt:
  SND_playNote(0, 349.23, 0);
  delay(250);
  SND_playNote(1, 523.25, 745);
  delay(250);
  SND_playNote(2, 698.46, 745);
  delay(250);
  rfSND();
  delay(250);
  rfSND();
  delay(250);
  rfSND();
cwb:
  SND_setCHType(0, 0);
  SND_setCHType(1, 0);
  SND_setCHType(2, 0);
  SND_setCHType(3, 0);
  resetTimer(0);
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
  //pmem[pms - 1] = 255;
  mkvar("VER", 5, 0, "0.0.0.20");
  //mkvar("VER", 5, 0, "R.I.P.");
  mkvar("REV", 5, 0, "Alpha");
  Serial.println(F("Started ESPBASIC"));
  printString("ESPBASIC v" + getvar("VER") + " r" + getvar("REV") + "\n");
  //printString(getvar("E"));
  printString(String(pms, DEC) + F(" bytes total.\n"));
  printString(String(getFreePM(), DEC) + F(" bytes free.\n"));
  bool exitBASIC = false;
  String lineString;
  String CMD;
  String ARG;
  bool nac = true;
  tcm = 1;
  do {
    prompt(">");
    tcx = ltcx; tcy = ltcy; printString(line);
    printChar(13);
    lineString = line; lineString.trim();
    CMD = lineString + ' '; CMD.toUpperCase();
    ARG = getBackStr(lineString, CMD.indexOf(' ') + 1);
    ARG.trim();
    CMD = getFrontStr(CMD, CMD.indexOf(' '));
    //if (lineString.indexOf(' ') > -1 && lineString.indexOf(' ') == CMD.indexOf(' ')) {ARG = getBackStr(lineString, lineString.indexOf(' '));}
    nac = true;
    if (CMD == "") {
      nac = false;
    }
    if (CMD == "RESET") {
      ESP.restart();
      nac = false;
    }
    if (CMD == "DUMMY") {
      dummy();
      nac = false;
    }
    if (CMD == "CLS") {
      cls(); locate(0, 0);
      nac = false;
    }
    if (CMD == "PRINT" || CMD == "?") {
      if (ARG != "") {
        bool dpnlc = true;
        dpnlc = (ARG.charAt(ARG.length() - 1) == ';');
        if (dpnlc) {
          ARG = getFrontStr(ARG, ARG.length() - 1);
          if (ARG.length() == 0) {
            goto aib;
          }
        }
        String printStr = getval(ARG);
        if (gve > 0) {
          pgve(printStr);
        } else {
          printString(printStr);
          if (!dpnlc) {
            printChar(13);
          }
        }
      } else {
        printChar(13);
      }
aib:
      nac = false;
    }
    if (CMD == "MEMORY" || CMD == "MEM") {
      printString(String(pms, DEC) + F(" bytes total, ") + String(getFreePM(), DEC) + F(" bytes free.\n"));
      nac = false;
    }
    if (CMD == "SYSINFO" || CMD == "SYSINF") {
      printString("ESPBASIC v" + getvar("VER") + " r" + getvar("REV") + "\n");
      nac = false;
    }
    if (CMD == "SNDTEST") {
      unsigned long stdelay = timer() + 5000;
      printString("Testing sound...\n");
      SND_setCHType(0, 0);
      SND_setCHType(1, 1);
      SND_setCHType(2, 2);
      SND_setCHType(3, 3);
      for (byte i = 0; i < 4; i++) {
        for (int j = 160; j <= 1600; j++) {
          SND_playNote(i, j, 0);
          delay(5);
          //SQW[0].setFrequency(j);
        }
        delay(5);
        rfSND();
      }
      nac = false;
    }
    // Comment the next line to omit the extra commands.
#include "FunCMDs.h";
    //     ^^^^^^^^^^^^^
    if (CMD == "CPU40") {
      setCpuFrequencyMhz(40);
      nac = false;
    }
    if (CMD == "CPU80") {
      setCpuFrequencyMhz(80);
      nac = false;
    }
    if (CMD == "CPU160") {
      setCpuFrequencyMhz(160);
      nac = false;
    }
    if (CMD == "CPU240") {
      setCpuFrequencyMhz(240);
      nac = false;
    }
    if (CMD == "VMODE0") {
      locate(0, 0);
      setvm(0, false);
      cls();
      nac = false;
    }
    if (CMD == "VMODE1") {
      locate(0, 0);
      setvm(1, false);
      cls();
      nac = false;
    }
    if (CMD == "VMODE2") {
      locate(0, 0);
      setvm(2, false);
      cls();
      nac = false;
    }
    if (CMD == "VMODE3") {
      locate(0, 0);
      setvm(0, true);
      cls();
      nac = false;
    }
    if (CMD == "VMODE4") {
      locate(0, 0);
      setvm(1, true);
      cls();
      nac = false;
    }
    if (CMD == "VMODE5") {
      locate(0, 0);
      setvm(2, true);
      cls();
      nac = false;
    }
    if (nac) {
      printErr(3, CMD);
    }
  } while (!exitBASIC);
  do {} while (true);
}
void dummy() {
  tcm = 1;
  do {
    drawCursor(tcx, tcy);
    rfKB();
    if (chr == 27) {
      return;
    }
    if (chr == 137) {
      fgc++;
      setFGColor(fgc);
      chr = 0;
    }
    if (chr == 139) {
      bgc++;
      setBGColor(bgc);
      chr = 0;
    }
    if (chr == 136) {
      fgc--;
      setFGColor(fgc);
      chr = 0;
    }
    if (chr == 138) {
      bgc--;
      setBGColor(bgc);
      chr = 0;
    }
    if (chr == 140) {
      rfScrTxt();
      chr = 0;
    }
    if (chr == 141) {
      clrTxtBfr();
      rfScrTxt();
      chr = 0;
    }
    if ((chr > 31 && chr < 128) || chr == 8 || chr == 128 || chr == 129 || chr == 130 || chr == 131 || chr == 13) {
      printChar(chr);
    }
  } while (true);
}
void setvm(byte vm, bool sl) {
  String res = "";
  if (vm == 0) {
    res = VGA_256x384_60Hz;
    vlines = 48;
    vclmns = 32;
  } else if (vm == 1) {
    res = VGA_320x200_75Hz;
    vlines = 25;
    vclmns = 40;
  } else if (vm == 2) {
    res = QVGA_320x240_60Hz;
    vlines = 30;
    vclmns = 40;
  }
  if (sl) {
    res += " MultiScanBlank";
  }
  char cres[128];
  res.toCharArray(cres, 128);
  VGA.setResolution(cres);
  GFX.selectFont(&fabgl::FONT_8x8);
  GFX.setGlyphOptions(GlyphOptions().FillBackground(true));
  GFX.setScrollingRegion(0, 0, VGA.getScreenWidth() - 1, VGA.getScreenHeight() - 1);
  setFGColor(fgc);
  setBGColor(bgc);
}
void pgve(String edat) {
  if (gve == 1) {
    printErr(5, "");
  } else if (gve == 2) {
    printErr(6, "");
  } else if (gve == 3) {
    printErr(8, "");
  } else if (gve == 4) {
    printErr(4, edat);
  } else if (gve == 5) {
    printErr(9, "");
  }
}
void SND_setVol(byte ch, byte nv) {
  SQW[ch].setVolume(nv);
  TRI[ch].setVolume(nv);
  SWT[ch].setVolume(nv);
  SNW[ch].setVolume(nv);
  NWV[ch].setVolume(nv);
}
void SND_playNote(byte ch, int nf, int nd) {
  SQW[ch].setFrequency(nf);
  TRI[ch].setFrequency(nf);
  SWT[ch].setFrequency(nf);
  SNW[ch].setFrequency(nf);
  NWV[ch].setFrequency(nf);
  nDur[ch] = nd + timer();
}
void SND_setCHType(byte ch, byte nt) {
  switch (nt) {
    case 0:
      SQW[ch].enable(true);
      TRI[ch].enable(false);
      SWT[ch].enable(false);
      SNW[ch].enable(false);
      NWV[ch].enable(false);
      break;
    case 1:
      TRI[ch].enable(true);
      SWT[ch].enable(false);
      SNW[ch].enable(false);
      NWV[ch].enable(false);
      SQW[ch].enable(false);
      break;
    case 2:
      SWT[ch].enable(true);
      SNW[ch].enable(false);
      NWV[ch].enable(false);
      SQW[ch].enable(false);
      TRI[ch].enable(false);
      break;
    case 3:
      SNW[ch].enable(true);
      NWV[ch].enable(false);
      SQW[ch].enable(false);
      TRI[ch].enable(false);
      SWT[ch].enable(false);
      break;
    case 4:
      NWV[ch].enable(true);
      SQW[ch].enable(false);
      TRI[ch].enable(false);
      SWT[ch].enable(false);
      SNW[ch].enable(false);
      break;
  }
}
void rfSND() {
  for (int i = 0; i < 4; i++) {
    //Serial.println("nDur["+String(i, DEC)+"]: "+String(nDur[i], DEC)+", timer(): "+String(timer(), DEC));
    if (timer() > nDur[i]) {
      /*
        SQW[i].enable(false);
        TRI[i].enable(false);
        SWT[i].enable(false);
        SNW[i].enable(false);
        NWV[i].enable(false);*/
      SQW[i].setFrequency(0);
      TRI[i].setFrequency(0);
      SWT[i].setFrequency(0);
      SNW[i].setFrequency(0);
      NWV[i].setFrequency(0);
    }
  }
}
void prompt(String pt) {
  for (long i = 0; i < 2047; i++) {
    line[i] = 0;
  }
  int lp = 0;
  int ttcx;
  int ttcy;
  int noc = 0;
  printString(pt); ltcx = tcx; ltcy = tcy; lp = 0; noc = 0; sc = 0;
  do {
    rfKB();
    drawCursor(tcx, tcy);
    if (chr == 8 && lp > 0) {
      noc--;
      lp--;
      for (int i = lp; i < 2047; i++) {
        line[i] = line[i + 1];
      }; line[2047] = 0;
      printChar(8);
      ttcx = tcx; ttcy = tcy; tcx = ltcx; tcy = ltcy; noscroll = true;
      printString(line); printChar(32); tcx = ttcx; tcy = ttcy; noscroll = false;
    }
    if (chr == 127) {
      if (noc - lp > 0) {
        noc--;
        for (int i = lp; i < 2047; i++) {
          line[i] = line[i + 1];
        }; line[2047] = 0;
        ttcx = tcx; ttcy = tcy; tcx = ltcx; tcy = ltcy; noscroll = true;
        printString(line); printChar(32); tcx = ttcx; tcy = ttcy; noscroll = false;
      }
    }
    if (lp < noc && chr == 131) {
      GFX.drawChar(tcx * 8, tcy * 8, getbfrc(tcx, tcy));
      tcx++;
      lp++;
      printChar(0);
    }
    if (lp > 0 && chr == 130) {
      GFX.drawChar(tcx * 8, tcy * 8, getbfrc(tcx, tcy));
      tcx--;
      lp--;
      printChar(0);
    }
    if (chr == 145) {
      GFX.drawChar(tcx * 8, tcy * 8, getbfrc(tcx, tcy)); ltcy = ltcy - sc; sc = 0;
      tcx = ltcx;
      tcy = ltcy;
      lp = 0;
    }
    if (chr == 146) {
      GFX.drawChar(tcx * 8, tcy * 8, getbfrc(tcx, tcy)); ltcy = ltcy - sc; sc = 0;
      tcx = ltcx; tcy = ltcy; printString(line); ltcy = ltcy - sc; sc = 0; lp = noc;
    }
    if (chr > 31 && chr < 127 && lp < 2048) {
      ltcy = ltcy - sc; sc = 0;
      if (tcm) {
        line[lp] = chr;
        printChar(chr);
        if (lp == noc) {
          noc++;
        } lp++;
      } else {
        noc++;
        for (int i = 2047; i > lp; i--) {
          line[i] = line[i - 1];
        }; line[lp] = chr;
        ttcx = tcx; ttcy = tcy; tcx = ltcx; tcy = ltcy; printString(line); tcx = ttcx; tcy = ttcy - sc;
        ltcy = ltcy - sc; lp++; printChar(255); sc = 0;
      }
    }
  } while (chr != 13);
}
String getval(String in) {
  String out = "";
  gve = 0;
  gvt = 0;
  String cbfr = "";
  //String tbfr = "";
  in.trim();
  if (in == "") {
    return "";
  }
  in += '+';
  char cchr = 0;
  int fcp = 0;
  bool inString = false;
  bool isString = false;
  bool isNumber = false;
  bool inPrnth  = false;
  for (int i = 0; i < in.length(); i++) {
    cchr = in.charAt(i);
    bool jcis = false;
    bool jcip = false;
    if (cchr == '"' && inString && !inPrnth) {
      inString = false;
      jcis = true;
    }
    if (cchr == ')' && inPrnth && !inString) {
      inPrnth = false;
      jcip = true;
    }
    if (cchr == 39 && !inString) {
      cbfr += cchr;
rachk:
      i++;
      cchr = in.charAt(i);
      if (cchr != 39) {
        cbfr += cchr;
        goto rachk;
      }
      cbfr += cchr;
      i++;
      cchr = in.charAt(i);
    }
    if (isOp(cchr) && !inString && !inPrnth) {
      if (cbfr == "") {
        gve = 1;
        return "";
      }
      int inoldlen = in.length();
      cbfr.trim();
      char fchr = cbfr.charAt(0);
      char lchr = cbfr.charAt(cbfr.length() - 1);
      if (fchr == '"') {
        if (lchr != '"') {
          gve = 1;
          return "";
        }
        isString = true;
        in = getFrontStr(in, fcp) + cbfr + getChoppedStr(in, i, 0);
      } else if (isNum(cbfr)) {
        isNumber = true;
        in = getFrontStr(in, fcp) + cbfr + getChoppedStr(in, i, 0);
      } else if (fchr == 39 && lchr == 39) {
        if (cbfr.length() != 3) {
          gve = 1;
          return "";
        }
        isString = true;
      } else if (fchr == '(' && lchr == ')') {
        String gvhv = getval(getChoppedStr(cbfr, 1, 1));
        if (gve > 0) {
          return gvhv;
        }
        if (isNumber && gvt == 1) {
          gve = 2;
          return "";
        }
        if (isString && gvt == 0) {
          gve = 2;
          return "";
        }
        if (gvt == 1) {
          gvhv = '"' + gvhv + '"';
          isString = true;
        } else {
          isNumber = true;
        }
        in = getFrontStr(in, fcp) + gvhv + getChoppedStr(in, i, 0);
      } else if (isAlphaNumeric(fchr) && lchr == ')') {
        in = getFrontStr(in, fcp) + getfunc(cbfr) + getChoppedStr(in, i, 0);
        if (gfe > 0) {
          gve = 4;
          return getFrontStr(cbfr, cbfr.indexOf('('));
        }
      } else if (isValVN(cbfr)) {
        if (getvart(cbfr, true) == 5) {
          isString = true;
          in = getFrontStr(in, fcp) + '"' + getvar(cbfr) + '"' + getChoppedStr(in, i, 0);
        } else {
          isNumber = true;
          if (getvart(cbfr, false) == 0) {
            in = getFrontStr(in, fcp) + '0' + getChoppedStr(in, i, 0);
          } else {
            in = getFrontStr(in, fcp) + getvar(cbfr) + getChoppedStr(in, i, 0);
          }
        }
      } else {
        gve = 3;
        return "";
      }
      cbfr = "";
      i += in.length() - inoldlen;
      fcp = i + 1;
    } else {
      cbfr += cchr;
    }
    if (cchr == '(' && !jcis && !inPrnth) {
      inPrnth = true;
    }
    if (cchr == 39 && !jcip && !inString) {
      inString = true;
    }
  }
  if (inString || inPrnth) {
    gve = 1;
    return "";
  }
  if (isString && isNumber) {
    gve = 2;
    return "";
  }
  gvt = isString;
  inString = false;
  if (isString) {
    inString = false;
    //bool inChar = false;
    bool jcis = false;
    for (int i = 0; i < in.length(); i++) {
      cchr = in.charAt(i);
      jcis = false;
      if (cchr == '"' && inString) {
        inString = false;
        jcis = true;
      }
      if (cchr == 39 && !inString) {
        i++;
        cchr = in.charAt(i);
        out += cchr;
        i++;
        cchr = in.charAt(i);
      }
      if (inString) {
        out += cchr;
      } else if (cchr == '-' || cchr == '*' || cchr == '/' || cchr == '^') {
        gve = 1;
        return "";
      }
      if (cchr == '"' && !inString && !jcis) {
        inString = true;
      }
    }
    in = getFrontStr(in, in.length() - 1);
  } else {
    //out = getChoppedStr(in, 0, 1);
    in = '+' + in;
    String v1 = "";
    String v2 = "";
    String vo = "";
    int opos = -1;
    int fpos = -1;
    int lpos = -1;
dep:
    opos = in.indexOf('^');
    if (opos != -1) {
      fpos = opos + 1;
nc2:
      cchr = in.charAt(fpos);
      if (!isOp(cchr)) {
        v2 += cchr;
        fpos++;
        goto nc2;
      }
      lpos = opos - 1;
nc1:
      cchr = in.charAt(lpos);
      if (!isOp(cchr)) {
        v1 = cchr + v1;
        lpos--;
        goto nc1;
      }
      vo = String(pow(v1.toFloat(), v2.toFloat()));
      in = getFrontStr(in, lpos + 1) + vo + getChoppedStr(in, fpos, 0);
    } else {
      goto dmd;
    }
    goto dep;
dmd:
    int p1 = in.indexOf('*');
    int p2 = in.indexOf('/');
    opos = -1;
    bool pn = p1 != -1 && p2 != -1;
    bool d = false;
    if (pn) {
      if (p1 < p2) {
        opos = p1;
        d = false;
      } else if (p2 < p1) {
        opos = p2;
        d = true;
      }
    } else {
      if (p1 != -1) {
        opos = p1;
        d = false;
      }
      if (p2 != -1) {
        opos = p2;
        d = true;
      }
    }
    if (opos != -1) {
      v1 = "";
      v2 = "";
      fpos = opos + 1;
nc4:
      cchr = in.charAt(fpos);
      if (!isOp(cchr)) {
        v2 += cchr;
        fpos++;
        goto nc4;
      }
      lpos = opos - 1;
nc3:
      cchr = in.charAt(lpos);
      if (!isOp(cchr)) {
        v1 = cchr + v1;
        lpos--;
        goto nc3;
      }
      //Serial.println(v1+", "+v2);
      if (d) {
        if (v2.toFloat() == 0) {
          gve = 5;
          return "";
        }
        vo = String(v1.toFloat() / v2.toFloat());
      } else {
        vo = String(v1.toFloat() * v2.toFloat());
      }
      in = getFrontStr(in, lpos + 1) + vo + getChoppedStr(in, fpos, 0);
    } else {
      goto das;
    }
    goto dmd;
das:
    v1 = "";
    gvn = 0;
    bool s = false;
    for (int i = 1; i < in.length(); i++) {
      cchr = in.charAt(i);
      if (cchr == '+') {
        if (s) {
          gvn -= v1.toFloat();
        } else {
          gvn += v1.toFloat();
        }
        s = false;
        v1 = "";
      } else if (cchr == '-') {
        if (s) {
          gvn -= v1.toFloat();
        } else {
          gvn += v1.toFloat();
        }
        s = true;
        v1 = "";
      } else {
        v1 += cchr;
      }
    }
    out = String(gvn);
  }
  //if (ap) {in = '(' + in + ')';}
  //if (isNumber && in == "") {in = "0.00";}
  gvt = isString;
  //out = in;
  return out;
}
String getfunc(String fstr) {
  String FUNC = getFrontStr(fstr, fstr.indexOf('('));
  String FARG = getMidStr(fstr, fstr.indexOf('('), fstr.lastIndexOf(')'));
  FUNC.toUpperCase();
  //Serial.println(FUNC);
  gfe = 1;
  return "";
}
/*bool getfunct(String str) {
  return false;
  }*/
bool isOp(char tc) {
  String ops = "+-*/^";
  return (ops.indexOf(tc) > -1);
}
bool isNum(String str) {
  unsigned int stringLength = str.length();
  if (stringLength == 0) {
    return false;
  }
  boolean seenDecimal = false;
  for (unsigned int i = 0; i < stringLength; ++i) {
    if (isDigit(str.charAt(i))) {
      continue;
    } else if (i == 0 && str.charAt(0) == '-') {
      continue;
    }
    if (str.charAt(i) == '.') {
      if (seenDecimal) {
        return false;
      }
      seenDecimal = true;
      continue;
    }
    return false;
  }
  return true;
}
/*String runFunc(String f, String a) {
  return "";
  }*/
String getChoppedStr(String s, int c1, int c2) {
  String ns = "";
  for (int i = c1; i < s.length() - c2; i++) {
    ns = ns + s.charAt(i);
  }
  return ns;
}
String getMidStr(String s, int c1, int c2) {
  String ns = "";
  for (int i = c1; i <= c2; i++) {
    ns = ns + s.charAt(i);
  }
  return ns;
}
String getFrontStr(String s, int c) {
  String ns = "";
  for (int i = 0; i < c; i++) {
    ns = ns + s.charAt(i);
  }
  return ns;
}
String getBackStr(String s, int c) {
  String ns = "";
  for (int i = c; i < s.length(); i++) {
    ns = ns + s.charAt(i);
  }
  return ns;
}
long getFreePM() {
  long fm = vmp;
  long fmt = fm;
  for (long i = 0; i < fmt; i++) {
    if (pmem[i] != 0) {
      fm--;
    }
  }
  return fm;
}
void clrpmem() {
  for (long i = 0; i < pms; i++) {
    pmem[i] = 0;
    //if (pmem[i] != 0) {Serial.println("pmem format error: byte " + String(i, DEC) + " did not comply.");}
  }
  for (long i = 0; i < 2047; i++) {
    line[i] = 0;
  }
}
bool mkvar(String vn, byte t, float vlng, String vstr) {
  //Serial.println("mkvar");
  vn.toUpperCase();
  if (!isValVN(vn)) {
    return false;
  }
  //Serial.println(findvar(vn, vmp + 1));
  if (findvar(vn, vmp) != -1) {
    return false;
  }
  if (t > 5) {
    t = 1;
  }
  /* 1 == int
     2 == uint
     3 == byte
     4 == float
     5 == string
  */
  unsigned int vl;
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
  unsigned int vsize = vn.length() + vl + 5;
  long srchofst;
  int za;
  char chold;
  pmp = vmp - vsize;
  vmp = pmp;
  for (long i = vmp; i < pms; i++) {
    srchofst = 0;
    do {
      chold = pmem[i] + srchofst;
      srchofst++;
    } while (chold == 0);
    if (srchofst > vsize) {
      pmp = i;
    }
  }
  //Serial.print(F("srchofst: ")); Serial.println(String(srchofst, DEC)); Serial.print(F("vsize: ")); Serial.println(String(vsize, DEC));
  //Serial.print(F("pmp: ")); Serial.println(String(pmp, DEC));
  pmWStr(vn); pmWChr(0); pmWChr(highByte(vl)); pmWChr(lowByte(vl)); pmWChr(t);
  if (t != 5) {
    //int e;
    for (byte i = vl - 1; i >= 0; i--) {
      pmWChr(getByte(vlng, i));
    }
  } else {
    pmWStr(vstr); pmWChr(0);
  }
  //for (long i = vmp; i < pms; i++) {Serial.println(String(i, DEC) + ": " + String(pmem[i], DEC));};
  return true;
}
String getvar(String vn) {
  //Serial.println("getvar");
  vn.toUpperCase();
  long vpos = findvar(vn, vmp);
  if (vpos == -1) {
    return "";
  }
  byte vtype = getvart(vn, false);
  //Serial.println(vtype);
  if (vtype == 0) {
    if (getvart(vn, true) == 5) {
      return "";
    } else {
      return "0.00";
    }
  }
  String vval;
  if (vtype == 5) {
    long vdp = vpos + vn.length() + 4;
    while (pmem[vdp] != 0) {
      vval += pmem[vdp];
      vdp++;
    }
  }
  return vval;
}
void delvar(String vn) {
  vn.toUpperCase();
  long vpos = findvar(vn, vmp);
  if (vpos > -1) {
    long vdp = vpos; //+ vn.length() + 3;
    while (pmem[vdp] != 0) {
      vdp++;
      pmem[vdp] = 0;
    }
  }
}
int getvart(String vn, bool at) {
  //Serial.println("getvart");
  vn.toUpperCase(); vn.trim();
  if (isValVN(vn)) {
    long vpos = findvar(vn, vmp);
    if (vpos != -1) {
      return pmem[vpos + vn.length() + 3];
    }
    if (at) {
      if (vn.charAt(vn.length() - 1) == '$') {
        return 5;
      }
      return 1;
    } else {
      return 0;
    }
  } else {
    return -1;
  }
}
bool isValVN(String str) {
  if (str.length() == 0) {
    return false;
  }
  //if (isNum(str)) {
  //  return false;
  //}
  int anc = 0;
  for (int i = 0 ; i < str.length(); i++) {
    if (isAlphaNumeric(str.charAt(i)) || str.charAt(i) == '_') {
      anc++;
    }
    if (i == str.length() - 1 && str.charAt(i) == '$') {
      anc++;
    }
  }
  if (anc == str.length()) {
    return true;
  } else {
    return false;
  }
}
byte getByte(long v, byte p) {
  p = p * 8;
  return bitRead(v, p + 0) + (bitRead(v, p + 1) * 2) + (bitRead(v, p + 2) * 4) + (bitRead(v, p + 3) * 8) + (bitRead(v, p + 4) * 16) + (bitRead(v, p + 5) * 32) + (bitRead(v, p + 6) * 64) + (bitRead(v, p + 7) * 128);
}
/*byte setByte(long v, byte p) {
  p = p * 8;
  return bitRead(v,p+0)+(bitRead(v,p+1)*2)+(bitRead(v,p+2)*4)+(bitRead(v,p+3)*8)+(bitRead(v,p+4)*16)+(bitRead(v,p+5)*32)+(bitRead(v,p+6)*64)+(bitRead(v,p+7)*128);
  }*/
long findCharPM(byte fc, long pos) {
  for (long i = pos; i < pms; i++) {
    if (pmem[i] == fc) {
      return i;
    }
  }
  return -1;
}
long findCharRevPM(char fc, long pos) {
  for (long i = pos; i >= 0; i--) {
    //Serial.print(F("b: ")); Serial.println(i); Serial.print("v: "); Serial.println(String(pmem[i], DEC));
    if (pmem[i] == fc) {
      return i;
    }
  }
  return -1;
}
long findvar(String fvn, long pos) {
  //Serial.println("findvar");
  //Serial.println(fvn);
  String vbfr = "";
  char cvchr = 0;
  long fvpos = pos;
  for (long i = pos; i < pms - 1; i++) {
    cvchr = pmem[i];
    //Serial.println(String(i, DEC) + ", " + String(cvchr, DEC) + ", " + vbfr);
    if (cvchr == 0) {
      if (vbfr == fvn) {
        return fvpos;
      } else {
        i += (256 * pmem[i + 1]) + pmem[i + 2] + 4 ;
        fvpos = i + 1;
        vbfr = "";
      }
    } else {
      vbfr += cvchr;
    }
  }
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
  for (byte i = 0; i < vlines; i++) {
    for (byte j = 0; j < vclmns; j++) {
      GFX.drawChar(j * 8, i * 8, getbfrc(j, i));
    }
  }
}
void clrTxtBfr() {
  for (byte i = 0; i < vlines; i++) {
    for (byte j = 0; j < vclmns; j++) {
      setbfrc(j, i, 0);
    }
  }
}
void setFGColor(byte color) {
  fgc = color;
  GFX.setPenColor((bitRead(color, 5) * 2 + bitRead(color, 4)) * 64, (bitRead(color, 3) * 2 + bitRead(color, 2)) * 64, (bitRead(color, 1) * 2 + bitRead(color, 0)) * 64);
}
void setBGColor(byte color) {
  bgc = color;
  GFX.setBrushColor((bitRead(color, 5) * 2 + bitRead(color, 4)) * 64, (bitRead(color, 3) * 2 + bitRead(color, 2)) * 64, (bitRead(color, 1) * 2 + bitRead(color, 0)) * 64);
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
  if (tbp > vlines - 1) {
    tbp = 0;
  }
  if (tbp < 0) {
    tbp = vlines - 1;
  }
  yp = yp + tbp;
  if (yp > vlines - 1) {
    yp = yp - vlines;
  }
  return tbfr[yp][xp];
}
void setbfrc(byte xp, byte yp, char wchar) {
  if (tbp > vlines - 1) {
    tbp = 0;
  }
  if (tbp < 0) {
    tbp = vlines - 1;
  }
  yp = yp + tbp;
  if (yp > vlines - 1) {
    yp = yp - vlines;
  }
  tbfr[yp][xp] = wchar;
}
void cbfrcln() {
  for (byte i = 0; i < vclmns; i++) {
    setbfrc(i, vlines - 1, 0);
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
  clrTxtBfr();
}
unsigned long timer() {
  return millis() - tsv;
}
void resetTimer(unsigned long v) {
  tsv = millis() - v;
}
void printChar(char c) {
  //Serial.println(c);
  //Serial.println(tcx);
  //Serial.println(tcy);
  bool isBS = false;
  if (c == 13 || c == 10) {
    GFX.drawChar(tcx * 8, tcy * 8, getbfrc(tcx, tcy));
    tcy++;
    tcx = 0;
    c = 0;
  }
  if (c == 8) {
    GFX.drawChar(tcx * 8, tcy * 8, getbfrc(tcx, tcy));
    tcx--;
    c = 0;
    isBS = true;
  }
  if (c == 127) {
    GFX.drawChar(tcx * 8, tcy * 8, 32);
    setbfrc(tcx, tcy, 0);
    c = 0;
  }
  if (c == 131) {
    GFX.drawChar(tcx * 8, tcy * 8, getbfrc(tcx, tcy));
    tcx++;
    c = 0;
  }
  if (c == 130) {
    GFX.drawChar(tcx * 8, tcy * 8, getbfrc(tcx, tcy));
    tcx--;
    c = 0;
  }
  if (c == 129) {
    GFX.drawChar(tcx * 8, tcy * 8, getbfrc(tcx, tcy));
    tcy++;
    c = 0;
  }
  if (c == 128) {
    GFX.drawChar(tcx * 8, tcy * 8, getbfrc(tcx, tcy));
    if (tcy > 0) {
      tcy--;
    } c = 0;
  }
  if (c == 255) {
    tcx++;
    c = 0;
  }
  if (c != 0) {
    GFX.drawChar(tcx * 8, tcy * 8, c);
    setbfrc(tcx, tcy, c);
    tcx++;
  }
  if (tcx < 0 && tcy > 0) {
    tcy--;
    tcx = tcx + VGA.getScreenWidth() / 8;
  }
  if (tcx < 0 && tcy <= 0) {
    tcx++;
  }
  if (tcx >= VGA.getScreenWidth() / 8)  {
    tcx = 0;
    tcy++;
  }
  if (tcy >= VGA.getScreenHeight() / 8 && !noscroll) {
    tcy--;
    GFX.scroll(0, -8);
    tbp++;
    cbfrcln();
    sc++;
  }
  if (isBS) {
    setbfrc(tcx, tcy, 0);
    GFX.drawChar(tcx * 8, tcy * 8, 32);
  }
}
void printString(String s) {
  //Serial.println(s.length());
  for (int i = 0; i < s.length(); i++) {
    printChar(s.charAt(i));
  }
}
void printErr(byte e, String etxt) {
  sicon(0); if (e > 9 || e < 0) {
    e = 0;
  }
  switch (e) {
    case 0:
      printString(F("UNKNOWN ERROR"));
      break;
    case 1:
      printString(F("FATAL SYSTEM ERROR"));
      break;
    case 2:
      printString(F("SYSTEM ERROR"));
      break;
    case 3:
      printString(F("NOT A COMMAND"));
      break;
    case 4:
      printString(F("NOT A FUNCTION"));
      break;
    case 5:
      printString(F("SYNTAX ERROR"));
      break;
    case 6:
      printString(F("TYPE MISMATCH"));
      break;
    case 7:
      printString(F("OUT OF MEMORY"));
      break;
    case 8:
      printString(F("INVALID VARIABLE NAME"));
      break;
    case 9:
      printString(F("DIVIDE BY ZERO"));
      break;
  }
  if (etxt != "") {
    printString(": " + etxt);
  }
  printChar(13);
}
void locate(int x, int y) {
  tcx = x * 8;
  tcy = y * 8;
}
void rfKB() {
  chr = 0;
  auto KB0 = PS2.keyboard();
  if (KB0->virtualKeyAvailable()) {
    //if (Serial.peek() > -1) { printChar(Serial.read()); }
    bool down;
    auto inkey = KB0->getNextVirtualKey(&down);
    if (down) {
      chr = KB0->virtualKeyToASCII(inkey);
    }
    ctlkey = false;
    altkey = false;
    sftkey = false;
    if (KB0->isVKDown(fabgl::VK_LCTRL) || KB0->isVKDown(fabgl::VK_RCTRL)) {
      ctlkey = true;
    }
    if (KB0->isVKDown(fabgl::VK_LALT) || KB0->isVKDown(fabgl::VK_RALT)) {
      altkey = true;
    }
    if (KB0->isVKDown(fabgl::VK_LSHIFT) || KB0->isVKDown(fabgl::VK_RSHIFT)) {
      sftkey = true;
    }
    if (ctlkey && altkey && chr == 127) {
      esp_task_wdt_init(1, true);
      esp_task_wdt_add(NULL);
      while (true);
    }
    if (ctlkey && sftkey && chr == 27) {
      ESP.restart();
    }
    if (chr > 96 && chr < 128 && sftkey) {
      chr = chr - 32;
    }
    if (sftkey) {
      switch (chr) {
        case 48:
          chr = 41;
          break;
        case 49:
          chr = 33;
          break;
        case 50:
          chr = 64;
          break;
        case 51:
          chr = 35;
          break;
        case 52:
          chr = 36;
          break;
        case 53:
          chr = 37;
          break;
        case 54:
          chr = 94;
          break;
        case 55:
          chr = 38;
          break;
        case 56:
          chr = 42;
          break;
        case 57:
          chr = 40;
          break;
        case 45:
          chr = 95;
          break;
        case 61:
          chr = 43;
          break;
        case 91:
          chr = 123;
          break;
        case 93:
          chr = 125;
          break;
        case 92:
          chr = 124;
          break;
        case 59:
          chr = 58;
          break;
        case 39:
          chr = 34;
          break;
        case 44:
          chr = 60;
          break;
        case 46:
          chr = 62;
          break;
        case 47:
          chr = 63;
          break;
        case 96:
          chr = 126;
          break;
      }
    }
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
        case fabgl::VK_HOME:
          chr = 145;
          break;
        case fabgl::VK_END:
          chr = 146;
          break;
        case fabgl::VK_KP_HOME:
          chr = 145;
          break;
        case fabgl::VK_KP_END:
          chr = 146;
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
