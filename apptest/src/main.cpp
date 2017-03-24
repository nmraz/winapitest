#include "base/CmdLine.h"
#include "base/eventLoop/TaskRunner.h"
#include "base/eventLoop/TaskEventLoop.h"
#include "base/logging/logging.h"
#include "base/logging/loggingSinks.h"
#include "base/Timer.h"
#include "base/thread/Thread.h"
#include "base/thread/threadName.h"

namespace chrono = std::chrono;
using namespace std::literals;

using Millis = chrono::duration<double, std::milli>;

int wmain(int argc, wchar_t** argv) {
	base::CmdLine cmdLine(argc, argv);

	logging::init(std::make_unique<logging::StdoutSink>(), logging::Level::trace, cmdLine.hasFlag("logging-colorize"));
	base::setCurrentThreadName("MainThread");

	base::TaskRunner runner;
	base::TaskEventLoop loop;
	
	base::Timer timer1, timer2;
	base::Thread thr([] { return std::make_unique<base::TaskEventLoop>(); }, "WorkerThread");

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

		thr.taskRunner().postTaskAndThen([] {
			LOG(trace) << "sending value 5";
			return 5;
		}, [] (int n) {
			LOG(trace) << "recieved value " << n;
		});
	});

	loop.run();
}
