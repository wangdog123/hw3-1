const int buttonPin = 12;
const int r = 3;
const int g = 4;
const int b = 5;

int LEDOrder = 0;
unsigned long LEDchangeTimer = 0;
int mode = 0;
bool blinkOn = true;

int lastButtonState = HIGH;
unsigned long buttonPressTime = 0;
unsigned long buttonReleaseTime = 0;
bool isHolding = false;
const unsigned long longPressDuration = 500;

// 🔧 LED總開關
bool isLEDon = true;

// 🔧 把 lastLEDOrder 拉出來成全域變數，方便重設
int lastLEDOrder = -1;

void setup() {
  pinMode(r, OUTPUT);
  pinMode(g, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(4800);
}

void loop() {
  int buttonState = digitalRead(buttonPin);
  unsigned long currentTime = millis();

  // 按下瞬間
  if (buttonState == LOW && lastButtonState == HIGH) {
    buttonPressTime = currentTime;
    isHolding = true;
  }

  // 長按偵測：切換LED電源
  if (buttonState == LOW && (currentTime - buttonPressTime) > longPressDuration && isHolding) {
    isLEDon = !isLEDon;
    Serial.print("Power: ");
    Serial.println(isLEDon ? "ON" : "OFF");

    if (!isLEDon) {
      LEDColor(1, 1, 1);
    }

    isHolding = false;
  }

  // 放開按鈕
  if (buttonState == HIGH && lastButtonState == LOW) {
    buttonReleaseTime = currentTime;
    unsigned long pressDuration = buttonReleaseTime - buttonPressTime;

    if (pressDuration < longPressDuration) {
      if (isLEDon) {
        mode++;
        if (mode > 1) mode = 0;

        Serial.print("mode=");
        Serial.println(mode);

        // 🔧 重置所有與顏色更新有關的狀態
        LEDchangeTimer = currentTime;
        lastLEDOrder = -1;  // ← 這行是關鍵，避免卡色！
      }
    }
  }

  // LED啟動時執行循環
  if (isLEDon) {
    cycle();
  }

  lastButtonState = buttonState;
}

void LEDColor(bool rr, bool gg, bool bb) {
  digitalWrite(r, rr);
  digitalWrite(g, gg);
  digitalWrite(b, bb);
}

void cycle() {
  unsigned long currentTime = millis();

  if (mode == 0) {
    if (currentTime - LEDchangeTimer >= 1500) {
      LEDchangeTimer = currentTime;
      LEDOrder++;
      if (LEDOrder > 2) LEDOrder = 0;
    }

    if (LEDOrder != lastLEDOrder) {
      lastLEDOrder = LEDOrder;
      switch (LEDOrder) {
        case 0: LEDColor(0, 1, 0); break;
        case 1: LEDColor(1, 0, 0); break;
        case 2: LEDColor(0, 0, 1); break;
      }
    }
  }

  else if (mode == 1) {
    if (currentTime - LEDchangeTimer >= 500) {
      blinkOn = !blinkOn;
      LEDchangeTimer = currentTime;
    }
    LEDColor(blinkOn ? 1 : 0, blinkOn ? 1 : 0, blinkOn ? 1 : 0);
  }
}
