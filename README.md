# Project3
ADAS CANoe




---

## ✅ 시스템 요약

### 🔧 Front MDPS

* **기능**

  * 조향 서보모터 제어

* **HW Pin Mapping**

| ShieldBuddy Pin | TC275 Pin | Function   |
| --------------- | --------- | ---------- |
| A14             | P20.7     | CAN0 RX    |
| A15             | P20.8     | CAN0 TX    |
| D7              | P02.5     | 조향 서보모터 제어 |

---

### 🔧 Middle BCU

* **기능**

  1. 시스템 시작 시 Welcome Light 제어
  2. 100ms 주기로 하이빔 상태 제어
  3. **FOTA 기능 지원**

     * CAN 수신: 기본/ADB/OTA 명령 처리
     * CAN 송신: 하이빔 상태
     * 수신된 Firmware Data → Flash에 Write

* **HW Pin Mapping**

| ShieldBuddy Pin | TC275 Pin | Function |
| --------------- | --------- | -------- |
| A14             | P20.7     | CAN0 RX  |
| A15             | P20.8     | CAN0 TX  |
| D8              | P02.6     | LEDL     |
| D9              | P02.7     | LEDR     |

---

### 🔧 Rear MCU

* **기능**

  * **10ms 주기**: 속도 측정 및 모터 제어
  * **50ms 주기**: 초음파 센서 거리 측정
  * **CAN 송신**: 속도, 제어값, 초음파 데이터

* **HW Pin Mapping**

| ShieldBuddy Pin | TC275 Pin | Function             |
| --------------- | --------- | -------------------- |
| A14             | P20.7     | CAN0 RX              |
| A15             | P20.8     | CAN0 TX              |
| D10             | P10.5     | Right Encoder B (방향) |
| D11             | P10.3     | Right Encoder A (속도) |
| D12             | P10.1     | Left Encoder B (방향)  |
| D13             | P10.2     | Left Encoder A (속도)  |
| D6              | P02.4     | 모터 방향                |
| D5              | P02.3     | 모터 PWM 출력            |
| D2              | P02.0     | 초음파 ECHO 입력          |
| D3              | P02.1     | 초음파 TRIG 출력          |

---

