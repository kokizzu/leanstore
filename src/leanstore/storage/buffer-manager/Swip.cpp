#include "Swip.hpp"
// -------------------------------------------------------------------------------------
namespace leanstore {
// -------------------------------------------------------------------------------------
Swip::Swip(u64 pid): pid(pid) {}
// -------------------------------------------------------------------------------------
bool Swip::isSwizzled()
{
   return (pid & swizzle_bit) == swizzle_bit;
}
// -------------------------------------------------------------------------------------
u64 Swip::asInteger() { return pid; }
// -------------------------------------------------------------------------------------
void Swip::swizzle(BufferFrame *bf) {
   pid = u64(bf) | swizzle_bit;
}
// -------------------------------------------------------------------------------------
BufferFrame& Swip::getBufferFrame() {
   return *reinterpret_cast<BufferFrame*>(pid & unswizzle_mask);
}
// -------------------------------------------------------------------------------------
}
// -------------------------------------------------------------------------------------