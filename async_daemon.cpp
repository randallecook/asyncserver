#include <signal.h>
#include <stdarg.h>
#include <unistd.h>
#include <thread>
#include <sys/time.h>
#include "AsyncDaemon.h"

using namespace std;

static bool receiveRequests(const char* addr, const char* port);

int main(int argc, char** argv)
{
	bool success = receiveRequests("0.0.0.0", "8787");

	return success ? 0 : 1;
}

static bool receiveRequests(const char* addr, const char* port)
{
	bool success = true;

	// prepare for termination signals
	sigset_t signals;
	sigemptyset(&signals);
	sigaddset(&signals, SIGHUP);
	sigaddset(&signals, SIGINT);
	sigaddset(&signals, SIGQUIT);
	sigaddset(&signals, SIGTERM);
	struct timespec timeout_spec = { 0, 100000000 };  // 0.1 seconds

	// we must block the signals so that sigtimedwait can catch them
	pthread_sigmask(SIG_BLOCK, &signals, 0);

	// create our main daemon object
	AsyncDaemon ad(addr, port);

	// launch the receive thread
	thread receive_loop(&AsyncDaemon::receiveLoop, &ad);

	while (true)
	{
		int result = sigtimedwait(&signals, 0, &timeout_spec);

		if (result == -1 && errno == EAGAIN)
		{
			// simple sigwait timeout: if no errors, just continue
			if (ad.isRunningNormally())
			{
				continue;
			}
			else
			{
				logMsg("trouble elsewhere, exiting signal loop");
				success = false;
				break;
			}
		}
		else if (result > 0)
		{
			logMsg("terminating with signal %d", result);
			ad.initiateShutdown();
			break;
		}
		else
		{
			logMsg("trouble waiting for a signal (%d, %d, %s)", result, errno, strerror(errno));
			ad.initiateShutdown();
			success = false;
			break;
		}
	}

	// wait for the receive thread to stop
	receive_loop.join();

	return success;
}

void logMsg(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	struct timeval t = { 0 };
	gettimeofday(&t, 0);
	double now = t.tv_sec + t.tv_usec / 1000000.0;

	char msg_format[1024];
	snprintf(msg_format, sizeof(msg_format), "%.6f: %s\n", now, format);

	vprintf(msg_format, args);	
	
	va_end(args);
}


