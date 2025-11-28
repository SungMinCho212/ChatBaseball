# ChatBaseball - 숫자 야구 멀티플레이어 게임

언리얼 엔진 C++로 구현한 턴제 멀티플레이어 숫자 야구 게임

## 게임 규칙

- 서버가 1~9 사이 중복 없는 3자리 숫자 생성
- 플레이어별 최대 3회 시도, 턴당 30초 제한
- Strike(S): 숫자와 자리 일치 / Ball(B): 숫자만 일치 / Out: 불일치
- 3S 달성시 승리, 모두 기회 소진시 무승부
- 턴 시간 내 미시도시 자동 기회 소진

## 구현 클래스

### CBGameModeBase
- 정답 숫자 생성 및 검증
- S/B/Out 판정 로직
- 턴 시스템 관리 (순환, 시간 제한)
- 승리/무승부 판정 및 게임 리셋

### CBPlayerState
- 시도 횟수 관리 (현재/최대)
- 승리 여부, 턴 상태 관리
- 네트워크 리플리케이션 (Replicated, ReplicatedUsing)

### CBPlayerController
- RPC 통신: `ServerRPCProcessChatMessage`, `ClientRPCPrintChatMessageString`
- UI 위젯 생성 및 관리

### CBChatInput (UI)
- 채팅 입력 위젯 (`EditableTextBox_ChatInput`)

### CBTurnTimer (UI)
- 턴 타이머 표시 (`TextBlock_Timer`, `TextBlock_CurrentPlayer`)
- 5초 이하시 빨간색 경고

## 네트워크 구조

**Client → Server → All Clients**
```
클라이언트: 채팅 입력
→ ServerRPC: 서버로 전송
→ GameMode: 게임 로직 처리
→ ClientRPC: 모든 클라이언트에 결과 브로드캐스트
```

**Replication**
- PlayerState의 시도 횟수, 턴 상태 등이 서버→클라이언트 자동 동기화

## 설정 방법

### 1. 빌드
Visual Studio에서 `ChatBaseball.sln` 빌드

### 2. 에디터 설정
- Project Settings → Maps & Modes
  - Default GameMode: `CBGameModeBase`
  - Player Controller: `CBPlayerController`
  - Player State: `CBPlayerState`

### 3. 위젯 블루프린트 생성

**WBP_ChatInput** (Parent: CBChatInput)
- Editable Text Box 추가, 이름: `EditableTextBox_ChatInput`

**WBP_TurnTimer** (Parent: CBTurnTimer)
- Text Block 추가, 이름: `TextBlock_CurrentPlayer`
- Text Block 추가, 이름: `TextBlock_Timer`

### 4. PlayerController BP 설정
- `CBPlayerController` 기반 BP 생성
- Chat Input Widget Class: WBP_ChatInput
- Turn Timer Widget Class: WBP_TurnTimer

### 5. 멀티플레이어 실행
```
Play 설정:
- Number of Players: 2 이상
- Net Mode: Play As Listen Server
```

## 주요 기능

### 필수 기능
- ✅ 멀티플레이어 채팅 (RPC)
- ✅ 정답 숫자 생성 (1~9, 중복 없음)
- ✅ 입력 검증 (3자리, 숫자만, 중복 없음)
- ✅ S/B/Out 계산
- ✅ 시도 횟수 관리 및 표시
- ✅ 승리/무승부 판정
- ✅ 게임 자동 리셋

### 도전 기능
- ✅ 턴 제어 시스템 (플레이어 순환)
- ✅ 턴당 30초 시간 제한
- ✅ 턴 타이머 UI
- ✅ 시간 초과시 자동 기회 소진
- ✅ 실시간 시간 표시 (5초 이하 빨간색)

## 파일 구조

```
Source/ChatBaseball/
├── Game/
│   ├── CBGameModeBase.h/cpp
├── Player/
│   ├── CBPlayerController.h/cpp
│   └── CBPlayerState.h/cpp
└── UI/
    ├── CBChatInput.h/cpp
    └── CBTurnTimer.h/cpp
```

## 개발 환경

- Unreal Engine 5
- C++ (Visual Studio)
- 네트워크: Client-Server 구조
