#include "H264VideoServerMediaSubsession.hh"
#include "H264VideoFramedSource.hh"

#include <H264VideoStreamFramer.hh>
#include <cstdio>

namespace my {
H264VideoServerMediaSubsession::H264VideoServerMediaSubsession(
  UsageEnvironment& env,
  // FramedSource* source,
  StreamBaseRef stream,
  int trackId)
  : H264VideoFileServerMediaSubsession(env, "", True)
  // , mSource(source)
  // , mSink(nullptr)
  , mStream(stream)
  , mTrackId(trackId)
// , mDoneFlag(0)
// , mAuxSDPLine(nullptr)
{
  // DBG(DBG_L5,
  //     "XPR_RTSP: "
  //     "MyH264VideoServerMediaSubsession::MyH264VideoServerMediaSubsession(%p,
  //     "
  //     "%p) = %p",
  //     &env,
  //     source,
  //     this);
}

H264VideoServerMediaSubsession::~H264VideoServerMediaSubsession(void)
{
  // DBG(DBG_L5,
  //     "XPR_RTSP: "
  //     "MyH264VideoServerMediaSubsession::~MyH264VideoServerMediaSubsession()
  //     = "
  //     "%p",
  //     this);
  // if (mAuxSDPLine) {
  //   free(mAuxSDPLine);
  //   mAuxSDPLine = NULL;
  // }
  // mSource = NULL;
  // mSink = NULL;
  // mDoneFlag = 0;
}

// void MyH264VideoServerMediaSubsession::afterPlayingDummy(void* clientData)
// {
//   MyH264VideoServerMediaSubsession* subsess =
//     (MyH264VideoServerMediaSubsession*)clientData;
//   subsess->afterPlayingDummy1();
// }

// void MyH264VideoServerMediaSubsession::afterPlayingDummy1()
// {
//   // Unschedule any pending 'checking' task:
//   envir().taskScheduler().unscheduleDelayedTask(nextTask());
//   // Signal the event loop that we're done:
//   setDoneFlag();
// }

// void MyH264VideoServerMediaSubsession::setDoneFlag()
// {
//   mDoneFlag = ~0x00;
// }

// void MyH264VideoServerMediaSubsession::checkForAuxSDPLine(void* clientData)
// {
//   MyH264VideoServerMediaSubsession* subsess =
//     (MyH264VideoServerMediaSubsession*)clientData;
//   subsess->checkForAuxSDPLine1();
// }

// void MyH264VideoServerMediaSubsession::checkForAuxSDPLine1()
// {
//   const char* dasl = NULL;
//   nextTask() = NULL;
//   if (mAuxSDPLine != NULL) {
//     // Signal the event loop that we're done:
//     setDoneFlag();
//   } else if (mSink != NULL && (dasl = mSink->auxSDPLine()) != NULL) {
//     mAuxSDPLine = strDup(dasl);
//     mSink = NULL;
//     // DBG(DBG_L4,
//     //     "XPR_RTSP: MyH264VideoServerMediaSubsession(%p): AuxSDPLine [%s]",
//     //     this,
//     //     mAuxSDPLine);
//     // Signal the event loop that we're done:
//     setDoneFlag();
//   } else if (!mDoneFlag) {
//     // try again after a brief delay:
//     int uSecsToDelay = 10000; // 10 ms
//     nextTask() = envir().taskScheduler().scheduleDelayedTask(
//       uSecsToDelay, (TaskFunc*)checkForAuxSDPLine, this);
//   }
// }

// char const* MyH264VideoServerMediaSubsession::getAuxSDPLine(
//   RTPSink* rtpSink,
//   FramedSource* inputSource)
// {
//   if (mAuxSDPLine) {
//     return mAuxSDPLine;
//   }
//   if (mSink == NULL) {
//     // we're not already setting it up for another, concurrent stream
//     // Note: For H264 video files, the 'config' information
//     // ("profile-level-id" and "sprop-parameter-sets") isn't known until we
//     // start reading the file.  This means that "rtpSink"s "auxSDPLine()"
//     // will be NULL initially, and we need to start reading data from our
//     // file until this changes.
//     mSink = rtpSink;
//     // Start reading the file:
//     mSink->startPlaying(*inputSource, afterPlayingDummy, this);
//     // Check whether the sink's 'auxSDPLine()' is ready:
//     checkForAuxSDPLine(this);
//   }
//   envir().taskScheduler().doEventLoop(&mDoneFlag);
//   return mAuxSDPLine;
// }

FramedSource* H264VideoServerMediaSubsession::createNewStreamSource(
  unsigned clientSessionId,
  unsigned& estBitrate)
{
  printf("MyH264VideoServerMediaSubsession::createNewStreamSource: %d\n",
         clientSessionId);

  estBitrate = 5000;
  H264VideoFramedSource* src =
    new H264VideoFramedSource(envir(), mStream, mTrackId);
  // if (mStream->discreteInput()) {
  //   // DBG(DBG_L4,
  //   //     "XPR_RTSP: MyH264VideoServerMediaSubsession(%p): Use Discrete
  //   //     Input!", this);
  //   return H264VideoStreamDiscreteFramer::createNew(envir(), src, False);
  // }
  return H264VideoStreamFramer::createNew(envir(), src, False);
}

// RTPSink* MyH264VideoServerMediaSubsession::createNewRTPSink(
//   Groupsock* rtpGroupsock,
//   unsigned char rtpPayloadTypeIfDynamic,
//   FramedSource* inputSource)
// {
//   OutPacketBuffer::maxSize = 320000;
//   return H264VideoRTPSink::createNew(
//     envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
// }

H264VideoServerMediaSubsession* H264VideoServerMediaSubsession::createNew(
  UsageEnvironment& env,
  // FramedSource* source,
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
  printf("MyH264VideoServerMediaSubsession::startStream: %d\n",
         clientSessionId);
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
  printf("MyH264VideoServerMediaSubsession::pauseStream: %d\n",
         clientSessionId);
  mStream->stop(mTrackId);
  H264VideoFileServerMediaSubsession::pauseStream(clientSessionId, streamToken);
}
} // namespace my
