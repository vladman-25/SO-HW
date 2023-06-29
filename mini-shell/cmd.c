// SPDX-License-Identifier: BSD-3-Clause

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmd.h"
#include "utils.h"

#define READ		0
#define WRITE		1

/**
 * Internal change-directory command.
 */
static bool shell_cd(word_t *dir)
{
	if (dir == NULL) {
		return true;
	}
	if (chdir(dir->string) == -1) {
        return false;
    }
    return true;
}

/**
 * Internal exit/quit command.
 */
static int shell_exit(void)
{
	return SHELL_EXIT;
}


char* get_string(word_t* verb) {
	word_t* piece = verb;
	char *mema = calloc(1000,sizeof(char));
	while (piece) {
		if (piece->expand) {
			if (getenv(piece->string) != NULL) {
				strcat(mema, getenv(piece->string));
			}
		} else {
			strcat(mema, piece->string);
		}
		piece = piece->next_part;
	}
	return mema;
}

int external_command(simple_command_t *s, int level, command_t *father) {
	pid_t pid = fork();

	if (pid < 0) { /* error occurred */
		return 1;

	} else if (pid == 0) { /* child process */
		char* command = get_string(s->verb);
		word_t* auxParams = s->params;
		char* argument_list[100];
		int idx = 1;
		int out_desc = -1;
		int out_ret = -1;
		int err_desc = -1;
		int err_ret = -1;
		int in_desc = -1;
		int in_ret = -1;

		argument_list[0] = command;
		if (s->in && (strcmp(get_string(s->verb), "echo") != 0)) {
			argument_list[idx] = get_string(s->in);
			idx++;
		}
		while(auxParams) {
			argument_list[idx] = get_string(auxParams);
			auxParams = auxParams->next_word;
			idx++;
		}
		if (s->in &&  (strcmp(get_string(s->verb), "echo") != 0)) { // ==
			in_desc = open(get_string(s->in), O_RDONLY | O_CREAT, 0644);
			in_ret = dup2(in_desc, 0);
		}
		if (s->out) {
			out_desc = open(get_string(s->out), O_WRONLY | O_CREAT | (s->io_flags == 0 ? O_TRUNC : O_APPEND), 0644);
			out_ret = dup2(out_desc, 1);
		}
		if (s->err) {
			if (s->out == NULL) {
				err_desc = open(get_string(s->err), O_WRONLY | O_CREAT | (s->io_flags == 0 ? O_TRUNC : O_APPEND), 0644);
				err_ret = dup2(err_desc, 2);
			} else {
				if (strcmp(get_string(s->err),get_string(s->out)) != 0) {
					err_desc = open(get_string(s->err), O_WRONLY | O_CREAT | (s->io_flags == 0 ? O_TRUNC : O_APPEND), 0644);
					err_ret = dup2(err_desc, 2);
				} else {
					err_ret = dup2(out_desc, 2);
				}
			}
		}
		argument_list[idx] = NULL;
		int status_code = execvp(command, argument_list);

		close(in_ret);
		close(out_ret);
		close(err_ret);
		exit(status_code);
	} else { /* parent process */

		int wstatus;
		waitpid(pid, &wstatus, 0);
		int stat = WEXITSTATUS(wstatus);

		if(stat != 0) {
			fprintf(stderr,"Execution failed for '%s'\n",s->verb->string);
		}
		return stat;
	}
}

int cd_command(simple_command_t *s, int level, command_t *father) {
	int out_desc = -1;
	int out_ret = -1;
	int err_desc = -1;
	int err_ret = -1;
	int in_desc = -1;
	int in_ret = -1;

	if (s->out) {
		out_desc = open(s->out->string, O_WRONLY | O_CREAT | (s->io_flags == 0 ? O_TRUNC : O_APPEND), 0644);
		out_ret = dup2(out_desc, 1);
	}
	if (s->err) {
		if (s->out == NULL) {
			err_desc = open(s->err->string, O_WRONLY | O_CREAT | (s->io_flags == 0 ? O_TRUNC : O_APPEND), 0644);
			err_ret = dup2(err_desc, 2);
		} else {
			if (strcmp(s->err->string, s->out->string) != 0) {
				err_desc = open(s->err->string, O_WRONLY | O_CREAT | (s->io_flags == 0 ? O_TRUNC : O_APPEND), 0644);
				err_ret = dup2(err_desc, 2);
			} else {
				err_ret = dup2(out_desc, 2);
			}
		}
	}
	bool ret = shell_cd(s->params);

	if (ret == false) {
		printf("No such file or directory\n");
	}
	close(in_ret);
	close(out_ret);
	close(err_ret);
	return 1 - ret;
}
/**
 * Parse a simple command (internal, environment variable assignment,
 * external command).
 */


int set_var(simple_command_t *s) {

	word_t* piece = s->verb->next_part->next_part;
	char *mema = calloc(1000, sizeof(char));
	while (piece) {
		if (piece->expand) {
			strcat(mema, getenv(piece->string));
		} else {
			strcat(mema, piece->string);
		}
		piece = piece->next_part;
	}
	return setenv(s->verb->string, mema, 1);
}

static int parse_simple(simple_command_t *s, int level, command_t *father)
{
	int exit_code = 0;

	if (!strcmp(s->verb->string, "cd")) {
		exit_code = cd_command(s, level, father);
	} else if (!strcmp(s->verb->string, "exit") || !strcmp(s->verb->string, "quit")) {
		exit_code = shell_exit();
	} else if (s->verb->next_part && !strcmp(s->verb->next_part->string, "=")) {
		exit_code = set_var(s);
	} else {
		exit_code = external_command(s, level, father);
	}
	return exit_code;
}

/**
 * Process two commands in parallel, by creating two children.
 */
static bool run_in_parallel(command_t *cmd1, command_t *cmd2, int level,
		command_t *father)
{
	pid_t pid = fork();
	int ret = 1;
	if (pid < 0) { /* error occurred */
		return 1;
	} else if (pid == 0) { /* child process */
		int ret = parse_command(cmd1, level, father);
		exit(ret);
	} else {
		
		pid_t pid2 = fork();
		if (pid2 < 0) {
			return 1;
		} else if (pid2 == 0) {
			int ret = parse_command(cmd2, level, father);
			exit(ret);
		} else {
			int wstatus1;
			waitpid(pid, &wstatus1, 0);
			int wstatus2;
			waitpid(pid2, &wstatus2, 0);
			return 0;
		}
	}
	return ret;
}

/**
 * Run commands by creating an anonymous pipe (cmd1 | cmd2).
 */
static bool run_on_pipe(command_t *cmd1, command_t *cmd2, int level,
		command_t *father)
{
	int pipefd[2];

	if (pipe(pipefd) == -1) { /* create pipe */
		perror("pipe");
		return 1;
	}

	pid_t pid = fork();
	if (pid < 0) { /* error occurred */
		return 1;
	} else if (pid == 0) { /* child process */
		close(pipefd[0]);

		if (dup2(pipefd[1], IO_OUT_APPEND) == -1) {
			perror("dup2");
			return false;
		}
		int ret = parse_command(cmd1, level, father);

		exit(ret);
	} else {
		close(pipefd[1]); /* close unused write end of the pipe */
		int old_io = dup(IO_REGULAR);
		if (dup2(pipefd[0], IO_REGULAR) == -1) {
			perror("dup2");
			return false;
		}
		int ret = parse_command(cmd2, level, father);

		dup2(old_io,IO_REGULAR);
		return ret;
	}

	return 0;
}

/**
 * Parse and execute a command.
 */
int parse_command(command_t *c, int level, command_t *father)
{
	/* TODO: sanity checks */

	if (c->op == OP_NONE) {
		return parse_simple(c->scmd, level, father);
	}

	switch (c->op) {
	case OP_SEQUENTIAL: ;
		parse_command(c->cmd1, level, father);
		return parse_command(c->cmd2, level, father);
		break;

	case OP_PARALLEL: ;
		return run_in_parallel(c->cmd1, c->cmd2, level, father);
		break;

	case OP_CONDITIONAL_NZERO: ;
		int ret1 = parse_command(c->cmd1, level, father);
		int ret2 = 0;

		if (ret1 != 0) {
			ret2 = parse_command(c->cmd2, level, father);
			return ret2;
		} else {
			return ret1;
		}
		break;

	case OP_CONDITIONAL_ZERO: ;
		int ret1z = parse_command(c->cmd1, level, father);
		int ret2z = 0;

		if (ret1z == 0) {
			ret2z = parse_command(c->cmd2, level, father);
			return ret2z;
		} else {
			return ret1z;
		}
		break;

	case OP_PIPE: ;
		return run_on_pipe(c->cmd1, c->cmd2, level, father);
		break;

	default:
		return SHELL_EXIT;
	}

	return 0;
}
