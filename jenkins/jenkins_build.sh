#!/bin/bash

SERVER_USER="~~~"
SERVER_IP="~~~"

CONTAINER_NAME="jenkins"                    # 깃허브 훅업용 Jenkins가 실행 중인 Docker 컨테이너 이름
CONTAINER_FLAG_PATH="/mnt/trigger_flag.txt" # 컨테이너 내부 플래그 파일 경로

WIN_IP=$(cat /mnt/d/ip_info.txt)    # Windows IP 주소를 파일에서 읽어오기 # 네트워크마다 변경 필요

LOCAL_JENKINS_URL="http://${WIN_IP}:8080"

JOB_NAME="Test_CANoe"
JENKINS_USER="~~~"
JENKINS_API_TOKEN="~~~"

# 1. SSH 접속 후 docker exec로 컨테이너 내부 flag 읽기
status=$(ssh ${SERVER_USER}@${SERVER_IP} "docker exec ${CONTAINER_NAME} cat ${CONTAINER_FLAG_PATH}")

now=$(date '+%Y-%m-%d %H:%M:%S')  # 현재 연월일 시간
echo "<=========   $now   ==========>"

if [ "$status" = "yes" ]; then

  echo "[+] ✅ 빌드 요청 감지! Jenkins Job 실행 중..."

  # Crumb 발급 요청 (jq 버전)
  CRUMB=$(curl -s --user "${JENKINS_USER}:${JENKINS_API_TOKEN}" \
      "${LOCAL_JENKINS_URL}/crumbIssuer/api/json" | jq -r '.crumbRequestField + ":" + .crumb')

  echo "[DEBUG] Crumb: $CRUMB"

  # Jenkins 빌드 요청
  curl -X POST "${LOCAL_JENKINS_URL}/job/${JOB_NAME}/build" \
      -H "${CRUMB}" \
      --user "${JENKINS_USER}:${JENKINS_API_TOKEN}"
  echo "[+] 👴 Jenkins 빌드 요청 완료"

  # 2. 플래그 초기화 - SSH로 컨테이너 내부 파일 내용 변경
  ssh ${SERVER_USER}@${SERVER_IP} "docker exec ${CONTAINER_NAME} bash -c 'echo no > ${CONTAINER_FLAG_PATH}'"
  echo "[+] 🚩 플래그 초기화 완료"

else
  echo "[-] ❌ 빌드 요청 없음 (flag=$status)"

echo " "
fi
