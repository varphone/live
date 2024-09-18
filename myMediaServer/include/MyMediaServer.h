#ifndef _MY_MEDIA_SERVER_H
#define _MY_MEDIA_SERVER_H

#ifdef __cplusplus
extern "C"
{
#endif

  /// 创建媒体服务器
  void* mmsCreate(void);

  /// 销毁媒体服务器
  /// \param mms 媒体服务器对象
  void mmsDestroy(void* mms);

  /// 在当前线程运行媒体服务器
  /// \param mms 媒体服务器对象
  /// \return 0 成功；其他 失败
  int mmsRun(void* mms);

  /// 在其他线程运行媒体服务器
  /// \param mms 媒体服务器对象
  /// \return 0 成功；其他 失败
  int mmsSpawn(void* mms);

#ifdef __cplusplus
}
#endif

#endif // _MY_MEDIA_SERVER_H
