#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include "shared.h"

void handle_client(int client_socket) {
    // ── 1. Nhận tên file (kết thúc bằng '\n') ─────────────────
    std::string filename;
    char c;
    while (recv(client_socket, &c, 1, 0) > 0 && c != '\n')
        filename += c;

    // ── 2. Nhận kích thước file (8 bytes, uint64_t) ────────────
    uint64_t filesize = 0;
    recv(client_socket, &filesize, sizeof(filesize), MSG_WAITALL);
    filesize = be64toh(filesize); // đổi về byte order của máy

    std::cout << "Nhan file: " << filename
              << " (" << filesize << " bytes)\n";

    // ── 3. Nhận nội dung file và lưu xuống ────────────────────
    std::ofstream out("received_" + filename, std::ios::binary);
    char buffer[BUFFER_SIZE];
    uint64_t received = 0;

    while (received < filesize) {
        uint64_t remain = filesize - received;
        int to_read = (remain < BUFFER_SIZE) ? remain : BUFFER_SIZE;
        int bytes = recv(client_socket, buffer, to_read, 0);
        if (bytes <= 0) break;
        out.write(buffer, bytes);
        received += bytes;
    }

    // ── 4. Phản hồi client ─────────────────────────────────────
    if (received == filesize) {
        std::cout << "Luu thanh cong: received_" << filename << "\n";
        const char* ok = "DONE: da luu file thanh cong";
        send(client_socket, ok, strlen(ok), 0);
    } else {
        const char* err = "ERROR: nhan file that bai";
        send(client_socket, err, strlen(err), 0);
    }

    close(client_socket);
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(PORT);

    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 5);
    std::cout << "Server cho nhan file tren cong " << PORT << "\n";

    while (true) {
        int client_sock = accept(server_fd, nullptr, nullptr);
        handle_client(client_sock);
    }
}