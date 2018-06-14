/******************************************
 * Adam Bajguz
******************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>

#define FILE_ID "1"

#define MAKE_FILENAME(name) name FILE_ID ".txt"
#define INPUT_FILENAME MAKE_FILENAME("in")
#define OUTPUT_FILENAME "out.txt"
#define ANSWER_FILENAME MAKE_FILENAME("out")

unsigned long long int it = 0;

#define CELL_HEIGHT 5

#define MAP prison_info.prison_map
#define MAP_PTR prison_info->prison_map
#define PRISON_MAP(x, y) (MAP + (x) + (y) * COL)
#define PRISON_MAP_PTR(x, y) (MAP_PTR + (x) + (y) * COL_PTR)

#define ROW prison_info.row_num
#define ROW_PTR prison_info->row_num

#define COL prison_info.col_num
#define COL_PTR prison_info->col_num

#define VISITED prison_info.cell_visited
#define VISITED_PTR prison_info->cell_visited
#define CELL_VISITED_PTR(x, y) (VISITED_PTR + (x) + (y) * COL_PTR)

#define BANDIT_X prison_info.bandit_x
#define BANDIT_X_PTR prison_info->bandit_x

#define BANDIT_Y prison_info.bandit_y
#define BANDIT_Y_PTR prison_info->bandit_y

#define DEATH_LVL prison_info.death_level
#define DEATH_LVL_PTR prison_info->death_level

struct prison_data {
    unsigned short *prison_map;
    unsigned short row_num, col_num;
    unsigned char *cell_visited;
    unsigned short bandit_x, bandit_y;
    unsigned short death_level;
};

unsigned int calculate_water_volume(const struct prison_data *const prison_info, const short x, const short y);

void write_output_to_file(unsigned int water_volume);

unsigned int get_correct_answer_from_file();

int main() {
    clock_t exec_begin = clock();

    struct prison_data prison_info;

    unsigned int cells_num;

    unsigned int minimum = UINT_MAX, maximum = 0;

    struct first_five
    {
        unsigned int level, num;
    } F5 [6] = {0};

    //##################################################
    //              FILE READING - START
    //==================================================
    {
        FILE *in = fopen(INPUT_FILENAME, "rt");
        if (in) {
            fscanf(in, "%hu %hu", &ROW, &COL);
            cells_num = ROW * COL;
            MAP = malloc(sizeof(*MAP) * cells_num);

            for (unsigned int cell = 0, tmp; cell < cells_num; ++cell) {
                fscanf(in, "%hu", &tmp);
                *(MAP + cell) = tmp;

                if(cells_num > 1) {
                    if (tmp < minimum)
                        minimum = tmp;

                    if (tmp > maximum)
                        maximum = tmp;

                    if (!F5[5].level)
                        for (char i = 0; i < 6; ++i) {
                            if (F5[i].level == 0)
                                F5[i].level = tmp;

                            if (F5[i].level == tmp) {
                                ++F5[i].num;
                                break;
                            }
                        }
                }
            }

            fscanf(in, "%hu %hu", &BANDIT_X, &BANDIT_Y);
            fclose(in);
        } else {
            printf("Error reading from %s!", INPUT_FILENAME);
            return 1;
        }

        --BANDIT_X;
        --BANDIT_Y;

        VISITED = calloc(sizeof(*VISITED), cells_num);
        DEATH_LVL = *PRISON_MAP(BANDIT_X, BANDIT_Y) - 1;
    }
    //==================================================
    //              FILE READING - END
    //##################################################


    //##################################################
    //                ALGORITHM - START
    //==================================================
    clock_t exec_begin_no_data_load = clock();
    clock_t exec_end;

    unsigned int water_volume = 1;

    if(minimum == maximum){
        water_volume = cells_num;
        exec_end = clock();
        printf("Single level!\n");
    }
    else if (!F5[5].level && *PRISON_MAP(BANDIT_X, BANDIT_Y) == minimum && maximum - minimum <= 5)
    {
        water_volume = 0;

        for(char i=0; i<5; ++i)
            if(F5[i].level > DEATH_LVL)
                water_volume += (F5[i].level - DEATH_LVL)*F5[i].num;

        exec_end = clock();
        printf("Cells only on levels 1-5!\n");
    }
    else if (cells_num > 1)
    {
        water_volume = calculate_water_volume(&prison_info, BANDIT_X, BANDIT_Y);
        exec_end = clock();
        printf("Recursive algorithm was used!\n");
    }
    else {
        exec_end = clock();
        printf("1x1 prison!\n");
    }
    //==================================================
    //                ALGORITHM - END
    //##################################################


    write_output_to_file(water_volume);

    free(MAP);
    free(VISITED);


    double exec_time = ((double) (exec_end - exec_begin)) / CLOCKS_PER_SEC;
    double exec_time_no_load = ((double) (exec_end - exec_begin_no_data_load)) / CLOCKS_PER_SEC;

    printf("\n");
    printf("           OUTPUT: %u cubic meters of water flowed out.\n", water_volume);
    printf("  EXPECTED OUTPUT: %d cubic meters of water flowed out.\n", get_correct_answer_from_file());

    printf("\nPRISON INFORMATION:\n");
    printf("        Size: x=%u; y=%u\n", prison_info.col_num, prison_info.row_num);
    printf("   Min level: %u\n", minimum);
    printf("   Max level: %u\n", maximum);

    printf("\nSTATISTICS:\n");
    printf("   Execution time: %f seconds.\n", exec_time);
    printf("   Execution time without data loading: %f seconds.\n", exec_time_no_load);
    printf("   Number of called functions: %d.\n", it);

    return 0;
}

unsigned int calculate_water_volume(const struct prison_data *const prison_info, const short x, const short y) {
    ++it;

    *CELL_VISITED_PTR(x, y) = 1;

    // if the cell is below the 'death_level' -> water_volume = CELL HEIGHT
    // else if the 'death_level' is between floor and ceiling -> water_volume = ceiling - death_level

    unsigned int water_volume = (*PRISON_MAP_PTR(x, y) - DEATH_LVL_PTR > CELL_HEIGHT) ? CELL_HEIGHT : *PRISON_MAP_PTR(x, y) - DEATH_LVL_PTR;

    for (short x_diff = -1, new_x; x_diff <= 1; ++x_diff) {
        new_x = x + x_diff;

        for (short y_diff = -1, new_y; y_diff <= 1; ++y_diff)
            if (!(x_diff == y_diff && x_diff == 0 && y_diff == 0)) {
                new_y = y + y_diff;

                if (!(new_x < 0 || new_x > COL_PTR - 1 || new_y < 0 || new_y > ROW_PTR - 1)) // check if we go out of the tables
                    if (!*CELL_VISITED_PTR(new_x, new_y)) // check if we have visited the cell
                        if (!(*PRISON_MAP_PTR(new_x, new_y) - DEATH_LVL_PTR < 0)) // check if the depth of the cell is above the 'death_level'
                            if (!(abs(*PRISON_MAP_PTR(new_x, new_y) - *PRISON_MAP_PTR(x, y)) < 0 || abs(*PRISON_MAP_PTR(new_x, new_y) - *PRISON_MAP_PTR(x, y)) >= 5)) // check if water pours into your neighbour's cell
                                water_volume += calculate_water_volume(prison_info, new_x, new_y);
            }
    }

    return water_volume;
}

void write_output_to_file(unsigned int water_volume) {
    FILE *out = fopen(OUTPUT_FILENAME, "wt");
    if (out) {
        fprintf(out, "%d", water_volume);
        fclose(out);
    } else
        printf("\nError writing to %s!\n", OUTPUT_FILENAME);
}

unsigned int get_correct_answer_from_file() {
    FILE *verify = fopen(ANSWER_FILENAME, "rt");
    unsigned int correct_answer;

    if (verify) {
        fscanf(verify, "%u", &correct_answer);
        fclose(verify);
    } else
        printf("\nError reading from %s!\n", ANSWER_FILENAME);

    return correct_answer;
}

