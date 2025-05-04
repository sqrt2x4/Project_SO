#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ncurses.h>

int pipefd[2];
pid_t monitor_pid = -1;
int monitor_running = 0;

void send_command_to_monitor(const char *cmd) {
    if (monitor_pid <= 0 || !monitor_running) {
        printw("[Error] Monitor is not running.\n");
        refresh();
        return;
    }
    write(pipefd[1], cmd, strlen(cmd));
    kill(monitor_pid, SIGUSR1);
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
    printw("4. Stop Monitor\n");
    printw("5. Exit\n");
    printw("Choose an option: ");
    refresh();
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
                if (pipe(pipefd) == -1) {
                    perror("pipe");
                    endwin();
                    exit(1);
                }
                monitor_pid = fork();
                if (monitor_pid == 0) {
                    close(pipefd[1]);
                    dup2(pipefd[0], STDIN_FILENO);
                    execl("./monitor", "monitor", NULL);
                    perror("execl");
                    exit(1);
                } else {
                    close(pipefd[0]);
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

            case '4':
                if (!monitor_running) {
                    printw("[Error] No monitor running.\n");
                } else {
                    send_command_to_monitor("STOP");
                    usleep(500000);
                }
                break;

            case '5':
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
