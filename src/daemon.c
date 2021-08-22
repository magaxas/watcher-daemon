#include "main.h"

void daemonize(char *pid_file)
{
	pid_t pid = 0;
	pid_t sid = 0;

	//Create child process
	pid = fork();

	//Fork failure
	if (pid < 0) {
		printf("Fork failed!\n");
		exit(1);
	}

	//Terminate parent process
	if (pid > 0) {
		printf("PID of child process: %d\n", pid);
		exit(0);
	}

	//Set new session
	sid = setsid();
	printf("SID: %d\n", sid);
	if (sid < 0) {
		exit(1);
	}

	//Set file creation mode
	umask(0);

	//Change the current working directory to root.
	chdir("/");

	//Close stdin, stdout and stderr
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	//Write PID to file if specified
	if (pid_file != NULL) {
		if (unlink(pid_file) < 0) {
			perror("Failed to unlink pid lockfile");
		}
		char str[256] = "";
		int pid_fd = open(pid_file, O_RDWR | O_CREAT, 0640);

		if (pid_fd < 0 || lockf(pid_fd, F_TLOCK, 0) < 0) {
			exit(1);
		}
		sprintf(str, "%d\n", getpid());
		write(pid_fd, str, strlen(str));
	}
}
