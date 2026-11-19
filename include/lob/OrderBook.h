#pragma once

// ============================================================================
// HƯỚNG DẪN MILESTONE 1: THIẾT KẾ CẤU TRÚC DỮ LIỆU CỐT LÕI (OrderBook.h)
// ============================================================================
// Mục tiêu file này: Khai báo đầy đủ "hình dáng" dữ liệu và giao diện hàm,
// CHƯA viết phần thân xử lý chi tiết (phần đó sẽ nằm ở OrderBook.cpp).

// BƯỚC 1: Include các thư viện chuẩn (STL) cần thiết
// - <cstdint>    : để dùng kiểu số nguyên cố định kích thước (uint64_t, int64_t)
// - <map>        : cấu trúc Cây Đỏ-Đen (Red-Black Tree) lưu các mức giá có thứ tự
// - <list>       : Danh sách liên kết đôi (Doubly Linked List) làm hàng đợi lệnh
// - <functional> : để dùng std::greater cho cây Bids (sắp xếp giá giảm dần)
// - <optional>   : (C++17) dùng cho hàm getBestBid() / getBestAsk() khi sổ trống
// -> Hãy viết các dòng #include ở ngay dưới đây:
#include <cstdint>
#include <map>
#include <list>
#include <functional>
#include <algorithm>
// BƯỚC 2: Định nghĩa Type Aliases bằng từ khóa `using` (chuẩn C++ hiện đại)
// - Tạo các tên kiểu rõ nghĩa: OrderId, Price, Quantity (đều dựa trên uint64_t).
// - Tư duy quan trọng: Tại sao trong hệ thống giao dịch KHÔNG dùng `double` cho Price?
//   Vì `double` bị sai số dấu phẩy động (vd: 0.1 + 0.2 != 0.3). Thay vào đó, người ta
//   lưu giá bằng số nguyên ở đơn vị nhỏ nhất (vd: 100.50$ -> 10050 cents).
// -> Hãy khai báo 3 type alias (OrderId, Price, Quantity) ở dưới đây:
using OrderId = std::uint64_t;
using Price = std::uint64_t;
using Quantity = std::uint64_t;

// BƯỚC 3: Định nghĩa phe của lệnh bằng `enum class Side`
// - Gồm 2 giá trị: Buy và Sell.
// - Tư duy C++: Tại sao nên dùng `enum class` thay vì `enum` thường hoặc `bool`?
//   (`enum class` tránh xung đột tên và tránh truyền nhầm kiểu ngầm định).
// -> Hãy khai báo `enum class Side` ở dưới đây:
enum class Side
{
    Buy,
    Sell
};
// BƯỚC 4: Thiết kế `struct Order` (Đại diện cho 1 lệnh đặt mua/bán)
// - Gợi ý các thuộc tính tối thiểu:
//   + id               : OrderId (mã định danh duy nhất của lệnh)
//   + side             : Side (Buy hay Sell)
//   + price            : Price (mức giá muốn giao dịch)
//   + initialQuantity  : Quantity (số lượng đặt ban đầu - hữu ích để in log/lịch sử)
//   + remainingQuantity: Quantity (số lượng còn lại chưa khớp - thay đổi khi khớp 1 phần)
// - Mẹo C++: Bạn có thể viết thêm một constructor cho Order hoặc dùng
//   "member initializer" / aggregate initialization khi tạo Order.
// -> Hãy viết `struct Order` ở dưới đây:
struct Order
{
    OrderId id;
    Side side;
    Price price;
    Quantity initialQuantity;
    Quantity remainingQuantity;

    Order(OrderId id, Side side, Price price, Quantity quantity) : id(id), side(side), price(price), initialQuantity(quantity), remainingQuantity(quantity) {}
};

// BƯỚC 5: Thiết kế `struct LimitLevel` (Đại diện cho TẤT CẢ lệnh tại CÙNG 1 mức giá)
// - Nguyên tắc Price-Time Priority: Tại cùng một mức giá, lệnh vào trước khớp trước (FIFO).
// - Gợi ý các thuộc tính:
//   + price      : Price (mức giá của tầng này)
//   + totalVolume: Quantity (tổng remainingQuantity của tất cả lệnh đang chờ ở mức giá này;
//                  lưu sẵn giúp xem thanh khoản mức giá trong O(1) thay vì duyệt cả list)
//   + orders     : std::list<Order> (hàng đợi FIFO; chọn std::list vì sau này Phase 3
//                  khi xóa lệnh ở giữa danh sách ta cần iterator không bị vô hiệu hóa)
// -> Hãy viết `struct LimitLevel` ở dưới đây:
struct LimitLevel
{
    Price price = 0;
    Quantity totalVolume = 0;
    std::list<Order> orders;
};

// BƯỚC 6: Thiết kế `class OrderBook`
// - Phần `private:`:
//   + bids_ : std::map<Price, LimitLevel, std::greater<Price>>
//             (Vì sao dùng std::greater? Để mức giá MUA CAO NHẤT luôn nằm ở bids_.begin()!)
//   + asks_ : std::map<Price, LimitLevel>
//             (Mặc định std::less: mức giá BÁN THẤP NHẤT luôn nằm ở asks_.begin()!)
//   + (Ghi chú cho Phase 3): Sau này ta sẽ thêm std::unordered_map để hủy lệnh O(1),
//     tạm thời ở Phase 1-2 chưa cần thêm vội để tập trung nắm vững logic khớp lệnh.
//   + Các hàm helper nội bộ (nếu muốn tách nhỏ logic), ví dụ:
//     void matchOrder(Order& incomingOrder);
//
// - Phần `public:`:
//   + void addOrder(const Order& order);   // Nhận lệnh mới, thử khớp, dư thì đưa lên sổ
//   + bool cancelOrder(OrderId id);        // (Sẽ hoàn thiện ở Phase 3)
//   + std::optional<Price> getBestBid() const; // Lấy giá mua tốt nhất (const vì không sửa dữ liệu)
//   + std::optional<Price> getBestAsk() const; // Lấy giá bán tốt nhất
//   + void printOrderBook() const;         // In toàn bộ sổ lệnh hiện tại ra console
// -> Hãy viết `class OrderBook` ở dưới đây:
class OrderBook
{
private:
    std::map<Price, LimitLevel, std::greater<Price>> bids_;
    std::map<Price, LimitLevel> asks_;
    // phase 3

    void matchOrder(Order &inputOrder)
    {
        if (inputOrder.side == Side::Buy)
        {

            while (!asks_.empty() && inputOrder.remainingQuantity > 0 && inputOrder.price >= asks_.begin()->first)
            {
                auto bestAskIt = asks_.begin();
                Price bestAskPrice = bestAskIt->first;
                LimitLevel &level = bestAskIt->second;
                while (!level.orders.empty() && inputOrder.remainingQuantity > 0)
                {
                    Order &restingOrder = level.orders.front();
                    auto matchingOrder = std::min(inputOrder.remainingQuantity, restingOrder.remainingQuantity);

                    inputOrder.remainingQuantity -= matchingOrder;
                    restingOrder.remainingQuantity -= matchingOrder;
                    level.totalVolume -= matchingOrder;
                    if (restingOrder.remainingQuantity == 0)
                    {
                        level.orders.pop_front();
                    }
                }
                if (level.orders.empty())
                {
                    asks_.erase(bestAskIt);
                }
            }
        }
        else if (inputOrder.side == Side::Sell)
        {

            while (!bids_.empty() && inputOrder.remainingQuantity > 0 && inputOrder.price <= bids_.begin()->first)
            {
                auto bestBuyIt = bids_.begin();
                Price bestBuyPrice = bestBuyIt->first;
                LimitLevel &level = bestBuyIt->second;
                while (!level.orders.empty() && inputOrder.remainingQuantity > 0)
                {
                    Order &restingOrder = level.orders.front();
                    auto matchingOrder = std::min(inputOrder.remainingQuantity, restingOrder.remainingQuantity);

                    inputOrder.remainingQuantity -= matchingOrder;
                    restingOrder.remainingQuantity -= matchingOrder;
                    level.totalVolume -= matchingOrder;
                    if (restingOrder.remainingQuantity == 0)
                    {
                        level.orders.pop_front();
                    }
                }
                if (level.orders.empty())
                {
                    bids_.erase(bestBuyIt);
                }
            }
        }
    }

public:
    void addOrder(const Order &order);
    bool cancelOrder(OrderId id); // phase 3
    bool getBestBid(Price &outPrice) const;
    bool getBestAsk(Price &outPrice) const;
    void printOrderBook() const;
};
