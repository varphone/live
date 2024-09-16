#ifndef _MY_H264_VIDEO_FRAMED_SOURCE_HH
#define _MY_H264_VIDEO_FRAMED_SOURCE_HH

#include "Stream.hh"
#include "live.hh"

namespace my {
/// 基于帧的 H264 视频源
class H264VideoFramedSource : public ::FramedSource
{
public:
  H264VideoFramedSource(::UsageEnvironment& env,
                        StreamBaseRef stream,
                        int trackId);
  H264VideoFramedSource(const H264VideoFramedSource& rhs);
  virtual ~H264VideoFramedSource(void);

public:
  // FramedSource interfaces
  virtual void doGetNextFrame();
  virtual unsigned int maxFrameSize() const;

  // Methods
  void fetchFrame();

  // Properties
  StreamBaseRef stream(void);

  //
  static void getNextFrame(void* ptr);

private:
  virtual ::Boolean isH264VideoStreamFramer() const;

private:
  ::TaskToken mCurrentTask;
  StreamBaseRef mStream{ nullptr };
  int mTrackId{ 0 };
  int64_t mLastPTS{ 0 };
};
} // namespace my

#endif // _MY_H264_VIDEO_FRAMED_SOURCE_HH
