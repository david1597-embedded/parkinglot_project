from flask import Flask, jsonify, request, session
from flask_cors import CORS
import mysql.connector
from mysql.connector import Error
import sys
from datetime import datetime

app = Flask(__name__)
CORS(app)  # 모든 클라이언트에 CORS 허용
app.secret_key = 'your-secret-key'  # 세션 관리를 위한 시크릿 키 (실제 배포 시 보안 키로 변경)

# MySQL 데이터베이스 연결 함수
def get_db_connection(db_name):
    try:
        connection = mysql.connector.connect(
            host='localhost',
            database=db_name,
            user='root',
            password='1234'
        )
        if connection.is_connected():
            print(f"Successfully connected to the MySQL database {db_name}")
            return connection
    except Error as e:
        print(f"Error connecting to MySQL: {e}")
        return None
    except Exception as e:
        print(f"Unexpected error during database connection: {e}")
        return None

# 사용자 로그 테이블 생성 (최초 실행 시)
def init_user_logs_table():
    connection = get_db_connection('User')
    if connection is None:
        print("Failed to connect to User database for table initialization")
        return
    try:
        cursor = connection.cursor()
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS user_logs (
                id INT AUTO_INCREMENT PRIMARY KEY,
                user_id VARCHAR(255),
                user_name VARCHAR(255),
                event VARCHAR(50),
                event_time DATETIME
            )
        """)
        connection.commit()
        print("user_logs table initialized")
    except Error as e:
        print(f"Error initializing user_logs table: {e}")
    finally:
        if 'cursor' in locals():
            cursor.close()
        if connection.is_connected():
            connection.close()

# 활성 사용자 테이블 생성
def init_active_users_table():
    connection = get_db_connection('User')
    if connection is None:
        print("Failed to connect to User database for table initialization")
        return
    try:
        cursor = connection.cursor()
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS active_users (
                user_id VARCHAR(255) PRIMARY KEY,
                user_name VARCHAR(255),
                login_time DATETIME
            )
        """)
        connection.commit()
        print("active_users table initialized")
    except Error as e:
        print(f"Error initializing active_users table: {e}")
    finally:
        if 'cursor' in locals():
            cursor.close()
        if connection.is_connected():
            connection.close()

# 사용자 목록 조회
@app.route('/users', methods=['GET'])
def get_users():
    connection = get_db_connection('User')
    if connection is None:
        return jsonify({"error": "Failed to connect to database"}), 500
    
    try:
        cursor = connection.cursor(dictionary=True)
        cursor.execute("SELECT id, name, password, num FROM data")
        users = cursor.fetchall()
        return jsonify(users)
    except Error as e:
        print(f"Database error during query execution: {e}")
        return jsonify({"error": f"Database error: {e}"}), 500
    finally:
        if 'cursor' in locals():
            cursor.close()
        if connection.is_connected():
            connection.close()

# 로그인 처리
@app.route('/login', methods=['POST'])
def login():
    data = request.get_json()
    id = data.get('id')
    password = data.get('password')

    if not id or not password:
        return jsonify({"error": "Username and password are required"}), 400

    connection = get_db_connection("User")
    if connection is None:
        return jsonify({"error": "Failed to connect to database"}), 500
    
    try:
        cursor = connection.cursor(dictionary=True)
        cursor.execute("SELECT id, name, password, num FROM data WHERE id = %s", (id,))
        user = cursor.fetchone()
        
        if user:
            if user['password'] == password:
                # 활성 사용자 테이블에 추가
                cursor.execute("""
                    INSERT INTO active_users (user_id, user_name, login_time)
                    VALUES (%s, %s, %s)
                    ON DUPLICATE KEY UPDATE user_name = %s, login_time = %s
                """, (user['id'], user['name'], datetime.now(), user['name'], datetime.now()))
                connection.commit()
                
                # 로그인 로그 기록
                cursor.execute("""
                    INSERT INTO user_logs (user_id, user_name, event, event_time)
                    VALUES (%s, %s, %s, %s)
                """, (user['id'], user['name'], 'login', datetime.now()))
                connection.commit()
                
                return jsonify({
                    "message": "Login successful",
                    "id": user['id'],
                    "name": user['name'],
                    "num": user['num']
                }), 200
            else:
                return jsonify({"error": "Invalid password"}), 401
        else:
            return jsonify({"error": "Username not found"}), 404
    except Error as e:
        print(f"Database error during login: {e}")
        return jsonify({"error": f"Database error: {e}"}), 500
    finally:
        if 'cursor' in locals():
            cursor.close()
        if connection.is_connected():
            connection.close()

# 로그아웃 처리
@app.route('/logout', methods=['POST'])
def logout():
    data = request.get_json()
    user_id = data.get('id')
    user_name = data.get('name')

    if not user_id or not user_name:
        return jsonify({"error": "User ID and name are required"}), 400

    connection = get_db_connection('User')
    if connection is None:
        return jsonify({"error": "Failed to connect to database"}), 500

    try:
        cursor = connection.cursor()
        # 로그아웃 로그 기록
        cursor.execute("""
            INSERT INTO user_logs (user_id, user_name, event, event_time)
            VALUES (%s, %s, %s, %s)
        """, (user_id, user_name, 'logout', datetime.now()))
        
        # 활성 사용자 목록에서 제거
        cursor.execute("DELETE FROM active_users WHERE user_id = %s", (user_id,))
        connection.commit()
        
        return jsonify({"message": "Logout successful"}), 200
    except Error as e:
        print(f"Database error during logout: {e}")
        return jsonify({"error": f"Database error: {e}"}), 500
    finally:
        if 'cursor' in locals():
            cursor.close()
        if connection.is_connected():
            connection.close()

# 현재 접속자 조회
@app.route('/active_users', methods=['GET'])
def get_active_users():
    connection = get_db_connection('User')
    if connection is None:
        return jsonify({"error": "Failed to connect to database"}), 500
    
    try:
        cursor = connection.cursor(dictionary=True)
        cursor.execute("SELECT user_id, user_name, login_time FROM active_users")
        active_users = cursor.fetchall()
        return jsonify({"active_users": active_users}), 200
    except Error as e:
        print(f"Database error during active users query: {e}")
        return jsonify({"error": f"Database error: {e}"}), 500
    finally:
        if 'cursor' in locals():
            cursor.close()
        if connection.is_connected():
            connection.close()

# 데이터 조회
@app.route('/fetch_data', methods=['POST'])
def fetch_data():
    data = request.get_json()
    num = data.get('num')

    if not num:
        return jsonify({"error": "num is required"}), 400

    table_name = f"park{num}"
    result = {
        "DHT11": {},
        "Park": {}
    }

    dht11_conn = get_db_connection('DHT11')
    if dht11_conn is None:
        return jsonify({"error": "Failed to connect to DHT11 database"}), 500

    try:
        cursor = dht11_conn.cursor(dictionary=True)
        cursor.execute(f"SELECT * FROM {table_name} WHERE id = (SELECT MAX(id) FROM {table_name})")
        result["DHT11"] = cursor.fetchone() or {}
    except Error as e:
        print(f"Database error in DHT11: {e}")
        return jsonify({"error": f"DHT11 database error: {e}"}), 500
    finally:
        if 'cursor' in locals():
            cursor.close()
        if dht11_conn.is_connected():
            dht11_conn.close()

    park_conn = get_db_connection('Park')
    if park_conn is None:
        return jsonify({"error": "Failed to connect to Park database"}), 500

    try:
        cursor = park_conn.cursor(dictionary=True)
        cursor.execute(f"SELECT * FROM {table_name} WHERE id = (SELECT MAX(id) FROM {table_name})")
        result["Park"] = cursor.fetchone() or {}
    except Error as e:
        print(f"Database error in Park: {e}")
        return jsonify({"error": f"Park database error: {e}"}), 500
    finally:
        if 'cursor' in locals():
            cursor.close()
        if park_conn.is_connected():
            park_conn.close()

    return jsonify({"message": "Data fetched successfully", "data": result}), 200

if __name__ == '__main__':
    # 테이블 초기화
    init_user_logs_table()
    init_active_users_table()
    
    connection = get_db_connection('User')
    if connection:
        connection.close()
        print("Initial database connection test successful")
    try:
        app.run(host='0.0.0.0', debug=True, port=5001)
    except Exception as e:
        print(f"Error starting Flask application: {e}")
        sys.exit(1)