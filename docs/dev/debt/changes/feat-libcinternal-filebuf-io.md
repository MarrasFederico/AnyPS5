# libSceLibcInternal

## change [_ZNSt13basic_filebufIwSt11char_traitsIwEE4syncEv]

- [basic_filebuf<char>::sync and basic_filebuf<wchar_t>::sync](../../../core/libs/prx/libSceLibcInternal/Filebuf.cpp) (libSceLibcInternal, NIDs `VNaqectsZNs`, `+WvmZi3216M`) - declared from their mangled names (`self` -> `int`): a null `FILE*` at `0xa8` returns 0, else calls vtable `overflow` (`0x20`, `-1`) and `fflush` on the stream (`0` or `-1`), following [__msvc_filebuf.hpp#L626-L632](https://github.com/microsoft/STL/blob/770da5aa457af11f0a796600e1aa6f7f6fce14a6/stl/inc/__msvc_filebuf.hpp#L626-L632). The console `libSceLibcInternal.sprx` was not read; pre-C++11 Dinkumware cleared `_Wrotesome` and putback state whose `0x70..0xa7` offsets remain ambiguous. Unreadable object/vtable or null `overflow` slot throws.

## remove | `_ZNSt13basic_filebufIcSt11char_traitsIcEE4syncEv` | `VNaqectsZNs` |

## change [_ZNSt13basic_filebufIwSt11char_traitsIwEE7seekoffElNSt5_IosbIiE8_SeekdirENS4_9_OpenmodeE]

- [seekoff](../../../core/libs/prx/libSceLibcInternal/Filebuf.cpp) (libSceLibcInternal, NID `g7gjCDEedJA`) - closed-file branch only: a null `FILE*` at 0xa8 returns -1 with zero position and state, as [__msvc_filebuf.hpp#L574-L591](https://github.com/microsoft/STL/blob/770da5aa457af11f0a796600e1aa6f7f6fce14a6/stl/inc/__msvc_filebuf.hpp#L574-L591) does. Open files throw: the seek needs `_Endwrite` (fields `_Pcvt` and `_Wrotesome`) and the putback undo (`_Mychar`, `_Set_eback`, `_Set_egptr`), none with an offset proved in this repository. The returned `GuestLocale::FilePosition` assumes the Dinkumware `fpos` of three 64-bit words {offset, `fpos_t`, state} of [iosfwd#L49-L131](https://github.com/microsoft/STL/blob/770da5aa457af11f0a796600e1aa6f7f6fce14a6/stl/inc/iosfwd#L49-L131) with the `std::int64_t` `fpos_t` of [Api.hpp](../../../core/libs/prx/libc/include/Api.hpp); `_Mbstatet` size is unproved, but three words is the smallest such `fpos`, so the write stays inside the caller slot.

## change [_ZNSt13basic_filebufIcSt11char_traitsIcEE9underflowEv]

- [underflow](../../../core/libs/prx/libSceLibcInternal/Filebuf.cpp) (libSceLibcInternal, NID `Nl6si1CfINw`) - declared from its mangled name with `self` and an `int_type` return, still throwing. Every branch of [__msvc_filebuf.hpp#L427-L437](https://github.com/microsoft/STL/blob/770da5aa457af11f0a796600e1aa6f7f6fce14a6/stl/inc/__msvc_filebuf.hpp#L427-L437) tests the get-area handles of the stream-buffer base, and `GuestLocale::Streambuf` leaves 0x08-0x38 unnamed; `_Pcvt` and `_Mychar` also have no offset proved in this repository, so the member waits for guest layout evidence.

## change [_ZNSt13basic_filebufIwSt11char_traitsIwEE9underflowEv]

- [underflow](../../../core/libs/prx/libSceLibcInternal/Filebuf.cpp) (libSceLibcInternal, NID `hXsvfky362s`) - declared from its mangled name with `self` and an `int_type` return, still throwing; same missing offsets as the narrow member: the get-area handles of the base buffer (unnamed 0x08-0x38 in `GuestLocale::Streambuf`), `_Pcvt` and `_Mychar`, against [__msvc_filebuf.hpp#L427-L437](https://github.com/microsoft/STL/blob/770da5aa457af11f0a796600e1aa6f7f6fce14a6/stl/inc/__msvc_filebuf.hpp#L427-L437).

## change [_ZNSt13basic_filebufIwSt11char_traitsIwEE5uflowEv]

- [uflow](../../../core/libs/prx/libSceLibcInternal/Filebuf.cpp) (libSceLibcInternal, NID `VrXGNMTgNdE`) - declared from its mangled name with `self` and an `int_type` return, still throwing. The reference [__msvc_filebuf.hpp#L439-L481](https://github.com/microsoft/STL/blob/770da5aa457af11f0a796600e1aa6f7f6fce14a6/stl/inc/__msvc_filebuf.hpp#L439-L481) consumes the get area through the base handles (unnamed 0x08-0x38 in `GuestLocale::Streambuf`), undoes the putback through `_Mychar`, and picks the direct or converting read on `_Pcvt`; none has an offset proved here, and `underflow` cannot be called as a substitute because it is equally pending.

## change [_ZNSt13basic_filebufIcSt11char_traitsIcEE9_EndwriteEv]

- [_Endwrite](../../../core/libs/prx/libSceLibcInternal/Filebuf.cpp) (libSceLibcInternal, NID `JTwt9OTgk1k`) - declared from its mangled name with `self` and no return, still throwing. [__msvc_filebuf.hpp#L668-L699](https://github.com/microsoft/STL/blob/770da5aa457af11f0a796600e1aa6f7f6fce14a6/stl/inc/__msvc_filebuf.hpp#L668-L699) starts by testing `_Pcvt` and `_Wrotesome` and updates `_Wrotesome` and `_State` on the unshift result; no offset in 0x68-0xa7 of the guest object is proved by code here, so even the fast return would guess.

## change [_ZNSt13basic_filebufIwSt11char_traitsIwEE9_EndwriteEv]

- [_Endwrite](../../../core/libs/prx/libSceLibcInternal/Filebuf.cpp) (libSceLibcInternal, NID `cV6KpJiF0Ck`) - declared from its mangled name with `self` and no return, still throwing; same missing `_Pcvt`, `_Wrotesome` and `_State` offsets as the narrow member, against [__msvc_filebuf.hpp#L668-L699](https://github.com/microsoft/STL/blob/770da5aa457af11f0a796600e1aa6f7f6fce14a6/stl/inc/__msvc_filebuf.hpp#L668-L699).

## change [_ZNSt13basic_filebufIcSt11char_traitsIcEE5imbueERKSt6locale]

- [imbue](../../../core/libs/prx/libSceLibcInternal/Filebuf.cpp) (libSceLibcInternal, NID `qM0gUepGWT0`) - declared with `self` and the `GuestLocale::Implementation* const*` locale reference already used by the wide `time_put` `Getcat`, still throwing. The reference [__msvc_filebuf.hpp#L634-L637](https://github.com/microsoft/STL/blob/770da5aa457af11f0a796600e1aa6f7f6fce14a6/stl/inc/__msvc_filebuf.hpp#L634-L637) selects the codecvt facet from the locale and `_Initcvt` writes `_Pcvt` and resets the buffer ([#L705-L713](https://github.com/microsoft/STL/blob/770da5aa457af11f0a796600e1aa6f7f6fce14a6/stl/inc/__msvc_filebuf.hpp#L705-L713)); the facet table of `GuestLocale::Implementation` exists, but `_Pcvt` has no proven guest offset.

## change [_ZNSt13basic_filebufIwSt11char_traitsIwEE5imbueERKSt6locale]

- [imbue](../../../core/libs/prx/libSceLibcInternal/Filebuf.cpp) (libSceLibcInternal, NID `kmzNbhlkddA`) - declared with `self` and the `GuestLocale::Implementation* const*` locale reference, still throwing; same missing `_Pcvt` offset and buffer-reset handles as the narrow member, against [__msvc_filebuf.hpp#L634-L637](https://github.com/microsoft/STL/blob/770da5aa457af11f0a796600e1aa6f7f6fce14a6/stl/inc/__msvc_filebuf.hpp#L634-L637).

## change [_ZNSt13basic_filebufIwSt11char_traitsIwEED1Ev]

- [D1](../../../core/libs/prx/libSceLibcInternal/Filebuf.cpp) (libSceLibcInternal, NID `XcuCO1YXaRs`) - declared from its mangled name with `self` and no return (the Itanium complete destructor), still throwing. The reference [__msvc_filebuf.hpp#L145-L152](https://github.com/microsoft/STL/blob/770da5aa457af11f0a796600e1aa6f7f6fce14a6/stl/inc/__msvc_filebuf.hpp#L145-L152) undoes the putback and closes the file only when `_Closef` is set, through `_Endwrite` ([#L319-L333](https://github.com/microsoft/STL/blob/770da5aa457af11f0a796600e1aa6f7f6fce14a6/stl/inc/__msvc_filebuf.hpp#L319-L333)); `_Closef` has no proven offset, so guessing would close guest files the buffer does not own.
