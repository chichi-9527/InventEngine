#include "IEpch.h"
#include "IUIImgui.h"


#include "imgui_impl_glfw.h"

#ifdef USE_OPENGL
#include "imgui_impl_opengl3.h"
#endif // #ifdef USE_OPENGL

namespace INVENT
{
	void IUIImgui::Init(GLFWwindow* window)
	{
		IMGUI_CHECKVERSION();
		auto imgui_context = ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
#ifdef USE_OPENGL
		ImGui_ImplOpenGL3_Init();
#endif // #ifdef USE_OPENGL
	}

	void IUIImgui::End()
	{
#ifdef USE_OPENGL
		ImGui_ImplOpenGL3_Shutdown();
#endif // #ifdef USE_OPENGL
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void IUIImgui::StartFrame()
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//ImGui::ShowDemoWindow(); // Show demo window! :)
	}

	void IUIImgui::Render()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}