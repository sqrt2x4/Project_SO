#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAX_USERNAME 32
#define MAX_CLUE     256

typedef struct {
    int id;
    char username[MAX_USERNAME];
    double latitude;
    double longitude;
    char clue[MAX_CLUE];
    int value;
} Treasure;


void add_treasure(const char *hunt_id) {
    Treasure t;

    printf("Enter ID: ");
    scanf("%d", &t.id);
    printf("Enter username: ");
    getchar();
    fgets(t.username, MAX_USERNAME, stdin);
    t.username[strcspn(t.username, "\n")] = '\0';
    printf("Enter latitude and longitude: ");
    scanf("%lf %lf", &t.latitude, &t.longitude);
    printf("Enter clue: ");
    getchar();
    fgets(t.clue, MAX_CLUE, stdin);
    t.clue[strcspn(t.clue, "\n")] = '\0';
    printf("Enter value: ");
    scanf("%d", &t.value);

    char dir_path[64], file_path[128];
    snprintf(dir_path, sizeof(dir_path), "./%s", hunt_id);
    snprintf(file_path, sizeof(file_path), "./%s/treasures.dat", hunt_id);

    mkdir(dir_path, 0755);

    int fd = open(file_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        perror("open");
        return;
    }

    write(fd, &t, sizeof(Treasure));
    close(fd);

    printf("Treasure added.\n");
}


void remove_treasure(const char *hunt_id, int treasure_id) {
    Treasure t;
    char file_path[128], temp_path[128];
    snprintf(file_path, sizeof(file_path), "./%s/treasures.dat", hunt_id);
    snprintf(temp_path, sizeof(temp_path), "./%s/temp.dat", hunt_id);

    int fd = open(file_path, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return;
    }

    int temp_fd = open(temp_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (temp_fd < 0) {
        perror("open temp");
        close(fd);
        return;
    }

    int found = 0;
    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        if (t.id == treasure_id) {
            found = 1;
            continue;  // skip writing this treasure
        }
        write(temp_fd, &t, sizeof(Treasure));
    }

    close(fd);
    close(temp_fd);

    if (found) {
        remove(file_path);                 // delete old file
        rename(temp_path, file_path);      // replace with updated one
        printf("Treasure ID %d removed.\n", treasure_id);
    } else {
        remove(temp_path);                 // discard temp
        printf("Treasure ID %d not found.\n", treasure_id);
    }
}


void list_treasures(const char *hunt_id) {
    Treasure t;
    char file_path[128];
    snprintf(file_path, sizeof(file_path), "./%s/treasures.dat", hunt_id);

    int fd = open(file_path, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return;
    }

    printf("Treasure list for hunt %s:\n", hunt_id);
    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        printf("ID: %d, User: %s, Value: %d\n", t.id, t.username, t.value);
    }

    close(fd);
}

void view_treasure(const char *hunt_id, int treasure_id) {
    Treasure t;
    char file_path[128];
    snprintf(file_path, sizeof(file_path), "./%s/treasures.dat", hunt_id);

    int fd = open(file_path, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return;
    }

    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        if (t.id == treasure_id) {
            printf("Treasure Details:\n");
            printf("ID: %d\nUser: %s\nCoords: %.6f, %.6f\nClue: %s\nValue: %d\n",
                   t.id, t.username, t.latitude, t.longitude, t.clue, t.value);
            close(fd);
            return;
        }
    }

    printf("Treasure with ID %d not found.\n", treasure_id);
    close(fd);
}

void remove_hunt(const char *hunt_id) {
    char command[128];
    snprintf(command, sizeof(command), "rm -rf ./%s", hunt_id);
    int result = system(command);

    if (result == 0) {
        printf("Hunt '%s' removed successfully.\n", hunt_id);
    } else {
        printf("Failed to remove hunt '%s'.\n", hunt_id);
    }
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage:\n");
        printf("  %s add <hunt_id>\n", argv[0]);
        printf("  %s list <hunt_id>\n", argv[0]);
        printf("  %s view <hunt_id> <id>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "add") == 0) {
        add_treasure(argv[2]);
    } else if (strcmp(argv[1], "list") == 0) {
        list_treasures(argv[2]);
    } else if (strcmp(argv[1], "view") == 0 && argc == 4) {
        view_treasure(argv[2], atoi(argv[3]));
    } else if (strcmp(argv[1], "remove_treasure") == 0 && argc == 4) {
        remove_treasure(argv[2], atoi(argv[3]));
    } else if (strcmp(argv[1], "remove_hunt") == 0) {
        remove_hunt(argv[2]);
    } else {
        printf("Invalid command or arguments.\n");
    }

    return 0;
}
