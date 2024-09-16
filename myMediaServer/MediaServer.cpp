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
// class SampleStream : public StreamBase
// {
// public:
//   SampleStream()
//     : StreamBase()
//   {
//     SPDLOG_DEBUG("ExampleStream::ExampleStream() @ {}", (void*)this);
//   }

//   ~SampleStream()
//   {
//     SPDLOG_DEBUG("ExampleStream::~ExampleStream() @ {}", (void*)this);
//   }

//   FrameBaseRef getNextFrame(int trackId) override
//   {
//     // printf("ExampleStream::getNextFrame: %d\n", trackId);
//     uint32_t frameSize = 0;
//     mFile.read(reinterpret_cast<char*>(&frameSize), sizeof(frameSize));
//     if (!mFile)
//       return nullptr;
//     auto frame = H264Frame::alloc(frameSize);
//     mFile.read(reinterpret_cast<char*>(frame->data()), frameSize);
//     if (!mFile) {
//       delete frame;
//       return nullptr;
//     }
//     frame->setSize(frameSize);
//     frame->setPTS(mPTS);
//     mPTS += 33333;
//     return std::shared_ptr<FrameBase>(frame);
//   }

//   bool isEos() override { return mFile.eof(); }

//   int numFrames(int trackId) override
//   {
//     // printf("ExampleStream::numFrames: %d\n", trackId);
//     return mFile.eof() ? 0 : 1;
//   }

//   int start(int trackId) override
//   {
//     SPDLOG_DEBUG("ExampleStream::start({}) @ {}", trackId, (void*)this);
//     if (mFile.is_open())
//       mFile.close();
//     mFile.open("example.264.framed", std::ios::binary);
//     return 0;
//   }

//   int stop(int trackId) override
//   {
//     SPDLOG_DEBUG("ExampleStream::stop({}) @ {}", trackId, (void*)this);
//     mFile.close();
//     return 0;
//   }

//   static std::shared_ptr<StreamBase> get()
//   {
//     static std::shared_ptr<StreamBase> stream(new SampleStream());
//     return stream;
//   }

// private:
//   std::ifstream mFile;
//   int64_t mPTS{ 0 };
// };

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

// class LiveStreams
// {
// public:
//   ~LiveStreams()
//   {
//     mStreams.clear();
//     SPDLOG_DEBUG("LiveStreams::~LiveStreams() @ {}", (void*)this);
//   }

//   static std::shared_ptr<LiveStreams> get()
//   {
//     static std::shared_ptr<LiveStreams> streams(new LiveStreams());
//     return streams;
//   }

//   // void setStream(int streamId, std::shared_ptr<StreamBase> stream)
//   // {
//   //   std::lock_guard<std::mutex> lock(mMutex);
//   //   mStreams.push_back(stream);
//   // }

//   // void clearStream(int streamId)
//   // {
//   //   std::lock_guard<std::mutex> lock(mMutex);
//   //   auto it = std::find(mStreams.begin(), mStreams.end(), stream);
//   //   if (it != mStreams.end())
//   //     mStreams.erase(it);
//   // }

//   std::shared_ptr<StreamBase> findStream(int streamId)
//   {
//     std::lock_guard<std::mutex> lock(mMutex);
//     if (streamId < 0 || streamId >= mStreams.size())
//       return nullptr;
//     return mStreams[streamId];
//   }

//   void pushFrameToAll(FrameBaseRef frame)
//   {
//     std::lock_guard<std::mutex> lock(mMutex);
//     SPDLOG_DEBUG("LiveStreams::pushFrameToAll: {}", frame->size());
//     for (auto& stream : mStreams) {
//       stream->pushFrame(0, frame);
//     }
//   }

//   void startLiveSource()
//   {
//     auto thread = new std::thread([this]() {
//       std::ifstream file("example.264.framed", std::ios::binary);
//       if (!file) {
//         SPDLOG_ERROR("Failed to open example.264.framed");
//         return;
//       }
//       int64_t pts = 0;
//       mRunning.store(true);
//       while (mRunning.load()) {
//         uint32_t frameSize = 0;
//         file.read(reinterpret_cast<char*>(&frameSize), sizeof(frameSize));
//         if (!file) {
//           file.clear();
//           file.seekg(0, std::ios::beg);
//           continue;
//         }
//         auto frame = H264Frame::alloc(frameSize);
//         file.read(reinterpret_cast<char*>(frame->data()), frameSize);
//         if (!file) {
//           delete frame;
//           file.clear();
//           file.seekg(0, std::ios::beg);
//           continue;
//         }
//         frame->setSize(frameSize);
//         frame->setPTS(pts);
//         pts += 33333;
//         this->pushFrameToAll(std::shared_ptr<FrameBase>(frame));
//         std::this_thread::sleep_for(std::chrono::milliseconds(33));
//       }
//     });
//     mThread = thread;
//   }

//   void stopLiveSource()
//   {
//     mRunning.store(false);
//     if (mThread) {
//       mThread->join();
//       delete mThread;
//       mThread = nullptr;
//     }
//   }

// private:
//   LiveStreams()
//   {
//     SPDLOG_DEBUG("LiveStreams::LiveStreams() @ {}", (void*)this);
//     for (int i = 0; i < 16; ++i) {
//       auto stream = std::make_shared<H264VideoStream>();
//       mStreams.push_back(stream);
//     }
//   }

// private:
//   std::mutex mMutex;
//   std::vector<StreamBaseRef> mStreams{};
//   std::atomic_bool mRunning{ false };
//   std::thread* mThread{ nullptr };
// };

static void setupLiveStreams(RTSPServerBase* rtspServer, UsageEnvironment& env)
{
  for (int i = 0; i < 16; ++i) {
    auto name = std::to_string(i);
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
} // namespace my

int main(int argc, char** argv)
{
  spdlog::set_level(spdlog::level::debug);

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

  spdlog::info("My Media Server");
  spdlog::info("Version: {} (Live Media Library version {})",
               MY_MEDIA_SERVER_VERSION_STRING,
               LIVEMEDIA_LIBRARY_VERSION_STRING);

  my::setupExampleSMS(rtspServer, *env, "example");
  my::setupLiveStreams(rtspServer, *env);

  // *env << "Play streams from this server using the URL\n";
  // if (weHaveAnIPv4Address(*env)) {
  //   char* rtspURLPrefix = rtspServer->ipv4rtspURLPrefix();
  //   *env << "\t" << rtspURLPrefix << "<filename>\n";
  //   delete[] rtspURLPrefix;
  //   if (weHaveAnIPv6Address(*env))
  //     *env << "or\n";
  // }
  // if (weHaveAnIPv6Address(*env)) {
  //   char* rtspURLPrefix = rtspServer->ipv6rtspURLPrefix();
  //   *env << "\t" << rtspURLPrefix << "<filename>\n";
  //   delete[] rtspURLPrefix;
  // }
  // *env << "where <filename> is a file present in the current directory.\n";

  // *env << "Each file's type is inferred from its name suffix:\n";
  // *env << "\t\".264\" => a H.264 Video Elementary Stream file\n";
  // *env << "\t\".265\" => a H.265 Video Elementary Stream file\n";
  // *env << "\t\".aac\" => an AAC Audio (ADTS format) file\n";
  // *env << "\t\".ac3\" => an AC-3 Audio file\n";
  // *env << "\t\".amr\" => an AMR Audio file\n";
  // *env << "\t\".dv\" => a DV Video file\n";
  // *env << "\t\".m4e\" => a MPEG-4 Video Elementary Stream file\n";
  // *env << "\t\".mkv\" => a Matroska audio+video+(optional)subtitles file\n";
  // *env << "\t\".mp3\" => a MPEG-1 or 2 Audio file\n";
  // *env << "\t\".mpg\" => a MPEG-1 or 2 Program Stream (audio+video) file\n";
  // *env << "\t\".ogg\" or \".ogv\" or \".opus\" => an Ogg audio and/or video "
  //         "file\n";
  // *env << "\t\".ts\" => a MPEG Transport Stream file\n";
  // *env << "\t\t(a \".tsx\" index file - if present - provides server 'trick "
  //         "play' support)\n";
  // *env << "\t\".vob\" => a VOB (MPEG-2 video with AC-3 audio) file\n";
  // *env << "\t\".wav\" => a WAV Audio file\n";
  // *env << "\t\".webm\" => a WebM audio(Vorbis)+video(VP8) file\n";
  // *env << "See http://www.My.com/mediaServer/ for additional
  // documentation.\n";

  // Also, attempt to create a HTTP server for RTSP-over-HTTP tunneling.
  // Try first with the default HTTP port (80), and then with the alternative
  // HTTP port numbers (8000 and 8080).

  if (rtspServer->setUpTunnelingOverHTTP(80) ||
      rtspServer->setUpTunnelingOverHTTP(8000) ||
      rtspServer->setUpTunnelingOverHTTP(8080)) {
    *env << "(We use port " << rtspServer->httpServerPortNum()
         << " for optional RTSP-over-HTTP tunneling).)\n";
  } else {
    *env << "(RTSP-over-HTTP tunneling is not available.)\n";
  }

  my::LiveSampleStreams::get()->startLiveSource();
  env->taskScheduler().doEventLoop(); // does not return
  my::LiveSampleStreams::get()->stopLiveSource();

  return 0; // only to prevent compiler warning
}
