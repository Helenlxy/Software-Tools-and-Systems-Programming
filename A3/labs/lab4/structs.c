#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define NAMESIZE 20

struct point {
    int x;
    int y;
};

struct rectangle {
    struct point upperleft;
    struct point lowerright;
    char label[NAMESIZE + 1];
};

/* create_point dynamically allocates memory (using malloc) to store a point, 
 * and gives it initial values.  Returns a pointer to the newly created structure
 */
struct point *create_point(int x, int y) {
    struct point *p = malloc(sizeof(struct point));
    if (p == NULL) {
        perror("Error allocating space for point.");
        exit(1);
    }
    p->x = x;
    p->y = y;
    return p;
}

/* create_rectangle dynamically allocates memory to store a rectangle,
 * gives it initial values, and returns a pointer to the newly created rectangle.
 */
struct rectangle *create_rectangle(struct point ul, struct point lr, char *label) {
    struct rectangle *r = malloc(sizeof(struct rectangle));
    /* TASK 1: fill in the rest of this function */
    
    return r;
}



/* TASK 2: Write two versions of the function to calculate the area of the
 *  rectangle.
 */

/* Compute the area of the rectangle r.  Assume all point values
 * are positive. 
 * area1 takes the struct itself as the parameter.
 */
int area1(struct rectangle r) {
    /* TASK 2: complete this function and replace the 0 */
    return 0;
}

/* Compute the area of the rectangle r.  Assume all point values
 * are positive. 
 * area2 takes a pointer to the struct as a parameter
 */
int area2(struct rectangle *r) {
    /* TASK 2: complete this function and replace the 0 */
    return 0;
}


/* TASK 3: Complete function change_label that changes the label
 *         to newlabel. Notice that change_label takes a pointer to the 
 *         struct as an argument 
 */
void change_label(struct rectangle *r, char *newlabel) {


}

/* TASK 4: Write a broken version of change_label by copying your change_label
 * code and changing it so that rectangle r is passed as the struct not as 
 * a pointer. Remember to change the symbol to access the fields of r from 
 * -> to . Otherwise your code should be the same.
 *
 * Add a call in main to broken_change_label and demonstrate the problem.
 */
void broken_change_label(struct rectangle r, char * newlabel) {


}
   

void print_rectangle(struct rectangle *r) {
    printf("(%d, %d) (%d, %d) %s\n", r->upperleft.x, r->upperleft.y, 
        r->lowerright.x, r->lowerright.y, r->label);
}


int main(void) {    
    
    char *str1 = "Big rectangle";
    char *str2 = "Square";
  
    struct point *p1 = create_point(10, 10);
    struct point *p2 = create_point(100, 100);
    
    struct rectangle *r1 = create_rectangle(*p1, *p2, str1);
    print_rectangle(r1);
    printf("    expecting: (10, 10) (100, 100) Big rectangle\n");
    
    free(p2);
    p2 = create_point(20, 20);
    
    struct rectangle r2; 
    strncpy(r2.label, str2, NAMESIZE);
    r2.label[NAMESIZE] = '\0';
    r2.upperleft = *p1;
    r2.lowerright = *p2;

    free(p1);
    free(p2);
    
    print_rectangle(&r2);
    printf("    expecting: (10, 10) (20, 20) Square\n");
    

    /* TASK 2: Add code to calculate the area of r1 and r2 using both versions
     * of the function on each rectangle.
     */


    /* TASK 3: Add code to call change_label on r1 and change it to 
     *   "newname rectangle"    
     */
     

    /* TASK 3: Add code to call change_label on r2 and change it to 
     *   "newname square"    
     */


    /* TASK 4: Add code to call broken_change_label on both r1 and r2
     *         and demonstrate the problem.
     */
     
    
    free(r1);
    return 0;
}
