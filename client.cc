#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <filesystem>
#include "shared.h"

namespace fs = std::filesystem;

// ── Quét file .txt và .docx trong thư mục hiện tại ────────────
std::vector<std::string> scan_local_files() {
    std::vector<std::string> files;

    for (const auto& entry : fs::directory_iterator("/home/hoainam0508/Documents/BTL_MMT/files")) {
        if (!entry.is_regular_file()) continue;
        std::string ext = entry.path().extension().string();
        if (ext == ".txt" || ext == ".docx")
            files.push_back(entry.path().filename().string());
    }
    return files;
}

int main() {
    // ── 1. Quét và hiển thị file có thể gửi ───────────────────
    std::vector<std::string> files = scan_local_files();

    if (files.empty()) {
        std::cout << "Khong tim thay file .txt hoac .docx nao!\n";
        return 1;
    }

    std::cout << "\n===== FILE CO THE GUI LEN SERVER =====\n";
    for (int i = 0; i < (int)files.size(); i++)
        std::cout << "  [" << i + 1 << "] " << files[i] << "\n";
    std::cout << "======================================\n";

    // ── 2. Người dùng chọn file ───────────────────────────────
    int choice = 0;
    while (choice < 1 || choice > (int)files.size()) {
        std::cout << "Chon file de gui (1-" << files.size() << "): ";
        std::cin >> choice;
    }
    std::string filename = files[choice - 1];

    // ── 3. Kết nối tới server ─────────────────────────────────
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &addr.sin_addr);

    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Khong the ket noi toi server!\n";
        return 1;
    }

    // ── 4. Gửi tên file (kết thúc bằng '\n') ─────────────────
    std::string header = filename + "\n";
    send(sock, header.c_str(), header.size(), 0);

    // ── 5. Gửi kích thước file (8 bytes) ─────────────────────
    std::string filepath = "/home/hoainam0508/Documents/BTL_MMT/files/" + filename;
    uint64_t filesize = fs::file_size(filepath);
    uint64_t filesize_net = htobe64(filesize); // đổi sang network byte order
    send(sock, &filesize_net, sizeof(filesize_net), 0);

    // ── 6. Gửi nội dung file ──────────────────────────────────
    std::ifstream in(filepath, std::ios::binary);
    char buffer[BUFFER_SIZE];
    uint64_t sent = 0;

    std::cout << "Dang gui " << filename << "...\n";
    while (in.read(buffer, sizeof(buffer)) || in.gcount() > 0) {
        int bytes = in.gcount();
        send(sock, buffer, bytes, 0);
        sent += bytes;
        // Hiển thị tiến độ đơn giản
        std::cout << "\r  " << sent << " / " << filesize << " bytes" << std::flush;
    }
    std::cout << "\n";

    // ── 7. Nhận phản hồi từ server ────────────────────────────
    char response[256] = {0};
    recv(sock, response, sizeof(response), 0);
    std::cout << "Server: " << response << "\n";

    close(sock);
    return 0;
}