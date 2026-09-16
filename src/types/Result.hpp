#pragma once

#include <type_traits>
template<class T, class E>
class Result {
public:
    Result() {
        
    }
    
    Result(T&& value) {
        this->m_value = value;
        this->m_hasValue = true;
    }

    Result(E&& err) {
        this->m_error = err;
        this->m_hasValue = false;
    }

    operator bool() const {
        return this->m_hasValue;
    }

    const T& Val() const {
        return this->m_value;
    }
    

private:
    T m_value{};
    E m_error{};
    bool m_hasValue = false;
};

