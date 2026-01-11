#ifndef _INVENT_ENGINE_PCH_
#define _INVENT_ENGINE_PCH_


#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "Windows.h"
#endif // _WIN32

#include <boost/asio.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sinks/async_frontend.hpp>
#include <boost/log/sinks/sync_frontend.hpp>


#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <cstring>
#include <format>

#include <iostream>
#include <fstream>
#include <memory>
#include <mutex>
#include <functional>
#include <thread>

#include <math.h>

#include "ILog.h"

#ifdef USE_OPENGL

#include "glad/glad.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#endif // USE_OPENGL

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#if defined(_WIN32) && defined(USE_OPENGL)
#include "wglext.h"
#endif


#endif // !_INVENT_ENGINE_PCH_
