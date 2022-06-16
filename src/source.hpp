#pragma once

#include <memory>
#include <string>

class SourceObject {
  const std::string rawSource;

public:
  SourceObject(const std::string &rawSource) : rawSource(rawSource) {}

  const std::string &getRawSource() const { return rawSource; };

  std::string getHightlightedPosition(const size_t position) const {
    std::size_t lineBegin = rawSource.rfind('\n', position);
    if (lineBegin == std::string::npos)
      lineBegin = 0;

    std::size_t lineEnd = rawSource.find('\n', position);
    if (lineEnd == std::string::npos)
      lineEnd = (rawSource.size() - 1);

    auto line = rawSource.substr(lineBegin, lineEnd - lineBegin);
    auto relativeTokenPosition = position - lineBegin;
    std::string highlightLine(line.size(), ' ');
    highlightLine.at(relativeTokenPosition) = '^';
    const std::string prefix = " | ";
    return prefix + line + '\n' + prefix + highlightLine;
  }
};

using Source = std::shared_ptr<SourceObject>;
