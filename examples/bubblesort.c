#include <stdio.h>
#include <stdlib.h>

// Heap usage via malloc (brk syscall) plus nested loops.
static void bubble_sort(int* a, int n) {
  for (int i = 0; i < n - 1; i++) {
    for (int j = 0; j < n - i - 1; j++) {
      if (a[j] > a[j + 1]) {
        int tmp = a[j];
        a[j] = a[j + 1];
        a[j + 1] = tmp;
      }
    }
  }
}

int main(void) {
  const int input[] = {5, 3, 8, 1, 9, 2, 7, 4, 6, 0};
  int* a = (int*)malloc(sizeof(input));
  if (!a) {
    printf("malloc failed\n");
    return 1;
  }

  for (int i = 0; i < 10; i++)
    a[i] = input[i];
  bubble_sort(a, 10);

  for (int i = 0; i < 10; i++)
    printf("%d ", a[i]);
  printf("\n");
  free(a);
  return 0;
}