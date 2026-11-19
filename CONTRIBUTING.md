# Hướng dẫn Làm việc nhóm (Fork + Pull Request Workflow)

Dự án áp dụng quy trình **Fork + Pull Request** chuẩn Open-Source để rèn kỹ năng làm việc nhóm thực tế và học sâu bản chất qua việc review code lẫn nhau.

## 1. Thiết lập lần đầu

### Với người sở hữu repo gốc (`ruicosta-bk`)
- Repo chính là `https://github.com/ruicosta-bk/limit-order-book.git` (`origin`).
- **Không code trực tiếp trên `main`.** Luôn tạo nhánh `feat/...` -> push lên `origin` -> mở Pull Request vào `main` và nhờ bạn kia review trước khi merge.

### Với thành viên tham gia qua Fork
1. Bấm nút **Fork** trên trang `https://github.com/ruicosta-bk/limit-order-book` về tài khoản GitHub của bạn.
2. Clone bản Fork về máy và gắn thêm `upstream` trỏ về repo gốc:
   ```bash
   git clone https://github.com/<username-cua-ban>/limit-order-book.git
   cd limit-order-book
   git remote add upstream https://github.com/ruicosta-bk/limit-order-book.git
   git remote -v
   ```

## 2. Chu trình làm một tính năng mới

1. **Đồng bộ nhánh `main` mới nhất từ repo gốc:**
   ```bash
   git checkout main
   git fetch upstream          # (Người giữ repo gốc dùng: git pull origin main)
   git merge upstream/main
   git push origin main
   ```
2. **Tạo nhánh riêng cho đầu việc (1 PR = 1 mục tiêu rõ ràng):**
   ```bash
   git checkout -b feat/p1-core-api
   ```
   Quy ước tiền tố nhánh: `feat/`, `fix/`, `refactor/`, `test/`, `perf/`, `docs/`.
3. **Code, build và chạy test bắt buộc xanh trên máy:**
   ```bash
   cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
   cmake --build build
   ctest --test-dir build --output-on-failure
   ```
4. **Commit theo Conventional Commits:**
   - `feat: add Trade struct and return execution reports from addOrder`
   - `fix: remove filled resting order from orderIndex to avoid dangling iterator`
   - `test: add FIFO and multi-level sweep unit tests`
5. **Push nhánh lên Fork và mở Pull Request vào `ruicosta-bk/limit-order-book:main`:**
   ```bash
   git push -u origin feat/p1-core-api
   ```

## 3. Checklist Review chéo trước khi Merge PR

Người review kiểm tra 4 tiêu chí sau trước khi bấm **Approve & Merge**:
- [ ] **Build & CI xanh:** Không có compiler warning (`-Wall -Wextra -Wpedantic`), toàn bộ test trong `ctest` pass 100%.
- [ ] **Độ phức tạp đúng bản chất:** Có thao tác nào vô tình rơi xuống $O(N)$ thay vì $O(1)$ / $O(\log M)$ không?
- [ ] **An toàn bộ nhớ C++:** Có nguy cơ *Dangling Reference* sau `erase()`, *Iterator Invalidation*, hoặc quên xóa khỏi `unordered_map` không?
- [ ] **Trường hợp biên (Edge Cases):** Đã kiểm tra `quantity == 0`, sổ trống, khớp sạch 1 tầng giá, trùng `OrderId` chưa?
