# rpi_send.py
import socket
import cv2
import numpy as np
from picamera2 import Picamera2
import time

UDP_IP = "192.168.100.1"  #  PC IP 주소
UDP_PORT = 5005           # 전송 포트

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

picam2 = Picamera2()
# picam2.preview_configuration.main.size = (320, 240)
picam2.preview_configuration.main.format = "RGB888"
picam2.configure("preview")
picam2.start()
time.sleep(1)

while True:
    frame = picam2.capture_array()

    # ==== [1] ROI 설정: 상단 40% 제거 ====
    h = frame.shape[0]
    roi = frame[int(h*0.3):, :]  # 하단 60%만 남김

    # ==== [2] 그레이스케일 변환 ====
    gray = cv2.cvtColor(roi, cv2.COLOR_BGR2GRAY)

    # ==== [3] JPEG 압축 및 바이너리 변환 ====
    _, img_encoded = cv2.imencode('.jpg', gray, [int(cv2.IMWRITE_JPEG_QUALITY), 70])
    data = img_encoded.tobytes()

    # ==== [4] UDP 전송: 60,000바이트씩 분할 전송 ====
    max_size = 60000
    for i in range(0, len(data), max_size):
        sock.sendto(data[i:i + max_size], (UDP_IP, UDP_PORT))

    time.sleep(0.2)  # 100ms 간격                                                                                                      