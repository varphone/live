#include "H264VideoFramedSource.hh"

#include <spdlog/spdlog.h>

namespace my {
H264VideoFramedSource::H264VideoFramedSource(UsageEnvironment& env,
                                             StreamBaseRef stream,
                                             int trackId)
  : FramedSource(env)
  , mStream(stream)
  , mTrackId(trackId)
  , mLastPTS(0)
{
  SPDLOG_TRACE("H264VideoFramedSource::H264VideoFramedSource() @ {}", (void*)this);
}

H264VideoFramedSource::H264VideoFramedSource(const H264VideoFramedSource& rhs)
  : FramedSource(rhs.envir())
  , mStream(rhs.mStream)
  , mTrackId(rhs.mTrackId)
  , mLastPTS(rhs.mLastPTS)
{
  SPDLOG_TRACE("H264VideoFramedSource::H264VideoFramedSource() @ {}", (void*)this);
}

H264VideoFramedSource::~H264VideoFramedSource(void) {}

void H264VideoFramedSource::doGetNextFrame()
{
  fetchFrame();
}

unsigned int H264VideoFramedSource::maxFrameSize() const
{
  return 500000;
}

void H264VideoFramedSource::fetchFrame()
{
  // Close if EOS
  if (mStream->isEos()) {
    handleClosure();
    return;
  }

  // Check video queue, if empty, delay for next.
  if (mStream->numFrames(mTrackId) == 0) {
    nextTask() =
      envir().taskScheduler().scheduleDelayedTask(1000, getNextFrame, this);
    return;
  }

  // 取出一帧数据
  auto frame = mStream->getNextFrame(mTrackId);

  // 如果数据为空, 则延迟获取下一帧
  if (!frame) {
    nextTask() =
      envir().taskScheduler().scheduleDelayedTask(0, getNextFrame, this);
    return;
  }

  // 填充信息
  fFrameSize = frame->size();
  if (fFrameSize > fMaxSize) {
    fNumTruncatedBytes = fFrameSize - fMaxSize;
    fFrameSize = fMaxSize;
  }
  // 设置时间戳
  auto pts = frame->pts();
  fPresentationTime.tv_sec = pts / 1000000;
  fPresentationTime.tv_usec = pts % 1000000;
  fDurationInMicroseconds = frame->duration();
  // 拷贝数据
  memcpy(fTo, frame->data(), fFrameSize);

#if 1
  nextTask() = envir().taskScheduler().scheduleDelayedTask(
    0, (TaskFunc*)FramedSource::afterGetting, this);
#else
  afterGetting(this);
#endif
}

StreamBaseRef H264VideoFramedSource::stream(void)
{
  return mStream;
}

void H264VideoFramedSource::getNextFrame(void* ptr)
{
  if (ptr) {
    ((H264VideoFramedSource*)ptr)->fetchFrame();
  }
}

Boolean H264VideoFramedSource::isH264VideoStreamFramer() const
{
  return True;
}

} // namespace my
