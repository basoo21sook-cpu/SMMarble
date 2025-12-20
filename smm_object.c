//
//  smm_node.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include "smm_common.h"
#include "smm_object.h"
#include <string.h>

#define MAX_NODENR        100
#define MAX_NODETYPE      7
//#define MAX_GRADE 13


#if 0
#define GRADE_A+          0
#define GRADE_A0          1
#define GRADE_A-          2
#define GRADE_B+          3
#define GRADE_B0          4
#define GRADE_B-          5
#define GRADE_C+          6
#define GRADE_C0          7
#define GRADE_C-          8
#define GRADE_D+          9
#define GRADE_D0          10
#define GRADE_D-          11
#define GRADE_F           12
#endif

typedef enum {
    GRADE_APLUS  = 0,
    GRADE_A0      = 1,
    GRADE_AMINUS = 2,
    GRADE_BPLUS  = 3,
    GRADE_B0      = 4,
    GRADE_BMINUS = 5,
    GRADE_CPLUS  = 6,
    GRADE_C0      = 7,
    GRADE_CMINUS = 8,
    GRADE_DPLUS  = 9,
    GRADE_D0      = 10,
    GRADE_DMINUS = 11,
    GRADE_F       = 12,
    GRADE_MAX     = 13
} smmGrade_e;

static char smmObj_nodeName[MAX_NODETYPE][MAX_CHARNAME] = {
       "lecture",
       "restaurant",
       "laboratory",
       "home",
       "gotoLab",
       "foodChance",
       "festival"
};

static char smmObj_gradeName[SMMNODE_MAX_GRADE][MAX_CHARNAME] = {
       "A+",
       "A0",
       "A-",
       "B+",
       "B0",
       "B-",
       "C+",
       "C0",
       "C-",
       "D+",
       "D0",
       "D-",
       "F"
};

//structure type definition
typedef struct {
    char name[MAX_CHARNAME];
    int objType;
    int type;
    int credit;
    int energy;
    int grade;
} smmObj_object_t;

static smmObj_object_t *smmObj_board;


//object generation
void* smmObj_genObject(char* name, int objType, int type, int credit, int energy, int grade)
{
    smmObj_object_t* ptr;
    ptr = (smmObj_object_t*)malloc(sizeof(smmObj_object_t));
    
    strcpy(ptr->name, name);
    ptr->type = type;
    ptr->objType = objType;
    ptr->credit = credit;
    ptr->energy = energy;
    ptr->grade = grade;
    
    return ((void*)ptr);
}



//member retrieving
char* smmObj_getObjectName(void *ptr)
{
      smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
      
      return (objPtr->name);
}

int smmObj_getObjectType(int node_nr)
{
      return (smmObj_board[node_nr].type);
}

int smmObj_getObjectCredit(int node_nr)
{
      return (smmObj_board[node_nr].credit);
}

int smmObj_getObjectEnergy(void *ptr)
{
      smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
      
      return (objPtr->energy);
}

char* smmObj_getTypeName(int node_type)
{
      return (smmObj_nodeName[node_type]);
}


#if 1
char* smmObj_getNodeName(int player)
{
    return smmObj_nodeName[player];
}
char* smmObj_getGradeName(smmGrade_e grade)
{
    return smmObj_gradeName[grade];
}
#endif

