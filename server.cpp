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
static bool winsock_inited = false;
void socket_global_init(){
if(!winsock_inited){ WSADATA wsa; WSAStartup(MAKEWORD(2,2), &wsa); winsock_inited=true; }
}
void socket_global_cleanup(){ if(winsock_inited){ WSACleanup(); winsock_inited=false; } }
#define CLOSESOCK closesocket
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define INVALID_SOCKET (-1)
#define SOCKET int
#define CLOSESOCK close
void socket_global_init(){}
void socket_global_cleanup(){}
#endif


static std::string recv_until_headers_done(SOCKET fd){
std::string data;
char buf[4096];
while(true){
int n = recv(fd, buf, sizeof(buf), 0);
if(n <= 0) break; // client closed or error
data.append(buf, buf + n);
if(data.find("\r\n\r\n") != std::string::npos) break; // end of headers
if(data.size() > 65536) break; // safety guard
}
return data;
}

int main(int argc, char** argv){
int port = 8080;
if(argc >= 2) port = std::stoi(argv[1]);


socket_global_init();


SOCKET srv = socket(AF_INET, SOCK_STREAM, 0);
if(srv == INVALID_SOCKET){ std::cerr << "socket() failed\n"; socket_global_cleanup(); return 1; }


int opt = 1;
#ifdef _WIN32
setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
#else
setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif


sockaddr_in addr{};
addr.sin_family = AF_INET;
addr.sin_addr.s_addr = htonl(INADDR_ANY);
addr.sin_port = htons((uint16_t)port);


if(bind(srv, (sockaddr*)&addr, sizeof(addr)) < 0){
std::cerr << "bind() failed on port " << port << "\n";
CLOSESOCK(srv); socket_global_cleanup(); return 1;
}


if(listen(srv, 16) < 0){
std::cerr << "listen() failed\n";
CLOSESOCK(srv); socket_global_cleanup(); return 1;
}


std::cout << "Serving on http://127.0.0.1:" << port << "\n";


while(true){
sockaddr_in cli{}; socklen_t clen = sizeof(cli);
SOCKET c = accept(srv, (sockaddr*)&cli, &clen);
if(c == INVALID_SOCKET){ continue; }


std::string req = recv_until_headers_done(c);
std::cout << "\n=== Request ===\n" << req << std::endl;


const std::string body = R"(<html><body><h1>Hello from C++ raw socket!</h1><p>It works 🎉</p></body></html>)";
std::string resp;
resp += "HTTP/1.1 200 OK\r\n";
resp += "Content-Type: text/html; charset=utf-8\r\n";
resp += "Connection: close\r\n";
resp += "Content-Length: " + std::to_string(body.size()) + "\r\n";
resp += "\r\n";
resp += body;


send(c, resp.c_str(), (int)resp.size(), 0);
CLOSESOCK(c);
}


CLOSESOCK(srv);
socket_global_cleanup();
return 0;
}
//打開 server.cpp
//按 Ctrl+Shift+B → 編譯出 server.exe
//然後在terminal執行 server.exe 
//.\server.exe 8080