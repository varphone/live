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
// Implementation

#include "RTSPServer.hh"
#include "H264VideoServerMediaSubsession.hh"
#include "Stream.hh"

#include <regex>
#include <spdlog/spdlog.h>
#include <string.h>

namespace my {

RTSPServer* RTSPServer::createNew(UsageEnvironment& env,
                                  Port ourPort,
                                  UserAuthenticationDatabase* authDatabase,
                                  unsigned reclamationTestSeconds)
{
  int ourSocketIPv4 = setUpOurSocket(env, ourPort, AF_INET);
  int ourSocketIPv6 = setUpOurSocket(env, ourPort, AF_INET6);
  if (ourSocketIPv4 < 0 && ourSocketIPv6 < 0)
    return NULL;

  return new RTSPServer(env,
                        ourSocketIPv4,
                        ourSocketIPv6,
                        ourPort,
                        authDatabase,
                        reclamationTestSeconds);
}

RTSPServer::RTSPServer(UsageEnvironment& env,
                       int ourSocketIPv4,
                       int ourSocketIPv6,
                       Port ourPort,
                       UserAuthenticationDatabase* authDatabase,
                       unsigned reclamationTestSeconds)
  : ::RTSPServer(env,
                 ourSocketIPv4,
                 ourSocketIPv6,
                 ourPort,
                 authDatabase,
                 reclamationTestSeconds)
{
}

RTSPServer::~RTSPServer() {}

void RTSPServer::setPlaybackStreamProvider(StreamProviderRef provider)
{
  mPlaybackStreamProvider = provider;
}

// static ServerMediaSession* createNewSMS(UsageEnvironment& env,
//                                         char const* fileName,
//                                         FILE* fid); // forward

void RTSPServer::lookupServerMediaSession(
  char const* streamName,
  lookupServerMediaSessionCompletionFunc* completionFunc,
  void* completionClientData,
  Boolean isFirstLookupInSession)
{
  SPDLOG_DEBUG("RTSPServer::lookupServerMediaSession({}, {})",
               streamName,
               isFirstLookupInSession);
  ServerMediaSession* sms = nullptr;
  auto s = std::string(streamName);
  std::regex re(R"(track\/\d+.*)");
  if (std::regex_match(s, re)) {
    sms = lookupPlaybackSMS(streamName, isFirstLookupInSession);
  } else {
    sms = getServerMediaSession(streamName);
  }
  if (completionFunc != NULL) {
    (*completionFunc)(completionClientData, sms);
  }
}

ServerMediaSession* RTSPServer::lookupPlaybackSMS(
  char const* rawStreamName,
  Boolean isFirstLookupInSession)
{
  SPDLOG_DEBUG("RTSPServer::lookupPlaybackSMS({}, {})",
               rawStreamName,
               isFirstLookupInSession);
  ServerMediaSession* sms = nullptr;
  auto uri = StreamUri(rawStreamName);
  if (mPlaybackStreamProvider) {
    auto stream = mPlaybackStreamProvider->lookup(uri);
    if (stream) {
      sms = ServerMediaSession::createNew(
        envir(), rawStreamName, rawStreamName, "My Media Server");
      sms->addSubsession(
        H264VideoServerMediaSubsession::createNew(envir(), stream, 0));
      addServerMediaSession(sms);
      auto url = rtspURL(sms);
      spdlog::info("Stream \"{}\" ready.", url);
      delete[] url;
    }
  }
  return sms;
}

} // namespace my
