//
//  main.c
//  Map-Reduce
//
//  Created by Tushar Sharma on 10/4/18.
//  Copyright © 2018 Tushar Sharma. All rights reserved.
//

#include <stdio.h>

int main(int argc, const char * argv[]) {
    // insert code here...
    //printf("Hello, World!\n");
    
    if (argc!= 13){
        printf("Invalid number of inputs");
        return 1;
    }
    int type; //[0] for wordcount [1] sort
    int impl; //[0] for procs [1] thread
    int maps; // num of maps
    int reduces; // num of reduces
    char inFile[250]; // input file
    char outFile[250]; // output file
    
    if(strcmp(argv[2],"wordcount") == 0){
        type = 0;
    }else{
        type = 1;
    }
    
    if(strcmp(argv[4],"procs") == 0){
        impl = 0;
    }else{
        impl = 1;
    }
    
    maps = atoi(argv[6]);
    reduces = atoi(argv[8]);
    strcpy(inFile, argv[10]);
    strcpy(outFile, argv[12]);
    
    

    
    

    
    
    return 0;
    
   }
