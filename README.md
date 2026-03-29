 🛡️ File_Encryption_Program

**File_Encryption_Program**은 C++을 사용하여 고성능 암호화 및 복호화 기능을 제공하는 시스템 프로그래밍 프로젝트입니다. 블록 암호 알고리즘(3DES)를 직접 구현하였으며, 멀티스레딩를 적용했습니다.

## 🚀 Key Features

* **Symmetric-Key Cryptography**: 
    * **DES & 3DES** 알고리즘 구현.
    * **OFB (Output Feedback)** 모드 등을 지원하여 블록 암호의 안전한 운용 가능.
* **Performance Optimization**: 
    * `std::thread`를 활용한 병렬 처리를 통해 대용량 파일 암호화 시 CPU 자원을 효율적으로 사용합니다.
* **System-Level Programming**: 
    * 비트 단위 연산(Bitwise Operations)과 메모리 관리를 최적화하여 암호화 성능을 극대화했습니다.
    * 파일 입출력 스트림을 활용한 안정적인 데이터 핸들링을 구현했습니다.

## 🛠️ Tech Stack

* **Language**: C++ 17 이상
* **Concepts**: Cryptography, Multi-threading, Bit Manipulation, File I/O
* **Environment**: Linux / GCC (C++ 표준 라이브러리 기반)

## 📂 Project Structure

```text
.
├── main.cpp          # 프로그램 진입점 및 CLI 인터페이스
├── include/
│   ├── cryptologic/
│   │   ├── festiel.cpp/.h         # DES feistel 구조
│   │   ├── DES.cpp                # DES 서브 구조 (IP, key scheduling, FP, etc...)
│   │   ├── mode.cpp               # 블럭 암호 운용 모드 (CBC, OFB, CTR, etc...), 3DES 구조
│   │   └──  cipher.cpp            # OFB 모드 선택 (변경 가능), 패리티 비트 set
│   └── interface/
│       ├── consolePrinter.cpp     # 로딩 CLI 메인함수
│       └──  iprogress.h           # 로딩 CLI 가상함수
├── test_case_generator/
│       └── test.cpp               # KB 단위 테스트 텍스트 파일 생성
└── README.md
```

💻 Usage
빌드 방법

[Main Program]: g++ -std=c++17 -pthread main.cpp include/cryptologic/*.cpp include/cryptologic/DES/*.cpp include/interface/consolePrinter.cpp include/interface/iprogress.h

[Test Program]: g++ test.cpp

실행 예시

1. 파일 암호화 (DES-OFB 모드)
./[file_name] -en [1/0] -f [file_path] -k [key] ([key2])

2. 테스트 파일 생성 (KB 단위)
./[file_name] [file_size]

📝 Implementation Details
 * Bit-level Accuracy: DES의 초기 치환(IP), 선택 치환(PC-1, PC-2) 등 복잡한 비트 연산을 정확하게 구현하기 위해 비트셋 연산을 최적화했습니다.
 * Thread-Safe Design: 멀티스레드 환경에서 데이터 경합을 방지하고 작업 처리 속도를 높이기 위해 파일을 블록 단위로 분할하여 병렬로 처리합니다.
 * Padding Mechanism: 데이터 블록 크기를 맞추기 위한 PKCS#7 패딩 기법을 적용하여 가변 길이 파일에 대한 호환성을 확보했습니다.

⚠️ Disclaimer
본 프로그램은 교육 및 연구 목적으로 제작되었으며, 실제 운영 환경에서 매우 민감한 보안 데이터를 다룰 경우에는 검증된 오픈소스 암호화 라이브러리(예: OpenSSL) 사용을 권장합니다.
