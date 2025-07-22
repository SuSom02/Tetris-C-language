#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <time.h>

/* 타이머  */
#define CCHAR 0
#ifdef CTIME
#undef CTIME
#endif
#define CTIME 1

/* 왼쪽, 오른쪽, 아래, 회전  */
#define LEFT 0
#define RIGHT 1
#define DOWN 2
#define ROTATE 3


/* 블록 모양 */
#define I_BLOCK 0
#define	T_BLOCK 1
#define S_BLOCK 2
#define Z_BLOCK 3


#define L_BLOCK 4
#define J_BLOCK 5
#define O_BLOCK 6

/* 게임 시작, 게임 종료 */
#define GAME_START 0
#define GAME_END 1

/*

 * 블록 모양(I, T, S, Z, L, J, O) 
 * 4*4 배열의 2차원 배열
 * 모든 블록의 모양을 표시
 *
 * 블록의 모양을 표시
 * 왼쪽, 오른쪽, 아래, 회전 
 * 4*4 배열의 2차원 배열
 * 모든 블록의 모양을 표시
 *
 * 4*4*4 배열의 3차원 배열
 */


char i_block[4][4][4] = {
    {  // 첫 번째 회전 상태
        {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} },
    {  // 두 번째 회전 상태
        {0, 0, 0, 1}, {0, 0, 0, 1}, {0, 0, 0, 1}, {0, 0, 0, 1} },
    {  // 세 번째 회전 상태
        {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 1} },
    {  // 네 번째 회전 상태
        {1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0} }
};

char t_block[4][4][4] =
	{
			{{1, 0, 0, 0},   {1, 1, 0, 0},   {1, 0, 0, 0},   {0, 0, 0, 0}},
			{{1, 1, 1, 0},   {0, 1, 0, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}},
			{{0, 0, 1, 0},   {0, 1, 1, 0},   {0, 0, 1, 0},   {0, 0, 0, 0}},
			{{0, 0, 0, 0},   {0, 1, 0, 0},   {1, 1, 1, 0},   {0, 0, 0, 0}}
	};


char s_block[4][4][4] =
	{
			{{1, 0, 0, 0},   {1, 1, 0, 0},   {0, 1, 0, 0},   {0, 0, 0, 0}},
			{{0, 1, 1, 0},   {1, 1, 0, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}},
			{{0, 1, 0, 0},   {0, 1, 1, 0},   {0, 0, 1, 0},   {0, 0, 0, 0}},
			{{0, 0, 0, 0},   {0, 1, 1, 0},   {1, 1, 0, 0},   {0, 0, 0, 0}}

	};

char z_block[4][4][4] =
	{
			{{0, 1, 0, 0},   {1, 1, 0, 0},   {1, 0, 0, 0},   {0, 0, 0, 0}},
			{{1, 1, 0, 0},   {0, 1, 1, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}},
			{{0, 0, 1, 0},   {0, 1, 1, 0},   {0, 1, 0, 0},   {0, 0, 0, 0}},
			{{0, 0, 0, 0},   {1, 1, 0, 0},   {0, 1, 1, 0},   {0, 0, 0, 0}}

	};

char l_block[4][4][4] =
	{
			{{1, 0, 0, 0},   
			 {1, 0, 0, 0},   
			 {1, 1, 0, 0},   
			 {0, 0, 0, 0}},

			{{1, 1, 1, 0},   
			 {1, 0, 0, 0},   
			 {0, 0, 0, 0},   
			 {0, 0, 0, 0}},

			{{0, 1, 1, 0},   
			 {0, 0, 1, 0},   
			 {0, 0, 1, 0},   
			 {0, 0, 0, 0}},

			{{0, 0, 0, 0},   
			 {0, 0, 1, 0},   
			 {1, 1, 1, 0},   
			 {0, 0, 0, 0}}

	};

char j_block[4][4][4] =
	{
			{{0, 1, 0, 0},   
			 {0, 1, 0, 0},   
			 {1, 1, 0, 0},   
			 {0, 0, 0, 0}},

			{{1, 0, 0, 0},   
			 {1, 1, 1, 0},   
			 {0, 0, 0, 0},   
			 {0, 0, 0, 0}},

			{{1, 1, 0, 0},   
			 {1, 0, 0, 0},   
			 {1, 0, 0, 0},   
			 {0, 0, 0, 0}},

			{{1, 1, 1, 0},   
			 {0, 0, 1, 0},   
			 {0, 0, 0, 0},   
			 {0, 0, 0, 0}}
	};


char o_block[4][4][4] = {
    {{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},
    {{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},
    {{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},
    {{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}
};


/* 테트리스 판을 2차원 배열로 표현
 * 2차원 배열의 2차원 배열
 * 모든 블록의 모양을 표시
 *
 * 20*8 배열
 * 모든 블록의 모양을 표시
 * 모든 블록의 모양을 표시*/

char tetris_table[21][10];

/* 게임 종료 때 마다
 * 이름과 득점수와 
 * 날짜와 시간과 순위를 저장
 * */
static struct result
{
	char name[30];
	long point;
	int year;
	int month;
	int day;
	int hour;
	int min;
	int rank;
}temp_result;

int block_number = 0;  /*블록 번호*/
int next_block_number = 0; /*다음 블록 번호 */
int block_state = 0; /*블록 상태, 왼쪽, 오른쪽, 아래, 회전  */

int x = 3, y = 0; /*블록의 위치*/

int game = GAME_END; /*게임 시작, 게임 종료*/
int best_point = 0; /* 최고 점수*/

long point = 0; /* 현재 점수*/

// 터미널 제어
void checkTerminalSize(void);
void disableEcho(void);
void restoreTerminal(void);

// 입력 처리
int kbhit(void);
int getch(void);
int getKey(void);

// 블록 관련
char (*getCurrentBlock(void))[4];
char (*getNextBlock(int n))[4];
void drawNextBlock(void);
void drawFrame(void);
void drawBlock(int x, int y, char block[4][4]);
void eraseBlock(int x, int y, char block[4][4]);
void drawGhostBlock(int x, int y, char block[4][4]);
int checkCollision(int x, int y, char block[4][4]);
void fixBlock(int x, int y, char block[4][4]);
void clearLines(void);
void shuffleBag(void);
void generateNewBlock(void);
int isGameOver(void);

// 점수 및 결과
void insertRecrod(void);
void sortResult(void);
void showRecordAndInputName(void);
void pointAndTempResultInit(void);

// 게임 루프
int game_start(void);

// 기록 출력 및 검색
void search_result(void);
void print_result(void);


// 비교 함수
int compare(const void *recordA, const void *recordB);

int display_menu(void); /* 메뉴 표시*/

struct termios oldt, newt;

void disableEcho(void) {
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void restoreTerminal(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

// 입력 버퍼에 데이터가 있는지 확인 (비차단 입력 모드 설정)
int kbhit(void) {
    struct timeval tv = {0, 1000};  // 10ms 대기
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
}
// 문자 하나 입력 받기
int getch(void) {
    char ch;
    read(STDIN_FILENO, &ch, 1);
    return ch;
}
// 키보드 입력 값 리턴
int getKey(void) {
    char c = getch();
    if (c == '\x1b') {  // ESC
        if (!kbhit()) return 27;

        char seq[3];
        if (read(STDIN_FILENO, &seq[0], 1) == 0) return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) == 0) return '\x1b';

        if (seq[0] == '[') {
            if (seq[1] >= 'A' && seq[1] <= 'D') {
                switch (seq[1]) {
                    case 'A': return 1000; // 위
                    case 'B': return 1001; // 아래
                    case 'C': return 1002; // 오른쪽
                    case 'D': return 1003; // 왼쪽
                }
            } else if (seq[1] == '5' || seq[1] == '6') {
                if (read(STDIN_FILENO, &seq[2], 1) == 0) return '\x1b';
                if (seq[2] == '~') {
                    if (seq[1] == '5') return 1004; // Page Up
                    if (seq[1] == '6') return 1005; // Page Down
                }
            }
        }
        return '\x1b';
    }
    return c;
}

char (*getCurrentBlock(void))[4] {
    switch (block_number) {
        case I_BLOCK: return i_block[block_state];
        case T_BLOCK: return t_block[block_state];
        case S_BLOCK: return s_block[block_state];
        case Z_BLOCK: return z_block[block_state];
        case L_BLOCK: return l_block[block_state];
        case J_BLOCK: return j_block[block_state];
        case O_BLOCK: return o_block[block_state];
    }
    return NULL;
}

char (*getNextBlock(int n))[4] {
    switch (n) {
        case I_BLOCK: return i_block[3];
        case T_BLOCK: return t_block[0];
        case S_BLOCK: return s_block[0];
        case Z_BLOCK: return z_block[0];
        case L_BLOCK: return l_block[0];
        case J_BLOCK: return j_block[0];
        case O_BLOCK: return o_block[0];
    }
    return NULL;
}

void drawNextBlock(void) {
    char (*block)[4] = getNextBlock(next_block_number);

	printf("\x1b[3;25HNext:");
	for (int r = 0; r < 6; r++) {
        for (int c = 0; c < 7; c++) {
			if(r == 0) {
				if(c == 0) {
					printf("\x1b[%d;%dH⬜", 3 + r, 30 + c * 2);
				} else if(c == 6) {
					printf("\x1b[%d;%dH⬜", 3 + r, 30 + c * 2);
				} else {
					printf("\x1b[%d;%dH⬜", 3 + r, 30 + c * 2);
				}
			} else if(r == 5) {
				if(c == 0) {
					printf("\x1b[%d;%dH⬜", 3 + r, 30 + c * 2);
				} else if(c == 6) {
					printf("\x1b[%d;%dH⬜", 3 + r, 30 + c * 2);
				} else {
					printf("\x1b[%d;%dH⬜", 3 + r, 30 + c * 2);
				}
			}
			else if(c == 0 || c == 6) {
				printf("\x1b[%d;%dH⬜", 3 + r, 30 + c * 2);
			}
		}
	}
    
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (block[r][c] == 1)
                printf("\x1b[%d;%dH🟩", 4 + r, 34 + c * 2);
            else
                printf("\x1b[%d;%dH  ", 4 + r, 34 + c * 2);
        }
    }
}

void drawFrame(void) {
    // 게임 영역에 대한 출력: (row: 0~18)
    for (int row = 1; row < 19; row++) {
        for (int col = 0; col <= 9; col++) {
            if (col == 0 || col == 9) {
                printf("\x1b[%d;%dH⬜", row + 1, (col + 1) * 2);
            } else if (tetris_table[row][col]) {
                printf("\x1b[%d;%dH🟩", row + 1, (col + 1) * 2);
            } else if (col >= 1 && col <= 8) {
                printf("\x1b[%d;%dH\x1b[32m⬛\x1b[0m", row + 1, (col + 1) * 2);
            } else {
                printf("\x1b[%d;%dH ", row + 1, (col + 1) * 2);
            }
        }
    }

    // 바닥 테두리 부분 출력 (row: 19)
    for (int col = 0; col <= 9; col++) {
        if (col == 0) printf("\x1b[%d;%dH⬜", 20, 2);
        else if (col == 9) printf("\x1b[%d;%dH⬜", 20, 20);
        else printf("\x1b[%d;%dH⬜", 20, (col + 1) * 2);
    }

	printf("\x1b[2;25HScore: %ld", point);
	drawNextBlock();
	printf("\x1b[14;25HRotation: [I]");
	printf("\x1b[15;25HLeft: [J]");
	printf("\x1b[16;25HRight: [L]");
	printf("\x1b[17;25HDown: [K]");
	printf("\x1b[18;25HHard Drop: [A]");
	printf("\x1b[19;25HGame Stop: [P]");
    fflush(stdout);
}

void drawBlock(int x, int y, char block[4][4]) {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (block[r][c]) {
                printf("\x1b[%d;%dH🟩", y + r + 1, (x + c + 1) * 2);
            }
        }
    }
    fflush(stdout);
}

void eraseBlock(int x, int y, char block[4][4]) {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (block[r][c]) {
                printf("\x1b[%d;%dH ", y + r + 1, (x + c + 1) * 2);
            }
        }
    }
    fflush(stdout);
}

int checkCollision(int x, int y, char block[4][4]) {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (block[r][c]) { // 블럭이 맵 경계나 다른 블럭에 겹치는지 확인
                int nx = x + c;
                int ny = y + r;
                /*
                 * 맵 범위: col: 1~8, row: 0~19
                 * 이 밖을 벗어나거나 tetris_table[ny][nx]에서 이미 블럭이 있다면 충돌이 났음
                 * 이때는 반환을 정상적인 반응이 아닌 1로 반응을 함.
                 */
                if (nx <= 0 || nx >= 9 || ny < 0 || ny >= 19 || tetris_table[ny][nx])
                    return 1;
            }
        }
    }
    return 0;
}

void drawGhostBlock(int x, int y, char block[4][4]) {
    int ghost_y = y;

    // 바닥까지 시뮬레이션
    while (!checkCollision(x, ghost_y + 1, block)) {
        ghost_y++;
    }

    // 고스트 블럭 출력
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (block[r][c]) {
                printf("\x1b[%d;%dH\x1b[36m🟦\x1b[0m", ghost_y + r + 1, (x + c + 1) * 2);
            }
        }
    }
    fflush(stdout);
}

void fixBlock(int x, int y, char block[4][4]) {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (block[r][c]) {
                tetris_table[y + r][x + c] = 1;
            }
        }
    }
}

void clearLines(void) {
    for (int r = 18; r >= 0; r--) {
        int full = 1;
        for (int c = 1; c < 9; c++) { // 내부만 검사 (1~8)
            if (!tetris_table[r][c]) {
                full = 0;
                break;
            }
        }
        if (full) {
            for (int row = r; row > 0; row--) {
                for (int col = 1; col < 9; col++) {
                    tetris_table[row][col] = tetris_table[row - 1][col];
                }
            }
            for (int col = 1; col < 9; col++) {
                tetris_table[0][col] = 0;
            }
            point += 100;
            r++; // 같은 줄 다시 검사
        }
    }
}

int blockBag[7];
int bagIndex = 7;

void shuffleBag(void) {
    for (int i = 0; i < 7; i++) blockBag[i] = i;

    for (int i = 6; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp    = blockBag[i];
        blockBag[i] = blockBag[j];
        blockBag[j] = temp;
    } // {0, 1, 2, 3, 4, 5, 6} >> {6, 3, 2, 1, 4, 5, 0} 이렇게 섞는 과정

    bagIndex = 0;
}

void generateNewBlock(void) {
    if (bagIndex >= 7) { shuffleBag(); }

    block_number = next_block_number;
    /* 
     * 랜덤성은 유지하되 최대한 중복이 덜 나오게 하는 법 없나... 좀 찾아봐야겠다.
     *
     *Fisher–Yates Shuffle 이라는 알고리즘 참고하여 넣었습니다.
     *[동작 원리]
     * 7개의 블럭을 하나의 set으로 보고 섞습니다.   shuffleBag(); 에서 진행.
     * 그렇게 섞은 배열 하나씩 차례대로 씀.         blockBag[bagIndex++];에서 진행
     * 7개의 한 set을 다 쓰면 다시 set을 섞음.      if (bagIndex >= 7) { shuffleBag(); }에서 진행
     */
    next_block_number = blockBag[bagIndex++];

    block_state = 0;
    x = 3; y = 0;
}

int isGameOver(void) {
    return checkCollision(x, y, getCurrentBlock());
}

int display_menu(void) {
    // 
	int menu = 0;

	while(1)
	{
		system("clear");
		printf("\n\n\t\t\t\tText Tetris");
		printf("\n\t\t\t============================");
		printf("\n\t\t\t\tGAME MENU\t\n");
		printf("\n\t\t\t============================");
		printf("\n\t\t\t   1) Game Start");
		printf("\n\t\t\t   2) Search history");
		printf("\n\t\t\t   3) Record Output");
		printf("\n\t\t\t   4) QUIT");
		printf("\n\t\t\t============================");
		printf("\n\t\t\t\t\t SELECT : ");
		scanf("%d",&menu);
		if(menu < 1 || menu > 4)
		{
			continue;
		}
		else
		{
			return menu;
		}
	}
	return 0;
}

int compare(const void *recordA, const void *recordB) {
	const struct result *cmpA = (const struct result *)recordA;
	const struct result *cmpB = (const struct result *)recordB;

	return cmpB->point - cmpA->point;
}

void sortResult(void) {
	// 정렬할 내용 불러오기 + 정렬 진행
	char line[256];
    FILE *result = fopen("./result.txt", "a+");
	if(result == NULL) {
		perror("파일 열기 실패");
		return;
	}
	rewind(result);

	struct result records[10000];
	int count = 0;
	while(fgets(line, sizeof(line), result)) {
		// %[^,]: 콤마 나오기 전 문자열을 받아오기 위함
		int checkScan = sscanf(line, "Name: %[^,], Point: %ld, Date: %d-%d-%d %d:%d, Rank: %d",
									records[count].name,
									&records[count].point,
									&records[count].year, &records[count].month, &records[count].day,
									&records[count].hour, &records[count].min,
									&records[count].rank);

		if (checkScan == 8) count++;
	}
	qsort(records, count, sizeof(records[0]), compare);
	for (int i = 0; i < count; i++) { records[i].rank = i + 1; }
	
	// 정렬한 내용을 다시 쓰기
	result = fopen("./result.txt", "w");
	if(result == NULL) {
		perror("파일 열기 실패");
		return;
	}
	for (int i = 0; i < count; i++) { 
		fprintf(result,	"Name: %s, Point: %ld, Date: %d-%02d-%02d %02d:%02d, Rank: %d\n",
					 records[i].name,
					 records[i].point,
					 records[i].year, records[i].month, records[i].day, records[i].hour, records[i].min,
					 records[i].rank);
	}
	fclose(result);
}

void insertRecrod(void) {
	FILE *result = fopen("./result.txt", "a+");
	if(result == NULL) {
		perror("파일 열기 실패");
		return;
	}
	rewind(result);

	printf("\x1b[?25l\x1b[17;12H[\x1b[36mS\x1b[0maving]|"); fflush(stdout); usleep(500000);
	printf("\x1b[17;12H[\x1b[36mSa\x1b[0mving]/"); fflush(stdout); usleep(800000);
	printf("\x1b[17;12H[\x1b[36mSav\x1b[0ming]-"); fflush(stdout); usleep(600000);
	printf("\x1b[17;12H[\x1b[36mSav\x1b[0ming]\\"); fflush(stdout); sleep(1);
	printf("\x1b[17;12H[\x1b[36mSavi\x1b[0mng]|"); fflush(stdout); usleep(700000);
    printf("\x1b[17;12H[\x1b[36mSavi\x1b[0mng]/"); fflush(stdout); sleep(1);
	fprintf(result,	"Name: %s, Point: %ld, Date: %d-%02d-%02d %02d:%02d, Rank: %d\n",
		temp_result.name,
		temp_result.point,
		temp_result.year, temp_result.month, temp_result.day, temp_result.hour, temp_result.min,
		0);
	printf("\x1b[17;12H[\x1b[36mSavin\x1b[0mg]-"); fflush(stdout); usleep(900000);
    printf("\x1b[17;12H[\x1b[36mSaving\x1b[0m]\\"); fflush(stdout); usleep(700000);
    printf("\x1b[17;12H[\x1b[36mSaving\x1b[0m]|"); fflush(stdout); sleep(1);
    printf("\x1b[17;12H[\x1b[36mSaving\x1b[0m]/"); fflush(stdout); usleep(500000);
    printf("\x1b[17;12H[\x1b[36mSaving\x1b[0m]-"); fflush(stdout); sleep(1);

	fclose(result);

	printf("\x1b[17;12H[\x1b[36mSave completed!\x1b[0m]   "); fflush(stdout); sleep(1);
	printf("\x1b[?25h"); fflush(stdout);
}

void showRecordAndInputName(void) {
    char name[30] = "";
    int key;
    int len = 0;

    time_t t = time(NULL);
    struct tm *now = localtime(&t);

    temp_result.year = now->tm_year + 1900;
    temp_result.month = now->tm_mon + 1;
    temp_result.day = now->tm_mday;
    temp_result.hour = now->tm_hour;
    temp_result.min = now->tm_min;

    // 결과 화면 출력
    printf("\x1b[8;1H--------------------Game Over!------------------");
    printf("\x1b[9;1H                                      ");
    printf("\x1b[10;1H                   Your Record             ");
    printf("\x1b[11;1H                                      ");
    printf("\x1b[12;1H           Point: \x1b[36m%ld\x1b[0m                      ", point);
    printf("\x1b[13;1H                                      ");
    printf("\x1b[14;1H           Date: \x1b[36m%d-%02d-%02d %02d:%02d\x1b[0m                        ", 
           temp_result.year, temp_result.month, temp_result.day,
           temp_result.hour, temp_result.min);
    printf("\x1b[15;1H                                      ");
	printf("\x1b[16;1H                                      ");
    printf("\x1b[16;1H           Enter your name: ");
    printf("\x1b[17;1H                                      ");
    printf("\x1b[18;1H------------------------------------------------");

    // 게임 중 껐던 커서는 보이게 하기
    printf("\x1b[?25h");

    // 위치 상수 정의
    #define INPUT_ROW 16
    #define INPUT_COL 29               // 이름 입력 시작 위치
    #define LEN_DISPLAY_COL 55        // [len: %d] 위치

    while (1) {
        // 이름 입력 영역 출력
        printf("\x1b[%d;%dH", INPUT_ROW, INPUT_COL);
        printf("\x1b[36m%s\x1b[0m", name);

        // 길이 정보 출력
        printf(" [len: %d]   ", len);

        // 커서 위치 조정
        printf("\x1b[%d;%dH", INPUT_ROW, INPUT_COL + len);

        fflush(stdout);

        key = 0;
        if (kbhit()) key = getKey();

        if (key == '\n' || key == '\r') {
            if (len > 0) break;
            else {
                printf("\x1b[18;1H                                                ");
                printf("\x1b[18;12H\x1b[31mPlease enter your name.\x1b[0m");
                printf("\x1b[19;1H------------------------------------------------");
                continue;
            }
        } else if ((key == 127 || key == 8) && len > 0) {
            name[--len] = '\0';
			printf("\x1b[%d;%dH \x1b[%d;%dH", INPUT_ROW, INPUT_COL + len, INPUT_ROW, INPUT_COL + len);
        } else if (key >= 32 && key <= 126 && len < 29) {
            name[len++] = key;
            name[len] = '\0';
        }
    }

    // 결과 저장
    temp_result.point = point;
    strcpy(temp_result.name, name);
	insertRecrod();
    sortResult();
}

void pointAndTempResultInit(void) {
	point = 0;

	strcpy(temp_result.name, "");
	temp_result.point = 0;
	temp_result.year = 0;
	temp_result.month = 0;
	temp_result.day = 0;
	temp_result.hour = 0;
	temp_result.min = 0;
	temp_result.rank = 0;
}

int game_start(void) {
    checkTerminalSize();
	pointAndTempResultInit();
    system("clear");
    disableEcho();
    printf("\x1b[?25l"); // 커서 숨김
    srand(time(NULL));

    char backup_table[21][10];
    memcpy(backup_table, tetris_table, sizeof(tetris_table));
    memset(tetris_table, 0, sizeof(tetris_table));

    generateNewBlock();

    int key = 0;
    int drop_delay = 500000; // 블럭 낙하 주기: 500ms
    int input_delay = 10000; // 키 입력 처리 주기: 10ms
    int elapsed_time = 0;

    struct timeval prev, curr;
    gettimeofday(&prev, NULL); //커널이 이 순간의 시스템 시계를 읽어옴 즉 지나면 갱신되지 않음

    while (1) {
        gettimeofday(&curr, NULL); // 매 순간마다 커널이 시스템 시계를 읽어옴
		// tv_sec: 초 (seconds) || tv_usec: 마이크로초 (microseconds)
        int diff = (curr.tv_sec - prev.tv_sec) * 1000000 + (curr.tv_usec - prev.tv_usec);
        prev = curr; // 지난 시간과 현재까지 흘러온 시간 차 계산 후 다시 현재로 돌림
        elapsed_time += diff;

        char (*block)[4] = getCurrentBlock();
        eraseBlock(x, y, block);

        if (kbhit()) {
            key = getKey();
            int new_x = x, new_y = y, new_state = block_state;

            if (key == 'j' || key == 'J') new_x--;
            else if (key == 'l' || key == 'L') new_x++;
            else if (key == 'k' || key == 'K') new_y++;
            else if (key == 'i' || key == 'I') {
                new_state = (block_state + 1) % 4;
				// 회전 후 충돌이 없는지 검토
                char (*test_block)[4];
                switch (block_number) {
                    case I_BLOCK: test_block = i_block[new_state]; break;
                    case T_BLOCK: test_block = t_block[new_state]; break;
                    case S_BLOCK: test_block = s_block[new_state]; break;
                    case Z_BLOCK: test_block = z_block[new_state]; break;
                    case L_BLOCK: test_block = l_block[new_state]; break;
                    case J_BLOCK: test_block = j_block[new_state]; break;
                    case O_BLOCK: test_block = o_block[new_state]; break;
                }

				// 벽 근처에서 회전 시 충돌나면 안쪽으로 밀어줌
                if (!checkCollision(x, y, test_block)) {
                    block_state = new_state;
                } else if (!checkCollision(x - 1, y, test_block)) {
                    x--; block_state = new_state;
                } else if (!checkCollision(x + 1, y, test_block)) {
                    x++; block_state = new_state;
                } else if (!checkCollision(x - 2, y, test_block)) {
                    x -= 2; block_state = new_state;
                } else if (!checkCollision(x + 2, y, test_block)) {
                    x += 2; block_state = new_state;
                }
                continue;
            }
            else if (key == 'a' || key == 'A') {
                while (!checkCollision(x, y + 1, block)) {
                    y++;
                }
                fixBlock(x, y, block);
                clearLines();
                generateNewBlock();
                if (isGameOver()) break;
                continue;
            }
            else if (key == 'p' || key == 'P') break;

            if (!checkCollision(new_x, new_y, block_number == O_BLOCK ? o_block[new_state] : getCurrentBlock())) {
                x = new_x; y = new_y; block_state = new_state;
            }
        }

        // 블럭 자동 낙하
        if (elapsed_time >= drop_delay) {
            if (!checkCollision(x, y + 1, block)) {
                y++;
            } else {
                fixBlock(x, y, block);
                clearLines();
                generateNewBlock();
                if (isGameOver()) break;
            }
            elapsed_time = 0; // 타이머 초기화
        }

        drawFrame();
        drawGhostBlock(x, y, getCurrentBlock());
        drawBlock(x, y, getCurrentBlock());
		/*
		 *usleep(200000); 이러면 입력 반응따라 블럭이 내려오는 시간도 바뀜... 여길 수정해야 할 듯
		 *[수정안]
		 *usleep(input_delay); 주기는 저대로 두고 
		 *차라리 블럭이 내려올 시기임을 알려주는 타이머를 추가 후 타이머가 울리면 블럭이 내려오게 하자.
		 */
        usleep(input_delay);
    }

    printf("\x1b[?25h"); // 커서 다시 보이게
    showRecordAndInputName();
    restoreTerminal();
    return 1;
}

void search_result(void) {
    checkTerminalSize();
    system("clear");
    printf("\x1b[3J\x1b[2J\x1b[H");
    fflush(stdout);

    disableEcho();
    sortResult();  // 정렬된 파일 준비

    #define SEARCH_INPUT_ROW 2
    #define SEARCH_INPUT_COL 20
    #define MAX_NAME_DISPLAY 30

    char line[256];
    char currSearchName[30] = "";
    int len = 0;
    int key = 0;

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int max_lines = w.ws_row;

    struct result matches[1000];  // 검색 결과 저장
    int match_count = 0;
    int current_page = 0;
    int lines_per_page = (max_lines - 12) / 6;

    while (1) {
        len = 0;
        currSearchName[0] = '\0';
        current_page = 0;

        // 이름 입력 UI
        printf("\x1b[2J\x1b[H");
        printf("\x1b[%d;3HEnter your name:  [len: %d]", SEARCH_INPUT_ROW, len);
        printf("\x1b[3;3H                                                                                  ");
        printf("\x1b[4;3HExit: \x1b[36m[Esc]\x1b[0m || Try search: \x1b[36m[Enter]\x1b[0m\x1b[?25h");

        while (1) {
            printf("\x1b[%d;%dH", SEARCH_INPUT_ROW, SEARCH_INPUT_COL);
            printf("\x1b[36m%s\x1b[0m [len: %d]  ", currSearchName, len);
            printf("\x1b[%d;%dH", SEARCH_INPUT_ROW, SEARCH_INPUT_COL + len);
            fflush(stdout);

            if (kbhit()) key = getKey();
            else continue;

            if (key == 27) { restoreTerminal(); return; }
            else if ((key == 127 || key == 8) && len > 0) {
                currSearchName[--len] = '\0';
            }
            else if ((key == '\n' || key == '\r')) {
				if(len > 0) break;
				else {
					printf("\x1b[4;3H\x1b[2K");
        			printf("\x1b[4;3H\x1b[31mPlease enter your name.\x1b[0m");
					printf("\x1b[5;3H\x1b[2K");
					printf("\x1b[6;3HExit: \x1b[36m[Esc]\x1b[0m || Try search: \x1b[36m[Enter]\x1b[0m\x1b[?25h");
				}
			}
            else if (key >= 32 && key <= 126 && len < 29) {
                currSearchName[len++] = key;
                currSearchName[len] = '\0';
            }
        }

        // 검색 결과 수집
        FILE *result = fopen("./result.txt", "a+");
        if (result == NULL) { perror("파일 열기 실패"); restoreTerminal(); return; }
        rewind(result);

        match_count = 0;
        while (fgets(line, sizeof(line), result)) {
            sscanf(line, "Name: %[^,], Point: %ld, Date: %d-%d-%d %d:%d, Rank: %d",
                matches[match_count].name,
                &matches[match_count].point,
                &matches[match_count].year, &matches[match_count].month, &matches[match_count].day,
                &matches[match_count].hour, &matches[match_count].min,
                &matches[match_count].rank);

            if (strstr(matches[match_count].name, currSearchName) != NULL) {
                match_count++;
            }
        }
        fclose(result);

        // 결과 없을 때
        if (match_count == 0) {
            printf("\x1b[4;1H\x1b[0J================== Rank ==================\n\n");
            printf("\x1b[6;16H\x1b[31m[No Record]\x1b[0m\n\n");
            printf("==========================================\n\n");
            printf("Exit: \x1b[36m[Esc]\x1b[0m || Try again search: \x1b[36m[Enter]\x1b[0m\x1b[?25h");
            fflush(stdout);
            while (1) {
				int key = getKey();
				if(key == '\n' || key == '\r') break;
				else if(key == 27) {
					restoreTerminal();
					system("clear");
					return;
				}
			}
            continue;
        }

        // 페이지 탐색 루프
        while (1) {
            system("clear");
            printf("\x1b[3J\x1b[2J\x1b[H");
            printf("\x1b[2;1H================== Rank ==================\n\n");

            int start = current_page * lines_per_page;
            int end = start + lines_per_page;
            if (end > match_count) end = match_count;

            for (int i = start; i < end; i++) {
                printf("\tRank: \x1b[36m%d\x1b[0m\n", matches[i].rank);
                printf("\tName: \x1b[36m%s\x1b[0m\n", matches[i].name);
                printf("\tPoint: \x1b[36m%ld\x1b[0m\n", matches[i].point);
                printf("\tDate: \x1b[36m%d-%02d-%02d %02d:%02d\x1b[0m\n",
                       matches[i].year, matches[i].month, matches[i].day,
                       matches[i].hour, matches[i].min);
                printf("\n\t----------------------\n\n");
            }

            printf("\tPage \x1b[36m%d\x1b[0m / %d  (Total \x1b[36m%d\x1b[0m results)\n", current_page + 1, (match_count + lines_per_page - 1) / lines_per_page, match_count);
			printf("==========================================\n");
            printf("Exit: \x1b[36m[Esc]\x1b[0m || Record inquiry: \x1b[36m[Page Up/Down]\x1b[0m\x1b[?25h || Try again search: \x1b[36m[Enter]\x1b[0m");
            fflush(stdout);

            key = getKey();
            if (key == 27) { restoreTerminal(); return; }
            else if (key == '\n' || key == '\r') break;
            else if (key == 1005 && end < match_count) current_page++;     // Page Down
            else if (key == 1004 && current_page > 0) current_page--;      // Page Up
        }
    }

    restoreTerminal();
    system("clear");
}

void print_result(void) {
    checkTerminalSize();
	system("clear");
    printf("\x1b[3J\x1b[2J\x1b[H");
    fflush(stdout);

	char line[256];
    int key = 0;

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int max_lines = w.ws_row;

    struct result matches[1000];  // 검색 결과 저장
    int match_count = 0;
    int current_page = 0;
    int lines_per_page = (max_lines - 12) / 6;
	
	disableEcho();
	sortResult();	// 정렬할 내용 불러오기 + 정렬 진행 + 새롭게 정렬한 내용 담기

	// 결과 수집
	FILE *result = fopen("./result.txt", "a+");
	if (result == NULL) { perror("파일 열기 실패"); restoreTerminal(); return; }
    rewind(result);

	match_count = 0;
	while (fgets(line, sizeof(line), result)) {
		sscanf(line, "Name: %[^,], Point: %ld, Date: %d-%d-%d %d:%d, Rank: %d",
			matches[match_count].name,
			&matches[match_count].point,
			&matches[match_count].year, &matches[match_count].month, &matches[match_count].day,
			&matches[match_count].hour, &matches[match_count].min,
			&matches[match_count].rank);

		match_count++;
	}
	fclose(result);

	// 페이지 탐색 루프
	while (1) {
		system("clear");
        printf("\x1b[3J\x1b[2J\x1b[H");

        if (match_count == 0) {
            printf("\x1b[2;1H\x1b[0J================== Rank ==================\n\n");
            printf("\x1b[4;16H\x1b[31m[No Record]\x1b[0m\n\n");
            printf("==========================================\n\n");
            printf("Exit: \x1b[36m[Esc]\x1b[0m\x1b[?25h");
            fflush(stdout);
            while (1) {
				int key = getKey();
				if(key == '\n' || key == '\r') break;
				else if(key == 27) {
					restoreTerminal();
					system("clear");
					return;
				}
			}
            continue;
        }

		printf("\x1b[2;1H================== Rank ==================\n\n");

		int start = current_page * lines_per_page;
		int end = start + lines_per_page;
		if (end > match_count) end = match_count;

		for (int i = start; i < end; i++) {
			printf("\tRank: \x1b[36m%d\x1b[0m\n", matches[i].rank);
			printf("\tName: \x1b[36m%s\x1b[0m\n", matches[i].name);
			printf("\tPoint: \x1b[36m%ld\x1b[0m\n", matches[i].point);
			printf("\tDate: \x1b[36m%d-%02d-%02d %02d:%02d\x1b[0m\n",
					matches[i].year, matches[i].month, matches[i].day,
					matches[i].hour, matches[i].min);
			printf("\n\t----------------------\n\n");
		}

		printf("\tPage \x1b[36m%d\x1b[0m / %d  (Total \x1b[36m%d\x1b[0m results)\n", current_page + 1, (match_count + lines_per_page - 1) / lines_per_page, match_count);
		printf("==========================================\n");
		printf("Exit: \x1b[36m[Esc]\x1b[0m || Record inquiry: \x1b[36m[Page Up/Down]\x1b[0m\x1b[?25h");
		fflush(stdout);

		key = getKey();
		if (key == 27) break;
		else if (key == 1005 && end < match_count) current_page++;     // Page Down
		else if (key == 1004 && current_page > 0) current_page--;      // Page Up
	}

	restoreTerminal();
}

void checkTerminalSize(void) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    if (w.ws_row < 20 || w.ws_col < 60) {
        printf("\x1b[2J\x1b[H");
        printf("\x1b[31m[Error]\x1b[0m 화면 크기가 너무 작습니다.\n");
        printf("최소 20행, 60열 이상 필요합니다. 현재: %d행 %d열\n", w.ws_row, w.ws_col);
        printf("터미널 창을 키운 뒤 다시 시도해주세요.\n");
        getKey();
        exit(1);
    }
}

int main(void) {
	int menu = 1;

	while(menu)
	{
		menu = display_menu();

		if(menu == 1) {
			game = GAME_START;
			menu = game_start();
		}
		else if(menu == 2) {
			search_result();
		}
		else if(menu == 3) {
			print_result();
		}
		else if(menu == 4) {
			exit(0);
		}
	}

	return 0;
}
