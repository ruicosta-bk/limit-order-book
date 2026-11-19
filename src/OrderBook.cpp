#include "lob/OrderBook.h"
#include <iostream>
#include <algorithm> // Gợi ý: dùng std::min khi tính khối lượng khớp giữa 2 lệnh

// ============================================================================
// HƯỚNG DẪN MILESTONE 2: CÀI ĐẶT LOGIC MATCHING ENGINE (OrderBook.cpp)
// ============================================================================
// Lưu ý cú pháp C++: Khi định nghĩa hàm của class ở file .cpp, luôn nhớ gắn
// tiền tố `OrderBook::` trước tên hàm (ví dụ: `void OrderBook::addOrder(...)`).

// ----------------------------------------------------------------------------
// 1. Cài đặt `addOrder` (và hàm helper `matchOrder` nếu bạn tách riêng)
// ----------------------------------------------------------------------------
// Tư duy thuật toán Price-Time Priority:
//   Bước A: Copy lệnh đầu vào thành một biến tạm có thể sửa đổi (vd: `Order incoming = order;`),
//           vì `incoming.remainingQuantity` sẽ giảm dần trong lúc khớp.
//   Bước B: Thử khớp lệnh với phe ĐỐI DIỆN:
//     - Nếu incoming.side == Side::Buy -> đối chiếu với `asks_`:
//       + Vòng lặp chạy khi: `!asks_.empty()` VÀ `incoming.remainingQuantity > 0`
//         VÀ giá của incoming >= giá tốt nhất bên bán (`asks_.begin()->first`).
//       + Lấy tham chiếu tới mức giá tốt nhất: `auto bestAskIt = asks_.begin();`
//         `LimitLevel& level = bestAskIt->second;`
//       + Lặp qua hàng đợi `level.orders` khi `!level.orders.empty()` và `incoming.remainingQuantity > 0`:
//         * Lấy lệnh đứng đầu hàng đợi: `Order& restingOrder = level.orders.front();`
//         * Tính lượng khớp: `Quantity tradedQty = std::min(incoming.remainingQuantity, restingOrder.remainingQuantity);`
//         * Trừ `tradedQty` khỏi `incoming.remainingQuantity`, `restingOrder.remainingQuantity`,
//           và `level.totalVolume`.
//         * In log giao dịch vừa khớp (ví dụ: [TRADE] Buy #id <-> Sell #id | Qty | Price).
//         * Nếu `restingOrder.remainingQuantity == 0` -> xóa khỏi đầu hàng: `level.orders.pop_front();`
//       + CỰC KỲ QUAN TRỌNG (Bẫy C++): Sau vòng lặp bên trong, kiểm tra nếu `level.orders.empty()`
//         thì xóa mức giá này khỏi cây: `asks_.erase(bestAskIt);`
//         (Không dùng lại biến `level` sau khi đã `erase(bestAskIt)` vì nó sẽ thành dangling reference!)
//     - Nếu incoming.side == Side::Sell -> đối xứng hoàn toàn với `bids_`:
//       + Điều kiện giá khớp: `incoming.price <= bids_.begin()->first`.
//
//   Bước C: Sau khi đã cố gắng khớp hết mức có thể, nếu `incoming.remainingQuantity > 0`:
//     - Đưa phần còn dư của `incoming` vào sổ (phe của chính nó: Buy -> `bids_`, Sell -> `asks_`):
//       + Tìm hoặc tự động tạo `LimitLevel` tại `book[incoming.price]` (chú ý gán `level.price = incoming.price`
//         nếu mức giá đó mới được tạo lần đầu!).
//       + Cộng `incoming.remainingQuantity` vào `level.totalVolume`.
//       + Đẩy `incoming` vào cuối hàng đợi: `level.orders.push_back(incoming);`

// ----------------------------------------------------------------------------
// 2. Cài đặt `getBestBid() const` và `getBestAsk() const`
// ----------------------------------------------------------------------------
// - Nếu cây rỗng (`empty()`) -> trả về `std::nullopt`.
// - Nếu không rỗng -> trả về `begin()->first` (chính là key Price đứng đầu cây).

// ----------------------------------------------------------------------------
// 3. Cài đặt `cancelOrder(OrderId id)`
// ----------------------------------------------------------------------------
// - Giai đoạn đầu (Phase 2): Tạm trả về `false` (chưa cài) hoặc duyệt tìm thử để cảm nhận
//   vì sao tìm không có Hash Map sẽ chậm O(N).
// - Sang Phase 3: Ta sẽ nâng cấp lên O(1) tìm kiếm bằng `std::unordered_map` + iterator!

// ----------------------------------------------------------------------------
// 4. Cài đặt `printOrderBook() const`
// ----------------------------------------------------------------------------
// - Gợi ý bố cục in chuẩn giống bảng điện tử chứng khoán/crypto:
//   + Phần trên (ASKS - Bán): In từ giá CAO xuống giá THẤP để mức giá bán rẻ nhất (Best Ask)
//     nằm sát đường kẻ giữa nhất.
//     Mẹo STL: Vì `asks_` đang xếp tăng dần, hãy dùng reverse iterator (`asks_.rbegin()` tới `asks_.rend()`)!
//   + Đường kẻ giữa: hiển thị Spread (chênh lệch = Best Ask - Best Bid, nếu cả hai phe đều có lệnh).
//   + Phần dưới (BIDS - Mua): In bình thường từ `bids_.begin()` tới `bids_.end()` (từ cao xuống thấp).
