#include <editor/editor.hpp>

#include <stdio.h>

#include <SDL3/SDL.h>

#include <imgui.h>
#include <imgui_md.h>
#include <imgui_stdlib.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>

#include <editor/ogl.hpp>
#include <editor/window.hpp>

Editor::Window window;

static ImFont *font_h1[4];
static ImFont *font_h2[4];
static ImFont *font_h3[4];
static ImFont *font_h4[4];
static ImFont *font_h5[4];
static ImFont *font_h6[4];
static ImFont *font_p[4];
static ImFont *font_small[4];
static ImFont *font_tiny[4];

static float font_h1_size = 47.78f;
static float font_h2_size = 39.81f;
static float font_h3_size = 33.18f;
static float font_h4_size = 27.65f;
static float font_h5_size = 23.04f;
static float font_h6_size = 19.2f;
static float font_p_size = 16.0f;
static float font_small_size = 13.33;
static float font_tiny_size = 11.11f;

static ImFont *g_font_regular;
static ImFont *g_font_bold;
static ImFont *g_font_bold_large;
static ImTextureID g_texture1 = 0;

static void calculateFontScaling(float normal_font_size, float ratio)
{
    font_h1_size = normal_font_size * pow(ratio, 6);
    font_h2_size = normal_font_size * pow(ratio, 5);
    font_h3_size = normal_font_size * pow(ratio, 4);
    font_h4_size = normal_font_size * pow(ratio, 3);
    font_h5_size = normal_font_size * pow(ratio, 2);
    font_h6_size = normal_font_size * pow(ratio, 1);
    font_p_size = normal_font_size;
    font_small_size = normal_font_size / pow(ratio, 1);
    font_tiny_size = normal_font_size / pow(ratio, 2);
}

static int loadMakdownFontScales(ImGuiIO &io, int font_type, const char *filename)
{
    font_p[font_type] = io.Fonts->AddFontFromFileTTF(filename, font_p_size);
    font_h1[font_type] = io.Fonts->AddFontFromFileTTF(filename, font_h1_size);
    font_h2[font_type] = io.Fonts->AddFontFromFileTTF(filename, font_h2_size);
    font_h3[font_type] = io.Fonts->AddFontFromFileTTF(filename, font_h3_size);
    font_h4[font_type] = io.Fonts->AddFontFromFileTTF(filename, font_h4_size);
    font_h5[font_type] = io.Fonts->AddFontFromFileTTF(filename, font_h5_size);
    font_h6[font_type] = io.Fonts->AddFontFromFileTTF(filename, font_h6_size);
    font_tiny[font_type] = io.Fonts->AddFontFromFileTTF(filename, font_small_size);

    return 0;
}

int32_t Editor::init()
{
    int error = SDL_Init(SDL_INIT_VIDEO);
    if (!error)
    {
        printf("[E][SDL] %s\n", SDL_GetError());
        return error;
    }
    printf("Init SDL3\n");

    error = window.create(800, 600, "hello");
    if (error)
    {
        SDL_Quit();
        return error;
    }
    window.use();
    printf("Init Editor\n");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Dear ImGui style
    // ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(window.getHandle(), window.getContext());
    ImGui_ImplOpenGL3_Init("#version 330 core");

    io.IniFilename = 0;

    calculateFontScaling(16, 1.125);

    loadMakdownFontScales(io, 0, "data/ttf/Hack-Regular.ttf");
    loadMakdownFontScales(io, 1, "data/ttf/Hack-Bold.ttf");
    loadMakdownFontScales(io, 2, "data/ttf/Hack-Italic.ttf");
    loadMakdownFontScales(io, 3, "data/ttf/Hack-BoldItalic.ttf");

    return 0;
}

int32_t Editor::terminate()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    window.destroy();
    printf("Terminate Editor\n");
    return 0;
}

void Editor::clear()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void Editor::startFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void Editor::EndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

#include "imgui_md.h"

// Fonts and images (ImTextureID) must be loaded in other place
// see https://github.com/ocornut/imgui/blob/master/docs/FONTS.md

struct my_markdown : public imgui_md
{
    ImFont *get_font() const override
    {
        if (m_is_table_header)
        {
            return font_p[1];
        }

        int type = 0;
        if (m_is_strong)
        {
            type = 1;
        }
        if (m_is_em)
        {
            type += 2;
        }

        switch (m_hlevel)
        {
        case 0:
            return font_p[type];
        case 1:
            return font_h1[type];
        case 2:
            return font_h2[type];
        case 3:
            return font_h3[type];
        case 4:
            return font_h4[type];
        case 5:
            return font_h5[type];
        case 6:
            return font_h6[type];
        case 7:
            return font_small[type];
        case 8:
            return font_tiny[type];
        default:
            return g_font_bold;
        }
    };

    void open_url() const override
    {
        // platform dependent code
        SDL_OpenURL(m_href.c_str());
    }

    bool get_image(image_info &nfo) const override
    {
        // use m_href to identify images
        nfo.texture_id = g_texture1;
        nfo.size = {40, 20};
        nfo.uv0 = {0, 0};
        nfo.uv1 = {1, 1};
        nfo.col_tint = {1, 1, 1, 1};
        nfo.col_border = {0, 0, 0, 0};
        return true;
    }

    void html_div(const std::string &dclass, bool e) override
    {
        if (dclass == "red")
        {
            if (e)
            {
                m_table_border = false;
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
            }
            else
            {
                ImGui::PopStyleColor();
                m_table_border = true;
            }
        }
    }
};

// call this function to render your markdown
void Editor::markdown(const char *str, const char *str_end)
{
    static my_markdown s_printer;
    s_printer.print(str, str_end);
}
