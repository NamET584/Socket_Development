
Dự án minh họa cơ chế truyền nhiều file tuần tự từ Client lên Server bằng Socket TCP trên Linux.

🛠 Công nghệ & Cấu trúc
Ngôn ngữ: C++17 (Linux).

Thành phần:

Server: Nhận file, lưu với tiền tố received_, gửi phản hồi (ACK).

Client: Chọn file từ thư mục files/ và gửi dữ liệu.

Shared: Hàm send_all/recv_all chống mất mát gói tin (short read/write).

📑 Giao thức truyền (Framing Protocol)
Do TCP là dạng stream, mỗi file được đóng gói theo cấu trúc:

Filename: Chuỗi ký tự kết thúc bằng \n.

Filesize: 8 bytes (uint64_t) định dạng Big-endian.

File Data: Nội dung file (chia nhỏ 64KB/chunk).

Kết thúc: Gửi frame đặc biệt END\n + size 0.

🔄 Luồng hoạt động
Kết nối: Client kết nối tới Server (Port 8083).

Truyền tải: Client gửi từng file -> Server lưu file -> Server gửi OK hoặc ERR.

Tổng kết: Sau khi nhận END, Server gửi tổng số file đã nhận thành công.

💻 Cách chạy nhanh
Bash
# 1. Build project
chmod +x build.sh && ./build.sh

# 2. Chạy Server (Terminal 1)
./install/server

# 3. Chạy Client (Terminal 2)
./install/client
# Chọn số thứ tự file hoặc gõ 'all' để gửi
Đặc điểm nổi bật: Hỗ trợ gửi nhiều file cùng lúc, xử lý lỗi mạng bằng buffer size lớn (64KB), tương thích kiến trúc CPU nhờ Big-endian.
