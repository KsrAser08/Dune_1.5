/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#include "display.h"
#include "io.h"

// 출력할 내용들의 좌상단(topleft) 좌표
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };
const POSITION system_map_pos = { MAP_HEIGHT + 2, 0 };
const POSITION status_map_pos = { 1, MAP_WIDTH + 2 };
const POSITION consol_map_pos = { MAP_HEIGHT + 2, MAP_WIDTH + 2 };

char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

char system_mes_frontbuf[SYSTEM_MES_HEIGHT][SYSTEM_MES_WIDTH] = { 0 };
char system_mes_backbuf[SYSTEM_MES_HEIGHT][SYSTEM_MES_WIDTH] = { 0 };

char status_frontbuf[STATUS_HEIGHT][STATUS_WIDTH] = { 0 };
char status_backbuf[STATUS_HEIGHT][STATUS_WIDTH] = { 0 };

char consol_frontbuf[CONSOL_HEIGHT][CONSOL_WIDTH] = { 0 };
char consol_backbuf[CONSOL_HEIGHT][CONSOL_WIDTH] = { 0 };

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);

void system_mes_project(char src[SYSTEM_MES_HEIGHT][SYSTEM_MES_WIDTH], char dest[SYSTEM_MES_HEIGHT][SYSTEM_MES_WIDTH]);
void display_system_map(char system_map[SYSTEM_MES_HEIGHT][SYSTEM_MES_WIDTH]);

void status_project(char src[STATUS_HEIGHT][STATUS_WIDTH], char dest[STATUS_HEIGHT][STATUS_WIDTH]);
void display_status_map(char status_map[STATUS_HEIGHT][STATUS_WIDTH]);

void consol_project(char src[CONSOL_HEIGHT][CONSOL_WIDTH], char dest[CONSOL_HEIGHT][CONSOL_WIDTH]);
void display_consol_map(char consol_map[CONSOL_HEIGHT][CONSOL_WIDTH]);

void display_cursor(CURSOR cursor);


void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	char system_map[SYSTEM_MES_HEIGHT][SYSTEM_MES_WIDTH],
	char status_map[STATUS_HEIGHT][STATUS_WIDTH],
	char consol_map[CONSOL_HEIGHT][CONSOL_WIDTH],
	CURSOR cursor)
{ 
	display_resource(resource);
	display_map(map);
	display_cursor(cursor);
	display_system_map(system_map);
	display_status_map(status_map);
	display_consol_map(consol_map);
	// display_object_info()
	// display_commands()
	// ...
}

void display_resource(RESOURCE resource) {
	set_color(COLOR_RESOURCE);
	gotoxy(resource_pos);
	printf("spice = %d/%d, population=%d/%d\n",
		resource.spice, resource.spice_max,
		resource.population, resource.population_max
	);
}

// subfunction of draw_map()
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]) {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			for (int k = 0; k < N_LAYER; k++) {
				if (src[k][i][j] >= 0) {
					dest[i][j] = src[k][i][j];
				}
			}
		}
	}
}

void system_mes_project(char src[SYSTEM_MES_HEIGHT][SYSTEM_MES_WIDTH], char dest[SYSTEM_MES_HEIGHT][SYSTEM_MES_WIDTH]) {
	for (int i = 0; i < SYSTEM_MES_HEIGHT; i++) {
		for (int j = 0; j < SYSTEM_MES_WIDTH; j++) {
			if (src[i][j] >= 0) {
				dest[i][j] = src[i][j];
			}
		}
	}
}

void status_project(char src[STATUS_HEIGHT][STATUS_WIDTH], char dest[STATUS_HEIGHT][STATUS_WIDTH]) {
	for (int i = 0; i < STATUS_HEIGHT; i++) {
		for (int j = 0; j < STATUS_WIDTH; j++) {
			if (src[i][j] >= 0) {
				dest[i][j] = src[i][j];
			}
		}
	}
}

void consol_project(char src[CONSOL_HEIGHT][CONSOL_WIDTH], char dest[CONSOL_HEIGHT][CONSOL_WIDTH]) {
	for (int i = 0; i < CONSOL_HEIGHT; i++) {
		for (int j = 0; j < CONSOL_WIDTH; j++) {
			if (src[i][j] >= 0) {
				dest[i][j] = src[i][j];
			}
		}
	}
}

void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	project(map, backbuf);

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (frontbuf[i][j] != backbuf[i][j]) {
				//배경 색
				if (backbuf[i][j] == ' ') {
					POSITION pos = { i, j };
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT + 220);
				}
				//아군 베이스 색
				else if (backbuf[i][j] == 'B' && i > MAP_HEIGHT - (MAP_HEIGHT / 2)) {
					POSITION pos = { i, j };
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT + 15);
				}
				//적군 베이스 색
				else if (backbuf[i][j] == 'B') {
					POSITION pos = { i, j };
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT + 64);
				}
				//아군 하베스터 색(수정 필요)
				else if (backbuf[i][j] == 'H' && i > MAP_HEIGHT - (MAP_HEIGHT / 2)) {
					POSITION pos = { i, j };
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT + 15);
				}
				//적군 하베스터 색(수정 필요)
				else if (backbuf[i][j] == 'H' && i < MAP_HEIGHT - (MAP_HEIGHT / 2)) {
					POSITION pos = { i, j };
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT + 64);
				}
				//중립 장판 색
				else if (backbuf[i][j] == 'P') {
					POSITION pos = { i, j };
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT);
				}
				//중립 바위 색
				else if (backbuf[i][j] == 'R') {
					POSITION pos = { i, j };
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT + 128);
				}
				//자원 스파이스 색
				else if (backbuf[i][j] == '5' || backbuf[i][j] == '3') {
					POSITION pos = { i, j };
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT + 48);
				}
				//중립 샌드웜 색
				else if (backbuf[i][j] == 'W') {
					POSITION pos = { i, j };
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT + 96);
				}
				else {
					POSITION pos = { i, j };
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT);
				}
			}
			frontbuf[i][j] = backbuf[i][j];
		}
	}
}

void display_system_map(char system_map[SYSTEM_MES_HEIGHT][SYSTEM_MES_WIDTH]) {
	system_mes_project(system_map, system_mes_backbuf);

	for (int i = 0; i < SYSTEM_MES_HEIGHT; i++) {
		for (int j = 0; j < SYSTEM_MES_WIDTH; j++) {
			if (system_mes_frontbuf[i][j] != system_mes_backbuf[i][j]) {
				POSITION pos = { i, j };
				printc(padd(system_map_pos, pos), system_mes_backbuf[i][j], COLOR_DEFAULT);
			}
			system_mes_frontbuf[i][j] = system_mes_backbuf[i][j];
		}
	}
}

void display_status_map(char status_map[STATUS_HEIGHT][STATUS_WIDTH]) {
	status_project(status_map, status_backbuf);

	for (int i = 0; i < STATUS_HEIGHT; i++) {
		for (int j = 0; j < STATUS_WIDTH; j++) {
			if (status_frontbuf[i][j] != status_backbuf[i][j]) {
				POSITION pos = { i, j };
				printc(padd(status_map_pos, pos), status_backbuf[i][j], COLOR_DEFAULT);
			}
			status_frontbuf[i][j] = status_backbuf[i][j];
		}
	}
}

void display_consol_map(char consol_map[CONSOL_HEIGHT][CONSOL_WIDTH]) {
	consol_project(consol_map, consol_backbuf);

	for (int i = 0; i < CONSOL_HEIGHT; i++) {
		for (int j = 0; j < CONSOL_WIDTH; j++) {
			if (consol_frontbuf[i][j] != consol_backbuf[i][j]) {
				POSITION pos = { i, j };
				printc(padd(consol_map_pos, pos), consol_backbuf[i][j], COLOR_DEFAULT);
			}
			consol_frontbuf[i][j] = consol_backbuf[i][j];
		}
	}
}

// frontbuf[][]에서 커서 위치의 문자를 색만 바꿔서 그대로 다시 출력
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;

	char ch = frontbuf[prev.row][prev.column];

	//배경 색
	if (frontbuf[prev.row][prev.column] == ' ') {
		printc(padd(map_pos, prev), ch, COLOR_DEFAULT + 224);
	}
	//아군 본진 색
	else if (frontbuf[prev.row][prev.column] == 'B' && prev.row > MAP_HEIGHT - (MAP_HEIGHT / 2)) {
		printc(padd(map_pos, prev), ch, COLOR_DEFAULT + 15);
	}
	//적군 본진 색
	else if (frontbuf[prev.row][prev.column] == 'B') {
		printc(padd(map_pos, prev), ch, COLOR_DEFAULT + 64);
	}
	//아군 하베스터 색(수정 필요)
	else if (frontbuf[prev.row][prev.column] == 'H' && prev.row > MAP_HEIGHT - (MAP_HEIGHT / 2)) {
		printc(padd(map_pos, prev), ch, COLOR_DEFAULT + 15);
	}
	//적군 하베스터 색(수정 필요)
	else if (frontbuf[prev.row][prev.column] == 'H') {
		printc(padd(map_pos, prev), ch, COLOR_DEFAULT + 64);
	}
	//중립 장판 색
	else if (frontbuf[prev.row][prev.column] == 'P') {
		printc(padd(map_pos, prev), ch, COLOR_DEFAULT);
	}
	//중립 바위 색
	else if (frontbuf[prev.row][prev.column] == 'R') {
		printc(padd(map_pos, prev), ch, COLOR_DEFAULT + 128);
	}
	//자원 스파이스 색
	else if (frontbuf[prev.row][prev.column] == '5' || frontbuf[prev.row][prev.column] == '3') {
		printc(padd(map_pos, prev), ch, COLOR_DEFAULT + 48);
	}
	//중립 샌드웜 색
	else if (frontbuf[prev.row][prev.column] == 'W') {
		printc(padd(map_pos, prev), ch, COLOR_DEFAULT + 96);
	}
	else {
		printc(padd(map_pos, prev), ch, COLOR_DEFAULT);
	}
	ch = frontbuf[curr.row][curr.column];
	printc(padd(map_pos, curr), ch, COLOR_CURSOR);
}