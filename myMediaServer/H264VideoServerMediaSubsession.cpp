#include "H264VideoServerMediaSubsession.hh"
#include "H264VideoFramedSource.hh"

#include <cstdio>
#include <spdlog/spdlog.h>

namespace my {
H264VideoServerMediaSubsession::H264VideoServerMediaSubsession(
  UsageEnvironment& env,
  StreamBaseRef stream,
  int trackId)
  : H264VideoFileServerMediaSubsession(env, "", True)
  , mStream(stream)
  , mTrackId(trackId)
{
  SPDLOG_TRACE("H264VideoServerMediaSubsession::H264VideoServerMediaSubsession() @ {}", (void*)this);
}

H264VideoServerMediaSubsession::~H264VideoServerMediaSubsession(void)
{
  SPDLOG_TRACE("H264VideoServerMediaSubsession::~H264VideoServerMediaSubsession() @ {}", (void*)this);
}

FramedSource* H264VideoServerMediaSubsession::createNewStreamSource(
  unsigned clientSessionId,
  unsigned& estBitrate)
{
  estBitrate = 5000;
  H264VideoFramedSource* src =
    new H264VideoFramedSource(envir(), mStream, mTrackId);
  return H264VideoStreamFramer::createNew(envir(), src, False);
}

H264VideoServerMediaSubsession* H264VideoServerMediaSubsession::createNew(
  UsageEnvironment& env,
  StreamBaseRef stream,
  int trackId)
{
  return new H264VideoServerMediaSubsession(env, stream, trackId);
}

void H264VideoServerMediaSubsession::startStream(
  unsigned clientSessionId,
  void* streamToken,
  TaskFunc* rtcpRRHandler,
  void* rtcpRRHandlerClientData,
  unsigned short& rtpSeqNum,
  unsigned& rtpTimestamp,
  ServerRequestAlternativeByteHandler* serverRequestAlternativeByteHandler,
  void* serverRequestAlternativeByteHandlerClientData)
{
  mStream->start(mTrackId);
  H264VideoFileServerMediaSubsession::startStream(
    clientSessionId,
    streamToken,
    rtcpRRHandler,
    rtcpRRHandlerClientData,
    rtpSeqNum,
    rtpTimestamp,
    serverRequestAlternativeByteHandler,
    serverRequestAlternativeByteHandlerClientData);
}

void H264VideoServerMediaSubsession::pauseStream(unsigned clientSessionId,
                                                 void* streamToken)
{
  mStream->stop(mTrackId);
  H264VideoFileServerMediaSubsession::pauseStream(clientSessionId, streamToken);
}
} // namespace my
