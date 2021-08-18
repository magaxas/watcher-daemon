#include "main.h"

void daemonize()
{
	pid_t pid = 0;
	pid_t sid = 0;

	//Create child process
	pid = fork();

	//Fork failure
	if (pid < 0)
	{
		printf("Fork failed!\n");
		exit(1);
	}

	//Terminate parent process
	if (pid > 0)
	{
		printf("PID of child process: %d\n", pid);
		exit(0);
	}

	//Set new session
	sid = setsid();
	printf("SID: %d\n", sid);
	if (sid < 0)
	{
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
}
