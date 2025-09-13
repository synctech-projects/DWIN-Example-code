#include <DHT.h>                                // اضافه کردن کتابخانه DHT برای سنسور دما و رطوبت
#include <SoftwareSerial.h>                     // اضافه کردن کتابخانه SoftwareSerial برای ساخت پورت سریال مجازی

// --- تعریف پین‌ها ---
#define RX_PIN D3                              // تعریف پایه RX برای سریال مجازی (دریافت داده)
#define TX_PIN D4                              // تعریف پایه TX برای سریال مجازی (ارسال داده)
#define DHTPIN D2                              // تعریف پایه متصل به سنسور DHT11
#define DHTTYPE DHT11                          // تعیین نوع سنسور DHT (اینجا مدل DHT11)

#define LED_D8 D8                              // تعریف پایه LED اول
#define LED_D7 D7                              // تعریف پایه LED دوم
#define LED_D6 D6                              // تعریف پایه LED سوم
#define LED_D5 D5                              // تعریف پایه LED چهارم

DHT dht(DHTPIN, DHTTYPE);                      // مقداردهی اولیه شیء dht برای کار با سنسور دما/رطوبت
SoftwareSerial virtualSerial(RX_PIN, TX_PIN);  // ساخت شیء virtualSerial برای ارتباط سریال مجازی

// --- ارسال دما و پاک‌کردن بافر بعد از ارسال ---
void sendTemperatureWithMillis(unsigned long interval = 1000) {    // تابع برای ارسال دما در هر بازه زمانی معین با پیش فرض ۱ ثانیه
  static unsigned long lastSend = 0;                               // متغیر استاتیک جهت ثبت آخرین زمان ارسال داده
  unsigned long _now = millis();                                   // دریافت تعداد میلی‌ثانیه‌های گذشته از شروع اجرا
  if (_now - lastSend >= interval) {                               // اگر مدت زمان کافی از آخرین ارسال گذشته باشد
    lastSend = _now;                                               // بروزرسانی زمان آخرین ارسال
    float temp = dht.readTemperature();                            // خواندن دمای فعلی از سنسور DHT11
    if (!isnan(temp)) {                                            // اگر مقدار بدست آمده معتبر است (NaN نباشد)
      uint8_t data[] = {                                           // ساخت آرایه داده برای ارسال (فرمت پکت سفارشی شامل هدر و مقدار دما)
        0x5A, 0xA5, 0x05, 0x82, 0x10, 0x00, 0x00, (uint8_t)temp
      };
      virtualSerial.write(data, sizeof(data));                     // ارسال پکت روی پورت سریال مجازی
      Serial.print("Sending on virtual serial: ");                 // چاپ پیام بر روی پورت سریال اصلی جهت دیباگ
      for (int i = 0; i < sizeof(data); i++) {                    // نمایش هگزادسیمال داده‌های ارسال شده
        Serial.print("0x");
        Serial.print(data[i], HEX);
        Serial.print(" ");
      }
      Serial.print("  (Temp=");
      Serial.print(temp);
      Serial.println(")");

      // ------ پاک سازی بافر پس از ارسال ------
      while (virtualSerial.available()) virtualSerial.read();      // خالی کردن بافر خواندنی سریال مجازی برای جلوگیری از اضافه‌بار بافر
    } else {
      Serial.println("Failed to read temperature from DHT11!");   // اگر مقدار سنسور نامعتبر باشد، پیام خطا چاپ می‌شود
    }
  }
}

// --- SETUP ---
void setup() {
  Serial.begin(9600);                             // راه‌اندازی پورت سریال اصلی با نرخ 9600
  virtualSerial.begin(115200);                    // راه‌اندازی سریال مجازی با نرخ 115200
  dht.begin();                                    // مقداردهی اولیه سنسور DHT

  pinMode(LED_D8, OUTPUT);                        // تنظیم پایه LED‌ها به عنوان خروجی
  pinMode(LED_D7, OUTPUT);
  pinMode(LED_D6, OUTPUT);
  pinMode(LED_D5, OUTPUT);

  digitalWrite(LED_D8, LOW);                      // خاموش کردن همه LED‌ها در شروع
  digitalWrite(LED_D7, LOW);
  digitalWrite(LED_D6, LOW);
  digitalWrite(LED_D5, LOW);

  Serial.println("Setup completed. Starting virtual serial..."); // نمایش پیام آماده به کار بودن ماژول
}

// --- LOOP ---
void loop() {
  sendTemperatureWithMillis(1000);                // فراخوانی تابع ارسال دوره‌ای دما (هر ۱ ثانیه)

  // --- دریافت و پردازش ---
  const int PACKET_SIZE = 8;                      // تعداد بایت‌های لازم برای تشکیل یک بسته کامل (پکت)
  if (virtualSerial.available() >= PACKET_SIZE) { // اگر بایت کافی جهت تشکیل پکت جدید در بافر سریال مجازی وجود داشته باشد
    uint8_t buffer[PACKET_SIZE];                  // تعریف آرایه برای ذخیره بسته دریافتی
    for (int i = 0; i < PACKET_SIZE; i++) {       
      buffer[i] = virtualSerial.read();           // خواندن تک‌تک بایت‌ها از بافر سریال مجازی و ذخیره در آرایه
    }

    // بررسی صحت هدر
    if (buffer[0] == 0x5A && buffer[1] == 0xA5) { // بررسی درست بودن هدر بسته (دوتا بایت اول باید این مقدار را داشته باشند)
      uint16_t address = ((uint16_t)buffer[4] << 8) | buffer[5]; // استخراج آدرس مقصد از دو بایت پنجم و ششم بسته
      switch (address) {
        case 0X1001:                               // اگر آدرس 1001 بود
          digitalWrite(LED_D8, !digitalRead(LED_D8)); // وضعیت LED_D8 را تغییر (روشن/خاموش) بده
          Serial.println("Toggled LED D8");
          break;
        case 0X1002:
          digitalWrite(LED_D7, !digitalRead(LED_D7));
          Serial.println("Toggled LED D7");
          break;
        case 0X1003:
          digitalWrite(LED_D6, !digitalRead(LED_D6));
          Serial.println("Toggled LED D6");
          break;
        case 0X1004:
          digitalWrite(LED_D5, !digitalRead(LED_D5));
          Serial.println("Toggled LED D5");
          break;
        default:                                    // اگر آدرس ناشناخته باشد
          Serial.println("Invalid address, buffer cleared.");
          break;
      }
    } else {                                        // اگر هدر بسته صحیح نیست
      Serial.println("Invalid header, buffer cleared.");
    }

    // ------ پاک سازی بافر پس از دریافت و پردازش ------
    while (virtualSerial.available()) virtualSerial.read(); // خالی کردن باقی‌مانده بافر سریال مجازی جهت جلوگیری از اختلالات بسته بعدی
  }
}
