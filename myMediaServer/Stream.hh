#ifndef _MY_STREAM_HH
#define _MY_STREAM_HH

#include "Frame.hh"

#include <memory>
#include <mutex>
#include <spdlog/spdlog.h>
#include <vector>

namespace my {
// 前置声明
class StreamBase;
using StreamBaseRef = std::shared_ptr<StreamBase>;

/// 流基类
class StreamBase
{
public:
  /// 析构函数
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

/// 流 URI
class StreamUri
{
public:
  /// 构造函数
  /// \param streamName 流名称, 格式为: track/0?&startTime=0&endTime=0
  StreamUri(char const* streamName);
  /// 析构函数
  ~StreamUri();

  /// 获取原始名称
  std::string const& raw() const { return mRaw; }
  /// 获取路径
  std::string const& path() const { return mPath; }
  /// 获取轨道 ID
  int trackId() const { return mTrackId; }
  /// 获取开始时间
  int64_t startTime() const { return mStartTime; }
  /// 获取结束时间
  int64_t endTime() const { return mEndTime; }

private:
  /// 原始名称
  std::string mRaw{};
  /// 路径
  std::string mPath{};
  /// 轨道 ID
  int mTrackId{ -1 };
  /// 开始时间
  int64_t mStartTime{ -1 };
  /// 结束时间
  int64_t mEndTime{ -1 };
};

class StreamProvider;
using StreamProviderRef = std::shared_ptr<StreamProvider>;

/// 流提供者
class StreamProvider
{
public:
  /// 析构函数
  virtual ~StreamProvider() {}
  /// 查找流
  /// \param uri 流 URI
  /// \return 流共享指针，如果没有找到返回 nullptr。
  virtual StreamBaseRef lookup(StreamUri uri) = 0;
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

template<>
struct fmt::formatter<my::StreamUri> : fmt::formatter<std::string>
{
  auto format(my::StreamUri my, format_context& ctx) const
    -> decltype(ctx.out())
  {
    return format_to(ctx.out(),
                     "{{Path:{}, TrackId: {}, StartTime: {}, EndTime: {}}}",
                     my.path(),
                     my.trackId(),
                     my.startTime(),
                     my.endTime());
  }
};

#endif // _MY_STREAM_HH
