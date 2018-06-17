#include "base/asio/file.h"
#include "base/asio/io_event_loop.h"
#include "base/command_line.h"
#include "base/event_loop/loop_task_runner.h"
#include "base/event_loop/next_tick.h"
#include "base/logging/logging.h"
#include "base/logging/logging_sinks.h"
#include "base/task_runner/run_task.h"
#include "base/timer.h"
#include "base/thread/thread.h"
#include "base/thread/thread_name.h"
#include "base/win/scoped_co_init.h"
#include "base/win/win_event_loop.h"
#include "ui/gfx/animation/animation.h"
#include "ui/gfx/animation/easing.h"
#include "ui/gfx/geom/path.h"
#include <filesystem>

namespace chrono = std::chrono;
using namespace std::literals;

using millis = chrono::duration<double, std::milli>;

int wmain(int argc, const wchar_t** argv) {
  base::command_line cmd_line(argc, argv);

  base::win::scoped_co_init init_com;

  logging::init(std::make_unique<logging::stdout_sink>(), logging::level::trace, cmd_line.has_flag("logging-colorize"));
  
  base::loop_task_runner::init_for_this_thread();
  base::set_current_thread_name("Main");

  base::timer timer1, timer2;
  base::thread io_thread([] { return std::make_unique<base::io_event_loop>(); }, "IO");

  chrono::steady_clock::time_point start_time;

  timer1.set_callback([&] {
    LOG(info) << "timer1: elapsed time: "
      << millis(chrono::steady_clock::now() - start_time).count() << "ms";
    base::loop_task_runner::current()->post_quit();
  });

  timer2.set_callback([&] {
    LOG(info) << "timer2: elapsed time: "
      << millis(chrono::steady_clock::now() - start_time).count() << "ms";
  });

  base::next_tick([&] {
    LOG(info) << "started";
    start_time = chrono::steady_clock::now();

    timer1.set(5s);
    timer2.set(2s);

    base::run_task(*io_thread.task_runner(), [] {
      LOG(info) << "Opening and writing to file";

      auto file_holder = std::make_unique<base::file>(
        std::filesystem::u8path("test.txt"),
        base::file::out,
        base::file::create_disp::create_always
      );
      base::file* file = file_holder.get();

      return file->write(0, "Test file!").then([file_holder = std::move(file_holder)](auto&& result) {
        LOG(info) << "Closing file";
        file_holder->close();
        return result;
      });
    }).then([](base::expected<unsigned long> result) {
      try {
        unsigned long bytes_written = result.get();
        LOG(info) << "Wrote " << bytes_written << " bytes of data";
      } catch (const std::exception& e) {
        LOG(error) << "Failed to write to file: " << e.what();
      }
    });

    ::MessageBoxW(nullptr, L"This is a message box", L"Message Box", MB_OK);
  });


  gfx::path p;
  p.move_to({ 1, 2 });
  p.line_to({ 3, 2 });
  p.line_to({ 3, 4 });
  p.line_to({ 1, 4 });
  LOG(info) << "Path area: " << p.area();
  p.quad_to({ 2, 3 }, { 1, 2 });
  LOG(info) << "Path area: " << p.area();


  gfx::animation<gfx::pointf> anim(gfx::easing::ease_in, [](gfx::animation<gfx::pointf>& anim) {
    gfx::pointf val = anim.val();
    LOG(trace) << "(" << val.x() << ", " << val.y() << ")";

    if (!anim.is_running()) {
      LOG(info) << "Animation complete";
    }
  });

  anim.set({ 1, 2 });
  anim.set_duration(500ms);
  anim.animate_to({ 3, 4 });


  base::win::win_event_loop loop;
  loop.run();
}
