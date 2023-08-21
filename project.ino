#include <Wire.h> // Wire 라이브러리 추가
#include <LiquidCrystal_I2C.h> // LCD 라이브러리 추가
#include <DHT.h> // DHT 라이브러리 추가
#include <Adafruit_NeoPixel.h>  // NeoPixel 라이브러리 추가
#include <SoftwareSerial.h> // SoftwareSerial 라이브러리 추가

#define DHTPIN A0 // DHT핀 설정
#define DHTTYPE DHT11 //DHT 타입 설정

#define LED_PIN 3 // 네오픽셀 D핀과 연결한 아두이노 핀 번호
#define LED_COUNT 12 // 네오픽셀 LED 개수
#define BRIGHTNESS 255 // 네오픽셀 LED 밝기(0 ~ 255)

#define TRIG 9 //TRIG 핀 설정 (초음파 보내는 핀)
#define ECHO 8 //ECHO 핀 설정 (초음파 받는 핀)

int color_r = 0; // RED
int color_g = 0; // GREEN
int color_b = 0; // BLUE
int sumC = 0; // 밝기 합
bool led = false; // led on/off

int L_motor_L = 5; // 모터드라이브보드 왼쪽 핀 설정
int L_motor_R = 6; // 모터드라이브보드 왼쪽 핀 설정
int R_motor_L = 10; // 모터드라이브보드 오른쪽 핀 설정
int R_motor_R = 11; // 모터드라이브보드 오른쪽 핀 설정

bool water = false; // 워터펌프 on/off
bool check = false; // 워터펌프 작동 확인
bool fan = false; // fan on/off

DHT dht(DHTPIN,DHTTYPE); // DHT 클래스 함수 선언

LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD 객체 선언

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800); //네오픽셀 함수 선언

SoftwareSerial BtSerial(12,13); // 블루투스 모듈 핀 선언

void setup()
{
  Serial.begin(9600); // 시리얼 통신 선언
  BtSerial.begin(9600); // 블루투스 통신 선언
  
  lcd.init(); // LCD화면 초기화
  lcd.backlight(); // LCD 백라이트

  dht.begin(); // DHT 사용시작

  pinMode(L_motor_L, OUTPUT); // 핀 모드 OUTPUT 설정
  pinMode(L_motor_R, OUTPUT); // 핀 모드 OUTPUT 설정
  pinMode(R_motor_L, OUTPUT); // 핀 모드 OUTPUT 설정
  pinMode(R_motor_R, OUTPUT); // 핀 모드 OUTPUT 설정

  pinMode(TRIG, OUTPUT);  // 핀 모드 OUTPUT 설정
  pinMode(ECHO, INPUT); // 핀 모드 INPUT 설정

  strip.begin(); // 네오픽셀 초기화
  strip.setBrightness(BRIGHTNESS); // 네오픽셀 밝기 설정
  strip.show(); // 네오픽셀 출력
}

void loop()
{
  int temp = dht.readTemperature(); // temp 변수에 DHT 온도값 저장
  int hum = dht.readHumidity(); // hum 변수에 DHT 습도값 저장
  int hum2 = map(analogRead(A1), 0, 1023, 0, 100); // hum2 변수에 토양수분값 0~100으로 매핑 후 저장

  long duration, distance; // duration, distance 변수 선언

  lcd.clear(); // 모든 글자 지우기
  lcd.setCursor(0, 0); // 커서 지정
  lcd.print("Smart Garden"); // 문자 출력

  digitalWrite(TRIG, LOW); // TRIG핀에 LOW값 설정
  delayMicroseconds(2); // microseconds 딜레이 2
  digitalWrite(TRIG, HIGH); // TRIG핀에 HIGH값 설정
  delayMicroseconds(10); // microseconds 딜레이 10
  digitalWrite(TRIG, LOW); // TRIG핀에 LOW값 설정
  duration = pulseIn (ECHO, HIGH); // duration 변수에 초음파센서 값 저장
  distance = duration * 17 / 1000; // distance 변수에 duration * 17 / 1000 값 저장(거리값을 센티미터로 변환)

  if(distance >= 17) { // distance가 17보다 크거나 같다면
    lcd.clear(); // 모든 글자 지우기
    lcd.setCursor(0, 0); // 커서 지정(0, 0)
    lcd.print("Warning!"); // 문자 출력
    lcd.setCursor(0, 1); // 커서 지정(0, 1)
    lcd.print("Garden is open"); // 문자 출력

    tone(2, 200); // 부저 출력핀, 진동수 설정
  }

  else if(temp > 30 && hum >= 85) { // temp가 30보다 크고, hum이 85보다 크거나 같다면
    lcd.clear(); // 모든 글자 지우기
    lcd.setCursor(0, 0); // 커서 지정(0, 0)
    lcd.print("Too High!"); // 문자 출력
    lcd.setCursor(0, 1); // 커서 지정(0, 1)
    lcd.print("T:"); // 문자 출력
    lcd.print(temp); // 변수 출력
    lcd.print("C  "); // 문자 출력
    lcd.print("H:"); // 문자 출력
    lcd.print(hum); // 변수 출력
    lcd.print("%"); // 문자 출력

    analogWrite(R_motor_L, 0); // R_motor_L핀에 0값 설정
    analogWrite(R_motor_R, 255); // R_motor_R핀에 255값 설정
    noTone(2); // 부저 종료
  }

  else if(temp > 30) { // temp가 30보다 크다면
    lcd.clear(); // 모든 글자 지우기
    lcd.setCursor(0, 0); // 커서 지정(0, 0)
    lcd.print("Warning!"); // 문자 출력
    lcd.setCursor(0, 1); // 커서 지정(0, 1)
    lcd.print("Too Hot "); // 문자 출력
    lcd.print(temp); // 변수 출력
    lcd.print("C"); // 문자 출력

    analogWrite(R_motor_L, 0); // R_motor_L핀에 0값 설정
    analogWrite(R_motor_R, 255); // R_motor_R핀에 255값 설정
    noTone(2); // 부저 종료
  }

  else if(temp < 25) { // temp가 25보다 작다면
    lcd.clear(); // 모든 글자 지우기
    lcd.setCursor(0, 0); // 커서 지정(0, 0)
    lcd.print("Warning!"); // 문자 출력
    lcd.setCursor(0, 1); // 커서 지정(0, 1)
    lcd.print("Too Cool "); // 문자 출력
    lcd.print(temp); // 변수 출력
    lcd.print("C"); // 문자 출력
    noTone(2); // 부저 종료
  }

  else if(hum >= 85) { // hum이 85보다 크거나 같다면
    lcd.clear(); // 모든 글자 지우기
    lcd.setCursor(0, 0); // 커서 지정(0, 0)
    lcd.print("Warning! (H)"); // 문자 출력
    lcd.setCursor(0, 1); // 커서 지정(0, 1)
    lcd.print("Too High "); // 문자 출력
    lcd.print(hum); // 변수 출력
    lcd.print("%"); // 문자 출력

    color_r = 255; // color_r 변수에 255값 저장
    color_g = 255; // color_g 변수에 255값 저장
    color_b = 255; // color_b 변수에 255값 저장

    analogWrite(R_motor_L, 0); // R_motor_L핀에 0값 설정
    analogWrite(R_motor_R, 255); // R_motor_R핀에 255값 설정
    noTone(2); // 부저 종료
  }

  else if(hum < 20) { // hum이 20보다 작다면
    lcd.clear(); // 모든 글자 지우기
    lcd.setCursor(0, 0); // 커서 지정(0, 0)
    lcd.print("Warning! (H)"); // 문자 출력
    lcd.setCursor(0, 1); // 커서 지정(0, 1)
    lcd.print("Too Low "); // 문자 출력
    lcd.print(hum); // 변수 출력
    lcd.print("%"); // 문자 출력
    noTone(2); // 부저 종료
  }

  else if(hum2 > 80) { // hum2가 80보다 크다면
    lcd.clear(); // 모든 글자 지우기
    lcd.setCursor(0, 0); // 커서 지정(0, 0)
    lcd.print("Warning! (SM)"); // 문자 출력
    lcd.setCursor(0, 1); // 커서 지정(0, 1)
    lcd.print("Too High "); // 문자 출력
    lcd.print(hum2); // 변수 출력
    lcd.print("%"); // 문자 출력
    noTone(2); // 부저 종료
  }

  else if(hum2 < 40) { // hum2가 40보다 작다면
    lcd.clear(); // 모든 글자 지우기
    lcd.setCursor(0, 0); // 커서 지정(0, 0)
    lcd.print("Warning! (SM)"); // 문자 출력
    lcd.setCursor(0, 1); // 커서 지정(0, 1)
    lcd.print("Water, please "); // 문자 출력
    lcd.print(hum2); // 변수 출력
    lcd.print("%"); // 문자 출력
    water = true;
    noTone(2); // 부저 종료
  }

  else { // 위 경우에 해당하지 않는다면
    lcd.setCursor(0, 1); // 커서 지정(0, 1)
    lcd.print("T:"); // 문자 출력
    lcd.print(temp); // 문자 출력
    lcd.print(" H:"); // 문자 출력
    lcd.print(hum); // 변수 출력
    lcd.print(" SM:"); // 문자 출력
    lcd.print(hum2); // 변수 출력

    if(!fan) { // fan이 false라면
      analogWrite(R_motor_R, 0); // R_motor_R핀에 0값 설정
      analogWrite(R_motor_L, 0); // R_motor_L핀에 0값 설정
    }

    water = false; // water 변수에 false값 저장
    check = true; // check 변수에 true값 저장
    noTone(2); // 부저 종료

    if(!led) { // led가 false이면
      color_r = 0; // color_r 변수에 0값 저장
      color_g = 0; // color_g 변수에 0값 저장
      color_b = 0; // color_b 변수에 0값 저장
    }
  }

  if(water && check) { // water와 check가 true라면
      analogWrite(L_motor_L, 0); // L_motor_L핀에 0값 설정
      analogWrite(L_motor_R, 255); // L_motor_R핀에 255값 설정
      delay(5000); // 딜레이 5000(5초)
      analogWrite(L_motor_L, 0); // L_motor_L핀에 0값 설정
      analogWrite(L_motor_R, 0); // L_motor_R핀에 0값 설정

      check = false; // check 변수에 false값 저장
  }

  if(BtSerial.available() > 2) { // BTSerial에 입력된 값이 2byte 보다 크다면
    color_r = BtSerial.read(); // RED
    color_g = BtSerial.read(); // GREEN
    color_b = BtSerial.read(); // BLUE
    sumC = color_r + color_g + color_b; // 모든 색 값의 합
    if(sumC) // sumC값이 0 이상이면
      led = true; // led 변수 값 true
    else // sumC값이 0 이상이 아니면
      led = false; // led 변수 값 false
  }

  else { // BTSerial에 입력된 값이 2byte 초과가 아니면
    byte app = BtSerial.read(); // app 변수에 블루투스 수신 값 저장
    if(app == 'A') { // app이 A라면
      BtSerial.print(String(hum2)); // hum2값을 블루투스 모듈에서 전송
    }
    else if(app == 'B') { // app이 B라면
      BtSerial.print(String(temp)); // temp값을 블루투스 모듈에서 전송
      BtSerial.print(","); // ,를 블루투스 모듈에서 전송
      BtSerial.print(String(hum)); // hum값을 블루투스 모듈에서 전송
    }
    else if(app == 'O') { // app이 O라면
      analogWrite(R_motor_L, 0); // R_motor_L핀에 0값 설정
      analogWrite(R_motor_R, 255); // R_motor_R핀에 255값 설정
      fan = true; // fan 변수에 true값 저장
    }
    else if(app == 'F') { // app이 F라면
      analogWrite(R_motor_L, 0); // R_motor_L핀에 0값 설정
      analogWrite(R_motor_R, 0); // R_motor_R핀에 0값 설정
      fan = false; // fan 변수에 false값 저장
    }
    else if(app == 'W') { // app이 W라면
      analogWrite(L_motor_L, 0); // L_motor_L핀에 0값 설정
      analogWrite(L_motor_R, 255); // L_motor_R핀에 255값 설정
    }
    else if(app == 'S') { // app이 S라면
      analogWrite(L_motor_L, 0); // L_motor_L핀에 0값 설정
      analogWrite(L_motor_R, 0); // L_motor_R핀에 0값 설정
    }
  }

  for (int i = 0; i < LED_COUNT; i++) { // LED 수 만큼 반복
    strip.setPixelColor(i, color_r, color_g, color_b, 0); // 네오픽셀 색 설정
  }
  strip.show(); // 네오픽셀 출력

  delay(500); // 딜레이 500(0.5초)
}