#pragma once
#include "Swip.hpp"
#include "Units.hpp"
#include "leanstore/sync-primitives/PlainGuard.hpp"
// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------
#include <atomic>
#include <vector>
// -------------------------------------------------------------------------------------
namespace leanstore
{
namespace buffermanager
{
// -------------------------------------------------------------------------------------
const u64 PAGE_SIZE = 16 * 1024;
// -------------------------------------------------------------------------------------
struct BufferFrame {
  enum class State : u8 { FREE = 0, HOT = 1, COOL = 2 };
  struct Header {
    struct ContentionTracker {
      u32 restarts_counter = 0;
      u32 access_counter = 0;
      s32 last_modified_pos = -1;
      void reset()
      {
        restarts_counter = 0;
        access_counter = 0;
        last_modified_pos = -1;
      }
    };
    // TODO: for logging
    atomic<u64> lastWrittenLSN = 0;
    atomic<State> state = State::FREE;  // INIT:
    atomic<bool> isWB = false;
    bool isCooledBecauseOfReading = false;
    PID pid = 9999;             // INIT:
    HybridLatch latch = 0;  // INIT: // ATTENTION: NEVER DECREMENT
    // -------------------------------------------------------------------------------------
    BufferFrame* next_free_bf = nullptr;
    ContentionTracker contention_tracker;
    // -------------------------------------------------------------------------------------
    u64 debug;
  };
  struct alignas(512) Page {
    u64 LSN = 0;
    u64 dt_id = 9999;                                                                 // INIT: datastructure id
    u64 magic_debugging_number;                                                       // ATTENTION
    u8 dt[PAGE_SIZE - sizeof(LSN) - sizeof(dt_id) - sizeof(magic_debugging_number)];  // Datastruture BE CAREFUL HERE !!!!!
    // -------------------------------------------------------------------------------------
    operator u8*() { return reinterpret_cast<u8*>(this); }
    // -------------------------------------------------------------------------------------
  };
  // -------------------------------------------------------------------------------------
  struct Header header;
  // -------------------------------------------------------------------------------------
  struct Page page;  // The persisted part
  // -------------------------------------------------------------------------------------
  bool operator==(const BufferFrame& other) { return this == &other; }
  // -------------------------------------------------------------------------------------
  inline bool isDirty() const { return header.lastWrittenLSN != page.LSN; }
  // -------------------------------------------------------------------------------------
  // Pre: bf is exclusively locked
  void reset()
  {
    assert(!header.isWB);
    header.latch.assertExclusivelyLatched();
    header.lastWrittenLSN = 0;
    header.state = State::FREE;  // INIT:
    header.isWB = false;
    header.isCooledBecauseOfReading = false;
    header.pid = 9999;
    header.next_free_bf = nullptr;
    header.contention_tracker.reset();
  }
  // -------------------------------------------------------------------------------------
  BufferFrame() { header.latch->store(0ul); }
};
// -------------------------------------------------------------------------------------
static constexpr u64 EFFECTIVE_PAGE_SIZE = sizeof(BufferFrame::Page::dt);
// -------------------------------------------------------------------------------------
static_assert(sizeof(BufferFrame::Page) == PAGE_SIZE, "");
// -------------------------------------------------------------------------------------
static_assert((sizeof(BufferFrame) - sizeof(BufferFrame::Page)) == 512, "");
// -------------------------------------------------------------------------------------
}  // namespace buffermanager
}  // namespace leanstore
// -------------------------------------------------------------------------------------
