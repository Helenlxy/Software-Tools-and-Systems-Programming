#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hcq.h"
#define INPUT_BUFFER_SIZE 256

/*
 * Return a pointer to the struct student with name stu_name
 * or NULL if no student with this name exists in the stu_list
 */
Student *find_student(Student *stu_list, char *student_name) {  
    Student *curr = stu_list;
    while(curr != NULL)
    {
        if (strcmp(curr->name, student_name) == 0)
        {
            return curr;
        }
        curr = curr->next_overall;
    }
    return NULL;
}



/*   Return a pointer to the ta with name ta_name or NULL
 *   if no such TA exists in ta_list. 
 */
Ta *find_ta(Ta *ta_list, char *ta_name) {
    Ta *curr = ta_list;
    while(curr != NULL)
    {
        if (strcmp(curr->name, ta_name) == 0)
        {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}


/*  Return a pointer to the course with this code in the course list
 *  or NULL if there is no course in the list with this code.
 */
Course *find_course(Course *courses, int num_courses, char *course_code) {
    for (int i = 0; i < num_courses; i++)
    {
        if (strcmp(courses[i].code, course_code) == 0)
        {

            return &(courses[i]);
        }
    }
    return NULL;
}
    

/* Add a student to the queue with student_name and a question about course_code.
 * if a student with this name already has a question in the queue (for any
   course), return 1 and do not create the student.
 * If course_code does not exist in the list, return 2 and do not create
 * the student struct.
 * For the purposes of this assignment, don't check anything about the 
 * uniqueness of the name. 
 */
int add_student(Student **stu_list_ptr, char *student_name, char *course_code,
    Course *course_array, int num_courses) {
    //Handle special cases
    if (find_student(*stu_list_ptr, student_name) != NULL)
    {
        return 1;
    }else if (find_course(course_array, num_courses, course_code) == NULL)
    {
        return 2;
    }

    //Create a student
    Student *new_stu = malloc(sizeof(struct student));
    if (new_stu == NULL) {
       perror("malloc for new student");
       exit(1);
    }
    new_stu->name = malloc(sizeof(student_name));
    if (new_stu->name == NULL) {
       perror("malloc for new student's name");
       exit(1);
    }
    strcpy(new_stu->name, student_name);
    new_stu->arrival_time = malloc(sizeof(time_t));
    if (new_stu->arrival_time == NULL) {
       perror("malloc for new student's arrival time");
       exit(1);
    }
    time(new_stu->arrival_time);
    new_stu->course = find_course(course_array, num_courses, course_code);
    new_stu->next_overall = NULL;
    new_stu->next_course = NULL;

    //Find the last student for two linked list
    Student *prev = NULL;
    Student *curr = *stu_list_ptr;
    
    Student *curr1 = NULL;
    while(curr != NULL)
    {
        if (strcmp(course_code, curr->course->code) == 0)
        {
            curr1 = curr;
        }
        prev = curr;
        curr = curr->next_overall;

    }

    //Add the student to 2 lists
    if (prev == NULL)
    {
        *stu_list_ptr = new_stu;
    }else{
        prev->next_overall = new_stu;
    }
    
    if (curr1 != NULL)
    {
        curr1->next_course = new_stu;
    }

    //Manage the head and tail student of the course
    Course *course_ptr = find_course(course_array, num_courses, course_code);
    course_ptr->tail = new_stu;
    if (curr1 == NULL)
    {
        course_ptr->head = new_stu;
    }

    return 0;
}


/* Student student_name has given up waiting and left the help centre
 * before being called by a Ta. Record the appropriate statistics, remove
 * the student from the queues and clean up any no-longer-needed memory.
 *
 * If there is no student by this name in the stu_list, return 1.
 */
int give_up_waiting(Student **stu_list_ptr, char *student_name) {
    //Handle special case
    Student *curr_stu = find_student(*stu_list_ptr, student_name);
    if (curr_stu == NULL)
    {
        return 1;
    }

    //Record the appropriate statistics
    Course *curr_cour = curr_stu->course;
    time_t curr_time = time(NULL);
    curr_cour->wait_time += difftime(curr_time, *(curr_stu->arrival_time));
    curr_cour->bailed++;

    //preparetion for removing
    Student *prev = NULL;
    Student *curr = *stu_list_ptr;
    Student *prev_course = NULL;
    while(strcmp(curr->name, student_name) != 0)
    {
        if (strcmp(curr->course->code, curr_cour->code) == 0)
        {
            prev_course = curr;
        }
        prev = curr;
        curr = curr->next_overall;
    }

    //if this student is the first one, change student_list_ptr to the next one. otherwise remove it from overall list
    if (curr == *stu_list_ptr)
    {
        *stu_list_ptr = curr->next_overall;
    }else{
        prev->next_overall = curr->next_overall;
    }

    //remove it from course list and change the head and tail of this course
    if (prev_course != NULL)
    {
        prev_course->next_course = curr->next_course;
    }else{
        curr_cour->head = curr->next_course;
    }
    if (curr->next_course == NULL)
    {
        curr_cour->tail = prev_course;
    }

    //clean up!
    free(curr_stu->arrival_time);
    free(curr_stu->name);
    free(curr_stu);

    return 0;
}

/* Create and prepend Ta with ta_name to the head of ta_list. 
 * For the purposes of this assignment, assume that ta_name is unique
 * to the help centre and don't check it.
 */
void add_ta(Ta **ta_list_ptr, char *ta_name) {
    // first create the new Ta struct and populate
    Ta *new_ta = malloc(sizeof(Ta));
    if (new_ta == NULL) {
       perror("malloc for TA");
       exit(1);
    }
    new_ta->name = malloc(strlen(ta_name)+1);
    if (new_ta->name  == NULL) {
       perror("malloc for TA name");
       exit(1);
    }
    strcpy(new_ta->name, ta_name);
    new_ta->current_student = NULL;

    // insert into front of list
    new_ta->next = *ta_list_ptr;
    *ta_list_ptr = new_ta;
}

/* The TA ta is done with their current student. 
 * Calculate the stats (the times etc.) and then 
 * free the memory for the student. 
 * If the TA has no current student, do nothing.
 */
void release_current_student(Ta *ta) {
    if (ta->current_student != NULL)
    {
        time_t curr_time = time(NULL);
        Student *curr_stu = ta->current_student;
        Course *curr_cour = curr_stu->course;
        curr_cour->help_time += difftime(curr_time, *(curr_stu->arrival_time));
        curr_cour->helped += 1;
        free(curr_stu->arrival_time);
        free(curr_stu->name);
        free(ta->current_student);
        ta->current_student = NULL;
    }

}

/* Remove this Ta from the ta_list and free the associated memory with
 * both the Ta we are removing and the current student (if any).
 * Return 0 on success or 1 if this ta_name is not found in the list
 */
int remove_ta(Ta **ta_list_ptr, char *ta_name) {
    Ta *head = *ta_list_ptr;
    if (head == NULL) {
        return 1;
    } else if (strcmp(head->name, ta_name) == 0) {
        // TA is at the head so special case
        *ta_list_ptr = head->next;
        release_current_student(head);
        // memory for the student has been freed. Now free memory for the TA.
        free(head->name);
        free(head);
        return 0;
    }
    while (head->next != NULL) {
        if (strcmp(head->next->name, ta_name) == 0) {
            Ta *ta_tofree = head->next;
            //  We have found the ta to remove, but before we do that 
            //  we need to finish with the student and free the student.
            //  You need to complete this helper function
            release_current_student(ta_tofree);

            head->next = head->next->next;
            // memory for the student has been freed. Now free memory for the TA.
            free(ta_tofree->name);
            free(ta_tofree);
            return 0;
        }
        head = head->next;
    }
    // if we reach here, the ta_name was not in the list
    return 1;
}






/* TA ta_name is finished with the student they are currently helping (if any)
 * and are assigned to the next student in the full queue. 
 * If the queue is empty, then TA ta_name simply finishes with the student 
 * they are currently helping, records appropriate statistics, 
 * and sets current_student for this TA to NULL.
 * If ta_name is not in ta_list, return 1 and do nothing.
 */
int take_next_overall(char *ta_name, Ta *ta_list, Student **stu_list_ptr) {
    Ta *ta = find_ta(ta_list, ta_name);
    if (ta == NULL)
    {
        return 1;
    }
    release_current_student(ta);
    if (*stu_list_ptr != NULL)
    {
        Student *next = *stu_list_ptr;
        ta->current_student = next;
        next->course->head = next->next_course;
        time_t curr_time = time(NULL);
        next->course->wait_time += difftime(curr_time, *(next->arrival_time));
        time(next->arrival_time);
        *stu_list_ptr = next->next_overall;
    }

    return 0;
}



/* TA ta_name is finished with the student they are currently helping (if any)
 * and are assigned to the next student in the course with this course_code. 
 * If no student is waiting for this course, then TA ta_name simply finishes 
 * with the student they are currently helping, records appropriate statistics,
 * and sets current_student for this TA to NULL.
 * If ta_name is not in ta_list, return 1 and do nothing.
 * If course is invalid return 2, but finish with any current student. 
 */
int take_next_course(char *ta_name, Ta *ta_list, Student **stu_list_ptr, char *course_code, Course *courses, int num_courses) {
    Ta *ta = find_ta(ta_list, ta_name);
    Course *course = find_course(courses, num_courses, course_code);
    if (ta == NULL)
    {
        return 1;
    }
    release_current_student(ta);
    if (course == NULL)
    {
        return 2;
    }
    if (course->head != NULL)
    {
        Student *next = course->head;
        ta->current_student = next;
        course->head = next->next_course;

        Student *prev = NULL;
        Student *curr = *stu_list_ptr;
        while(strcmp(curr->name, next->name) != 0)
        {
            prev = curr;
            curr = curr->next_overall;
        }
        if (curr == *stu_list_ptr)
        {
            *stu_list_ptr = curr->next_overall;
        }else{
            prev->next_overall = curr->next_overall;
        }

        time_t curr_time = time(NULL);
        next->course->wait_time += difftime(curr_time, *(next->arrival_time));
        time(next->arrival_time);
    }
    return 0;
}


/* For each course (in the same order as in the config file), print
 * the <course code>: <number of students waiting> "in queue\n" followed by
 * one line per student waiting with the format "\t%s\n" (tab name newline)
 * Uncomment and use the printf statements below. Only change the variable
 * names.
 */
void print_all_queues(Student *stu_list, Course *courses, int num_courses) {
    for (int i = 0; i < num_courses; i++)
    {
        Student *student = courses[i].head;
        int num = 0;
        while(student != NULL){
            num++;
            student = student->next_course;
        }
        printf("%s: %d in queue\n", courses[i].code, num);
        student = courses[i].head;
        while(student != NULL){
            printf("\t%s\n",student->name); 
            student = student->next_course;
        }
    }
    /*Student *curr = stu_list;
    while(curr != NULL){
        printf("%s\n", curr->name);
        curr = curr->next_overall;
    }*/
}


/*
 * Print to stdout, a list of each TA, who they are serving at from what course
 * Uncomment and use the printf statements 
 */
void print_currently_serving(Ta *ta_list) {
    if (ta_list == NULL)
    {
        printf("No TAs are in the help centre.\n");
    }else{
        Ta *curr = ta_list;
        while(curr != NULL){
            if (curr->current_student == NULL)
            {
                printf("TA: %s has no student\n", curr->name);
            }else{
                Course *course = curr->current_student->course;
                printf("TA: %s is serving %s from %s\n", curr->name, curr->current_student->name, course->code);
            }
            curr = curr->next;
        }
    }
}


/*  list all students in queue (for testing and debugging)
 *   maybe suggest it is useful for debugging but not included in marking? 
 */ 
void print_full_queue(Student *stu_list) {
    Student *curr = stu_list;
    while(curr != NULL){
        printf("%s\n", curr->name);
        curr = curr->next_overall;
    }

}

/* Prints statistics to stdout for course with this course_code
 * See example output from assignment handout for formatting.
 *
 */
int stats_by_course(Student *stu_list, char *course_code, Course *courses, int num_courses, Ta *ta_list) {

    // TODO: students will complete these next pieces but not all of this 
    //       function since we want to provide the formatting
    Course *found = find_course(courses, num_courses, course_code);
    if (found == NULL)
    {
        return 1;
    }

    Student *curr_stu = found->head;
    int students_waiting = 0;
    while(curr_stu != NULL)
    {
        students_waiting++;
        curr_stu = curr_stu->next_course;
    }

    Ta *curr_ta = ta_list;
    int students_being_helped = 0;
    while(curr_ta != NULL)
    {
        if (curr_ta->current_student != NULL)
        {
            if (strcmp(course_code, curr_ta->current_student->course->code) == 0)
            {
                students_being_helped++;
            }
        }
        
        curr_ta = curr_ta->next;
    }
    // You MUST not change the following statements or your code 
    //  will fail the testing. 

    printf("%s:%s \n", found->code, found->description);
    printf("\t%d: waiting\n", students_waiting);
    printf("\t%d: being helped currently\n", students_being_helped);
    printf("\t%d: already helped\n", found->helped);
    printf("\t%d: gave_up\n", found->bailed);
    printf("\t%f: total time waiting\n", found->wait_time);
    printf("\t%f: total time helping\n", found->help_time);

    return 0;
}


/* Dynamically allocate space for the array course list and populate it
 * according to information in the configuration file config_filename
 * Return the number of courses in the array.
 * If the configuration file can not be opened, call perror() and exit.
 */
int config_course_list(Course **courselist_ptr, char *config_filename) {
    FILE *config_file;
    int num;

    config_file = fopen(config_filename, "r");
    if (config_file == NULL)
    {
        perror("The file can't be opened.");
        exit(1);
    }
    fscanf(config_file, "%d", &num);
    *courselist_ptr = malloc(sizeof(Course) * num);
    if (courselist_ptr == NULL) {
       perror("malloc for course list");
       exit(1);
    }
    int curr = 0;
    char name[7];
    char *descr = malloc(sizeof(char) * (INPUT_BUFFER_SIZE - 6));
    if (descr == NULL) {
       perror("malloc for description");
       exit(1);
    }

    while(fscanf(config_file, "%s %[^\n]s", name, descr) != EOF){
        strcpy((*courselist_ptr)[curr].code, name);
        (*courselist_ptr)[curr].description = descr;
        (*courselist_ptr)[curr].head = NULL;
        (*courselist_ptr)[curr].tail = NULL;
        (*courselist_ptr)[curr].helped = 0;
        (*courselist_ptr)[curr].bailed = 0;
        (*courselist_ptr)[curr].wait_time = 0.0;
        (*courselist_ptr)[curr].help_time = 0.0;
        if (curr < num - 1)
        {
            descr = malloc(sizeof(char) * (INPUT_BUFFER_SIZE - 6));
            if (descr == NULL) {
                perror("malloc for description");
                exit(1);
            }
        }
        curr++;
    }
    
    if(fclose(config_file) != 0){
        perror("fclose");
        exit(1);
    }
    return num;
}
