#ifndef HCQ_H
#define HCQ_H
#define BUF_SIZE 60

/* Students are kept in order by time with newest 
   students at the end of the lists. */
struct student{
    char *name;
    struct course *course;
    struct student *next_overall;
};

/* Tas are kept in reverse order of their time of addition. Newest
   Tas are kept at the head of the list. */ 
struct ta{
    char *name;
    struct student *current_student;
    struct ta *next;
};

struct course{
    char code[7];
};

struct sockname {
    char buf[BUF_SIZE];
    int inbuf;           // How many bytes currently in buffer?
    int room;  // How many bytes remaining in buffer?
    char *after;       // Pointer to position after the data in buf
    int sock_fd;
    char *username;
    int ista;//0 -> student, 1 -> ta
    struct sockname *next;
    int state;//0 -> waiting for the name, 1 -> waiting for the role, 2 -> waiting for the course, 3 -> waiting for the commands, -1 -> to be closed
};

typedef struct sockname Sockname;
typedef struct student Student;
typedef struct course Course;
typedef struct ta Ta;

// helper functions not directly related to only one command in the API
Student *find_student(Student *stu_list, char *student_name);
Ta *find_ta(Ta *ta_list, char *ta_name);

// functions provided as the API to a help-centre queue
int add_student(Student **stu_list_ptr, char *student_name, char *course_num,
    Course *courses, int num_courses);
int give_up_waiting(Student **stu_list_ptr, char *student_name);

void add_ta(Ta **ta_list_ptr, char *ta_name);
int remove_ta(Ta **ta_list_ptr, char *ta_name);
void release_current_student(Ta *ta);

//  if student is currently being served then this finishes this student
//    if there is no-one else waiting then the currently being served gets
//    set to null 
Student *next_overall(char *ta_name, Ta **ta_list_ptr, Student **stu_list_ptr);

// list currently being served by current TAs
char* print_currently_serving(Ta *ta_list);

// list all students in queue 
char* print_full_queue(Student *stu_list);

#endif
