#ifndef _MY_SAMPLES_HH
#define _MY_SAMPLES_HH

#include "Frame.hh"
#include "Stream.hh"
#include <fstream>
#include <memory>
#include <spdlog/spdlog.h>
#include <string>
#include <thread>
#include <vector>

namespace my {
/// H264 帧读取器
class H264FramedReader
{
public:
  /// 构造函数
  /// \param fileName 文件名
  H264FramedReader(const std::string& fileName);
  /// 析构函数
  ~H264FramedReader();
  /// 清空状态及缓冲区
  void clear();
  /// 是否结束
  bool eof() { return mFile.eof(); }
  /// 获取下一个帧
  FrameBaseRef getNextFrame();
  /// 倒带
  bool rewind();

private:
  /// 文件名
  std::string mFileName{};
  /// 文件
  std::ifstream mFile{};
  /// PTS
  uint64_t mPTS{ 0 };
};

/// 示例流
class SampleStream : public StreamBase
{
public:
  /// 构造函数
  /// \param loopPlay 是否循环播放
  SampleStream(bool loopPlay = false)
    : StreamBase()
    , mLoopPlay(loopPlay)
  {
    SPDLOG_DEBUG("SampleStream::SampleStream() @ {}", (void*)this);
  }

  /// 析构函数
  virtual ~SampleStream()
  {
    SPDLOG_DEBUG("SampleStream::~SampleStream() @ {}", (void*)this);
  }

  FrameBaseRef getNextFrame(int trackId) override
  {
    SPDLOG_TRACE("SampleStream::getNextFrame({}) @ {}", trackId, (void*)this);
    if (mFramedReader.eof()) {
      if (mLoopPlay) {
        mFramedReader.rewind();
      } else {
        return nullptr;
      }
    }
    return mFramedReader.getNextFrame();
  }

  bool isEos() override { return mFramedReader.eof(); }

  int numFrames(int trackId) override { return mFramedReader.eof() ? 0 : 1; }

  int start(int trackId, unsigned int clientSessionId) override
  {
    SPDLOG_DEBUG("SampleStream::start({}) @ {}", trackId, (void*)this);
    mFramedReader.rewind();
    return 0;
  }

  int stop(int trackId, unsigned int clientSessionId) override
  {
    SPDLOG_DEBUG("SampleStream::stop({}) @ {}", trackId, (void*)this);
    mFramedReader.clear();
    return 0;
  }

  static StreamBaseRef get()
  {
    static StreamBaseRef stream(new SampleStream());
    return stream;
  }

private:
  H264FramedReader mFramedReader{ "example.264.framed" };
  bool mLoopPlay{ false };
};

class LiveSampleStreams;
using LiveSampleStreamsRef = std::shared_ptr<LiveSampleStreams>;

/// 直播示例流
class LiveSampleStreams
{
public:
  /// 析构函数
  ~LiveSampleStreams()
  {
    mStreams.clear();
    SPDLOG_DEBUG("LiveSampleStreams::~LiveSampleStreams() @ {}", (void*)this);
  }

  /// 查找流
  StreamBaseRef findStream(int streamId)
  {
    std::lock_guard<std::mutex> lock(mMutex);
    if (streamId < 0 || streamId >= mStreams.size())
      return nullptr;
    return mStreams[streamId];
  }

  /// 推送帧到指定流
  void pushFrame(int streamId, FrameBaseRef frame)
  {
    std::lock_guard<std::mutex> lock(mMutex);
    if (streamId < 0 || streamId >= mStreams.size())
      return;
    mStreams[streamId]->pushFrame(0, frame);
  }

  /// 推送帧到所有流
  void pushFrameToAll(FrameBaseRef frame)
  {
    std::lock_guard<std::mutex> lock(mMutex);
    SPDLOG_TRACE("LiveSampleStreams::pushFrameToAll: {}", frame->size());
    for (auto& stream : mStreams) {
      stream->pushFrame(0, frame);
    }
  }

  /// 启动直播源
  void startLiveSource()
  {
    // 创建线程在后台运行
    auto thread = new std::thread([this]() {
      // 创建帧读取器
      H264FramedReader reader("example.264.framed");
      // 标记线程运行
      mRunning.store(true);
      // 循环
      while (mRunning.load()) {
        // 获取下一个帧
        auto frame = reader.getNextFrame();
        if (!frame) {
          if (reader.rewind())
            continue;
          break;
        }
        // 推送帧到所有流
        this->pushFrameToAll(frame);
        // 模拟 30fps
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
      }
      SPDLOG_DEBUG("LiveSampleStreams::LiveSourceThread() exited!");
    });

    mThread = thread;
  }

  /// 停止直播源
  void stopLiveSource()
  {
    mRunning.store(false);
    if (mThread) {
      mThread->join();
      delete mThread;
      mThread = nullptr;
    }
  }

public:
  /// 获取单例
  static LiveSampleStreamsRef get()
  {
    static LiveSampleStreamsRef streams(new LiveSampleStreams());
    return streams;
  }

private:
  /// 构造函数
  LiveSampleStreams()
  {
    SPDLOG_DEBUG("LiveSampleStreams::LiveSampleStreams() @ {}", (void*)this);
    for (int i = 0; i < 16; ++i) {
      auto stream = std::make_shared<H264VideoStream>();
      mStreams.push_back(stream);
    }
  }

private:
  /// 互斥锁
  std::mutex mMutex{};
  /// 流列表
  std::vector<StreamBaseRef> mStreams{};
  /// 运行标记
  std::atomic_bool mRunning{ false };
  /// 线程
  std::thread* mThread{ nullptr };
};

class PlaybackSampleStreams;
using PlaybackSampleStreamsRef = std::shared_ptr<PlaybackSampleStreams>;

/// 回放示例流
class PlaybackSampleStreams : public StreamProvider
{
public:
  /// 析构函数
  virtual ~PlaybackSampleStreams();

  /// 查找流
  StreamBaseRef lookup(StreamUri uri) override;

public:
  /// 获取单例
  static PlaybackSampleStreamsRef get();

private:
  /// 构造函数
  PlaybackSampleStreams();
};

} // namespace my

#endif // _MY_SAMPLE_HH
