#ifndef _MY_STREAM_HH
#define _MY_STREAM_HH

#include "Frame.hh"
#include "spdlog/spdlog.h"

#include <memory>
#include <mutex>
#include <vector>

namespace my {
// 前置声明
class StreamBase;
using StreamBaseRef = std::shared_ptr<StreamBase>;

/// 流基类
class StreamBase
{
public:
  virtual ~StreamBase() {}
  /// 获取下一个帧
  /// \param trackId 轨道 ID, 从 0 开始
  /// \return 下一个帧
  virtual FrameBaseRef getNextFrame(int trackId) { return nullptr; }
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
  /// 启动
  /// \param trackId 轨道 ID, 从 0 开始
  /// \param clientSessionId 客户端会话 ID
  /// \return 0 成功, 非 0 失败
  virtual int start(int trackId, unsigned int clientSessionId = 0) { return 0; }
  /// 停止
  /// \param trackId 轨道 ID, 从 0 开始
  /// \param clientSessionId 客户端会话 ID
  /// \return 0 成功, 非 0 失败
  virtual int stop(int trackId, unsigned int clientSessionId = 0) { return 0; }
  /// 推送帧
  /// \param trackId 轨道 ID, 从 0 开始
  /// \param frame 帧共享指针
  virtual void pushFrame(int trackId, FrameBaseRef frame) {}

private:
  bool mEos{ false };
};

/// H264 视频流
class H264VideoStream : public StreamBase
{
public:
  H264VideoStream()
    : mFrames()
  {
  }
  ~H264VideoStream() {}

  // StreamBase interface
  FrameBaseRef getNextFrame(int trackId) override
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

  void pushFrame(int trackId, FrameBaseRef frame) override
  {
    std::lock_guard<std::mutex> lock(mMutex);
    if (mFrames.size() > 10) {
      mFrames.erase(mFrames.begin());
    }
    mFrames.push_back(frame);
  }

  int start(int trackId, unsigned int clientSeesionId) override
  {
    SPDLOG_DEBUG("H264VideoStream::start({}) @ {}", trackId, (void*)this);
    return 0;
  }

  int stop(int trackId, unsigned int clientSeesionId) override
  {
    SPDLOG_DEBUG("H264VideoStream::stop({}) @ {}", trackId, (void*)this);
    return 0;
  }

private:
  std::mutex mMutex;
  std::vector<FrameBaseRef> mFrames{};
  bool mEos;
};
} // namespace my

#endif // _MY_STREAM_HH
