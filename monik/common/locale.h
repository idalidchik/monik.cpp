// locale.h
#pragma once
#ifndef __MONIK_COMMON_LOCALE_H__
#define __MONIK_COMMON_LOCALE_H__

#include "monik/common/common.h"

namespace monik {

class setlocale_t final {
    setlocale_t(){}
    ~setlocale_t(){}
    using lock_guard = std::lock_guard<std::mutex>;
    std::mutex mutex;
    std::string locale;
    static setlocale_t & instance() {
        static setlocale_t obj;
        return obj;
    }
public:
    static std::string get() {
        lock_guard lock(instance().mutex);
        return instance().locale;
    }
    static void set(std::string && s) {
        lock_guard lock(instance().mutex);
        if (s != instance().locale) {
            MONIK_TRACE("setlocale = \"", s, "\"");
            setlocale(LC_ALL, s.c_str());
            instance().locale = std::move(s);
        }
    }
    static void set(std::string const & s) {
        setlocale_t::set(std::string(s));
    }
    static void set_default() {
        lock_guard lock(instance().mutex);
        if (!instance().locale.empty()) {
            MONIK_TRACE("setlocale = \"\"");
            setlocale(LC_ALL, "");
            instance().locale.clear();
        }
    }
    class auto_locale: noncopyable {
        std::string old;
    public:
        auto_locale(): old(setlocale_t::get()) {
            setlocale_t::set_default();
        }
        explicit auto_locale(std::string && s): old(setlocale_t::get()) {
            setlocale_t::set(std::move(s));
        }
        ~auto_locale() {
            setlocale_t::set(std::move(old));
        }
    };
};

} // namespace monik

#endif // __MONIK_COMMON_LOCALE_H__
