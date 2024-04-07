#include "s21_cat.h"

int main(int argc, char *argv[]) {
  int array[8] = {0};  //храним наличие флагов 0 -> флага нет, 1 -> флаг есть
  if (parserFlag(array, argc, argv) == 0) printf("Опции не заданы\n");

  FILE *file = NULL;
  for (int i = 1; i < argc; i++) {
    if ((file = fopen(argv[i], "r")) != NULL) {
      cat(file, array);
      fclose(file);
    }
  }
}

int parserFlag(int array[], int argc, char **argv) {
  int c = 0;
  int index = 0;
  char *shortOpts = "+bevnstTE";
  while ((c = getopt_long(argc, argv, shortOpts, longOpts, &index)) != -1) {
    if (c == 'b') {
      array[0] = 1;
    } else if (c == 'e') {
      array[1] = 1;  // e
      array[5] = 1;  // v
      array[6] = 1;  // E
    } else if (c == 'n') {
      array[2] = 1;
    } else if (c == 's')
      array[3] = 1;
    else if (c == 't') {
      array[4] = 1;
      array[5] = 1;  // v
      array[7] = 1;  // T
    } else if (c == 'T') {
      array[7] = 1;
    } else if (c == 'v') {
      array[5] = 1;  // v
    } else if (c == 'E') {
      array[6] = 1;
    }
  }

  return (c);
}

void cat(FILE *file, int array[]) {
  int count = 1;
  int ch;
  int enterc = 0, nonempty = 0, flag = 0;
  while ((ch = getc(file)) != EOF) {
    if (ch == '\n')
      enterc++;
    else
      enterc = 0;

    if (enterc == 0) {
      if (nonempty == 0) {
        print_b(array[0], &count);
        print_n(array[2], array[0], &count);
        nonempty = 1;
      }
      flag = print_v(array[5], ch);  //отслеживаем был ли использован -v
      print_tT(array[4], array[7], flag, ch);

    }

    else if (enterc == 1) {
      if (nonempty == 0) {
        print_n(array[2], array[0], &count);
        print_v(array[5], ch);
        print_eE(array[1], array[6]);
        putchar(ch);
      } else {
        print_v(array[5], ch);
        print_e(array[1]);
        putchar(ch);
        enterc = 0;
        nonempty = 0;
      }
    }

    else if (enterc == 2) {
      if (nonempty == 0) {
        if (array[3] == 0) {
          print_n(array[2], array[0], &count);
          print_v(array[5], ch);
          print_e(array[1]);
          putchar(ch);
        }
        enterc = 1;
      }
    }
  }
}

void print_b(int arrnum1, int *ptr) {
  if (arrnum1 == 1) {
    printf("%6d\t", *ptr);
    (*ptr)++;
  }
}
void print_e(int arrnum1) {
  if (arrnum1 == 1) putchar('$');
}
void print_n(int arrnum1, int arrnum2, int *ptr) {
  if (arrnum1 == 1 && arrnum2 == 0) {
    printf("%6d\t", *ptr);
    (*ptr)++;
  }
}

void print_s(int arrnum1, char ch, int *ptr) {
  if (arrnum1 == 0) {
    putchar(ch);
  } else
    (*ptr)++;
}
void print_t(int arrnum1, int flag, char ch) {
  if (arrnum1 == 1) {
    if (ch == '\t') {
      printf("^I");
    } else if (flag == 0)
      putchar(ch);
  } else if (arrnum1 == 0 && flag == 0)
    putchar(ch);
}

int print_v(int arrnum1, char symbol) {
  int ret = 0;
  int ch = symbol;
  if (arrnum1 == 1) {
    if (ch < 0) {
      ch &= 127;  // ch = ch & 127
      ch += 128;
      if (ch > 127 && ch < 160) {
        printf("M-^");
        ch -= 64;
        putchar(ch);
        ret = 1;
      }
    } else if (((ch >= 0 && ch < 9) || (ch > 10 && ch < 32)) && ch != '\t') {
      printf("^");
      putchar(ch + 64);
      ret = 1;
    } else if (ch == 127) {
      printf("^?");
      ret = 1;
    }
  }
  return ret;  // 1 если флаг применялся
}

void print_tT(int arrnum1, int arrnum2, int flag, int ch) {
  if (arrnum1 == 1 && arrnum2 == 0)
    print_t(arrnum1, flag, ch);
  else if (arrnum2 == 1 && arrnum1 == 0)
    print_t(arrnum2, flag, ch);
  else if (arrnum1 == 1 && arrnum2 == 1)
    print_t(arrnum1, flag, ch);
  else if (arrnum1 == 0 && arrnum2 == 0)
    print_t(arrnum1, flag, ch);
}

void print_eE(int arrnum1, int arrnum2) {
  if (arrnum1 == 1 && arrnum2 == 0)
    print_e(arrnum1);
  else if (arrnum2 == 1 && arrnum1 == 0)
    print_e(arrnum2);
  else if (arrnum1 == 1 && arrnum2 == 1)
    print_e(arrnum1);
  else if (arrnum1 == 0 && arrnum2 == 0)
    print_e(arrnum1);
}
