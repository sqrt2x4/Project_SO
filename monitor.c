#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>

#define TREASURE_FILE "treasures.dat"

struct Treasure {
    char id[32];
    char username[32];
    float lat, lon;
    char clue[64];
    int value;
};

typedef struct Treasure Treasure;

volatile sig_atomic_t got_signal = 0;

void handle_sigusr1(int sig) {
    got_signal = 1;
}

void list_all_hunts() {
    DIR *d = opendir(".");
    if (!d) return;

    struct dirent *entry;
    char output[1024] = "";

    while ((entry = readdir(d)) != NULL) {
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char path[128];
            snprintf(path, sizeof(path), "%s/%s", entry->d_name, TREASURE_FILE);
            int fd = open(path, O_RDONLY);
            if (fd < 0) continue;
            struct stat st;
            fstat(fd, &st);
            int count = st.st_size / sizeof(Treasure);
            char line[256];
            snprintf(line, sizeof(line), "Hunt: %s — %d treasures\n", entry->d_name, count);
            strcat(output, line);
            close(fd);
        }
    }
    closedir(d);
    write(STDOUT_FILENO, output, strlen(output));
}

void list_treasures(const char *hunt) {
    char path[128];
    snprintf(path, sizeof(path), "%s/%s", hunt, TREASURE_FILE);
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        char err[128];
        snprintf(err, sizeof(err), "[Error] Could not open treasure file for hunt '%s'\n", hunt);
        write(STDOUT_FILENO, err, strlen(err));
        return;
    }

    char output[2048] = "";
    Treasure t;
    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        char line[256];
        snprintf(line, sizeof(line), "ID: %s, User: %s, Lat: %.2f, Lon: %.2f, Clue: %s, Value: %d\n",
                 t.id, t.username, t.lat, t.lon, t.clue, t.value);
        strcat(output, line);
    }
    close(fd);
    write(STDOUT_FILENO, output, strlen(output));
}

int main() {
    struct sigaction sa;
    sa.sa_handler = handle_sigusr1;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &sa, NULL);

    char buffer[256];

    while (1) {
        pause();
        if (got_signal) {
            memset(buffer, 0, sizeof(buffer));
            read(STDIN_FILENO, buffer, sizeof(buffer));

            if (strcmp(buffer, "LIST_HUNTS") == 0) {
                list_all_hunts();
            } else if (strncmp(buffer, "list_treasures", 14) == 0) {
                char hunt[128];
                sscanf(buffer, "list_treasures %s", hunt);
                list_treasures(hunt);
            } else if (strcmp(buffer, "STOP") == 0) {
                usleep(1000000);
                const char *msg = "[Monitor] Stopping...\n";
                write(STDOUT_FILENO, msg, strlen(msg));
                break;
            } else {
                const char *unk = "[Monitor] Unknown command\n";
                write(STDOUT_FILENO, unk, strlen(unk));
            }
            got_signal = 0;
        }
    }
    return 0;
}
