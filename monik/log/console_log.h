// console_log.h
//
#pragma once
#ifndef __MONIK_LOG_CONSOLE_LOG_H__
#define __MONIK_LOG_CONSOLE_LOG_H__

#include "monik/log/message.h"

namespace monik { namespace log { 

class console_log final : public channel { // thread safe
public:
    explicit console_log(buf_size_t);
    ~console_log() override;
    bool empty() const override;
    void log(message_with_severity &&) override; 
private:
    class data_type;
    const std::unique_ptr<data_type> m_data;
};

} // log
} // monik

#endif // __MONIK_LOG_CONSOLE_LOG_H__

