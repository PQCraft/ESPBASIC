///*
//Uncomment this section to enable the crash command
if (CMD == "CRASH") {
  sicon(3); printString("Are you sure?");
ck:
  rfKB();
  drawCursor(tcx, tcy);
  if (chr == 'y' || chr == 'Y') {
    goto oof;
  } else if (chr != 0) {
    goto whew;
  }
  goto ck;
oof:
  randomSeed(timer());
  long rip;
  do {
    rip = random(-98304, 165940);
    rfKB();
    drawCursor(tcx, tcy);
    pmem[rip] = random(0, 256 * chr);
    tbfr[random(-16, 65)][random(-4, 41)] = random(0, 256);
    GFX.setPenColor(random(0, 256), random(0, 256), random(0, 256));
    GFX.setBrushColor(random(0, 256), random(0, 256), random(0, 256));
    GFX.drawChar(random(-16384, 16384), random(-16384, 16384), random(0, 256));
    setFGColor(fgc);
    setBGColor(bgc);
    rfScrTxt();
  } while (true);
whew:
  printChar(13);
  nac = false;
}
//*/
///*
//Uncomment this section to enable the C64 theme command
if (CMD == "THEME64") {
  fgc = 43;
  bgc = 18;
  setFGColor(fgc);
  setBGColor(bgc);
  rfScrTxt();
  nac = false;
}
//*/
///*
//Uncomment this section to enable the C128 theme command
if (CMD == "THEME128") {
  fgc = 46;
  bgc = 21;
  setFGColor(fgc);
  setBGColor(bgc);
  rfScrTxt();
  nac = false;
}
//*/
///*
//Uncomment this section to enable the default theme command
if (CMD == "THEMEDF") {
  fgc = 60;
  bgc = 1;
  setFGColor(fgc);
  setBGColor(bgc);
  rfScrTxt();
  nac = false;
}
//*/
///*
//Uncomment this section to enable the b&w theme command
if (CMD == "THEMEBW") {
  fgc = 42;
  bgc = 0;
  setFGColor(fgc);
  setBGColor(bgc);
  rfScrTxt();
  nac = false;
}
//*/
///*
//Uncomment this section to enable the b&w bright theme command
if (CMD == "THEMEBWB") {
  fgc = 63;
  bgc = 0;
  setFGColor(fgc);
  setBGColor(bgc);
  rfScrTxt();
  nac = false;
}
//*/
