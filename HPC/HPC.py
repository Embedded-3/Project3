import numpy as np
import cv2
import socket
import math
import time
import threading
from py_canoe import CANoe
import pythoncom
import json, base64
from Crypto.Cipher import AES, PKCS1_OAEP
from Crypto.PublicKey import RSA
from Crypto.Signature import pkcs1_15
from Crypto.Hash import SHA256
import paho.mqtt.client as mqtt
import ssl

OTA_flag = False
dt_ms = 100
angle = 0
angle_lock = threading.Lock()
terminate_flag = False
ESA_start_angle = 0
ESA_Setup_Flag = 0
Total_ms = 0
step = 0
buffer = b""
first = 1

UDP_IP = "0.0.0.0"
UDP_PORT = 5005
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

# ===== OTA 관련 함수 =====
def on_connect(client, userdata, flags, rc):
    global OTA_flag
    OTA_flag = False
    print("[🔗 SUB] Connected with result code", rc)
    client.subscribe("ota/topic")

def on_message(client, userdata, msg):
    global OTA_flag
    print("[📩 SUB] OTA message received")
    payload = json.loads(msg.payload.decode())

    # Base64 디코딩
    enc_aes_key = base64.b64decode(payload["aes_key"])
    nonce = base64.b64decode(payload["nonce"])
    tag = base64.b64decode(payload["tag"])
    cipher_data = base64.b64decode(payload["cipher_data"])
    signature = base64.b64decode(payload["signature"])

    # RSA 복호화 (AES 키)
    with open("C:/mqtt_certs/rsa_priv.pem", "rb") as f:
        rsa_priv = RSA.import_key(f.read())
    rsa_cipher = PKCS1_OAEP.new(rsa_priv)
    aes_key = rsa_cipher.decrypt(enc_aes_key)

    # AES 복호화
    aes = AES.new(aes_key, AES.MODE_EAX, nonce)
    firmware = aes.decrypt_and_verify(cipher_data, tag)

    # 서명 검증
    hash = SHA256.new(firmware)
    with open("C:/mqtt_certs/rsa_pub.pem", "rb") as f:
        rsa_pub = RSA.import_key(f.read())
    try:
        pkcs1_15.new(rsa_pub).verify(hash, signature)
        print("[✅ SUB] Signature valid! OTA file trusted.")
        with open("firmware.hex", "wb") as f:
            f.write(firmware)
        OTA_flag = True
        client.disconnect()
        
    except:
        print("[❌ SUB] Signature mismatch! Rejecting OTA.")


# ===== LKAS 관련 함수 =====
def detect_edges(gray):
    blurred = cv2.GaussianBlur(gray, (5, 5), 0)
    return cv2.Canny(blurred, 50, 150)

def region_of_interest(img):
    height, width = img.shape
    mask = np.zeros_like(img)
    polygon = np.array([[
        (0, height),
        (0, height // 2),
        (width, height // 2),
        (width, height)
    ]], np.int32)
    cv2.fillPoly(mask, polygon, 255)
    cropped = cv2.bitwise_and(img, mask)
    return cropped

def detect_line_segments(cropped_edges):
    return cv2.HoughLinesP(cropped_edges, 1, np.pi / 180, threshold=50,
                           minLineLength=10, maxLineGap=20)

def make_points(image, line_params):
    height = image.shape[0]
    slope, intercept = line_params
    y1 = height
    y2 = int(height * 0.6)
    try:
        x1 = int((y1 - intercept) / slope)
        x2 = int((y2 - intercept) / slope)
    except ZeroDivisionError:
        x1 = x2 = 0
    return [[x1, y1, x2, y2]]

def average_slope_intercept(image, segments):
    if segments is None:
        return []

    left, right = [], []
    for segment in segments:
        for x1, y1, x2, y2 in segment:
            if x1 == x2: continue
            slope = (y2 - y1) / (x2 - x1)
            intercept = y1 - slope * x1
            if slope < -0.5:
                left.append((slope, intercept))
            elif slope > 0.5:
                right.append((slope, intercept))

    lane_lines = []
    if left:
        avg = np.mean(left, axis=0)
        lane_lines.append(make_points(image, avg)[0])
    if right:
        avg = np.mean(right, axis=0)
        lane_lines.append(make_points(image, avg)[0])

    return lane_lines

def compute_steering_angle(image, lines):
    height, width = image.shape

    if len(lines) == 0:
        return 0
    if len(lines) == 1:
        x1, _, x2, _ = lines[0]
        x_offset = x2 - x1
    else:
        _, _, lx2, _ = lines[0]
        _, _, rx2, _ = lines[1]
        mid = width // 2
        x_offset = (lx2 + rx2) // 2 - mid

    y_offset = height // 2
    angle_rad = math.atan2(x_offset, y_offset)
    angle_deg = angle_rad * 180.0 / math.pi

    angle_deg = max(-30, min(30, angle_deg))

    angle_rounded = round(angle_deg / 5) * 5

    return int(angle_rounded)

def draw_lines(image, lines):
    out = image.copy()
    for x1, y1, x2, y2 in lines:
        color = (0, 255, 0) if x1 < image.shape[1] // 2 else (0, 0, 255)
        cv2.line(out, (x1, y1), (x2, y2), color, 3)
    return out


# ===== ADB 관련 함수 =====
def control(steering):
    BASE = 0b11000000  # 192
    left = BASE
    right = BASE

    if steering in [5, 10, 15, 20, 25, 30]:
        level = steering // 5
        right = BASE | (((1 << level) - 1) << (6 - level))
        print(f"Right{level}")
    elif steering in [-5, -10, -15, -20, -25, -30]:
        level = abs(steering) // 5
        left = BASE | (((1 << level) - 1) << (6 - level))
        print(f"Left{level}")
    else:
        print("Default")

    return left, right


# ===== ESA 관련 함수 =====
def ESA_total_time_ms(v_kmph):
    return int(max(500, min(0.5 * 100000 / v_kmph, 2000)))  # 500ms ~ 3000ms


def ESA_angle(step, angle_start, T_total_ms):
    if (step * dt_ms) >= T_total_ms:
        return 0
    t_ratio = (step * dt_ms) / T_total_ms
    easing = 0.5 + 0.5 * math.cos(math.pi * t_ratio)
    raw_angle = angle_start * easing
    rounded_to_5deg = round(raw_angle / 5) * 5
    return int(rounded_to_5deg)


def control_thread():
    pythoncom.CoInitialize()
    global angle, terminate_flag, ESA_Setup_Flag, ESA_start_angle, Total_ms, step, first
    canoe_inst = CANoe()
    canoe_inst.open(canoe_cfg=r'C:\Users\USER\Desktop\pjt3\Project3\CANoe\team3pjt.cfg')
    canoe_inst.start_measurement()
    time.sleep(5)
    canoe_inst.set_system_variable_value('ADAS::Python_OnOff',1)

    # ===== SYSTEM VAL =====
    ADAS_ONOff = canoe_inst.get_system_variable_value('ADAS::ADAS_ONOff')
    # ADAS_ONOff=1
    ADAS_Steering = canoe_inst.get_system_variable_value('ADAS::ADAS_Steering')
    # ADAS_Steering=0
    ESA_OnOff = canoe_inst.get_system_variable_value('ADAS::ESA_OnOff')
    # ESA_Onoff=0
    ESA_Alert = canoe_inst.get_system_variable_value('ADAS::ESA_Alert')
    # ESA_Alert=0
    ADB_Left = canoe_inst.get_system_variable_value('ADAS::ADB_Left')
    ADB_OnOff = canoe_inst.get_system_variable_value('ADAS::ADB_OnOff')
    ADB_Right = canoe_inst.get_system_variable_value('ADAS::ADB_Right')
    Car_Speed = canoe_inst.get_system_variable_value('ADAS::Car_Speed')
    # Car_Speed = 30
    LKAS_OnOff = canoe_inst.get_system_variable_value('ADAS::LKAS_OnOff')
    Python_OnOff = canoe_inst.get_system_variable_value('ADAS::Python_OnOff')
    USS_Detection = canoe_inst.get_system_variable_value('ADAS::USS_Detection')
    # USS_Detection=40
    USS_Distance = canoe_inst.get_system_variable_value('ADAS::USS_Distance')
    # USS_Distance=30
    # SteeringAngle = canoe_inst.get_system_variable_value('FRONT::MDPS::SteeringAngle')
    SteeringAngle=30
    LeftTurnLight_Status = canoe_inst.get_system_variable_value('MIDDLE::MBCU::HLAMP::LeftTurnLight_Status')
    # LeftTurnLight_Status=0

    while not terminate_flag:
        ADAS_ONOff = canoe_inst.get_system_variable_value('ADAS::ADAS_ONOff')
        ADAS_Steering = canoe_inst.get_system_variable_value('ADAS::ADAS_Steering')
        ESA_Onoff = canoe_inst.get_system_variable_value('ADAS::ESA_OnOff')
        ESA_Alert = canoe_inst.get_system_variable_value('ADAS::ESA_Alert')
        ADB_OnOff = canoe_inst.get_system_variable_value('ADAS::ADB_OnOff')
        ADB_Left = canoe_inst.get_system_variable_value('ADAS::ADB_Left')
        ADB_Right = canoe_inst.get_system_variable_value('ADAS::ADB_Right')
        # Car_Speed = canoe_inst.get_system_variable_value('ADAS::Car_Speed')
        Car_Speed = 50
        LKAS_OnOff = canoe_inst.get_system_variable_value('ADAS::LKAS_OnOff')
        Python_OnOff = canoe_inst.get_system_variable_value('ADAS::Python_OnOff')
        USS_Detection = canoe_inst.get_system_variable_value('ADAS::USS_Detection')
        USS_Distance = canoe_inst.get_system_variable_value('ADAS::USS_Distance')
        # SteeringAngle = canoe_inst.get_system_variable_value('FRONT::MDPS::SteeringAngle')
        LeftTurnLight_Status = canoe_inst.get_system_variable_value('MIDDLE::MBCU::HLAMP::LeftTurnLight_Status')
        Ignition = canoe_inst.get_system_variable_value('FRONT::VCU::Ignition')

        if Ignition:
            first = 0
            if ADAS_ONOff:
                canoe_inst.set_system_variable_value('ADAS::LKAS_OnOff', 1)
                canoe_inst.set_system_variable_value('ADAS::ADB_OnOff',1)
                # Condition 1 : 차선 변경을 위한 방향 지시등 ON
                if ESA_Onoff or LeftTurnLight_Status:
                    # LeftTurnLight_Status = 0
                    # LKAS 종료 & ESA 시작
                    canoe_inst.set_system_variable_value('ADAS::LKAS_OnOff', 0)
                    canoe_inst.set_system_variable_value('ADAS::ADB_OnOff',0)
                    canoe_inst.set_system_variable_value('ADAS::ESA_OnOff', 1)
                    print("START ESA")
                    # Condition 2 : 측후방 물체가 설정 값 이내로 들어옴
                    # if False:
                    # if True:
                    if USS_Detection <= USS_Distance:
                        # 충돌 경고 알람 발생
                        canoe_inst.set_system_variable_value('ADAS::ESA_Alert', 1)
                        print(f"Collision Alert!!---->{ADAS_Steering}")
                        # Condition 3 : 충돌 위험 감지 못하고 차선 변경 시도
                        # if False:
                        # if True:
                        if ADAS_Steering <= -5:
                            if not ESA_Setup_Flag:
                                # 초기 ESA setting
                                Total_ms = ESA_total_time_ms(Car_Speed)
                                ESA_start_angle = 15
                                ESA_Setup_Flag = 1

                            # ESA 조향이 끝났으면
                            if(step*dt_ms) > Total_ms:
                                ESA_Setup_Flag = 0
                                step = 0
                                Total_ms = 0
                                ESA_start_angle = 0
                                canoe_inst.set_system_variable_value('ADAS::ESA_OnOff', 0)
                                canoe_inst.set_system_variable_value('MIDDLE::MBCU::HLAMP::LeftTurnLight_Status',0)
                            # ESA에서 Steering 보조
                            else:
                                ADAS_Steering = ESA_angle(step,ESA_start_angle,Total_ms)
                                step += 1
                                canoe_inst.set_system_variable_value('ADAS::ADAS_Steering', ADAS_Steering)
                                print(f"ESA Take Steering Control------------>{ADAS_Steering}")
                                

                # LKAS가 기본값
                else:
                    canoe_inst.set_system_variable_value('ADAS::ESA_Alert', 0)
                    canoe_inst.set_system_variable_value('ADAS::ADAS_Steering', angle)
                    SteeringAngle = angle
                    print(f"LKAS---------->{angle}")

                    Lctl, Rctl = control(SteeringAngle)
                    canoe_inst.set_system_variable_value('ADAS::ADB_Left',Lctl)
                    canoe_inst.set_system_variable_value('ADAS::ADB_Right',Rctl)
                    print(Lctl, Rctl)
            else:
                print("=========ADAS OFF=========")
        else:
            if not first:
                client = mqtt.Client()
                client.on_connect = on_connect
                client.on_message = on_message
                client.tls_set(
                    ca_certs="C:/mqtt_certs/ca.crt",
                    certfile="C:/mqtt_certs/client.crt",
                    keyfile="C:/mqtt_certs/client.key",
                    tls_version=ssl.PROTOCOL_TLSv1_2
                )
                client.connect("localhost", 8883)
                client.loop_start()
                timeout_s = 10 
                elapsed = 0
                while not OTA_flag and elapsed < timeout_s:
                    time.sleep(0.1)
                    elapsed += 0.1
                # if OTA_flag:
                #     time.sleep(2)
                #     canoe_inst.set_system_variable_value('MIDDLE::HU::OTA_Accept', 1)
                client.loop_stop()
                client.disconnect()
                canoe_inst.set_system_variable_value('ADAS::Update_Alarm', 1)

        time.sleep(0.2)
        

def vision_thread():
    global angle, terminate_flag, buffer
    while not terminate_flag:
        try:
            packet, _ = sock.recvfrom(65507)
            buffer += packet

            if len(packet) < 60000:  # 마지막 패킷
                np_data = np.frombuffer(buffer, dtype=np.uint8)
                gray_img = cv2.imdecode(np_data, cv2.IMREAD_GRAYSCALE)
                buffer = b''

                if gray_img is None:
                    continue

                edges = detect_edges(gray_img)
                roi = region_of_interest(edges)
                segments = detect_line_segments(roi)
                lines = average_slope_intercept(gray_img, segments)
                computed_angle = compute_steering_angle(gray_img, lines)
                with angle_lock:
                    angle = computed_angle
                # 시각화
                color_img = cv2.cvtColor(gray_img, cv2.COLOR_GRAY2BGR)
                visual = draw_lines(color_img, lines)
                # cv2.imshow("Received Gray", gray_img)
                cv2.imshow("Lane Detection", visual)

                if cv2.waitKey(1) & 0xFF == ord('q'):
                    break

        except Exception as e:
            print(f"[오류] {e}")
            buffer = b''  # 손상 방지용 초기화
            continue
    
    
def main():
    global terminate_flag
    t1 = threading.Thread(target=vision_thread)
    t2 = threading.Thread(target=control_thread)
    t1.start()
    t2.start()

    try:
        # 메인 스레드는 대기만 하고 있다가 Ctrl+C 감지
        while t1.is_alive() and t2.is_alive():
            time.sleep(0.5)

    except KeyboardInterrupt:
        print("\n🛑 Ctrl+C 감지됨: 종료 처리 중...")
        terminate_flag = True

    finally:
        t1.join()
        # t2.join()
        cv2.destroyAllWindows()
        print("✅ 모든 종료 완료")

if __name__ == "__main__":
    main()
