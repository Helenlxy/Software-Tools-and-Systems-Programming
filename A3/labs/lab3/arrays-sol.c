#include <stdio.h>

/* When we pass arrays into functions we pass the name of the array as a 
 * and the size of the array.  Inside the function you can use the array
 * as normal. E.g. a[0], a[size-1]
 */
 
/* Return the sum of the elements of the array */
int sum(int *a, int size) {
    int i;
    int sum = 0;
    for(i = 0; i < size; i++) {
        sum += a[i];
    }
    return sum;  /* Change the return value */
}

/* Return the largest element in the array */
int biggest(int *a, int size){
    int i;
    int max = a[0];
    for(i = 0; i < size; i ++) {
        if(a[i] > max) {
            max = a[i];
        }
    }
    return max; /*Change the return value */
}

/* Return the average of the elements in the array.  
 * (Use sum to implement this)
 */
double average(int *a, int size) {
    return (double)sum(a, size) / size;
}

void reverse(int *a, int size) {
    int i = 0;
    int j = size - 1;

    while(i < j) {
        int temp;
        temp = a[i];
        a[i] = a[j];
        a[j] = temp;
        i++;
        j--;
    }
}

void print_array(int *a, int size) {
    int i;
    for(i = 0; i < size; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");
}



int main() {
    
    /* Test sum */
    int a[3] = {1, 2, 3};
    printf("1. sum returned %d. Expecting 6.\n", sum(a, 3));
    
    int b[1]  = {10};
    printf("2. sum returned %d. Expecting 10.\n", sum(b, 1));
    
    /* Test biggest */
    printf("3. biggest returned %d. Expecting 3.\n", biggest(a, 3));
    printf("4. biggest returned %d. Expecting 10\n", biggest(b, 1));
    int c[4] = {4, -1, 0, 3};
    printf("5. biggest returned %d. Expecting 4\n", biggest(c, 1));
    int d[3] = {-4, -1, -5};
    printf("6. biggest returned %d. Expecting -1\n", biggest(d, 3));
    
    /* Test average */
    printf("7. average returned %f. Expecting 2.0\n", average(a, 3));
    printf("8. average returned %f. Expecting 10.0\n", average(b, 1));
    printf("9. average returned %f. Expecting 1.5\n", average(c, 4));
    printf("10. average returned %f. Expecting -3.333\n", average(d, 3));

    /* Test reverse */
    printf("Reversing d - original: ");
    print_array(d, 3);
    printf("              reversed: ");
    reverse(d, 3);
    print_array(d, 3);

    printf("Reversing c - original: ");
    print_array(c, 4);
    printf("              reversed: ");
    reverse(c, 4);
    print_array(c, 4);

    printf("Reversing b - original: ");
    print_array(b, 1);
    printf("              reversed: ");
    reverse(b, 1);
    print_array(b, 1);
    return 0;
}
