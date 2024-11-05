#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"

void init(void);
void system_msg(void);
void game_map(void);
void intro(void);
void outro(void);
void cursor_move(DIRECTION dir);
void sample_obj_move(void);
POSITION sample_obj_next_position(void);


/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { {1, 1}, {1, 1} };


/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };

RESOURCE resource = { 
	.spice = 0,
	.spice_max = 0,
	.population = 0,
	.population_max = 0
};

OBJECT_SAMPLE obj = {
	.pos = {1, 1},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'o',
	.move_period = 300,
	.next_move_time = 300
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
	.B = {MAP_HEIGHT - 6, MAP_WIDTH - (MAP_WIDTH + 24)}
};

/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));

	init();
	//system_msg();
	game_map();
	intro();
	display(resource, map, cursor);

	while (1) {
		// loop 돌 때마다(즉, TICK==10ms마다) 키 입력 확인
		KEY key = get_key();

		// 키 입력이 있으면 처리
		if (is_arrow_key(key)) {
			cursor_move(ktod(key));
		}
		else {
			// 방향키 외의 입력
			switch (key) {
			case k_quit: outro();
			case k_none:
			case k_undef:
			default: break;
			}
		}

		// 샘플 오브젝트 동작
		sample_obj_move();

		// 화면 출력
		display(resource, map, cursor);
		Sleep(TICK);
		sys_clock += 10;
	}
}

/* ================= subfunctions =================== */
void intro(void) {
	printf("Dune 1.5");
	Sleep(2000);
	system("cls");
}

void outro(void) {
	printf("exiting...\n");
	exit(0);
}

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
	
	// object sample
	map[1][obj.pos.row][obj.pos.column] = 'o';
}

//시스템 메세지(수정 필요)
/*void system_mes(void) {
	for (int i = 0; i < SYSTEM_MES_HEIGHT - 1; i++) {
		for (int j = 0; j < SYSTEM_MES_WIDTH; j++) {
			for (int k = 0; k < SYSTEM_MES_WIDTH - 1; k++) {
				map[0][0][j] = '#';
				map[0][MAP_HEIGHT - 1][j] = '#';
				map[0][i][0] = '#';
				map[0][i][MAP_WIDTH - 1] = '#';
				map[0][i][j] = ' ';
			}
		}
	}
}*/



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