#ifndef _MY_MEDIA_SERVER_HH
#define _MY_MEDIA_SERVER_HH

#include "RTSPServer.hh"
#include "live.hh"

namespace my {
/// 媒体服务器
class MediaServer
{
public:
  /// 构造函数
  MediaServer();
  /// 析构函数
  virtual ~MediaServer();
  /// 在其他线程中运行媒体服务器
  void spawn();
  /// 在当前线程中启动媒体服务器
  int run();

private:
  TaskScheduler* mScheduler;
  UsageEnvironment* mEnv;
  RTSPServer* mRtspServer;
};
} // namespace my

#endif // _MY_MEDIA_SERVER_HH
