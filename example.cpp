#include <SDL.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_sdl2.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <apifilesystem/ghc/filesystem.hpp>
#include <apifilesystem/filesystem.hpp>
#include <ctime>

#if defined(_WIN32)
#include <windows.h>
#endif

#include <GL/glew.h>
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#if (defined(_WIN32) && defined(_MSC_VER))
#pragma comment(lib, "opengl32.lib")
#endif

#include "ImFileDialog.h"

// SDL defines main
#undef main

int main(int argc, char *argv[]) {
  bool run = true;
  srand(time(nullptr));
  #if defined(_WIN32)
  SetConsoleOutputCP(CP_UTF8);
  #endif

  // init sdl2
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
    printf("Failed to initialize SDL2\n");
    return 0;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  Uint32 windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;

  // create the window
  int wndWidth = 1024, wndHeight = 768;
  SDL_Window *wnd = SDL_CreateWindow("File Dialog", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, wndWidth, wndHeight, windowFlags);
  SDL_MaximizeWindow(wnd);
  
  // create the GL context
  SDL_GLContext glContext = SDL_GL_CreateContext(wnd);
  SDL_GL_MakeCurrent(wnd, glContext);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);

  // init glew
  glewExperimental = true;
  if (glewInit() != GLEW_OK) {
    printf("Failed to initialize GLEW\n");
    return 0;
  }

  // imgui
  static std::string ini;
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  #if defined(_WIN32)
  if (ngs::fs::environment_get_variable("IMGUI_CONFIG_HOME").empty())
    ngs::fs::environment_set_variable("IMGUI_CONFIG_HOME", ngs::fs::environment_get_variable("USERPROFILE"));
  #else
  if (ngs::fs::environment_get_variable("IMGUI_CONFIG_HOME").empty())
    ngs::fs::environment_set_variable("IMGUI_CONFIG_HOME", ngs::fs::environment_get_variable("HOME"));
  #endif
  if (ngs::fs::environment_get_variable("IMGUI_CONFIG_FOLDER").empty())
    ngs::fs::environment_set_variable("IMGUI_CONFIG_FOLDER", "filedialogs");
  if (ngs::fs::environment_get_variable("IMGUI_CONFIG_FAVS").empty())
    ngs::fs::environment_set_variable("IMGUI_CONFIG_FAVS", "filedialogs-favs.txt");
  if (ngs::fs::environment_get_variable("IMGUI_CONFIG_INI").empty())
    ngs::fs::environment_set_variable("IMGUI_CONFIG_INI", "filedialogs-conf.ini");
  #if defined(_WIN32)
  std::string config_path = ngs::fs::environment_expand_variables("${IMGUI_CONFIG_HOME}\\.config\\${IMGUI_CONFIG_FOLDER}");
  if (!config_path.empty()) {
    if (!ngs::fs::directory_exists(config_path)) ngs::fs::directory_create(config_path);
    ini = ngs::fs::environment_expand_variables(config_path + "\\${IMGUI_CONFIG_INI}");
    int attr = GetFileAttributesW(ghc::filesystem::path(config_path).wstring().c_str());
    if ((attr & FILE_ATTRIBUTE_HIDDEN) == 0) SetFileAttributesW(ghc::filesystem::path(config_path).wstring().c_str(), attr | FILE_ATTRIBUTE_HIDDEN);
  }
  #else
  std::string config_path = ngs::fs::environment_expand_variables("${IMGUI_CONFIG_HOME}/.config/${IMGUI_CONFIG_FOLDER}");
  if (!config_path.empty()) {
    if (!ngs::fs::directory_exists(config_path)) ngs::fs::directory_create(config_path);
    ini = ngs::fs::environment_expand_variables(config_path + "/${IMGUI_CONFIG_INI}");
  }
  #endif
  io.IniFilename = ((!ini.empty()) ? ini.c_str() : nullptr);
  
  ImFontConfig config;
  config.MergeMode = true; 
  ImWchar ranges[] = { 0x0020, 0xFFFF, 0 };
  std::vector<std::string> fonts;
  std::string font = ngs::fs::directory_contents_first(ngs::fs::executable_get_directory() + "fonts", "*.ttf;*.otf", false, false);
  if (!font.empty()) {
    fonts.push_back(font);
    while (!font.empty()) {
      font = ngs::fs::directory_contents_next();
      fonts.push_back(font);
    }
  }
  ngs::fs::directory_contents_close();
  for (unsigned i = 0; i < fonts.size(); i++)
    if (ngs::fs::file_exists(fonts[i]))
      io.Fonts->AddFontFromFileTTF(fonts[i].c_str(), 20, (!i) ? nullptr : &config, ranges);

  ImGui::StyleColorsLight();
  //ImGui::StyleColorsDark();
  //ImGui::StyleColorsClassic();
  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowBorderSize = 0.0;
  style.WindowPadding = ImVec2(15, 15);
  style.WindowRounding = 0.0f;
  style.FramePadding = ImVec2(5, 5);
  style.FrameRounding = 4.0f;
  style.ItemSpacing = ImVec2(12, 8);
  style.ItemInnerSpacing = ImVec2(8, 6);
  style.IndentSpacing = 25.0f;
  style.ScrollbarSize = 15.0f;
  style.ScrollbarRounding = 9.0f;
  style.GrabMinSize = 5.0f;
  style.GrabRounding = 3.0f;
  /*style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  style.Colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };
  style.Colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
  style.Colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
  style.Colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
  style.Colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
  style.Colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
  style.Colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
  style.Colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
  style.Colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
  style.Colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
  style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
  style.Colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
  style.Colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };*/
  ImGui_ImplSDL2_InitForOpenGL(wnd, glContext);
  ImGui_ImplOpenGL3_Init();

  // ImFileDialog requires you to set the CreateTexture and DeleteTexture
  ifd::FileDialog::Instance().CreateTexture = [](uint8_t *data, int w, int h, char fmt) -> void * {
    GLuint tex;

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, (fmt == 0) ? GL_BGRA : GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    return (void *)(uintptr_t)tex;
  };
  ifd::FileDialog::Instance().DeleteTexture = [](void *tex) {
    GLuint texID = (GLuint)(uintptr_t)tex;
    glDeleteTextures(1, &texID);
  };

  SDL_Event event;
  while (run) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT)
        run = false;
      else if (event.type == SDL_WINDOWEVENT) {
        if (event.window.event == SDL_WINDOWEVENT_MOVED || event.window.event == SDL_WINDOWEVENT_MAXIMIZED || event.window.event == SDL_WINDOWEVENT_RESIZED) {
          Uint32 wndFlags = SDL_GetWindowFlags(wnd);
          SDL_GetWindowSize(wnd, &wndWidth, &wndHeight);
        }
      }

      // let ImGui handle the events
      ImGui_ImplSDL2_ProcessEvent(&event);
    }

    if (!run) break;

    // Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Simple window
    ImGui::Begin("Control Panel");
    if (ImGui::Button("Open File"))
      ifd::FileDialog::Instance().Open("FileOpenDialog", "Open File", "PNG Image Files (*.png){.png},.*", false);
    if (ImGui::Button("Open Directory"))
      ifd::FileDialog::Instance().Open("DirectoryOpenDialog", "Open Directory", "");
    if (ImGui::Button("Save File"))
      ifd::FileDialog::Instance().Save("FileSaveDialog", "Save File", "PNG Image Files (*.png){.png},.*");
    ImGui::End();

    // file dialogs
    if (ifd::FileDialog::Instance().IsDone("FileOpenDialog")) {
      if (ifd::FileDialog::Instance().HasResult()) {
        const std::vector<ghc::filesystem::path>& res = ifd::FileDialog::Instance().GetResults();
        for (const auto& r : res) // FileOpenDialog supports multiselection
          printf("OPEN[%s]\n", r.u8string().c_str());
      }
      ifd::FileDialog::Instance().Close();
    }
    if (ifd::FileDialog::Instance().IsDone("DirectoryOpenDialog")) {
      if (ifd::FileDialog::Instance().HasResult()) {
        std::string res = ifd::FileDialog::Instance().GetResult().u8string();
        printf("DIRECTORY[%s]\n", res.c_str());
      }
      ifd::FileDialog::Instance().Close();
    }
    if (ifd::FileDialog::Instance().IsDone("FileSaveDialog")) {
      if (ifd::FileDialog::Instance().HasResult()) {
        std::string res = ifd::FileDialog::Instance().GetResult().u8string();
        printf("SAVE[%s]\n", res.c_str());
      }
      ifd::FileDialog::Instance().Close();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glViewport(0, 0, wndWidth, wndHeight);

    // render Dear ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(wnd);
  }

  // Dear ImGui cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  // SDL2 cleanup
  SDL_GL_DeleteContext(glContext);
  SDL_DestroyWindow(wnd);
  SDL_Quit();

  return 0;
}
