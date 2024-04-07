#include <getopt.h>
#include <locale.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

struct Grep {
  int* flagsArray;  //массив для кол-ва каждого из флагов
  int* filesArray;  //массив для индексов файлов в командной строке
  char** templatesPtr;  //указатель на массив шаблонов
  char* resultFlagO;
  int templatesCount;
  int resultFlagIV;  // 0 - шаблон нашелся
  int resultFlagI;
  int countFlagC;  //считаем кол-во совпадений по шаблону
  int countLine;  //выводим номер строки перед самой строкой
  int indexOptind;  //храним индекс первого шаблона, если нет ни одного -e
  int countfiles;   //колво файлов
  int printstrindex;  //если находимся на последнем шаблоне, то можем печатать
                      //флаг n
  int printfileindex;
  int templateInLine;  //был ли найден один из данных шаблонов в строке
  int templateInFile;
  int templateIsFind;
  int endFile;
  int countTemplates;
};

struct Grep* init(int argc, char** argv);
int match(char* string, char* pattern, regex_t* re);  // dfbd
int parserFlag(int filesArray[], int*, int*, int flagsArray[], int argc,
               char** argv, char*** templatesPtr);
void forFlags(struct Grep* ptrMygrep, char* line, char** argv, int i);
int flagE(char* string, char* pattern);
void flagIV(struct Grep*, char* string, char* pattern, int flagsArray[]);
void flagCL(struct Grep*, char nameFile[]);
int flagN(struct Grep*, char* string, char nameFile[]);
int flagF(char** templatesPtr, char* namefile, int count, int flagS);
int checkFiles(char** argv, char* optarg, int** filesArray, int argc,
               int flagf);
int forTemplatesRealloc(char*** templatesPtr, int rescount);
