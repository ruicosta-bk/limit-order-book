#include <iostream>
#include "lob/OrderBook.h"

// ============================================================================
// HƯỚNG DẪN MILESTONE 2+: KỊCH BẢN KIỂM THỬ (main.cpp)
// ============================================================================
// Mục tiêu: Đóng vai một sàn giao dịch thu nhỏ, tự tạo ra các tình huống (test cases)
// để chứng minh OrderBook của bạn chạy đúng cả trong trường hợp chuẩn lẫn trường hợp biên.

int main() {
    // BƯỚC 1: Khởi tạo một đối tượng OrderBook
    //   OrderBook book;

    // BƯỚC 2: Kịch bản 1 - Treo lệnh chưa khớp (Resting Orders)
    // - Thử thêm 2 lệnh Bán (Sell) ở các mức giá khác nhau, vd: 105 (qty 10), 102 (qty 5).
    // - Thử thêm 2 lệnh Mua (Buy) giá thấp hơn giá bán, vd: 99 (qty 8), 100 (qty 12).
    // - Gọi book.printOrderBook();
    // -> Kiểm tra kỳ vọng: Best Bid phải là 100, Best Ask phải là 102, Spread = 2.

    // BƯỚC 3: Kịch bản 2 - Kiểm tra FIFO (cùng mức giá)
    // - Thêm thêm một lệnh Mua nữa cũng ở giá 100 (vd: ID khác, qty 5).
    // -> Kiểm tra kỳ vọng: Tổng khối lượng ở mức giá 100 phải là 12 + 5 = 17.
    //    Khi có lệnh Bán giá 100 với qty 10 đập vào, lệnh đặt TRƯỚC (qty 12) phải bị trừ trước!

    // BƯỚC 4: Kịch bản 3 - Khớp qua nhiều mức giá (Price Sweep) & Khớp một phần (Partial Fill)
    // - Thử đặt 1 lệnh Mua với giá 105 và số lượng lớn (vd: qty 12).
    // -> Kiểm tra kỳ vọng:
    //    + Nó phải ăn sạch mức giá 102 (5 đơn vị) trước -> mức giá 102 biến mất khỏi Asks!
    //    + Sau đó ăn tiếp 7 đơn vị ở mức giá 105 -> mức giá 105 còn dư 3 đơn vị.
    //    + In sổ lệnh ra để xác nhận trạng thái cuối cùng hoàn toàn khớp lý thuyết.

    // BƯỚC 5 (Mở rộng): Khi bạn đã nắm vững 4 bước trên, hãy nói với Mentor để chúng ta
    // bổ sung: Cancel Order O(1), xử lý edge case (qty = 0, trùng OrderId), và viết Unit Test!

    return 0;
}
