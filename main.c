//
//  main.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include "smm_common.h"
#include "smm_database.h"
#include "smm_object.h"
#include <string.h>
#include <time.h>

#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH "marbleFoodConfig.txt"
#define FESTFILEPATH "marbleFestConfig.txt"

// 각 구조체 노드 개수
static int smm_board_nr;
static int smm_food_nr;
static int smm_festival_nr;
static int smm_player_nr;

// 플레이터 정보의 각 노드( 구조체 )
typedef struct {
  char name[MAX_CHARNAME]; // 이름
  int pos; // 위치
  int credit; // 내 전체 학점
  int energy; // 내 전체 에너지
  int flag_graduated;  // 내 졸업 상태
  int flag_experiment;     // 0/1 : 실험중 상태
  int exp_success_crit;    // 실험 성공 기준값(1~MAX_DIE)
} smm_player_t;

// 플레이어 정보 담는 배열을 가르키는 포인터
smm_player_t *smm_players;

void generatePlayers(int n, int initEnergy); // 새로운 플레이어 생성
void printPlayerStatus(void); // 플레이어 상태를 턴 마다 출력하기

// function prototypes
#if 1
void printGrades(int player);       // 지금까지 수강한 과목 출력하기
float calcAverageGrade(int player); // calculate average grade of the player
smmGrade_e takeLecture(int player, char *lectureName, int credit); // take the lecture (insert a grade of the player) 
//void printGrades(int player); //print all the grade history of the player
#endif

static int findFirstNodeOfType(int nodeType) { // 실험실을 찾기위한 함수
  for (int i = 0; i < smm_board_nr; i++) {
    if (smmObj_getObjectType(smmdb_getData(LISTNO_NODE,i)) == nodeType) return i;
  }
  return -1;
}

void *findGrade(int player, char *lectureName) // 해당과목 수강여부 확인
{
  int size = smmdb_len(LISTNO_OFFSET_GRADE + player);
  int i;

  for (i = 0; i < size; i++) {
    void *ptr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
    if (strcmp(smmObj_getObjectName(ptr), lectureName) == 0) {
      return ptr;
    }
  }

  return NULL;
}

void printGrades(int player) { // 플레이어가 수강한 과목 출력
  int size = smmdb_len(LISTNO_OFFSET_GRADE + player);

  printf("%s's Grades\n", smm_players[player].name);

  int i;
  for (i = 0; i < size; i++) {
    void *ptr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
    printf("%2d) %-20s  %2d  %s\n", \
       i + 1, \
       smmObj_getObjectName(ptr), \
       smmObj_getObjectCredit(ptr), \
       smmObj_getGradeName((smmGrade_e)smmObj_getObjectGrade(ptr)));
  }
}

int isGraduated(void) // 졸업 여부 출력
{
  int i;
  for (i = 0; i < smm_player_nr; i++) {
    if (smm_players[i].flag_graduated == 1){
      // 1일때 출력
      printGrades(i);
      return 1;
    }
  }

  return 0;
}

void goForward(int player, int step) { // make player go "step" steps on the
                                       // board (check if player is graduated)
  int i;
  void *ptr;

  // player_pos[player] = player_pos[player]+ step;
  ptr = smmdb_getData(LISTNO_NODE, smm_players[player].pos);
  printf("start from %i(%s) (%i)\n", smm_players[player].pos,
         smmObj_getObjectName(ptr), step);
  for (i = 0; i < step; i++) {
    smm_players[player].pos = (smm_players[player].pos + 1) % smm_board_nr;
    ptr = smmdb_getData(LISTNO_NODE, smm_players[player].pos);
    if(smm_players[player].pos == 0){
      smm_players[player].energy += smmObj_getObjectEnergy(ptr);
    }
    printf("  => moved to %i(%s)\n", smm_players[player].pos,
           smmObj_getObjectName(ptr));
  }
}

void printPlayerStatus(void) {
  int i;
  for (i = 0; i < smm_player_nr; i++) {
    void* ptr = smmdb_getData(LISTNO_NODE, smm_players[i].pos);
    printf("%s - position:%i(%s), credit:%i, energy:%i\n", smm_players[i].name, \
           smm_players[i].pos, smmObj_getTypeName(smmObj_getObjectType(ptr)), \
           smm_players[i].credit, smm_players[i].energy);
  }
}

void generatePlayers(int n, int initEnergy) // 새 플레이어 생성
{
  int i;

  smm_players = (smm_player_t *)malloc(n * sizeof(smm_player_t));

  for (i = 0; i < n; i++) {
    smm_players[i].pos = 0;
    smm_players[i].credit = 0;
    smm_players[i].energy = initEnergy;
    smm_players[i].flag_graduated = 0;
    smm_players[i].flag_experiment = 0;
    smm_players[i].exp_success_crit = 0;


    printf("Input %i-th player name:", i + 1);
    scanf("%s", &smm_players[i].name[0]);
    fflush(stdin);
  }
}

int rolldie(int player) {
  char c;
  printf(" Press any key to roll a die (press g to see grade): ");
  c = getchar();
  //fflush(stdin);

#if 1
  if (c == 'g')
    printGrades(player);
#endif

  while (c != '\n' && c != EOF) c = getchar();

  return (rand() % MAX_DIE + 1);
}

// action code when a player stays at a node
void actionNode(int player) {
  void *ptr = smmdb_getData(LISTNO_NODE, smm_players[player].pos);

  int type = smmObj_getObjectType(ptr);
  int credit = smmObj_getObjectCredit(ptr);
  int energy = smmObj_getObjectEnergy(ptr);
  int grade;
  void *gradePtr;

  printf(" --> player %i (%s) pos :%i, type : %s, credit : %i, energy : %i\n", player + 1, smm_players[player].name \
        , smm_players[player].pos, smmObj_getTypeName(type), credit, energy);

  switch (type) {
  case SMMNODE_TYPE_LECTURE:
    if (findGrade(player, smmObj_getObjectName(ptr)) == NULL) {
      smm_players[player].credit += credit;
      smm_players[player].energy -= energy;

      grade = rand() % SMMNODE_MAX_GRADE;

      printf(" [Lecture] %s takes '%s' (+%d credit, -%d energy) => grade %s\n", \
               smm_players[player].name, \
               smmObj_getObjectName(ptr), \
               credit, \
               energy, \
               smmObj_getGradeName((smmGrade_e)grade));
               
      gradePtr =
          smmObj_genObject(smmObj_getObjectName(ptr), SMMNODE_OBJTYPE_GRADE,
                           type, credit, energy, grade);
      smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
    } else {
      // 강의를 이미 수강한 경우 들었다고 출력
        printf(" [Lecture] %s already took '%s' (skip)\n", \
               smm_players[player].name, smmObj_getObjectName(ptr));
    }
    break;

  case SMMNODE_TYPE_RESTAURANT:
    smm_players[player].energy += energy;
    break;

  case SMMNODE_TYPE_LABORATORY:{
    // 실험중 상태 아닐때 차단
    if (smm_players[player].flag_experiment == 0) {
      printf(" [Lab] (not in experiment) nothing happens.\n");
      break;
    }

    // 실험 시도마다 에너지 소모
    smm_players[player].energy -= energy;

    int die = (rand() % MAX_DIE) + 1;
    int crit = smm_players[player].exp_success_crit;

    printf(" [Lab] try experiment: die=%d, criterion=%d, energy-%d -> now %d\n",
          die, crit, energy, smm_players[player].energy);

    // 기준값 이상이면 실험 종료, 아니면 실험중 유지
    if (die >= crit) {
      smm_players[player].flag_experiment = 0;
      smm_players[player].exp_success_crit = 0;
      printf(" [Lab] SUCCESS! experiment finished.\n");
    } else {
      printf(" [Lab] FAIL... stay in laboratory (cannot move)\n");
      // flag_experiment 유지, pos도 그대로(이미 실험실에 있음)
    }
    break;
  }

  case SMMNODE_TYPE_HOME:
    // 이동 후 졸업했는지 확인
    if (smm_players[player].credit >= GRADUATE_CREDIT) {
      smm_players[player].flag_graduated = 1;
    }
    break;

  case SMMNODE_TYPE_GOTOLAB:
  {
    // 실험중 상태로 전환하고 성공기준값 랜덤 지정하고 실험실로 이동함
    int labPos = findFirstNodeOfType(SMMNODE_TYPE_LABORATORY);

    smm_players[player].flag_experiment = 1;
    smm_players[player].exp_success_crit = (rand() % MAX_DIE) + 1;

    printf(" [GoToLab] experiment ON, success criterion = %d\n",
          smm_players[player].exp_success_crit);

    if (labPos >= 0) {
      printf(" [GoToLab] move to laboratory node %d\n", labPos);
      smm_players[player].pos = labPos;
    } else {
      printf(" [GoToLab][WARN] no laboratory node found on board!\n");
    }
    break;
  }

    case SMMNODE_TYPE_FOODCHANGE:{
    // 음식카드 1장 랜덤으로 뽑아서 에너지 보충
    int nFood = smmdb_len(LISTNO_FOODCARD);
    if (nFood <= 0) {
      printf(" [FoodChance][WARN] no food cards!\n");
      break;
    }

    int idx = rand() % nFood;
    void *foodPtr = smmdb_getData(LISTNO_FOODCARD, idx);

    int gain = smmObj_getObjectEnergy(foodPtr);
    smm_players[player].energy += gain;

    printf(" [FoodChance] drew '%s' (+%d energy) -> energy=%d\n",
          smmObj_getObjectName(foodPtr), gain, smm_players[player].energy);
    break;
  }

  case SMMNODE_TYPE_FESTIVAL:{
      int nFest = smmdb_len(LISTNO_FESTCARD);
      if (nFest <= 0) {
          printf(" [Festival] no festival cards.\n");
          break;
      }

      int idx = rand() % nFest;
      void *festPtr = smmdb_getData(LISTNO_FESTCARD, idx);
      if (!festPtr) {
          printf(" [Festival] failed to get festival card.\n");
          break;
      }

      const char *mission = smmObj_getObjectName(festPtr);
      printf(" [Festival] mission card: %s\n", mission ? mission : "(null)");
      break;
  }

  default:
    break;
  }
}

int main(int argc, const char *argv[]) {

  FILE *fp;
  char name[MAX_CHARNAME];
  int type;
  int credit;
  int energy;
  int turn;

  smm_board_nr = 0;
  smm_food_nr = 0;
  smm_festival_nr = 0;

  srand(time(NULL));

  // 1. import parameters
  // ---------------------------------------------------------------------------------
  // 1-1. boardConfig
  if ((fp = fopen(BOARDFILEPATH, "r")) == NULL) {
    printf("[ERROR] failed to open %s. This file should be in the same "
           "directory of SMMarble.exe.\n",
           BOARDFILEPATH);
    getchar();
    return -1;
  }

  printf("Reading board component......\n");
  while (fscanf(fp, "%s %i %i %i", name, &type, &credit, &energy) == 4) // read a node parameter set
  {
    // store the parameter set
    void *ptr;
    printf("%s %i %i %i\n", name, type, credit, energy);
    ptr = smmObj_genObject(name, SMMNODE_OBJTYPE_BOARD, type, credit, energy, 0);
    smm_board_nr += smmdb_addTail(LISTNO_NODE, ptr) + 1;
    // 오류 의심 : 성공일때 반환 값이 0 이므로 +1을 해야, 총 갯수 나옴
  }
  fclose(fp);
  printf("Total number of board nodes : %i\n", smm_board_nr);

#if 1
  // 2. food card config
  if ((fp = fopen(FOODFILEPATH, "r")) == NULL) {
    printf("[ERROR] failed to open %s. This file should be in the same "
           "directory of SMMarble.exe.\n",
           FOODFILEPATH);
    return -1;
  }

  printf("\n\nReading food card component......\n");
  while (fscanf(fp, "%s %i", name, &energy) == 2) // read a food parameter set
  {
    // store the parameter set
    void *ptr;
    printf("%s %i\n", name, energy);
    ptr = smmObj_genObject(name, SMMNODE_OBJTYPE_FOOD, 0, 0, energy, 0);
    smm_food_nr += smmdb_addTail(LISTNO_FOODCARD, ptr) + 1;
  }
  fclose(fp);
  printf("Total number of food cards : %i\n", smm_food_nr);

  // 3. festival card config
  if ((fp = fopen(FESTFILEPATH, "r")) == NULL) {
    printf("[ERROR] failed to open %s. This file should be in the same "
           "directory of SMMarble.exe.\n",
           FESTFILEPATH);
    return -1;
  }

  printf("\n\nReading festival card component......\n");
  while (fscanf(fp, "%s", name) == 1) // read a festival card string
  {
    // store the parameter set
    void *ptr;
    printf("%s\n", name);
    ptr = smmObj_genObject(name, SMMNODE_OBJTYPE_FEST, 0, 0, 0, 0);
    smm_festival_nr += smmdb_addTail(LISTNO_FESTCARD, ptr) + 1;
  }
  fclose(fp);
  printf("Total number of festival cards : %i\n", smm_festival_nr);

#endif

  // 2. Player configuration
  // ---------------------------------------------------------------------------------

  do {
    // input player number to player_nr
    printf("Input player number:");
    scanf("%i", &smm_player_nr);
    fflush(stdin);

    if (smm_player_nr <= 0 || smm_player_nr > MAX_PLAYER)
      printf("Invalid player number!\n");
  } while (smm_player_nr <= 0 || smm_player_nr > MAX_PLAYER);

  generatePlayers(smm_player_nr, smmObj_getObjectEnergy(
                                     smmdb_getData(SMMNODE_OBJTYPE_BOARD, 0)));

  turn = 0;
  // 3. SM Marble game starts
  // ---------------------------------------------------------------------------------
  while (isGraduated() == 0) // is anybody graduated?
  {
    int die_result;

    // 4-1. initial printing
    printPlayerStatus();

    // 실험중일 경우 움직이는 거 건너 뛰기
    if(!(smm_players[turn].flag_experiment && smmObj_getObjectType(smmdb_getData(LISTNO_NODE, smm_players[turn].pos)) == SMMNODE_TYPE_LABORATORY)){
      // 4-2. die rolling (if not in experiment)
      die_result = rolldie(turn);

      // 4-3. go forward
      goForward(turn, die_result);
      // pos = pos + 2;
    }

    // 4-4. take action at the destination node of the board
    actionNode(turn);

    // 4-5. next turn
    turn = (turn + 1) % smm_player_nr;
  }

  free(smm_players);

  //system("PAUSE");
  return 0;
}
