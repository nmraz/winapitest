﻿#include "base/asio/file.h"
#include "base/asio/io_event_loop.h"
#include "base/command_line.h"
#include "base/event_loop/task_runner.h"
#include "base/event_loop/task_event_loop.h"
#include "base/logging/logging.h"
#include "base/logging/logging_sinks.h"
#include "base/timer.h"
#include "base/thread/thread.h"
#include "base/thread/thread_name.h"

namespace chrono = std::chrono;
using namespace std::literals;

using millis = chrono::duration<double, std::milli>;

int wmain(int argc, const wchar_t** argv) {
	base::command_line cmd_line(argc, argv);

	logging::init(std::make_unique<logging::stdout_sink>(), logging::level::trace, cmd_line.has_flag("logging-colorize"));
	base::set_current_thread_name("Main");

	base::timer timer1, timer2;
	base::thread io_thread([] { return std::make_unique<base::io_event_loop>(); }, "IO");

	chrono::steady_clock::time_point start_time;

	timer1.on_fire([&] {
		LOG(trace) << "timer1: elapsed time: "
			<< millis(chrono::steady_clock::now() - start_time).count() << "ms";
		base::task_runner::current().post_quit();
	});

	timer2.on_fire([&] {
		LOG(trace) << "timer2: elapsed time: "
			<< millis(chrono::steady_clock::now() - start_time).count() << "ms";
	});

	base::task_runner::current().post_task([&] {
		LOG(trace) << "started";
		start_time = chrono::steady_clock::now();

		timer1.set(5s);
		timer2.set(2s);

		io_thread.task_runner().post_task([] {
			auto file = std::make_shared<base::file>("test.txt", base::file::out | base::file::create_always);
			auto data = std::make_shared<std::string>(3000, 'h');

			file->write(0, *data, [file, data](unsigned long written, const std::error_code& err) {
				LOG(trace) << "wrote " << written << " bytes of data: " << err.message();
			});
		});
	});

	base::task_event_loop loop;
	loop.run();
}
