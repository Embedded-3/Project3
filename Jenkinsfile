pipeline {
    agent any

    stages {
        // stage('Checkout') {
        //     steps {
        //         echo 'canoe branch'
        //         //git branch: 'CANoe_Study', url: 'https://github.com/Embedded-3/Project3-1.git'
        //     }
        // }

        // stage('Lint') {
        //     steps {
        //         echo '변경 및 추가된 C/H 파일 목록 추출 중...'

        //         script {
        //             def changedFilesRaw = sh(
        //                 script: 'git diff --diff-filter=AM --name-only HEAD~1 HEAD | grep -E "\\.(c|h)$" || true',
        //                 returnStdout: true
        //             ).trim()

        //             if (changedFilesRaw == "") {
        //                 echo "변경 또는 추가된 C 소스 파일이 없습니다."
        //                 currentBuild.result = 'SUCCESS'
        //                 return
        //             }

        //             // 줄바꿈으로 된 파일 목록을 공백으로 변환해서 한 줄로 만듦
        //             def changedFiles = changedFilesRaw.replaceAll("\\r?\\n", " ")

        //             echo "변경 및 추가된 파일:\n${changedFilesRaw}"

        //             sh "cppcheck --enable=all --inconclusive --quiet --force ${changedFiles}"
        //         }
        //     }
        // }

        // stage('Build') {
        //     steps {
        //         echo '빌드 단계 진행 중...'
        //         //sh 'gcc -o test1 test1.c'
        //     }
        // }
        


        stage('Trigger') {
            steps {
                echo "Trigger ...ing"
                script {
                    def flagFile = '/mnt/trigger_flag.txt'
                    def flag = readFile(flagFile).trim()
                    echo "현재 flag 값: ${flag}"

                    if (flag == 'no') {
                        echo "flag가 'no'입니다. 'yes'로 변경합니다."
                        writeFile file: flagFile, text: 'yes\n'
                    } else {
                        echo "flag가 'yes'입니다. 변경하지 않습니다."
                    }
                }
            }
        }
    }
}