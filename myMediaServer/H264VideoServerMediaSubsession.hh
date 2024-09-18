#ifndef _MY_H264_VIDEO_SERVER_MEDIA_SUBSESSION_HH
#define _MY_H264_VIDEO_SERVER_MEDIA_SUBSESSION_HH

#include "Stream.hh"
#include "live.hh"

namespace my {
/// H264 视频服务媒体会话
class H264VideoServerMediaSubsession : public H264VideoFileServerMediaSubsession
{
public:
  virtual ~H264VideoServerMediaSubsession(void);

  // Override H264VideoFileServerMediaSubsession interfaces
  // \param "estBitrate" is the stream's estimated bitrate, in kbps
  virtual FramedSource* createNewStreamSource(unsigned clientSessionId,
                                              unsigned& estBitrate);
  // Static Methods
  static H264VideoServerMediaSubsession* createNew(UsageEnvironment& env,
                                                   StreamBaseRef stream,
                                                   int trackId);

protected:
  H264VideoServerMediaSubsession(UsageEnvironment& env,
                                 StreamBaseRef stream,
                                 int trackId);

  void startStream(
    unsigned clientSessionId,
    void* streamToken,
    TaskFunc* rtcpRRHandler,
    void* rtcpRRHandlerClientData,
    unsigned short& rtpSeqNum,
    unsigned& rtpTimestamp,
    ServerRequestAlternativeByteHandler* serverRequestAlternativeByteHandler,
    void* serverRequestAlternativeByteHandlerClientData) override;
  void pauseStream(unsigned clientSessionId, void* streamToken) override;

private:
  StreamBaseRef mStream{ nullptr };
  int mTrackId{ 0 };
};
} // namespace my

#endif // _MY_H264_VIDEO_SERVER_MEDIA_SUBSESSION_HH
