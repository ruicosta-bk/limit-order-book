#include "lob/OrderBook.h"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

int g_passed = 0;
int g_failed = 0;

void expect(bool condition, const std::string& expr, const std::string& testName) {
    if (!condition) {
        throw std::runtime_error("Assertion failed in [" + testName + "]: " + expr);
    }
}

#define EXPECT_TRUE(cond) expect((cond), #cond, __func__)
#define EXPECT_EQ(actual, expected) expect(((actual) == (expected)), #actual " == " #expected, __func__)

template <typename Fn>
void runTest(const std::string& name, Fn&& fn) {
    try {
        fn();
        ++g_passed;
        std::cout << "[PASS] " << name << "\n";
    } catch (const std::exception& ex) {
        ++g_failed;
        std::cerr << "[FAIL] " << name << " -> " << ex.what() << "\n";
    }
}

void testOrderConstruction() {
    Order order(1, Side::Buy, 10050, 25);
    EXPECT_EQ(order.id, 1ULL);
    EXPECT_TRUE(order.side == Side::Buy);
    EXPECT_EQ(order.price, 10050ULL);
    EXPECT_EQ(order.initialQuantity, 25ULL);
    EXPECT_EQ(order.remainingQuantity, 25ULL);
}

void testOrderBookInstantiation() {
    OrderBook book;
    (void)book;
    EXPECT_TRUE(true);
}

// TODO (Phase 1): Thêm test kiểm tra getBestBid() / getBestAsk() trả về std::nullopt khi sổ rỗng.
// TODO (Phase 2): Thêm test cho:
//   - Resting orders (Best Bid / Best Ask / Spread)
//   - FIFO priority tại cùng 1 mức giá
//   - Partial fill & Full fill (kiểm tra danh sách Trade trả về và giá khớp theo Maker)
//   - Multi-level price sweep (xóa sạch tầng giá rỗng khỏi map)
//   - Edge cases: quantity == 0, price == 0
// TODO (Phase 3): Thêm test cho Cancel O(1), Cancel lệnh không tồn tại, Cancel lệnh đã khớp hết,
//                 Duplicate OrderId, và Modify Order (giảm qty giữ FIFO vs tăng qty/đổi giá mất FIFO).

} // namespace

int main() {
    runTest("OrderConstruction", testOrderConstruction);
    runTest("OrderBookInstantiation", testOrderBookInstantiation);

    std::cout << "\nSummary: " << g_passed << " passed, " << g_failed << " failed.\n";
    return g_failed == 0 ? 0 : 1;
}
