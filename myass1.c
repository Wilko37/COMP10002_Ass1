/* Program to implement Australian House of Representatives preferential
voting and determine election outcomes from vote preference orderings.
Skeleton program written by Alistair Moffat, ammoffat@unimelb.edu.au,
August 2025, with the intention that it be modified by students
to add functionality, as required by the assignment specification.
All included code is (c) Copyright University of Melbourne, 2025
Student Authorship Declaration:
(1) I certify that except for the code provided in the initial skeleton
file, the program contained in this submission is completely my own
individual work, except where explicitly noted by further comments that
provide details otherwise. I understand that work that has been
developed by another student, or by me in collaboration with other
students, or by non-students as a result of request, solicitation, or
payment, may not be submitted for assessment in this subject. I
understand that submitting for assessment work developed by or in
collaboration with other students or non-students constitutes Academic
Misconduct, and may be penalized by mark deductions, or by other
penalties determined via the University of Melbourne Academic Honesty
Policy, as described at https://academicintegrity.unimelb.edu.au.
(2) I also certify that I have not provided a copy of this work in either
softcopy or hardcopy or any other form to any other student, and nor will
I do so until after the marks are released. I understand that providing
my work to other students, regardless of my intention or any undertakings
made to me by that other student, is also Academic Misconduct.
(3) I further understand that providing a copy of the assignment
specification to any form of code authoring or assignment tutoring
service, or drawing the attention of others to such services and code
that may have been made available via such a service, may be regarded as
Student General Misconduct (interfering with the teaching activities of
the University and/or inciting others to commit Academic Misconduct). I
understand that an allegation of Student General Misconduct may arise
regardless of whether or not I personally make use of such solutions or
sought benefit from such actions.
Signed by: [Lachlan Wilkinson, 1734480]
Dated: [11/09/2025]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <assert.h>

#define MAXCHARS 20
#define MAXVOTES 999
#define MAXCANDIDATES 20

#define STAGE1 1
#define STAGE2 2
#define STAGE3 3

/*************************************************************************/

typedef struct {
    char name[MAXCHARS+1]; /* Candidate name */
    int votes; /* Candidate's votes */
    int eliminated; /* Is Candidate eliminated? */
} Candidate_t;

/* Input + setup */
int getword(char W[], int limit);
void create_candidate_array(Candidate_t Candidates[], int num_candidates);
int create_votes_array(int cols, int votes[][MAXCANDIDATES]);

/* Stage 1 */
void print_heading(int stage);
void do_stage1(int stage, int *cols, int *rows, Candidate_t Candidates[],
               int votes[][MAXCANDIDATES]);
void print_votes_row(int num_cols, int rows[], Candidate_t Candidates[]);
int linear_search_for_vote(int row[], int rank, int num_cols);

/* Stage 2 + 3 core */
void do_stage2_and_3(int stage, int cols, int rows, 
                     Candidate_t Candidates[], int votes[][MAXCANDIDATES]);
void voting_loop(int stage, int cols, int rows, 
                 Candidate_t Candidates[], int votes[][MAXCANDIDATES]);
void run_round(int stage, int cols, int rows, int round,
               Candidate_t Candidates[], int votes[][MAXCANDIDATES],
               int *majority);

/* Round helpers */
void create_zero_votes(Candidate_t Candidates[], int n);
void distribute_votes(Candidate_t Candidates[], int votes[][MAXCANDIDATES],
                      int cols, int row);
int check_for_majority(int *majority, Candidate_t Candidates[], int cols, 
                        int rows);
int find_lowest_votes(Candidate_t Candidates[], int cols);
void eliminate_candidate(Candidate_t Candidates[], int cols);
int check_and_print_winner(int *majority, Candidate_t Candidates[],
                           int cols, int rows);
void count_votes(Candidate_t Candidates[], int votes[][MAXCANDIDATES],
                 int cols, int rows);

/* Printing helpers */
void print_voting_round(Candidate_t Candidates[], int cols, int rows, 
                        int round);
Candidate_t* prepare_for_print(int stage, int cols,
                             Candidate_t Candidates[], 
                             Candidate_t sorted[]);
void print_round_results(int stage, int cols, int rows, int round,
                         Candidate_t Candidates[]);

/* Sorting + copying */
void int_swap(int *p1, int *p2);
void str_swap(char s1[MAXCHARS+1], char s2[MAXCHARS+1]);
void sort_candidates(Candidate_t A[], int n);
void copy_candidates(int cols, Candidate_t from[], Candidate_t to[]);
void reset_candidates(int cols, Candidate_t source[], Candidate_t dest[]);

/*************************************************************************/

/* Main program provides traffic control */
int main(int argc, char *argv[]) {

    int cols, rows;
    Candidate_t candidates[MAXCANDIDATES];  
    /* Create a copy and a working array*/     
    Candidate_t master[MAXCANDIDATES];           
    Candidate_t working[MAXCANDIDATES];          
    int votes[MAXVOTES][MAXCANDIDATES];

    do_stage1(STAGE1, &cols, &rows, candidates, votes);

    /* Stage 2 */
    copy_candidates(cols, candidates, master);
    reset_candidates(cols, master, working);
    do_stage2_and_3(STAGE2, cols, rows, working, votes);

    /* Stage 2 */
    reset_candidates(cols, master, working);
    do_stage2_and_3(STAGE3, cols, rows, working, votes);

    printf("\ntadaa!\n");
    return 0;
}
/*************************************************************************/
/* Extract a single word out of the standard input, of not
   more than limit characters, argument array W must be
   limit+1 characters or bigger.
   function adapted from Figure 7.13 of Programming, Problem Solving,
   and Abstraction with C, by Alistair Moffat
*/
int getword(char W[], int limit) {
    int c, len=0;
    /* First, skip over any non alphabetics */
    while ((c=getchar())!=EOF && !isalpha(c)) {
        /* do nothing more */
    }
    if (c==EOF) {
        return EOF;
    }
    /* Ok, first character of next word has been found */
    W[len++] = c;
    while (len<limit && (c=getchar())!=EOF && isalpha(c)) {
        /* Another character to be stored */
        W[len++] = c;
    }
    /* Now close off the string */
    W[len] = '\0';
    return 0;
}
/*************************************************************************/
void print_heading(int stage){
    /* Add the weird extra line space after stage 2 */
    if (stage == 3){
        printf("\n");
    }
    printf("\nStage %d\n=======\n", stage);
}
/*************************************************************************/
/* The required stage 1 output:
    -- no. Candidates
    -- no. votes
    -- last row of votes
*/
void do_stage1(int stage, int *cols, int *rows, Candidate_t Candidates[],
               int votes[][MAXCANDIDATES]){
    print_heading(stage);

    /*Read number of columns from first num in doc*/
    scanf("%d", cols);

    /*Create an array for both Candidates and votes*/
    create_candidate_array(Candidates, *cols);
    *rows = create_votes_array(*cols, votes);
    
    printf("read %d candidates and %d votes\n", *cols, *rows);
    printf("voter %d preferences...\n", *rows);

    /*Print the last voters prefences*/
    print_votes_row(*cols, votes[*rows-1], Candidates);
    
}
/*************************************************************************/
/* Creates an array of votes
    -- rows are differnent voters
    -- columns correspond to Candidates
*/
int create_votes_array(int cols, int votes[][MAXCANDIDATES]){
    int value;
    int cur_col=0, cur_row=0;
    while (scanf("%d", &value) ==1) {
        votes[cur_row][cur_col++] = value;
        if (cur_col == cols) {
            /* If we get to the end of the row */
            cur_col = 0;
            cur_row++;
        }
    }
    return cur_row;
}
/*************************************************************************/
/* Creates an array of strings for each Candidate (column num becomes 
   row num)
*/
void create_candidate_array(Candidate_t Candidates[], int num_candidates) {
    for (int i=0; i<num_candidates; i++) {
        getword(Candidates[i].name, MAXCHARS);
        Candidates[i].votes = 0;
        Candidates[i].eliminated = 0;
    }
}
/*************************************************************************/
/* Finds and returns the index of a specific rank of vote in a specific 
   voter's row.
   (a linear search)
*/
int linear_search_for_vote(int row[], int rank, int num_cols){
    for (int i=0; i<num_cols; i++){
        if (row[i] == rank) {
            /* We found the vote! */
            return i;
        }
    }
    /* Fail-safe incase vote not found */
    return -1;
}
/*************************************************************************/
void print_votes_row(int num_cols, int row[], Candidate_t Candidates[]){
    for (int rank=1; rank<=num_cols; rank++){
        int index = linear_search_for_vote(row, rank, num_cols);
        printf("    rank %2d: %s\n", rank, Candidates[index].name);
    }
}
/*************************************************************************/
/* The required stage 2 output:
    -- Each Candidates corresponding votes and percentage of vote
    -- Repeat rounds by elimnating lowest voted
    -- Stop once cndaite has >= 50% of votes

    The required stage 3 output:
    -- Same as stage 2. however, now Candidates are orderded by number 
       of votes
*/
void do_stage2_and_3(int stage, int cols, int rows, 
                     Candidate_t Candidates[], int votes[][MAXCANDIDATES]){

    print_heading(stage);
    voting_loop(stage, cols, rows, Candidates, votes);
    
}
/*************************************************************************/
/* Sets all candiates votes to 0
*/
void create_zero_votes(Candidate_t Candidates[], int n){
    for (int i=0; i<n; i++){
        Candidates[i].votes = 0;
    }
}
/*************************************************************************/
/* Checks one row of votes and assigns vote according to highets ranked 
   Candidate still remaining.
*/
void distribute_votes(Candidate_t Candidates[], int votes[][MAXCANDIDATES],
                      int cols, int row){
    int index;
    for (int rank=1; rank<cols+1; rank++){
        /* Increase rank of vote we are searching for untill we find a 
        Candidate who hasn't been eliminated*/
         index = linear_search_for_vote(votes[row], rank, cols);

        if (!Candidates[index].eliminated){
            /* If not elimnated, add 1 to tally of the Candidate*/
            Candidates[index].votes++;
            break;
        }
    }
}
/*************************************************************************/
int check_for_majority(int *majority, Candidate_t Candidates[], int cols, 
                        int rows){
    for (int i=0; i<cols; i++){
        if (Candidates[i].votes > rows/2){
            /* Yep majority has been reached */
            *majority = 1;
            return i;
        }
    }
    return -1; /* No winner */
}
/*************************************************************************/
void print_voting_round(Candidate_t Candidates[], int cols, int rows,
                        int round){
    printf("round %d...\n", round);
    
    for (int i=0; i<cols; i++){
        if (!Candidates[i].eliminated){
            /* Ensures only Candidates still in are printed*/
            double percentage = 100 * Candidates[i].votes / (double)rows;
            printf("    %-20s:%4d votes, %5.1lf%%\n", Candidates[i].name, 
                Candidates[i].votes, percentage);
        }
    }

    printf("    ----\n");
}
/*************************************************************************/
int find_lowest_votes(Candidate_t Candidates[], int cols){
    int index = -1;

    for (int i=0; i<cols; i++){
        if (!Candidates[i].eliminated){
            if (index == -1){
                /* Sets first active Candidate as inital index */
                index = i; 
            }
            else if (Candidates[i].votes < Candidates[index].votes){
                /* If there is a tie, just take the first Candidate 
                   found */
                index = i;
            }
        }
    }

    return index;
}
/*************************************************************************/ 
/* Function adapted from Figure 6.7 of Programming, Problem Solving,
   and Abstraction with C, by Alistair Moffat */
void int_swap(int *p1, int *p2) {
    int tmp;
    tmp = *p1;
    *p1 = *p2;
    *p2 = tmp;
}
/*************************************************************************/
void str_swap(char s1[MAXCHARS+1], char s2[MAXCHARS+1]) {
    char tmp[MAXCHARS+1];
    strcpy(tmp, s1);
    strcpy(s1, s2);
    strcpy(s2, tmp);
}
/*************************************************************************/
/* Function adapted from Figure 7.3 of Programming, Problem Solving,
   and Abstraction with C, by Alistair Moffat */
void sort_candidates(Candidate_t A[], int n) {
    int i, j;
    for (i=1; i<n; i++) {
        for (j=i-1; j>=0; j--) {
            /* Sort alphabetically if tied */
            if (A[j+1].votes > A[j].votes || 
               (A[j+1].votes == A[j].votes && 
                strcmp(A[j+1].name, A[j].name) < 0)) {
                /* Swap fields individually */
                int_swap(&A[j].votes, &A[j+1].votes);
                str_swap(A[j].name, A[j+1].name);
                int_swap(&A[j].eliminated, &A[j+1].eliminated);
            }
            else {
                break; /* Correct position */
            }
        }
    }
}
/*************************************************************************/
/* Copy Candidates array field by field */
void copy_candidates(int cols, Candidate_t from[], Candidate_t to[]) {
    for (int i=0; i<cols; i++) {
        strcpy(to[i].name, from[i].name); /* Copy Candidate name */
        to[i].votes = from[i].votes; /* Copy vote count */
        to[i].eliminated = from[i].eliminated; /* Copy elimination status*/
    }
}
/*************************************************************************/
/* Prints out each stage of the election, elinmanting the fewest votes at 
   the end of each round.
   If it's stage 3, than order Candidates before printing.
*/
void voting_loop(int stage, int cols, int rows, 
                     Candidate_t Candidates[], int votes[][MAXCANDIDATES]){

    int majority = 0; /* Set to 1 if a majority has been reached */
    int round = 1;
    while (!majority){
        run_round(stage, cols, rows, round, Candidates, votes, &majority);
        round ++;
    }
}
/*************************************************************************/
/* Execute a single round of voting */
void run_round(int stage, int cols, int rows, int round,
               Candidate_t Candidates[], int votes[][MAXCANDIDATES],
               int *majority) {
    count_votes(Candidates, votes, cols, rows);
    print_round_results(stage, cols, rows, round, Candidates);

    if (!check_and_print_winner(majority, Candidates, cols, rows)) {
        eliminate_candidate(Candidates, cols);
    }
}
/*************************************************************************/
Candidate_t* prepare_for_print(int stage, int cols,
                             Candidate_t Candidates[], 
                             Candidate_t sorted[]) {
    if (stage == 3) {
        /* Use sorted copies */
        copy_candidates(cols, Candidates, sorted);
        sort_candidates(sorted, cols);
        return sorted;   
    }
    /* Use originals */
    return Candidates; 
}
/*************************************************************************/
/* Reset votes and redistribute based on current preferences */
void count_votes(Candidate_t Candidates[], int votes[][MAXCANDIDATES],
                 int cols, int rows) {
    create_zero_votes(Candidates, cols);
    for (int i=0; i<rows; i++) {
        distribute_votes(Candidates, votes, cols, i);
    }
}
/*************************************************************************/
/* Print round results, with Candidates sorted if stage 3 */
void print_round_results(int stage, int cols, int rows, int round,
                         Candidate_t Candidates[]) {
    Candidate_t sorted[MAXCANDIDATES];
    Candidate_t *to_print = prepare_for_print(stage, cols, Candidates, 
        sorted);
    print_voting_round(to_print, cols, rows, round);
}
/*************************************************************************/
/* Eliminate the Candidate with the fewest votes */
void eliminate_candidate(Candidate_t Candidates[], int cols) {
    int index = find_lowest_votes(Candidates, cols);
    printf("    %s is eliminated and votes distributed\n\n",
            Candidates[index].name);
    Candidates[index].eliminated = 1;
}
/*************************************************************************/
/* Check if majority has been reached, and print winner if so */
int check_and_print_winner(int *majority, Candidate_t Candidates[],
                           int cols, int rows) {
    int winner = check_for_majority(majority, Candidates, cols, rows);
    if (*majority) {
        printf("    %s is declared elected\n", Candidates[winner].name);
        return 1;
    }
    return 0;
}
/*************************************************************************/
/* Reset candidate state for a new stage */
void reset_candidates(int cols, Candidate_t source[], Candidate_t dest[]) {
    copy_candidates(cols, source, dest);
    for (int i = 0; i < cols; i++) {
        dest[i].votes = 0;
        dest[i].eliminated = 0;
    }
}
/*************************************************************************/
/* algorithms are fun! */