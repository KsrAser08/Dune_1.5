#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"

/*================= 함수 정렬 =================*/
void init(void);
void game_map(void);
void intro(void);
void outro(void);
void cursor_move(DIRECTION dir);

void clear_messages(void);

void sample_obj_move(void);
void sandworm_obj_move(int sandworm_make_spice1);
void sandworm1_obj_move(int sandworm_make_spice2);

POSITION near_unit(POSITION pos); // 가장 가까운 유닛 구하기
POSITION sample_obj_next_position(void); // E 움직임
POSITION sandworm_obj_next_position(void); // 샌드웜 움직임
POSITION sandworm1_obj_next_position(void); // 샌드웜 움직임

void space_prass(void); // 스페이스바를 눌렀을때
void ESC_prass(void); // ESC를 눌렀을때
void B_prass(RESOURCE* resource, CURSOR* cursor); // B를 눌렀을때

char check_cursor_position(void); // 현재 커서에 있는 문자를 인식
void ally_base_info(void); // 아군 베이스 현재 정보
void ally_harvester_info(void); // 아군 하베스터 현재 정보
void stone_rock_info(void); // 돌, 바위 현재 정보
void sandworm_info(void); // 샌드웜 현재 정보
void plate_info(void); // 장판 현재 정보
void spice_info(void); // 스파이스 현재 정보
void desert_info(void); // 사막 정보 표시

/*추가선언*/
void harvester_harvest(void);
POSITION move_harvester(void);
POSITION find_nearest_spice(POSITION pos);


/*========== 명령창 ==========*/
void normally_command(void);
void command_ally_base(void);
void command_ally_harvester(void);
void command_can_build(void);
/*========= 생산 & 건설 =========*/
void create_harvester(RESOURCE *resource);
bool is_cursor_all_on_P(POSITION current_pos, CURSOR* cursor, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
bool is_cursor_all_on_sand(POSITION current_pos, CURSOR* cursor, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
/*========= 시스템 메세지 출력 =========*/
void system_message(const char* mes);


/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
int cursor_clock = 0;
int build = 0;

CURSOR cursor = { {1, 1}, {1, 1}, .size = 1 };
/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };
char system_map[SYSTEM_MES_HEIGHT][SYSTEM_MES_WIDTH] = { 0 };
char status_map[STATUS_HEIGHT][STATUS_WIDTH] = { 0 };
char consol_map[CONSOL_HEIGHT][CONSOL_WIDTH] = { 0 };

//자원 정보
RESOURCE resource = {  
.spice = 10,
.spice_max = 20,
.population = 5,
.population_max = 20
}; 

/*=============== 유닛 동작 ===============*/

OBJECT_SAMPLE obj = {
.pos = {1, 1},
.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
.repr = 'E',
.move_period = 300,
.next_move_time = 300
};

SANDWORM sandworm1 = {
.pos = {MAP_HEIGHT - (MAP_HEIGHT - 3), MAP_WIDTH - (MAP_WIDTH - 10)},
.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
.sandworm_repr = 'W',
.move_period = 1320,
.next_move_time = 1320
};

SANDWORM sandworm = {
.pos = {MAP_HEIGHT - 7, (MAP_WIDTH + 36) - MAP_WIDTH},
.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
.sandworm_repr = 'W',
.move_period = 1320,
.next_move_time = 1320
};

// 키보드 클릭에 관한 구조체 설정
CLICK click = {
	.on_click_space = 0,
	.on_click_H = 0
};

//하베스터의 선택과 행동에 관한 구조체 기본
HARVESTER harvester = {
	.harvester_harvest = 0, //하베스터 수확명령이 떨어졌는가

	.harvester_repr = 'H',
	.move_period = 700,
	.next_move_time = 700
};

//아군 베이스 설정 'B'
OBJECT_BUILDING obj_ally_build = {
.repr = 'B',
.layer = 0,
.A = {MAP_HEIGHT - 3, MAP_WIDTH - (MAP_WIDTH - 1)},
.B = {MAP_HEIGHT - 2, MAP_WIDTH - (MAP_WIDTH - 1)},
.C = {MAP_HEIGHT - 3, MAP_WIDTH - (MAP_WIDTH - 2)},
.D = {MAP_HEIGHT - 2, MAP_WIDTH - (MAP_WIDTH - 2)},
.formation = 1
};
//적군 베이스 설정 'B'
OBJECT_BUILDING obj_enemy_build = {
.repr = 'B',
.layer = 0,
.A = {MAP_HEIGHT - (MAP_HEIGHT - 2), MAP_WIDTH - (MAP_WIDTH + 3)},
.B = {MAP_HEIGHT - (MAP_HEIGHT - 3), MAP_WIDTH - (MAP_WIDTH + 3)},
.C = {MAP_HEIGHT - (MAP_HEIGHT - 2), MAP_WIDTH - (MAP_WIDTH + 2)},
.D = {MAP_HEIGHT - (MAP_HEIGHT - 3), MAP_WIDTH - (MAP_WIDTH + 2)},
.formation = 2
};
//아군 하베스터 추가 'H'
OBJECT_BUILDING obj_ally_Harvester = {
.repr = 'H',
.layer = 1,
.A = {MAP_HEIGHT - 4, MAP_WIDTH - (MAP_WIDTH - 1)}, //아군 하베스터
.formation = 1
};
//적군 하베스터 추가 'H'
OBJECT_BUILDING obj_enemy_Harvester = {
.repr = 'H',
.layer = 1,
.A = {MAP_HEIGHT - (MAP_HEIGHT - 3), ((MAP_WIDTH * 2) - 2) - MAP_WIDTH}, //적군 하베스터
.formation = 2
};
//아군측, 적군측에 위치한 스파이스
OBJECT_BUILDING obj_spice = {
.repr = '5',
.layer = 0,
.A = {MAP_HEIGHT - 8, MAP_WIDTH - (MAP_WIDTH - 1)}, //아군측 스파이스
.B = {MAP_HEIGHT - (MAP_HEIGHT - 8), MAP_WIDTH - (MAP_WIDTH + 2)} //적군측 스파이스

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

//건물 선택
BUILDING building = {
	.barracks = 0, // 병영
	.dormitory = 0, // 숙소
	.garage = 0, // 창고
	.plate = 0, // 장판
	.shelter = 0 // 은신처
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
			case k_space: {
				if (cursor.size == 1) space_prass(); // 커서의 크기가 1일때만 정보출력
				else if (cursor.size == 2) { // 커서의 크기가 2일때만 건설조건 확인
					if (is_cursor_all_on_P(cursor.current, &cursor, map)) { //모두 P위에 있어, 'True'를 반환받기
						// 병영 건설
						if (building.barracks == 1 && resource.spice >= 4) { // B-B선택(병영건설)
							POSITION curr = cursor.current;
							int size = cursor.size;
							for (int i = 0; i < size; i++) {
								for (int j = 0; j < size; j++) {
									POSITION build_pos = { curr.row + i, curr.column + j };
									map[0][build_pos.row][build_pos.column] = 'b';  // 병영 출력
								}
							}
							building.plate = 0;
							cursor.size = 1;
							resource.spice -= 4;
							clear_messages();
							normally_command();
							system_message("성공적으로 병영이 건설되었습니다.");
						}
						// 은신처 건설
						else if (building.shelter == 1 && resource.spice >= 5) {
							POSITION curr = cursor.current;
							int size = cursor.size;
							for (int i = 0; i < size; i++) {
								for (int j = 0; j < size; j++) {
									POSITION build_pos = { curr.row + i, curr.column + j };
									map[0][build_pos.row][build_pos.column] = 'S';  // 병영 출력
								}
							}
							building.shelter = 0;
							cursor.size = 1;
							resource.spice -= 5;
							clear_messages();
							normally_command();
							system_message("성공적으로 은신처가 건설되었습니다.");
						}
						// 숙소 건설
						else if (building.dormitory == 1 && resource.spice >= 2) {
							POSITION curr = cursor.current;
							int size = cursor.size;
							for (int i = 0; i < size; i++) {
								for (int j = 0; j < size; j++) {
									POSITION build_pos = { curr.row + i, curr.column + j };
									map[0][build_pos.row][build_pos.column] = 'D';  // 병영 출력
								}
							}
							building.dormitory = 0;
							cursor.size = 1;
							resource.spice -= 2;
							resource.population_max += 10;
							clear_messages();
							normally_command();
							system_message("성공적으로 숙소가 건설되었습니다.");
						}
						// 창고 건설
						else if (building.garage == 1 && resource.spice >= 4) {
							POSITION curr = cursor.current;
							int size = cursor.size;
							for (int i = 0; i < size; i++) {
								for (int j = 0; j < size; j++) {
									POSITION build_pos = { curr.row + i, curr.column + j };
									map[0][build_pos.row][build_pos.column] = 'G';  // 창고 출력
								}
							}
							building.garage = 0;
							cursor.size = 1;
							resource.spice -= 4;
							resource.spice_max += 10;
							clear_messages();
							normally_command();
							system_message("성공적으로 창고가 건설되었습니다.");
						}
						// 장판 위에 장판을 지으려고 할때
						else if (building.plate == 1) system_message("장판 위에 장판을 건설할 수 없습니다.");
						// 자원이 조건을 충족하지 못했을 경우
						else system_message("자원이 부족합니다.");
					}
					else if (is_cursor_all_on_sand(cursor.current, &cursor, map)) {
						// 장판 설치(건설)
						int width = MAP_WIDTH / 2;
						if (building.plate == 1 && resource.spice >= 1) { // 장판 P
							POSITION curr = cursor.current;
							int size = cursor.size;
							for (int i = 0; i < size; i++) {
								for (int j = 0; j < size; j++) {
									POSITION build_pos = { curr.row + i, curr.column + j };
									map[0][build_pos.row][build_pos.column] = 'P';  // 장판 출력
								}
							}
							cursor.size = 1;
							resource.spice -= 1;
							clear_messages();
							normally_command();
							system_message("성공적으로 장판이 건설되었습니다.");
						}
						// 사막 위에 건물을 지으려고 할때
						else if (building.plate != 1) system_message("건물은 장판 위에 건설해야합니다.");
						else system_message("자원이 부족합니다.");
					}
					else system_message("위치가 올바르지 않습니다.");
				}
				break;
			}
			case k_h: {
				if (click.on_click_space == 1) {
					create_harvester(&resource);
					click.on_click_space = 0;
				}
				break;
			}
			case k_H: {
				if (click.on_click_space == 1) {
					create_harvester(&resource);
					click.on_click_space = 0;
				}
				break;
			}
			case k_b: B_prass(&resource, &cursor); break;
			case k_B: B_prass(&resource, &cursor); break;
			case k_esc:	ESC_prass(); break;
			case k_none:
			case k_undef:
			default: break;
			}

			// 샘플 오브젝트 동작
			sample_obj_move();
			// 샌드웜 오브젝트 움직임
			sandworm_obj_move(sandworm_make_spice1);
			sandworm1_obj_move(sandworm_make_spice2);
			// 수정 요함
			/*if (harvester.harvester_harvest == 1) {
				harvester_harvest();
			}*/
		
			// 화면 출력
			display(resource, map, system_map, status_map, consol_map, cursor);
			Sleep(TICK);
			sys_clock += 10;
		}
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

// # 모양 맵생성 & 독수리
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
	map[1][obj.pos.row][obj.pos.column] = 'E';
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
	map[obj_ally_Harvester.layer][obj_ally_Harvester.A.row][obj_ally_Harvester.A.column] = obj_ally_Harvester.repr;
	map[obj_enemy_Harvester.layer][obj_enemy_Harvester.A.row][obj_enemy_Harvester.A.column] = obj_enemy_Harvester.repr;
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
	if (cursor.size == 2) {
		if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 3 &&
			1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 3) {

			cursor.previous = cursor.current;  // 이전 위치 저장
			cursor.current = new_pos;          // 새 위치로 커서 이동
		}
	}
	else {
		// validation check
		if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 && \
			1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {

			cursor.previous = cursor.current;
			cursor.current = new_pos;
		}
	}
}

/* ================= sample object movement =================== */

//(샌드웜) 가장 가까운 유닛 구하는 함수
POSITION near_unit(POSITION pos) {
	int min_distance = MAP_WIDTH * MAP_HEIGHT;
	POSITION nearest_unit = { -1, -1 }; 

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			// 레이어 1에서 유닛('H') 찾기
			if (map[1][i][j] == 'H') {
				POSITION unit_pos = { i, j };

				// psub를 사용하여 두 위치의 차이를 계산
				POSITION diff = psub(pos, unit_pos); // pos와 유닛 위치의 벡터 차이 계산
				int distance = abs(diff.row) + abs(diff.column); // 맨해튼 거리 계산

				// 더 가까운 유닛이 발견되면 업데이트
				if (distance < min_distance) {
					min_distance = distance;
					nearest_unit = unit_pos;
				}
			}
		}
	}
	return nearest_unit;
}

/*추가 수정*/
//(하베스터) 가장 가까운 스파이스 찾는 함수

POSITION find_nearest_spice(POSITION pos) {
	int min_distance = MAP_WIDTH * MAP_HEIGHT;
	POSITION nearest_spice = { -1, -1 };

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			// 레이어 1에서 유닛('H') 찾기
			if (map[0][i][j] == '1' || map[0][i][j] == '2' || map[0][i][j] == '3' || map[0][i][j] == '4' || map[0][i][j] == '5' || map[0][i][j] == '6' || map[0][i][j] == '7') {
				POSITION spice_pos = { i, j };

				// psub를 사용하여 두 위치의 차이를 계산
				POSITION diff = psub(pos, spice_pos); // pos와 유닛 위치의 벡터 차이 계산
				int distance = abs(diff.row) + abs(diff.column); // 맨해튼 거리 계산

				// 더 가까운 유닛이 발견되면 업데이트
				if (distance < min_distance) {
					min_distance = distance;
					nearest_spice = spice_pos;
				}
			}
		}
	}
	return nearest_spice;
}
//하베스터 수확
void harvester_harvest(void) {
	POSITION prev = harvester.pos;  // 현재 위치 저장

	if (sys_clock <= harvester.next_move_time) {
		// 아직 시간이 안 됨
		return;
	}

	// 현재 위치를 비움
	map[1][harvester.pos.row][harvester.pos.column] = -1;

	// 다음 위치 계산
	harvester.pos = move_harvester();

	// 다음 위치로 이동
	map[1][harvester.pos.row][harvester.pos.column] = harvester.harvester_repr;

	

	harvester.next_move_time = sys_clock + harvester.move_period;
}
//하베스터 움직임 정하기
POSITION move_harvester(void) {
	POSITION nearest_spice = find_nearest_spice(harvester.pos);  // 가장 가까운 유닛 찾기

	if (nearest_spice.row == -1 && nearest_spice.column == -1) {
		sandworm1.adjacent_to_unit = 0;  // 인접 상태 초기화
		return harvester.pos;
	}

	if (abs(harvester.pos.row - nearest_spice.row) + abs(harvester.pos.column - nearest_spice.column) == 1) {
		if (harvester.adjacent_to_spice == 1) {
			if (nearest_spice.row >= 0 && nearest_spice.row < MAP_HEIGHT &&
				nearest_spice.column >= 0 && nearest_spice.column < MAP_WIDTH) {
				int random = rand() % 3 + 2;
				if (map[0][nearest_spice.row][nearest_spice.column] >= '1' && map[0][nearest_spice.row][nearest_spice.column] <= '7') {
					map[0][nearest_spice.row][nearest_spice.column] -= random;
					harvester.adjacent_to_spice = 0;  // 인접 상태 초기화
					return harvester.pos;
				}
			}
			else {
				harvester.adjacent_to_spice = 1;
				return harvester.pos;  // 현재 위치 유지
			}
		}

		harvester.adjacent_to_spice = 0;

		harvester.dest = nearest_spice;

		POSITION diff = psub(harvester.dest, harvester.pos);

		DIRECTION dirs[2];
		if (abs(diff.row) >= abs(diff.column)) {
			dirs[0] = (diff.row > 0) ? d_down : d_up;
			dirs[1] = (diff.column > 0) ? d_right : d_left;
		}
		else {
			dirs[0] = (diff.column > 0) ? d_right : d_left;
			dirs[1] = (diff.row > 0) ? d_down : d_up;
		}

		for (int i = 0; i < 2; i++) {
			POSITION next_pos = pmove(harvester.pos, dirs[i]);

			if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 &&
				1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 &&
				map[1][next_pos.row][next_pos.column] < 0) {
				if (map[1][next_pos.row][next_pos.column] != 'R' &&
					map[1][next_pos.row][next_pos.column] != 'P') {
					return next_pos;
				}
			}
		}

		return harvester.pos;  // 유효한 이동 경로가 없으면 제자리 유지
	}
}


// 'o'유닛 이동 (나중에 레이어2(공중)에 독수리(?)로 만들예정)
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

/*============ 유닛 이동 관련 ============*/
// 우측 하단 샌드웜 이동
POSITION sandworm_obj_next_position(void) {
	int witch = MAP_HEIGHT / 2;
	POSITION nearest_unit = near_unit(sandworm.pos);  // 가장 가까운 유닛 찾기

	if (nearest_unit.row == -1 && nearest_unit.column == -1) {
		sandworm.adjacent_to_unit = 0;  // 인접 상태 초기화
		return sandworm.pos;
	}

	if (abs(sandworm.pos.row - nearest_unit.row) + abs(sandworm.pos.column - nearest_unit.column) == 1) {
		if (sandworm.adjacent_to_unit == 1) {
			if (nearest_unit.row >= 0 && nearest_unit.row < MAP_HEIGHT &&
				nearest_unit.column >= 0 && nearest_unit.column < MAP_WIDTH) {
				map[1][nearest_unit.row][nearest_unit.column] = ' ';  // 유닛 제거 (빈 공간으로 설정)
				sandworm.pos = nearest_unit;  // 샌드웜의 위치를 유닛 위치로 변경
				sandworm.adjacent_to_unit = 0;  // 인접 상태 초기화
				system_message("샌드웜이 유닛을 먹었습니다!");  // 시스템 메시지
				if (nearest_unit.row > witch) resource.population -= 5;
				return sandworm.pos;
			}
		}
		else {
			sandworm.adjacent_to_unit = 1;
			return sandworm.pos;  // 현재 위치 유지
		}
	}

	sandworm.adjacent_to_unit = 0;

	sandworm.dest = nearest_unit;

	POSITION diff = psub(sandworm.dest, sandworm.pos);

	DIRECTION dirs[2];
	if (abs(diff.row) >= abs(diff.column)) {
		dirs[0] = (diff.row > 0) ? d_down : d_up;
		dirs[1] = (diff.column > 0) ? d_right : d_left;
	}
	else {
		dirs[0] = (diff.column > 0) ? d_right : d_left;
		dirs[1] = (diff.row > 0) ? d_down : d_up;
	}

	for (int i = 0; i < 2; i++) {
		POSITION next_pos = pmove(sandworm.pos, dirs[i]);

		if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 &&
			1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 &&
			map[1][next_pos.row][next_pos.column] < 0) {
			if (map[1][next_pos.row][next_pos.column] != 'R' &&
				map[1][next_pos.row][next_pos.column] != 'P') {
				return next_pos;
			}
		}
	}

	return sandworm.pos;  // 유효한 이동 경로가 없으면 제자리 유지
}

// 좌측 상단 샌드웜 이동
POSITION sandworm1_obj_next_position(void) {
	int witch = MAP_HEIGHT / 2;
	POSITION nearest_unit = near_unit(sandworm1.pos);  // 가장 가까운 유닛 찾기

	if (nearest_unit.row == -1 && nearest_unit.column == -1) {
		sandworm1.adjacent_to_unit = 0;  // 인접 상태 초기화
		return sandworm1.pos;
	}

	if (abs(sandworm1.pos.row - nearest_unit.row) + abs(sandworm1.pos.column - nearest_unit.column) == 1) {
		if (sandworm1.adjacent_to_unit == 1) {
			if (nearest_unit.row >= 0 && nearest_unit.row < MAP_HEIGHT &&
				nearest_unit.column >= 0 && nearest_unit.column < MAP_WIDTH) {
				map[1][nearest_unit.row][nearest_unit.column] = ' ';  // 유닛 제거 (빈 공간으로 설정)
				sandworm1.pos = nearest_unit;  // 샌드웜1의 위치를 유닛 위치로 변경
				sandworm1.adjacent_to_unit = 0;  // 인접 상태 초기화
				system_message("샌드웜이 유닛을 먹었습니다!");  // 시스템 메시지
				if (nearest_unit.row > witch) resource.population -= 5;
				return sandworm1.pos;
			}
		}
		else {
			sandworm1.adjacent_to_unit = 1;
			return sandworm1.pos;  // 현재 위치 유지
		}
	}

	sandworm1.adjacent_to_unit = 0;

	sandworm1.dest = nearest_unit;

	POSITION diff = psub(sandworm1.dest, sandworm1.pos);

	DIRECTION dirs[2];
	if (abs(diff.row) >= abs(diff.column)) {
		dirs[0] = (diff.row > 0) ? d_down : d_up;
		dirs[1] = (diff.column > 0) ? d_right : d_left;
	}
	else {
		dirs[0] = (diff.column > 0) ? d_right : d_left;
		dirs[1] = (diff.row > 0) ? d_down : d_up;
	}

	for (int i = 0; i < 2; i++) {
		POSITION next_pos = pmove(sandworm1.pos, dirs[i]);

		if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 &&
			1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 &&
			map[1][next_pos.row][next_pos.column] < 0) {
			if (map[1][next_pos.row][next_pos.column] != 'R' &&
				map[1][next_pos.row][next_pos.column] != 'P') {
				return next_pos;
			}
		}
	}

	return sandworm1.pos;  // 유효한 이동 경로가 없으면 제자리 유지
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
	POSITION prev = sandworm.pos;  // 현재 위치 저장

	if (sys_clock <= sandworm.next_move_time) {
		// 아직 시간이 안 됨
		return;
	}

	// 현재 위치를 비움
	map[1][sandworm.pos.row][sandworm.pos.column] = -1;

	// 다음 위치 계산
	sandworm.pos = sandworm_obj_next_position();

	// 다음 위치로 이동
	map[1][sandworm.pos.row][sandworm.pos.column] = sandworm.sandworm_repr;

	// 이동했을 경우만 스파이스 생성 (이전 위치와 현재 위치가 다를 때)
	if (prev.row != sandworm.pos.row || prev.column != sandworm.pos.column) {
		sandworm_make_spice1 = rand() % 100 + 1;  // 1 ~ 100
		if (sandworm_make_spice1 <= 5) {  // 5% 확률로 스파이스 생성
			int spice_ran = rand() % 5 + 3;  // '3' ~ '7'
			map[1][prev.row][prev.column] = '0' + spice_ran;
			system_message("샌드웜이 스파이스를 생성했습니다.");
		}
	}

	sandworm.next_move_time = sys_clock + sandworm.move_period;
}

void sandworm1_obj_move(int sandworm_make_spice2) {
	POSITION prev = sandworm1.pos;  // 현재 위치 저장

	if (sys_clock <= sandworm1.next_move_time) {
		// 아직 시간이 안 됨
		return;
	}

	// 현재 위치를 비움
	map[1][sandworm1.pos.row][sandworm1.pos.column] = -1;

	// 다음 위치 계산
	sandworm1.pos = sandworm1_obj_next_position();

	// 다음 위치로 이동
	map[1][sandworm1.pos.row][sandworm1.pos.column] = sandworm1.sandworm_repr;

	// 이동했을 경우만 스파이스 생성 (이전 위치와 현재 위치가 다를 때)
	if (prev.row != sandworm1.pos.row || prev.column != sandworm1.pos.column) {
		sandworm_make_spice2 = rand() % 100 + 1;  // 1 ~ 100
		if (sandworm_make_spice2 <= 5) {  // 5% 확률로 스파이스 생성
			int spice_ran = rand() % 5 + 3;  // '3' ~ '7'
			map[1][prev.row][prev.column] = '0' + spice_ran;
			system_message("샌드웜이 스파이스를 생성했습니다.");
		}
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
	for (int i = 3; i < CONSOL_HEIGHT + 1; i++) {
		for (int j = 4; j < CONSOL_WIDTH + 1; j++) {
			pos.row = MAP_HEIGHT + i;
			pos.column = MAP_WIDTH + j;
			gotoxy(pos);
			printf(" ");
		}
	}
}

/*================= 입력 문자 확인 ====================*/
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
	//아군 베이스를 인식했을때
	if (current_char == 'B') {
		clear_messages();
		ally_base_info();
		command_ally_base();
		system_message("베이스를 선택했습니다.");
		click.on_click_space = 1;
	}
	else if (current_char == 'H') {
		clear_messages();
		ally_harvester_info();
		command_ally_harvester();
		system_message("하베스터를 선택했습니다.");
		click.on_click_H = 1;
	}
	else if (current_char == 'R') {
		clear_messages();
		stone_rock_info();
	}
	else if (current_char == 'W') {
		clear_messages();
		sandworm_info();
	}
	else if (current_char == '3' || current_char == '4' || current_char == '5' ||
		current_char == '6' || current_char == '7') {
		clear_messages();
		spice_info();
	}
	else if (current_char == 'P') {
		clear_messages();
		plate_info();
	}
	/*else if (current_char == ' ') {
		clear_messages();
		desert_info();
	}*/
	
}
// ESC를 눌렀을때
void ESC_prass(void) {
	/*================ 구조체 값 정리 ================*/
	cursor.size = 1;	// 커서 1x1사이즈로 초기화
	building.barracks = 0; // 건물 선택 관련 구조체값 0으로 되돌리기
	building.dormitory = 0;
	building.garage = 0;
	building.plate = 0;
	building.shelter = 0;
	// 하베스터 선택 초기화
	click.on_click_space = 0;
	click.on_click_H = 0;
	/*===============================================*/
	display_map();
	system_message("선택을 취소합니다.");
	POSITION pos;
	for (int i = 2; i < STATUS_HEIGHT - 1; i++) {
		for (int j = 4; j < STATUS_WIDTH - 2; j++) {
			pos.row = i;
			pos.column = MAP_WIDTH + j;
			gotoxy(pos);
			printf(" ");
		}
	}
	for (int i = 3; i < CONSOL_HEIGHT + 1; i++) {
		for (int j = 3; j < CONSOL_WIDTH + 1; j++) {
			pos.row = MAP_HEIGHT + i;
			pos.column = MAP_WIDTH + j;
			gotoxy(pos);
			printf(" ");
		}
	}
	normally_command();
}
// 사막 위에 건물 짓기위해 B 눌렀을때
void B_prass(RESOURCE* resource, CURSOR* cursor) {
	clear_messages();
	char current_char = check_cursor_position();
	if (current_char == ' ') { // 커서가 사막(건물X)에 위치해있을때
		command_can_build(); // 사막 위 건설 가능한 건물 출력
		unsigned char k;
		k = _getch(); // 키 입력 대기
		// 장판 건설
		if (k == 'p' || k == 'P') {
			cursor->size = 2; //커서 크기를 2X2로 변경
			building.plate = 1;
			system_message("건물을 건설할 위치로 이동 후 스페이스바 입력 (취소: ESC)");
	}
		// 병영 건설
		else if (k == 'b' || k == 'B') { 
			cursor->size = 2; //커서 크기를 2X2로 변경
			building.barracks = 1;
			system_message("건물을 건설할 위치로 이동 후 스페이스바 입력 (취소: ESC)");
		}
		// 은신처 건설
		else if (k == 's' || k == 'S') {
			cursor->size = 2; //커서 크기를 2X2로 변경
			building.shelter = 1;
			system_message("건물을 건설할 위치로 이동 후 스페이스바 입력 (취소: ESC)");
		}
		// 숙소 건설
		else if (k == 'd' || k == 'D') { 
			cursor->size = 2; //커서 크기를 2X2로 변경
			building.dormitory = 1;
			system_message("건물을 건설할 위치로 이동 후 스페이스바 입력 (취소: ESC)");
		}
		// 창고 건설
		else if (k == 'g' || k == 'G') {
			cursor->size = 2; //커서 크기를 2X2로 변경
			building.garage = 1;
			system_message("건물을 건설할 위치로 이동 후 스페이스바 입력 (취소: ESC)");
		}

	}
}

/*========== 유닛, 오브젝트, 건물 정보 출력 ===========*/
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


/*======== 명령어 출력 ========*/
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
	printf("H: 스파이스 수확하기, M: 하베스터 움직이기");
}
void command_can_build(void){
	POSITION pos;
	pos.row = MAP_HEIGHT + 2;
	pos.column = MAP_WIDTH + 4;
	pos.row += 1;
	gotoxy(pos);
	printf("건설 가능한 건물 목록 : ");
	pos.row += 1;
	gotoxy(pos);
	printf("\n");
	pos.row += 1;
	gotoxy(pos);
	printf("P: 장판 건설	G: 창고 건설	S: 은신처 건설");
	pos.row += 1;
	gotoxy(pos);
	printf("\n");
	pos.row += 1;
	gotoxy(pos);
	printf("D: 숙소 건설	B: 병영 건설");
	
}

/*========= 상태창 정보 출력 ==========*/
const char* many_massage[ALL_MESSAGE];
const char* backbuf_massage = NULL;

/*======= 상태메세지 출력 ========*/
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

/*=========== 생산 & 건설 ===========*/
// 하베스터 생산
void create_harvester(RESOURCE *resource) {
	POSITION pos;
	char current_char = check_cursor_position();
	if (click.on_click_space == 1 && current_char == 'B') {
		if (resource->spice >= 5 && resource->population + 5 <= resource->population_max) {
			pos.row = MAP_HEIGHT - 4;
			pos.column = MAP_WIDTH - (MAP_WIDTH - 2);
			if (map[1][pos.row][pos.column] != 'H') {
				map[1][pos.row][pos.column] = 'H'; // 하베스터 생성
				resource->spice -= 5;
				resource->population += 5;
				clear_messages();
				system_message("하베스터가 성공적으로 생성되었습니다.");
			}
			else system_message("하베스터가 이미 생성되어 있습니다.");
		}
		else {
			system_message("자원 또는 인구수가 부족합니다.");
		}
	}
}
// 2X2 커서가 모두 P위에 있는지 검사하는 함수
bool is_cursor_all_on_P(POSITION current_pos, CURSOR* cursor, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	for (int i = 0; i < cursor->size; i++) {
		for (int j = 0; j < cursor->size; j++) {
			POSITION check_pos = { current_pos.row + i, current_pos.column + j };
			// 해당 위치가 'P'인지 확인
			if (map[0][check_pos.row][check_pos.column] != 'P') {
				return false;  // 'P'가 아닌 곳이 있으면 실패
			}
		}
	}
	return true;  // 커서의 4면 모두 'P' 위에 있으면 성공
}
// 2x2커서가 모두 사막위에 있는지 검사하는 함수 (장판전용)
bool is_cursor_all_on_sand(POSITION current_pos, CURSOR * cursor, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	for (int i = 0; i < cursor->size; i++) {
		for (int j = 0; j < cursor->size; j++) {
			POSITION check_pos = { current_pos.row + i, current_pos.column + j };
			// 해당 위치가 'P'인지 확인
			if (map[0][check_pos.row][check_pos.column] != ' ') {
				return false;  // 'P'가 아닌 곳이 있으면 실패
			}
		}
	}
	return true;  // 커서의 4면 모두 'P' 위에 있으면 성공
}
