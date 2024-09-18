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
// My Media Server
// main program

#include "MediaServer.hh"
#include "Frame.hh"
#include "H264VideoServerMediaSubsession.hh"
#include "RTSPServer.hh"
#include "Samples.hh"
#include "Stream.hh"
#include "version.hh"

#include <atomic>
#include <fstream>
#include <memory>
#include <mutex>
#include <spdlog/spdlog.h>
#include <thread>
#include <vector>

namespace my {
void setupExampleSMS(RTSPServerBase* rtspServer,
                     UsageEnvironment& env,
                     char const* streamName)
{
  auto sms = ServerMediaSession::createNew(
    env, streamName, streamName, "My Media Server");
  sms->addSubsession(
    H264VideoServerMediaSubsession::createNew(env, SampleStream::get(), 0));
  rtspServer->addServerMediaSession(sms);

  auto url = rtspServer->rtspURL(sms);
  spdlog::info("Stream \"{}\" ready.", url);
  delete[] url;
}

static void setupLiveStreams(RTSPServerBase* rtspServer, UsageEnvironment& env)
{
  for (int i = 0; i < 16; ++i) {
    auto name = "live/" + std::to_string(i);
    auto sms = ServerMediaSession::createNew(
      env, name.c_str(), name.c_str(), "My Media Server");
    sms->addSubsession(H264VideoServerMediaSubsession::createNew(
      env, LiveSampleStreams::get()->findStream(i), 0));
    rtspServer->addServerMediaSession(sms);
    auto url = rtspServer->rtspURL(sms);
    spdlog::info("Stream \"{}\" ready.", url);
    delete[] url;
  }
}

static void setupPlaybackStreams(RTSPServerBase* rtspServer,
                                 UsageEnvironment& env)
{
  auto myRtspServer = dynamic_cast<my::RTSPServer*>(rtspServer);
  auto provider = PlaybackSampleStreams::get();
  myRtspServer->setPlaybackStreamProvider(provider);
}

MediaServer::MediaServer()
{
  // Begin by setting up our usage environment:
  TaskScheduler* scheduler = BasicTaskScheduler::createNew();
  UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);

  UserAuthenticationDatabase* authDB = NULL;
#ifdef ACCESS_CONTROL
  // To implement client access control to the RTSP server, do the following:
  authDB = new UserAuthenticationDatabase;
  authDB->addUserRecord("username1",
                        "password1"); // replace these with real strings
  // Repeat the above with each <username>, <password> that you wish to allow
  // access to the server.
#endif

  // Create the RTSP server.  Try first with the default port number (554),
  // and then with the alternative port number (8554):
  RTSPServer* rtspServer;
  portNumBits rtspServerPortNum = 554;
  rtspServer = my::RTSPServer::createNew(*env, rtspServerPortNum, authDB);
  if (rtspServer == NULL) {
    rtspServerPortNum = 8554;
    rtspServer = my::RTSPServer::createNew(*env, rtspServerPortNum, authDB);
  }
  if (rtspServer == NULL) {
    SPDLOG_ERROR("Failed to create RTSP server: {}", env->getResultMsg());
    exit(1);
  }

  mScheduler = scheduler;
  mEnv = env;
  mRtspServer = rtspServer;
}

MediaServer::~MediaServer() {}

int MediaServer::run()
{
  spdlog::info("My Media Server");
  spdlog::info("Version: {} (Live Media Library version {})",
               MY_MEDIA_SERVER_VERSION_STRING,
               LIVEMEDIA_LIBRARY_VERSION_STRING);

  my::setupExampleSMS(mRtspServer, *mEnv, "example");
  my::setupLiveStreams(mRtspServer, *mEnv);
  my::setupPlaybackStreams(mRtspServer, *mEnv);

  my::LiveSampleStreams::get()->startLiveSource();
  mEnv->taskScheduler().doEventLoop(); // does not return
  my::LiveSampleStreams::get()->stopLiveSource();
  return 0;
}

void MediaServer::spawn()
{
  std::thread([this] { run(); }).detach();
}

} // namespace my

// C API
//==================================================
#include <MyMediaServer.h>

void* mmsCreate(void)
{
  return new my::MediaServer();
}

void mmsDestroy(void* mms)
{
  delete static_cast<my::MediaServer*>(mms);
}

int mmsRun(void* mms)
{
  return static_cast<my::MediaServer*>(mms)->run();
}

int mmsSpawn(void* mms)
{
  static_cast<my::MediaServer*>(mms)->spawn();
  return 0;
}
