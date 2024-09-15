#include "MyH264VideoFramedSource.hh"

MyH264VideoFramedSource::MyH264VideoFramedSource(UsageEnvironment& env,
                                                 MyStreamBase* stream,
                                                 int trackId)
  : FramedSource(env)
  , mStream(stream)
  , mTrackId(trackId)
  , mLastPTS(0)
{
  // DBG(DBG_L5,
  //     "XPR_RTSP: MyH264VideoFramedSource::MyH264VideoFramedSource(%p) = %p",
  //     &env,
  //     this);
}

MyH264VideoFramedSource::MyH264VideoFramedSource(
  const MyH264VideoFramedSource& rhs)
  : FramedSource(rhs.envir())
  , mStream(rhs.stream())
  , mTrackId(rhs.mTrackId)
  , mLastPTS(rhs.mLastPTS)
{
}

MyH264VideoFramedSource::~MyH264VideoFramedSource(void)
{
  // DBG(DBG_L5,
  //     "XPR_RTSP: MyH264VideoFramedSource::~MyH264VideoFramedSource() = %p",
  //     this);
  envir().taskScheduler().unscheduleDelayedTask(mCurrentTask);
}

void MyH264VideoFramedSource::doGetNextFrame()
{
  fetchFrame();
}

unsigned int MyH264VideoFramedSource::maxFrameSize() const
{
  return 500000;
}

void MyH264VideoFramedSource::fetchFrame()
{
  // Check video queue, if empty, delay for next.
  if (mStream->numFrames(mTrackId) == 0) {
    nextTask() =
      envir().taskScheduler().scheduleDelayedTask(1000, getNextFrame, this);
    return;
  }
  // Fetch one block from video queue.
  //   XPR_StreamBlock* ntb = mStream->getVideoFrame();
  //   if (ntb) {
  //     fFrameSize = XPR_StreamBlockLength(ntb);
  //     if (mStream->appendOriginPTS() == 1)
  //       fFrameSize += H264_OPTS_FRM_LEN;
  //     if (fFrameSize > fMaxSize) {
  //       fNumTruncatedBytes = fFrameSize - fMaxSize;
  //       fFrameSize = fMaxSize;
  //     }
  //     if (mStream->appendOriginPTS() == 1) {
  //       uint32_t n = fFrameSize - H264_OPTS_FRM_LEN;
  //       int64_t usecs = XPR_StreamBlockPTS(ntb);
  //       const uint8_t sei[H264_OPTS_HDR_LEN] = { 0x00, 0x00, 0x00, 0x01,
  //                                                0x06, 0x05, 12,   'O',
  //                                                'P',  'T',  'S' };
  //       memcpy(fTo, sei, sizeof(sei));
  //       memcpy(fTo + H264_OPTS_HDR_LEN, &usecs, sizeof(int64_t));
  //       memcpy(fTo + H264_OPTS_FRM_LEN, XPR_StreamBlockData(ntb), n);
  //     } else {
  //       memcpy(fTo, XPR_StreamBlockData(ntb), fFrameSize);
  //     }
  // #if 1
  //     if (mStream->discreteInput() == 1) {
  //       // Force PTS to ntb->pts
  //       int64_t usecs = XPR_StreamBlockPTS(ntb);
  //       fPresentationTime.tv_sec += usecs / 1000000;
  //       fPresentationTime.tv_usec = usecs % 1000000;
  //     } else {
  //       // Setup PTS with ntb->pts
  //       if (fPresentationTime.tv_sec == 0 && fPresentationTime.tv_usec == 0)
  //       {
  //         gettimeofday(&fPresentationTime, NULL);
  //         fDurationInMicroseconds = 10000;
  //       } else {
  //         int64_t usecs = XPR_StreamBlockPTS(ntb) - mLastPTS;
  //         if (usecs) {
  //           usecs += fPresentationTime.tv_usec;
  //           fPresentationTime.tv_sec += usecs / 1000000;
  //           fPresentationTime.tv_usec = usecs % 1000000;
  //         }
  //       }
  //     }
  //     mLastPTS = XPR_StreamBlockPTS(ntb);
  // #else
  //     // Auto filled by H264VideoSteamFramer
  //     fPresentationTime.tv_sec = 0;
  //     fPresentationTime.tv_usec = 0;
  // #endif
  //     mStream->releaseVideoFrame(ntb);
  //     if (fNumTruncatedBytes > 0) {
  //       DBG(DBG_L2,
  //           "XPR_RTSP: MyH264VideoFramedSource(%p): %u bytes truncated.",
  //           this,
  //           fNumTruncatedBytes);
  //     }
  //   } else {
  //     // Should never run here
  //     DBG(DBG_L1,
  //         "XPR_RTSP: MyH264VideoFramedSource(%p): BUG: %s:%d\n",
  //         this,
  //         __FILE__,
  //         __LINE__);
  //     // Clear
  //     fFrameSize = 0;
  //     fNumTruncatedBytes = 0;
  //   }

  // 取出一帧数据
  auto frame = mStream->getNextFrame(mTrackId);
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
  // 拷贝数据
  memcpy(fTo, frame->data(), fFrameSize);

  afterGetting(this);
}

MyStreamBase* MyH264VideoFramedSource::stream(void) const
{
  return mStream;
}

void MyH264VideoFramedSource::getNextFrame(void* ptr)
{
  if (ptr) {
    ((MyH264VideoFramedSource*)ptr)->fetchFrame();
  }
}

Boolean MyH264VideoFramedSource::isH264VideoStreamFramer() const
{
  return True;
}
