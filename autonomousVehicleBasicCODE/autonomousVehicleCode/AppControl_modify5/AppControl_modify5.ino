#include <Servo.h>
#include "Pitches.h"

// debug 관련 설정
//#define DEBUG               // DEBUG모드 해제시에는 이 줄을 주석처리, 시리얼 모니터는 "새 줄" 선택
unsigned long debug_time;
boolean debug_driving_status = false;

// Pin map
#define LIGHT    13  // Light 제어 핀
#define LED      13  // LED 제어 핀
#define M_PWM    5   // DC모터 PWM 핀
#define M_DIR1   7   // DC모터 DIR1 핀
#define M_DIR2   8   // DC모터 DIR2 핀

#define M2_PWM    6   // DC모터2 PWM 핀
#define M2_DIR1   11   // DC모터2 DIR1 핀
#define M2_DIR2   12   // DC모터2 DIR2 핀

#define SERVO    9   // 서보모터 핀
#define BUZZER   3   // 버저 핀
#define BATTERY  A0  // 배터리 체크 핀

#define FC_TRIG 13   // 전방 초음파 센서 TRIG 핀
#define FC_ECHO 10  // 전방 초음파 센서 ECHO 핀
#define FL_TRIG A4  // 전방좌측 초음파 센서 TRIG 핀
#define FL_ECHO A3  // 전방좌측 초음파 센서 ECHO 핀
#define FR_TRIG 3   // 전방우측 초음파 센서 TRIG 핀
#define FR_ECHO 4   // 전방우측 초음파 센서 ECHO 핀
#define L_TRIG  A2  // 좌측 초음파 센서 TRIG 핀
#define L_ECHO  A1  // 좌측 초음파 센서 ECHO 핀
#define R_TRIG  2   // 우측 초음파 센서 TRIG 핀
#define R_ECHO  A5  // 우측 초음파 센서 ECHO 핀

#define MAX_DISTANCE  2000 // 초음파 센서의 최대 감지거리

// 자동차 튜닝 파라미터
int servo_dir = 1; // 서보 회전 방향(동일: 1, 반대:-1)
int motor_dir = 1; // 모터 회전 방향(동일:1, 반대:-1)
int angle_limit = 60; // 서보 모터 회전 제한 각 (단위: 도)
int angle_offset = 0; // 서보 모터 중앙각 오프셋 (단위: 도)
int max_rc_pwm = 255; // RC조종 모터 최대 출력 (0 ~ 255)
int min_rc_pwm = 110; // RC조종 모터 최소 출력 (0 ~ 255)
int punch_pwm = 200; // 정지 마찰력 극복 출력 (0 ~ 255)
int punch_time = 40; // 정지 마찰력 극복 시간 (단위 msec)
int stop_time = 30; // 전진후진 전환 시간 (단위 msec)
int melody_tempo = 3500; // 멜로디 연주 속도
int melody_num = 41; // 멜로디 음 개수
int battery_cell = 2; // 배터리 셀 개수
float voltage_error = 1.08; // 전압 오차 (1이 오차 없음)
// 자율주행 튜닝 파라미터
//int max_ai_pwm = 220; // 자율주행 모터 최대 출력 (0 ~ 255)
int max_ai_pwm = 220; // 자율주행 모터 최대 출력 (0 ~ 255)
int min_ai_pwm = 80; // 자율주행 모터 최소 출력 (0 ~ 255)
int center_detect = 160; // 전방 감지 거리 (단위: mm)
int center_start = 110; // 전방 출발 거리 (단위: mm)
//int center_stop = 65; // 전방 멈춤 거리 (단위: mm)
int center_stop = 65; // 전방 멈춤 거리 (단위: mm)
int diagonal_detect = 80; // 대각 감지 거리 (단위: mm)
int diagonal_start = 95; // 대각 출발 거리 (단위: mm)
int diagonal_stop = 65; // 대각 멈춤 거리 (단위: mm)
int side_detect = 250; // 좌우 감지 거리 (단위: mm)
int side_start = 160; // 좌우 조향 시작 거리 (단위: mm)
int side_stop = 50; // 좌우 조향 끝 거리 (단위: mm)
float steering_gain = 1.5; // 좌우 조향 증폭상수


// 멜로디 음계
int melody[] = {
    NOTE_E5, NOTE_DS5, NOTE_E5, NOTE_DS5, NOTE_E5, NOTE_B4, NOTE_D5, NOTE_C5, NOTE_A4, 0,
    NOTE_C4, NOTE_E4, NOTE_A4, NOTE_B4, 0, NOTE_E4, NOTE_GS4, NOTE_B4, NOTE_C5, 0,
    NOTE_E4, NOTE_E5, NOTE_DS5, NOTE_E5, NOTE_DS5, NOTE_E5, NOTE_B4, NOTE_D5, NOTE_C5, NOTE_A4, 0,
    NOTE_C4, NOTE_E4, NOTE_A4, NOTE_B4, 0, NOTE_E4, NOTE_C5, NOTE_B4, NOTE_A4, 0
};
// 멜로디 음 빠르기
int duration[] = {
    16, 16, 16, 16, 16, 16, 16, 16, 8, 16,
    16, 16, 16, 8, 16, 16, 16, 16, 8, 16,
    16, 16, 16, 16, 16, 16, 16, 16, 16, 8, 16,
    16, 16, 16, 8, 16, 16, 16, 16, 4, 16
};

Servo servo;
float cur_steering;
float cur_speed;
float max_pwm;
float min_pwm;
bool sound = false;
bool autoDriving = false;
int melody_index = 0;
unsigned long melody_time;
unsigned long battery_time;
unsigned long rc_time;
float f_center;
float f_left;
float f_right;
float left;
float right;
float b_center;

// 이전 초음파 거리 값, 에러 처리시 사용
//float prev_f_center = 0;
//float prev_f_left = 0;
//float prev_f_right = 0;
//float prev_left = 0;
//float prev_right = 0;


// 앞바퀴 조향
void SetSteering(float steering)
{
    cur_steering = constrain(steering, -1, 1);
    
    float angle = cur_steering * angle_limit;
    if(servo_dir < 0)
        angle = -angle;
    
    int servoAngle = angle + 90;
    if(servo_dir < 0)
        servoAngle -= angle_offset;
    else
        servoAngle += angle_offset;
    servoAngle = constrain(servoAngle, 0, 180);
    
    servo.write(servoAngle);
}

// 뒷바퀴 모터회전
void SetSpeed(float speed)
{
    
    speed = constrain(speed, -0.7, 0.7);

    if((cur_speed * speed < 0) // 움직이는 중 반대 방향 명령이거나
        || (cur_speed != 0 && speed == 0)) // 움직이다가 정지라면
    {
        cur_speed = 0;
        digitalWrite(M_PWM, HIGH);
        digitalWrite(M_DIR1, LOW);
        digitalWrite(M_DIR2, LOW);

        digitalWrite(M2_PWM, HIGH);
        digitalWrite(M2_DIR1, LOW);
        digitalWrite(M2_DIR2, LOW);

        
        if(stop_time > 0) 
          delay(stop_time);
    }

    if(cur_speed == 0 && speed != 0) // 정지상태에서 출발이라면
    {
        if(punch_time > 0)
        {
            if(speed * motor_dir > 0)
            {
                analogWrite(M_PWM, punch_pwm);
                digitalWrite(M_DIR1, HIGH);
                digitalWrite(M_DIR2, LOW);

                analogWrite(M2_PWM, punch_pwm);
                digitalWrite(M2_DIR1, HIGH);
                digitalWrite(M2_DIR2, LOW);
            }
            else if(speed * motor_dir < 0)
            {
                analogWrite(M_PWM, punch_pwm);
                digitalWrite(M_DIR1, LOW);
                digitalWrite(M_DIR2, HIGH);

                analogWrite(M2_PWM, punch_pwm);
                digitalWrite(M2_DIR1, LOW);
                digitalWrite(M2_DIR2, HIGH);
            }
            
            delay(punch_time);
        }
    }

    if(speed != 0) // 명령이 정지가 아니라면
    {
        int pwm = abs(speed) * (max_pwm - min_pwm) + min_pwm;           // 0 ~ 255로 변환
        if(speed * motor_dir > 0)
        {
            analogWrite(M_PWM, pwm);
            digitalWrite(M_DIR1, HIGH);
            digitalWrite(M_DIR2, LOW);

            analogWrite(M2_PWM, pwm);
            digitalWrite(M2_DIR1, HIGH);
            digitalWrite(M2_DIR2, LOW);
        }
        else if(speed * motor_dir < 0)
        {
            analogWrite(M_PWM, pwm);
            digitalWrite(M_DIR1, LOW);
            digitalWrite(M_DIR2, HIGH);

            analogWrite(M2_PWM, pwm);
            digitalWrite(M2_DIR1, LOW);
            digitalWrite(M2_DIR2, HIGH);
        }
    }

    cur_speed = speed;
}

// 라이트 켜기
void LightON()
{
    digitalWrite(LIGHT, HIGH);
}

// 라이트 끄기
void LightOFF()
{
    digitalWrite(LIGHT, LOW);
}

// 멜로디 시작
void StartMelody()
{
    sound = true;
    melody_index = 0;
    melody_time = millis();
    tone(BUZZER, melody[melody_index]);
}

// 멜로디 종료
void StopMelody()
{
    sound = false;
    noTone(BUZZER);
}

// 멜로디
void PlayMelody()
{
    if(sound == false)
        return;
    
    unsigned long t = millis();
    if(t - melody_time >= (melody_tempo / duration[melody_index]))
    {
        melody_index++;
        if(melody_index >= melody_num)
            melody_index = 0;
        
        if(melody[melody_index] == 0)
            noTone(BUZZER);
        else
            tone(BUZZER, melody[melody_index]);
        melody_time = t;
    }
}

// 배터리 체크, 저전압시 경고음
void CheckBattery()
{
    
    // 1초에 한번씩 체크
    if(millis() - battery_time < 1000)
        return;
  
    float voltage = (float)analogRead(BATTERY) / 1023;      
    voltage *= 5;                                           // 아두이노 5V
    voltage *= 3;                                           // 회로상 전압분배 비율 (VIN을 10K, 20K 저항으로 분배)
    voltage *= voltage_error;

    if(voltage < (2.8 * battery_cell))
    {
//        while(true)
//        {
//            tone(BUZZER, NOTE_C5);
//            digitalWrite(LED, HIGH);
//            delay(1000);
//            
//            noTone(BUZZER);
//            digitalWrite(LED, LOW);
//            delay(1000);      
//        }
    }
    else
    {
        #ifndef DEBUG
        Serial.print("B");
        Serial.print(battery_cell);
        Serial.print(":");
        Serial.println(voltage);
        #endif
    }
    
    battery_time = millis();
    rc_time = millis();
        
}

// 초음파 거리측정
float GetDistance(int trig, int echo)
{ 
    digitalWrite(trig, LOW);
    delayMicroseconds(4);
    digitalWrite(trig, HIGH);
    delayMicroseconds(20);
    digitalWrite(trig, LOW);
    
    unsigned long duration = pulseIn(echo, HIGH, 5000);
    if(duration == 0)
        return MAX_DISTANCE;
    else
        return duration * 0.17;     // 음속 340m/s
}

// 자율주행 시작
void StartAutoDriving()
{
    autoDriving = true;
    max_pwm = max_ai_pwm;
    min_pwm = min_ai_pwm;
    LightOFF();
    StopMelody();
    
    SetSteering(0);
    SetSpeed(0);
}

// 자율주행 종료
void StopAutoDriving()
{
    autoDriving = false;
    max_pwm = max_rc_pwm;
    min_pwm = min_rc_pwm;
    SetSteering(0);
    SetSpeed(0);
}

// 자율주행
void AutoDriving()
{
    if(!autoDriving)
        return;

    f_center = 0;
    f_left = 0;
    f_right = 0;
    left = 0;
    right = 0;
    // 판단
    // 판단 근거가 없다면 이전 상태와 동일하게 수행
    float compute_speed = cur_speed;
    float compute_steering = cur_steering;
    
    if(cur_speed == 0) // 현재 정지 중인 상태이면
    {
        f_center = GetDistance(FC_TRIG, FC_ECHO);
        
        if(f_center > center_start) // 전방에 감지되는 것이 충분히 멀다면
        {
            // 출발한다
            compute_speed = 0.1;
            compute_steering = 0;
        }
    }
    else if(cur_speed > 0) // 현재 전진 중인 상태이면
    {
        f_center = GetDistance(FC_TRIG, FC_ECHO);
        f_left = GetDistance(FL_TRIG, FL_ECHO);
        f_right = GetDistance(FR_TRIG, FR_ECHO);
        if(f_center <= center_stop || f_left <= diagonal_stop || f_right <= diagonal_stop)  // 전방에 감지되면, diagonal_stop = 60
        {

          #ifdef DEBUG
            // 한번씩 체크        
//            if(millis() - debug_time < 500)
//                return;

            //debug_driving_status = false;       // 일시 정지 모드
            
            Serial.println("R");

            Serial.print("f_center:");Serial.println(f_center);
            Serial.print("center_stop:");Serial.println(center_stop);
            Serial.print("f_left:");Serial.println(f_left);
            Serial.print("diagonal_stop:");Serial.println(diagonal_stop);
            Serial.print("f_right:");Serial.println(f_right);
            Serial.print("diagonal_stop:");Serial.println(diagonal_stop);

            
            Serial.print(f_center);Serial.print("\t");
            Serial.print(f_left);Serial.print("\t");
            Serial.print(f_right);Serial.print("\t");
            Serial.print(left);Serial.print("\t");
            Serial.print(right);Serial.print("\t");
            Serial.print(compute_steering);Serial.print("\t");
            Serial.print(compute_speed);Serial.println();
            //Serial.println("-----------------------------");   
            debug_time = millis(); 
        #endif
        
            // 후진한다
            compute_speed = -0.1;
            // 아래 기존 소스 주석처리
//            if(cur_steering > 0)
//                compute_steering = -1;
//            else
//                compute_steering = 1;

            if (f_left > f_right) 
            {
                compute_steering = 0.8;
            }
            else 
            {
                compute_steering = -0.8;
            }
            
        }
        else if(f_left <= diagonal_detect || f_right <= diagonal_detect) // 좌우측방 어느 곳이라도 감지된다면, diagonal_detect = 75
        {
//            if(f_center > center_detect) // 전방은 감지되지 않는다면
//            {
//              #ifdef DEBUG
//                  Serial.println("FRONT NO DETECT");
//              #endif
//              // 좌우측방 중 한 곳만 감지되었음
//                if(f_left < f_right) // 좌측방이 감지되었다면
//                {
//                    compute_steering = 0.75;
//                }
//                else if(f_right < f_left) // 우측방이 감지되었다면
//                {
//                    compute_steering = -0.75;
//                }
//            }
        }
        else
        {
            // 전방과 좌우 센서만으로 제어한다
            // 속도 결정
            if(f_center <= center_detect) // 전방에 감지된다면
            {
                // 거리에 따른 속도 결정
                compute_speed = (float)(f_center - center_stop) / (float)(center_detect - center_stop);
            }
            else // 전방에 없다면
            {
                // 최고 속도
                compute_speed = 0.7;
            }
        
            // 조향 결정
            left = GetDistance(L_TRIG, L_ECHO);
            right = GetDistance(R_TRIG, R_ECHO);
            if(left <= side_start && right <= side_start) // 좌우 모두 감지되면
            {
                // 거리차에 따라 조향한다
                float diff = (float)(right - side_stop) - (float)(left - side_stop);
                diff /= (float)(side_start - side_stop);
                // 결과에 Gain을 적용하여 반응성을 높일 수 있다
                compute_steering = diff * steering_gain * 0.5;
            }
            else if(f_center <= center_detect && (left > side_detect || right > side_detect))
            // 전방은 감지되는데 어느 한쪽이라도 감지되지 않는다면
            {
                if(left <= side_detect) // 좌측이 감지된다면
                {
                    // 좌측으로 최대 조향
                    compute_steering = 0.75;
                }
                else if(right <= side_detect) // 우측이 감지된다면
                {
                    // 우측으로 최대 조향
                    compute_steering = -0.75;
                }
            }
        }
}
    else
    {
        // 현재 후진 중인 상태이면
        f_center = GetDistance(FC_TRIG, FC_ECHO);
        f_right = GetDistance(FR_TRIG, FR_ECHO);
        f_left = GetDistance(FL_TRIG, FL_ECHO);
        if(f_center > center_start && f_left > diagonal_start && f_right > diagonal_start) // 전방에 감지되지 않으면
        {
            // 조향을 반대로 꺾고 회피 종료
            compute_speed = 0.1;
            if(cur_steering > 0) // 우 조향 중이면
                compute_steering = -0.75;
            else // 좌 조향 중이면
             compute_steering = 0.75;
        }
    }


    #ifdef DEBUG
        if (debug_driving_status) {     // 주행 모드
            // 제어
            SetSteering(compute_steering);
            SetSpeed(compute_speed);
        }
        else {                          // 일시정지 모드, 현재 정보만 시리얼 모니터로 보낸다.
            SetSteering(0);
            SetSpeed(0);
    
            if(millis() - debug_time < 3000)
                return;            
            Serial.println("PAUSE");
            // 모든 방향의 초음파 값을 가져온다.
            f_center = GetDistance(FC_TRIG, FC_ECHO);
            f_right = GetDistance(FR_TRIG, FR_ECHO);
            f_left = GetDistance(FL_TRIG, FL_ECHO);
            left = GetDistance(L_TRIG, L_ECHO);
            right = GetDistance(R_TRIG, R_ECHO);
        
            Serial.print(f_center);Serial.print("\t");
            Serial.print(f_left);Serial.print("\t");
            Serial.print(f_right);Serial.print("\t");
            Serial.print(left);Serial.print("\t");
            Serial.print(right);Serial.print("\t");
            Serial.print(compute_steering);Serial.print("\t");
            Serial.print(compute_speed);Serial.println();
            debug_time = millis(); 
        }
    #else
        // 제어
        SetSteering(compute_steering);
        SetSpeed(compute_speed);
    #endif
    

    

//    #ifdef DEBUG
//        // 한번씩 체크        
//        if(millis() - debug_time < 500)
//            return;
//            
//        Serial.print(f_center);Serial.print("\t");
//        Serial.print(f_left);Serial.print("\t");
//        Serial.print(f_right);Serial.print("\t");
//        Serial.print(left);Serial.print("\t");
//        Serial.print(right);Serial.print("\t");
//        Serial.print(compute_steering);Serial.print("\t");
//        Serial.print(compute_speed);Serial.println();
//        //Serial.println("-----------------------------");   
//        debug_time = millis(); 
//    #endif
}

void setup()
{
    max_pwm = max_rc_pwm;
    min_pwm = min_rc_pwm;
    
    servo.attach(SERVO);    
    pinMode(M_PWM, OUTPUT);
    pinMode(M_DIR1, OUTPUT);
    pinMode(M_DIR2, OUTPUT);

    pinMode(M2_PWM, OUTPUT);
    pinMode(M2_DIR1, OUTPUT);
    pinMode(M2_DIR2, OUTPUT);
    
    pinMode(LIGHT, OUTPUT);
    pinMode(LED, OUTPUT);
    pinMode(FC_TRIG, OUTPUT);
    pinMode(FC_ECHO, INPUT);
    pinMode(FL_TRIG, OUTPUT);
    pinMode(FL_ECHO, INPUT);
    pinMode(FR_TRIG, OUTPUT);
    pinMode(FR_ECHO, INPUT);
    //pinMode(BC_TRIG, OUTPUT);
    //pinMode(BC_ECHO, INPUT);
    pinMode(L_TRIG, OUTPUT);
    pinMode(L_ECHO, INPUT);
    pinMode(R_TRIG, OUTPUT);
    pinMode(R_ECHO, INPUT);
    
    digitalWrite(LIGHT, LOW);
    SetSteering(0);
    SetSpeed(0);
    
    //Serial.begin(9600);
    Serial.begin(9600);
    
    battery_time = millis();
    debug_time = millis(); 

    #ifdef DEBUG        
        Serial.print("FC");Serial.print("\t");
        Serial.print("FL");Serial.print("\t");
        Serial.print("FR");Serial.print("\t");
        Serial.print("L");Serial.print("\t");
        Serial.print("R");Serial.print("\t");
        Serial.print("STEERING");Serial.print("\t");
        Serial.print("SPEED");Serial.println();
        //Serial.println("-----------------------------");
    #endif

    StartAutoDriving();
}

void loop()
{    
    CheckBattery();

    /*
        수신데이터 샘플
        T:1
        P:1
        L:1
        S:1
        A:1
     */
    if(Serial.available() > 0)
    {
        String packet = Serial.readStringUntil('\n');
        if(packet != 0)
        {
            int index = packet.indexOf(':');
            if(index >= 0)
            {
                String cmd = packet.substring(0, index);
                String param = packet.substring(index + 1);
                if(cmd.equals("T") && !autoDriving)         // 조향명령
                {
                    SetSteering(param.toFloat());
                    rc_time = millis();
                }
                else if(cmd.equals("P") && !autoDriving)    // 속도명령
                {
                    SetSpeed(param.toFloat());
                    rc_time = millis();
                }
                else if(cmd.equals("L") && !autoDriving)    // 라이트명령
                {
                    if(param.toInt() == 1)
                        LightON();
                    else
                        LightOFF();
                }
                else if(cmd.equals("S") && !autoDriving)    // 사운드명령
                {
                    if(param.toInt() == 1)
                        StartMelody();
                    else
                        StopMelody();
                }
                else if(cmd.equals("A"))                    // 자율주행명령
                {
                    if(param.toInt() == 1)
                        StartAutoDriving();
                    else
                        StopAutoDriving();

                    #ifdef DEBUG
                        if(param.toInt() == 1)
                            debug_driving_status = true;
                        else
                            debug_driving_status = false;
                    #endif
                }
            }

            #ifdef DEBUG
                String cmd = packet;
                if(cmd.equals(" "))         // 디버그 모드시 정지, 시작 토글 명령
                {
                    if (debug_driving_status) 
                        debug_driving_status = false;
                    else
                        debug_driving_status = true;
                }
            #endif
            

        }
    }

    PlayMelody();
    AutoDriving();

    // 직접 컨트롤시 3초 동안 블루투스 신호 없으면 정지
    if(!autoDriving)
    {
        if(millis() - rc_time > 3000)
            SetSpeed(0);
    }
}
