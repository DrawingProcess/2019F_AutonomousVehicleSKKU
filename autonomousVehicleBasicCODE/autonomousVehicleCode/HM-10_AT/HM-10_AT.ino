#include <SoftwareSerial.h>
SoftwareSerial BTSerial(2, 4);  // 2, 4번 핀에 꽂혀 있는 점프선은 제거해 주세요

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);
}

void loop() {
  if (BTSerial.available()) {
    Serial.write(BTSerial.read());
  }
  if (Serial.available()){
    BTSerial.write(Serial.read());
  }
}

/*
    - 시리얼모니터 설정 : 
        Both NL & CR 선택
        9600 보드레이트 선택

    - AT+VERSION
    - AT+NAME
    - AT+NAME이름
    
 */
