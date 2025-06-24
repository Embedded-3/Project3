# Project3
ADAS CANoe


---

## Front MDPS
### 1. 기능
조향 제어  

### 2. HW pin
| ShieldBuddy Pin | TC275 Pin Assignment | function |
|--|--|--|
| a14 | p20.7 | CAN0 RX |
| a15 | p20.8 | CAN0 TX |
| d7 | p02.5 | 조향 서보모터 제어 |

---

## Middle BCU
### 1. 기능
1. 시작시 welcome light  
100ms마다 하이빔 제어  

2. FOTA를 통해 Welcome Light 기능 업데이트  

can 송신 : 하이빔 상태  

can 수신 :  
- 기본 주행 => 기본 하이빔  
- ADB => 특정 LED만 ON  
- OTA Trigger => OTA UPDATE 준비  
- OTA Firmware data => 수신 후, Write Flash  

### 2. HW pin  

| ShieldBuddy Pin | TC275 Pin Assignment | function |
|--|--|--|
| a14 | p20.7 | CAN0 RX |
| a15 | p20.8 | CAN0 TX |
| d8 | p02.6 | LEDL |
| d9 | p02.7 | LEDR |


---

## Rear MCU
### 1. 기능
10ms 마다 속도 측정, 모터 제어  
can 송신 : 속도, 모터 제어 값  

50ms 마다 초음파 측정  
can 송신 : 초음파 데이터  


### 2. HW pin
| ShieldBuddy Pin | TC275 Pin Assignment | function |
|--|--|--|
| a14 | p20.7 | CAN0 RX |
| a15 | p20.8 | CAN0 TX |
| d10 | p10.5 | RIGHT ENCODER B (방향) |
| d11 | p10.3 | RIGHT ENCODER A (속도) |
| d12 | p10.1 | LEFT ENCODER B (방향) |
| d13 | p10.2 | LEFT ENCODER A (속도) |
| d6 | p02.4 | 모터 방향 |
| d5 | p02.3 | 모터 pwm 출력 |
| d2 | p02.0 | 초음파 ECHO INPUT | 
| d3 | p02.1 | 초음파 TRIG OUT | 






