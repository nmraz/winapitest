#include "base/eventLoop/TaskRunner.h"
#include "base/eventLoop/TaskEventLoop.h"
#include "base/logging/logging.h"
#include "base/logging/loggingSinks.h"
#include "base/Timer.h"

namespace chrono = std::chrono;
using namespace std::literals;

using Millis = chrono::duration<double, std::milli>;

int main() {
	logging::init(std::make_unique<logging::StdoutSink>(), logging::Level::trace, true);

	base::TaskRunner runner;
	base::TaskEventLoop loop;
	
	base::Timer timer1, timer2;

	chrono::time_point<chrono::steady_clock> startTime;

	timer1.onFire([&] {
		LOG(trace) << "timer1: elapsed time: "
			<< chrono::duration_cast<Millis>(chrono::steady_clock::now() - startTime).count() << "ms";
		runner.postQuit();
	});

	timer2.onFire([&] {
		LOG(trace) << "timer2: elapsed time: "
			<< chrono::duration_cast<Millis>(chrono::steady_clock::now() - startTime).count() << "ms";
	});

	runner.postTask([&] {
		LOG(trace) << "started";
		startTime = chrono::steady_clock::now();

		timer1.set(5s);
		timer2.set(2s);
	});

	loop.run();
}
