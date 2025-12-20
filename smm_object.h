//
//  smm_object.h
//  SMMarble object
//
//  Created by Juyeop Kim on 2023/11/05.
//

#ifndef smm_object_h
#define smm_object_h

/* node type :
    lecture,
    restaurant,
    laboratory,
    home,
    experiment,
    foodChance,
    festival
*/
#define SMMNODE_TYPE_LECTURE                0
#define SMMNODE_TYPE_RESTAURANT             1
#define SMMNODE_TYPE_LABORATORY             2
#define SMMNODE_TYPE_HOME                   3
#define SMMNODE_TYPE_GOTOLAB                4
#define SMMNODE_TYPE_FOODCHANGE             5
#define SMMNODE_TYPE_FESTIVAL               6

#define SMMNODE_OBJTYPE_BOARD     0
#define SMMNODE_OBJTYPE_GRADE     1
#define SMMNODE_OBJTYPE_FOOD      2
#define SMMNODE_OBJTYPE_FEST      3

/* grade :
    A+,
    A0,
    A-,
    B+,
    B0,
    B-,
    C+,
    C0,
    C-
*/
#define SMMNODE_MAX_GRADE         9

typedef enum {
    GRADE_APLUS  = 0,
    GRADE_A0      = 1,
    GRADE_AMINUS = 2,
    GRADE_BPLUS  = 3,
    GRADE_B0      = 4,
    GRADE_BMINUS = 5,
    GRADE_CPLUS  = 6,
    GRADE_C0      = 7,
    GRADE_CMINUS = 8
} smmGrade_e;
// enum으로 grade관리

//object generation
void* smmObj_genObject(char* name, int objType, int type, int credit, int energy, int grade);

char* smmObj_getObjectName(void *ptr);
int smmObj_getObjectGrade(void *ptr);
int smmObj_getObjectEnergy(void *ptr);
int smmObj_getObjectCredit(void *ptr);
int smmObj_getObjectType(void *ptr);
/*
char* smmObj_getNodeName(int node_nr);
int smmObj_getNodeType(int node_nr);
int smmObj_getNodeCredit(int node_nr);
int smmObj_getNodeEnergy(int node_nr);
int smmObj_getNodeGrade(int node_nr);*/
//member retrieving


//element to string
char* smmObj_getTypeName(int node_type);
char* smmObj_getGradeName(smmGrade_e grade);


#endif /* smm_object_h */
