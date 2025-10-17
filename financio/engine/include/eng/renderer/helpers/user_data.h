#pragma once

namespace eng {

template<typename T>
struct request_userdata {
    T    result{};
    bool request_ended = false;
};

    
} // namespace eng
