#include "MyFrame.hh"

MyH264Frame::~MyH264Frame()
{
  if (mOwned) {
    delete[] mData;
  }
}

MyH264Frame* MyH264Frame::alloc(int capacity)
{
  auto frame = new MyH264Frame();
  frame->mData = new uint8_t[capacity];
  frame->mSize = capacity;
  frame->mOwned = true;
  return frame;
}

MyH264Frame* MyH264Frame::fromOwned(uint8_t* data,
                                    int size,
                                    int64_t pts,
                                    bool isKeyFrame)
{
  auto frame = new MyH264Frame();
  frame->mData = data;
  frame->mSize = size;
  frame->mPTS = pts;
  frame->mIsKeyFrame = isKeyFrame;
  frame->mOwned = true;
  return frame;
}

MyH264Frame* MyH264Frame::fromStatic(uint8_t* data,
                                     int size,
                                     int64_t pts,
                                     bool isKeyFrame)
{
  auto frame = new MyH264Frame();
  frame->mData = data;
  frame->mSize = size;
  frame->mPTS = pts;
  frame->mIsKeyFrame = isKeyFrame;
  frame->mOwned = false;
  return frame;
}
