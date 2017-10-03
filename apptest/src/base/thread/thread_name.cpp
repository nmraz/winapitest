#include "thread_name.h"

#include "base/unicode.h"
#include "base/win/last_error.h"
#include <map>
#include <mutex>
#include <utility>
#include <Windows.h>

namespace base {
namespace {

// Adapted from https://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx

constexpr DWORD thread_name_exception = 0x406D1388;

#pragma pack(push, 8)
typedef struct tagTHREADNAME_INFO {
  DWORD dwType; // Must be 0x1000.  
  LPCSTR szName; // Pointer to name (in user addr space).  
  DWORD dwThreadID; // Thread ID (-1=caller thread).  
  DWORD dwFlags; // Reserved for future use, must be zero.  
} THREADNAME_INFO;
#pragma pack(pop)  

void set_debugger_thread_name(std::string_view name) {
  THREADNAME_INFO info;
  info.dwType = 0x1000;
  info.szName = name.data();
  info.dwThreadID = static_cast<DWORD>(-1);
  info.dwFlags = 0;

#pragma warning(push)
#pragma warning(disable: 6320 6322) // constant EXCEPTION_EXECUTE_HANDLER, empty __except block
  __try {
    ::RaiseException(thread_name_exception, 0, sizeof(info) / sizeof(ULONG_PTR), reinterpret_cast<ULONG_PTR*>(&info));
  } __except (EXCEPTION_EXECUTE_HANDLER) {}
#pragma warning(pop)
}


using set_thread_desc_func = HRESULT(*)(HANDLE, PCWSTR);

void set_windows_thread_name(std::string_view name) {
  static auto* set_thread_desc =
    reinterpret_cast<set_thread_desc_func>(::GetProcAddress(::GetModuleHandleW(L"Kernel32.dll"), "SetThreadDescription"));

  // SetThreadDescription is new in win 10 v1703 (Creators Update)
  if (set_thread_desc) {
    win::throw_if_failed(
      set_thread_desc(::GetCurrentThread(), widen(name).c_str()),
      "Failed to set thread name"
    );
  } else if (::IsDebuggerPresent()) {  // fall back to older method (exception)
    set_debugger_thread_name(name);
  }
}


std::map<std::thread::id, std::string> thread_name_map;
std::mutex thread_name_lock;

}  // namespace

void set_current_thread_name(std::string name) {
  set_windows_thread_name(name.c_str());

  std::lock_guard<std::mutex> hold(thread_name_lock);
  thread_name_map[std::this_thread::get_id()] = std::move(name);
}

void cleanup_current_thread_name() {
  std::lock_guard<std::mutex> hold(thread_name_lock);

  auto it = thread_name_map.find(std::this_thread::get_id());
  if (it != thread_name_map.end()) {
    thread_name_map.erase(it);
  }
}


const std::string& get_thread_name(std::thread::id id) {
  std::lock_guard<std::mutex> hold(thread_name_lock);
  return thread_name_map[id];
}

const std::string& get_current_thread_name() {
  return get_thread_name(std::this_thread::get_id());
}

}  // namespace base