#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define ROW 10
#define COL 10
int main() {
    srand(time(NULL));

    int tmp;
    FILE *f = fopen("in19.txt", "wt");
    fprintf(f, "%d %d\n", ROW, COL);
    for (unsigned int i = 0; i < ROW; i++) {
        for (unsigned int j = 0; j < COL; j++) {

            tmp = 1+rand()%6;
            fprintf(f, "%d ", tmp);
        }
        fprintf(f, "\n");
    }
    fprintf(f, "1 1");
    fclose(f);
    return 0;
}
