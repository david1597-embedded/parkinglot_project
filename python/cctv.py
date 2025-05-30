import cv2
import datetime
from flask import Flask, Response

app = Flask(__name__)


cap1 = cv2.VideoCapture(0)
if not cap1.isOpened():
    print("CCTV1 ī�޶� ���� ����. ������ Ȯ���ϼ���.")
    exit()

def generate_frames(cap, label):
    """ī�޶� �����ӿ� �ؽ�Ʈ�� �߰��ϰ� MJPEG ��Ʈ���� ����"""
    while True:
        ret, frame = cap.read()
        if not ret:
            print(f"{label} ī�޶󿡼� ������ �б� ����.")
            break

        # ���� �ð�
        current_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

        # �� �߰� (���� ���)
        cv2.putText(
            frame,
            label,
            (10, 20),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.6,
            (0, 255, 0),
            1
        )

        # �ð� �߰� (���� �ϴ�)
        text_size = cv2.getTextSize(current_time, cv2.FONT_HERSHEY_SIMPLEX, 0.6, 1)[0]
        text_x = frame.shape[1] - text_size[0] - 5
        text_y = frame.shape[0] - 5
        cv2.putText(
            frame,
            current_time,
            (text_x, text_y),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.6,
            (0, 255, 0),
            1
        )

        # JPEG ���ڵ�
        ret, buffer = cv2.imencode('.jpg', frame)
        frame = buffer.tobytes()

        # MJPEG ��Ʈ���� ����
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')

@app.route('/')
def index():
    return """
    <html>
        <head><title>CCTV Streaming</title></head>
        <body>
            <h1>CCTV Streaming</h1>
            <h2>CCTV1</h2>
            <img src="/video_feed1" width="50%">
        </body>
    </html>
    """

@app.route('/video_feed1')
def video_feed1():
    return Response(generate_frames(cap1, "CCTV1"),
                    mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__ == '__main__':
    try:
        app.run(host='0.0.0.0', port=5000, threaded=True)
    finally:
        if cap1.isOpened():
            cap1.release()
        print("CCTV1 ī�޶� ���ҽ� ���� �Ϸ�.")
