
# socket_implement

這個專案是使用 C++ 所撰寫的 Socket 程式，實現了簡單的 Client / Server 架構。  
適合用於學習 socket 通訊、thread 處理、訊息傳輸的基本概念與實作。

---

## 📁 檔案說明

| 檔案名稱      | 說明                         |
|---------------|------------------------------|
| `client.cpp`  | 用戶端程式，負責發送請求訊息 |
| `server.cpp`  | 伺服器端程式，負責接收訊息處理 |

---

## 🚀 使用方式

你可以分別在兩個終端機視窗執行 server 和 client：

```bash
# 編譯
g++ server.cpp -o server
g++ client.cpp -o client

# 執行 Server
./server

# 另開終端機視窗執行 Client
./client
```

---

## 🛠 編譯環境

- 編譯器：`g++` (建議使用 `g++ 9+`)
- 作業系統：Linux / WSL / macOS / Windows（需支援 socket）

---

## 📌 備註

- Server 啟動後會在指定 port 等待 Client 連線
- Client 啟動後會主動連線並傳送測試訊息
- 適合用於 Socket 應用程式設計入門練習

---

## 🧑‍💻 作者

WenWLYeh @ [GitHub](https://github.com/av121036)
