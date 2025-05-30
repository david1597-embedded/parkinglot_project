
import easyocr
import time
import cv2
import matplotlib.pyplot as plt
import re
import logging
from datetime import datetime

# CPU use
reader = easyocr.Reader(['ko'], gpu=False)

log_file_path = 'data.txt'
keyword = 'CARIN'
number_log_file_path = 'number_data.txt'

logger = logging.getLogger('parking_log')
logger.setLevel(logging.INFO)

formatter = logging.Formatter('%(asctime)s %(message)s', datefmt='%Y-%m-%d %H:%M:%S')


file_handler = logging.FileHandler(number_log_file_path)
file_handler.setFormatter(formatter)
logger.addHandler(file_handler)


def log_parking_event(car_number):
    now = datetime.now()
    log_message = f'{car_number}'
    logger.info(log_message)


def recognize_number(image_path):
    try:
        img = cv2.imread(image_path)

        if img is None:
            print("can not open image")
        else:
            plt.figure(figsize=(8, 8))
            plt.imshow(img[:, :, ::-1])
            plt.axis('off')

            result = reader.readtext(img) 

            THRESHOLD = 0.1 

            plates = []
            current_plate = ""

            for bbox, text, conf in result:
                if conf > THRESHOLD:
                    current_plate += text 
                    cv2.rectangle(img, pt1=tuple(map(int, bbox[0])), pt2=tuple(map(int, bbox[2])), color=(0, 255, 0),
                                  thickness=3)

                    if len(current_plate) > 5:  
                        plates.append(current_plate)
                        current_plate = ""
            
            if current_plate:
                plates.append(current_plate)

            print(f"{plates[0]}")
            log_parking_event(plates[0])

            plt.figure(figsize=(8, 8))
            plt.imshow(img[:, :, ::-1])
            plt.axis('off')

    except Exception as e:
        print(f"error recognize car number {e}")


def monitor_log():
    last_position = 0
    while True:
        try:
            with open(log_file_path, 'r') as f:
                f.seek(last_position)
                new_lines = f.readlines()
                for line in new_lines:
                    if keyword in line:
                        print(f"'{keyword}' captured: {line.strip()}")
                        capture_and_process()
                last_position = f.tell()
            time.sleep(1) 
        except FileNotFoundError:
            print(f"can not find file '{log_file_path}'")
            time.sleep(5)
        except Exception as e:
            print(f"error log monitoring: {e}")
            time.sleep(5)

def capture_image():
    try:
        # 0 base camera
        cap = cv2.VideoCapture(1)
        if not cap.isOpened():
            print("can not open camera!")
            return

        ret, frame = cap.read()
        if not ret:
            print("can not read frame!")
            cap.release()
            return

        timestamp = time.strftime("%Y%m%d_%H%M%S")
        image_path = f'captured_{timestamp}.jpg' 
        cv2.imwrite(image_path, frame)
        print(f"image saved: {image_path}")
        cap.release()
        recognize_number(image_path) 
    except Exception as e:
        print(f"error while capturing : {e}")


def capture_and_process():
    print("capture and recognize car number")
    
    capture_image()

if __name__ == "__main__":
    monitor_log()























































