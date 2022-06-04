# mginx

<div align="center">
<img src="https://user-images.githubusercontent.com/54441505/171986791-972d5370-8d19-4abc-b98c-4d90a0c3320e.png" width="400" />
</div>


# 주요 폴더

### webserver
- 웹 서버의 통신부
### config
- 웹 서버 설정 파일 파싱 및 구조화
### request
- 클라이언트로부터 온 요청을 파싱
### response
- 클라이언트에게 보낼 응답을 생성
### cgi
- cgi작업이 필요한 경우 프로그램을 작동시켜 응답에 변경을 가함
### autoindex
- 오토인덱스 구현
### utils
- 유틸함수 및 구조체
### client
- 테스트를 위한 클라이언트

# 실행방법
* Makefile을 이용해 실행합니다.

## RUN COMMANDS
### make run
- run webserv
- 컴파일하여 웹 서브 프로그램을 만들고 동작시킵니다.

### make test
- run webserv && run client
- 웹 서브 프로그램을 만들고 동작시킵니다.
- 클라이언트 또한 동작시켜 다른 터미널에서 띄웁니다.

### make tester
- run webserv && open new terminal window(for running 42tester)
- 웹 서브를 동작시킨 후에 테스터를 작동킬 수 있는 명령어가 타이핑된 터미널을 띄웁니다.
- VSC 환경에서 이 명령어를 실행하기 위해서는 "시스템 환경설정 > 보안 및 개인 정보 보호 > 개인 정보 보호 > 손쉬운 사용" 탭에서 Code를 체크해야 합니다.
   ![스크린샷 2022-06-04 오후 3 19 58](https://user-images.githubusercontent.com/54441505/171987290-dd6803c0-cef6-4188-b7ba-b597aab34adf.png)
   - `@osascript -e 'tell application "System Events" to keystroke "./tester http://localhost:8000" '`가 동작하도록 하기 위해서 입니다.


---

## 기본 Makefile 명령어 설명 | BASICS COMMANDS
### make
- compile server, client
- 서버와 클라이언트를 컴파일합니다.

### make re
- fclean && make again
- make 명령어를 통해 생성된 모든 파일을 삭제하고 다시 make합니다.

### make clean
- remove object files
- make 명령어를 통해 생성된 오브젝트 파일을 삭제합니다.

### make fclean
- remove object files, log file and execute file
- 오브젝트 파일, 실행파일, 로그 파일을 삭제합니다.
- 내부적으로 make clean 명령을 사용합니다.
