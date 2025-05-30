import serial
import subprocess
import time
from datetime import datetime
import os

# 블루투스 설정
BLUETOOTH_MAC = "00:23:10:01:11:A4"  # HC-05의 블루투스 MAC
RFCOMM_CHANNEL = 1
RFCOMM_DEVICE = "/dev/rfcomm0"

def setup_rfcomm():
    """rfcomm 장치를 설정"""
    try:
        # 기존 rfcomm 해제 (실패해도 계속 진행)
        subprocess.run(["sudo", "rfcomm", "release", RFCOMM_DEVICE], check=False)
        time.sleep(1)
       
        # rfcomm 바인딩
        subprocess.run(
            ["sudo", "rfcomm", "bind", RFCOMM_DEVICE, BLUETOOTH_MAC, str(RFCOMM_CHANNEL)],
            check=True
        )
        print(f"RFCOMM bound to {RFCOMM_DEVICE}")
       
        # 권한 설정
        if os.path.exists(RFCOMM_DEVICE):
            subprocess.run(["sudo", "chmod", "666", RFCOMM_DEVICE], check=True)
            print(f"Permissions set for {RFCOMM_DEVICE}")
            return True
    except subprocess.CalledProcessError as e:
        print(f"Failed to setup RFCOMM: {e}")
        return False

def cleanup_rfcomm():
    """rfcomm 장치 해제"""
    try:
        subprocess.run(["sudo", "rfcomm", "release", RFCOMM_DEVICE], check=False)
        print(f"RFCOMM {RFCOMM_DEVICE} released")
    except subprocess.CalledProcessError as e:
        print(f"Failed to release RFCOMM: {e}")

def main():
    # 시리얼 연결 객체
    ser = None
    connected = False
    reconnect_delay = 5  # 재연결 대기 시간 (초)
    last_data_time = time.time()
    data_timeout = 10  # 데이터 없이 10초 경과시 연결 체크
   
    try:
        # 초기 연결 설정
        if not setup_rfcomm():
            print("Initial RFCOMM setup failed")
            return
       
        while True:
            # 연결이 없거나 끊어졌을 때 연결 시도
            if not connected:
                try:
                    print("Attempting to connect to HC-05...")
                    ser = serial.Serial(RFCOMM_DEVICE, 9600, timeout=1)
                    connected = True
                    print("Connected to HC-05")
                except serial.SerialException as e:
                    print(f"Connection failed: {e}")
                    print(f"Retrying in {reconnect_delay} seconds...")
                    time.sleep(reconnect_delay)
                    # RFCOMM 재설정
                    setup_rfcomm()
                    continue
           
            # 연결된 상태에서 데이터 읽기 시도
            try:
                current_time = time.time()
               
                # 일정 시간 데이터가 없으면 연결 상태 확인
                if current_time - last_data_time > data_timeout:
                    if ser.in_waiting < 0:
                        print("Connection appears unstable, resetting...")
                        ser.close()
                        connected = False
                        cleanup_rfcomm()
                        time.sleep(1)
                        setup_rfcomm()
                        continue
               
                # 데이터가 있으면 읽기
                if ser.in_waiting > 0:
                    data = ser.readline().decode('utf-8', errors='replace').strip()
                    last_data_time = current_time  # 데이터 수신 시간 갱신
                   
                    if data:  # 빈 데이터가 아닌 경우에만 처리
                        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                        print(f"{timestamp}: {data}")
                       
                        # 데이터 로깅
                        with open("data.txt", "a") as f:
                            f.write(f"{timestamp},{data}\n")
               
                # 잠시 대기 (CPU 사용률 감소)
                time.sleep(0.1)
               
            except (serial.SerialException, OSError) as e:
                print(f"Serial error: {e}")
                if ser:
                    ser.close()
                connected = False
                cleanup_rfcomm()
                time.sleep(1)
                setup_rfcomm()
               
    except KeyboardInterrupt:
        print("Program stopped by user")
    finally:
        if ser and ser.is_open:
            ser.close()
        cleanup_rfcomm()

if __name__ == "__main__":
    main()

