#include <getopt.h>
#include <stdio.h>
#include <string.h>
void cat(FILE* file, int array[]);
void print_b(int arrnum1, int* ptr);
void print_e(int arrnum1);
void print_n(int arrnum1, int arrnum2, int* ptr);
void print_s(int arrnum1, char ch, int* ptr);
void print_t(int arrnum1, int flag, char ch);
int print_v(int arrnum1, char ch);
void print_tT(int arrnum1, int arrnum2, int flag, int ch);
void print_eE(int arrnum1, int arrnum2);
int parserFlag(int array[], int argc, char** argv);

struct option longOpts[] = {
    {"number-nonblank", no_argument, 0, 'b'},
    {"show-ends", no_argument, 0, 'E'},
    {"number", no_argument, 0, 'n'},
    {"squeeze-blank", no_argument, 0, 's'},
};
