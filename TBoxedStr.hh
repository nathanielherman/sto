#pragma once

#include "TBoxed.hh"
#include "str.hh"

template <TOpacity Opaque> 
class TBoxedStr {
public:
    typedef typename TWrapped<lcdf::Str, Opaque>::version_type version_type;



    lcdf::Str access() const {
        return lcdf::Str(s_, length_);
    }
    std::string snapshot(TransProxy item) const {
        std::string x;
        while (1) {
            version_type v0 = version_;
            fence();
            x.assign(s_, length_);
            fence();
            version_type v1 = version_;
            if (v0 == v1 || v1.is_locked()) {
                if (Opaque == TOpacity::opaque)
                    item.observe(v1, false);
                return x;
            }
            relax_fence();
        }
    }
    std::string wait_snapshot(TransProxy item, bool remember) const {
        unsigned n = 0;
        std::string x;
        while (1) {
            version_type v0 = version_;
            fence();
            x.assign(s_, length_);
            fence();
            version_type v1 = version_;
            if (v0 == v1 && !v1.is_locked_elsewhere(item.transaction())) {
                
    }
    std::string read(TransProxy item) const {
    }
    bool writable(lcdf::Str x) const {
        return x.length() <= capacity_;
    }
    bool writable(const std::string& x) const {
        return x.length() <= capacity_;
    }
    void write(lcdf::Str x) {
        assert(x.length() <= capacity_);
        memcpy(s_, x.data(), x.length());
        length_ = x.length();
    }
    void write(const std::string& x) {
        assert(x.length() <= capacity_);
        memcpy(s_, x.data(), x.length());
        length_ = x.length();
    }

private:
    version_type version_;
    uint32_t length_;
    uint32_t capacity_;
    char s_[0];
};
