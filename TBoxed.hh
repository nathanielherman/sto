#pragma once
#include "TWrapped.hh"

template <typename T,
          TOpacity Opaque = TOpacity::opaque,
          bool Trivial = mass::is_trivially_copyable<T>::value
          > class TBoxed;

template <typename T, TOpacity Opaque>
class TBoxed<T, Opaque, true /* trivial */> {
public:
    typedef typename TWrapped<T, Opaque>::version_type version_type;
    typedef typename TWrapped<T, Opaque>::read_type read_type;

    template <typename... Args> TBoxed(Args&&... args)
        : value_(std::forward<Args>(args)...) {
    }

    const T& access() const {
        return value_.access();
    }
    T& access() {
        return value_.access();
    }
    read_type snapshot(TransProxy item) const {
        return value_.snapshot(item, version_);
    }
    read_type wait_snapshot(TransProxy item, bool remember) const {
        return value_.wait_snapshot(item, version_, remember);
    }
    read_type read(TransProxy item) const {
        return value_.read(item, version_);
    }
    bool writable(const T& v) const {
        return true;
    }
    void write(const T& v) {
        value_.write(v);
    }
    void write(T&& v) {
        value_.write(std::move(v));
    }

private:
    version_type version_;
    TWrapped<T, Opaque> value_;
};

template <typename T, TOpacity Opaque>
class TBoxed<T, Opaque, false /* !trivial */> {
public:
    typedef typename TWrapped<T, Opaque>::version_type version_type;
    typedef const T& read_type;

    template <typename... Args> TBoxed(Args&&... args)
        : value_(std::forward<Args>(args)...) {
    }

    const T& access() const {
        return value_;
    }
    T& access() {
        return value_;
    }
    read_type snapshot(TransProxy) const {
        return value_;
    }
    read_type wait_snapshot(TransProxy item, bool remember) const {
        item.observe(version_, remember);
        fence();
        return value_;
    }
    read_type read(TransProxy item) const {
        item.observe(version_, true);
        fence();
        return value_;
    }
    bool writable(const T&) const {
        return false;
    }
    void write(const T&) {
        always_assert(false);
    }
    void write(T&&) {
        always_assert(false);
    }

private:
    version_type version_;
    T value_;
};
