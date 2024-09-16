/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// Copyright (c) 1996-2024, Live Networks, Inc.  All rights reserved
// A subclass of "RTSPServer" that creates "ServerMediaSession"s on demand,
// based on whether or not the specified stream name exists as a file
// Header file

#ifndef _MY_RTSP_SERVER_HH
#define _MY_RTSP_SERVER_HH

#include "Stream.hh"
#include "live.hh"

#include <regex>
#include <string>

namespace my {
class DynamicStreamResolver
{
public:
  virtual ~DynamicStreamResolver() {}

  virtual ServerMediaSession* lookupServerMediaSession(
    char const* streamName,
    bool isFirstLookupInSession) = 0;
};

class RTSPServer : public RTSPServerBase
{
public:
  /// 创建新的 RTSP 服务器
  static RTSPServer* createNew(UsageEnvironment& env,
                               Port ourPort,
                               UserAuthenticationDatabase* authDatabase,
                               unsigned reclamationTestSeconds = 65);

  /// 设置回放流提供者
  void setPlaybackStreamProvider(StreamProviderRef provider);

protected:
  RTSPServer(UsageEnvironment& env,
             int ourSocketIPv4,
             int ourSocketIPv6,
             Port ourPort,
             UserAuthenticationDatabase* authDatabase,
             unsigned reclamationTestSeconds);
  // called only by createNew();
  virtual ~RTSPServer();

protected: // redefined virtual functions
  virtual void lookupServerMediaSession(
    char const* streamName,
    lookupServerMediaSessionCompletionFunc* completionFunc,
    void* completionClientData,
    Boolean isFirstLookupInSession);

private:
  /// 查找回放流的 ServerMediaSession 对象
  /// \param streamName 流名称
  /// \param isFirstLookupInSession 是否是会话中的第一次查找
  ServerMediaSession* lookupPlaybackSMS(char const* streamName,
                                        Boolean isFirstLookupInSession);

private:
  /// 回放流提供者
  StreamProviderRef mPlaybackStreamProvider{ nullptr };
};

} // namespace my

#endif // _MY_RTSP_SERVER_HH
