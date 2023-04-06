/*
 * Date     : 230406
 * Author   : Delcoii
 * 
 * 부틀리 차량 모터 구동 제어 (릴레이, 2단3핀 토글스위치 이용)
 * 
 * 토글스위치를 통해 나노 디지털입력
 * HIGH -> 모터 그라운드 쇼트
 * LOW  -> 모터 그라운드 오픈
 * 내부 풀업은 나노도 있다고 하니 0.1uF 캐패시터를 토글스위치쪽에 달자
 * 
 * 다빈이랑 고베규카츠먹고 카페 데이트하면서 만듬
 */


#define RELAY_SIGNAL_OUT    7
#define YELLOW_LED_OUT      8
#define TOGGLE_SWITCH_IN    4


void setup() {
  
  pinMode(RELAY_SIGNAL_OUT, OUTPUT);
  pinMode(YELLOW_LED_OUT, OUTPUT);

  pinMode(TOGGLE_SWITCH_IN, INPUT_PULLUP);
  
}

void loop() {
  
  bool switchState = digitalRead(TOGGLE_SWITCH_IN);

  if (switchState == HIGH)
  {
    digitalWrite(RELAY_SIGNAL_OUT, HIGH);
    digitalWrite(YELLOW_LED_OUT, HIGH);
  }

  else
  {
    digitalWrite(RELAY_SIGNAL_OUT, LOW);
    digitalWrite(YELLOW_LED_OUT, LOW);
  }

}
