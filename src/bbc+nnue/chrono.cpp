#include <io.h>
#include <cerrno>
#include <fcntl.h>

#include "chrono.h"
#include "pragma.h"

int is_quit = 0;
int moves_to_go = 30;
long move_time = -1;
long base_time = -1;
long increment = 0;
long start_time = 0;
long stop_time = 0;
int is_time_set = 0;
int is_stopped = 0;

long get_time_ms() {
#ifdef _WIN64
	return static_cast<long>(GetTickCount64());
#else
	struct timeval timeValue;
	gettimeofday(&timeValue, NULL);
	return timeValue.tv_sec * 1000 + timeValue.tv_usec / 1000;
#endif
}

int input_waiting() {
#ifndef WIN32
	fd_set readfds;
	struct timeval tv;
	FD_ZERO(&readfds);
	FD_SET(fileno(stdin), &readfds);
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	select(16, &readfds, 0, 0, &tv);

	return (FD_ISSET(fileno(stdin), &readfds));
#else
	static int pipe;
	static HANDLE inh;
	DWORD dw;

	if (static int init = 0; !init) {
		init = 1;
		inh = GetStdHandle(STD_INPUT_HANDLE);
		pipe = !GetConsoleMode(inh, &dw);
		if (!pipe) {
			SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
			FlushConsoleInputBuffer(inh);
		}
	}

	if (pipe) {
		if (!PeekNamedPipe(inh, nullptr, 0, nullptr, &dw, nullptr))
			return 1;
		return static_cast<int>(dw);
	}
	GetNumberOfConsoleInputEvents(inh, &dw);
	return dw <= 1 ? 0 : static_cast<int>(dw);

#endif
}

void read_input() {
	char input[256] = "";

	if (input_waiting()) {
		int bytes;
		is_stopped = 1;

		do {
			bytes = _read(_fileno(stdin), input, 256);
		}

		while (bytes < 0);

		if (char* endc = strchr(input, '\n'))
			*endc = 0;

		if (strlen(input) > 0) {
			if (!strncmp(input, "quit", 4))
				is_quit = 1;

			else if (!strncmp(input, "stop", 4))
				is_quit = 1;
		}
	}
}

void reset_time_control() {
	is_quit = 0;
	moves_to_go = 30;
	move_time = -1;
	base_time = -1;
	increment = 0;
	start_time = 0;
	stop_time = 0;
	is_time_set = 0;
	is_stopped = 0;
}