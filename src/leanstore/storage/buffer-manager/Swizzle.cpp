#include "Swizzle.hpp"
// -------------------------------------------------------------------------------------
namespace leanstore {
// -------------------------------------------------------------------------------------
Swizzle::Swizzle(u64 pid): pid(pid) {}
// -------------------------------------------------------------------------------------
bool Swizzle::isSwizzled()
{
   return (pid & swizzle_bit) == swizzle_bit;
}
// -------------------------------------------------------------------------------------
u64 Swizzle::asInteger() { return pid; }
// -------------------------------------------------------------------------------------
void Swizzle::swizzle(BufferFrame *bf) {
   pid.store(u64(bf) | swizzle_bit);
}
// -------------------------------------------------------------------------------------
BufferFrame& Swizzle::getBufferFrame() {
   return *reinterpret_cast<BufferFrame*>(pid & unswizzle_mask);
}
// -------------------------------------------------------------------------------------
}
// -------------------------------------------------------------------------------------