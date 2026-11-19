# Limit Order Book & Matching Engine (C++)

## 1. Giới thiệu dự án (Overview & Motivation)

### 1.1. Bản chất bài toán: Bộ máy tìm kiếm sự đồng thuận về giá và phân bổ tài nguyên tự động
Trong bất kỳ thị trường giao dịch tập trung nào (từ sàn chứng khoán truyền thống như NASDAQ, Sở Giao dịch Hàng hóa CME cho tới các sàn tài sản số hiện đại), câu hỏi cốt lõi luôn là: **Làm thế nào để hàng triệu người mua và người bán với kỳ vọng giá khác nhau có thể tìm thấy điểm đồng thuận công bằng, minh bạch và tức thời?**

**Limit Order Book (Sổ lệnh giới hạn - LOB)** và **Matching Engine (Bộ máy khớp lệnh)** chính là lời giải ở tầng lõi kỹ thuật cho bài toán kinh tế đó:
- **Tìm kiếm sự đồng thuận về giá (*Automated Price Discovery*):** Thị trường luôn tồn tại hai lực đối lập — phe **Mua (Bids)** muốn mua với giá thấp nhưng người trả giá *cao nhất* (`Best Bid`) được ưu tiên đứng đầu; phe **Bán (Asks)** muốn bán với giá cao nhưng người chào giá *thấp nhất* (`Best Ask`) được ưu tiên đứng đầu. Khoảng cách giữa hai mức giá tốt nhất này tạo nên **Bid-Ask Spread**. Ngay tại khoảnh khắc một lệnh mới đi vào làm hai mức giá cắt nhau ($\text{Price}_{\text{Buy}} \ge \text{Price}_{\text{Sell}}$), sự đồng thuận về giá được xác lập và giao dịch tự động diễn ra ở mức giá của lệnh đang nằm chờ trên sổ (*Maker/Resting Order Price*).
- **Phân bổ tài nguyên tự động & công bằng (*Fair Resource Allocation*):** Hệ thống vận hành nghiêm ngặt theo thuật toán **Price-Time Priority (Ưu tiên Giá trước – Thời gian sau / FIFO)**:
  1. **Ưu tiên về Giá (*Price Priority*):** Người mua trả giá cao hơn hoặc người bán chào giá thấp hơn luôn được phân bổ thanh khoản trước.
  2. **Ưu tiên về Thời gian (*Time Priority*):** Tại cùng một mức giá (`LimitLevel`), lệnh nào đến sàn trước sẽ được xếp đầu hàng đợi và khớp trước.
  3. **Xử lý linh hoạt các trạng thái thanh khoản:** Tự động hỗ trợ khớp toàn bộ (*Full Fill*), khớp một phần (*Partial Fill*), quét thanh khoản xuyên qua nhiều tầng giá liên tiếp (*Multi-level Price Sweep*), và tự động treo phần khối lượng còn dư lên sổ lệnh để cung cấp thanh khoản cho các giao dịch kế tiếp.

---

### 1.2. Mục tiêu cá nhân & Giá trị kỹ thuật của dự án (Learning & Engineering Goals)
Dự án này được xây dựng trong quá trình mình là sinh viên năm 2, với mục tiêu vượt ra khỏi các bài tập môn học rời rạc để giải quyết một bài toán hệ thống thực tế có yêu cầu khắt khe về cả **Cấu trúc dữ liệu & Giải thuật (DSA)** lẫn **Kỹ thuật lập trình C++ (Modern C++ Engineering)**:

1. **Chuyển hóa kiến thức DSA từ lý thuyết sang hệ thống thực chiến:**
   - Hiểu sâu lý do **Cây Tìm Kiếm Nhị Phân Cân Bằng (Balanced BST / Red-Black Tree — `std::map`)** được dùng để quản lý các tầng giá (`LimitLevel`), giúp duy trì thứ tự giá tự động khi thêm/xóa tầng giá trong $O(\log M)$ và truy xuất giá tốt nhất (`Best Bid` / `Best Ask`) ở đầu cây chỉ trong $O(1)$.
   - Kết hợp **Danh sách liên kết đôi (Doubly Linked List — `std::list`)** làm hàng đợi FIFO tại mỗi mức giá để vừa thêm/xóa đầu hàng trong $O(1)$, vừa giữ **tính ổn định của Iterator (*Iterator Stability*)**.
   - Tiến tới kết hợp **Bảng băm (`std::unordered_map`) + Iterator** để giải bài toán hủy lệnh bất kỳ ở giữa hàng đợi (`Cancel Order`) trong thời gian trung bình $O(1)$ thay vì duyệt tuyến tính $O(N)$.

2. **Rèn luyện tư duy và chuẩn mực code C++ chuyên nghiệp:**
   - **Thiết kế dữ liệu tài chính an toàn:** Loại bỏ hoàn toàn kiểu số thực dấu phẩy động (`float`/`double`) khỏi logic giá và khối lượng để tránh sai số làm tròn chuẩn IEEE 754; thay bằng số nguyên kích thước cố định 64-bit (`std::uint64_t`) biểu diễn đơn vị giá nhỏ nhất (*ticks / cents*).
   - **Kiến trúc code sạch và an toàn kiểu (*Type Safety & Clean Architecture*):** Tách biệt rõ ràng giữa khai báo giao diện (`.h`) và cài đặt chi tiết (`.cpp`), không làm ô nhiễm không gian tên (`using namespace std;`) trong header, sử dụng `enum class`, Type Aliases (`using`), Member Initializer List, `const`-correctness và quản lý chặt chẽ tham chiếu/vòng đời bộ nhớ (tránh *Dangling Reference* và *Iterator Invalidation* khi xóa node trên cây).

3. **Định hướng Mã nguồn mở (Open-Source) & Phát triển nghề nghiệp:**
   - Dự án được công khai trên **GitHub** vừa làm hồ sơ năng lực kỹ thuật (Portfolio) phản ánh quá trình tự học, tư duy tối ưu hóa và khả năng viết code C++ có cấu trúc rõ ràng, vừa là một nền tảng mở để cộng đồng và các bạn sinh viên cùng quan tâm tới mảng **C++ / Systems / FinTech / Quantitative Trading** có thể đóng góp, thảo luận và mở rộng thêm nhiều tính năng nâng cao (như lệnh `Market`, `IOC`, `FOK`, tối ưu Cache Locality với *Object Pool / Intrusive Linked List*, và đo đạc Benchmark thông lượng xử lý lệnh).

## 2. Cấu trúc dữ liệu & Độ phức tạp (Data Structures & Complexity)
- **Quản lý các mức giá (Price Levels):** Dùng `std::map` (Red-Black Tree) -> Tìm Best Bid/Ask trong $O(1)$ (qua `begin()`), thêm/xóa mức giá trong $O(\log M)$ với $M$ là số mức giá.
- **Hàng đợi lệnh tại mỗi mức giá:** Dùng `std::list` -> Đảm bảo FIFO và hỗ trợ xóa phần tử bất kỳ khi có iterator trong $O(1)$.
- *(Phase 3 sẽ cập nhật thêm: Hash Map cho thao tác hủy lệnh $O(1)$).*

## 3. Cách biên dịch và chạy (Build & Run)
Sử dụng **CMake** (hỗ trợ mọi compiler: `g++`, `clang++`, `MSVC`):
```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
./build/lob_cli
```
*(Nếu không cài Ninja, bỏ cờ `-G Ninja` để dùng generator mặc định của hệ thống. Xem thêm quy trình làm việc nhóm tại [CONTRIBUTING.md](CONTRIBUTING.md)).*

## 4. Lộ trình phát triển (Roadmap)
- [ ] Milestone 1: Thiết kế `Order`, `LimitLevel`, `OrderBook`
- [ ] Milestone 2: Cài đặt Matching Engine (Price-Time Priority)
- [ ] Milestone 3: Hỗ trợ Hủy lệnh (`Cancel Order`) tối ưu $O(1)$
- [ ] Milestone 4: Unit Tests & Benchmark hiệu năng
