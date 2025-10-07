#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <functional>
#include <sstream>
#include <memory>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <map>

// Order matters here
#include "eng/core/base.h"
#include "eng/core/logger.h"
#include "eng/core/timer.h"

#ifdef ENG_FRONTEND
  #include "eng/runtime/application.h"
  #include "eng/renderer/renderer_api.h"
  #include "eng/ui/imgui_layer.h"
#endif

// #include <eng/renderer/helpers/webgpu_fmt_formatters.h>
