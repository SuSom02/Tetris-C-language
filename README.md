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
