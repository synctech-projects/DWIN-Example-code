#include <DHT.h>
#include <SoftwareSerial.h>

// تنظیم سریال مجازی و سنسور
#define RX_PIN D5            // پین RX سریال مجازی (GPIO14)
#define TX_PIN D6            // پین TX سریال مجازی (GPIO12)
#define DHTPIN D2            // پین متصل به دیتای DHT11
#define DHTTYPE DHT11        // نوع سنسور DHT11

DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial virtualSerial(RX_PIN, TX_PIN); // تعریف سریال مجازی

void setup() {
  Serial.begin(9600);            // راه‌اندازی سریال سخت‌افزاری برای Debug
  virtualSerial.begin(115200);     // راه‌اندازی سریال مجازی
  dht.begin();                   // راه‌اندازی سنسور
  Serial.println("Setup completed. Starting virtual serial...");
}

void loop() {
  // خواندن دما از سنسور
  float temp = dht.readTemperature(); // دریافت داده دما به صورت شناور

  if (isnan(temp)) { // بررسی خطا در خواندن داده از سنسور
    Serial.println("Failed to read temperature from DHT11!");
    delay(2000);
    return;
  }

  // تبدیل مقدار دما به مقدار صحیح (بدون اعشار)
  uint8_t header1 = 0x5A;           // بایت اول هدر
  uint8_t header2 = 0xA5;           // بایت دوم هدر
  uint8_t byteCount = 0x05;         // تعداد بایت‌های ارسالی (5 بایت)
  uint8_t operation = 0x82;         // عملیات
  uint8_t memoryAddress1 = 0x10;    // آدرس حافظه بایت اول
  uint8_t memoryAddress2 = 0x00;    // آدرس حافظه بایت دوم
  uint8_t temperature = (uint8_t)temp; // مقدار دما (number)

  // داده فرمت‌شده برای ارسال
  uint8_t data[] = {
    header1, header2, byteCount, operation, memoryAddress1, memoryAddress2, 0x00, temperature
  };

  // ارسال داده‌ها از طریق سریال مجازی
  for (int i = 0; i < sizeof(data); i++) {
    virtualSerial.write(data[i]); // ارسال هر بایت به صورت خام
  }

  // نمایش داده‌ها برای Debug در مانیتور سریال
  Serial.print("Sending on virtual serial: ");
  for (int i = 0; i < sizeof(data); i++) {
    Serial.print("0x");
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  delay(2000); // تاخیر 2 ثانیه برای خواندن دوباره
}
