# pub_secure.py
import json, base64, time
from Crypto.Cipher import AES, PKCS1_OAEP
from Crypto.PublicKey import RSA
from Crypto.Random import get_random_bytes
from Crypto.Signature import pkcs1_15
from Crypto.Hash import SHA256
import paho.mqtt.client as mqtt
import ssl

# 1. 펌웨어 읽기
with open("firmware.hex", "rb") as f:
    firmware = f.read()

# 2. AES 암호화
aes_key = get_random_bytes(16)
cipher = AES.new(aes_key, AES.MODE_EAX)
cipher_data, tag = cipher.encrypt_and_digest(firmware)

# 3. AES 키를 RSA 공개키로 암호화
with open("C:/mqtt_certs/rsa_pub.pem", "rb") as f:
    rsa_pub = RSA.import_key(f.read())
rsa_cipher = PKCS1_OAEP.new(rsa_pub)
enc_aes_key = rsa_cipher.encrypt(aes_key)

# 4. SHA256 + RSA 서명
hash = SHA256.new(firmware)
with open("C:/mqtt_certs/rsa_priv.pem", "rb") as f:
    rsa_priv = RSA.import_key(f.read())
signature = pkcs1_15.new(rsa_priv).sign(hash)

# 5. JSON 메시지 구성
message = {
    "aes_key": base64.b64encode(enc_aes_key).decode(),
    "nonce": base64.b64encode(cipher.nonce).decode(),
    "tag": base64.b64encode(tag).decode(),
    "cipher_data": base64.b64encode(cipher_data).decode(),
    "signature": base64.b64encode(signature).decode()
}

# 6. MQTT TLS 전송
client = mqtt.Client()
client.tls_set(
    ca_certs="C:/mqtt_certs/ca.crt",
    certfile="C:/mqtt_certs/client.crt",
    keyfile="C:/mqtt_certs/client.key",
    tls_version=ssl.PROTOCOL_TLSv1_2
)
client.connect("localhost", 8883)
client.loop_start()
client.publish("ota/topic", json.dumps(message))
time.sleep(1)  # 전송 완료 대기
client.loop_stop()
client.disconnect()
