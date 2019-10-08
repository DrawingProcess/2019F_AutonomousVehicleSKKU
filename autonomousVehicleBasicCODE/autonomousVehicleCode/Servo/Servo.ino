#include <Servo.h>
Servo servo;

#define SERVO1_PIN 9      // 서보모터1 연결핀

void setup() {
    Serial.begin(9600);
    
    //서보모터 초기화
    servo.attach(SERVO1_PIN);

    delay(100);

    servo.write(60);        // 왼쪽 이동(정상동작 확인용 코드)
    delay(500);
    servo.write(120);       // 오른쪽 이동(정상동작 확인용 코드)
    delay(500);
    servo.write(90);        // 가운데 90도 설정
    
    delay(100);

    servo.detach();
}

void loop() {
}
