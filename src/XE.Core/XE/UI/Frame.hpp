
#ifndef __XE_UI_FRAME_HPP__
#define __XE_UI_FRAME_HPP__

#include <cstddef>
#include <string>
#include <XE/Math/Vector.hpp>

namespace XE::UI {
    struct WidgetProperties {
        std::string Caption;
        XE::Math::Vector2i Position;
        XE::Math::Vector2i Size;
    };

    class Widget {
    public:
        virtual Widget* GetChild(const int index) = 0;
        virtual const Widget* GetChild(const int index) const = 0;
        virtual int GetChildCount() const = 0;

        virtual WidgetProperties GetProperties() const = 0;
        virtual void SetProperties(const WidgetProperties &properties) = 0;
    };

    /**
     * @brief Abstract a (possible) native OS window frame, for presenting rendering content to the user.
     */
    class Frame : public Widget {
    public:

    };
}

#endif
