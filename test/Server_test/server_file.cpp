#include "Mginx.hpp"

int main(){
    // [setting]
    long max_fd = 0;
    
    // fd_set 필요
    fd_set			_fd_set;

    // fd set 초기화
    FD_ZERO(&_fd_set);

    // 1. host, port 정보를 파싱된 형태로 받아옴
    // t_listen listen_info;
    // listen_info.host = 0;
    // listen_info.port = 8000;

    // std::vector<t_listen> listens_v;
    // listens_v.push_back(listen_info); 
    // listens_v.push_back(listen_info2); 

    // std::vector<t_listen> socket_fd_v;
  

    // ===========
    // server listens setting
  std::vector<t_listen>::const_iterator listen_iter_v = listens_v.begin();
	for ( listen_iter_v ; listen_iter_v != listens_v.end() ; listen_iter_v++ ){

    // socket 생성 => 반환값: socket fd
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    // 구조체 초기화 (addr)
    struct sockaddr_in socket_addr;
    int addrlen = sizeof(socket_addr);

    socket_addr.sin_family = AF_INET;        
    socket_addr.sin_addr.s_addr = listen_info.host || INADDR_ANY;  // ip가 있으면 ip 넣어줌
    socket_addr.sin_port = htons(listen_info.port);   

   // 구조체 bind
    bind(socket_fd, (struct sockaddr *)&socket_addr,sizeof(socket_addr)); // 

    socket_fd_v.push_back();
  }

  

    

 
    
    // listen
    listen(socket_fd, 1000);

    // ============
    // 에러 안나면 socket fd를 fd_set에서 켜줌
    FD_SET(socket_fd, &_fd_set);

    // max fd 갱신
    if (socket_fd > max_fd)
		max_fd = socket_fd;

    std::cout <<  "setting 완료" <<  std::endl;

    // run ============
    // run을 위한 준비과정
    fd_set _reading_set; // 
    fd_set _writing_set; // 
    struct timeval	timeout;
    timeout.tv_sec  = 10;
	  timeout.tv_usec = 0;

    // fd_set => _reding_set, _writing_set 초기화
    while(1){
    
    int ret = 0;
    FD_ZERO(&_writing_set); 
 
    while(ret == 0){
        memcpy(&_reading_set, &_fd_set, sizeof(_fd_set));
        ret = select(max_fd + 1, &_reading_set, NULL, NULL, &timeout); // WRITING SET 설정필요함
        std::cout << "기다리는 중입니다." << ret << std::endl; // -1이면 에러임
    }
    
    std::cout << ret << std::endl; // -1이면 에러임 // 에러처리 필요

    char				buffer[BUF_SIZE] = {0, };
    long connected_fd = accept(socket_fd, NULL, NULL); // 이제 데이터를 주고 받을 준비가 됨

    recv(connected_fd, buffer, BUF_SIZE - 1, 0); // 사실상 read랑 같음
    std::cout << buffer << std::endl;
    send(connected_fd, "hello world form server", 30, 0);
    }

    // =================

}