
#ifndef __XE_EVENT_HPP__
#define __XE_EVENT_HPP__

#include <functional>

namespace XE {
    template<typename ... Args>
    class Event {
    public:
        virtual void connect(std::function<void (Args ... args)> callback) = 0;

        virtual void raise(Args ... args) = 0;
    };

}

#endif
