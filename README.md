# OS_practice_homework

## 프로그램 진행

+1. Header(mod num)은 5이며, 0~4까지의 각 헤더에 버퍼 할당
+2. 각 헤더에 속한 버퍼는 3~10개 랜덤, 각 아이디는 1~99 랜덤, 상태는 랜덤
+3. 정의된 디스플레이 함수에 따라 콘솔에 버퍼 캐시 모든 정보 출력
+4. 사용자로부터 찾을 값을 입력
+5. getblk 알고리즘 실행

## get block 알고리즘의 구현
### Scenario1 : 해시 큐에 존재하며 프리리스트에 속하는 경우
### free_list에서 제거하고 값을 return
### Scenario2 : 해시 큐에 존재하지 않으며 프리리스트가 있는 경우
### free_list 첫 번째 값을 제거하고 block을 추가
### Scenario3 : 해시 큐에 존재하지 않으며 프리리스트 첫 번째가 delay인 경우
### free_list에서 delay를 건너 뛰고 처음으로 delay가 아닌 값을 제거하고 block 추가
### 그 과정에서 delay는 write으로 상태가 변경됨
### Scenario4 : 해시 큐에 존재하지 않으며 프리리스트가 없는 경우
### block release 대기 - 임의 시나리오로 3초 sleep하며 첫 번째로 발견된 unlock block을 프리리스트에 추가한다.
### Scenario5 : 해시 큐에 존재하며 프리리스트가 있지만 상태가 busy인 경우
### 프로세스 3초 sleep 이후 block상태를 unlock으로 변경, 이후 이를 사용 (lock으로 변경)

---
## 사용법
### 알고리즘 실행 시 사용자가 100을 입력하기 전까지 시나리오를 테스트하며 및 반복
