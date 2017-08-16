#include "base/asio/file.h"
#include "base/asio/io_event_loop.h"
#include "base/command_line.h"
#include "base/event_loop/task_runner.h"
#include "base/event_loop/next_tick.h"
#include "base/logging/logging.h"
#include "base/logging/logging_sinks.h"
#include "base/timer.h"
#include "base/thread/thread.h"
#include "base/thread/thread_name.h"
#include "base/win/scoped_co_init.h"
#include "ui/gfx/animation/animation.h"
#include "ui/gfx/animation/easing.h"
#include "ui/gfx/path.h"
#include "ui/event_loop.h"

namespace chrono = std::chrono;
using namespace std::literals;

using millis = chrono::duration<double, std::milli>;

int wmain(int argc, const wchar_t** argv) {
	base::command_line cmd_line(argc, argv);

	base::win::scoped_co_init init_com;

	logging::init(std::make_unique<logging::stdout_sink>(), logging::level::trace, cmd_line.has_flag("logging-colorize"));
	base::set_current_thread_name("Main");

	base::timer timer1, timer2;
	base::thread io_thread([] { return std::make_unique<base::io_event_loop>(); }, "IO");

	chrono::steady_clock::time_point start_time;

	timer1.on_fire([&] {
		LOG(info) << "timer1: elapsed time: "
			<< millis(chrono::steady_clock::now() - start_time).count() << "ms";
		base::task_runner::current().post_quit();
	});

	timer2.on_fire([&] {
		LOG(info) << "timer2: elapsed time: "
			<< millis(chrono::steady_clock::now() - start_time).count() << "ms";
	});

	base::next_tick([&] {
		LOG(info) << "started";
		start_time = chrono::steady_clock::now();

		timer1.set(5s);
		timer2.set(2s);

		io_thread.task_runner().post_task([] {
			auto file = std::make_shared<base::file>("test.txt", base::file::out | base::file::create_always);
			auto data = std::make_shared<std::string>(3000, 'h');

			file->write(0, *data, [file, data] (const std::error_code& err, unsigned long written) {
				LOG(info) << "wrote " << written << " bytes of data: " << err.message();
			});
		});

		::MessageBoxW(nullptr, L"This is a message box", L"Message Box", MB_OK);
	});


	gfx::path p;
	p.move_to({ 1, 2 });
	p.line_to({ 3, 2 });
	p.line_to({ 3, 4 });
	p.line_to({ 1, 4 });
	p.quad_to({ 2, 3 }, { 1, 2 });
	LOG(info) << "Path area: " << p.area();


	gfx::animation anim(gfx::easing::ease_in);

	anim.set_callback([&](double value, bool done) {
		LOG(trace) << value;

		if (done) {
			LOG(info) << "Done";
		}
	});
	anim.set_duration(500ms);
	anim.enter();

	base::set_timeout([&]() {
		LOG(info) << "Leaving";
		anim.leave();
	}, 300ms);

	base::set_timeout([&]() {
		LOG(info) << "Entering";
		anim.enter();
	}, 400ms);


	ui::event_loop loop;
	loop.run();
}
