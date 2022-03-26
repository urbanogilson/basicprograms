/*
As a user, I want to be able to locate an existing contact in a list using the
partial or full number.

Write a program that finds the number P in the list of numbers B with length N
and returns the name of contact A.

A - List of names (max. 10 chars per name)
B - List of phone numbers
N - Length of list
P - Number to be found

If the number is not on the list, return "NO CONTACT"
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *solution(char *A[], char *B[], int N, char *P);

void test(char *A[], char *B[], int N, char *P, char *answer);

void case1();

void case2();

void case3();

void case4();

void case5();

void case6();

int main(void) {
  case1();
  case2();
  case3();
  case4();
  case5();
  case6();
}

char *solution(char *A[], char *B[], int N, char *P) {
  // 10 + 1 for the null terminator character '\0'/
  char *name = malloc(sizeof(char) * 11);

  if (NULL == name) {
    return NULL;
  }

  bool containsContacts = false;

  for (int i = 0; i < N; i++) {
    if (NULL != strstr(B[i], P)) {
      if (containsContacts) {
        int lenName = strlen(name), lenContact = strlen(A[i]);

        if (strncmp(name, A[i], lenName <= lenContact ? lenName : lenContact) >
            0) {
          strcpy(name, A[i]);
        }
      } else {
        containsContacts = true;
        strcpy(name, A[i]);
      }
    }
  }

  if (!containsContacts) {
    strcpy(name, "NO CONTACT");
  }

  return name;
}

void test(char *A[], char *B[], int N, char *P, char *answer) {
  char *ret = solution(A, B, N, P);

  printf("%s -> %s\n", ret, strcmp(ret, answer) == 0 ? "Passed" : "Failed");

  free(ret);
}

void case1() {
  char *A[] = {"hello", "world"};
  char *B[] = {"999999999", "777888999"};
  char *P = "88999";
  int N = 2;
  char *answer = "world";

  test(A, B, N, P, answer);
}

void case2() {
  char *A[] = {"hello", "gilson", "urbano"};
  char *B[] = {"2315456456456", "77788787988918999", "111111"};
  char *P = "1";
  int N = 3;
  char *answer = "gilson";

  test(A, B, N, P, answer);
}

void case3() {
  char *A[] = {"hello", "world", "test"};
  char *B[] = {"999999999", "777888999", "3313"};
  char *P = "333";
  int N = 3;
  char *answer = "NO CONTACT";

  test(A, B, N, P, answer);
}

void case4() {
  char *A[] = {"A", "B", "C", "D0", "D1", "D2", "E", "F"};
  char *B[] = {"123456",  "654321", "456", "4567",
               "9874567", "789",    "963", "852"};
  char *P = "4567";
  int N = 8;
  char *answer = "D0";

  test(A, B, N, P, answer);
}

void case5() {
  char *A[] = {"A"};
  char *B[] = {"123456"};
  char *P = "123";
  int N = 1;
  char *answer = "A";

  test(A, B, N, P, answer);
}

void case6() {
  char *A[] = {};
  char *B[] = {};
  char *P = "123";
  int N = 0;
  char *answer = "NO CONTACT";

  test(A, B, N, P, answer);
}