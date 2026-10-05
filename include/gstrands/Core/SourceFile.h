#pragma once

#include <array>
#include <chrono>
#include <compare>
#include <cstdint>

namespace gstrands {

/// Represents a source file signature.
struct SourceFileSignature {
  uint64_t FileSize;
  std::chrono::sys_time<std::chrono::nanoseconds> Timestamp;
  std::array<uint8_t, 32> Blake3Digest;

  std::strong_ordering operator<=>(const SourceFileSignature&) const = default;
};

} // namespace gstrands