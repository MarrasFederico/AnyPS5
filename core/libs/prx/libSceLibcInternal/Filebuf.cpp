#include "prx/libSceLibcInternal/include/InventoryExports6.hpp"
#include "prx/libc/include/Api.hpp"
#include "prx/libc/include/General.hpp"
#include "prx/libc/include/GuestLocale.hpp"
#include <cerrno>
#include <cstdint>
#include <limits>

namespace {

constexpr std::uintptr_t FileOffset = 0xa8;

void UnlockFilebuf_nid_no_patch(void* self) {
    const auto address = reinterpret_cast<std::uintptr_t>(self);
    if (address > std::numeric_limits<std::uintptr_t>::max() - FileOffset)
        NotImplemented_nid_no_patch(__func__);
    FileStream* stream = nullptr;
    const int savedErrno = errno;
    const bool copied = GuestCopyIn_nid_no_patch(&stream, reinterpret_cast<const void*>(address + FileOffset), sizeof(stream));
    errno = savedErrno;
    if (!copied) NotImplemented_nid_no_patch(__func__);
    if (stream != nullptr) _Unlockfilelock_nid_postfix(stream);
}

void* FilebufAddress_nid_no_patch(void* self, std::uintptr_t offset) {
    const auto address = reinterpret_cast<std::uintptr_t>(self);
    if (address > std::numeric_limits<std::uintptr_t>::max() - offset)
        NotImplemented_nid_no_patch(__func__);
    return reinterpret_cast<void*>(address + offset);
}

template <typename T>
T ReadFilebufField_nid_no_patch(void* self, std::uintptr_t offset) {
    T value{};
    const int savedErrno = errno;
    const bool copied = GuestCopyIn_nid_no_patch(&value, FilebufAddress_nid_no_patch(self, offset), sizeof(value));
    errno = savedErrno;
    if (!copied) NotImplemented_nid_no_patch(__func__);
    return value;
}

int FilebufSync_nid_no_patch(void* self) {
    FileStream* stream = ReadFilebufField_nid_no_patch<FileStream*>(self, FileOffset);
    if (stream == nullptr) return 0;
    const GuestLocale::StreambufVtable* vtable =
        ReadFilebufField_nid_no_patch<const GuestLocale::StreambufVtable*>(self, offsetof(GuestLocale::Streambuf, vtable));
    if (vtable == nullptr) NotImplemented_nid_no_patch(__func__);
    using OverflowFn = int (APS5_VABI*)(GuestLocale::Streambuf*, int);
    const auto overflow = ReadFilebufField_nid_no_patch<OverflowFn>(
        const_cast<GuestLocale::StreambufVtable*>(vtable), offsetof(GuestLocale::StreambufVtable, overflow));
    if (overflow == nullptr) NotImplemented_nid_no_patch(__func__);
    if (overflow(static_cast<GuestLocale::Streambuf*>(self), -1) == -1) return 0;
    return fflush_nid_postfix(stream) == 0 ? 0 : -1;
}

}

extern "C" {

void APS5_VABI _ZNSt13basic_filebufIcSt11char_traitsIcEE7_UnlockEv_nid_postfix(void* self) {
    UnlockFilebuf_nid_no_patch(self);
}

void APS5_VABI _ZNSt13basic_filebufIwSt11char_traitsIwEE7_UnlockEv_nid_postfix(void* self) {
    UnlockFilebuf_nid_no_patch(self);
}

int APS5_VABI _ZNSt13basic_filebufIcSt11char_traitsIcEE4syncEv_nid_postfix(void* self) {
    return FilebufSync_nid_no_patch(self);
}

int APS5_VABI _ZNSt13basic_filebufIwSt11char_traitsIwEE4syncEv_nid_postfix(void* self) {
    return FilebufSync_nid_no_patch(self);
}

GuestLocale::FilePosition APS5_VABI _ZNSt13basic_filebufIwSt11char_traitsIwEE7seekoffElNSt5_IosbIiE8_SeekdirENS4_9_OpenmodeE_nid_postfix(void* self, long offset, std::int32_t way, std::int32_t mode) {
    (void)offset;
    (void)way;
    (void)mode;
    const FileStream* stream = ReadFilebufField_nid_no_patch<FileStream*>(self, FileOffset);
    if (stream == nullptr) return GuestLocale::FilePosition{-1, 0, 0};
    NotImplemented_nid_no_patch(__func__);
}

std::int32_t APS5_VABI _ZNSt13basic_filebufIcSt11char_traitsIcEE9underflowEv_nid_postfix(void* self) {
    (void)self;
    NotImplemented_nid_no_patch(__func__);
}

std::int32_t APS5_VABI _ZNSt13basic_filebufIwSt11char_traitsIwEE9underflowEv_nid_postfix(void* self) {
    (void)self;
    NotImplemented_nid_no_patch(__func__);
}

std::int32_t APS5_VABI _ZNSt13basic_filebufIwSt11char_traitsIwEE5uflowEv_nid_postfix(void* self) {
    (void)self;
    NotImplemented_nid_no_patch(__func__);
}

void APS5_VABI _ZNSt13basic_filebufIcSt11char_traitsIcEE9_EndwriteEv_nid_postfix(void* self) {
    (void)self;
    NotImplemented_nid_no_patch(__func__);
}

void APS5_VABI _ZNSt13basic_filebufIwSt11char_traitsIwEE9_EndwriteEv_nid_postfix(void* self) {
    (void)self;
    NotImplemented_nid_no_patch(__func__);
}

void APS5_VABI _ZNSt13basic_filebufIcSt11char_traitsIcEE5imbueERKSt6locale_nid_postfix(void* self, const GuestLocale::Implementation* const* loc) {
    (void)self;
    (void)loc;
    NotImplemented_nid_no_patch(__func__);
}

void APS5_VABI _ZNSt13basic_filebufIwSt11char_traitsIwEE5imbueERKSt6locale_nid_postfix(void* self, const GuestLocale::Implementation* const* loc) {
    (void)self;
    (void)loc;
    NotImplemented_nid_no_patch(__func__);
}

void APS5_VABI _ZNSt13basic_filebufIwSt11char_traitsIwEED1Ev_nid_postfix(void* self) {
    (void)self;
    NotImplemented_nid_no_patch(__func__);
}

}
