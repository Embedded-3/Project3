# sub_secure.py
import json, base64
from Crypto.Cipher import AES, PKCS1_OAEP
from Crypto.PublicKey import RSA
from Crypto.Signature import pkcs1_15
from Crypto.Hash import SHA256
import paho.mqtt.client as mqtt
import ssl

def on_connect(client, userdata, flags, rc):
    print("[🔗 SUB] Connected with result code", rc)
    client.subscribe("ota/topic")

def on_message(client, userdata, msg):
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
    except:
        print("[❌ SUB] Signature mismatch! Rejecting OTA.")

# MQTT TLS 연결
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
client.loop_forever()
