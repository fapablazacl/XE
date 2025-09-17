
#pragma once 

[[deprecated]]
struct ImageData {
    int width = 0;
    int height = 0;
    int bpp = 0;
    uint8_t* pixels = nullptr;

    ImageData() {}
};


[[deprecated]]
class Image {
public:
    virtual ~Image() {}

    virtual ImageData getData() const = 0;
};
