#ifndef __EDITOR_WINDOW_HEADER_FILE__
#define __EDITOR_WINDOW_HEADER_FILE__

#include <string>

#include <Eigen/Dense>

typedef struct SDL_Window SDL_Window;
typedef struct SDL_GLContextState *SDL_GLContext;

namespace Editor
{
    class Window
    {
    public:
        Window();
        ~Window();

        int32_t create(int width, int height, const char *title);
        void destroy();

        void use();
        void swap();

        static void pollEvents();

        void setClearColor(Eigen::Vector4f color);

        bool keep_open;

        SDL_Window *getHandle();
        SDL_GLContext getContext();

    private:
        SDL_Window *handle;
        SDL_GLContext ogl_context;

        std::string title;

        Eigen::Vector4f background;
        Eigen::Vector2i size;
    };
}

#endif