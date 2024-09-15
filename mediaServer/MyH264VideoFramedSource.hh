#ifndef _MY_H264_VIDEO_FRAMED_SOURCE_HH
#define _MY_H264_VIDEO_FRAMED_SOURCE_HH

#include "MyStream.hh"
#include <FramedSource.hh>

/// 基于帧的 H264 视频源
class MyH264VideoFramedSource : public FramedSource
{
public:
  MyH264VideoFramedSource(UsageEnvironment& env,
                          MyStreamBase* stream,
                          int trackId);
  MyH264VideoFramedSource(const MyH264VideoFramedSource& rhs);
  virtual ~MyH264VideoFramedSource(void);

public:
  // FramedSource interfaces
  virtual void doGetNextFrame();
  virtual unsigned int maxFrameSize() const;

  // Methods
  void fetchFrame();

  // Properties
  MyStreamBase* stream(void) const;

  //
  static void getNextFrame(void* ptr);

private:
  virtual Boolean isH264VideoStreamFramer() const;

private:
  TaskToken mCurrentTask;
  MyStreamBase* mStream{ nullptr };
  int mTrackId{ 0 };
  int64_t mLastPTS{ 0 };
};

#endif // _MY_H264_VIDEO_FRAMED_SOURCE_HH
