#ifndef _INVENT_ENGINE_PCH_
#define _INVENT_ENGINE_PCH_



#ifdef USE_BOOST

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

#endif // USE_BOOST

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>

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
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#endif // USE_OPENGL

#ifdef INVENT_USE_WINDOWS
#include "Windows.h"
#endif // INVENT_USE_WINDOWS


#endif // !_INVENT_ENGINE_PCH_
