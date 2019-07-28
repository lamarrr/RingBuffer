/**
 * @file ring_buffer.h
 * @author Basit Ayantunde (rlamarrr@gmail.com)
 * @brief
 * @version 0.1
 * @date 2019-07-25
 *
 * @copyright Copyright (c) 2019
 *
 */
#ifndef RING_BUFFER_H

#define RING_BUFFER_H

#include <type_traits>
#include <utility>

#include <cstdio>
#include <cstring>

template <typename T>
struct CRefLarger {
  static constexpr bool value = (sizeof(T*) >= sizeof(T));
};

template <typename T, bool CRefLarger = true>
struct ParamT_Helper {
  using type = T;
};

template <typename T>
struct ParamT_Helper<T, false> {
  using type = const T&;
};

template <typename ByteT>
using ParamT = typename ParamT_Helper<ByteT, CRefLarger<ByteT>::value>::type;
typedef void (*ErrorHandler_t)();

void DoNothing();

template <typename ByteT, size_t N, ErrorHandler_t overrun_handler = DoNothing>
class RingBuffer {
  ByteT buffer_[N];

  ByteT* last_write_;
  ByteT* next_read_;

 public:
  using param_t = ParamT<ByteT>;
  static_assert(N > 2, "Buffer size must be greater than 2");
  static_assert(std::is_default_constructible<ByteT>::value,
                "Type must be default constructible");
  static constexpr size_t size = N;

  RingBuffer() : buffer_{}, last_write_{buffer_}, next_read_{buffer_} {
    buffer_[0] = ByteT{};
  }
  RingBuffer(const std::initializer_list<ByteT>& values)
      : buffer_{}, last_write_{buffer_}, next_read_{buffer_} {
    static_assert(
        values.size() == N,
        "Size of Initializer list must be equal to template parameter N");
  }

  RingBuffer(const RingBuffer<ByteT, N>& lbuffer) {
    size_t rdist = lbuffer.next_read_ - lbuffer.buffer_;
    size_t wdist = lbuffer.last_write_ - lbuffer.buffer_;

    memcpy(buffer_, lbuffer.buffer_, sizeof(ByteT) * N);
    next_read_ = buffer_ + rdist;
    last_write_ = buffer_ + wdist;
  }
  RingBuffer(RingBuffer<ByteT, N>&& rbuffer) {
    // fix
    std::swap(rbuffer.buffer_, buffer_);

    next_read_ = rbuffer.next_read_;
    last_write_ = rbuffer.last_write_;
  }
  RingBuffer& operator=(const RingBuffer<ByteT, N>& lbuffer) {
    RingBuffer tmp{lbuffer};
    std::swap(tmp, *this);
    return *this;
  }
  RingBuffer& operator=(RingBuffer<ByteT, N>&& rbuffer) {
    this->buffer_ = rbuffer.buffer_;
    this->last_write_ = rbuffer.last_write_;
    this->next_read_ = rbuffer.next_read_;
    rbuffer.buffer_ = nullptr;
    return *this;
  }
  ~RingBuffer() = default;

  //
  inline ByteT* begin() noexcept { return buffer_; }
  inline ByteT* end() noexcept { return buffer_ + N; }

  inline const ByteT* cbegin() const noexcept { return buffer_; }
  inline const ByteT* cend() const noexcept { return buffer_ + N; }

  //
  inline const ByteT* next_read() const noexcept { return next_read_; }
  inline const ByteT* last_write() const noexcept { return last_write_; }
  inline const ByteT* next_write() const noexcept {
    return cbegin() + ((last_write_index() + 1) % N);
  }

  inline ByteT* next_read() noexcept { return next_read_; }
  inline ByteT* last_write() noexcept { return last_write_; }
  inline ByteT* next_write() noexcept {
    return begin() + ((last_write_index() + 1) % N);
  }

  //
  inline size_t next_read_index() const noexcept {
    return next_read_ - cbegin();
  }
  inline size_t last_write_index() const noexcept {
    return last_write_ - cbegin();
  }

  /**
   * @brief Writes [value] to the next write position in the buffer
   *
   * @parameter value:
   */
  inline void WriteNext(param_t value) {
    ByteT* n_write = next_write();
    if (n_write == next_read()) {
      overrun_handler();
      return;
    }
    last_write_ = n_write;
    *last_write_ = value;
  }

  inline param_t ReadNext() {
    ByteT* n_read = begin() + ((next_read_index() + 1) % N);
    if (n_read == next_write()) {
      overrun_handler();
      return param_t{};
    }

    next_read_ = n_read;
    return *next_read_;
  }

  /**
   * @brief non range-checking
   *
   * @parameter index:
   * @returns ByteT&:
   */
  ByteT& operator[](size_t index) { return buffer_[index]; }
};
#endif
