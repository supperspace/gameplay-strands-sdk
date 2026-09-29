#pragma once

#include <cstdint>

namespace gstrands {

class SourceLocation {

public:
  explicit SourceLocation(const uint32_t Offset = 0)
    : Offset(Offset) {}

  SourceLocation operator+(const uint32_t InOffset) const {
    return SourceLocation(Offset + InOffset);
  }

private:
  uint32_t Offset = 0;
};

class SourceRange {
public:
  SourceRange() = default;

  SourceRange(SourceLocation InStart, SourceLocation InEnd)
    : Start(InStart), End(InEnd) {}

  SourceLocation getStartLoc() const {
    return Start;
  }

  SourceLocation getEndLoc() const {
    return End;
  }

  explicit(false) operator SourceLocation() const {
    return getStartLoc();
  }

private:
  SourceLocation Start;
  SourceLocation End;
};

} // namespace gstrands