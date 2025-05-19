#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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

struct Score {
    char username[32];
    int total;
};

typedef struct Score Score;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "[Error] Usage: %s <hunt_name>\n", argv[0]);
        return 1;
    }

    char path[256];
    snprintf(path, sizeof(path), "%s/%s", argv[1], TREASURE_FILE);

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        printf("[Error] Could not open treasure file '%s'.\n", path);
        return 1;
    }

    Treasure t;
    Score scores[100];
    int count = 0;
    memset(scores, 0, sizeof(scores));

    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        if (strlen(t.username) == 0) continue;

        int found = 0;
        for (int i = 0; i < count; ++i) {
            if (strcmp(scores[i].username, t.username) == 0) {
                scores[i].total += t.value;
                found = 1;
                break;
            }
        }

        if (!found) {
            strncpy(scores[count].username, t.username, sizeof(scores[count].username) - 1);
            scores[count].total = t.value;
            count++;
        }
    }

    close(fd);

    if (count == 0) {
        printf("[Info] No scores to display for '%s'.\n", argv[1]);
    } else {
        for (int i = 0; i < count; ++i) {
            printf("User: %s, Score: %d\n", scores[i].username, scores[i].total);
        }
    }

    fflush(stdout);
    return 0;
}
