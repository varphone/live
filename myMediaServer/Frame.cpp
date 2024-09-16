#include "Frame.hh"
#include <memory>

namespace my {
H264Frame::~H264Frame()
{
  if (mOwned) {
    delete[] mData;
  }
}

H264FrameRef H264Frame::alloc(int capacity)
{
  auto frame = std::make_shared<H264Frame>();
  frame->mData = new uint8_t[capacity];
  frame->mSize = capacity;
  frame->mOwned = true;
  return frame;
}

H264FrameRef H264Frame::fromOwned(uint8_t* data,
                                  int size,
                                  int64_t pts,
                                  bool isKeyFrame)
{
  auto frame = std::make_shared<H264Frame>();
  frame->mData = data;
  frame->mSize = size;
  frame->mPTS = pts;
  frame->mIsKeyFrame = isKeyFrame;
  frame->mOwned = true;
  return frame;
}

H264FrameRef H264Frame::fromStatic(uint8_t* data,
                                   int size,
                                   int64_t pts,
                                   bool isKeyFrame)
{
  auto frame = std::make_shared<H264Frame>();
  frame->mData = data;
  frame->mSize = size;
  frame->mPTS = pts;
  frame->mIsKeyFrame = isKeyFrame;
  frame->mOwned = false;
  return frame;
}
} // namespace my