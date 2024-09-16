#ifndef _MY_FRAME_HH
#define _MY_FRAME_HH

#include <cstdint>
#include <memory>

namespace my {

class FrameBase;

using FrameBaseRef = std::shared_ptr<FrameBase>;

/// 帧基类
class FrameBase
{
public:
  /// 析构
  virtual ~FrameBase() {}
  /// 数据
  virtual uint8_t* data() { return nullptr; }
  /// 大小
  virtual int size() { return 0; }
  /// 时间戳, 单位微妙
  virtual int64_t pts() { return 0; }
  /// 时长, 单位微妙
  virtual int64_t duration() { return 33333; }
  /// 是否音频帧
  virtual bool isAudioFrame() { return false; }
  /// 是否视频帧
  virtual bool isVideoFrame() { return false; }
  /// 是否关键帧
  virtual bool isKeyFrame() { return false; }
  /// 是否 H264 帧
  virtual bool isH264Frame() { return false; }
  /// 是否 H265 帧
  virtual bool isH265Frame() { return false; }
};

class H264Frame;
using H264FrameRef = std::shared_ptr<H264Frame>;

class H264Frame : public FrameBase
{
public:
  H264Frame() = default;
  ~H264Frame();

  // MyFrameBase interface
  uint8_t* data() override { return mData; }
  int size() override { return mSize; }
  int64_t pts() override { return mPTS; }
  bool isVideoFrame() override { return true; }
  bool isKeyFrame() override { return mIsKeyFrame; }
  bool isH264Frame() override { return true; }

  // MyH264Frame interface
  void setSize(int size) { mSize = size; }
  void setPTS(int64_t pts) { mPTS = pts; }
  void setKeyFrame(bool isKeyFrame) { mIsKeyFrame = isKeyFrame; }

  static H264FrameRef alloc(int capacity);
  static H264FrameRef fromOwned(uint8_t* data,
                                int size,
                                int64_t pts,
                                bool isKeyFrame);
  static H264FrameRef fromStatic(uint8_t* data,
                                 int size,
                                 int64_t pts,
                                 bool isKeyFrame);

private:
  H264Frame(const H264Frame&) = delete;

private:
  bool mOwned{ false };
  uint8_t* mData{ nullptr };
  int mSize{ 0 };
  int64_t mPTS{ 0 };
  bool mIsKeyFrame{ false };
};

} // namespace my

#endif // _MY_FRAME_HH
