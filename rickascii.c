#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

void cleanup() {
	puts("\x1b[2J\x1b[0H\x1b[?25h\x1b[u\x1b[m");

#ifdef DEBUG
	system("killall aplay afplay 2>/dev/null");
	exit(0);
#endif
}

int main(int argc, char *argv[]) {
#ifdef DEBUG
	system("afplay rick.wav &");
	signal(SIGINT, cleanup);
#else
	system("(/usr/bin/bzcat /rick/rick.wav.bz2 | /usr/local/bin/aplay -q -r 8000) &");
	signal(SIGINT, SIG_IGN);
#endif

	puts("\033[H\033[J");

	double timePerFrame = 1.0 / (double)25.0;
	int frame = 0, nextFrame = 0, i = 0;
	time_t started = time(NULL);

	while (!feof(stdin)) {
		char line[1000];
		fgets(line, sizeof line, stdin);
		fputs(line, stdout);

		if (i % 32 == 0) {
			frame++;

			time_t elapsed = time(NULL) - started;
			double repose = (frame * timePerFrame) - elapsed;

			if (repose > 0.0) {
				usleep(repose * 100000);
			}

			nextFrame = elapsed / timePerFrame;
		}

		i++;
	}

	cleanup();
	return 0;
}
