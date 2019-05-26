# OS_practice_homework

## PRJ-1. Buffer-cache 프로그램 진행

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

## PRJ-2. DMA transfer 프로그램 개요

1. Input으로 Task 개수를 입력, 이때 Task의 종류는 무작위로 할당.
2. 입력받은 Task를 IDE Controller에 할당.
3. IDE Controller(Device Controller)는 할당된 Task를 처리 후, DMA Controller에게 알림.
4. DMA Controller는 IDE Controller가 처리한 Task들을 Memory Buffer에 할당.
5. 모든 IDE Controller의 Task가 종료되면 Output으로 알림.

---
## 상세 진행 및 사용법
**1. 시작 시 Task갯수(500개 이상)와 _Device type_ 의 수(3개 이상)를 입력받음**  

**2. _Device type_ 별로 _IDEcontroller_(이하 _IDEcon_)가 세팅 ex) device가 3개면 3개의 _IDEcon_ 생성-세팅)**  

**3. _IDEcon_ 개수 2배의 스레드가 생성되고 각각 _assigning, processing_ 작업 진행**  

**4. 각 스레드들이 할당, 처리 병행 ex) _IDEcon_ 이 3개면 -> 할당 3개, 처리 3개 = 총 6개**  

**- _Processing thread_ 는 한 번에 하나의 처리만 하고 다른 스레드에게 _critical section_ 양보**  

**- _Processing_ 되는 task는 곧바로 _DMAcontroller_ 에 push 함**  

**5. 모든 _assigning, processing_ 완료 -> 모든 작업 처리 완료 출력 및 _DMAcon transfer_ 시작 알림**  

**- 여기까지 진행 후 출력 확인을 위한 5초 간 Sleep이 존재**  

**6. _DMA transfer_ 는 각 _IDEcon_ 으로부터 processing이 누적된 순서로 _memory buffer_ 에 진행**  

**7. 모든 작업 완료 시 Output**

---

## HW-1. Banker's algorithm 프로그램 설명

1. Input으로 Instance 개수, 각 Instance가 갖는 resource 개수, 프로세서 할당
2. 프로세서에 Max, Alloc resource를 조건에 맞게 할당 (조건에 부합하지 않을 경우 종료)
3. 할당이 정상적으로 완료된 경우 각 instance, available, process resource 상태 출력
4. Safety algorithm을 통해 safety or not safety 판별
5. Safety하다고 판별될 경우 순서 출력

## HW-2. Contiguous allocation 프로그램 설명

1. Input으로 Memory size할당
2. 프로세스 번호 입력 (프로세스 번호가 0일 경우 프로그램 종료, 이미 존재하는 프로세스 Request 불가)
3. 프로세스 사이즈 입력 (0이 입력될 경우 해당 프로세스에 대한 Release작업)
4. Request와 Release 명령을 구분하여 Function call
5. 각 상태에 맞게 Best Fit을 출력한다

### 상세 설명

1. Bestfit 처리 기준은 사용 가능한 메모리 중, 요구 size보다 크거나 같은것 block중 최소가 되는 것으로 한다.
2. Coalescing은 삭제되는 메모리 앞쪽 뒤쪽에 대해 진행, usable이 양쪽 다 존재할 경우 앞 block 병합 이후 뒤 block 병합
3. Compaction은 총 usable한 memory가 요구 사이즈 이상으로 있으나 각 블럭 size가 모두 요구 size에 부합하지 않을경우 진행
4. Compaction algorithm은 sliding 기법 사용 - 존재하는 process들을 순서대로 모두 address 0 방향으로 밀어넣는다