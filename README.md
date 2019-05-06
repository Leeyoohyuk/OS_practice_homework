# OS_practice_homework

## HW-1. Buffer-cache 프로그램 진행

1. Header(mod num)은 5이며, 0\~4까지의 각 헤더에 버퍼 할당  
2. 각 헤더에 속한 버퍼는 3\~10개 랜덤, 각 아이디는 1~99 랜덤, 상태는 랜덤  
3. 정의된 디스플레이 함수에 따라 콘솔에 버퍼 캐시 모든 정보 출력  
4. 사용자로부터 찾을 값을 입력  
5. getblk 알고리즘 실행  

## get block 알고리즘의 구현
Scenario1 : 해시 큐에 존재하며 프리리스트에 속하는 경우  
free_list에서 제거하고 값을 return  
Scenario2 : 해시 큐에 존재하지 않으며 프리리스트가 있는 경우  
free_list 첫 번째 값을 제거하고 block을 추가  
Scenario3 : 해시 큐에 존재하지 않으며 프리리스트 첫 번째가 delay인 경우  
free_list에서 delay를 건너 뛰고 처음으로 delay가 아닌 값을 제거하고 block 추가  
그 과정에서 delay는 write으로 상태가 변경됨  
Scenario4 : 해시 큐에 존재하지 않으며 프리리스트가 없는 경우  
block release 대기 - 임의 시나리오로 3초 sleep하며 첫 번째로 발견된 unlock block을 프리리스트에 추가  
Scenario5 : 해시 큐에 존재하며 프리리스트가 있지만 상태가 busy인 경우  
프로세스 3초 sleep 이후 block상태를 unlock으로 변경, 이후 이를 사용 (lock으로 변경)  

---
## 사용법
### 알고리즘 실행 시 사용자가 100을 입력하기 전까지 시나리오를 테스트하며 및 반복
---

## HW-2. DMA transfer 프로그램 진행

1. Input으로 Task 개수를 입력, 이때 Task의 종류는 무작위로 할당.
2. 입력받은 Task를 IDE Controller에 할당.
3. IDE Controller(Device Controller)는 할당된 Task를 처리 후, DMA Controller에게 알림.
4. DMA Controller는 IDE Controller가 처리한 Task들을 Memory Buffer에 할당.
5. 모든 IDE Controller의 Task가 종료되면 Output으로 알림.

---
## 사용법
**1. Task 개수 지정**  
**2. Task Type 개수를 입력(device 종류가 몇 개인지, 개수는 최소 3개 이상)**  
**3. 5종류의 device 중 작업을 진행할 device 선택, 한 줄에 선택지를 2에서 선택한 만큼 입력한다.**  
**4. 출력을  assigning, processing 파악**  
**5. DMAcontroller가 모든 processing task를 memory buffer에 할당한 경우 출력을 통해 확인**  
