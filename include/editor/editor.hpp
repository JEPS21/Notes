#ifndef __EDITOR_EDITOR_HEADER_FILE__
#define __EDITOR_EDITOR_HEADER_FILE__

#include <stdint.h>

#include <editor/window.hpp>

extern Editor::Window window;
extern bool keep_running;
namespace Editor{

    int32_t init();
    int32_t terminate();

    void clear();
    void startFrame();
    void EndFrame();

    void markdown(const char* str, const char* str_end);

};



#endif
