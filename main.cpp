#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <SPI.h>

// Chân chuông báo động
#define BUZZER_PIN 15

// Các chân cho bộ điều khiển bằng nút bấm
#define BUTTON_MENU 12
#define BUTTON_UP 13
#define BUTTON_DOWN 14
#define BUTTON_LEFT 25
#define BUTTON_RIGHT 26

// Chân sensor nhiệt độ
#define TEMP_SENSOR_PIN 4

// Chân relay điều khiển sợi đốt, quạt
#define RELAY_HEATER 16 // (Đèn đỏ)
#define RELAY_FAN 17   //  (Đèn xanh)

// Loại cảm biến
#define DHTTYPE DHT22

RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(TEMP_SENSOR_PIN, DHTTYPE);

// Các trạng thái của hệ thống
enum MenuState
{
  SET_START_TIME,
  SET_HATCH_TIME,
  SET_TEMPERATURE,
  RUNNING
};
MenuState currentState = SET_START_TIME;

// Định dạng thời gian bắt đầu/ dự kiến: dd/mm/yyyy:hh
int startTime[4] = {1, 1, 2024, 0};
int hatchTime[4] = {1, 1, 2024, 0};
float setTemperature = 37.5;

// Điều chỉnh ngày/tháng/năm/giờ
int DateTime_index = 0;

bool isRunning = false;
bool buzzer_state = false;
unsigned long lastBuzzerTime = 0;

// Hàm màn hình hiển thị cấu hình
void displayMenu();

// Hàm hiển thị trong quá trình ấp
void runIncubator();

// Hàm cấu hình các thông số
void handleMenu();

// Hàm điều chỉnh giá trị thời gian
void adjustValue(int increment);

// Hàm bật/tắt sợi đốt/quạt
void controlTemperature(float temperature);

// Hàm kiểm tra báo động
void checkHatchTime(DateTime now);

void setup()
{
  Serial.begin(115200);
  
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_MENU, INPUT_PULLUP);
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(RELAY_HEATER, OUTPUT);
  pinMode(RELAY_FAN, OUTPUT);

  Wire.begin();
  rtc.begin();
  dht.begin();

  lcd.init();
  lcd.backlight();

  displayMenu();
}

void loop()
{
  if (isRunning)
  {
    runIncubator();
  }
  else
  {
    handleMenu();
  }
}

void handleMenu()
{
  if (digitalRead(BUTTON_MENU) == LOW)
  {
    currentState = static_cast<MenuState>((currentState + 1) % 4);
    DateTime_index = 0;
    delay(300);
    if (currentState == RUNNING)
    {
      isRunning = true;
    }
    else
    {
      displayMenu();
    }
  }

  
  if (digitalRead(BUTTON_UP) == LOW)
  {
    adjustValue(1);
    delay(300);
  }

  if (digitalRead(BUTTON_DOWN) == LOW)
  {
    adjustValue(-1);
    delay(300);
  }

  if (digitalRead(BUTTON_LEFT) == LOW)
  {
    DateTime_index = (DateTime_index - 1 + 4) % 4;
    displayMenu();
    delay(300);
  }

  if (digitalRead(BUTTON_RIGHT) == LOW)
  {
    DateTime_index = (DateTime_index + 1) % 4;
    displayMenu();
    delay(300);
  }
}

void adjustValue(int increment)
{
  switch (currentState)
  {
  case SET_START_TIME:
    startTime[DateTime_index] += increment;
    if (DateTime_index == 0)
    {
      if (startTime[DateTime_index] > 31)
        startTime[DateTime_index] = 1;
      if (startTime[DateTime_index] < 1)
        startTime[DateTime_index] = 31;
    }
    else if (DateTime_index == 1)
    {
      if (startTime[DateTime_index] > 12)
        startTime[DateTime_index] = 1;
      if (startTime[DateTime_index] < 1)
        startTime[DateTime_index] = 12;
    }
    else if (DateTime_index == 3)
    {
      if (startTime[DateTime_index] > 24)
        startTime[DateTime_index] = 0;
      if (startTime[DateTime_index] < 0)
        startTime[DateTime_index] = 24;
    }
    displayMenu();
    break;
  case SET_HATCH_TIME:
    hatchTime[DateTime_index] += increment;
    if (DateTime_index == 0)
    {
      if (hatchTime[DateTime_index] > 31)
        hatchTime[DateTime_index] = 1;
      if (hatchTime[DateTime_index] < 1)
        hatchTime[DateTime_index] = 31;
    }
    else if (DateTime_index == 1)
    {
      if (hatchTime[DateTime_index] > 12)
        hatchTime[DateTime_index] = 1;
      if (hatchTime[DateTime_index] < 1)
        hatchTime[DateTime_index] = 12;
    }
    else if (DateTime_index == 3)
    {
      if (hatchTime[DateTime_index] > 24)
        hatchTime[DateTime_index] = 0;
      if (hatchTime[DateTime_index] < 0)
        hatchTime[DateTime_index] = 24;
    }
    displayMenu();
    break;
  case SET_TEMPERATURE:
    setTemperature += increment * 0.1;
    if (setTemperature < 0)
      setTemperature = 0;
    displayMenu();
    break;
  default:
    break;
  }
}

void displayMenu()
{
  lcd.clear();
  switch (currentState)
  {
  case SET_START_TIME:
    lcd.setCursor(0, 0);
    lcd.print("Start Time:");
    lcd.setCursor(0, 1);
    lcd.printf("%02d/%02d/%04d : %02d", startTime[0], startTime[1], startTime[2], startTime[3]);
    break;
  case SET_HATCH_TIME:
    lcd.setCursor(0, 0);
    lcd.print("Hatch Time:");
    lcd.setCursor(0, 1);
    lcd.printf("%02d/%02d/%04d : %02d", hatchTime[0], hatchTime[1], hatchTime[2], hatchTime[3]);
    break;
  case SET_TEMPERATURE:
    lcd.setCursor(0, 0);
    lcd.print("Temperature:");
    lcd.setCursor(0, 1);
    lcd.printf("%.1f C", setTemperature);
    break;
  default:
    break;
  }
}

void runIncubator()
{
  DateTime now = rtc.now();
  float temperature = dht.readTemperature();

  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  int thoi_gian_da_ap = (now.hour() * 60 + now.minute()) - (startTime[3] * 60); 
  lcd.print("E: ");
  lcd.print(thoi_gian_da_ap / 60);
  lcd.print("h");

  int thoi_gian_con_lai = (hatchTime[3] * 60) - (now.hour() * 60 + now.minute()); 
  lcd.setCursor(9, 1);
  lcd.print("R: ");
  lcd.print(thoi_gian_con_lai / 60);
  lcd.print("h");

  controlTemperature(temperature);
  checkHatchTime(now);
}

void controlTemperature(float temperature)
{
  if (temperature < setTemperature)
  {
    digitalWrite(RELAY_HEATER, HIGH);
    digitalWrite(RELAY_FAN, LOW);
  }
  else if (temperature > setTemperature)
  {
    digitalWrite(RELAY_HEATER, LOW);
    digitalWrite(RELAY_FAN, HIGH);
  }
  else
  {
    digitalWrite(RELAY_HEATER, LOW);
    digitalWrite(RELAY_FAN, LOW);
  }
}

void checkHatchTime(DateTime now)
{
  int currentTimeInMinutes = now.hour() * 60 + now.minute();
  if (currentTimeInMinutes >= hatchTime[3] * 60 && !buzzer_state)
  {
    buzzer_state = true;
    lastBuzzerTime = millis();
  }

  if (buzzer_state)
  {
    if ((millis() - lastBuzzerTime) >= 300000)
    {
      lastBuzzerTime = millis();
      tone(BUZZER_PIN, 1000);
      delay(10000);
      noTone(BUZZER_PIN);
    }

    if (digitalRead(BUTTON_MENU) == LOW)
    {
      buzzer_state = false;
      noTone(BUZZER_PIN);
    }
  }
}
