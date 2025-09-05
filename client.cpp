#include <iostream>
#include <string>
#include <vector>
#include <cstring>


#ifdef _WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
using socklen_t = int;
static bool winsock_inited2 = false;
void socket_global_init2(){ if(!winsock_inited2){ WSADATA wsa; WSAStartup(MAKEWORD(2,2), &wsa); winsock_inited2=true; } }
void socket_global_cleanup2(){ if(winsock_inited2){ WSACleanup(); winsock_inited2=false; } }
#define CLOSESOCK2 closesocket
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define INVALID_SOCKET (-1)
#define SOCKET int
#define CLOSESOCK2 close
void socket_global_init2(){}
void socket_global_cleanup2(){}
#endif

static bool send_all(SOCKET fd, const char* data, size_t len){
size_t sent = 0;
while(sent < len){
int n = send(fd, data + sent, (int)(len - sent), 0);
if(n <= 0) return false;
sent += (size_t)n;
}
return true;
}


int main(int argc, char** argv){
const char* host = "127.0.0.1";
int port = 8080;
const char* path = "/";
if(argc >= 2) host = argv[1];
if(argc >= 3) port = std::stoi(argv[2]);
if(argc >= 4) path = argv[3];


socket_global_init2();


SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
if(s == INVALID_SOCKET){ std::cerr << "socket() failed\n"; socket_global_cleanup2(); return 1; }


sockaddr_in addr{};
addr.sin_family = AF_INET;
addr.sin_port = htons((uint16_t)port);
#ifdef _WIN32
addr.sin_addr.s_addr = inet_addr(host); // simple: host must be IPv4 literal
#else
addr.sin_addr.s_addr = inet_addr(host);
#endif


if(connect(s, (sockaddr*)&addr, sizeof(addr)) < 0){
std::cerr << "connect() failed to " << host << ":" << port << "\n";
CLOSESOCK2(s); socket_global_cleanup2(); return 1;
}


std::string req;
req += "GET "; req += path; req += " HTTP/1.1\r\n";
req += "Host: "; req += host; req += ":" + std::to_string(port) + "\r\n";
req += "User-Agent: cpp-raw-socket/0.1\r\n";
req += "Accept: */*\r\n";
req += "Connection: close\r\n\r\n";


if(!send_all(s, req.c_str(), req.size())){
std::cerr << "send() failed\n"; CLOSESOCK2(s); socket_global_cleanup2(); return 1;
}


std::string resp;
char buf[4096];
while(true){
int n = recv(s, buf, sizeof(buf), 0);
if(n <= 0) break;
resp.append(buf, buf + n);
}


std::cout << resp << std::endl;


CLOSESOCK2(s);
socket_global_cleanup2();
return 0;
}
//打開 client.cpp
//再按 Ctrl+Shift+B → 編譯出 client.exe
//當server.exe 正在執行時，在terminal執行 client.exe
//.\client.exe 127.0.0.1 8080
//也可以用瀏覽器開啟 http://127.0.0.1:8080