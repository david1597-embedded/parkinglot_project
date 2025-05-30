STM32 내부 핀 설정 값들 (STM32-F411RE)

PC0 ~ PC4 (주차공간 점유 표시 LED) -> 출력\
PA0 PA1 PA4 PA6 PA7 PA7 PB1 (차례대로 AC1 CH0 CH1 CH4 CH6 CH7 CH ) -> ADC 측정\
PC6 PC7 -> 블루투스모듈(HC-05)와 연결 USART6 Rx Tx 연결 (라즈베리파이와 통신)\
PA9 PA10 -> 블루투스모듈(HC-05)와 연결 USART1 Rx Tx 연결 (안드로이드 어플리케이션과 통신)\
PB8 PB9 -> I2C LCD와 연결할 I2C 핀\
TIM1 -> 온습도 센서 DHT11 delay용\
TIM2 -> DC 모터 드라이버에 연결할 PWM 채널 생성용\
TIM3 -> HC-SR04 CH1사용하여 초음파센서 에코 측정 (Input Capture Interrupt)\
TIM4 -> 주차장 입구 모터 동작시간 측정용 (Elapsed Callback) (추가 적으로 CH1 PWM 사용하여 LED 점멸 기능 추가 고려)\
TIM9 -> LCD 모듈 delay 용\
TIM10 -> 스테퍼 모터 스텝 업데이트 용


시스템 클럭 소스 HSE - >100MHZ로 변경 분주비 100-1 변경하면 카운터 주기 1us로 맞출 수 있음



안드로이드 스튜디오에서 어플리케이션 개발( 로그인기능, 데이터베이스 연동해 데이터 출력기능, 웹 라이브 스트리밍 서버 구축 및 송출 기능)
