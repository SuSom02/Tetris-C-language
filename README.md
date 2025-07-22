# Tetris-C-language
    대학에서 주어진 과제로 C 언어로 테트리스를 만들어봤습니다.

# What I Learned While Building a Tetris Game
```
1. 블럭 생성시 랜덤성은 유지하되 최대한 연속되는 패턴의 블럭이 안 나오게끔 하고자 [Fisher–Yates Shuffle] 알고리즘을 찾아서 이용했습니다.

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

    [실적용]
    void generateNewBlock(void) {
        if (bagIndex >= 7) { shuffleBag(); }
    
        block_number = next_block_number;
        /* 
         *[동작 원리]
         * 7개의 블럭을 하나의 set으로 보고 섞습니다.   shuffleBag(); 에서 진행.
         * 그렇게 섞은 배열 하나씩 차례대로 씀.         blockBag[bagIndex++];에서 진행
         * 7개의 한 set을 다 쓰면 다시 set을 섞음.      if (bagIndex >= 7) { shuffleBag(); }에서 진행
         */
        next_block_number = blockBag[bagIndex++];
    
        block_state = 0;
        x = 3; y = 0;
    }
    
```
```
2. 블럭이 한칸 씩 내려오는 것과 키 입력 처리 주기를 따로 두지 않으면 발생하는 문제

    초기 프로토 타입 때 아래 코드와 같은 방식으로 임시 제작을 했습니다.
        generateNewBlock();
        while() {
            keystroke processing();
            go down one block();
            usleep(delay);
        }
    하지만 이러면 usleep(delay);에서
    delay 시간이 길수록 블럭이 천천히 내려와서 게임 난이도는 쉬워지나 다음 키 입력 처리까지 반응 느려져서 게임이 마치 작동을 안하는 것처럼 인식이 되는 반면
    delay 시간이 짧을수록 블럭이 빠르게 내려오는 대신 다음 키 입력 처리 반응 빨라져서 게임이 원활하게 작동하는 인식을 받을 수 있었습니다.

    그래서 전 고민하다가 블럭이 내려오는 시간과 키 입력 처리를 받는 시간을 따로 돌아가게끔 구성을 했습니다.

    [실적용]
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
        __int drop_delay = 500000; // 블럭 낙하 주기: 500ms__
        int input_delay = 10000; // 키 입력 처리 주기: 10ms__
        int elapsed_time = 0;
    
        struct timeval prev, curr;
        gettimeofday(&prev, NULL); //커널이 이 순간의 시스템 시계를 읽어옴 즉 지나면 갱신되지 않음
    
        while (1) {
            gettimeofday(&curr, NULL); // 매 순간마다 커널이 시스템 시계를 읽어옴
            int diff = (curr.tv_sec - prev.tv_sec) * 1000000 + (curr.tv_usec - prev.tv_usec);  // tv_sec: 초 (seconds) || tv_usec: 마이크로초 (microseconds)
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
    
```

# How to use
    0. 실행 가능한 OS 다음과 같습니다
        - linux
        - Mac OS

    1. 실행 명령 예시는 아래와 같습니다.
        make
        ./tetris

    2. menu 선택 중
        1) Game start : 요구하셨던 사항에 맞춰 [A], [I], [J], [K], [L], [P] 키에 대한 기능을 구현했습니다.
                        쌓여진 블럭이 맵 높이보다 높아지거나 [P] 키를 눌러 게임 종료하게 되면 Game Over! 라는 UI와 함께 누적 점수, 기록되는 현재 시각이 뜹니다.
                        그리고 밑에 저장 시 기록하고 싶은 이름도 입력할 수 있게 하였습니다. 이름 입력은 영어만 지원하며, 길이 제한 29자입니다.
                        #. 만약 이름 입력 없이 [Enter]를 누르게 된다면 "Please enter your name." 라는 경고문이 출력되며 저장이 이뤄지지 않습니다.

                        + 실제 저장은 빠르게 이루어지고 있지만 이거와는 별개로 저장되고 있음을 보여주고 싶어 [Saving]/ >> [Saving]- >> [Save completed!] 것도 추가해봤습니다.


        2) Search history : 게임 종료 후 기록 저장할 때 입력한 이름으로 탐색할 수 있게 구현했습니다.
                            이름 검색 시 이름 입력은 영어만 지원하며, 이름의 일부분 검색도 지원합니다. 예시) search name: su >> result record: Insu, susom 이라는 이름으로 기록한 내용이 나옵니다.
                            그리고 대소문자 구분을 합니다. 예시) search name: lee >> result record: lee (Lee 라는 유저를 찾는 거라면 'Lee'라고 검색을 해야 합니다.)
                            #. 만약 이름 입력 없이 [Enter]를 누르게 된다면 "Please enter your name." 라는 경고문이 출력되며 저장이 이뤄지지 않습니다.

                            찾는 내용의 기록이 길어진다면 [Page UP/DOWN] 키로 페이지를 넘기면서 기록을 확인하실 수 있습니다.
                            기록을 나타나는 순서는 rank 기준입니다.

                            찾기를 종료하고 싶다면 [ESC]를 누르면 됩니다.


        3) Record Output : 전체 기록을 확인할 수 있도록 구현했습니다.
                            전체 기록이 길어진다면 [Page UP/DOWN] 키로 페이지를 넘기면서 기록을 확인하실 수 있습니다.
                            기록을 나타나는 순서는 rank 기준입니다.

                            찾기를 종료하고 싶다면 [ESC]를 누르면 됩니다.

        4) QUIT : 프로그램이 종료됩니다.

    긴 설명 읽어주셔서 감사합니다.
