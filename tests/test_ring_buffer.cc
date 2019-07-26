/**
 * @file test_ring_buffer.cc
 * @author Basit Ayantunde (rlamarrr@gmail.com)
 * @brief
 * @version 0.1
 * @date 2019-07-25
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "gtest/gtest.h"
/**
 *
 *
 *
 *  TODO: Each message must contain a word count for ease of copying as rx data
 is not a c style string
 *
 *
 *
 *
 *
 *
 *


 */
#include "ring_buffer.h"

class OverrunException : std::exception {
  const char* what_;

 public:
  explicit OverrunException(const char* msg) : what_{msg}, std::exception{} {}
  const char* what() const noexcept { return what_; }
};

void ExceptionedHandler() { throw OverrunException{"Exception"}; }

TEST(RingBufferTest, CorrectSingleCycleWrite) {
  RingBuffer<char, 5, ExceptionedHandler> a{};

  // first cycle
  EXPECT_NO_THROW(a.WriteNext('A'));
  EXPECT_EQ(a.ReadNext(), 'A');
  EXPECT_NO_THROW(a.WriteNext('B'));
  EXPECT_EQ(a.ReadNext(), 'B');
  EXPECT_NO_THROW(a.WriteNext('C'));
  EXPECT_EQ(a.ReadNext(), 'C');
  EXPECT_NO_THROW(a.WriteNext('D'));
  EXPECT_EQ(a.ReadNext(), 'D');
  EXPECT_NO_THROW(a.WriteNext('E'));
  EXPECT_EQ(a.ReadNext(), 'E');
  EXPECT_THROW(a.ReadNext(), OverrunException);
}

TEST(RingBufferTest, ContinuousSeq_RW_Correct) {
  RingBuffer<char, 5, ExceptionedHandler> a{};

  // first cycle
  for (size_t i = 0; i < 20000; i++) {
    EXPECT_NO_THROW(a.WriteNext(i % 256));
    EXPECT_EQ(a.ReadNext(), static_cast<char>(i % 256));
  }
  EXPECT_THROW(a.ReadNext(), OverrunException);
}

TEST(RingBufferTest, ContinuousSeq_RW_EXPECT) {
  RingBuffer<char, 5, ExceptionedHandler> a{};

  // continuous
  for (size_t i = 0; i < 200000; i++) {
    EXPECT_NO_THROW(a.WriteNext(i % 256));
    EXPECT_NO_THROW(a.ReadNext());
  }
  EXPECT_THROW(a.ReadNext(), OverrunException);

  RingBuffer<char, 5, ExceptionedHandler> b{};

  // continuous
  for (size_t i = 0; i < 200000; i++) {
    EXPECT_NO_THROW(a.WriteNext(i % 256));
    EXPECT_NO_THROW(a.ReadNext());
  }
  EXPECT_THROW(a.ReadNext(), OverrunException);
}

TEST(RingBufferTest, OverrunWrite) {
  RingBuffer<char, 5, ExceptionedHandler> a{};

  // first cycle
  EXPECT_NO_THROW(a.WriteNext('A'));
  EXPECT_NO_THROW(a.WriteNext('B'));
  EXPECT_NO_THROW(a.WriteNext('C'));
  EXPECT_NO_THROW(a.WriteNext('D'));
  EXPECT_THROW(a.WriteNext('E'), OverrunException);
}

TEST(RingBufferTest, EmptyRead) {
  RingBuffer<char, 5, ExceptionedHandler> a{};

  // first cycle
  EXPECT_THROW(a.ReadNext(), OverrunException);
}

TEST(RingBufferTest, OverrunRead) {
  RingBuffer<char, 5, ExceptionedHandler> a{};

  // first cycle
  EXPECT_NO_THROW(a.WriteNext('A'));
  EXPECT_NO_THROW(a.WriteNext('B'));

  EXPECT_NO_THROW(a.ReadNext());
  EXPECT_NO_THROW(a.ReadNext());
  EXPECT_THROW(a.ReadNext(), OverrunException);
}

TEST(RingBufferTest, CopyAssignment) {
  RingBuffer<char, 4, ExceptionedHandler> a;
  a.WriteNext('a');
  a.WriteNext('b');
  a.WriteNext('c');

  RingBuffer<char, 4, ExceptionedHandler> b = a;
  for (int i = 0; i < a.size; i++) EXPECT_EQ(a[i], b[i]);
  EXPECT_NE(a.begin(), b.begin());
}

TEST(RingBufferTest, CopyConstructor) {
  RingBuffer<char, 4, ExceptionedHandler> a;
  a.WriteNext('a');
  a.WriteNext('b');
  a.WriteNext('c');

  RingBuffer<char, 4, ExceptionedHandler> b(a);
  for (int i = 0; i < a.size; i++) EXPECT_EQ(a[i], b[i]);

  EXPECT_NE(a.begin(), b.begin());
  EXPECT_NE(a.end(), b.end());
}

static char* dummy_null = nullptr;

TEST(RingBufferTest, MoveConstructor) {
  RingBuffer<char, 3, ExceptionedHandler> y;
  {
  RingBuffer<char, 3, ExceptionedHandler> x{y};
  }
  y[0];
}

TEST(RingBufferTest, MoveAssignment) {
  // as long as it doesn't loop forever
  RingBuffer<char, 3, ExceptionedHandler> x =
      RingBuffer<char, 3, ExceptionedHandler>{};
  // doesn't crash
  auto q = x[0];
  EXPECT_EQ(q, static_cast<char>(0));
}
TEST(RingBufferTest, UseCase) {
  RingBuffer<char, 2000, ExceptionedHandler> irq_buffer;
  constexpr char tx[] = "WIFI_CONNECT\r\n";
  for (char c : tx) irq_buffer.WriteNext(c);
  char rx[sizeof(tx)];
  for (char& c : rx) {
    c = irq_buffer.ReadNext();
  }

  EXPECT_STREQ(rx, tx);
}
/*


  //  EXPECT_EQ(a.ReadNext(), 'B');
  // EXPECT_EQ(a.ReadNext(), 'C');
  // EXPECT_EQ(a.ReadNext(), 'D');
  // EXPECT_EQ(a.ReadNext(), 'E');
TEST(RingBufferTest, ReadTest) {
  RingBuffer<char, 4, ExceptionedHandler> e;

  EXPECT_NO_THROW(e.WriteNext('A'));
EXPECT_EQ(e.ReadNext(), 'A');
  EXPECT_THROW(e.ReadNext(), OverrunException);
}


*/
int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}