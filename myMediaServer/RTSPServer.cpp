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

#include <liveMedia.hh>
#include <regex>
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

static ServerMediaSession* createNewSMS(UsageEnvironment& env,
                                        char const* fileName,
                                        FILE* fid); // forward

void RTSPServer::lookupServerMediaSession(
  char const* streamName,
  lookupServerMediaSessionCompletionFunc* completionFunc,
  void* completionClientData,
  Boolean isFirstLookupInSession)
{
  ServerMediaSession* sms = nullptr;

  auto s = std::string(streamName);
  std::regex re("track/[\\d]+");
  if (std::regex_match(s, re)) {
    // First, check whether the specified "streamName" exists as a local file:
    FILE* fid = fopen(streamName, "rb");
    Boolean const fileExists = fid != NULL;

    // Next, check whether we already have a "ServerMediaSession" for this file:
    sms = getServerMediaSession(streamName);
    Boolean const smsExists = sms != NULL;

    printf(
      "lookupServerMediaSession: streamName=%s, fileExists=%d, smsExists=%d\n",
      streamName,
      fileExists,
      smsExists);

    // Handle the four possibilities for "fileExists" and "smsExists":
    if (!fileExists) {
      if (smsExists) {
        // "sms" was created for a file that no longer exists. Remove it:
        removeServerMediaSession(sms);
      }

      sms = NULL;
    } else {
      if (smsExists && isFirstLookupInSession) {
        // Remove the existing "ServerMediaSession" and create a new one, in
        // case the underlying file has changed in some way:
        removeServerMediaSession(sms);
        sms = NULL;
      }

      if (sms == NULL) {
        sms = createNewSMS(envir(), streamName, fid);
        addServerMediaSession(sms);
      }

      fclose(fid);
    }
  } else {
    sms = getServerMediaSession(streamName);
  }
  if (completionFunc != NULL) {
    (*completionFunc)(completionClientData, sms);
  }
}

#define NEW_SMS(description)                                                   \
  do {                                                                         \
    char const* descStr =                                                      \
      description ", streamed by the LIVE555 Media Server";                    \
    sms = ServerMediaSession::createNew(env, fileName, fileName, descStr);     \
  } while (0)

static ServerMediaSession* createNewSMS(UsageEnvironment& env,
                                        char const* fileName,
                                        FILE* /*fid*/)
{
  // Use the file name extension to determine the type of "ServerMediaSession":
  char const* extension = strrchr(fileName, '.');
  if (extension == NULL)
    return NULL;

  ServerMediaSession* sms = NULL;
  Boolean const reuseSource = False;
  if (strcmp(extension, ".264") == 0) {
    // Assumed to be a H.264 Video Elementary Stream file:
    NEW_SMS("H.264 Video");
    // allow for some possibly large H.264 frames
    OutPacketBuffer::maxSize = 500000;
    sms->addSubsession(H264VideoFileServerMediaSubsession::createNew(
      env, fileName, reuseSource));
  } else if (strcmp(extension, ".265") == 0) {
    // Assumed to be a H.265 Video Elementary Stream file:
    NEW_SMS("H.265 Video");
    // allow for some possibly large H.265 frames
    OutPacketBuffer::maxSize = 500000;
    sms->addSubsession(H265VideoFileServerMediaSubsession::createNew(
      env, fileName, reuseSource));
  }

  return sms;
}

} // namespace my
