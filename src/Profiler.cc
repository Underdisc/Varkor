#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

#include "Error.h"
#include "ds/Vector.h"

#include "Profiler.h"

namespace Profiler {

typedef std::chrono::steady_clock Clock;
typedef std::chrono::nanoseconds Nanoseconds;
typedef std::chrono::microseconds Microseconds;
typedef std::chrono::milliseconds Milliseconds;

struct Block {
  Block(const char* name, const Clock::time_point& timestamp);
  const char* mName;
  Clock::time_point mStart;
  Clock::time_point mEnd;
  Ds::Vector<Block> mSubBlocks;
};

Block::Block(const char* name, const Clock::time_point& timestamp):
  mName(name), mStart(timestamp) {}

Ds::Vector<Block*> nBlockChain;
Ds::Vector<Block> nBlocks;

void Start(const char* name) {
  if (nBlockChain.Size() == 0) {
    nBlocks.Emplace(name, Clock::now());
    nBlockChain.Push(&nBlocks.Top());
    return;
  }
  nBlockChain.Top()->mSubBlocks.Emplace(name, Clock::now());
  nBlockChain.Push(&nBlockChain.Top()->mSubBlocks.Top());
}

void EndStart(const char* name) {
  End();
  Start(name);
}

void End() {
  LogAbortIf(
    nBlockChain.Size() == 0,
    "EndBlock can only be called if there is matching StartBlock call.");
  Block& currentBlock = *nBlockChain.Top();
  currentBlock.mEnd = Clock::now();
  nBlockChain.Pop();
}

void ShowBlock(const Block& block, const std::string& indent) {
  // Split the time in nanoseconds into seperate values for seconds,
  // milliseconds, microseconds, and nanoseconds and print out the time.
  Nanoseconds blockTime = block.mEnd - block.mStart;
  const long long seconds = blockTime.count() / Nanoseconds::period::den;
  long long shave = seconds * Nanoseconds::period::den;

  const long long nanoInMilli = 1'000'000;
  const long long millis = (blockTime.count() - shave) / nanoInMilli;
  shave += millis * nanoInMilli;

  const long long nanoInMicro = 1'000;
  const long long micros = (blockTime.count() - shave) / nanoInMicro;
  shave += micros * nanoInMicro;

  const long long nanos = blockTime.count() - shave;

  std::cout << indent << std::setw(3) << std::setfill('0') << seconds << ":"
            << std::setw(3) << std::setfill('0') << millis << ":"
            << std::setw(3) << std::setfill('0') << micros << ":"
            << std::setw(3) << std::setfill('0') << nanos << " " << block.mName;

  // Handle all of the block's sub-blocks if it has any.
  if (block.mSubBlocks.Size() == 0) {
    std::cout << std::endl;
    return;
  }

  std::string subBlockIndent = indent + "| ";
  std::cout << " {" << std::endl;
  for (const Block& subBlock: block.mSubBlocks) {
    ShowBlock(subBlock, subBlockIndent);
  }
  std::cout << indent << "}" << std::endl;
}

void Show() {
  LogAbortIf(
    nBlockChain.Size() != 0,
    "ShowBlocks can only be called if all blocks have been closed.");
  for (const Block& block: nBlocks) {
    ShowBlock(block, "");
  }
}

void Clear() {
  nBlocks.Clear();
}

} // namespace Profiler
