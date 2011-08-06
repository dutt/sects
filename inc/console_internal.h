#ifndef CONSOLE_INTERNAL_H
#define CONSOLE_INTERNAL_H

#include <cstring>

template<typename T>
class ConsoleBufferBase : public Console
{
public:
    ConsoleBufferBase(uint32 width, uint32 height)
        : Console(width, height) {
        info = new T[width * height];
    }

    ConsoleBufferBase(const ConsoleBufferBase& buf)
        : Console(buf.width, buf.height) {
        info = new T[width * height];
        memcpy(info, buf.info, sizeof(T) * width * height);
    }

    ConsoleBufferBase& operator=(const ConsoleBufferBase& buf) {
        if (&buf != this) {
            delete[] info;
            width = buf.width;
            height = buf.height;
            info = new T[width * height];
            memcpy(info, buf.info, sizeof(T) * width * height);
        }
        return *this;
    }

    virtual ~ConsoleBufferBase() {
        delete[] info;
    }

    virtual void resize(uint32 newWidth, uint32 newHeight)
    {
        if (newWidth != width || newHeight != height) {
            delete[] info;
            info = new T[newWidth * newHeight];
            width = newWidth;
            height = newHeight;
        }
    }

    virtual void blit(uint32 destX, uint32 destY, uint32 width, uint32 height,
                      const Console* source, uint32 srcX = 0, uint32 srcY = 0) {
        const ConsoleBufferBase* src = dynamic_cast<const ConsoleBufferBase*>(source);
        if (srcX == 0 && width == src->width) {
            if (destX == 0 && width == this->width) {
                memcpy(info + (destY * width), src->info + (srcY * width),
                       height * width * sizeof(T));
                return;
            }
        }

        if (width == 0)
            return;

        T* drow = info + (destY * this->width);
        const T* srow = src->info + (srcY * src->width);
        for (uint32 y = 0; y < height; ++y) {
            memcpy(drow + destX, srow + srcX, width * sizeof(T));
            drow += this->width;
            srow += src->width;
        }
    }

protected:
    T* info;
};

#endif /* CONSOLE_INTERNAL_H */
