#include "base/CmdLine.h"
#include "base/eventLoop/TaskRunner.h"
#include "base/eventLoop/TaskEventLoop.h"
#include "base/logging/logging.h"
#include "base/logging/loggingSinks.h"
#include "base/Timer.h"

namespace chrono = std::chrono;
using namespace std::literals;

using Millis = chrono::duration<double, std::milli>;

int wmain(int argc, wchar_t** argv) {
	base::CmdLine cmdLine(argc, argv);

	logging::init(std::make_unique<logging::StdoutSink>(), logging::Level::trace, cmdLine.hasFlag("logging-colorize"));

	base::TaskRunner runner;
	base::TaskEventLoop loop;
	
	base::Timer timer1, timer2;

	chrono::steady_clock::time_point startTime;

	timer1.onFire([&] {
		LOG(trace) << "timer1: elapsed time: "
			<< Millis(chrono::steady_clock::now() - startTime).count() << "ms";
		runner.postQuit();
	});

	timer2.onFire([&] {
		LOG(trace) << "timer2: elapsed time: "
			<< Millis(chrono::steady_clock::now() - startTime).count() << "ms";
	});

	runner.postTask([&] {
		LOG(trace) << "started";
		startTime = chrono::steady_clock::now();

		timer1.set(5s);
		timer2.set(2s);
	});

	loop.run();
}
