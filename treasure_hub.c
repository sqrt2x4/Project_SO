#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ncurses.h>

int pipe_to_monitor[2];
int pipe_from_monitor[2];
pid_t monitor_pid = -1;
int monitor_running = 0;

void send_command_to_monitor(const char *cmd) {
    if (monitor_pid <= 0 || !monitor_running) {
        printw("[Error] Monitor is not running.\n");
        refresh();
        return;
    }
    write(pipe_to_monitor[1], cmd, strlen(cmd));
    kill(monitor_pid, SIGUSR1);

    // Wait for and display monitor response
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    read(pipe_from_monitor[0], buffer, sizeof(buffer));
    printw("%s", buffer);
    refresh();
}

void sigchld_handler() {
    int status;
    waitpid(monitor_pid, &status, 0);
    printw("[Info] Monitor terminated with status %d\n", WEXITSTATUS(status));
    refresh();
    monitor_running = 0;
    monitor_pid = -1;
}

void display_menu() {
    clear();
    printw("Treasure Hunt Hub Menu:\n");
    printw("1. Start Monitor\n");
    printw("2. List Hunts\n");
    printw("3. List Treasures (enter hunt name)\n");
    printw("4. Calculate Scores\n");
    printw("5. Stop Monitor\n");
    printw("6. Exit\n");
    printw("Choose an option: ");
    refresh();
}

void run_score_calculator(const char *hunt) {
    int fd[2];
    if (pipe(fd) == -1) {
        printw("[Error] Failed to create pipe for score calculator.\n");
        return;
    }

    pid_t pid = fork();
    if (pid == 0) {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        execl("./score_calculator", "score_calculator", hunt, NULL);
        perror("execl");
        exit(1);
    } else {
        close(fd[1]);
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        read(fd[0], buf, sizeof(buf));
        printw("%s\n", buf);
        refresh();
        close(fd[0]);
        waitpid(pid, NULL, 0);
    }
}

int main() {
    initscr();
    cbreak();
    noecho();

    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);

    char input[256];

    while (1) {
        display_menu();
        int ch = getch();
        echo();
        switch (ch) {
            case '1':
                if (monitor_running) {
                    printw("[Info] Monitor already running.\n");
                    break;
                }
                if (pipe(pipe_to_monitor) == -1 || pipe(pipe_from_monitor) == -1) {
                    perror("pipe");
                    endwin();
                    exit(1);
                }
                monitor_pid = fork();
                if (monitor_pid == 0) {
                    dup2(pipe_to_monitor[0], STDIN_FILENO);
                    dup2(pipe_from_monitor[1], STDOUT_FILENO);
                    close(pipe_to_monitor[1]);
                    close(pipe_from_monitor[0]);
                    execl("./monitor", "monitor", NULL);
                    perror("execl");
                    exit(1);
                } else {
                    close(pipe_to_monitor[0]);
                    close(pipe_from_monitor[1]);
                    monitor_running = 1;
                    printw("[Info] Monitor started with PID %d\n", monitor_pid);
                }
                break;

            case '2':
                send_command_to_monitor("LIST_HUNTS");
                break;

            case '3': {
                printw("Enter hunt name: ");
                refresh();
                echo();
                getnstr(input, sizeof(input));
                char cmd[300];
                snprintf(cmd, sizeof(cmd), "list_treasures %s", input);
                send_command_to_monitor(cmd);
                break;
            }

            case '4': {
                printw("Enter hunt name: ");
                refresh();
                echo();
                getnstr(input, sizeof(input));
                run_score_calculator(input);
                break;
            }

            case '5':
                if (!monitor_running) {
                    printw("[Error] No monitor running.\n");
                } else {
                    send_command_to_monitor("STOP");
                    usleep(500000);
                }
                break;

            case '6':
                if (monitor_running) {
                    printw("[Error] Stop the monitor first.\n");
                } else {
                    endwin();
                    return 0;
                }
                break;

            default:
                printw("[Error] Unknown option.\n");
                break;
        }
        printw("\nPress any key to continue...");
        getch();
    }
    endwin();
    return 0;
}