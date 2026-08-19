// iostream standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef _IOSTREAM_
#define _IOSTREAM_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
// istream standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef _ISTREAM_
#define _ISTREAM_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
// __msvc_ostream.hpp internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef __MSVC_OSTREAM_HPP
#define __MSVC_OSTREAM_HPP
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
#include <ios>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN
#pragma vtordisp(push, 2) // compiler bug workaround

_EXPORT_STD extern "C++" template <class _Elem, class _Traits>
class basic_ostream : virtual public basic_ios<_Elem, _Traits> { // control insertions into a stream buffer
public:
    using _Myios = basic_ios<_Elem, _Traits>;
    using _Mysb  = basic_streambuf<_Elem, _Traits>;
    using _Iter  = ostreambuf_iterator<_Elem, _Traits>;
    using _Nput  = num_put<_Elem, _Iter>;

    explicit __CLR_OR_THIS_CALL basic_ostream(basic_streambuf<_Elem, _Traits>* _Strbuf, bool _Isstd = false) {
        _Myios::init(_Strbuf, _Isstd);
    }

    __CLR_OR_THIS_CALL basic_ostream(_Uninitialized, bool _Addit = true) {
        if (_Addit) {
            this->_Addstd(this); // suppress for basic_iostream
        }
    }

protected:
    __CLR_OR_THIS_CALL basic_ostream(basic_ostream&& _Right) noexcept(false) {
        _Myios::init();
        _Myios::move(_STD move(_Right));
    }

    basic_ostream& __CLR_OR_THIS_CALL operator=(basic_ostream&& _Right) noexcept /* strengthened */ {
        this->swap(_Right);
        return *this;
    }

    void __CLR_OR_THIS_CALL swap(basic_ostream& _Right) noexcept /* strengthened */ {
        if (this != _STD addressof(_Right)) {
            _Myios::swap(_Right);
        }
    }

public:
    __CLR_OR_THIS_CALL basic_ostream(const basic_ostream&)            = delete;
    basic_ostream& __CLR_OR_THIS_CALL operator=(const basic_ostream&) = delete;

    __CLR_OR_THIS_CALL ~basic_ostream() noexcept override {}

    using int_type = typename _Traits::int_type;
    using pos_type = typename _Traits::pos_type;
    using off_type = typename _Traits::off_type;

    class _Sentry_base { // stores thread lock and reference to output stream
    public:
        __CLR_OR_THIS_CALL _Sentry_base(basic_ostream& _Ostr) : _Myostr(_Ostr) { // lock the stream buffer, if there
            const auto _Rdbuf = _Myostr.rdbuf();
            if (_Rdbuf) {
                _Rdbuf->_Lock();
            }
        }

        __CLR_OR_THIS_CALL ~_Sentry_base() noexcept { // destroy after unlocking
            const auto _Rdbuf = _Myostr.rdbuf();
            if (_Rdbuf) {
                _Rdbuf->_Unlock();
            }
        }

        basic_ostream& _Myostr; // the output stream, for _Unlock call at destruction

        _Sentry_base& operator=(const _Sentry_base&) = delete;
    };

    class sentry : public _Sentry_base {
    public:
        explicit __CLR_OR_THIS_CALL sentry(basic_ostream& _Ostr) : _Sentry_base(_Ostr) {
            if (!_Ostr.good()) {
                _Ok = false;
                return;
            }

            const auto _Tied = _Ostr.tie();
            if (!_Tied || _Tied == _STD addressof(_Ostr)) {
                _Ok = true;
                return;
            }

            _Tied->flush();
            _Ok = _Ostr.good(); // store test only after flushing tie
        }

        _STL_DISABLE_DEPRECATED_WARNING
        __CLR_OR_THIS_CALL ~sentry() noexcept {
#if !_HAS_EXCEPTIONS
            const bool _Zero_uncaught_exceptions = true;
#elif _HAS_DEPRECATED_UNCAUGHT_EXCEPTION
            const bool _Zero_uncaught_exceptions = !_STD uncaught_exception(); // TRANSITION, ArchivedOS-12000909
#else // ^^^ _HAS_DEPRECATED_UNCAUGHT_EXCEPTION / !_HAS_DEPRECATED_UNCAUGHT_EXCEPTION vvv
            const bool _Zero_uncaught_exceptions = _STD uncaught_exceptions() == 0;
#endif // ^^^ !_HAS_DEPRECATED_UNCAUGHT_EXCEPTION ^^^

            if (_Zero_uncaught_exceptions) {
                this->_Myostr._Osfx();
            }
        }
        _STL_RESTORE_DEPRECATED_WARNING

        explicit __CLR_OR_THIS_CALL operator bool() const {
            return _Ok;
        }

        __CLR_OR_THIS_CALL sentry(const sentry&)            = delete;
        sentry& __CLR_OR_THIS_CALL operator=(const sentry&) = delete;

    private:
        bool _Ok; // true if stream state okay at construction
    };

#pragma push_macro("opfx")
#pragma push_macro("osfx")
#undef opfx
#undef osfx
    // TRANSITION, ABI: non-Standard opfx() is preserved for binary compatibility
    _DEPRECATE_IO_PFX_SFX bool __CLR_OR_THIS_CALL opfx() { // test stream state and flush tie stream as needed
        if (!this->good()) {
            return false;
        }

        const auto _Tied = _Myios::tie();
        if (!_Tied || _Myios::tie() == this) {
            return true;
        }

        _Tied->flush();
        return this->good();
    }

    // TRANSITION, ABI: non-Standard osfx() is preserved for binary compatibility
    _DEPRECATE_IO_PFX_SFX void __CLR_OR_THIS_CALL osfx() noexcept { // perform any wrapup
        _Osfx();
    }
#pragma pop_macro("osfx")
#pragma pop_macro("opfx")

    void __CLR_OR_THIS_CALL _Osfx() noexcept { // perform any wrapup
        _TRY_BEGIN
        if (this->good() && this->flags() & ios_base::unitbuf) {
            if (_Myios::rdbuf()->pubsync() == -1) { // flush stream as needed
                _Myios::setstate(ios_base::badbit);
            }
        }
        _CATCH_ALL
        _CATCH_END
    }

#ifdef _M_CEE_PURE
    basic_ostream& __CLR_OR_THIS_CALL operator<<(basic_ostream&(__clrcall* _Pfn)(basic_ostream&) ) {
        // call basic_ostream manipulator
        return _Pfn(*this);
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(_Myios&(__clrcall* _Pfn)(_Myios&) ) {
        // call basic_ios manipulator
        _Pfn(*this);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(ios_base&(__clrcall* _Pfn)(ios_base&) ) {
        // call ios_base manipulator
        _Pfn(*this);
        return *this;
    }
#endif // defined(_M_CEE_PURE)

    basic_ostream& __CLR_OR_THIS_CALL operator<<(basic_ostream&(__cdecl* _Pfn)(basic_ostream&) ) {
        // call basic_ostream manipulator
        return _Pfn(*this);
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(_Myios&(__cdecl* _Pfn)(_Myios&) ) {
        // call basic_ios manipulator
        _Pfn(*this);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(ios_base&(__cdecl* _Pfn)(ios_base&) ) {
        // call ios_base manipulator
        _Pfn(*this);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(bool _Val) { // insert a boolean
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (_Ok) { // state okay, use facet to insert
            const _Nput& _Nput_fac = _STD use_facet<_Nput>(this->getloc());

            _TRY_IO_BEGIN
            if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this, _Myios::fill(), _Val).failed()) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(short _Val) { // insert a short
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (_Ok) { // state okay, use facet to insert
            const _Nput& _Nput_fac  = _STD use_facet<_Nput>(this->getloc());
            ios_base::fmtflags _Bfl = this->flags() & ios_base::basefield;

            long _Tmp;
            if (_Bfl == ios_base::oct || _Bfl == ios_base::hex) {
                _Tmp = static_cast<long>(static_cast<unsigned short>(_Val));
            } else {
                _Tmp = static_cast<long>(_Val);
            }

            _TRY_IO_BEGIN
            if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this, _Myios::fill(), _Tmp).failed()) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    // NOTE:
    // If you are not using native wchar_t, the unsigned short inserter
    // is masked by an explicit specialization that treats an unsigned
    // short as a wide character.

    // To read or write unsigned shorts as integers with wchar_t streams,
    // make wchar_t a native type with the command line option /Zc:wchar_t.

    basic_ostream& __CLR_OR_THIS_CALL operator<<(unsigned short _Val) { // insert an unsigned short
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (_Ok) { // state okay, use facet to insert
            const _Nput& _Nput_fac = _STD use_facet<_Nput>(this->getloc());

            _TRY_IO_BEGIN
            if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this, _Myios::fill(), static_cast<unsigned long>(_Val))
                    .failed()) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(int _Val) { // insert an int
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (_Ok) { // state okay, use facet to insert
            const _Nput& _Nput_fac  = _STD use_facet<_Nput>(this->getloc());
            ios_base::fmtflags _Bfl = this->flags() & ios_base::basefield;

            long _Tmp;
            if (_Bfl == ios_base::oct || _Bfl == ios_base::hex) {
                _Tmp = static_cast<long>(static_cast<unsigned int>(_Val));
            } else {
                _Tmp = static_cast<long>(_Val);
            }

            _TRY_IO_BEGIN
            if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this, _Myios::fill(), _Tmp).failed()) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(unsigned int _Val) { // insert an unsigned int
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (_Ok) { // state okay, use facet to insert
            const _Nput& _Nput_fac = _STD use_facet<_Nput>(this->getloc());

            _TRY_IO_BEGIN
            if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this, _Myios::fill(), static_cast<unsigned long>(_Val))
                    .failed()) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(long _Val) { // insert a long
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (_Ok) { // state okay, use facet to insert
            const _Nput& _Nput_fac = _STD use_facet<_Nput>(this->getloc());

            _TRY_IO_BEGIN
            if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this, _Myios::fill(), _Val).failed()) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(unsigned long _Val) { // insert an unsigned long
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (_Ok) { // state okay, use facet to insert
            const _Nput& _Nput_fac = _STD use_facet<_Nput>(this->getloc());

            _TRY_IO_BEGIN
            if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this, _Myios::fill(), _Val).failed()) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(long long _Val) { // insert a long long
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (_Ok) { // state okay, use facet to insert
            const _Nput& _Nput_fac = _STD use_facet<_Nput>(this->getloc());

            _TRY_IO_BEGIN
            if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this, _Myios::fill(), _Val).failed()) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(unsigned long long _Val) { // insert an unsigned long long
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (_Ok) { // state okay, use facet to insert
            const _Nput& _Nput_fac = _STD use_facet<_Nput>(this->getloc());

            _TRY_IO_BEGIN
            if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this, _Myios::fill(), _Val).failed()) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(float _Val) { // insert a float
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (_Ok) { // state okay, use facet to insert
            const _Nput& _Nput_fac = _STD use_facet<_Nput>(this->getloc());

            _TRY_IO_BEGIN
            if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this, _Myios::fill(), static_cast<double>(_Val)).failed()) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(double _Val) { // insert a double
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (_Ok) { // state okay, use facet to insert
            const _Nput& _Nput_fac = _STD use_facet<_Nput>(this->getloc());

            _TRY_IO_BEGIN
            if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this, _Myios::fill(), _Val).failed()) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(long double _Val) { // insert a long double
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (_Ok) { // state okay, use facet to insert
            const _Nput& _Nput_fac = _STD use_facet<_Nput>(this->getloc());

            _TRY_IO_BEGIN
            if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this, _Myios::fill(), _Val).failed()) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(const void* _Val) { // insert a void pointer
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (_Ok) { // state okay, use facet to insert
            const _Nput& _Nput_fac = _STD use_facet<_Nput>(this->getloc());

            _TRY_IO_BEGIN
            if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this, _Myios::fill(), _Val).failed()) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

#if _HAS_CXX23
    template <class = void> // TRANSITION, ABI
    basic_ostream& operator<<(const volatile void* _Val) {
        return *this << const_cast<const void*>(_Val);
    }
#endif // _HAS_CXX23

#if _HAS_CXX17
    template <class = void> // TRANSITION, ABI
    basic_ostream& operator<<(nullptr_t) { // insert a null pointer
        return *this << "nullptr";
    }
#endif // _HAS_CXX17

    basic_ostream& __CLR_OR_THIS_CALL operator<<(_Mysb* _Strbuf) { // insert until end-of-file from a stream buffer
        ios_base::iostate _State = ios_base::goodbit;
        bool _Copied             = false;
        const sentry _Ok(*this);

        if (_Ok && _Strbuf) {
            for (int_type _Meta = _Traits::eof();; _Copied = true) { // extract another character from stream buffer
                _TRY_BEGIN
                _Meta = _Traits::eq_int_type(_Traits::eof(), _Meta) ? _Strbuf->sgetc() : _Strbuf->snextc();
                _CATCH_ALL
                // N4971 [ostream.inserters]/9: "If an exception was thrown
                // while extracting a character, the function sets failbit in the error state,
                // and if failbit is set in exceptions() the caught exception is rethrown."
                _Myios::setstate(ios_base::failbit, _Myios::exceptions() == ios_base::failbit);
                _CATCH_END

                if (_Traits::eq_int_type(_Traits::eof(), _Meta)) {
                    break; // end of file, quit
                }

                _TRY_IO_BEGIN
                if (_Traits::eq_int_type(_Traits::eof(), _Myios::rdbuf()->sputc(_Traits::to_char_type(_Meta)))) {
                    _State |= ios_base::badbit; // insertion failed, quit
                    break;
                }
                _CATCH_IO_END
            }
        }

        this->width(0);
        int _Setstate_with;
        if (_Strbuf) {
            if (_Copied) {
                _Setstate_with = _State;
            } else {
                _Setstate_with = _State | ios_base::failbit;
            }
        } else {
            _Setstate_with = ios_base::badbit;
        }

        _Myios::setstate(_Setstate_with);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL put(_Elem _Ch) { // insert a character
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (!_Ok) {
            _State |= ios_base::badbit;
        } else { // state okay, insert character
            _TRY_IO_BEGIN
            if (_Traits::eq_int_type(_Traits::eof(), _Myios::rdbuf()->sputc(_Ch))) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL write(const _Elem* _Str, streamsize _Count) {
        // insert _Count characters from array _Str
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (!_Ok) {
            _State |= ios_base::badbit;
        } else if (0 < _Count) { // state okay, insert characters
            _TRY_IO_BEGIN
            if (_Myios::rdbuf()->sputn(_Str, _Count) != _Count) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL flush() { // flush output stream
        const auto _Rdbuf = _Myios::rdbuf();
        if (_Rdbuf) { // buffer exists, flush it
            const sentry _Ok(*this);

            if (_Ok) {
                ios_base::iostate _State = ios_base::goodbit;
                _TRY_IO_BEGIN
                if (_Rdbuf->pubsync() == -1) {
                    _State |= ios_base::badbit; // sync failed
                }
                _CATCH_IO_END
                _Myios::setstate(_State);
            }
        }
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL seekp(pos_type _Pos) { // set output stream position to _Pos
        const sentry _Ok(*this);

        if (!this->fail()) {
            ios_base::iostate _State = ios_base::goodbit;
            _TRY_IO_BEGIN
            if (static_cast<off_type>(_Myios::rdbuf()->pubseekpos(_Pos, ios_base::out)) == -1) {
                _State |= ios_base::failbit; // seek failed
            }
            _CATCH_IO_END
            _Myios::setstate(_State);
        }

        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL seekp(off_type _Off, ios_base::seekdir _Way) {
        // change output stream position by _Off, according to _Way
        const sentry _Ok(*this);

        if (!this->fail()) {
            ios_base::iostate _State = ios_base::goodbit;
            _TRY_IO_BEGIN
            if (static_cast<off_type>(_Myios::rdbuf()->pubseekoff(_Off, _Way, ios_base::out)) == -1) {
                _State |= ios_base::failbit; // seek failed
            }
            _CATCH_IO_END
            _Myios::setstate(_State);
        }

        return *this;
    }

    pos_type __CLR_OR_THIS_CALL tellp() {
        const sentry _Ok(*this);

        if (!this->fail()) {
            _TRY_IO_BEGIN
            return _Myios::rdbuf()->pubseekoff(0, ios_base::cur, ios_base::out);
            _CATCH_IO_END
        }

        return pos_type{off_type{-1}};
    }
};

#pragma vtordisp(pop) // compiler bug workaround

#ifndef _NATIVE_WCHAR_T_DEFINED
// NOTE:
// If you are not using native wchar_t, the following explicit
// specialization will mask the member function (above) that treats
// an unsigned short as an integer.

// To read or write unsigned shorts as integers with wchar_t streams,
// make wchar_t a native type with the command line option /Zc:wchar_t.

template <>
inline basic_ostream<unsigned short, char_traits<unsigned short>>& __CLR_OR_THIS_CALL basic_ostream<unsigned short,
    char_traits<unsigned short>>::operator<<(unsigned short _Ch) { // extract a character
    using _Traits = char_traits<unsigned short>;

    ios_base::iostate _State = ios_base::goodbit;
    const sentry _Ok(*this);

    if (_Ok) { // state okay, insert
        streamsize _Pad = this->width() <= 1 ? 0 : this->width() - 1;

        _TRY_IO_BEGIN
        if ((this->flags() & ios_base::adjustfield) != ios_base::left) {
            for (; _State == ios_base::goodbit && 0 < _Pad; --_Pad) { // pad on left
                if (_Traits::eq_int_type(_Traits::eof(), this->rdbuf()->sputc(this->fill()))) {
                    _State |= ios_base::badbit;
                }
            }
        }

        if (_State == ios_base::goodbit && _Traits::eq_int_type(_Traits::eof(), this->rdbuf()->sputc(_Ch))) {
            _State |= ios_base::badbit;
        }

        for (; _State == ios_base::goodbit && 0 < _Pad; --_Pad) { // pad on right
            if (_Traits::eq_int_type(_Traits::eof(), this->rdbuf()->sputc(this->fill()))) {
                _State |= ios_base::badbit;
            }
        }
        _CATCH_IO_END
    }

    this->width(0);
    _Myios::setstate(_State);
    return *this;
}
#endif // _NATIVE_WCHAR_T_DEFINED

#if defined(_DLL_CPPLIB)

#if !defined(_CRTBLD) || defined(__FORCE_INSTANCE)
template class _CRTIMP2_PURE_IMPORT basic_ostream<char, char_traits<char>>;
template class _CRTIMP2_PURE_IMPORT basic_ostream<wchar_t, char_traits<wchar_t>>;
#endif // !defined(_CRTBLD) || defined(__FORCE_INSTANCE)

#ifdef __FORCE_INSTANCE
template class _CRTIMP2_PURE_IMPORT basic_ostream<unsigned short, char_traits<unsigned short>>;
#endif // defined(__FORCE_INSTANCE)
#endif // defined(_DLL_CPPLIB)

_EXPORT_STD template <class _Elem, class _Traits>
basic_ostream<_Elem, _Traits>& operator<<(basic_ostream<_Elem, _Traits>& _Ostr, const char* _Val) { // insert NTBS
    ios_base::iostate _State = ios_base::goodbit;
    streamsize _Count        = static_cast<streamsize>(_CSTD strlen(_Val));
    streamsize _Pad          = _Ostr.width() <= 0 || _Ostr.width() <= _Count ? 0 : _Ostr.width() - _Count;
    const typename basic_ostream<_Elem, _Traits>::sentry _Ok(_Ostr);

    if (!_Ok) {
        _State |= ios_base::badbit;
    } else { // state okay, insert characters
        _TRY_IO_BEGIN
        const ctype<_Elem>& _Ctype_fac = _STD use_facet<ctype<_Elem>>(_Ostr.getloc());
        if ((_Ostr.flags() & ios_base::adjustfield) != ios_base::left) {
            for (; 0 < _Pad; --_Pad) { // pad on left
                if (_Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ostr.fill()))) {
                    _State |= ios_base::badbit; // insertion failed, quit
                    break;
                }
            }
        }

        for (; _State == ios_base::goodbit && 0 < _Count; --_Count, ++_Val) {
            if (_Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ctype_fac.widen(*_Val)))) {
                _State |= ios_base::badbit;
            }
        }

        if (_State == ios_base::goodbit) {
            for (; 0 < _Pad; --_Pad) { // pad on right
                if (_Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ostr.fill()))) {
                    _State |= ios_base::badbit; // insertion failed, quit
                    break;
                }
            }
        }

        _Ostr.width(0);
        _CATCH_IO_(ios_base, _Ostr)
    }

    _Ostr.setstate(_State);
    return _Ostr;
}

_EXPORT_STD template <class _Elem, class _Traits>
basic_ostream<_Elem, _Traits>& operator<<(basic_ostream<_Elem, _Traits>& _Ostr, char _Ch) { // insert a character
    ios_base::iostate _State = ios_base::goodbit;
    const typename basic_ostream<_Elem, _Traits>::sentry _Ok(_Ostr);

    if (_Ok) { // state okay, insert
        const ctype<_Elem>& _Ctype_fac = _STD use_facet<ctype<_Elem>>(_Ostr.getloc());
        streamsize _Pad                = _Ostr.width() <= 1 ? 0 : _Ostr.width() - 1;

        _TRY_IO_BEGIN
        if ((_Ostr.flags() & ios_base::adjustfield) != ios_base::left) {
            for (; _State == ios_base::goodbit && 0 < _Pad; --_Pad) { // pad on left
                if (_Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ostr.fill()))) {
                    _State |= ios_base::badbit;
                }
            }
        }

        if (_State == ios_base::goodbit
            && _Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ctype_fac.widen(_Ch)))) {
            _State |= ios_base::badbit;
        }

        for (; _State == ios_base::goodbit && 0 < _Pad; --_Pad) { // pad on right
            if (_Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ostr.fill()))) {
                _State |= ios_base::badbit;
            }
        }
        _CATCH_IO_(ios_base, _Ostr)
    }

    _Ostr.width(0);
    _Ostr.setstate(_State);
    return _Ostr;
}

_EXPORT_STD template <class _Traits>
basic_ostream<char, _Traits>& operator<<(basic_ostream<char, _Traits>& _Ostr, const char* _Val) {
    // insert NTBS into char stream
    using _Elem = char;
    using _Myos = basic_ostream<_Elem, _Traits>;

    ios_base::iostate _State = ios_base::goodbit;
    streamsize _Count        = static_cast<streamsize>(_Traits::length(_Val));
    streamsize _Pad          = _Ostr.width() <= 0 || _Ostr.width() <= _Count ? 0 : _Ostr.width() - _Count;
    const typename _Myos::sentry _Ok(_Ostr);

    if (!_Ok) {
        _State |= ios_base::badbit;
    } else { // state okay, insert
        _TRY_IO_BEGIN
        if ((_Ostr.flags() & ios_base::adjustfield) != ios_base::left) {
            for (; 0 < _Pad; --_Pad) { // pad on left
                if (_Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ostr.fill()))) {
                    _State |= ios_base::badbit; // insertion failed, quit
                    break;
                }
            }
        }

        if (_State == ios_base::goodbit && _Ostr.rdbuf()->sputn(_Val, _Count) != _Count) {
            _State |= ios_base::badbit;
        }

        if (_State == ios_base::goodbit) {
            for (; 0 < _Pad; --_Pad) { // pad on right
                if (_Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ostr.fill()))) {
                    _State |= ios_base::badbit; // insertion failed, quit
                    break;
                }
            }
        }

        _Ostr.width(0);
        _CATCH_IO_(ios_base, _Ostr)
    }

    _Ostr.setstate(_State);
    return _Ostr;
}

_EXPORT_STD template <class _Traits>
basic_ostream<char, _Traits>& operator<<(basic_ostream<char, _Traits>& _Ostr, char _Ch) {
    // insert a char into char stream
    using _Elem = char;
    using _Myos = basic_ostream<_Elem, _Traits>;

    ios_base::iostate _State = ios_base::goodbit;
    const typename _Myos::sentry _Ok(_Ostr);

    if (_Ok) { // state okay, insert
        streamsize _Pad = _Ostr.width() <= 1 ? 0 : _Ostr.width() - 1;

        _TRY_IO_BEGIN
        if ((_Ostr.flags() & ios_base::adjustfield) != ios_base::left) {
            for (; _State == ios_base::goodbit && 0 < _Pad; --_Pad) { // pad on left
                if (_Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ostr.fill()))) {
                    _State |= ios_base::badbit;
                }
            }
        }

        if (_State == ios_base::goodbit && _Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ch))) {
            _State |= ios_base::badbit;
        }

        for (; _State == ios_base::goodbit && 0 < _Pad; --_Pad) { // pad on right
            if (_Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ostr.fill()))) {
                _State |= ios_base::badbit;
            }
        }
        _CATCH_IO_(ios_base, _Ostr)
    }

    _Ostr.width(0);
    _Ostr.setstate(_State);
    return _Ostr;
}

_EXPORT_STD template <class _Elem, class _Traits>
basic_ostream<_Elem, _Traits>& operator<<(basic_ostream<_Elem, _Traits>& _Ostr, const _Elem* _Val) { // insert NTCS
    using _Myos = basic_ostream<_Elem, _Traits>;

    ios_base::iostate _State = ios_base::goodbit;
    streamsize _Count        = static_cast<streamsize>(_Traits::length(_Val));
    streamsize _Pad          = _Ostr.width() <= 0 || _Ostr.width() <= _Count ? 0 : _Ostr.width() - _Count;
    const typename _Myos::sentry _Ok(_Ostr);

    if (!_Ok) {
        _State |= ios_base::badbit;
    } else { // state okay, insert
        _TRY_IO_BEGIN
        if ((_Ostr.flags() & ios_base::adjustfield) != ios_base::left) {
            for (; 0 < _Pad; --_Pad) { // pad on left
                if (_Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ostr.fill()))) {
                    _State |= ios_base::badbit; // insertion failed, quit
                    break;
                }
            }
        }

        if (_State == ios_base::goodbit && _Ostr.rdbuf()->sputn(_Val, _Count) != _Count) {
            _State |= ios_base::badbit;
        }

        if (_State == ios_base::goodbit) {
            for (; 0 < _Pad; --_Pad) { // pad on right
                if (_Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ostr.fill()))) {
                    _State |= ios_base::badbit; // insertion failed, quit
                    break;
                }
            }
        }

        _Ostr.width(0);
        _CATCH_IO_(ios_base, _Ostr)
    }

    _Ostr.setstate(_State);
    return _Ostr;
}

_EXPORT_STD template <class _Elem, class _Traits>
basic_ostream<_Elem, _Traits>& operator<<(basic_ostream<_Elem, _Traits>& _Ostr, _Elem _Ch) { // insert a character
    using _Myos = basic_ostream<_Elem, _Traits>;

    ios_base::iostate _State = ios_base::goodbit;
    const typename _Myos::sentry _Ok(_Ostr);

    if (_Ok) { // state okay, insert
        streamsize _Pad = _Ostr.width() <= 1 ? 0 : _Ostr.width() - 1;

        _TRY_IO_BEGIN
        if ((_Ostr.flags() & ios_base::adjustfield) != ios_base::left) {
            for (; _State == ios_base::goodbit && 0 < _Pad; --_Pad) { // pad on left
                if (_Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ostr.fill()))) {
                    _State |= ios_base::badbit;
                }
            }
        }

        if (_State == ios_base::goodbit && _Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ch))) {
            _State |= ios_base::badbit;
        }

        for (; _State == ios_base::goodbit && 0 < _Pad; --_Pad) { // pad on right
            if (_Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ostr.fill()))) {
                _State |= ios_base::badbit;
            }
        }
        _CATCH_IO_(ios_base, _Ostr)
    }

    _Ostr.width(0);
    _Ostr.setstate(_State);
    return _Ostr;
}

_EXPORT_STD template <class _Traits>
basic_ostream<char, _Traits>& operator<<(basic_ostream<char, _Traits>& _Ostr, const signed char* _Val) {
    // insert a signed char NTBS
    return _Ostr << reinterpret_cast<const char*>(_Val);
}

_EXPORT_STD template <class _Traits>
basic_ostream<char, _Traits>& operator<<(basic_ostream<char, _Traits>& _Ostr, signed char _Ch) { // insert a signed char
    return _Ostr << static_cast<char>(_Ch);
}

_EXPORT_STD template <class _Traits>
basic_ostream<char, _Traits>& operator<<(basic_ostream<char, _Traits>& _Ostr, const unsigned char* _Val) {
    // insert an unsigned char NTBS
    return _Ostr << reinterpret_cast<const char*>(_Val);
}

_EXPORT_STD template <class _Traits>
basic_ostream<char, _Traits>& operator<<(basic_ostream<char, _Traits>& _Ostr, unsigned char _Ch) {
    // insert an unsigned char
    return _Ostr << static_cast<char>(_Ch);
}

#ifdef __cpp_char8_t // These deleted overloads are specified in P1423.
// don't insert a UTF-8 NTBS
_EXPORT_STD template <class _Traits>
basic_ostream<char, _Traits>& operator<<(basic_ostream<char, _Traits>&, const char8_t*) = delete;
_EXPORT_STD template <class _Traits>
basic_ostream<wchar_t, _Traits>& operator<<(basic_ostream<wchar_t, _Traits>&, const char8_t*) = delete;

// don't insert a UTF-8 code unit
_EXPORT_STD template <class _Traits>
basic_ostream<char, _Traits>& operator<<(basic_ostream<char, _Traits>&, char8_t) = delete;
_EXPORT_STD template <class _Traits>
basic_ostream<wchar_t, _Traits>& operator<<(basic_ostream<wchar_t, _Traits>&, char8_t) = delete;
#endif // defined(__cpp_char8_t)

#if !_HAS_STREAM_INSERTION_OPERATORS_DELETED_IN_CXX20
#ifdef _NATIVE_WCHAR_T_DEFINED
_EXPORT_STD template <class _Traits>
basic_ostream<char, _Traits>& operator<<(basic_ostream<char, _Traits>&, wchar_t) = delete;

_EXPORT_STD template <class _Traits>
basic_ostream<char, _Traits>& operator<<(basic_ostream<char, _Traits>&, const wchar_t*) = delete;
#endif // _NATIVE_WCHAR_T_DEFINED

_EXPORT_STD template <class _Traits>
basic_ostream<char, _Traits>& operator<<(basic_ostream<char, _Traits>&, char16_t) = delete;
_EXPORT_STD template <class _Traits>
basic_ostream<char, _Traits>& operator<<(basic_ostream<char, _Traits>&, char32_t) = delete;

_EXPORT_STD template <class _Traits>
basic_ostream<wchar_t, _Traits>& operator<<(basic_ostream<wchar_t, _Traits>&, char16_t) = delete;
_EXPORT_STD template <class _Traits>
basic_ostream<wchar_t, _Traits>& operator<<(basic_ostream<wchar_t, _Traits>&, char32_t) = delete;

_EXPORT_STD template <class _Traits>
basic_ostream<char, _Traits>& operator<<(basic_ostream<char, _Traits>&, const char16_t*) = delete;
_EXPORT_STD template <class _Traits>
basic_ostream<char, _Traits>& operator<<(basic_ostream<char, _Traits>&, const char32_t*) = delete;

_EXPORT_STD template <class _Traits>
basic_ostream<wchar_t, _Traits>& operator<<(basic_ostream<wchar_t, _Traits>&, const char16_t*) = delete;
_EXPORT_STD template <class _Traits>
basic_ostream<wchar_t, _Traits>& operator<<(basic_ostream<wchar_t, _Traits>&, const char32_t*) = delete;
#endif // !_HAS_STREAM_INSERTION_OPERATORS_DELETED_IN_CXX20

template <class _Ostr, class _Ty, class = void>
struct _Can_stream_out : false_type {};

template <class _Ostr, class _Ty>
struct _Can_stream_out<_Ostr, _Ty, void_t<decltype(_STD declval<_Ostr&>() << _STD declval<const _Ty&>())>> : true_type {
};

_EXPORT_STD template <class _Ostr, class _Ty,
    enable_if_t<conjunction_v<is_convertible<_Ostr*, ios_base*>, _Can_stream_out<_Ostr, _Ty>>, int> = 0>
_Ostr&& operator<<(_Ostr&& _Os, const _Ty& _Val) { // insert to rvalue stream
    _Os << _Val;
    return _STD move(_Os);
}

_EXPORT_STD template <class _Elem, class _Traits>
basic_ostream<_Elem, _Traits>& __CLRCALL_OR_CDECL endl(
    basic_ostream<_Elem, _Traits>& _Ostr) { // insert newline and flush stream
    _Ostr.put(_Ostr.widen('\n'));
    _Ostr.flush();
    return _Ostr;
}

_EXPORT_STD template <class _Elem, class _Traits>
basic_ostream<_Elem, _Traits>& __CLRCALL_OR_CDECL ends(basic_ostream<_Elem, _Traits>& _Ostr) { // insert null character
    _Ostr.put(_Elem());
    return _Ostr;
}

_EXPORT_STD template <class _Elem, class _Traits>
basic_ostream<_Elem, _Traits>& __CLRCALL_OR_CDECL flush(basic_ostream<_Elem, _Traits>& _Ostr) { // flush stream
    _Ostr.flush();
    return _Ostr;
}

_EXPORT_STD template <class _Elem, class _Traits>
basic_ostream<_Elem, _Traits>& operator<<(basic_ostream<_Elem, _Traits>& _Ostr, const error_code& _Errcode) {
    // display error code
    return _Ostr << _Errcode.category().name() << ':' << _Errcode.value();
}
_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)

#endif // _STL_COMPILER_PREPROCESSOR
#endif // __MSVC_OSTREAM_HPP


#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN
template <class _Elem>
struct _NODISCARD _Null_terminator_guard {
    _Elem** _Str_ref;
    ~_Null_terminator_guard() {
        if (_Str_ref) {
            **_Str_ref = _Elem(); // add terminating null character
        }
    }
};

#pragma vtordisp(push, 2) // compiler bug workaround

_EXPORT_STD extern "C++" template <class _Elem, class _Traits>
class basic_istream : virtual public basic_ios<_Elem, _Traits> { // control extractions from a stream buffer
public:
    using _Myios = basic_ios<_Elem, _Traits>;
    using _Mysb  = basic_streambuf<_Elem, _Traits>;
    using _Iter  = istreambuf_iterator<_Elem, _Traits>;
    using _Ctype = ctype<_Elem>;
    using _Nget  = num_get<_Elem, _Iter>;

#if defined(__FORCE_INSTANCE)
    explicit __CLR_OR_THIS_CALL basic_istream(_Mysb* _Strbuf, bool _Isstd, bool _Skip_init) : _Chcount(0) {
        if (!_Skip_init) {
            _Myios::init(_Strbuf, _Isstd);
        }
    }
#endif // defined(__FORCE_INSTANCE)

    explicit __CLR_OR_THIS_CALL basic_istream(_Mysb* _Strbuf, bool _Isstd = false) : _Chcount(0) {
        _Myios::init(_Strbuf, _Isstd);
    }

    __CLR_OR_THIS_CALL basic_istream(_Uninitialized) {
        this->_Addstd(this);
    }

protected:
    __CLR_OR_THIS_CALL basic_istream(basic_istream&& _Right) noexcept(false) : _Chcount(_Right._Chcount) {
        _Myios::init();
        _Myios::move(_STD move(_Right));
        _Right._Chcount = 0;
    }

    basic_istream& __CLR_OR_THIS_CALL operator=(basic_istream&& _Right) noexcept /* strengthened */ {
        this->swap(_Right);
        return *this;
    }

    void __CLR_OR_THIS_CALL swap(basic_istream& _Right) noexcept /* strengthened */ {
        _Myios::swap(_Right);
        _STD swap(_Chcount, _Right._Chcount);
    }

public:
    __CLR_OR_THIS_CALL basic_istream(const basic_istream&)            = delete;
    basic_istream& __CLR_OR_THIS_CALL operator=(const basic_istream&) = delete;

    __CLR_OR_THIS_CALL ~basic_istream() noexcept override {}

    using int_type = typename _Traits::int_type;
    using pos_type = typename _Traits::pos_type;
    using off_type = typename _Traits::off_type;

    class _Sentry_base {
    public:
        __CLR_OR_THIS_CALL _Sentry_base(basic_istream& _Istr) : _Myistr(_Istr) {
            const auto _Rdbuf = _Myistr.rdbuf();
            if (_Rdbuf) {
                _Rdbuf->_Lock();
            }
        }

        __CLR_OR_THIS_CALL ~_Sentry_base() noexcept {
            const auto _Rdbuf = _Myistr.rdbuf();
            if (_Rdbuf) {
                _Rdbuf->_Unlock();
            }
        }

        basic_istream& _Myistr;

        _Sentry_base& operator=(const _Sentry_base&) = delete;
    };

    class sentry : public _Sentry_base {
    public:
        explicit __CLR_OR_THIS_CALL sentry(basic_istream& _Istr, bool _Noskip = false)
            : _Sentry_base(_Istr), _Ok(_Sentry_base::_Myistr._Ipfx(_Noskip)) {}

        explicit __CLR_OR_THIS_CALL operator bool() const {
            return _Ok;
        }

        __CLR_OR_THIS_CALL sentry(const sentry&)            = delete;
        sentry& __CLR_OR_THIS_CALL operator=(const sentry&) = delete;

    private:
        bool _Ok; // true if _Ipfx succeeded at construction
    };

    bool __CLR_OR_THIS_CALL _Ipfx(bool _Noskip = false) { // test stream state and skip whitespace as needed
        if (!this->good()) {
            _Myios::setstate(ios_base::failbit);
            return false;
        }

        // state okay, flush tied stream and skip whitespace
        const auto _Tied = _Myios::tie();
        if (_Tied) {
            _Tied->flush();
        }

        bool _Eof = false;
        if (!_Noskip && this->flags() & ios_base::skipws) { // skip whitespace
            const _Ctype& _Ctype_fac = _STD use_facet<_Ctype>(this->getloc());

            _TRY_IO_BEGIN
            int_type _Meta = _Myios::rdbuf()->sgetc();

            for (;; _Meta = _Myios::rdbuf()->snextc()) {
                if (_Traits::eq_int_type(_Traits::eof(), _Meta)) { // end of file, quit
                    _Eof = true;
                    break;
                } else if (!_Ctype_fac.is(_Ctype::space, _Traits::to_char_type(_Meta))) {
                    break; // not whitespace, quit
                }
            }
            _CATCH_IO_END
        }

        if (_Eof) {
            _Myios::setstate(ios_base::eofbit | ios_base::failbit);
        }

        return this->good();
    }

#pragma push_macro("ipfx")
#pragma push_macro("isfx")
#undef ipfx
#undef isfx
    // TRANSITION, ABI: non-Standard ipfx() is preserved for binary compatibility
    _DEPRECATE_IO_PFX_SFX bool __CLR_OR_THIS_CALL ipfx(bool _Noskip = false) {
        // test stream state and skip whitespace as needed
        return _Ipfx(_Noskip);
    }

    // TRANSITION, ABI: non-Standard isfx() is preserved for binary compatibility
    _DEPRECATE_IO_PFX_SFX void __CLR_OR_THIS_CALL isfx() {} // perform any wrapup
#pragma pop_macro("isfx")
#pragma pop_macro("ipfx")

#ifdef _M_CEE_PURE
    basic_istream& __CLR_OR_THIS_CALL operator>>(basic_istream&(__clrcall* _Pfn)(basic_istream&) ) {
        // call basic_istream manipulator
        return _Pfn(*this);
    }

    basic_istream& __CLR_OR_THIS_CALL operator>>(_Myios&(__clrcall* _Pfn)(_Myios&) ) { // call basic_ios manipulator
        _Pfn(*this);
        return *this;
    }

    basic_istream& __CLR_OR_THIS_CALL operator>>(ios_base&(__clrcall* _Pfn)(ios_base&) ) { // call ios_base manipulator
        _Pfn(*this);
        return *this;
    }
#endif // defined(_M_CEE_PURE)

    basic_istream& __CLR_OR_THIS_CALL operator>>(basic_istream&(__cdecl* _Pfn)(basic_istream&) ) {
        // call basic_istream manipulator
        return _Pfn(*this);
    }

    basic_istream& __CLR_OR_THIS_CALL operator>>(_Myios&(__cdecl* _Pfn)(_Myios&) ) { // call basic_ios manipulator
        _Pfn(*this);
        return *this;
    }

    basic_istream& __CLR_OR_THIS_CALL operator>>(ios_base&(__cdecl* _Pfn)(ios_base&) ) { // call ios_base manipulator
        _Pfn(*this);
        return *this;
    }

private:
    template <class _Ty>
    basic_istream& _Common_extract_with_num_get(_Ty& _Val) { // formatted extract with num_get
        ios_base::iostate _Err = ios_base::goodbit;
        const sentry _Ok(*this);

        if (_Ok) { // state okay, use facet to extract
            _TRY_IO_BEGIN
            _STD use_facet<_Nget>(this->getloc()).get(*this, {}, *this, _Err, _Val);
            _CATCH_IO_END
        }

        _Myios::setstate(_Err);
        return *this;
    }

    template <class = void>
    void _Increment_gcount() noexcept {
        if (_Chcount != _STD _Max_limit<streamsize>()) {
            ++_Chcount;
        }
    }

public:
    basic_istream& __CLR_OR_THIS_CALL operator>>(bool& _Val) { // extract a boolean
        return _Common_extract_with_num_get(_Val);
    }

    basic_istream& __CLR_OR_THIS_CALL operator>>(short& _Val) { // extract a short
        ios_base::iostate _Err = ios_base::goodbit;
        const sentry _Ok(*this);

        if (_Ok) { // state okay, use facet to extract
            _TRY_IO_BEGIN
            long _Lval;
            _STD use_facet<_Nget>(this->getloc()).get(*this, {}, *this, _Err, _Lval);
            if (_Lval < SHRT_MIN) {
                _Err |= ios_base::failbit;
                _Val = SHRT_MIN;
            } else if (_Lval > SHRT_MAX) {
                _Err |= ios_base::failbit;
                _Val = SHRT_MAX;
            } else {
                _Val = static_cast<short>(_Lval);
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_Err);
        return *this;
    }

    // NOTE:
    // If you are not using native wchar_t, the unsigned short extractor
    // is masked by an explicit specialization that treats an unsigned
    // short as a wide character.

    // To read or write unsigned shorts as integers with wchar_t streams,
    // make wchar_t a native type with the command line option /Zc:wchar_t.

    basic_istream& __CLR_OR_THIS_CALL operator>>(unsigned short& _Val) { // extract an unsigned short
        return _Common_extract_with_num_get(_Val);
    }

    basic_istream& __CLR_OR_THIS_CALL operator>>(int& _Val) { // extract an int
        static_assert(sizeof(int) == sizeof(long), "Bad overflow assumptions due to sizeof(int) != sizeof(long)");
        long _Result = _Val;
        _Common_extract_with_num_get(_Result);
        _Val = _Result;
        return *this;
    }

    basic_istream& __CLR_OR_THIS_CALL operator>>(unsigned int& _Val) { // extract an unsigned int
        return _Common_extract_with_num_get(_Val);
    }

    basic_istream& __CLR_OR_THIS_CALL operator>>(long& _Val) { // extract a long
        return _Common_extract_with_num_get(_Val);
    }

    basic_istream& __CLR_OR_THIS_CALL operator>>(unsigned long& _Val) { // extract an unsigned long
        return _Common_extract_with_num_get(_Val);
    }

    basic_istream& __CLR_OR_THIS_CALL operator>>(long long& _Val) { // extract a long long
        return _Common_extract_with_num_get(_Val);
    }

    basic_istream& __CLR_OR_THIS_CALL operator>>(unsigned long long& _Val) { // extract an unsigned long long
        return _Common_extract_with_num_get(_Val);
    }

    basic_istream& __CLR_OR_THIS_CALL operator>>(float& _Val) { // extract a float
        return _Common_extract_with_num_get(_Val);
    }

    basic_istream& __CLR_OR_THIS_CALL operator>>(double& _Val) { // extract a double
        return _Common_extract_with_num_get(_Val);
    }

    basic_istream& __CLR_OR_THIS_CALL operator>>(long double& _Val) { // extract a long double
        return _Common_extract_with_num_get(_Val);
    }

    basic_istream& __CLR_OR_THIS_CALL operator>>(void*& _Val) { // extract a void pointer
        return _Common_extract_with_num_get(_Val);
    }

    basic_istream& __CLR_OR_THIS_CALL operator>>(_Mysb* _Strbuf) { // extract until end-of-file into a stream buffer
        _Chcount = 0; // behaves as an unformatted input function
        const sentry _Ok(*this, true);
        ios_base::iostate _State = ios_base::goodbit;
        if (_Ok && _Strbuf) { // state okay, extract characters
            _TRY_IO_BEGIN
            for (int_type _Meta = _Myios::rdbuf()->sgetc();; _Meta = _Myios::rdbuf()->snextc()) {
                if (_Traits::eq_int_type(_Traits::eof(), _Meta)) { // end of file, quit
                    _State |= ios_base::eofbit;
                    break;
                }
                // got a character, insert it into buffer
                _TRY_BEGIN
                if (_Traits::eq_int_type(_Traits::eof(), _Strbuf->sputc(_Traits::to_char_type(_Meta)))) {
                    break;
                }

                _CATCH_ALL
                break;
                _CATCH_END

                _Increment_gcount();
            }
            _CATCH_IO_END
        }

        if (_Chcount == 0) { // If the function inserts no characters, it calls setstate(failbit)
            _State |= ios_base::failbit;
        }

        _Myios::setstate(_State);
        return *this;
    }

    int_type __CLR_OR_THIS_CALL get() { // extract a metacharacter
        int_type _Meta           = 0;
        ios_base::iostate _State = ios_base::goodbit;
        _Chcount                 = 0;
        const sentry _Ok(*this, true);

        if (!_Ok) {
            _Meta = _Traits::eof(); // state not okay, return EOF
        } else { // state okay, extract a character
            _TRY_IO_BEGIN
            _Meta = _Myios::rdbuf()->sgetc();

            if (_Traits::eq_int_type(_Traits::eof(), _Meta)) {
                _State |= ios_base::eofbit | ios_base::failbit; // end of file
            } else { // got a character, count it
                _Myios::rdbuf()->sbumpc();
                _Chcount = 1;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return _Meta;
    }

    basic_istream& __CLR_OR_THIS_CALL get(_Elem* _Str, streamsize _Count) { // get up to _Count characters into NTCS
        return get(_Str, _Count, _Myios::widen('\n'));
    }

    basic_istream& __CLR_OR_THIS_CALL get(_Elem* _Str, streamsize _Count, _Elem _Delim) {
        // get up to _Count characters into NTCS, stop before _Delim
        ios_base::iostate _State = ios_base::goodbit;
        _Chcount                 = 0;

        // ensure null termination of buffer with nonzero length
        const _Null_terminator_guard<_Elem> _Guard{0 < _Count ? &_Str : nullptr};

        const sentry _Ok(*this, true);

        if (_Ok && 0 < _Count) { // state okay, extract characters
            _TRY_IO_BEGIN
            int_type _Meta = _Myios::rdbuf()->sgetc();

            for (; 0 < --_Count; _Meta = _Myios::rdbuf()->snextc()) {
                if (_Traits::eq_int_type(_Traits::eof(), _Meta)) { // end of file, quit
                    _State |= ios_base::eofbit;
                    break;
                } else if (_Traits::to_char_type(_Meta) == _Delim) {
                    break; // got a delimiter, quit
                } else { // got a character, add it to string
                    *_Str++ = _Traits::to_char_type(_Meta);
                    _Increment_gcount();
                }
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_Chcount == 0 ? _State | ios_base::failbit : _State);
        return *this;
    }

    basic_istream& __CLR_OR_THIS_CALL get(_Elem& _Ch) { // get a character
        int_type _Meta = get();
        if (!_Traits::eq_int_type(_Traits::eof(), _Meta)) {
            _Ch = _Traits::to_char_type(_Meta);
        }

        return *this;
    }

    basic_istream& __CLR_OR_THIS_CALL get(_Mysb& _Strbuf) { // extract up to newline and insert into stream buffer
        return get(_Strbuf, _Myios::widen('\n'));
    }

    basic_istream& __CLR_OR_THIS_CALL get(_Mysb& _Strbuf, _Elem _Delim) {
        // extract up to delimiter and insert into stream buffer
        ios_base::iostate _State = ios_base::goodbit;
        _Chcount                 = 0;
        const sentry _Ok(*this, true);

        if (_Ok) { // state okay, use facet to extract
            _TRY_IO_BEGIN
            int_type _Meta = _Myios::rdbuf()->sgetc();

            for (;; _Meta = _Myios::rdbuf()->snextc()) {
                if (_Traits::eq_int_type(_Traits::eof(), _Meta)) { // end of file, quit
                    _State |= ios_base::eofbit;
                    break;
                } else { // got a character, insert it into stream buffer
                    _TRY_BEGIN
                    _Elem _Ch = _Traits::to_char_type(_Meta);
                    if (_Ch == _Delim || _Traits::eq_int_type(_Traits::eof(), _Strbuf.sputc(_Ch))) {
                        break;
                    }
                    _CATCH_ALL
                    break;
                    _CATCH_END
                    _Increment_gcount();
                }
            }
            _CATCH_IO_END
        }

        if (_Chcount == 0) {
            _State |= ios_base::failbit;
        }
        _Myios::setstate(_State);
        return *this;
    }

    basic_istream& __CLR_OR_THIS_CALL getline(_Elem* _Str, streamsize _Count) {
        // get up to _Count characters into NTCS, discard newline
        return getline(_Str, _Count, _Myios::widen('\n'));
    }

    basic_istream& __CLR_OR_THIS_CALL getline(_Elem* _Str, streamsize _Count, _Elem _Delim) {
        // get up to _Count characters into NTCS, discard _Delim
        ios_base::iostate _State = ios_base::goodbit;
        _Chcount                 = 0;

        // ensure null termination of buffer with nonzero length
        const _Null_terminator_guard<_Elem> _Guard{0 < _Count ? &_Str : nullptr};

        const sentry _Ok(*this, true);

        if (_Ok && 0 < _Count) { // state okay, use facet to extract
            int_type _Metadelim = _Traits::to_int_type(_Delim);

            _TRY_IO_BEGIN
            int_type _Meta = _Myios::rdbuf()->sgetc();

            for (;; _Meta = _Myios::rdbuf()->snextc()) {
                if (_Traits::eq_int_type(_Traits::eof(), _Meta)) { // end of file, quit
                    _State |= ios_base::eofbit;
                    break;
                } else if (_Meta == _Metadelim) { // got a delimiter, discard it and quit
                    _Increment_gcount();
                    _Myios::rdbuf()->sbumpc();
                    break;
                } else if (--_Count <= 0) { // buffer full, quit
                    _State |= ios_base::failbit;
                    break;
                } else { // got a character, add it to string
                    *_Str++ = _Traits::to_char_type(_Meta);
                    _Increment_gcount();
                }
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_Chcount == 0 ? _State | ios_base::failbit : _State);
        return *this;
    }

    basic_istream& __CLR_OR_THIS_CALL ignore(streamsize _Count = 1, int_type _Metadelim = _Traits::eof()) {
        // ignore up to _Count characters, discarding delimiter
        ios_base::iostate _State = ios_base::goodbit;
        _Chcount                 = 0;
        const sentry _Ok(*this, true);

        if (_Ok && 0 < _Count) { // state okay, use facet to extract
            _TRY_IO_BEGIN
            for (;;) { // get a metacharacter if more room in buffer
                int_type _Meta;
                if (_Count != _STD _Max_limit<streamsize>() && --_Count < 0) {
                    break; // buffer full, quit
                } else if (_Traits::eq_int_type(_Traits::eof(),
                               _Meta = _Myios::rdbuf()->sbumpc())) { // end of file, quit
                    _State |= ios_base::eofbit;
                    break;
                } else { // got a character, count it
                    _Increment_gcount();
                    if (_Meta == _Metadelim) {
                        break; // got a delimiter, quit
                    }
                }
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    template <class _CharType = _Elem, enable_if_t<is_same_v<_CharType, char>, int> = 0>
    basic_istream& ignore(const streamsize _Count, const _Elem _Delim) {
        return ignore(_Count, _Traits::to_int_type(_Delim));
    }

    basic_istream& __CLR_OR_THIS_CALL read(_Elem* _Str, streamsize _Count) { // read up to _Count characters into buffer
        ios_base::iostate _State = ios_base::goodbit;
        _Chcount                 = 0;
        const sentry _Ok(*this, true);

        if (_Ok && 0 < _Count) { // state okay, use facet to extract
            _TRY_IO_BEGIN
            const streamsize _Num = _Myios::rdbuf()->sgetn(_Str, _Count);
            _Chcount              = _Num;

            if (_Num != _Count) {
                _State |= ios_base::eofbit | ios_base::failbit; // short read
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    streamsize __CLR_OR_THIS_CALL readsome(_Elem* _Str, streamsize _Count) {
        // read up to _Count characters into buffer, without blocking
        ios_base::iostate _State = ios_base::goodbit;
        _Chcount                 = 0;
        const sentry _Ok(*this, true);
        streamsize _Num;

        if (!_Ok) {
            _State |= ios_base::failbit; // no buffer, fail
        } else if ((_Num = _Myios::rdbuf()->in_avail()) < 0) {
            _State |= ios_base::eofbit; // no characters available
        } else if (0 < _Count && 0 < _Num) { // read available
            read(_Str, _Num < _Count ? _Num : _Count);
        }

        _Myios::setstate(_State);
        return gcount();
    }

    int_type __CLR_OR_THIS_CALL peek() {
        ios_base::iostate _State = ios_base::goodbit;
        _Chcount                 = 0;
        int_type _Meta           = 0;
        const sentry _Ok(*this, true);

        if (!_Ok) {
            _Meta = _Traits::eof(); // state not okay, return EOF
        } else { // state okay, read a character
            _TRY_IO_BEGIN
            if (_Traits::eq_int_type(_Traits::eof(), _Meta = _Myios::rdbuf()->sgetc())) {
                _State |= ios_base::eofbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return _Meta;
    }

    basic_istream& __CLR_OR_THIS_CALL putback(_Elem _Ch) { // put back a character
        _Chcount                    = 0;
        ios_base::iostate _State    = ios_base::goodbit;
        ios_base::iostate _Oldstate = _Myios::rdstate();
        _Myios::clear(_Oldstate & ~ios_base::eofbit);
        const sentry _Ok(*this, true);

        if (_Ok) { // state okay, put character back
            _TRY_IO_BEGIN
            if (_Traits::eq_int_type(_Traits::eof(), _Myios::rdbuf()->sputbackc(_Ch))) {
                _State |= ios_base::badbit | _Oldstate;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    basic_istream& __CLR_OR_THIS_CALL unget() { // put back last read character
        _Chcount                    = 0;
        ios_base::iostate _State    = ios_base::goodbit;
        ios_base::iostate _Oldstate = _Myios::rdstate();
        _Myios::clear(_Oldstate & ~ios_base::eofbit);
        const sentry _Ok(*this, true);

        if (_Ok) { // state okay, put character back
            _TRY_IO_BEGIN
            if (_Traits::eq_int_type(_Traits::eof(), _Myios::rdbuf()->sungetc())) {
                _State |= ios_base::badbit | _Oldstate;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    _NODISCARD streamsize __CLR_OR_THIS_CALL gcount() const noexcept /* strengthened */ {
        // get count from last extraction
        return _Chcount;
    }

    int __CLR_OR_THIS_CALL sync() { // synchronize with input source
        const sentry _Ok(*this, true);

        const auto _Rdbuf = _Myios::rdbuf();
        if (!_Rdbuf) {
            return -1;
        }

        ios_base::iostate _State = ios_base::goodbit;
        _TRY_BEGIN
        if (_Rdbuf->pubsync() == -1) {
            _State |= ios_base::badbit;
        }
        _CATCH_ALL
        _Myios::setstate(ios_base::badbit, true);
        return -1;
        _CATCH_END

        if (_State & ios_base::badbit) {
            _Myios::setstate(ios_base::badbit);
            return -1;
        }

        return 0;
    }

    basic_istream& __CLR_OR_THIS_CALL seekg(pos_type _Pos) { // set input stream position to _Pos
        ios_base::iostate _State    = ios_base::goodbit;
        ios_base::iostate _Oldstate = _Myios::rdstate();
        _Myios::clear(_Oldstate & ~ios_base::eofbit);
        const sentry _Ok(*this, true);

        if (!this->fail()) {
            _TRY_IO_BEGIN
            if (static_cast<off_type>(_Myios::rdbuf()->pubseekpos(_Pos, ios_base::in)) == -1) {
                _State |= ios_base::failbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    basic_istream& __CLR_OR_THIS_CALL seekg(off_type _Off, ios_base::seekdir _Way) {
        // change input stream position by _Off, according to _Way
        ios_base::iostate _State    = ios_base::goodbit;
        ios_base::iostate _Oldstate = _Myios::rdstate();
        _Myios::clear(_Oldstate & ~ios_base::eofbit);
        const sentry _Ok(*this, true);

        if (!this->fail()) {
            _TRY_IO_BEGIN
            if (static_cast<off_type>(_Myios::rdbuf()->pubseekoff(_Off, _Way, ios_base::in)) == -1) {
                _State |= ios_base::failbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    pos_type __CLR_OR_THIS_CALL tellg() {
        const sentry _Ok(*this, true);

        if (!this->fail()) {
            _TRY_IO_BEGIN
            return _Myios::rdbuf()->pubseekoff(0, ios_base::cur, ios_base::in);
            _CATCH_IO_END
        }

        return pos_type{off_type{-1}};
    }

private:
    streamsize _Chcount{}; // the character count
};

#pragma vtordisp(pop) // compiler bug workaround

#ifndef _NATIVE_WCHAR_T_DEFINED
template <class _Elem, class _Traits>
basic_istream<_Elem, _Traits>& operator>>(basic_istream<_Elem, _Traits>& _Istr, _Elem& _Ch);

// NOTE:
// If you are not using native wchar_t, the following explicit
// specialization will mask the member function (above) that treats
// an unsigned short as an integer.

// To read or write unsigned shorts as integers with wchar_t streams,
// make wchar_t a native type with the command line option /Zc:wchar_t.

template <>
inline basic_istream<unsigned short, char_traits<unsigned short>>& __CLR_OR_THIS_CALL basic_istream<unsigned short,
    char_traits<unsigned short>>::operator>>(unsigned short& _Ch) { // extract a character
    return _STD operator>>(*this, _Ch);
}
#endif // !defined(_NATIVE_WCHAR_T_DEFINED)

#if defined(_DLL_CPPLIB)

#if !defined(_CRTBLD) || defined(__FORCE_INSTANCE)
template class _CRTIMP2_PURE_IMPORT basic_istream<char, char_traits<char>>;
template class _CRTIMP2_PURE_IMPORT basic_istream<wchar_t, char_traits<wchar_t>>;
#endif // !defined(_CRTBLD) || defined(__FORCE_INSTANCE)

#ifdef __FORCE_INSTANCE
template class _CRTIMP2_PURE_IMPORT basic_istream<unsigned short, char_traits<unsigned short>>;
#endif // defined(__FORCE_INSTANCE)
#endif // defined(_DLL_CPPLIB)

_EXPORT_STD extern "C++" template <class _Elem, class _Traits>
class basic_iostream : public basic_istream<_Elem, _Traits>,
                       public basic_ostream<_Elem, _Traits> { // control insertions and extractions from a stream buffer
public:
    using _Myis       = basic_istream<_Elem, _Traits>;
    using _Myos       = basic_ostream<_Elem, _Traits>;
    using _Myios      = basic_ios<_Elem, _Traits>;
    using char_type   = _Elem;
    using traits_type = _Traits;
    using int_type    = typename _Traits::int_type;
    using pos_type    = typename _Traits::pos_type;
    using off_type    = typename _Traits::off_type;

    explicit __CLR_OR_THIS_CALL basic_iostream(basic_streambuf<_Elem, _Traits>* _Strbuf)
        : _Myis(_Strbuf, false), _Myos(_Noinit, false) {}

protected:
    __CLR_OR_THIS_CALL basic_iostream(basic_iostream&& _Right) noexcept(false)
        : _Myis(_Right.rdbuf(), false), _Myos(_Noinit, false) {
        _Myios::init();
        _Myios::move(_STD move(_Right));
    }

    basic_iostream& __CLR_OR_THIS_CALL operator=(basic_iostream&& _Right) noexcept /* strengthened */ {
        this->swap(_Right);
        return *this;
    }

    void __CLR_OR_THIS_CALL swap(basic_iostream& _Right) noexcept /* strengthened */ {
        if (this != _STD addressof(_Right)) {
            _Myios::swap(_Right);
        }
    }

public:
    __CLR_OR_THIS_CALL basic_iostream(const basic_iostream&)            = delete;
    basic_iostream& __CLR_OR_THIS_CALL operator=(const basic_iostream&) = delete;

    __CLR_OR_THIS_CALL ~basic_iostream() noexcept override {}
};

#if defined(_DLL_CPPLIB)

#if !defined(_CRTBLD) || defined(__FORCE_INSTANCE)
template class _CRTIMP2_PURE_IMPORT basic_iostream<char, char_traits<char>>;
template class _CRTIMP2_PURE_IMPORT basic_iostream<wchar_t, char_traits<wchar_t>>;
#endif // !defined(_CRTBLD) || defined(__FORCE_INSTANCE)

#ifdef __FORCE_INSTANCE
template class _CRTIMP2_PURE_IMPORT basic_iostream<unsigned short, char_traits<unsigned short>>;
#endif // defined(__FORCE_INSTANCE)
#endif // defined(_DLL_CPPLIB)

template <class _Elem, class _Traits>
basic_istream<_Elem, _Traits>& _Istream_extract_into_buffer(
    basic_istream<_Elem, _Traits>& _Istr, size_t _Size, _Elem* _Str) {
    using _Myis              = basic_istream<_Elem, _Traits>;
    using _Ctype             = ctype<_Elem>;
    ios_base::iostate _State = ios_base::goodbit;
    size_t _Current          = 0;
    const typename _Myis::sentry _Ok(_Istr);

    if (_Ok) { // state okay, extract characters
        const _Ctype& _Ctype_fac = _STD use_facet<_Ctype>(_Istr.getloc());

        _TRY_IO_BEGIN
        size_t _Count       = _Size;
        const size_t _Width = static_cast<size_t>(_Istr.width());
        if (_Width > 0 && _Width < _Size) {
            _Count = _Width;
        }

        typename _Myis::int_type _Meta = _Istr.rdbuf()->sgetc();
        _Elem _Ch;

        for (; _Current < _Count - 1; _Meta = _Istr.rdbuf()->snextc(), (void) ++_Current) {
            if (_Traits::eq_int_type(_Traits::eof(), _Meta)) { // end of file, quit
                _State |= ios_base::eofbit;
                break;
            } else if (_Ctype_fac.is(_Ctype::space, _Ch = _Traits::to_char_type(_Meta)) || _Ch == _Elem()) {
                break; // whitespace or nul, quit
            } else {
                _Str[_Current] = _Traits::to_char_type(_Meta); // add it to string
            }
        }
        _CATCH_IO_(ios_base, _Istr)
    }
    _Analysis_assume_(static_cast<size_t>(_Current) < _Size); // TRANSITION, VSO-860375
    _Str[_Current] = _Elem(); // add terminating null character
    _Istr.width(0);
    if (_Current == 0) {
        _State |= ios_base::failbit;
    }
    _Istr.setstate(_State);
    return _Istr;
}

#if _HAS_CXX20 // P0487R1 Fixing operator>>(basic_istream&, CharT*)
#pragma warning(push)
#pragma warning(disable : 6001) // Using uninitialized memory '_Str'.
_EXPORT_STD template <class _Elem, class _Traits, size_t _Size>
basic_istream<_Elem, _Traits>& operator>>(
    basic_istream<_Elem, _Traits>& _Istr, _Out_writes_z_(_Size) _Elem (&_Str)[_Size]) {
    return _Istream_extract_into_buffer(_Istr, _Size, _Str);
}

_EXPORT_STD template <class _Traits, size_t _Size>
basic_istream<char, _Traits>& operator>>(
    basic_istream<char, _Traits>& _Istr, _Out_writes_z_(_Size) signed char (&_Str)[_Size]) {
    return _Istream_extract_into_buffer(_Istr, _Size, reinterpret_cast<char*>(_Str));
}

_EXPORT_STD template <class _Traits, size_t _Size>
basic_istream<char, _Traits>& operator>>(
    basic_istream<char, _Traits>& _Istr, _Out_writes_z_(_Size) unsigned char (&_Str)[_Size]) {
    return _Istream_extract_into_buffer(_Istr, _Size, reinterpret_cast<char*>(_Str));
}
#pragma warning(pop)
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
template <class _Elem, class _Traits>
basic_istream<_Elem, _Traits>& operator>>(basic_istream<_Elem, _Traits>& _Istr, _Elem* _Str) {
    return _Istream_extract_into_buffer(_Istr, SIZE_MAX, _Str);
}

template <class _Traits>
basic_istream<char, _Traits>& operator>>(basic_istream<char, _Traits>& _Istr, signed char* _Str) {
    return _Istream_extract_into_buffer(_Istr, SIZE_MAX, reinterpret_cast<char*>(_Str));
}

template <class _Traits>
basic_istream<char, _Traits>& operator>>(basic_istream<char, _Traits>& _Istr, unsigned char* _Str) {
    return _Istream_extract_into_buffer(_Istr, SIZE_MAX, reinterpret_cast<char*>(_Str));
}
#endif // ^^^ !_HAS_CXX20 ^^^

_EXPORT_STD template <class _Elem, class _Traits>
basic_istream<_Elem, _Traits>& operator>>(basic_istream<_Elem, _Traits>& _Istr, _Elem& _Ch) { // extract a character
    using _Myis = basic_istream<_Elem, _Traits>;

    typename _Myis::int_type _Meta;
    ios_base::iostate _State = ios_base::goodbit;
    const typename _Myis::sentry _Ok(_Istr);

    if (_Ok) { // state okay, extract characters
        _TRY_IO_BEGIN
        _Meta = _Istr.rdbuf()->sbumpc();
        if (_Traits::eq_int_type(_Traits::eof(), _Meta)) {
            _State |= ios_base::eofbit | ios_base::failbit; // end of file
        } else {
            _Ch = _Traits::to_char_type(_Meta); // got a character
        }
        _CATCH_IO_(ios_base, _Istr)
    }

    _Istr.setstate(_State);
    return _Istr;
}

_EXPORT_STD template <class _Traits>
basic_istream<char, _Traits>& operator>>(basic_istream<char, _Traits>& _Istr, signed char& _Ch) {
    // extract a signed char
    return _Istr >> reinterpret_cast<char&>(_Ch);
}

_EXPORT_STD template <class _Traits>
basic_istream<char, _Traits>& operator>>(basic_istream<char, _Traits>& _Istr, unsigned char& _Ch) {
    // extract an unsigned char
    return _Istr >> reinterpret_cast<char&>(_Ch);
}

template <class _Istr, class _Ty, class = void>
struct _Can_stream_in : false_type {};

template <class _Istr, class _Ty>
struct _Can_stream_in<_Istr, _Ty, void_t<decltype(_STD declval<_Istr&>() >> _STD declval<_Ty>())>> : true_type {};

_EXPORT_STD template <class _Istr, class _Ty,
    enable_if_t<conjunction_v<is_convertible<_Istr*, ios_base*>, _Can_stream_in<_Istr, _Ty>>, int> = 0>
_Istr&& operator>>(_Istr&& _Is, _Ty&& _Val) { // extract from rvalue stream
    _Is >> _STD forward<_Ty>(_Val);
    return _STD move(_Is);
}

_EXPORT_STD template <class _Elem, class _Traits>
basic_istream<_Elem, _Traits>& __CLRCALL_OR_CDECL ws(basic_istream<_Elem, _Traits>& _Istr) { // consume whitespace
    const typename basic_istream<_Elem, _Traits>::sentry _Ok(_Istr, true);

    if (_Ok) { // state okay, extract characters
        ios_base::iostate _State = ios_base::goodbit;
        const auto& _Ctype_fac   = _STD use_facet<ctype<_Elem>>(_Istr.getloc());

        _TRY_IO_BEGIN
        for (typename _Traits::int_type _Meta = _Istr.rdbuf()->sgetc();; _Meta = _Istr.rdbuf()->snextc()) {
            if (_Traits::eq_int_type(_Traits::eof(), _Meta)) { // end of file, quit
                _State |= ios_base::eofbit;
                break;
            } else if (!_Ctype_fac.is(ctype<_Elem>::space, _Traits::to_char_type(_Meta))) {
                break; // not whitespace, quit
            }
        }
        _CATCH_IO_(ios_base, _Istr)
        _Istr.setstate(_State);
    }

    return _Istr;
}
_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _ISTREAM_

// ostream standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef _OSTREAM_
#define _OSTREAM_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
// __msvc_ostream.hpp internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef __MSVC_OSTREAM_HPP
#define __MSVC_OSTREAM_HPP
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
#include <ios>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN
#pragma vtordisp(push, 2) // compiler bug workaround

_EXPORT_STD extern "C++" template <class _Elem, class _Traits>
class basic_ostream : virtual public basic_ios<_Elem, _Traits> { // control insertions into a stream buffer
public:
    using _Myios = basic_ios<_Elem, _Traits>;
    using _Mysb  = basic_streambuf<_Elem, _Traits>;
    using _Iter  = ostreambuf_iterator<_Elem, _Traits>;
    using _Nput  = num_put<_Elem, _Iter>;

    explicit __CLR_OR_THIS_CALL basic_ostream(basic_streambuf<_Elem, _Traits>* _Strbuf, bool _Isstd = false) {
        _Myios::init(_Strbuf, _Isstd);
    }

    __CLR_OR_THIS_CALL basic_ostream(_Uninitialized, bool _Addit = true) {
        if (_Addit) {
            this->_Addstd(this); // suppress for basic_iostream
        }
    }

protected:
    __CLR_OR_THIS_CALL basic_ostream(basic_ostream&& _Right) noexcept(false) {
        _Myios::init();
        _Myios::move(_STD move(_Right));
    }

    basic_ostream& __CLR_OR_THIS_CALL operator=(basic_ostream&& _Right) noexcept /* strengthened */ {
        this->swap(_Right);
        return *this;
    }

    void __CLR_OR_THIS_CALL swap(basic_ostream& _Right) noexcept /* strengthened */ {
        if (this != _STD addressof(_Right)) {
            _Myios::swap(_Right);
        }
    }

public:
    __CLR_OR_THIS_CALL basic_ostream(const basic_ostream&)            = delete;
    basic_ostream& __CLR_OR_THIS_CALL operator=(const basic_ostream&) = delete;

    __CLR_OR_THIS_CALL ~basic_ostream() noexcept override {}

    using int_type = typename _Traits::int_type;
    using pos_type = typename _Traits::pos_type;
    using off_type = typename _Traits::off_type;

    class _Sentry_base { // stores thread lock and reference to output stream
    public:
        __CLR_OR_THIS_CALL _Sentry_base(basic_ostream& _Ostr) : _Myostr(_Ostr) { // lock the stream buffer, if there
            const auto _Rdbuf = _Myostr.rdbuf();
            if (_Rdbuf) {
                _Rdbuf->_Lock();
            }
        }

        __CLR_OR_THIS_CALL ~_Sentry_base() noexcept { // destroy after unlocking
            const auto _Rdbuf = _Myostr.rdbuf();
            if (_Rdbuf) {
                _Rdbuf->_Unlock();
            }
        }

        basic_ostream& _Myostr; // the output stream, for _Unlock call at destruction

        _Sentry_base& operator=(const _Sentry_base&) = delete;
    };

    class sentry : public _Sentry_base {
    public:
        explicit __CLR_OR_THIS_CALL sentry(basic_ostream& _Ostr) : _Sentry_base(_Ostr) {
            if (!_Ostr.good()) {
                _Ok = false;
                return;
            }

            const auto _Tied = _Ostr.tie();
            if (!_Tied || _Tied == _STD addressof(_Ostr)) {
                _Ok = true;
                return;
            }

            _Tied->flush();
            _Ok = _Ostr.good(); // store test only after flushing tie
        }

        _STL_DISABLE_DEPRECATED_WARNING
        __CLR_OR_THIS_CALL ~sentry() noexcept {
#if !_HAS_EXCEPTIONS
            const bool _Zero_uncaught_exceptions = true;
#elif _HAS_DEPRECATED_UNCAUGHT_EXCEPTION
            const bool _Zero_uncaught_exceptions = !_STD uncaught_exception(); // TRANSITION, ArchivedOS-12000909
#else // ^^^ _HAS_DEPRECATED_UNCAUGHT_EXCEPTION / !_HAS_DEPRECATED_UNCAUGHT_EXCEPTION vvv
            const bool _Zero_uncaught_exceptions = _STD uncaught_exceptions() == 0;
#endif // ^^^ !_HAS_DEPRECATED_UNCAUGHT_EXCEPTION ^^^

            if (_Zero_uncaught_exceptions) {
                this->_Myostr._Osfx();
            }
        }
        _STL_RESTORE_DEPRECATED_WARNING

        explicit __CLR_OR_THIS_CALL operator bool() const {
            return _Ok;
        }

        __CLR_OR_THIS_CALL sentry(const sentry&)            = delete;
        sentry& __CLR_OR_THIS_CALL operator=(const sentry&) = delete;

    private:
        bool _Ok; // true if stream state okay at construction
    };

#pragma push_macro("opfx")
#pragma push_macro("osfx")
#undef opfx
#undef osfx
    // TRANSITION, ABI: non-Standard opfx() is preserved for binary compatibility
    _DEPRECATE_IO_PFX_SFX bool __CLR_OR_THIS_CALL opfx() { // test stream state and flush tie stream as needed
        if (!this->good()) {
            return false;
        }

        const auto _Tied = _Myios::tie();
        if (!_Tied || _Myios::tie() == this) {
            return true;
        }

        _Tied->flush();
        return this->good();
    }

    // TRANSITION, ABI: non-Standard osfx() is preserved for binary compatibility
    _DEPRECATE_IO_PFX_SFX void __CLR_OR_THIS_CALL osfx() noexcept { // perform any wrapup
        _Osfx();
    }
#pragma pop_macro("osfx")
#pragma pop_macro("opfx")

    void __CLR_OR_THIS_CALL _Osfx() noexcept { // perform any wrapup
        _TRY_BEGIN
        if (this->good() && this->flags() & ios_base::unitbuf) {
            if (_Myios::rdbuf()->pubsync() == -1) { // flush stream as needed
                _Myios::setstate(ios_base::badbit);
            }
        }
        _CATCH_ALL
        _CATCH_END
    }

#ifdef _M_CEE_PURE
    basic_ostream& __CLR_OR_THIS_CALL operator<<(basic_ostream&(__clrcall* _Pfn)(basic_ostream&) ) {
        // call basic_ostream manipulator
        return _Pfn(*this);
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(_Myios&(__clrcall* _Pfn)(_Myios&) ) {
        // call basic_ios manipulator
        _Pfn(*this);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(ios_base&(__clrcall* _Pfn)(ios_base&) ) {
        // call ios_base manipulator
        _Pfn(*this);
        return *this;
    }
#endif // defined(_M_CEE_PURE)

    basic_ostream& __CLR_OR_THIS_CALL operator<<(basic_ostream&(__cdecl* _Pfn)(basic_ostream&) ) {
        // call basic_ostream manipulator
        return _Pfn(*this);
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(_Myios&(__cdecl* _Pfn)(_Myios&) ) {
        // call basic_ios manipulator
        _Pfn(*this);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(ios_base&(__cdecl* _Pfn)(ios_base&) ) {
        // call ios_base manipulator
        _Pfn(*this);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(bool _Val) { // insert a boolean
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (_Ok) { // state okay, use facet to insert
            const _Nput& _Nput_fac = _STD use_facet<_Nput>(this->getloc());

            _TRY_IO_BEGIN
            if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this, _Myios::fill(), _Val).failed()) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(short _Val) { // insert a short
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (_Ok) { // state okay, use facet to insert
            const _Nput& _Nput_fac  = _STD use_facet<_Nput>(this->getloc());
            ios_base::fmtflags _Bfl = this->flags() & ios_base::basefield;

            long _Tmp;
            if (_Bfl == ios_base::oct || _Bfl == ios_base::hex) {
                _Tmp = static_cast<long>(static_cast<unsigned short>(_Val));
            } else {
                _Tmp = static_cast<long>(_Val);
            }

            _TRY_IO_BEGIN
            if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this, _Myios::fill(), _Tmp).failed()) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    // NOTE:
    // If you are not using native wchar_t, the unsigned short inserter
    // is masked by an explicit specialization that treats an unsigned
    // short as a wide character.

    // To read or write unsigned shorts as integers with wchar_t streams,
    // make wchar_t a native type with the command line option /Zc:wchar_t.

    basic_ostream& __CLR_OR_THIS_CALL operator<<(unsigned short _Val) { // insert an unsigned short
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (_Ok) { // state okay, use facet to insert
            const _Nput& _Nput_fac = _STD use_facet<_Nput>(this->getloc());

            _TRY_IO_BEGIN
            if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this, _Myios::fill(), static_cast<unsigned long>(_Val))
                    .failed()) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(int _Val) { // insert an int
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (_Ok) { // state okay, use facet to insert
            const _Nput& _Nput_fac  = _STD use_facet<_Nput>(this->getloc());
            ios_base::fmtflags _Bfl = this->flags() & ios_base::basefield;

            long _Tmp;
            if (_Bfl == ios_base::oct || _Bfl == ios_base::hex) {
                _Tmp = static_cast<long>(static_cast<unsigned int>(_Val));
            } else {
                _Tmp = static_cast<long>(_Val);
            }

            _TRY_IO_BEGIN
            if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this, _Myios::fill(), _Tmp).failed()) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(unsigned int _Val) { // insert an unsigned int
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (_Ok) { // state okay, use facet to insert
            const _Nput& _Nput_fac = _STD use_facet<_Nput>(this->getloc());

            _TRY_IO_BEGIN
            if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this, _Myios::fill(), static_cast<unsigned long>(_Val))
                    .failed()) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(long _Val) { // insert a long
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (_Ok) { // state okay, use facet to insert
            const _Nput& _Nput_fac = _STD use_facet<_Nput>(this->getloc());

            _TRY_IO_BEGIN
            if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this, _Myios::fill(), _Val).failed()) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(unsigned long _Val) { // insert an unsigned long
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (_Ok) { // state okay, use facet to insert
            const _Nput& _Nput_fac = _STD use_facet<_Nput>(this->getloc());

            _TRY_IO_BEGIN
            if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this, _Myios::fill(), _Val).failed()) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(long long _Val) { // insert a long long
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (_Ok) { // state okay, use facet to insert
            const _Nput& _Nput_fac = _STD use_facet<_Nput>(this->getloc());

            _TRY_IO_BEGIN
            if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this, _Myios::fill(), _Val).failed()) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(unsigned long long _Val) { // insert an unsigned long long
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (_Ok) { // state okay, use facet to insert
            const _Nput& _Nput_fac = _STD use_facet<_Nput>(this->getloc());

            _TRY_IO_BEGIN
            if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this, _Myios::fill(), _Val).failed()) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(float _Val) { // insert a float
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (_Ok) { // state okay, use facet to insert
            const _Nput& _Nput_fac = _STD use_facet<_Nput>(this->getloc());

            _TRY_IO_BEGIN
            if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this, _Myios::fill(), static_cast<double>(_Val)).failed()) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(double _Val) { // insert a double
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (_Ok) { // state okay, use facet to insert
            const _Nput& _Nput_fac = _STD use_facet<_Nput>(this->getloc());

            _TRY_IO_BEGIN
            if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this, _Myios::fill(), _Val).failed()) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(long double _Val) { // insert a long double
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (_Ok) { // state okay, use facet to insert
            const _Nput& _Nput_fac = _STD use_facet<_Nput>(this->getloc());

            _TRY_IO_BEGIN
            if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this, _Myios::fill(), _Val).failed()) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL operator<<(const void* _Val) { // insert a void pointer
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (_Ok) { // state okay, use facet to insert
            const _Nput& _Nput_fac = _STD use_facet<_Nput>(this->getloc());

            _TRY_IO_BEGIN
            if (_Nput_fac.put(_Iter(_Myios::rdbuf()), *this, _Myios::fill(), _Val).failed()) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

#if _HAS_CXX23
    template <class = void> // TRANSITION, ABI
    basic_ostream& operator<<(const volatile void* _Val) {
        return *this << const_cast<const void*>(_Val);
    }
#endif // _HAS_CXX23

#if _HAS_CXX17
    template <class = void> // TRANSITION, ABI
    basic_ostream& operator<<(nullptr_t) { // insert a null pointer
        return *this << "nullptr";
    }
#endif // _HAS_CXX17

    basic_ostream& __CLR_OR_THIS_CALL operator<<(_Mysb* _Strbuf) { // insert until end-of-file from a stream buffer
        ios_base::iostate _State = ios_base::goodbit;
        bool _Copied             = false;
        const sentry _Ok(*this);

        if (_Ok && _Strbuf) {
            for (int_type _Meta = _Traits::eof();; _Copied = true) { // extract another character from stream buffer
                _TRY_BEGIN
                _Meta = _Traits::eq_int_type(_Traits::eof(), _Meta) ? _Strbuf->sgetc() : _Strbuf->snextc();
                _CATCH_ALL
                // N4971 [ostream.inserters]/9: "If an exception was thrown
                // while extracting a character, the function sets failbit in the error state,
                // and if failbit is set in exceptions() the caught exception is rethrown."
                _Myios::setstate(ios_base::failbit, _Myios::exceptions() == ios_base::failbit);
                _CATCH_END

                if (_Traits::eq_int_type(_Traits::eof(), _Meta)) {
                    break; // end of file, quit
                }

                _TRY_IO_BEGIN
                if (_Traits::eq_int_type(_Traits::eof(), _Myios::rdbuf()->sputc(_Traits::to_char_type(_Meta)))) {
                    _State |= ios_base::badbit; // insertion failed, quit
                    break;
                }
                _CATCH_IO_END
            }
        }

        this->width(0);
        int _Setstate_with;
        if (_Strbuf) {
            if (_Copied) {
                _Setstate_with = _State;
            } else {
                _Setstate_with = _State | ios_base::failbit;
            }
        } else {
            _Setstate_with = ios_base::badbit;
        }

        _Myios::setstate(_Setstate_with);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL put(_Elem _Ch) { // insert a character
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (!_Ok) {
            _State |= ios_base::badbit;
        } else { // state okay, insert character
            _TRY_IO_BEGIN
            if (_Traits::eq_int_type(_Traits::eof(), _Myios::rdbuf()->sputc(_Ch))) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL write(const _Elem* _Str, streamsize _Count) {
        // insert _Count characters from array _Str
        ios_base::iostate _State = ios_base::goodbit;
        const sentry _Ok(*this);

        if (!_Ok) {
            _State |= ios_base::badbit;
        } else if (0 < _Count) { // state okay, insert characters
            _TRY_IO_BEGIN
            if (_Myios::rdbuf()->sputn(_Str, _Count) != _Count) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_END
        }

        _Myios::setstate(_State);
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL flush() { // flush output stream
        const auto _Rdbuf = _Myios::rdbuf();
        if (_Rdbuf) { // buffer exists, flush it
            const sentry _Ok(*this);

            if (_Ok) {
                ios_base::iostate _State = ios_base::goodbit;
                _TRY_IO_BEGIN
                if (_Rdbuf->pubsync() == -1) {
                    _State |= ios_base::badbit; // sync failed
                }
                _CATCH_IO_END
                _Myios::setstate(_State);
            }
        }
        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL seekp(pos_type _Pos) { // set output stream position to _Pos
        const sentry _Ok(*this);

        if (!this->fail()) {
            ios_base::iostate _State = ios_base::goodbit;
            _TRY_IO_BEGIN
            if (static_cast<off_type>(_Myios::rdbuf()->pubseekpos(_Pos, ios_base::out)) == -1) {
                _State |= ios_base::failbit; // seek failed
            }
            _CATCH_IO_END
            _Myios::setstate(_State);
        }

        return *this;
    }

    basic_ostream& __CLR_OR_THIS_CALL seekp(off_type _Off, ios_base::seekdir _Way) {
        // change output stream position by _Off, according to _Way
        const sentry _Ok(*this);

        if (!this->fail()) {
            ios_base::iostate _State = ios_base::goodbit;
            _TRY_IO_BEGIN
            if (static_cast<off_type>(_Myios::rdbuf()->pubseekoff(_Off, _Way, ios_base::out)) == -1) {
                _State |= ios_base::failbit; // seek failed
            }
            _CATCH_IO_END
            _Myios::setstate(_State);
        }

        return *this;
    }

    pos_type __CLR_OR_THIS_CALL tellp() {
        const sentry _Ok(*this);

        if (!this->fail()) {
            _TRY_IO_BEGIN
            return _Myios::rdbuf()->pubseekoff(0, ios_base::cur, ios_base::out);
            _CATCH_IO_END
        }

        return pos_type{off_type{-1}};
    }
};

#pragma vtordisp(pop) // compiler bug workaround

#ifndef _NATIVE_WCHAR_T_DEFINED
// NOTE:
// If you are not using native wchar_t, the following explicit
// specialization will mask the member function (above) that treats
// an unsigned short as an integer.

// To read or write unsigned shorts as integers with wchar_t streams,
// make wchar_t a native type with the command line option /Zc:wchar_t.

template <>
inline basic_ostream<unsigned short, char_traits<unsigned short>>& __CLR_OR_THIS_CALL basic_ostream<unsigned short,
    char_traits<unsigned short>>::operator<<(unsigned short _Ch) { // extract a character
    using _Traits = char_traits<unsigned short>;

    ios_base::iostate _State = ios_base::goodbit;
    const sentry _Ok(*this);

    if (_Ok) { // state okay, insert
        streamsize _Pad = this->width() <= 1 ? 0 : this->width() - 1;

        _TRY_IO_BEGIN
        if ((this->flags() & ios_base::adjustfield) != ios_base::left) {
            for (; _State == ios_base::goodbit && 0 < _Pad; --_Pad) { // pad on left
                if (_Traits::eq_int_type(_Traits::eof(), this->rdbuf()->sputc(this->fill()))) {
                    _State |= ios_base::badbit;
                }
            }
        }

        if (_State == ios_base::goodbit && _Traits::eq_int_type(_Traits::eof(), this->rdbuf()->sputc(_Ch))) {
            _State |= ios_base::badbit;
        }

        for (; _State == ios_base::goodbit && 0 < _Pad; --_Pad) { // pad on right
            if (_Traits::eq_int_type(_Traits::eof(), this->rdbuf()->sputc(this->fill()))) {
                _State |= ios_base::badbit;
            }
        }
        _CATCH_IO_END
    }

    this->width(0);
    _Myios::setstate(_State);
    return *this;
}
#endif // _NATIVE_WCHAR_T_DEFINED

#if defined(_DLL_CPPLIB)

#if !defined(_CRTBLD) || defined(__FORCE_INSTANCE)
template class _CRTIMP2_PURE_IMPORT basic_ostream<char, char_traits<char>>;
template class _CRTIMP2_PURE_IMPORT basic_ostream<wchar_t, char_traits<wchar_t>>;
#endif // !defined(_CRTBLD) || defined(__FORCE_INSTANCE)

#ifdef __FORCE_INSTANCE
template class _CRTIMP2_PURE_IMPORT basic_ostream<unsigned short, char_traits<unsigned short>>;
#endif // defined(__FORCE_INSTANCE)
#endif // defined(_DLL_CPPLIB)

_EXPORT_STD template <class _Elem, class _Traits>
basic_ostream<_Elem, _Traits>& operator<<(basic_ostream<_Elem, _Traits>& _Ostr, const char* _Val) { // insert NTBS
    ios_base::iostate _State = ios_base::goodbit;
    streamsize _Count        = static_cast<streamsize>(_CSTD strlen(_Val));
    streamsize _Pad          = _Ostr.width() <= 0 || _Ostr.width() <= _Count ? 0 : _Ostr.width() - _Count;
    const typename basic_ostream<_Elem, _Traits>::sentry _Ok(_Ostr);

    if (!_Ok) {
        _State |= ios_base::badbit;
    } else { // state okay, insert characters
        _TRY_IO_BEGIN
        const ctype<_Elem>& _Ctype_fac = _STD use_facet<ctype<_Elem>>(_Ostr.getloc());
        if ((_Ostr.flags() & ios_base::adjustfield) != ios_base::left) {
            for (; 0 < _Pad; --_Pad) { // pad on left
                if (_Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ostr.fill()))) {
                    _State |= ios_base::badbit; // insertion failed, quit
                    break;
                }
            }
        }

        for (; _State == ios_base::goodbit && 0 < _Count; --_Count, ++_Val) {
            if (_Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ctype_fac.widen(*_Val)))) {
                _State |= ios_base::badbit;
            }
        }

        if (_State == ios_base::goodbit) {
            for (; 0 < _Pad; --_Pad) { // pad on right
                if (_Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ostr.fill()))) {
                    _State |= ios_base::badbit; // insertion failed, quit
                    break;
                }
            }
        }

        _Ostr.width(0);
        _CATCH_IO_(ios_base, _Ostr)
    }

    _Ostr.setstate(_State);
    return _Ostr;
}

_EXPORT_STD template <class _Elem, class _Traits>
basic_ostream<_Elem, _Traits>& operator<<(basic_ostream<_Elem, _Traits>& _Ostr, char _Ch) { // insert a character
    ios_base::iostate _State = ios_base::goodbit;
    const typename basic_ostream<_Elem, _Traits>::sentry _Ok(_Ostr);

    if (_Ok) { // state okay, insert
        const ctype<_Elem>& _Ctype_fac = _STD use_facet<ctype<_Elem>>(_Ostr.getloc());
        streamsize _Pad                = _Ostr.width() <= 1 ? 0 : _Ostr.width() - 1;

        _TRY_IO_BEGIN
        if ((_Ostr.flags() & ios_base::adjustfield) != ios_base::left) {
            for (; _State == ios_base::goodbit && 0 < _Pad; --_Pad) { // pad on left
                if (_Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ostr.fill()))) {
                    _State |= ios_base::badbit;
                }
            }
        }

        if (_State == ios_base::goodbit
            && _Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ctype_fac.widen(_Ch)))) {
            _State |= ios_base::badbit;
        }

        for (; _State == ios_base::goodbit && 0 < _Pad; --_Pad) { // pad on right
            if (_Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ostr.fill()))) {
                _State |= ios_base::badbit;
            }
        }
        _CATCH_IO_(ios_base, _Ostr)
    }

    _Ostr.width(0);
    _Ostr.setstate(_State);
    return _Ostr;
}

_EXPORT_STD template <class _Traits>
basic_ostream<char, _Traits>& operator<<(basic_ostream<char, _Traits>& _Ostr, const char* _Val) {
    // insert NTBS into char stream
    using _Elem = char;
    using _Myos = basic_ostream<_Elem, _Traits>;

    ios_base::iostate _State = ios_base::goodbit;
    streamsize _Count        = static_cast<streamsize>(_Traits::length(_Val));
    streamsize _Pad          = _Ostr.width() <= 0 || _Ostr.width() <= _Count ? 0 : _Ostr.width() - _Count;
    const typename _Myos::sentry _Ok(_Ostr);

    if (!_Ok) {
        _State |= ios_base::badbit;
    } else { // state okay, insert
        _TRY_IO_BEGIN
        if ((_Ostr.flags() & ios_base::adjustfield) != ios_base::left) {
            for (; 0 < _Pad; --_Pad) { // pad on left
                if (_Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ostr.fill()))) {
                    _State |= ios_base::badbit; // insertion failed, quit
                    break;
                }
            }
        }

        if (_State == ios_base::goodbit && _Ostr.rdbuf()->sputn(_Val, _Count) != _Count) {
            _State |= ios_base::badbit;
        }

        if (_State == ios_base::goodbit) {
            for (; 0 < _Pad; --_Pad) { // pad on right
                if (_Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ostr.fill()))) {
                    _State |= ios_base::badbit; // insertion failed, quit
                    break;
                }
            }
        }

        _Ostr.width(0);
        _CATCH_IO_(ios_base, _Ostr)
    }

    _Ostr.setstate(_State);
    return _Ostr;
}

_EXPORT_STD template <class _Traits>
basic_ostream<char, _Traits>& operator<<(basic_ostream<char, _Traits>& _Ostr, char _Ch) {
    // insert a char into char stream
    using _Elem = char;
    using _Myos = basic_ostream<_Elem, _Traits>;

    ios_base::iostate _State = ios_base::goodbit;
    const typename _Myos::sentry _Ok(_Ostr);

    if (_Ok) { // state okay, insert
        streamsize _Pad = _Ostr.width() <= 1 ? 0 : _Ostr.width() - 1;

        _TRY_IO_BEGIN
        if ((_Ostr.flags() & ios_base::adjustfield) != ios_base::left) {
            for (; _State == ios_base::goodbit && 0 < _Pad; --_Pad) { // pad on left
                if (_Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ostr.fill()))) {
                    _State |= ios_base::badbit;
                }
            }
        }

        if (_State == ios_base::goodbit && _Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ch))) {
            _State |= ios_base::badbit;
        }

        for (; _State == ios_base::goodbit && 0 < _Pad; --_Pad) { // pad on right
            if (_Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ostr.fill()))) {
                _State |= ios_base::badbit;
            }
        }
        _CATCH_IO_(ios_base, _Ostr)
    }

    _Ostr.width(0);
    _Ostr.setstate(_State);
    return _Ostr;
}

_EXPORT_STD template <class _Elem, class _Traits>
basic_ostream<_Elem, _Traits>& operator<<(basic_ostream<_Elem, _Traits>& _Ostr, const _Elem* _Val) { // insert NTCS
    using _Myos = basic_ostream<_Elem, _Traits>;

    ios_base::iostate _State = ios_base::goodbit;
    streamsize _Count        = static_cast<streamsize>(_Traits::length(_Val));
    streamsize _Pad          = _Ostr.width() <= 0 || _Ostr.width() <= _Count ? 0 : _Ostr.width() - _Count;
    const typename _Myos::sentry _Ok(_Ostr);

    if (!_Ok) {
        _State |= ios_base::badbit;
    } else { // state okay, insert
        _TRY_IO_BEGIN
        if ((_Ostr.flags() & ios_base::adjustfield) != ios_base::left) {
            for (; 0 < _Pad; --_Pad) { // pad on left
                if (_Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ostr.fill()))) {
                    _State |= ios_base::badbit; // insertion failed, quit
                    break;
                }
            }
        }

        if (_State == ios_base::goodbit && _Ostr.rdbuf()->sputn(_Val, _Count) != _Count) {
            _State |= ios_base::badbit;
        }

        if (_State == ios_base::goodbit) {
            for (; 0 < _Pad; --_Pad) { // pad on right
                if (_Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ostr.fill()))) {
                    _State |= ios_base::badbit; // insertion failed, quit
                    break;
                }
            }
        }

        _Ostr.width(0);
        _CATCH_IO_(ios_base, _Ostr)
    }

    _Ostr.setstate(_State);
    return _Ostr;
}

_EXPORT_STD template <class _Elem, class _Traits>
basic_ostream<_Elem, _Traits>& operator<<(basic_ostream<_Elem, _Traits>& _Ostr, _Elem _Ch) { // insert a character
    using _Myos = basic_ostream<_Elem, _Traits>;

    ios_base::iostate _State = ios_base::goodbit;
    const typename _Myos::sentry _Ok(_Ostr);

    if (_Ok) { // state okay, insert
        streamsize _Pad = _Ostr.width() <= 1 ? 0 : _Ostr.width() - 1;

        _TRY_IO_BEGIN
        if ((_Ostr.flags() & ios_base::adjustfield) != ios_base::left) {
            for (; _State == ios_base::goodbit && 0 < _Pad; --_Pad) { // pad on left
                if (_Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ostr.fill()))) {
                    _State |= ios_base::badbit;
                }
            }
        }

        if (_State == ios_base::goodbit && _Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ch))) {
            _State |= ios_base::badbit;
        }

        for (; _State == ios_base::goodbit && 0 < _Pad; --_Pad) { // pad on right
            if (_Traits::eq_int_type(_Traits::eof(), _Ostr.rdbuf()->sputc(_Ostr.fill()))) {
                _State |= ios_base::badbit;
            }
        }
        _CATCH_IO_(ios_base, _Ostr)
    }

    _Ostr.width(0);
    _Ostr.setstate(_State);
    return _Ostr;
}

_EXPORT_STD template <class _Traits>
basic_ostream<char, _Traits>& operator<<(basic_ostream<char, _Traits>& _Ostr, const signed char* _Val) {
    // insert a signed char NTBS
    return _Ostr << reinterpret_cast<const char*>(_Val);
}

_EXPORT_STD template <class _Traits>
basic_ostream<char, _Traits>& operator<<(basic_ostream<char, _Traits>& _Ostr, signed char _Ch) { // insert a signed char
    return _Ostr << static_cast<char>(_Ch);
}

_EXPORT_STD template <class _Traits>
basic_ostream<char, _Traits>& operator<<(basic_ostream<char, _Traits>& _Ostr, const unsigned char* _Val) {
    // insert an unsigned char NTBS
    return _Ostr << reinterpret_cast<const char*>(_Val);
}

_EXPORT_STD template <class _Traits>
basic_ostream<char, _Traits>& operator<<(basic_ostream<char, _Traits>& _Ostr, unsigned char _Ch) {
    // insert an unsigned char
    return _Ostr << static_cast<char>(_Ch);
}

#ifdef __cpp_char8_t // These deleted overloads are specified in P1423.
// don't insert a UTF-8 NTBS
_EXPORT_STD template <class _Traits>
basic_ostream<char, _Traits>& operator<<(basic_ostream<char, _Traits>&, const char8_t*) = delete;
_EXPORT_STD template <class _Traits>
basic_ostream<wchar_t, _Traits>& operator<<(basic_ostream<wchar_t, _Traits>&, const char8_t*) = delete;

// don't insert a UTF-8 code unit
_EXPORT_STD template <class _Traits>
basic_ostream<char, _Traits>& operator<<(basic_ostream<char, _Traits>&, char8_t) = delete;
_EXPORT_STD template <class _Traits>
basic_ostream<wchar_t, _Traits>& operator<<(basic_ostream<wchar_t, _Traits>&, char8_t) = delete;
#endif // defined(__cpp_char8_t)

#if !_HAS_STREAM_INSERTION_OPERATORS_DELETED_IN_CXX20
#ifdef _NATIVE_WCHAR_T_DEFINED
_EXPORT_STD template <class _Traits>
basic_ostream<char, _Traits>& operator<<(basic_ostream<char, _Traits>&, wchar_t) = delete;

_EXPORT_STD template <class _Traits>
basic_ostream<char, _Traits>& operator<<(basic_ostream<char, _Traits>&, const wchar_t*) = delete;
#endif // _NATIVE_WCHAR_T_DEFINED

_EXPORT_STD template <class _Traits>
basic_ostream<char, _Traits>& operator<<(basic_ostream<char, _Traits>&, char16_t) = delete;
_EXPORT_STD template <class _Traits>
basic_ostream<char, _Traits>& operator<<(basic_ostream<char, _Traits>&, char32_t) = delete;

_EXPORT_STD template <class _Traits>
basic_ostream<wchar_t, _Traits>& operator<<(basic_ostream<wchar_t, _Traits>&, char16_t) = delete;
_EXPORT_STD template <class _Traits>
basic_ostream<wchar_t, _Traits>& operator<<(basic_ostream<wchar_t, _Traits>&, char32_t) = delete;

_EXPORT_STD template <class _Traits>
basic_ostream<char, _Traits>& operator<<(basic_ostream<char, _Traits>&, const char16_t*) = delete;
_EXPORT_STD template <class _Traits>
basic_ostream<char, _Traits>& operator<<(basic_ostream<char, _Traits>&, const char32_t*) = delete;

_EXPORT_STD template <class _Traits>
basic_ostream<wchar_t, _Traits>& operator<<(basic_ostream<wchar_t, _Traits>&, const char16_t*) = delete;
_EXPORT_STD template <class _Traits>
basic_ostream<wchar_t, _Traits>& operator<<(basic_ostream<wchar_t, _Traits>&, const char32_t*) = delete;
#endif // !_HAS_STREAM_INSERTION_OPERATORS_DELETED_IN_CXX20

template <class _Ostr, class _Ty, class = void>
struct _Can_stream_out : false_type {};

template <class _Ostr, class _Ty>
struct _Can_stream_out<_Ostr, _Ty, void_t<decltype(_STD declval<_Ostr&>() << _STD declval<const _Ty&>())>> : true_type {
};

_EXPORT_STD template <class _Ostr, class _Ty,
    enable_if_t<conjunction_v<is_convertible<_Ostr*, ios_base*>, _Can_stream_out<_Ostr, _Ty>>, int> = 0>
_Ostr&& operator<<(_Ostr&& _Os, const _Ty& _Val) { // insert to rvalue stream
    _Os << _Val;
    return _STD move(_Os);
}

_EXPORT_STD template <class _Elem, class _Traits>
basic_ostream<_Elem, _Traits>& __CLRCALL_OR_CDECL endl(
    basic_ostream<_Elem, _Traits>& _Ostr) { // insert newline and flush stream
    _Ostr.put(_Ostr.widen('\n'));
    _Ostr.flush();
    return _Ostr;
}

_EXPORT_STD template <class _Elem, class _Traits>
basic_ostream<_Elem, _Traits>& __CLRCALL_OR_CDECL ends(basic_ostream<_Elem, _Traits>& _Ostr) { // insert null character
    _Ostr.put(_Elem());
    return _Ostr;
}

_EXPORT_STD template <class _Elem, class _Traits>
basic_ostream<_Elem, _Traits>& __CLRCALL_OR_CDECL flush(basic_ostream<_Elem, _Traits>& _Ostr) { // flush stream
    _Ostr.flush();
    return _Ostr;
}

_EXPORT_STD template <class _Elem, class _Traits>
basic_ostream<_Elem, _Traits>& operator<<(basic_ostream<_Elem, _Traits>& _Ostr, const error_code& _Errcode) {
    // display error code
    return _Ostr << _Errcode.category().name() << ':' << _Errcode.value();
}
_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)

#endif // _STL_COMPILER_PREPROCESSOR
#endif // __MSVC_OSTREAM_HPP


#if _HAS_CXX23
#include <__msvc_filebuf.hpp>
#include <__msvc_print.hpp>
#include <format>
#endif // ^^^ _HAS_CXX23 ^^^

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN
#if _HAS_CXX20
#ifdef _CPPRTTI
_EXPORT_STD template <class _Elem, class _Traits>
basic_ostream<_Elem, _Traits>& emit_on_flush(basic_ostream<_Elem, _Traits>& _Ostr) {
    const auto _Sync_buf_ptr = dynamic_cast<_Basic_syncbuf_impl<_Elem, _Traits>*>(_Ostr.rdbuf());
    if (_Sync_buf_ptr) {
        _Sync_buf_ptr->set_emit_on_sync(true);
    }
    return _Ostr;
}

_EXPORT_STD template <class _Elem, class _Traits>
basic_ostream<_Elem, _Traits>& noemit_on_flush(basic_ostream<_Elem, _Traits>& _Ostr) {
    const auto _Sync_buf_ptr = dynamic_cast<_Basic_syncbuf_impl<_Elem, _Traits>*>(_Ostr.rdbuf());
    if (_Sync_buf_ptr) {
        _Sync_buf_ptr->set_emit_on_sync(false);
    }
    return _Ostr;
}

_EXPORT_STD template <class _Elem, class _Traits>
basic_ostream<_Elem, _Traits>& flush_emit(basic_ostream<_Elem, _Traits>& _Ostr) {
    _Ostr.flush();
    const auto _Sync_buf_ptr = dynamic_cast<_Basic_syncbuf_impl<_Elem, _Traits>*>(_Ostr.rdbuf());
    if (_Sync_buf_ptr) {
        ios_base::iostate _State = ios_base::goodbit;
        const typename basic_ostream<_Elem, _Traits>::sentry _Ok(_Ostr);
        if (!_Ok) {
            _State |= ios_base::badbit;
        } else {
            _TRY_IO_BEGIN
            const bool _Emit_failed = !_Sync_buf_ptr->_Do_emit();
            if (_Emit_failed) {
                _State |= ios_base::badbit;
            }
            _CATCH_IO_(ios_base, _Ostr)
        }
        _Ostr.setstate(_State);
    }
    return _Ostr;
}
#else // ^^^ defined(_CPPRTTI) / !defined(_CPPRTTI) vvv
_EXPORT_STD template <class _Elem, class _Traits>
basic_ostream<_Elem, _Traits>& emit_on_flush(basic_ostream<_Elem, _Traits>&) = delete; // requires /GR option
_EXPORT_STD template <class _Elem, class _Traits>
basic_ostream<_Elem, _Traits>& noemit_on_flush(basic_ostream<_Elem, _Traits>&) = delete; // requires /GR option
_EXPORT_STD template <class _Elem, class _Traits>
basic_ostream<_Elem, _Traits>& flush_emit(basic_ostream<_Elem, _Traits>&) = delete; // requires /GR option
#endif // ^^^ !defined(_CPPRTTI) ^^^
#endif // ^^^ _HAS_CXX20 ^^^

#if _HAS_CXX23
#ifdef _CPPRTTI
template <int = 0>
ios_base::iostate _Print_noformat_nonunicode(ostream& _Ostr, const string_view _Str) {
    // *LOCKED*
    //
    // This function is called from a context in which an ostream::sentry has been
    // constructed.
    ios_base::iostate _State = ios_base::goodbit;

    _TRY_IO_BEGIN
    const auto _Characters_written       = _Ostr.rdbuf()->sputn(_Str.data(), static_cast<streamsize>(_Str.size()));
    const bool _Was_insertion_successful = static_cast<size_t>(_Characters_written) == _Str.size();
    if (!_Was_insertion_successful) {
        _State |= ios_base::badbit;
    }
    _CATCH_IO_(ios_base, _Ostr)

    return _State;
}

template <int = 0>
ios_base::iostate _Print_newline_only_nonunicode(ostream& _Ostr) {
    // *LOCKED*
    //
    // This function is called from a context in which an ostream::sentry has been
    // constructed.
    ios_base::iostate _State = ios_base::goodbit;

    _TRY_IO_BEGIN
    const bool _Was_insertion_successful = _Ostr.rdbuf()->sputc('\n') == '\n';
    if (!_Was_insertion_successful) {
        _State |= ios_base::badbit;
    }
    _CATCH_IO_(ios_base, _Ostr)

    return _State;
}

template <int = 0>
void _Vprint_nonunicode_impl(
    const _Add_newline _Add_nl, ostream& _Ostr, const string_view _Fmt_str, const format_args _Fmt_args) {
    const ostream::sentry _Ok(_Ostr);
    ios_base::iostate _State = ios_base::goodbit;

    if (!_Ok) {
        _State |= ios_base::badbit;
    } else {
        // This is intentionally kept outside of the try/catch block in _Print_noformat_nonunicode()
        // (see N4950 [ostream.formatted.print]/3.2).
        string _Output_str = _STD vformat(_Ostr.getloc(), _Fmt_str, _Fmt_args);
        if (_Add_nl == _Add_newline::_Yes) {
            _Output_str.push_back('\n');
        }

        _State |= _STD _Print_noformat_nonunicode(_Ostr, _Output_str);
    }

    _Ostr.setstate(_State);
}

template <class _UnicodeConsoleFn, class _FallbackFn, class _Filebuf_type = filebuf>
ios_base::iostate _Do_on_maybe_unicode_console(
    ostream& _Ostr, _UnicodeConsoleFn _Unicode_console_func, _FallbackFn _Fallback_func) {
    // *LOCKED*
    //
    // This function is called from a context in which an ostream::sentry has been
    // constructed.
    ios_base::iostate _State = ios_base::goodbit;

    // The std::ostream& overload of vprint_unicode() expects you to determine whether the stream refers
    // to a unicode console or not based solely on the std::ostream& provided. That class simply doesn't
    // provide enough information to know this in every case. The best we can do (without breaking ABI)
    // is check if the underlying std::streambuf object of the std::ostream& actually refers to a std::filebuf
    // object. This requires the use of a dynamic_cast. (This is also why the std::ostream& overloads of
    // std::print() et al. are deleted when RTTI is disabled.)
    streambuf* const _Streambuf = _Ostr.rdbuf();
    const auto _Filebuf         = dynamic_cast<_Filebuf_type*>(_Streambuf);

    // If we got nullptr, then it probably isn't being used for a unicode console...
    if (_Filebuf == nullptr) {
        _State |= _Fallback_func();
        return _State;
    }

    FILE* const _File_stream = _Filebuf->_Myfile;
    const __std_unicode_console_retrieval_result _Unicode_console_retrieval_result{
        __std_get_unicode_console_handle_from_file_stream(_File_stream)};

    // See the documentation for __std_unicode_console_retrieval_result to understand why we do this.
    bool _Is_unicode_console;

#pragma warning(push)
#pragma warning(disable : 4061) // enumerator not explicitly handled by switch label
    switch (_Unicode_console_retrieval_result._Error) {
    case __std_win_error::_Success:
        _Is_unicode_console = true;
        break;

    case __std_win_error::_File_not_found:
        _Is_unicode_console = false;
        break;

    case __std_win_error::_Not_supported:
        return _State;

    default:
        return ios_base::failbit;
    }
#pragma warning(pop)

    if (_Is_unicode_console) {
        _TRY_IO_BEGIN
        const bool _Was_flush_successful = _Ostr.rdbuf()->pubsync() != -1;
        if (!_Was_flush_successful) {
            _State |= ios_base::badbit;
            return _State;
        }

        const __std_win_error _Unicode_console_print_result =
            _Unicode_console_func(_Unicode_console_retrieval_result._Console_handle);
        if (_Unicode_console_print_result != __std_win_error::_Success) {
            _State |= ios_base::badbit;
        }
        _CATCH_IO_(ios_base, _Ostr)
    } else {
        _State |= _Fallback_func();
    }

    return _State;
}

template <int = 0>
ios_base::iostate _Print_noformat_unicode(ostream& _Ostr, const string_view _Str) {
    const auto _Unicode_console = [&](const __std_unicode_console_handle _Console_handle) {
        return __std_print_to_unicode_console(_Console_handle, _Str.data(), _Str.size());
    };

    const auto _Fallback = [&] { return _STD _Print_noformat_nonunicode(_Ostr, _Str); };

    return _STD _Do_on_maybe_unicode_console(_Ostr, _Unicode_console, _Fallback);
}

template <int = 0>
ios_base::iostate _Print_newline_only_unicode(ostream& _Ostr) {
    const auto _Unicode_console = [](const __std_unicode_console_handle _Console_handle) {
        return __std_print_newline_only_to_unicode_console(_Console_handle);
    };

    const auto _Fallback = [&] { return _STD _Print_newline_only_nonunicode(_Ostr); };

    return _STD _Do_on_maybe_unicode_console(_Ostr, _Unicode_console, _Fallback);
}

template <int = 0>
void _Vprint_unicode_impl(
    const _Add_newline _Add_nl, ostream& _Ostr, const string_view _Fmt_str, const format_args _Fmt_args) {
    const ostream::sentry _Ok(_Ostr);
    ios_base::iostate _State = ios_base::goodbit;

    if (!_Ok) {
        _State |= ios_base::badbit;
    } else {
        // This is intentionally kept outside of the try/catch block in _Print_noformat_unicode()
        // (see N4950 [ostream.formatted.print]/3.2).
        string _Output_str = _STD vformat(_Ostr.getloc(), _Fmt_str, _Fmt_args);
        if (_Add_nl == _Add_newline::_Yes) {
            _Output_str.push_back('\n');
        }

        _State |= _STD _Print_noformat_unicode(_Ostr, _Output_str);
    }

    _Ostr.setstate(_State);
}

template <int = 0>
void _Print_noformat(ostream& _Ostr, const string_view _Str) {
    const ostream::sentry _Ok(_Ostr);
    ios_base::iostate _State = ios_base::goodbit;

    if (!_Ok) {
        _State |= ios_base::badbit;
    } else {
        if constexpr (_STD _Is_ordinary_literal_encoding_utf8()) {
            _State |= _STD _Print_noformat_unicode(_Ostr, _Str);
        } else {
            _State |= _STD _Print_noformat_nonunicode(_Ostr, _Str);
        }
    }

    _Ostr.setstate(_State);
}

template <class... _Types>
void _Print_impl(const _Add_newline _Add_nl, ostream& _Ostr, const format_string<_Types...> _Fmt, _Types&&... _Args) {
    constexpr bool _Has_format_args = sizeof...(_Types) > 0;

    // This is intentionally kept outside of the try/catch block in _Print_noformat_*()
    // (see N4950 [ostream.formatted.print]/3.2).

    if constexpr (_Has_format_args) {
        if constexpr (_STD _Is_ordinary_literal_encoding_utf8()) {
            _STD _Vprint_unicode_impl(_Add_nl, _Ostr, _Fmt.get(), _STD make_format_args(_Args...));
        } else {
            _STD _Vprint_nonunicode_impl(_Add_nl, _Ostr, _Fmt.get(), _STD make_format_args(_Args...));
        }
    } else {
        const string _Unescaped_str{_Unescape_braces(_Add_nl, _Fmt.get())};
        _STD _Print_noformat(_Ostr, _Unescaped_str);
    }
}

_EXPORT_STD template <class... _Types>
void print(ostream& _Ostr, const format_string<_Types...> _Fmt, _Types&&... _Args) {
    _STD _Print_impl(_Add_newline::_Nope, _Ostr, _Fmt, _STD forward<_Types>(_Args)...);
}

_EXPORT_STD template <class... _Types>
void println(ostream& _Ostr, const format_string<_Types...> _Fmt, _Types&&... _Args) {
    _STD _Print_impl(_Add_newline::_Yes, _Ostr, _Fmt, _STD forward<_Types>(_Args)...);
}

_EXPORT_STD template <int = 0> // improves throughput, see GH-2329
void println(ostream& _Ostr) {
    const ostream::sentry _Ok(_Ostr);
    ios_base::iostate _State = ios_base::goodbit;

    if (!_Ok) {
        _State |= ios_base::badbit;
    } else {
        if constexpr (_STD _Is_ordinary_literal_encoding_utf8()) {
            _State |= _STD _Print_newline_only_unicode(_Ostr);
        } else {
            _State |= _STD _Print_newline_only_nonunicode(_Ostr);
        }
    }

    _Ostr.setstate(_State);
}

_EXPORT_STD template <int = 0> // improves throughput, see GH-2329
void vprint_unicode(ostream& _Ostr, const string_view _Fmt_str, const format_args _Fmt_args) {
    _STD _Vprint_unicode_impl(_Add_newline::_Nope, _Ostr, _Fmt_str, _Fmt_args);
}

_EXPORT_STD template <int = 0> // improves throughput, see GH-2329
void vprint_nonunicode(ostream& _Ostr, const string_view _Fmt_str, const format_args _Fmt_args) {
    _STD _Vprint_nonunicode_impl(_Add_newline::_Nope, _Ostr, _Fmt_str, _Fmt_args);
}
#else // ^^^ defined(_CPPRTTI) / !defined(_CPPRTTI) vvv
_EXPORT_STD template <class... _Types>
void print(ostream&, format_string<_Types...>, _Types&&...) = delete; // requires /GR option

_EXPORT_STD template <class... _Types>
void println(ostream&, format_string<_Types...>, _Types&&...) = delete; // requires /GR option

_EXPORT_STD void vprint_unicode(ostream&, string_view, format_args)    = delete; // requires /GR option
_EXPORT_STD void vprint_nonunicode(ostream&, string_view, format_args) = delete; // requires /GR option
#endif // ^^^ !defined(_CPPRTTI) ^^^
#endif // ^^^ _HAS_CXX23 ^^^

_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _OSTREAM_


#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new
_STD_BEGIN
#ifdef _M_CEE_PURE
__PURE_APPDOMAIN_GLOBAL extern istream cin;
__PURE_APPDOMAIN_GLOBAL extern ostream cout;
__PURE_APPDOMAIN_GLOBAL extern ostream cerr;
__PURE_APPDOMAIN_GLOBAL extern ostream clog;
__PURE_APPDOMAIN_GLOBAL extern istream* _Ptr_cin;
__PURE_APPDOMAIN_GLOBAL extern ostream* _Ptr_cout;
__PURE_APPDOMAIN_GLOBAL extern ostream* _Ptr_cerr;
__PURE_APPDOMAIN_GLOBAL extern ostream* _Ptr_clog;

__PURE_APPDOMAIN_GLOBAL extern wistream wcin;
__PURE_APPDOMAIN_GLOBAL extern wostream wcout;
__PURE_APPDOMAIN_GLOBAL extern wostream wcerr;
__PURE_APPDOMAIN_GLOBAL extern wostream wclog;
__PURE_APPDOMAIN_GLOBAL extern wistream* _Ptr_wcin;
__PURE_APPDOMAIN_GLOBAL extern wostream* _Ptr_wcout;
__PURE_APPDOMAIN_GLOBAL extern wostream* _Ptr_wcerr;
__PURE_APPDOMAIN_GLOBAL extern wostream* _Ptr_wclog;
#else // ^^^ defined(_M_CEE_PURE) / !defined(_M_CEE_PURE) vvv
_EXPORT_STD extern "C++" __PURE_APPDOMAIN_GLOBAL _CRTDATA2_IMPORT istream cin;
_EXPORT_STD extern "C++" __PURE_APPDOMAIN_GLOBAL _CRTDATA2_IMPORT ostream cout;
_EXPORT_STD extern "C++" __PURE_APPDOMAIN_GLOBAL _CRTDATA2_IMPORT ostream cerr;
_EXPORT_STD extern "C++" __PURE_APPDOMAIN_GLOBAL _CRTDATA2_IMPORT ostream clog;
extern "C++" __PURE_APPDOMAIN_GLOBAL _CRTDATA2_IMPORT istream* _Ptr_cin;
extern "C++" __PURE_APPDOMAIN_GLOBAL _CRTDATA2_IMPORT ostream* _Ptr_cout;
extern "C++" __PURE_APPDOMAIN_GLOBAL _CRTDATA2_IMPORT ostream* _Ptr_cerr;
extern "C++" __PURE_APPDOMAIN_GLOBAL _CRTDATA2_IMPORT ostream* _Ptr_clog;

_EXPORT_STD extern "C++" __PURE_APPDOMAIN_GLOBAL _CRTDATA2_IMPORT wistream wcin;
_EXPORT_STD extern "C++" __PURE_APPDOMAIN_GLOBAL _CRTDATA2_IMPORT wostream wcout;
_EXPORT_STD extern "C++" __PURE_APPDOMAIN_GLOBAL _CRTDATA2_IMPORT wostream wcerr;
_EXPORT_STD extern "C++" __PURE_APPDOMAIN_GLOBAL _CRTDATA2_IMPORT wostream wclog;
extern "C++" __PURE_APPDOMAIN_GLOBAL _CRTDATA2_IMPORT wistream* _Ptr_wcin;
extern "C++" __PURE_APPDOMAIN_GLOBAL _CRTDATA2_IMPORT wostream* _Ptr_wcout;
extern "C++" __PURE_APPDOMAIN_GLOBAL _CRTDATA2_IMPORT wostream* _Ptr_wcerr;
extern "C++" __PURE_APPDOMAIN_GLOBAL _CRTDATA2_IMPORT wostream* _Ptr_wclog;

#ifdef _CRTBLD // TRANSITION, ABI: _Winit appears to be unused
class _CRTIMP2_PURE_IMPORT _Winit {
public:
    __thiscall _Winit();
    __thiscall ~_Winit() noexcept;

private:
    __PURE_APPDOMAIN_GLOBAL static int _Init_cnt;
};
#endif // defined(_CRTBLD)

#endif // ^^^ !defined(_M_CEE_PURE) ^^^
_STD_END
#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _IOSTREAM_

int main(){
    if(1){if(1){if(1){if(1){if(1){if(1){if(1){if(1){if(1){if(1){if(1){std::cout<<"helloworld"<<std::endl;}}}}}}}}}}}
    return 0;
}