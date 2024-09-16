#ifndef _MY_H264_VIDEO_SERVER_MEDIA_SUBSESSION_HH
#define _MY_H264_VIDEO_SERVER_MEDIA_SUBSESSION_HH

#include "Stream.hh"
#include "live.hh"

namespace my {
/// H264 视频服务媒体会话
class H264VideoServerMediaSubsession
  : public ::H264VideoFileServerMediaSubsession
{
public:
  virtual ~H264VideoServerMediaSubsession(void);

  // Override H264VideoFileServerMediaSubsession interfaces
  // \param "estBitrate" is the stream's estimated bitrate, in kbps
  virtual ::FramedSource* createNewStreamSource(unsigned clientSessionId,
                                                unsigned& estBitrate);
  // virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock,
  //                                   unsigned char rtpPayloadTypeIfDynamic,
  //                                   FramedSource* inputSource);

  // Used to implement "getAuxSDPLine()":
  // void checkForAuxSDPLine1();
  // void afterPlayingDummy1();
  // void setDoneFlag();

  // Static Methods
  static H264VideoServerMediaSubsession* createNew(::UsageEnvironment& env,
                                                   //  FramedSource* source,
                                                   StreamBaseRef stream,
                                                   int trackId);

  // static void afterPlayingDummy(void* ptr);
  // static void checkForAuxSDPLine(void* ptr);

protected:
  H264VideoServerMediaSubsession(::UsageEnvironment& env,
                                 //  FramedSource* source,
                                 StreamBaseRef stream,
                                 int trackId);

  // Override OnDemandServerMediaSubsession interfaces
  // virtual char const* getAuxSDPLine(RTPSink* rtpSink,
  //                                   FramedSource* inputSource);

  void startStream(
    unsigned clientSessionId,
    void* streamToken,
    ::TaskFunc* rtcpRRHandler,
    void* rtcpRRHandlerClientData,
    unsigned short& rtpSeqNum,
    unsigned& rtpTimestamp,
    ::ServerRequestAlternativeByteHandler* serverRequestAlternativeByteHandler,
    void* serverRequestAlternativeByteHandlerClientData) override;
  void pauseStream(unsigned clientSessionId, void* streamToken) override;

private:
  // FramedSource* mSource{ nullptr };
  // RTPSink* mSink{ nullptr };
  StreamBaseRef mStream{ nullptr };
  int mTrackId{ 0 };
  // char mDoneFlag;
  // char* mAuxSDPLine;
};
} // namespace my

#endif // _MY_H264_VIDEO_SERVER_MEDIA_SUBSESSION_HH
