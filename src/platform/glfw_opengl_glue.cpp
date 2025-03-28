#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot/implot.h"
#include "implot/implot_internal.h"
#include "platform/glue.hpp"
#include <cstdio>
#include <math.h>
#ifdef _WIN32
#include <winsock.h>
#endif

#include <GLFW/glfw3.h> // Will drag system OpenGL headers
namespace Platform {
static void glfw_error_callback(int error, const char *description) {
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

struct GLFWPlatformData {
  GLFWwindow *window;
  ImVec4 clear_color;
  ImGuiIO &io;
};

Data init(std::string window_title) {
#ifdef _WIN32
  INT rc;
  WSADATA wsaData;

  rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (rc) {
    printf("WSAStartup Failed.\n");
    return nullptr;
  }
#endif
  // Setup window
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    return nullptr;
  }

  // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
  // GL ES 2.0 + GLSL 100
  const char *glsl_version = "#version 100";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
  // GL 3.2 + GLSL 150
  const char *glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac
#else
  // GL 3.0 + GLSL 130
  const char *glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+
  // only glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only
#endif

  // Create window with graphics context
  GLFWwindow *window =
      glfwCreateWindow(1280, 720, window_title.c_str(), NULL, NULL);
  if (window == NULL) {
    return nullptr;
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad
  // Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
  //   io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable
  //   Multi-Viewport /
  // Platform Windows
  // io.ConfigViewportsNoAutoMerge = true;
  // io.ConfigViewportsNoTaskBarIcon = true;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // When viewports are enabled we tweak WindowRounding/WindowBg so platform
  // windows can look identical to regular ones.
  ImGuiStyle &style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  return new GLFWPlatformData{window, {0, 0, 0, 1}, io};
}

void prerender(Data data) {
  // Poll and handle events (inputs, window resize, etc.)
  // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to
  // tell if dear imgui wants to use your inputs.
  // - When io.WantCaptureMouse is true, do not dispatch mouse input data to
  // your main application, or clear/overwrite your copy of the mouse data.
  // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input
  // data to your main application, or clear/overwrite your copy of the
  // keyboard data. Generally you may always pass all inputs to dear imgui,
  // and hide them from your application based on those two flags.
  glfwPollEvents();

  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_DockNodeHost;
  // ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(),
  //  ImGuiDockNodeFlags_PassthruCentralNode);
}
void postrender(Data pdata) {
  GLFWPlatformData *data = static_cast<GLFWPlatformData *>(pdata);

  // Rendering
  ImGui::Render();
  int display_w, display_h;
  glfwGetFramebufferSize(data->window, &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);
  glClearColor(data->clear_color.x * data->clear_color.w,
               data->clear_color.y * data->clear_color.w,
               data->clear_color.z * data->clear_color.w, data->clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  // Update and Render additional Platform Windows
  // (Platform functions may change the current OpenGL context, so we
  // save/restore it to make it easier to paste this code elsewhere.
  //  For this specific demo app we could also call
  //  glfwMakeContextCurrent(window) directly)
  if (data->io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    GLFWwindow *backup_current_context = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(backup_current_context);
  }

  glfwSwapBuffers(data->window);
}

void cleanup(Data pdata) {
  GLFWPlatformData *data = static_cast<GLFWPlatformData *>(pdata);
  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();

  glfwDestroyWindow(data->window);
  glfwTerminate();
  delete data;

#ifdef _WIN32
  WSACleanup();
#endif
}

bool shouldclose(Data pdata) {
  GLFWPlatformData *data = static_cast<GLFWPlatformData *>(pdata);

  return glfwWindowShouldClose(data->window);
}

} // namespace Platform
