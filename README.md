# mginx

<div align="center">
<img src="https://user-images.githubusercontent.com/54441505/171986791-972d5370-8d19-4abc-b98c-4d90a0c3320e.png" width="400" />
</div>

# Directory structure

* webserver: socket communication part
* config: parsing config file   
* request: parsing request   
* response: generate response   
* cgi: run cgi program   
* autoindex: implement autoindex function   
* utils: util functions, structures   
* client: client program for test   

# How to run MGINX
0. Clone this repo, and Open the terminal
```shell
> git clone https://github.com/42Mginx/mginx.git
> cd mginx
```
1. Run with Makefile
```shell
> make
> ./webserv ./test/conf/mac_test.conf
```
- or just type like this
```shell
> make run
```
- print example <Port information on successful binding>
```shell
0: 8000
Bind success [8000/3]
0: 8001
Bind success [8001/4]
0: 8002
Bind success [8002/5]
```
- case1: All success

```shell
0: 8000
에러번호48: Address already in use
Could not bind [8000]
0: 8001
Bind success [8001/4]
0: 8002
Bind success [8002/5]
```
- case2: One fail[8000]/ Two success[8001, 8002]

2. Test with browser
- You can test using the successfully bound port.
- type: `localhost:${port}`
```shell
localhost:8000
```
 - You will see a page like this:
<img width="1006" alt="Screen Shot 2022-06-24 at 6 35 39 PM" src="https://user-images.githubusercontent.com/54441505/175508270-7e1f4677-16d2-4e07-ba2c-cd65893d04ee.png">

## RUN COMMANDS
### make run
- Compile to create webserver program
- Run the programs

### make test
- Compile and Run webserver program
- Compile and Run client program

- You can test  

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
