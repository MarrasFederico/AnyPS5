#include "prx/libSceLibcInternal/include/InventoryExports6.hpp"
#include "prx/libc/include/Api.hpp"
#include "prx/libc/include/FileStream.hpp"
#include "prx/libc/include/GuestLocale.hpp"
#include "tests/TemporaryFile.hpp"
#include <array>
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <fcntl.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

namespace {

using Sync = int (APS5_VABI*)(void*);
using Seekoff = GuestLocale::FilePosition (APS5_VABI*)(void*, long, std::int32_t, std::int32_t);

static_assert(std::is_same_v<decltype(&_ZNSt13basic_filebufIcSt11char_traitsIcEE4syncEv_nid_postfix), Sync>);
static_assert(std::is_same_v<decltype(&_ZNSt13basic_filebufIwSt11char_traitsIwEE4syncEv_nid_postfix), Sync>);
static_assert(std::is_same_v<decltype(&_ZNSt13basic_filebufIwSt11char_traitsIwEE7seekoffElNSt5_IosbIiE8_SeekdirENS4_9_OpenmodeE_nid_postfix), Seekoff>);
static_assert(std::is_same_v<decltype(&_ZNSt13basic_filebufIcSt11char_traitsIcEE9underflowEv_nid_postfix), std::int32_t (APS5_VABI*)(void*)>);
static_assert(std::is_same_v<decltype(&_ZNSt13basic_filebufIwSt11char_traitsIwEE5uflowEv_nid_postfix), std::int32_t (APS5_VABI*)(void*)>);
static_assert(std::is_same_v<decltype(&_ZNSt13basic_filebufIwSt11char_traitsIwEE9_EndwriteEv_nid_postfix), void (APS5_VABI*)(void*)>);
static_assert(std::is_same_v<decltype(&_ZNSt13basic_filebufIwSt11char_traitsIwEED1Ev_nid_postfix), void (APS5_VABI*)(void*)>);
static_assert(std::is_same_v<decltype(&_ZNSt13basic_filebufIwSt11char_traitsIwEE5imbueERKSt6locale_nid_postfix),
    void (APS5_VABI*)(void*, const GuestLocale::Implementation* const*)>);

struct FilebufStorage {
    std::array<std::byte, 0xa8> prefix;
    FileStream* file;
    std::array<std::byte, 32> suffix;
};

static_assert(offsetof(FilebufStorage, file) == 0xa8);
static_assert(offsetof(GuestLocale::StreambufVtable, overflow) == 0x20);

void Require(bool condition, const char* message) {
    if (condition) return;
    std::fprintf(stderr, "filebuf io: %s\n", message);
    std::abort();
}

template <typename TCall>
void RequireNotImplemented(TCall call, const char* name) {
    try {
        call();
    } catch (const std::runtime_error& error) {
        Require(std::string(error.what()) == std::string(name) + " not implemented", "a pending filebuf member threw another error");
        return;
    }
    Require(false, "a pending filebuf member returned");
}

unsigned overflowCalls = 0;
void* overflowSelf = nullptr;
int overflowArgument = 0;
int overflowResult = 0;

int APS5_VABI OverflowProbe(GuestLocale::Streambuf* self, int character) {
    ++overflowCalls;
    overflowSelf = self;
    overflowArgument = character;
    return overflowResult;
}

struct FakeVtable {
    std::array<std::uintptr_t, 5> slots;
};

FilebufStorage MakeObject(FileStream* file, const void* vtable) {
    FilebufStorage object{};
    object.prefix.fill(std::byte{0xa5});
    object.file = file;
    object.suffix.fill(std::byte{0x5a});
    std::memcpy(object.prefix.data(), &vtable, sizeof(vtable));
    return object;
}

void ReplaceWithReadOnlyDescriptor(FileStream& stream) {
#ifdef _WIN32
    const int descriptor = ::_open("NUL", _O_RDONLY | _O_BINARY);
    Require(descriptor >= 0 && ::_dup2(descriptor, ::_fileno(stream.GetHandle())) == 0, "the stream descriptor became read-only");
    Require(::_close(descriptor) == 0, "the spare descriptor closed");
#else
    const int descriptor = ::open("/dev/null", O_RDONLY);
    Require(descriptor >= 0 && ::dup2(descriptor, ::fileno(stream.GetHandle())) >= 0, "the stream descriptor became read-only");
    Require(::close(descriptor) == 0, "the spare descriptor closed");
#endif
}

long ReadDescriptor(FileStream& stream, char* buffer, std::size_t bytes) {
#ifdef _WIN32
    ::_lseeki64(::fileno(stream.GetHandle()), 0, SEEK_SET);
    return ::_read(::fileno(stream.GetHandle()), buffer, static_cast<unsigned>(bytes));
#else
    return static_cast<long>(::pread(::fileno(stream.GetHandle()), buffer, bytes, 0));
#endif
}

void CheckSyncClosedFile() {
    for (Sync sync : {_ZNSt13basic_filebufIcSt11char_traitsIcEE4syncEv_nid_postfix,
                      _ZNSt13basic_filebufIwSt11char_traitsIwEE4syncEv_nid_postfix}) {
        FilebufStorage object{};
        object.prefix.fill(std::byte{0xa5});
        object.suffix.fill(std::byte{0x5a});
        const auto empty = object;
        errno = 71;
        Require(sync(&object) == 0, "sync on a closed file buffer returns 0");
        Require(errno == 71, "sync on a closed file buffer preserves errno");
        Require(std::memcmp(&object, &empty, sizeof(object)) == 0, "sync on a closed file buffer leaves it unchanged");
    }
}

void CheckSyncFlushesWrittenBytes() {
    for (Sync sync : {_ZNSt13basic_filebufIcSt11char_traitsIcEE4syncEv_nid_postfix,
                      _ZNSt13basic_filebufIwSt11char_traitsIwEE4syncEv_nid_postfix}) {
        FileStream stream(TemporaryFile::Open());
        FakeVtable vtable{{0, 0, 0, 0, reinterpret_cast<std::uintptr_t>(&OverflowProbe)}};
        auto object = MakeObject(&stream, vtable.slots.data());
        const auto snapshot = object;
        Require(fputc_nid_postfix('x', &stream) != -1, "the guest putc wrote a byte");
        overflowResult = 1;
        overflowCalls = 0;
        Require(sync(&object) == 0, "sync on an open file buffer returns 0");
        Require(overflowCalls == 1, "sync calls the buffer overflow exactly once");
        Require(overflowSelf == &object, "sync calls the buffer overflow on the file buffer");
        Require(overflowArgument == -1, "sync calls the buffer overflow with eof");
        Require(std::memcmp(&object, &snapshot, sizeof(object)) == 0, "sync preserves surrounding buffer sentinels");
        char bytes[4] = {};
        Require(ReadDescriptor(stream, bytes, 1) == 1 && bytes[0] == 'x', "sync leaves the written byte in the file");
    }
}

void CheckSyncFailures() {
    for (Sync sync : {_ZNSt13basic_filebufIcSt11char_traitsIcEE4syncEv_nid_postfix,
                      _ZNSt13basic_filebufIwSt11char_traitsIwEE4syncEv_nid_postfix}) {
        FileStream homing(TemporaryFile::Open());
        FakeVtable vtable{{0, 0, 0, 0, reinterpret_cast<std::uintptr_t>(&OverflowProbe)}};
        auto object = MakeObject(&homing, vtable.slots.data());
        Require(fputc_nid_postfix('y', &homing) != -1, "the guest putc wrote a byte");
        ReplaceWithReadOnlyDescriptor(homing);
        overflowResult = -1;
        overflowCalls = 0;
        Require(sync(&object) == 0, "sync with a failed overflow returns 0");
        Require(overflowCalls == 1, "sync with a failed overflow still called the buffer overflow");

        FileStream flushing(TemporaryFile::Open());
        auto openAgain = MakeObject(&flushing, vtable.slots.data());
        Require(fputc_nid_postfix('z', &flushing) != -1, "the guest putc wrote a byte");
        ReplaceWithReadOnlyDescriptor(flushing);
        overflowResult = 1;
        overflowCalls = 0;
        Require(sync(&openAgain) == -1, "sync with a failed flush returns -1");
        Require(overflowCalls == 1, "sync with a failed flush called the buffer overflow");
    }
}

void CheckSyncNullVtable() {
    FileStream stream(TemporaryFile::Open());
    FakeVtable nullSlot{};
    for (Sync sync : {_ZNSt13basic_filebufIcSt11char_traitsIcEE4syncEv_nid_postfix,
                      _ZNSt13basic_filebufIwSt11char_traitsIwEE4syncEv_nid_postfix}) {
        auto object = MakeObject(&stream, nullptr);
        RequireNotImplemented([&] { sync(&object); }, "FilebufSync_nid_no_patch");
        auto emptyVtableObject = MakeObject(&stream, nullSlot.slots.data());
        RequireNotImplemented([&] { sync(&emptyVtableObject); }, "FilebufSync_nid_no_patch");
    }
}

void CheckSeekoffClosedFile() {
    FilebufStorage object{};
    object.prefix.fill(std::byte{0xa5});
    object.suffix.fill(std::byte{0x5a});
    const auto empty = object;
    errno = 71;
    const auto position =
        _ZNSt13basic_filebufIwSt11char_traitsIwEE7seekoffElNSt5_IosbIiE8_SeekdirENS4_9_OpenmodeE_nid_postfix(&object, 0, 1, 3);
    Require(position.offset == -1, "seekoff on a closed file buffer returns -1");
    Require(position.filePosition == 0 && position.state == 0, "the failed seekoff position has a zero file position and state");
    Require(errno == 71, "seekoff on a closed file buffer preserves errno");
    Require(std::memcmp(&object, &empty, sizeof(object)) == 0, "seekoff on a closed file buffer leaves it unchanged");
}

void CheckSeekoffOpenFileThrows() {
    FileStream stream(TemporaryFile::Open());
    auto object = MakeObject(&stream, nullptr);
    RequireNotImplemented(
        [&] {
            _ZNSt13basic_filebufIwSt11char_traitsIwEE7seekoffElNSt5_IosbIiE8_SeekdirENS4_9_OpenmodeE_nid_postfix(&object, 0, 1, 3);
        },
        "_ZNSt13basic_filebufIwSt11char_traitsIwEE7seekoffElNSt5_IosbIiE8_SeekdirENS4_9_OpenmodeE_nid_postfix");
}

void CheckPendingMembersThrow() {
    FilebufStorage object{};
    object.prefix.fill(std::byte{0xa5});
    object.suffix.fill(std::byte{0x5a});
    const auto empty = object;
    RequireNotImplemented([&] { _ZNSt13basic_filebufIcSt11char_traitsIcEE9underflowEv_nid_postfix(&object); },
        "_ZNSt13basic_filebufIcSt11char_traitsIcEE9underflowEv_nid_postfix");
    RequireNotImplemented([&] { _ZNSt13basic_filebufIwSt11char_traitsIwEE9underflowEv_nid_postfix(&object); },
        "_ZNSt13basic_filebufIwSt11char_traitsIwEE9underflowEv_nid_postfix");
    RequireNotImplemented([&] { _ZNSt13basic_filebufIwSt11char_traitsIwEE5uflowEv_nid_postfix(&object); },
        "_ZNSt13basic_filebufIwSt11char_traitsIwEE5uflowEv_nid_postfix");
    RequireNotImplemented([&] { _ZNSt13basic_filebufIcSt11char_traitsIcEE9_EndwriteEv_nid_postfix(&object); },
        "_ZNSt13basic_filebufIcSt11char_traitsIcEE9_EndwriteEv_nid_postfix");
    RequireNotImplemented([&] { _ZNSt13basic_filebufIwSt11char_traitsIwEE9_EndwriteEv_nid_postfix(&object); },
        "_ZNSt13basic_filebufIwSt11char_traitsIwEE9_EndwriteEv_nid_postfix");
    RequireNotImplemented([&] { _ZNSt13basic_filebufIcSt11char_traitsIcEE5imbueERKSt6locale_nid_postfix(&object, nullptr); },
        "_ZNSt13basic_filebufIcSt11char_traitsIcEE5imbueERKSt6locale_nid_postfix");
    RequireNotImplemented([&] { _ZNSt13basic_filebufIwSt11char_traitsIwEE5imbueERKSt6locale_nid_postfix(&object, nullptr); },
        "_ZNSt13basic_filebufIwSt11char_traitsIwEE5imbueERKSt6locale_nid_postfix");
    RequireNotImplemented([&] { _ZNSt13basic_filebufIwSt11char_traitsIwEED1Ev_nid_postfix(&object); },
        "_ZNSt13basic_filebufIwSt11char_traitsIwEED1Ev_nid_postfix");
    Require(std::memcmp(&object, &empty, sizeof(object)) == 0, "the pending members left the file buffer unchanged");
}

}

int main() {
    CheckSyncClosedFile();
    CheckSyncFlushesWrittenBytes();
    CheckSyncFailures();
    CheckSyncNullVtable();
    CheckSeekoffClosedFile();
    CheckSeekoffOpenFileThrows();
    CheckPendingMembersThrow();
    std::puts("filebuf io: sync and closed-file seek checks passed");
}
