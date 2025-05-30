import time
import pymysql

db_config = {
    "host": "10.10.10.106",
    "user": "root",
    "passwd": "1234",
    "db": "DHT11",
    "autocommit": True
}

def clear_database():
    try:
        conn = pymysql.connect(**db_config)
        with conn.cursor() as cur:
            cur.execute("DELETE FROM park1")
            print("✅ 기존 DB 데이터 삭제 완료")
        conn.close()
    except Exception as e:
        print("❌ DB 초기화 실패:", e)

def log_data():
    conn = pymysql.connect(**db_config)
    cursor = conn.cursor()
    sql = "INSERT INTO park1(recorded_at, temp, humi) VALUES(NOW(), %s, %s)"

    while True:
        try:
            with open("/home/test/project/data.txt", "r") as f:
                last_line = f.readlines()[-1].strip()
            data_part = last_line.split('[')[0].strip()
            parts = data_part.split('/')
            temperature = int(parts[0].split(':')[3])
            humidity = int(parts[1].split(':')[1])

            print(f"✅ 파싱 성공: Temp={temperature}, Humi={humidity}")
            cursor.execute(sql, (temperature, humidity))
        except Exception as e:
            print("❌ 파싱 또는 DB 저장 오류:", e)

        time.sleep(1)

if __name__ == "__main__":
    #clear_database()
    log_data()
