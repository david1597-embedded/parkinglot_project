# 🚗 주차장 환경 감시 및 관리 시스템

블로그를 통해 자세한 내용을 확인하세요!!

## 🧰기술 스택
![C](https://img.shields.io/badge/Language-C-blue?logo=c)
![Java](https://img.shields.io/badge/Language-Java-red?logo=openjdk)
![Python](https://img.shields.io/badge/Language-Python-yellow?logo=python)
![Flask](https://img.shields.io/badge/Framework-Flask-black?logo=flask)
![MariaDB](https://img.shields.io/badge/Database-MariaDB-blue?logo=mariadb)
![SQL](https://img.shields.io/badge/Query-SQL-lightgrey?logo=mysql)
![STM32F411RE](https://img.shields.io/badge/Board-STM32F411RE-darkgreen?logo=stmicroelectronics)
![Android](https://img.shields.io/badge/Platform-Android-green?logo=android)



## 📌 프로젝트 개요

STM32F411RE 마이크로컨트롤러 기반의 스마트 주차 시스템으로, 초음파 센서를 통해 **주차 공간 점유 상태를 감지**하고, **Bluetooth 통신을 통해 라즈베리파이와 연동**, **라즈베리파이 서버를 통한 실시간 영상 스트리밍**, **데이터베이스와 Flask API를 통한 라즈베리파이와 안드로이프 어플리케이션간 서버 통신**까지 지원해, 실시간으로 주차장의 환경 상태를 볼 수 있는 시스템을 구축함.

---

## ⚙️ 시스템 구성도

- **MCU**: STM32F411RE
- **센서**:
  - HC-SR04 (초음파 거리 측정)
  - DHT11 (온습도 측정)
- **모터**:
  - 스테퍼 모터 (입출차 게이트)
- **통신**:
  - HC-05 블루투스 ( 라즈베리파이 연동)
- **출력장치**:
  - I2C LCD
  - LED
- **서버**:
  - Raspberry Pi 기반 웹 스트리밍 서버
  - 데이터베이스와 안드로이드 어플리케이션 간 연동을 위한 웹 서버 구현
- **앱**:
  - Android Studio 기반 앱 (로그인, 영상)

---

## 📐 STM32 핀맵 요약

| 핀 | 기능 설명 |
|----|-----------|
| `PC0 ~ PC4` | 주차 공간 점유 표시 LED (GPIO Output) |
| `PA0, PA1, PA4, PA6, PA7, PB1` | 아날로그 센서 입력 (ADC) |
| `PC6, PC7` | USART6: HC-05 블루투스 모듈 (라즈베리파이) |
| `PB8, PB9` | I2C LCD 연결 |
| `TIM1` | DHT11 온습도 센서 Delay 용 |
| `TIM2` | DC 모터 PWM 제어용 |
| `TIM3` | 초음파 센서 Echo 캡처 (Input Capture Interrupt) |
| `TIM4` | 입구 모터 타이밍 및 LED PWM 점멸 |
| `TIM9` | LCD 딜레이 |
| `TIM10` | 스테퍼 모터 스텝 제어용 |

> 💡 **시스템 클럭**: HSE 사용, 100MHz 설정 → 분주비 100-1 사용으로 **1us 타이머 주기 확보**

---

## 📱 Android 앱 기능

- 🔐 로그인 기능 구현 (SQL에 회원 정보 저장)
- 📊 주차 상태 실시간 표시
- 📺 웹 스트리밍 영상 출력 (라즈베리파이 연동)
- 🗂️ http를 통한 데이터베이스에 데이터 요청 (로그인/로그아웃, 주차장 환경정보 요청)

---

## 🌐 라즈베리파이 웹 서버 기능

- 📷 실시간 카메라 영상 송출 (Flask + MJPEG)
- 📡 HC-05로 STM32에서 상태 정보 수신
- 🖥️ 안드로이드 어플리케이션에서 주차 공간, 주차장 상태 현황 표시
- 📺 데이터베이스 연동을 위한 flask 웹 API 구현

---

## 🧠 주요 기술 스택

| 카테고리 | 사용 기술 |
|----------|------------|
| Embedded | STM32 HAL, CubeMX, ADC/UART/PWM/Timer |
| Android App | Android Studio, flask, MySQL|
| Web Server | Flask, OpenCV, MJPEG/WebRTC |
| Streaming | Webcam, Flask live streaming server |
| Tools | STM32CubeIDE, VSCode, Linux |

---

## 📷 시연 이미지/영상



---

## 📁 프로젝트 구조

├── Core/
│   ├── Inc/                          # 헤더 파일 디렉토리
│   │   ├── adc.h                     # ADC 초기화 및 읽기 함수 정의
│   │   ├── buzzer.h                 # 부저 제어용 함수
│   │   ├── delay.h                  # Delay 유틸리티 함수
│   │   ├── dht11.h                  # 온습도 센서 (DHT11) 드라이버
│   │   ├── dma.h                    # DMA 설정
│   │   ├── gate.h                   # 게이트 모터 제어
│   │   ├── gpio.h                   # GPIO 초기화
│   │   ├── i2c.h / i2c_lcd.h        # I2C 및 I2C LCD 제어
│   │   ├── main.h                   # 메인 헤더
│   │   ├── tim.h                    # 타이머 설정
│   │   ├── usart.h                  # UART 설정
│   │   ├── ultrasonic.h             # 초음파 센서용 드라이버
│   │   ├── stm32f4xx_hal_conf.h     # HAL 설정 파일
│   │   └── stm32f4xx_it.h           # 인터럽트 핸들러 헤더
│
│   ├── Src/                          # 소스 파일 디렉토리
│   │   ├── adc.c
│   │   ├── buzzer.c
│   │   ├── delay.c
│   │   ├── dht11.c
│   │   ├── dma.c
│   │   ├── gate.c
│   │   ├── gpio.c
│   │   ├── i2c.c / i2c_lcd.c
│   │   ├── main.c                   # 메인 로직 진입점
│   │   ├── tim.c                    # 타이머 핸들링
│   │   ├── usart.c                  # UART 핸들링
│   │   ├── ultrasonic.c            # 초음파 센서 처리
│   │   ├── stm32f4xx_it.c           # 인터럽트 서비스 루틴
│   │   ├── stm32f4xx_hal_msp.c      # HAL MSP 초기화
│   │   ├── system_stm32f4xx.c       # 시스템 클럭 설정
│   │   ├── syscalls.c / sysmem.c    # 시스템 함수 구현 (printf 등)

---

##




