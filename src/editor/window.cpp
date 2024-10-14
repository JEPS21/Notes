#include <editor/window.hpp>

#include <assert.h>

#include <vector>

#include <SDL3/SDL.h>

#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>

#include <editor/ogl.hpp>

static std::vector<Editor::Window *> _windows;
static Editor::Window *_current_window = 0;

Editor::Window::Window() : keep_open(false), handle(0), ogl_context(0), size(0, 0)
{
}

Editor::Window::~Window()
{
}

int32_t Editor::Window::create(int width, int height, const char *title)
{
    if (handle)
    {
        printf("[W][SDL] Window was create\n");
        return 1;
    }

    if (ogl_context)
    {
        printf("[W][SDL] Window has OpenGL context\n");
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_WindowFlags window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
    handle = SDL_CreateWindow(title, width, height, window_flags);
    if (!handle)
    {
        printf("[E][SDL] Failed to create window: %s\n", SDL_GetError());
        return 2;
    }

    SDL_SetWindowPosition(handle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    printf("Create Window: %s\n", title);

    ogl_context = SDL_GL_CreateContext(handle);
    if (!ogl_context)
    {
        printf("[E][SDL] Failed to create OpenGL context: %s\n", SDL_GetError());

        SDL_DestroyWindow(handle);
        handle = 0;
        return 3;
    }

    this->title = title;
    size.x() = width;
    size.y() = height;

    this->keep_open = true;

    use();

    int glad_gl_loaded = gladLoadGL(SDL_GL_GetProcAddress);
    if (!glad_gl_loaded)
    {
        printf("[E][GLAD] Failed to load OpenGL functions: %s\n", SDL_GetError());

        SDL_GL_DestroyContext(ogl_context);
        ogl_context = 0;

        SDL_DestroyWindow(handle);
        handle = 0;

        return 4;
    }

    SDL_GL_SetSwapInterval(1);

    SDL_ShowWindow(handle);

    for (int i = 0; i < (int)_windows.size(); ++i)
    {
        if (_windows[i] == 0)
        {
            _windows[i] = this;
            return 0;
        }
    }
    _windows.push_back(this);

    return 0;
}

void Editor::Window::destroy()
{
    if (ogl_context)
    {
        SDL_GL_DestroyContext(ogl_context);
        ogl_context = 0;
    }
    if (handle)
    {
        SDL_DestroyWindow(handle);
        handle = 0;

        printf("Window %s destroy\n", title.c_str());
        title.clear();

        for (int i = 0; i < (int)_windows.size(); ++i)
        {
            if (_windows[i] == this)
            {
                _windows[i] = 0;
                return;
            }
        }

        assert(0);
    }
}

void Editor::Window::use()
{
    assert(handle && ogl_context);
    if (_current_window == this)
        return;
    if (!SDL_GL_MakeCurrent(handle, ogl_context))
    {
        printf("[E][SDL] can't make window %s and/or context current: %s\n", title.c_str(), SDL_GetError());
        return;
    }
    _current_window = this;
    printf("Current Window %s\n", title.c_str());
}

void Editor::Window::swap()
{
    SDL_GL_SwapWindow(handle);
}

void Editor::Window::pollEvents()
{
    static SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            for (int i = 0; i < (int)_windows.size(); ++i)
            {
                _windows[i]->keep_open = false;
            }
            break;

        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            for (int i = 0; i < (int)_windows.size(); ++i)
            {
                _windows[i]->keep_open = false;
            }
            break;
        
        default:
            break;
        }
    }
}

void Editor::Window::setClearColor(Eigen::Vector4f color)
{
    Window *current = _current_window;
    use();

    background = color;
    glClearColor(background.x(), background.y(), background.z(), background.w());

    if (current)
        current->use();
    else
        _current_window = current;
}

SDL_Window *Editor::Window::getHandle()
{
    return handle;
}

SDL_GLContext Editor::Window::getContext()
{
    return ogl_context;
}