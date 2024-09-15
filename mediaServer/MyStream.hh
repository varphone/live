#ifndef _MY_STREAM_HH
#define _MY_STREAM_HH

#include "MyFrame.hh"

#include <memory>
#include <mutex>
#include <vector>

/// 流基类
class MyStreamBase
{
public:
  virtual ~MyStreamBase() {}
  /// 获取下一个帧
  /// \param trackId 轨道 ID, 从 0 开始
  /// \return 下一个帧
  virtual std::shared_ptr<MyFrameBase> getNextFrame(int trackId)
  {
    return nullptr;
  }
  /// 是否有音频
  virtual bool hasAudio() { return false; }
  /// 是否有视频
  virtual bool hasVideo() { return false; }
  /// 是否结束
  virtual bool isEos() { return mEos; }
  /// 设置结束
  virtual void setEos() { mEos = true; }
  /// 获取轨道的帧数
  /// \param trackId 轨道 ID, 从 0 开始
  virtual int numFrames(int trackId) { return 0; }
  /// 获取轨道数
  virtual int numTracks() { return 0; }
  /// 最大音频帧大小，单位字节，默认 50000
  virtual int maxAudioFrameSize() { return 50000; }
  /// 最大视频帧大小，单位字节，默认 500000
  virtual int maxVideoFrameSize() { return 500000; }

private:
  bool mEos{ false };
};

/// 视频流
class MyVideoStream : public MyStreamBase
{
public:
  MyVideoStream()
    : mFrames()
  {
  }
  ~MyVideoStream() {}

  // MyStreamBase interface
  std::shared_ptr<MyFrameBase> getNextFrame(int trackId) override
  {
    std::lock_guard<std::mutex> lock(mMutex);
    if (mFrames.empty()) {
      return nullptr;
    }
    auto frame = mFrames.front();
    mFrames.erase(mFrames.begin());
    return frame;
  }

  bool hasVideo() override { return true; }
  int numFrames(int trackId) override { return mFrames.size(); }
  int numTracks() override { return 1; }

  // MyVideoStream interface
  void pushFrame(int trackId, std::shared_ptr<MyFrameBase> frame)
  {
    std::lock_guard<std::mutex> lock(mMutex);
    mFrames.push_back(frame);
  }

private:
  std::mutex mMutex;
  std::vector<std::shared_ptr<MyFrameBase>> mFrames;
  bool mEos;
};

#endif // _MY_STREAM_HH
