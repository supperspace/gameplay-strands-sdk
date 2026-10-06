#pragma once

#include <cstdint>
#include <utility>

namespace gstrands {

using FileID = uint32_t;

class SourceLocation {
public:
  explicit SourceLocation(const uint32_t Offset = 0)
    : Offset(Offset) {}

  SourceLocation operator+(const uint32_t InOffset) const {
    return SourceLocation(Offset + InOffset);
  }

  uint32_t getOffset() const {
    return Offset;
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

  friend SourceRange operator+(const SourceRange &L, const SourceRange &R) {
    SourceRange Res;
    Res.Start = SourceLocation(std::min(L.Start.getOffset(), R.Start.getOffset()));
    Res.End = SourceLocation(std::max(L.End.getOffset(), R.End.getOffset()));
    return Res;
  }

private:
  SourceLocation Start;
  SourceLocation End;
};

struct ExpandedSourceLocation {
  FileID File;
  uint32_t Column;
  uint32_t Row;
};

} // namespace gstrands