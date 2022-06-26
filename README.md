# mginx

<div align="center">
<img src="https://user-images.githubusercontent.com/54441505/171986791-972d5370-8d19-4abc-b98c-4d90a0c3320e.png" width="400" />
</div>

# Directory structure
|diretory|description|
|---|---|
| **webserver** | socket communication part |
| **config**    | parsing config file   |
| **request**   | parsing request   |
| **response**  | generate response  | 
| **cgi**       | run cgi program   |
| **autoindex** | implement autoindex function   |
| **utils**     | util functions, structures   |
| **client**    | client program for test   |

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

- You can test using client program we made:
<img width="682" alt="Screen Shot 2022-06-24 at 7 06 13 PM" src="https://user-images.githubusercontent.com/54441505/175513539-4e9a0ef8-a4a2-4760-9d8d-4ef4db3cd8d0.png">
   
   - you can make request by choosing the options
   - method(GET/POST/DELETE)
   - option(Simple/Bad request/CGI)
   - port(8000/8001)
   
### make tester
- Run webserver program
- Open a new terminal window to run tester
- To run this command in VSC enviroment, you need to follow steps:   
   - Open
      * System Preferences(시스템 환경설정)       
      * Security & Privacy(보안 및 개인 정보 보호)   
      * Privacy(개인 정보 보호)   
      * Accessibility(손쉬운 사용)   
   - Check `Code`
   ![스크린샷 2022-06-04 오후 3 19 58](https://user-images.githubusercontent.com/54441505/171987290-dd6803c0-cef6-4188-b7ba-b597aab34adf.png)
- This setting enables the operation of this command: `@osascript -e 'tell application "System Events" to keystroke "./tester http://localhost:8000" '`
- If you cannot change the settings, follow steps:
   - Open Terminal(location: root of this repo directory) and type:
   ```shell
   > make run
   ```
   - make sure the port(8000, 8001, 8002) you want to test is successfully bound
   - Open another(location: root of this repo directory) Terminal and type:
   ```shell
   > cd test/42_tester
   > ./tester http://localhost:8000
   ```
---

## BASIC Makefile COMMANDS
### make
- compile server program and client program

### make re
- `fclean` && `make` again

### make clean
- remove object files

### make fclean
- remove object files(`make clean`), log file and execute file
 
---
## When all ports fail to bind, kill the port
* Check PID
```shell
> lsof -i :8000
```
* Kill the port using PID
```shell
> kill -9 28423
```

[Kill port](https://codinhood.com/nano/macos/find-kill-proccess-port-macos)

 
