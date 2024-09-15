#ifndef _MY_H264_VIDEO_SERVER_MEDIA_SUBSESSION_HH
#define _MY_H264_VIDEO_SERVER_MEDIA_SUBSESSION_HH

#include <FramedSource.hh>
#include <Groupsock.hh>
#include <H264VideoRTPSink.hh>
#include <OnDemandServerMediaSubsession.hh>
#include <UsageEnvironment.hh>


#include "MyStream.hh"

/// H264 视频服务媒体会话
class MyH264VideoServerMediaSubsession : public OnDemandServerMediaSubsession
{
public:
  virtual ~MyH264VideoServerMediaSubsession(void);

  // Override OnDemandServerMediaSubsession interfaces
  // \param "estBitrate" is the stream's estimated bitrate, in kbps
  virtual FramedSource* createNewStreamSource(unsigned clientSessionId,
                                              unsigned& estBitrate);
  virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock,
                                    unsigned char rtpPayloadTypeIfDynamic,
                                    FramedSource* inputSource);

  // Used to implement "getAuxSDPLine()":
  void checkForAuxSDPLine1();
  void afterPlayingDummy1();
  void setDoneFlag();

  // Static Methods
  static MyH264VideoServerMediaSubsession* createNew(UsageEnvironment& env,
                                                     FramedSource* source,
                                                     MyStreamBase* stream,
                                                     int trackId);

  static void afterPlayingDummy(void* ptr);
  static void checkForAuxSDPLine(void* ptr);

protected:
  MyH264VideoServerMediaSubsession(UsageEnvironment& env,
                                   FramedSource* source,
                                   MyStreamBase* stream,
                                   int trackId);

  // Override OnDemandServerMediaSubsession interfaces
  virtual char const* getAuxSDPLine(RTPSink* rtpSink,
                                    FramedSource* inputSource);

private:
  FramedSource* mSource{ nullptr };
  RTPSink* mSink{ nullptr };
  MyStreamBase* mStream{ nullptr };
  int mTrackId{ 0 };
  char mDoneFlag;
  char* mAuxSDPLine;
};

#endif // _MY_H264_VIDEO_SERVER_MEDIA_SUBSESSION_HH
