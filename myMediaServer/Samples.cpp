#include "Samples.hh"
#include "Frame.hh"

namespace my {

H264FramedReader::H264FramedReader(const std::string& fileName)
  : mFileName(fileName)
{
  mFile.open(mFileName, std::ios::binary);
}

H264FramedReader::~H264FramedReader()
{
  if (mFile.is_open())
    mFile.close();
}

void H264FramedReader::clear()
{
  if (mFile.is_open())
    mFile.close();
  mPTS = 0;
}

FrameBaseRef H264FramedReader::getNextFrame()
{
  uint32_t frameSize = 0;
  mFile.read(reinterpret_cast<char*>(&frameSize), sizeof(frameSize));
  if (!mFile)
    return nullptr;
  auto frame = H264Frame::alloc(frameSize);
  mFile.read(reinterpret_cast<char*>(frame->data()), frameSize);
  if (!mFile) {
    return nullptr;
  }
  frame->setSize(frameSize);
  frame->setPTS(mPTS);
  mPTS += 33333;
  return frame;
}

bool H264FramedReader::rewind()
{
  if (mFile.is_open())
    mFile.close();
  mFile.open(mFileName, std::ios::binary);
  return mFile.is_open();
}

} // namespace my
