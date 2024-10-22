#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "autocomplete.h"

int cmpweightfunc(const void *a, const void *b){
    term *answer_a = (term *)a;
    term *answer_b = (term *)b;
    if (answer_a->weight > answer_b->weight){
        return -1;
    }
    if (answer_a->weight < answer_b->weight){
        return 1;
    }
    else{
        return 0;
    }
}

int cmpfunc(const void *a, const void *b){ 
    term *term_a = (term *)a;
    term *term_b = (term *)b;
    return strcmp(term_a->term, term_b->term);
}

void format_string(char str[]) {
    int i = 0;
    // Find the index of the first non-whitespace character
    while (str[i] == ' '){
        i++;
    }
    for (int n = i; n < strlen(str); n++){ //we can start at i to make it faster
        if (str[n] == '\n'){
            str[n] = '\0'; //change newline to null
        }
    }

    // Shift the non-whitespace characters to the beginning of the string
    memmove(str, str + i, strlen(str) - i + 1);
}

int my_atoi (char *num_as_str){
    int res = 0;
    while (*num_as_str >= '0' && *num_as_str <= '9'){
        res = (res * 10) + (*num_as_str - '0');
        num_as_str++; //if char isnt a digit, skip one char. 
    }
    return res;
}

void read_in_terms(term **terms, int *pnterms, char *filename){
    FILE *f = fopen(filename, "r");
    if (f == NULL){
        printf("File not found\n");
        exit(1);
    }
    char line[200]; 
    fgets(line, sizeof(line), f);
    //line now has the string representation of num of terms 
    //need to convert line to an integer and reference it with pnterms 
    int num = my_atoi(line);
    *pnterms = num;
    *terms = (term *)malloc(num * sizeof(term));
    if (*terms == NULL){
            printf("Malloc failed to allocate mem for terms \n");
            exit(1);
        }
   
    int cur_term = 0;
    while (fgets(line, sizeof(line), f)){
        //get rid of empty space in the begining of each line
        format_string(line);
        char *token = strtok(line, "\t\n");
        if (token != NULL){
            double current_weight = my_atoi(token);
            (*terms + cur_term)->weight = current_weight;
            token = strtok(NULL, "\t");
            char *current_term = token;
            strcpy((*terms + cur_term)->term, current_term);
      
            }
        cur_term++;
        }


        
    //sort terms
     qsort(*terms, num, sizeof(term), cmpfunc);
    // FOR TESTING FUNCTION 
    //  for (int m = 82641; m < 82819; m++){
    //     printf("For term no. %d\n", m);
    //     printf("The weight: %lf    ", (*terms+m)->weight);
    //     printf("The term: %s\n", (*terms+m)->term);
    // }
    fclose(f);
    }

int lowest_match(term *terms, int nterms, char *substr){
    //since we want function to run in Olog(n), use binary search algorithim
    int low = 0;
    int high = nterms - 1;

    while (low <= high){
        int mid = low + (high-low)/2;
        if (strncmp((terms+mid)->term, substr, strlen(substr)) < 0){
             //if return - then the current term is before the one we are looking for,
             // move up
            low = mid + 1;
        }
        else{
            //the current term is after the one we are looking for, move down
            high = mid - 1;
        }
    }
    return low;
}

int highest_match(term *terms, int nterms, char *substr){
    //since we want function to run in Olog(n), use binary search algorithim
    int low = 0;
    int high = nterms - 1;

    while (low <= high){
        int mid = low + (high-low)/2;
        if (strncmp((terms+mid)->term, substr, strlen(substr)) > 0){
             //if + then the current term is after the one we are looking for,
             // consider the half before this term
            high = mid - 1;
        }
        else{
            //the current term is after the one we are looking for, move down
            low = mid + 1;
        }
    }
    return high;
}

void autocomplete(term **answer, int *n_answer, term *terms, int nterms, char *substr){
    //first find the bounds, the section of terms that matches substr
    int low = lowest_match(terms, nterms, substr);
    int high = highest_match(terms, nterms, substr);

    //set number of total matches 
    *n_answer = (high - low + 1);
    //allocate mem for answer pointer, its alr been declared as term** no need to cast
    *answer = (term *)malloc((*n_answer) * sizeof(term));
    if (*answer == NULL){
            printf("Malloc failed to allocate mem for ans\n");
            exit(1);
        }

    
    //put terms and weights were they belong.
    for (int i = low; i <= high; i++){
        (*answer - low + i)->weight = (terms + i)->weight; 
        strcpy((*answer - low + i)->term, (terms + i)->term);
    }

    qsort(*answer, *n_answer, sizeof(term), cmpweightfunc);
    // for (int m = 0; m < *n_answer; m++){
        
    //     printf("For term no. %d\n", m);
    //     printf("The weight: %lf    ", (*answer+m)->weight);
    //     printf("The term: %s\n", (*answer+m)->term);
    // }
}


// void main()
// {
//     struct term *terms;
//     int nterms;
//     read_in_terms(&terms, &nterms, "cities.txt");
//     lowest_match(terms, nterms, "Tor");
//     highest_match(terms, nterms, "Tor");
    
//     struct term *answer;
//     int n_answer;
//     autocomplete(&answer, &n_answer, terms, nterms, "Tor");
//     printf("%s\n", answer->term);
// }