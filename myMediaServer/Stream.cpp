#include "Stream.hh"
#include "spdlog/fmt/bundled/format.h"

#include <regex>
#include <spdlog/spdlog.h>

namespace my {
StreamUri::StreamUri(char const* streamName)
  : mRaw(streamName)
{
  std::regex re(R"((track/(\d+))\?(.*))");
  std::smatch uriMatch;
  if (std::regex_match(mRaw, uriMatch, re)) {
    for (size_t i = 0; i < uriMatch.size(); ++i) {
      SPDLOG_TRACE("uriMatch[{}] = {}", i, uriMatch[i].str());
    }
    if (uriMatch.size() > 1) {
      mPath = std::string(uriMatch[1].str());
    }
    if (uriMatch.size() > 2) {
      mTrackId = std::stoi(uriMatch[2].str());
    }
    if (uriMatch.size() > 3) {
      std::regex re(R"((\w+)=([^&]+))");
      std::smatch queryMatch;
      std::string input(uriMatch[3].str());
      std::string::const_iterator searchStart(input.cbegin());

      while (std::regex_search(searchStart, input.cend(), queryMatch, re)) {
        SPDLOG_TRACE(
          "Query Key: {}, Value: {}", queryMatch[1].str(), queryMatch[2].str());
        if (queryMatch[1].str() == "start_time") {
          mStartTime = std::stoll(queryMatch[2].str());
        } else if (queryMatch[1].str() == "end_time") {
          mEndTime = std::stoll(queryMatch[2].str());
        }
        searchStart = queryMatch.suffix().first;
      }
    }
  }
}

StreamUri::~StreamUri() {}

} // namespace my
