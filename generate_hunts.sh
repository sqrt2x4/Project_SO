#!/bin/bash

gcc -o writer -x c - <<'EOF'
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct Treasure {
    char id[32];
    char username[32];
    float lat, lon;
    char clue[64];
    int value;
};

int main(int argc, char *argv[]) {
    FILE *f = fopen(argv[1], "wb");
    if (!f) return 1;
    for (int i = 0; i < 10; ++i) {
        struct Treasure t;
        snprintf(t.id, 32, "TID%02d", i);
        snprintf(t.username, 32, "user%d", i % 3);
        t.lat = 1.11 * i;
        t.lon = 2.22 * i;
        snprintf(t.clue, 64, "Clue %d", i);
        t.value = i + 1;
        fwrite(&t, sizeof(t), 1, f);
    }
    fclose(f);
    return 0;
}
EOF

for i in {1..40}; do
    dir="hunt_$i"
    mkdir -p "$dir"
    if (( i % 5 == 0 )); then
        # corrupted or empty
        touch "$dir/treasures.dat"
    else
        ./writer "$dir/treasures.dat"
    fi
done

rm writer
