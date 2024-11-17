#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"

/*====현재까지 구현한 내용====
1. 준비 - (구현완료)
2. 커서 & 상태창 - 방향키 더블클릭, 선택, 취소 (구현완료)
3. 중립 유닛 - 샌드웜 두마리 모두 움직이며 확률적(5%)으로 스파이스 생성 구현 완료(움직이지만 o 유닛처럼 제자리로 돌아감.) // 실행 계속 해보며 밸런스(스파이스 매장량, 샌드웜 스파이스 생성 확률) 조정 예정!
4. 유닛 생산 - H(하베스터)를 B베이스 위에서 명령어 h를 누를 경우에 본진 위에 생성됨 / esc를 누르면 명령창과 상태창이 없어지며 취소됨 / 스페이스바를 누른 뒤 다른키를 누르면 현 상태를 유지함 (구현 완료)
5. 시스템 메세지 - 새로운 메세지는 맨 아래에 과거에 출력된 메세지는 맨 위로가게 출력함 (구현완료)
*/

// 함수 정렬
void init(void);
void game_map(void);
void intro(void);
void outro(void);
void cursor_move(DIRECTION dir);

void clear_messages(void);

void sample_obj_move(void);
void sandworm_obj_move(int sandworm_make_spice1);
void sandworm1_obj_move(int sandworm_make_spice2);

POSITION sample_obj_next_position(void);
POSITION sandworm_obj_next_position(void);
POSITION sandworm1_obj_next_position(void);

void space_prass(void);
void ESC_prass(void);

char check_cursor_position(void); // 현재 커서에 있는 문자를 인식
void ally_base_info(void); // 아군 베이스 현재 정보
void ally_harvester_info(void); // 아군 하베스터 현재 정보
void stone_rock_info(void); // 돌, 바위 현재 정보
void sandworm_info(void); // 샌드웜 현재 정보
void plate_info(void); // 장판 현재 정보
void spice_info(void); // 스파이스 현재 정보
void desert_info(void); // 사막 정보 표시

/*==========명령창==========*/
void normally_command(void);
void command_ally_base(void);
void command_ally_harvester(void);

void create_harvester(RESOURCE *resource);

void system_message(const char* mes);

/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
int cursor_clock = 0;

CURSOR cursor = { {1, 1}, {1, 1} };
/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };
char system_map[SYSTEM_MES_HEIGHT][SYSTEM_MES_WIDTH] = { 0 };
char status_map[STATUS_HEIGHT][STATUS_WIDTH] = { 0 };
char consol_map[CONSOL_HEIGHT][CONSOL_WIDTH] = { 0 };

//자원 정보
RESOURCE resource = {  
.spice = 5,
.spice_max = 50,
.population = 0,
.population_max = 200
}; 

OBJECT_SAMPLE obj = {
.pos = {1, 1},
.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
.repr = 'o',
.move_period = 300,
.next_move_time = 300
};

OBJECT_SAMPLE sandworm1 = {
.pos = {MAP_HEIGHT - (MAP_HEIGHT - 3), MAP_WIDTH - (MAP_WIDTH - 10)},
.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
.repr = 'W',
.move_period = 1000,
.next_move_time = 1000
};

OBJECT_SAMPLE sandworm = {
.pos = {MAP_HEIGHT - 7, (MAP_WIDTH + 36) - MAP_WIDTH},
.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
.repr = 'W',
.move_period = 1000,
.next_move_time = 1000
};

//아군 베이스 설정 'B'
OBJECT_BUILDING obj_ally_build = {
.repr = 'B',
.layer = 0,
.A = {MAP_HEIGHT - 3, MAP_WIDTH - (MAP_WIDTH - 1)},
.B = {MAP_HEIGHT - 2, MAP_WIDTH - (MAP_WIDTH - 1)},
.C = {MAP_HEIGHT - 3, MAP_WIDTH - (MAP_WIDTH - 2)},
.D = {MAP_HEIGHT - 2, MAP_WIDTH - (MAP_WIDTH - 2)}
};
//적군 베이스 설정 'B'
OBJECT_BUILDING obj_enemy_build = {
.repr = 'B',
.layer = 0,
.A = {MAP_HEIGHT - (MAP_HEIGHT - 2), MAP_WIDTH - (MAP_WIDTH + 3)},
.B = {MAP_HEIGHT - (MAP_HEIGHT - 3), MAP_WIDTH - (MAP_WIDTH + 3)},
.C = {MAP_HEIGHT - (MAP_HEIGHT - 2), MAP_WIDTH - (MAP_WIDTH + 2)},
.D = {MAP_HEIGHT - (MAP_HEIGHT - 3), MAP_WIDTH - (MAP_WIDTH + 2)}
};
//아군, 적군 하베스터 추가 'H'
OBJECT_BUILDING obj_Harvester = {
.repr = 'H',
.layer = 1,
.A = {MAP_HEIGHT - 4, MAP_WIDTH - (MAP_WIDTH - 1)}, //아군 하베스터
.B = {MAP_HEIGHT - (MAP_HEIGHT - 4), MAP_WIDTH - (MAP_WIDTH + 2)} //적군 하베스터
};
//아군, 적군 스파이스 추가 'H'
OBJECT_BUILDING obj_spice = {
.repr = '5',
.layer = 0,
.A = {MAP_HEIGHT - 8, MAP_WIDTH - (MAP_WIDTH - 1)}, //아군 스파이스
.B = {MAP_HEIGHT - (MAP_HEIGHT - 8), MAP_WIDTH - (MAP_WIDTH + 2)} //적군 스파이스

};
//아군쪽 장판 추가 'P'
OBJECT_BUILDING obj_ally_plate = {
.repr = 'P',
.layer = 0,
.A = {MAP_HEIGHT - 3, MAP_WIDTH - (MAP_WIDTH - 3)},
.B = {MAP_HEIGHT - 2, MAP_WIDTH - (MAP_WIDTH - 3)},
.C = {MAP_HEIGHT - 3, MAP_WIDTH - (MAP_WIDTH - 4)},
.D = {MAP_HEIGHT - 2, MAP_WIDTH - (MAP_WIDTH - 4)}
};
//적군쪽 장판 추가 'P'
OBJECT_BUILDING obj_enemy_plate = {
.repr = 'P',
.layer = 0,
.A = {MAP_HEIGHT - (MAP_HEIGHT - 2), MAP_WIDTH - (MAP_WIDTH + 5)},
.B = {MAP_HEIGHT - (MAP_HEIGHT - 3), MAP_WIDTH - (MAP_WIDTH + 5)},
.C = {MAP_HEIGHT - (MAP_HEIGHT - 2), MAP_WIDTH - (MAP_WIDTH + 4)},
.D = {MAP_HEIGHT - (MAP_HEIGHT - 3), MAP_WIDTH - (MAP_WIDTH + 4)}
};
//좌측 상단 중립 돌 추가 'R'
OBJECT_BUILDING obj_stone1 = {
.repr = 'R',
.layer = 0,
.A = {MAP_HEIGHT - (MAP_HEIGHT - 5), MAP_WIDTH - (MAP_WIDTH - 18)},
.B = {MAP_HEIGHT - (MAP_HEIGHT - 6), MAP_WIDTH - (MAP_WIDTH - 18)},
.C = {MAP_HEIGHT - (MAP_HEIGHT - 5), MAP_WIDTH - (MAP_WIDTH - 19)},
.D = {MAP_HEIGHT - (MAP_HEIGHT - 6), MAP_WIDTH - (MAP_WIDTH - 19)},

};
//우측 하단 중립 돌 추가 'R'
OBJECT_BUILDING obj_stone2 = {
.repr = 'R',
.layer = 0,
.A = {MAP_HEIGHT - 5, MAP_WIDTH - 11},
.B = {MAP_HEIGHT - 6, MAP_WIDTH - 11},
.C = {MAP_HEIGHT - 5, MAP_WIDTH - 12},
.D = {MAP_HEIGHT - 6, MAP_WIDTH - 12}
};
//맵 이곳저곳 작은 돌 추가 'R'
OBJECT_BUILDING obj_small_stone = {
.repr = 'R',
.layer = 0,
.A = {MAP_HEIGHT - 7, MAP_WIDTH + 12},
.B = {MAP_HEIGHT - 12, MAP_WIDTH - 20}
};
//샌드웜 두마리 추가 'W'
OBJECT_BUILDING obj_sandworm = {
.repr = 'W',
.layer = 1,
.A = {MAP_HEIGHT - (MAP_HEIGHT - 3), MAP_WIDTH - (MAP_WIDTH - 10)},
.B = {MAP_HEIGHT - 7, (MAP_WIDTH + 36) - MAP_WIDTH}
};

/* ================= main() =================== */
int main(void) {
	int prevkey = 0;
	int sandworm_make_spice1 = 0, sandworm_make_spice2 = 0;
	int count = 0;
	clock_t lasttime = 0;

	srand((unsigned int)time(NULL));
	init();
	game_map();
	intro();
	display(resource, map, system_map,status_map, consol_map, cursor);
	normally_command();

	while (1) {

		// loop 돌 때마다(즉, TICK==10ms마다) 키 입력 확인
		KEY key = get_key();

		// 키 입력이 있으면 처리
		if (is_arrow_key(key)) {
			//140ms안에 클릭이 한번 더 있을때
			if (prevkey == key && (clock() - lasttime) <= 140) {
				for (int i = 0; i < 4; i++) {
					cursor_move(ktod(key));
					display(resource, map, system_map, status_map, consol_map, cursor);
				}
				lasttime = 0;
			}
			else {
				cursor_move(ktod(key));
			}
			lasttime = clock();
			prevkey = key;
		}
		else {
			// 방향키 외의 입력
			switch (key) {
			case k_quit: outro(); break;
			case k_space: space_prass(); break;
			case k_esc:	ESC_prass(); break;
			case k_h: create_harvester(&resource); break;
			case k_H: create_harvester(&resource); break;
			case k_none:
			case k_undef:
			default: break;
			}
		}

		// 샘플 오브젝트 동작
		sample_obj_move();
		// 샌드웜 오브젝트 움직임
		sandworm_obj_move(sandworm_make_spice1);
		sandworm1_obj_move(sandworm_make_spice2);
		
		//resource = 
		// 화면 출력
		display(resource, map, system_map, status_map, consol_map, cursor);
		Sleep(TICK);
		sys_clock += 10;
	}
}

/* ================= subfunctions =================== */

// 인트로
void intro(void) {
	printf("Dune 1.5");
	Sleep(2000);
	system("cls");
}  

// 아웃트로
void outro(void) {
	printf("exiting...\n");
	exit(0);
}

// #모양 지형생성
void init(void) {
	// layer 0(map[0])에 지형 생성
	// 게임 화면
	for (int j = 0; j < MAP_WIDTH; j++) {
		map[0][0][j] = '#';
		map[0][MAP_HEIGHT - 1][j] = '#';
	}
	for (int i = 1; i < MAP_HEIGHT - 1; i++) {
		map[0][i][0] = '#';
		map[0][i][MAP_WIDTH - 1] = '#';
		for (int j = 1; j < MAP_WIDTH - 1; j++) {
			map[0][i][j] = ' ';
		}
	}
	// layer 1(map[1])은 비워 두기(-1로 채움)
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[1][i][j] = -1;
		}
	}
	//system_map[]에 지형 생성
	//시스템 메세지 화면
	for (int j = 0; j < SYSTEM_MES_WIDTH; j++) {
		system_map[0][j] = '#';
		system_map[SYSTEM_MES_HEIGHT - 1][j] = '#';
	}
	for (int i = 1; i < SYSTEM_MES_HEIGHT - 1; i++) {
		system_map[i][0] = '#';
		system_map[i][SYSTEM_MES_WIDTH - 1] = '#';
		for (int j = 1; j < SYSTEM_MES_WIDTH - 1; j++) {
			system_map[i][j] = ' ';
		}
	}
	//status_map[]에 지형 생성
	//상태창 화면
	for (int j = 0; j < STATUS_WIDTH; j++) {
		status_map[0][j] = '#';
		status_map[STATUS_HEIGHT - 1][j] = '#';
	}
	for (int i = 1; i < STATUS_HEIGHT - 1; i++) {
		status_map[i][0] = '#';
		status_map[i][STATUS_WIDTH - 1] = '#';
		for (int j = 1; j < STATUS_WIDTH - 1; j++) {
			status_map[i][j] = ' ';
		}
	}
	//consol_map[]에 지형 생성
	//명령창 화면
	for (int j = 0; j < CONSOL_WIDTH; j++) {
		consol_map[0][j] = '#';
		consol_map[CONSOL_HEIGHT - 1][j] = '#';
	}
	for (int i = 1; i < CONSOL_HEIGHT - 1; i++) {
		consol_map[i][0] = '#';
		consol_map[i][CONSOL_WIDTH - 1] = '#';
		for (int j = 1; j < CONSOL_WIDTH - 1; j++) {
			consol_map[i][j] = ' ';
		}
	}
	// object sample
	map[1][obj.pos.row][obj.pos.column] = 'o';
}

// 건물,지형 / 유닛 맵 안에 배치
void game_map(void) {
	// 아군 베이스 추가 B
	map[obj_ally_build.layer][obj_ally_build.A.row][obj_ally_build.A.column] = obj_ally_build.repr;
	map[obj_ally_build.layer][obj_ally_build.B.row][obj_ally_build.B.column] = obj_ally_build.repr;
	map[obj_ally_build.layer][obj_ally_build.C.row][obj_ally_build.C.column] = obj_ally_build.repr;
	map[obj_ally_build.layer][obj_ally_build.D.row][obj_ally_build.D.column] = obj_ally_build.repr;
	// 적군 베이스 추가 B
	map[obj_enemy_build.layer][obj_enemy_build.A.row][obj_enemy_build.A.column] = obj_enemy_build.repr;
	map[obj_enemy_build.layer][obj_enemy_build.B.row][obj_enemy_build.B.column] = obj_enemy_build.repr;
	map[obj_enemy_build.layer][obj_enemy_build.C.row][obj_enemy_build.C.column] = obj_enemy_build.repr;
	map[obj_enemy_build.layer][obj_enemy_build.D.row][obj_enemy_build.D.column] = obj_enemy_build.repr;
	//아군, 적군 하베스터 추가 H
	map[obj_Harvester.layer][obj_Harvester.A.row][obj_Harvester.A.column] = obj_Harvester.repr;
	map[obj_Harvester.layer][obj_Harvester.B.row][obj_Harvester.B.column] = obj_Harvester.repr;
	//아군, 적군 스파이스 추가 H
	map[obj_spice.layer][obj_spice.A.row][obj_spice.A.column] = obj_spice.repr;
	map[obj_spice.layer][obj_spice.B.row][obj_spice.B.column] = obj_spice.repr;
	//아군쪽 장판 추가 P
	map[obj_ally_plate.layer][obj_ally_plate.A.row][obj_ally_plate.A.column] = obj_ally_plate.repr;
	map[obj_ally_plate.layer][obj_ally_plate.B.row][obj_ally_plate.B.column] = obj_ally_plate.repr;
	map[obj_ally_plate.layer][obj_ally_plate.C.row][obj_ally_plate.C.column] = obj_ally_plate.repr;
	map[obj_ally_plate.layer][obj_ally_plate.D.row][obj_ally_plate.D.column] = obj_ally_plate.repr;
	//적군쪽 장판 추가 P
	map[obj_enemy_plate.layer][obj_enemy_plate.A.row][obj_enemy_plate.A.column] = obj_enemy_plate.repr;
	map[obj_enemy_plate.layer][obj_enemy_plate.B.row][obj_enemy_plate.B.column] = obj_enemy_plate.repr;
	map[obj_enemy_plate.layer][obj_enemy_plate.C.row][obj_enemy_plate.C.column] = obj_enemy_plate.repr;
	map[obj_enemy_plate.layer][obj_enemy_plate.D.row][obj_enemy_plate.D.column] = obj_enemy_plate.repr;
	//좌측 상단 중립 돌 추가 R
	map[obj_stone1.layer][obj_stone1.A.row][obj_stone1.A.column] = obj_stone1.repr;
	map[obj_stone1.layer][obj_stone1.B.row][obj_stone1.B.column] = obj_stone1.repr;
	map[obj_stone1.layer][obj_stone1.C.row][obj_stone1.C.column] = obj_stone1.repr;
	map[obj_stone1.layer][obj_stone1.D.row][obj_stone1.D.column] = obj_stone1.repr;
	//우측 하단 중립 돌 추가 R
	map[obj_stone2.layer][obj_stone2.A.row][obj_stone2.A.column] = obj_stone2.repr;
	map[obj_stone2.layer][obj_stone2.B.row][obj_stone2.B.column] = obj_stone2.repr;
	map[obj_stone2.layer][obj_stone2.C.row][obj_stone2.C.column] = obj_stone2.repr;
	map[obj_stone2.layer][obj_stone2.D.row][obj_stone2.D.column] = obj_stone2.repr;
	//맵 이곳저곳에 작은 돌 추가 R
	map[obj_small_stone.layer][obj_small_stone.A.row][obj_small_stone.A.column] = obj_small_stone.repr;
	map[obj_small_stone.layer][obj_small_stone.B.row][obj_small_stone.B.column] = obj_small_stone.repr;
	//샌드웜 두마리 추가 W
	map[obj_sandworm.layer][obj_sandworm.A.row][obj_sandworm.A.column] = obj_sandworm.repr;
	map[obj_sandworm.layer][obj_sandworm.B.row][obj_sandworm.B.column] = obj_sandworm.repr;
}

// (가능하다면) 지정한 방향으로 커서 이동
void cursor_move(DIRECTION dir) {
	POSITION curr = cursor.current;
	POSITION new_pos = pmove(curr, dir);
	
	// validation check
	if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 && \
		1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {
		
		cursor.previous = cursor.current;
		cursor.current = new_pos;
	}
}

/* ================= sample object movement =================== */
POSITION sample_obj_next_position(void) {
	// 현재 위치와 목적지를 비교해서 이동 방향 결정	
	POSITION diff = psub(obj.dest, obj.pos);
	DIRECTION dir;

	// 목적지 도착. 지금은 단순히 원래 자리로 왕복
	if (diff.row == 0 && diff.column == 0) {
		if (obj.dest.row == 1 && obj.dest.column == 1) {
			// topleft --> bottomright로 목적지 설정
			POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
			obj.dest = new_dest;
		}
		else {
			// bottomright --> topleft로 목적지 설정
			POSITION new_dest = { 1, 1 };
			obj.dest = new_dest;
		}
		return obj.pos;
	}

	// 가로축, 세로축 거리를 비교해서 더 먼 쪽 축으로 이동
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// validation check
	// next_pos가 맵을 벗어나지 않고, (지금은 없지만)장애물에 부딪히지 않으면 다음 위치로 이동
	// 지금은 충돌 시 아무것도 안 하는데, 나중에는 장애물을 피해가거나 적과 전투를 하거나... 등등
	POSITION next_pos = pmove(obj.pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {

		return next_pos;
	}
	else {
		return obj.pos;  // 제자리
	}
}
// 우측 하단 샌드웜 이동
POSITION sandworm_obj_next_position(void) {
	// 현재 위치와 목적지를 비교해서 이동 방향 결정	
	POSITION diff = psub(sandworm.dest, sandworm.pos);
	DIRECTION dir;

	// 목적지 도착. 지금은 단순히 원래 자리로 왕복
	if (diff.row == 0 && diff.column == 0) {
		if (sandworm.dest.row == MAP_HEIGHT - 7 && sandworm.dest.column == (MAP_WIDTH + 36) - MAP_WIDTH) {
			// topleft --> bottomright로 목적지 설정
			POSITION new_dest = { 1, 1 };
			sandworm.dest = new_dest;
		}
		else {
			// bottomright --> topleft로 목적지 설정
			POSITION new_dest = { MAP_HEIGHT - 7, (MAP_WIDTH + 36) - MAP_WIDTH };
			sandworm.dest = new_dest;
		}
		return sandworm.pos;
	}

	// 가로축, 세로축 거리를 비교해서 더 먼 쪽 축으로 이동
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// validation check
	// next_pos가 맵을 벗어나지 않고, (지금은 없지만)장애물에 부딪히지 않으면 다음 위치로 이동
	// 지금은 충돌 시 아무것도 안 하는데, 나중에는 장애물을 피해가거나 적과 전투를 하거나... 등등
	POSITION next_pos = pmove(sandworm.pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {

		return next_pos;
	}
	else {
		return sandworm.pos;  // 제자리
	}
}
// 좌측 상단 샌드웜 이동
POSITION sandworm1_obj_next_position(void) {
	// 현재 위치와 목적지를 비교해서 이동 방향 결정	
	POSITION diff = psub(sandworm1.dest, sandworm1.pos);
	DIRECTION dir;

	// 목적지 도착. 지금은 단순히 원래 자리로 왕복
	if (diff.row == 0 && diff.column == 0) {
		if (sandworm1.dest.row == MAP_HEIGHT - (MAP_HEIGHT - 3) && sandworm1.dest.column == MAP_WIDTH - (MAP_WIDTH - 10)) {
			// topleft --> bottomright로 목적지 설정
			POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
			sandworm1.dest = new_dest;
		}
		else {
			// bottomright --> topleft로 목적지 설정
			POSITION new_dest = { MAP_HEIGHT - (MAP_HEIGHT - 3), MAP_WIDTH - (MAP_WIDTH - 10) };
			sandworm1.dest = new_dest;
		}
		return sandworm1.pos;
	}

	// 가로축, 세로축 거리를 비교해서 더 먼 쪽 축으로 이동
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// validation check
	// next_pos가 맵을 벗어나지 않고, (지금은 없지만)장애물에 부딪히지 않으면 다음 위치로 이동
	// 지금은 충돌 시 아무것도 안 하는데, 나중에는 장애물을 피해가거나 적과 전투를 하거나... 등등
	POSITION next_pos = pmove(sandworm1.pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {

		return next_pos;
	}
	else {
		return sandworm1.pos;  // 제자리
	}
}

void sample_obj_move(void) {
	if (sys_clock <= obj.next_move_time) {
		// 아직 시간이 안 됐음
		return;
	}

	// 오브젝트(건물, 유닛 등)은 layer1(map[1])에 저장
	map[1][obj.pos.row][obj.pos.column] = -1;
	obj.pos = sample_obj_next_position();
	map[1][obj.pos.row][obj.pos.column] = obj.repr;

	obj.next_move_time = sys_clock + obj.move_period;
}

void sandworm_obj_move(int sandworm_make_spice1) {
	POSITION prev;

	sandworm_make_spice1 = rand() % 100 + 1;  // 1 ~ 100까지 랜덤한 수 뽑기

	if (sys_clock <= sandworm.next_move_time) {
		// 아직 시간이 안 됐음
		return;
	}
	
	// 오브젝트(건물, 유닛 등)은 layer1(map[1])에 저장
	map[1][sandworm.pos.row][sandworm.pos.column] = -1;
	prev = sandworm.pos;
	sandworm.pos = sandworm_obj_next_position();
	map[1][sandworm.pos.row][sandworm.pos.column] = sandworm.repr;
	// 5% 확률로 스파이스 매장지 생성
	if (sandworm_make_spice1 <= 5) {
		map[1][prev.row][prev.column] = '3';
		system_message("샌드웜이 스파이스를 생성했습니다.");
	}

	sandworm.next_move_time = sys_clock + sandworm.move_period;
}

void sandworm1_obj_move(int sandworm_make_spice2) {
	POSITION prev;
	sandworm_make_spice2 = rand() % 100 + 1; // 1 ~ 100까지 랜덤한 수 뽑기

	if (sys_clock <= sandworm1.next_move_time) {
		// 아직 시간이 안 됐음
		return;
	}

	// 오브젝트(건물, 유닛 등)은 layer1(map[1])에 저장
	map[1][sandworm1.pos.row][sandworm1.pos.column] = -1;
	prev = sandworm1.pos;
	sandworm1.pos = sandworm1_obj_next_position();
	map[1][sandworm1.pos.row][sandworm1.pos.column] = sandworm1.repr;
	// 5% 확률로 스파이스 매장지 생성
	if (sandworm_make_spice2 <= 5) {
		map[1][prev.row][prev.column] = '3';
		system_message("샌드웜이 스파이스를 생성했습니다.");
	}

	sandworm1.next_move_time = sys_clock + sandworm1.move_period;
}


/*================= 문자 청소 =================*/
void clear_messages(void) {
	POSITION pos;
	for (int i = 2; i < STATUS_HEIGHT - 1; i++) {
		for (int j = 4; j < STATUS_WIDTH - 2; j++) {
			pos.row = i;
			pos.column = MAP_WIDTH + j;
			gotoxy(pos);
			printf(" ");
		}
	}
	for (int i = 3; i < CONSOL_HEIGHT - 1; i++) {
		for (int j = 4; j < CONSOL_WIDTH - 2; j++) {
			pos.row = MAP_HEIGHT + i;
			pos.column = MAP_WIDTH + j;
			gotoxy(pos);
			printf(" ");
		}
	}
}

/*================= 입력 문자 확인 =============================*/
// 커서에 위치해있는 문자 인식
char check_cursor_position(void) {
	int row = cursor.current.row;
	int column = cursor.current.column;

	if (map[1][row][column] >= 0) {
		return map[1][row][column];
	}
	return map[0][row][column];
}
// 스페이스바를 눌렀을때
void space_prass(void) {
	char current_char = check_cursor_position();
	//아군 베이스를 인식했을때(아군 적군 구별하는 방법 추가요함)
	if (current_char == 'B') {
		clear_messages();
		ally_base_info();
		command_ally_base();
	}
	else if (current_char == 'H') {
		clear_messages();
		ally_harvester_info();
		command_ally_harvester();
	}
	else if (current_char == 'R') {
		clear_messages();
		stone_rock_info();
	}
	else if (current_char == 'W') {
		clear_messages();
		sandworm_info();
	}
	else if (current_char == '5' || current_char == '3') {
		clear_messages();
		spice_info();
	}
	else if (current_char == 'P') {
		clear_messages();
		plate_info();
	}
	else if (current_char == ' ') {
		clear_messages();
		desert_info();
	}
}
// ESC를 눌렀을때
void ESC_prass(void) {
	POSITION pos;
	for (int i = 2; i < STATUS_HEIGHT - 1; i++) {
		for (int j = 4; j < STATUS_WIDTH - 2; j++) {
			pos.row = i;
			pos.column = MAP_WIDTH + j;
			gotoxy(pos);
			printf(" ");
		}
	}
	for (int i = 3; i < CONSOL_HEIGHT - 1; i++) {
		for (int j = 4; j < CONSOL_WIDTH - 2; j++) {
			pos.row = MAP_HEIGHT + i;
			pos.column = MAP_WIDTH + j;
			gotoxy(pos);
			printf(" ");
		}
	}
	normally_command();
}

/*===========유닛, 오브젝트, 건물 정보 출력===========*/
void ally_base_info(void){
	POSITION pos;
	pos.row = 2;
	pos.column = MAP_WIDTH + 4;
	gotoxy(pos);
	printf("- 건물 정보 -");
	pos.row += 1;
	gotoxy(pos);
	printf("오브젝트 이름: 아군 본진");
	pos.row += 1;
	gotoxy(pos);
	printf("오브젝트 정보: 건물");
	
}
void ally_harvester_info(void) {
	POSITION pos;
	pos.row = 2;
	pos.column = MAP_WIDTH + 4;
	gotoxy(pos);
	printf("- 유닛 정보 -");
	pos.row += 1;
	gotoxy(pos);
	printf("오브젝트 이름: 아군 하베스터");
	pos.row += 1;
	gotoxy(pos);
	printf("오브젝트 정보: 유닛");
}
void stone_rock_info(void) {
	POSITION pos;
	pos.row = 2;
	pos.column = MAP_WIDTH + 4;
	gotoxy(pos);
	printf("- 건물 정보 -");
	pos.row += 1;
	gotoxy(pos);
	printf("오브젝트 이름: 돌,바위");
	pos.row += 1;
	gotoxy(pos);
	printf("이 위치로는 유닛이 지나갈 수 없다.");
}
void sandworm_info(void) {
	POSITION pos;
	pos.row = 2;
	pos.column = MAP_WIDTH + 4;
	gotoxy(pos);
	printf("- 유닛 정보 -");
	pos.row += 1;
	gotoxy(pos);
	printf("오브젝트 이름: 샌드웜");
	pos.row += 1;
	gotoxy(pos);
	printf("오브젝트 정보: 유닛");
}
void plate_info(void) {
	POSITION pos;
	pos.row = 2;
	pos.column = MAP_WIDTH + 4;
	gotoxy(pos);
	printf("- 정보 - ");
	pos.row += 1;
	gotoxy(pos);
	printf("오브젝트 이름: 장판");
	pos.row += 1;
	gotoxy(pos);
	printf("이 위치에 건물을 지을수있다.");
}
void spice_info(void) {
	POSITION pos;
	pos.row = 2;
	pos.column = MAP_WIDTH + 4;
	gotoxy(pos);
	printf("- 자원 정보 -");
	pos.row += 1;
	gotoxy(pos);
	printf("오브젝트 이름: 스파이스");
	pos.row += 1;
	gotoxy(pos);
	printf("오브젝트 정보: 건물(자원)");
}
void desert_info(void) {
	POSITION pos;
	pos.row = 2;
	pos.column = MAP_WIDTH + 4;
	gotoxy(pos);
	printf("- 정보 -");
	pos.row += 1;
	gotoxy(pos);
	printf("오브젝트 이름: 사막");
	pos.row += 1;
	gotoxy(pos);
	printf("이 위치엔 건물을 지을수 없다.");
}


/*========명령어 출력========*/
void normally_command(void) {
	POSITION pos;
	pos.row = MAP_HEIGHT + 2;
	pos.column = MAP_WIDTH + 4;
	pos.row += 1;
	gotoxy(pos);
	printf("명령어:");
	pos.row += 1;
	gotoxy(pos);
	printf("\n");
	pos.row += 1;
	gotoxy(pos);
	printf("B: 건물 건설");
}
void command_ally_base(void) {
	POSITION pos;
	pos.row = MAP_HEIGHT + 2;
	pos.column = MAP_WIDTH + 4;
	pos.row += 1;
	gotoxy(pos);
	printf("명령어:");
	pos.row += 1;
	gotoxy(pos);
	printf("\n");
	pos.row += 1;
	gotoxy(pos);
	printf("H: 하베스터 생산");
}
void command_ally_harvester(void) {
	POSITION pos;
	pos.row = MAP_HEIGHT + 2;
	pos.column = MAP_WIDTH + 4;
	pos.row += 1;
	gotoxy(pos);
	printf("명령어:");
	pos.row += 1;
	gotoxy(pos);
	printf("\n");
	pos.row += 1;
	gotoxy(pos);
	printf("H: Harvest, M: Move");
}

/*=========상태창 정보 출력==========*/
const char* many_massage[ALL_MESSAGE];
const char* backbuf_massage = NULL;

// 상태 메세지를 제일 아래에 출력함.
void system_message(const char* mes) {
	POSITION pos;
	
	//시스템 메세지 칸 비우기
	for (int i = 3; i < SYSTEM_MES_HEIGHT + 1; i++) { 
		for (int j = 1; j < SYSTEM_MES_WIDTH - 1; j++) {
			pos.row = MAP_HEIGHT + i;
			pos.column = j;
			gotoxy(pos);
			printf(" ");
		}
	}
	//출력 시작 고정
	pos.column = 2;

	if (backbuf_massage != NULL) {
		// 전에 출력된 메세지들 한칸 위로 이동
		for (int i = 0; i < ALL_MESSAGE - 1; i++) {
			many_massage[i] = many_massage[i + 1];
		}
		many_massage[ALL_MESSAGE - 1] = backbuf_massage;
	}

	// 전에 출력된 메세지들 밀어서 출력
	for (int i = 0; i < ALL_MESSAGE; i++) {
		if (many_massage[i] != NULL) {
			pos.row = MAP_HEIGHT + 3 + i;
			gotoxy(pos);
			printf("%s", many_massage[i]);
		}
	}

	// 새 메세지 맨 아래 출력
	pos.row = MAP_HEIGHT + 8;
	gotoxy(pos);
	printf("%s", mes);
	backbuf_massage = mes;

}

/*===========하베스터 생산===========*/
void create_harvester(RESOURCE *resource) {
	POSITION pos;
	char current_char = check_cursor_position();

	if (current_char == 'B') {
		if (resource->spice >= 5 && resource->population + 5 <= resource->population_max) {
			pos.row = MAP_HEIGHT - 4;
			pos.column = MAP_WIDTH - (MAP_WIDTH - 2);
			map[1][pos.row][pos.column] = 'H';
			resource->spice -= 5;
			resource->population += 5;
			system_message("본진에서 성공적으로 하베스터를 생성했습니다.");
		}
		else {
			system_message("자원 또는 인구수가 부족합니다.");
		}
	}
}