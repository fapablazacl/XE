
#ifndef __XE_EVENTIMPL_HPP__
#define __XE_EVENTIMPL_HPP__

#include "Event.hpp"

#include <vector>

namespace XE {
    template<typename ... Args>
    class EventImpl : public Event<Args ...> {
    public:
        virtual void connect(std::function<void (Args ... args)> callback) override {
            m_callbacks.push_back(callback);
        }

        virtual void raise(Args ... args) override {
            for (auto cb : m_callbacks) {
                cb(args ...);
            }
        }

    private:
        std::vector<std::function<void (Args ...)>> m_callbacks;
    };
}

#endif
