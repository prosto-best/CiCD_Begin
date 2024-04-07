#include "s21_grep.h"
int main(int argc, char* argv[]) {
  setlocale(LC_ALL, "");
  struct Grep* ptrGrep = NULL;
  ptrGrep = init(argc, argv);
  if (ptrGrep != NULL) {
    FILE* file = NULL;
    char* line = NULL;
    for (int i = 0; i < ptrGrep->countfiles; i++) {
      if ((file = fopen(argv[ptrGrep->filesArray[i]], "r+")) != NULL) {
        ptrGrep->endFile = 0;
        size_t len = 0;
        ssize_t read;
        while ((read = getline(&line, &len, file)) != -1)
          forFlags(ptrGrep, line, argv, i);

        ptrGrep->endFile = 1;
        flagCL(ptrGrep, argv[ptrGrep->filesArray[i]]);

        if (ptrGrep->flagsArray[4] == 0 && ptrGrep->flagsArray[3] == 1) {
          if (ptrGrep->countfiles > 1 && ptrGrep->flagsArray[6] == 0)
            printf("%s:", argv[ptrGrep->filesArray[i]]);
          printf("%d\n", ptrGrep->countFlagC);
        }
        ptrGrep->templateInFile = 0;
        ptrGrep->countLine = 1;
        ptrGrep->countFlagC = 0;
        ptrGrep->printfileindex = 0;
      }
      if (file != NULL) fclose(file);
    }
    if (line != NULL) {
      free(line);
      line = NULL;
    }
    free(ptrGrep->flagsArray);
    ptrGrep->flagsArray = NULL;
    free(ptrGrep->filesArray);
    ptrGrep->filesArray = NULL;
    for (int i = 0; i < ptrGrep->countTemplates; i++) {
      if (ptrGrep->templatesPtr[i] != NULL) {
        free(ptrGrep->templatesPtr[i]);
        ptrGrep->templatesPtr[i] = NULL;
      }
    }
    if (ptrGrep->templatesPtr != NULL) {
      free(ptrGrep->templatesPtr);
      ptrGrep->templatesPtr = NULL;
    }
    if (ptrGrep->resultFlagO != NULL) {
      free(ptrGrep->resultFlagO);
      ptrGrep->resultFlagO = NULL;
    }
    free(ptrGrep);
    ptrGrep = NULL;
  }
  return 0;
}

struct Grep* init(int argc, char** argv) {
  struct Grep* ptrGrep = {0};
  ptrGrep = (struct Grep*)calloc(1, sizeof(struct Grep));
  if (ptrGrep != NULL) {
    int* flagsArray = (int*)calloc(10, sizeof(int));
    int* filesArray = (int*)calloc(argc, sizeof(int));
    char** templatesPtr = (char**)calloc(argc, sizeof(char*));

    if (templatesPtr != NULL && flagsArray != NULL && filesArray != NULL) {
      ptrGrep->templatesPtr = templatesPtr;
      ptrGrep->indexOptind = parserFlag(filesArray, &(ptrGrep->countTemplates),
                                        &(ptrGrep->countfiles), flagsArray,
                                        argc, argv, &(ptrGrep->templatesPtr));
      ptrGrep->flagsArray = flagsArray;
      ptrGrep->filesArray = filesArray;
      ptrGrep->countLine = 1;
      ptrGrep->countFlagC = 0;
    } else {
      for (int i = 0; templatesPtr[i]; i++) {
        if (templatesPtr[i] != NULL) {
          free(templatesPtr[i]);
          templatesPtr[i] = NULL;
        }
      }
      if (templatesPtr != NULL) {
        free(templatesPtr);
        templatesPtr = NULL;
      }
      if (flagsArray != NULL) {
        free(flagsArray);
        flagsArray = NULL;
      }
      if (filesArray != NULL) {
        free(filesArray);
        filesArray = NULL;
      }
      free(ptrGrep);
      ptrGrep = NULL;
    }
  }
  return ptrGrep;
}

void forFlags(struct Grep* ptrMygrep, char* line, char** argv, int j) {
  ptrMygrep->templateInLine = 0;
  ptrMygrep->templateIsFind = 0;
  if (ptrMygrep->templatesPtr[0] == NULL) {
    //в этом случае просто выполняем все найденные флаги к первому попавшемуся
    //шаблону
    ptrMygrep->printstrindex = -1;
    flagIV(ptrMygrep, line, argv[ptrMygrep->indexOptind],
           ptrMygrep->flagsArray);  //первым делом идем во флаг i
    flagCL(ptrMygrep, argv[ptrMygrep->filesArray[j]]);
    flagN(ptrMygrep, line, argv[ptrMygrep->filesArray[j]]);
  } else {
    //в этом случае применяем флаги к каждому найденному шаблону
    for (int i = 0; i < ptrMygrep->countTemplates; i++) {
      ptrMygrep->printstrindex = i;
      flagIV(ptrMygrep, line, ptrMygrep->templatesPtr[i],
             ptrMygrep->flagsArray);  //первым делом идем во флаг i

      flagCL(ptrMygrep, argv[ptrMygrep->filesArray[j]]);
      flagN(ptrMygrep, line, argv[ptrMygrep->filesArray[j]]);
    }
  }
  ptrMygrep->printstrindex = -1;
  ptrMygrep->countLine++;
}

void flagIV(struct Grep* ptrGrep, char* string, char* pattern,
            int flagsArray[]) {
  int status = -1;
  char* submatch = NULL;
  regex_t re;
  regmatch_t pmatch = {0};
  size_t nmatch = 1;
  if (flagsArray[1] == 0)  //флага i нет
    status = regcomp(&re, pattern, REG_EXTENDED);
  else
    status = regcomp(&re, pattern, REG_ICASE);

  if (status == 0) {
    status = regexec(&re, string, nmatch, &pmatch, 0);
    ptrGrep->resultFlagI = status;
    if ((status == 0 && flagsArray[2] == 0) ||
        (status != 0 && flagsArray[2] != 0)) {
      if (pmatch.rm_eo - pmatch.rm_so > 0) {
        submatch = (char*)calloc(pmatch.rm_eo - pmatch.rm_so + 1, sizeof(char));
        if (submatch != NULL) {
          strncpy(submatch, string + pmatch.rm_so, pmatch.rm_eo - pmatch.rm_so);
          submatch[pmatch.rm_eo - pmatch.rm_so] = '\0';
          if (ptrGrep->resultFlagO != NULL) {
            free(ptrGrep->resultFlagO);
            ptrGrep->resultFlagO = NULL;
          }
          ptrGrep->resultFlagO = submatch;
        }
      }
      status = 0;
    } else
      status = 1;
    ptrGrep->resultFlagIV = status;
  }
  regfree(&re);
}

void flagCL(struct Grep* ptrGrep, char nameFile[]) {
  if (ptrGrep->resultFlagIV == 0) {
    ptrGrep->templateInFile = 1;
    ptrGrep->templateInLine = 1;
  } else if (ptrGrep->templateInLine == 1 && ptrGrep->resultFlagIV != 0 &&
             ptrGrep->flagsArray[2] != 0) {
    ptrGrep->templateInLine = 0;
  }
  if (ptrGrep->resultFlagI == 0) ptrGrep->templateIsFind = 1;

  if (ptrGrep->flagsArray[4] == 1 && ptrGrep->endFile == 1) {
    if (ptrGrep->flagsArray[3] == 1 && ptrGrep->templateInFile == 1)
      printf("%s\n", nameFile);

    else if (ptrGrep->flagsArray[3] == 1 && ptrGrep->templateInFile != 1) {
      if (ptrGrep->countfiles > 1)
        printf("%s:%d\n", nameFile, 0);
      else
        printf("%d\n", 0);
    } else if (ptrGrep->templateInFile == 1 || ptrGrep->templateInLine == 1) {
      printf("%s\n", nameFile);
    }

  } else {
    if (ptrGrep->flagsArray[3] == 1 &&
        ptrGrep->printstrindex == ptrGrep->countTemplates - 1 &&
        ptrGrep->endFile == 0) {
      if (((ptrGrep->templateIsFind == 1 && ptrGrep->flagsArray[2] == 0) ||
           (ptrGrep->templateIsFind != 1 && ptrGrep->flagsArray[2] != 0))) {
        ptrGrep->countFlagC++;
      }
    }
  }
}

int flagN(struct Grep* ptrGrep, char* string, char nameFile[]) {
  if (ptrGrep->flagsArray[3] == 0 && ptrGrep->flagsArray[4] == 0 &&
      ptrGrep->flagsArray[5] == 1) {
    //в этих случаях принтуем номер строки и строку
    if (((ptrGrep->templateIsFind == 1 && ptrGrep->flagsArray[2] == 0) ||
         (ptrGrep->templateIsFind != 1 && ptrGrep->flagsArray[2] != 0))) {
      if (ptrGrep->printstrindex == ptrGrep->countTemplates - 1) {
        if (ptrGrep->flagsArray[9] == 0) {
          if (ptrGrep->countfiles > 1 && ptrGrep->flagsArray[6] == 0) {
            printf("%s:", nameFile);
          }
          printf("%d:%s", ptrGrep->countLine, string);
          if (strcmp(string + strlen(string) - 1, "\n")) printf("\n");
        } else if (ptrGrep->flagsArray[9] == 1 &&
                   ptrGrep->resultFlagO != NULL) {
          if (ptrGrep->countfiles > 1 && ptrGrep->flagsArray[6] == 0) {
            printf("%s:", nameFile);
          }
          printf("%d:%s\n", ptrGrep->countLine, ptrGrep->resultFlagO);
        }
      }
    }
  } else if (ptrGrep->flagsArray[3] == 0 && ptrGrep->flagsArray[4] == 0 &&
             ptrGrep->flagsArray[5] == 0) {
    if ((((ptrGrep->templateIsFind == 1 && ptrGrep->flagsArray[2] == 0) ||
          (ptrGrep->templateIsFind != 1 && ptrGrep->flagsArray[2] != 0))) &&
        ptrGrep->printstrindex == ptrGrep->countTemplates - 1) {
      if (ptrGrep->flagsArray[9] == 0) {
        if (ptrGrep->countfiles > 1 && ptrGrep->flagsArray[6] == 0) {
          printf("%s:", nameFile);
        }
        printf("%s", string);
        if (strcmp(string + strlen(string) - 1, "\n")) printf("\n");
      } else if (ptrGrep->flagsArray[9] == 1 && ptrGrep->resultFlagO != NULL) {
        if (ptrGrep->countfiles > 1 && ptrGrep->flagsArray[6] == 0) {
          printf("%s:", nameFile);
        }
        printf("%s\n", ptrGrep->resultFlagO);
      }
    }
  }
  return 0;
}

int flagF(char** templatesPtr, char* namefile, int counter, int flagS) {
  char* line2 = NULL;
  FILE* file2;
  int count = counter;
  if ((file2 = fopen(namefile, "r")) != NULL) {
    size_t len2 = 0;
    ssize_t read2;

    while ((read2 = getline(&line2, &len2, file2)) != -1) {
      templatesPtr[count] = strdup(line2);
      if (templatesPtr[count][strlen(line2) - 1] == '\n') {
        templatesPtr[count][strlen(line2) - 1] = 0;
      }
      count++;
    }
  } else if (flagS == 0)
    fprintf(stderr, "grep: %s: No such file or directory\n", namefile);

  if (line2 != NULL) {
    free(line2);
    line2 = NULL;
  }

  if (file2 != NULL) {
    fclose(file2);
    file2 = NULL;
  }

  return count;
}
int fopenFile(char* namefile) {
  char* line2 = NULL;
  FILE* file2;
  int copenfile = 0;
  if ((file2 = fopen(namefile, "r")) != NULL) {
    size_t len2 = 0;
    ssize_t read2;
    while ((read2 = getline(&line2, &len2, file2)) != -1) {
      copenfile++;
    }
  }

  if (line2 != NULL) {
    free(line2);
    line2 = NULL;
  }

  if (file2 != NULL) {
    fclose(file2);
    file2 = NULL;
  }
  return copenfile;
}
int forTemplatesRealloc(char*** templatesPtr, int rescount) {
  int result = 0;
  char*** tempPtr = templatesPtr;
  *templatesPtr = (char**)realloc(*templatesPtr, (rescount) * sizeof(char*));
  if ((*templatesPtr) != NULL)
    result = 1;
  else {
    printf("\nОшибка выделения памяти под шаблон\n");
    free(tempPtr);
    tempPtr = NULL;
  }
  return result;
}

int parserFlag(int filesArray[], int* countTemplates, int* countF, int array[],
               int argc, char** argv, char*** templatesPtr) {
  int c = 0, count = 0, copenfile = 0, index = 0;
  while ((c = getopt_long(argc, argv, "e:ivclnhsf:o", 0, &index)) != -1) {
    if (c == 'e') {
      array[0]++;
      if (count > argc) {
        if (forTemplatesRealloc(templatesPtr, count + copenfile) == 1)
          (*templatesPtr)[count] = strdup(optarg);
      } else
        (*templatesPtr)[count] = strdup(optarg);
      count++;

    } else if (c == 'i')
      array[1] = 1;
    else if (c == 'v')
      array[2] = 1;
    else if (c == 'c')
      array[3] = 1;
    else if (c == 'l')
      array[4] = 1;
    else if (c == 'n')
      array[5] = 1;
    else if (c == 'h')
      array[6] = 1;
    else if (c == 's')
      array[7] = 1;
    else if (c == 'f') {
      array[8] = 1;
      copenfile = fopenFile(optarg);  //узнаем кол-во строк в открываемом файле
      if (count + copenfile > argc) {
        if (forTemplatesRealloc(templatesPtr, count + copenfile) == 1)
          count = flagF(*templatesPtr, optarg, count, array[7]);
      } else
        count = flagF(*templatesPtr, optarg, count, array[7]);
      *countF = checkFiles(argv, optarg, &filesArray, argc, array[8]);

    } else if (c == 'o')
      array[9] = 1;
  }
  *countTemplates = count;
  if (array[8] == 0)
    *countF = checkFiles(argv, optarg, &filesArray, argc, array[8]);
  return optind;
}

int checkFiles(char** argv, char* optarg, int** filesArray, int argc,
               int flagf) {
  int countfiles = 0;
  FILE* file = NULL;
  for (int i = 1; i < argc; i++) {
    if (flagf == 0) {
      if ((file = fopen(argv[i], "r")) != NULL) {
        (*filesArray)[countfiles] = i;
        countfiles++;
      }
    } else if (flagf == 1) {
      if ((file = fopen(argv[i], "r")) != NULL && strcmp(argv[i], optarg)) {
        (*filesArray)[countfiles] = i;
        countfiles++;
      }
    }

    if (file != NULL) fclose(file);
  }

  return countfiles;
}
