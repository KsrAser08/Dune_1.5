#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include <conio.h>
#include <assert.h>

/* ================= system parameters =================== */
#define TICK 10		// time unit(ms)

#define N_LAYER 2
#define MAP_WIDTH	60	// 맵의 크기
#define MAP_HEIGHT	18

#define SYSTEM_MES_WIDTH	60	// 시스템 메세지의 크기
#define SYSTEM_MES_HEIGHT	8

#define STATUS_WIDTH	50 // 상태창의 크기
#define STATUS_HEIGHT	18

#define CONSOL_WIDTH	50 // 명령창의 크기
#define CONSOL_HEIGHT	8

#define ALL_MESSAGE 5 // 최대 시스템 메세지 갯수

#define MAX_UNIT 100  // 최대 유닛 수 정의



/* ================= 위치와 방향 =================== */
// 맵에서 위치를 나타내는 구조체
typedef struct {
	int row, column;
} POSITION;

// 커서 위치
typedef struct {
	POSITION previous;  // 직전 위치
	POSITION current;  // 현재 위치
	int size; // 커서 크기
} CURSOR;

// 입력 가능한 키 종류.
// 수업에서 enum은 생략했는데, 크게 어렵지 않으니 예제 검색
typedef enum {
	// k_none: 입력된 키가 없음. d_stay(안 움직이는 경우)에 대응
	k_none = 0, k_up, k_right, k_left, k_down,
	k_quit,
	k_undef, // 정의되지 않은 키 입력	
	k_space, //스페이스바 입력
	k_H, k_h, // H입력
	k_esc, // ESC입력
	k_b, k_B, // B입력
	k_s, k_S, // s입력
	k_f, k_F // k입력
} KEY;


// DIRECTION은 KEY의 부분집합이지만, 의미를 명확하게 하기 위해서 다른 타입으로 정의
typedef enum {
	d_stay = 0, d_up, d_right, d_left, d_down
} DIRECTION;

//아군 적군 타입나누기
typedef enum {
	ally = 0, neutral, enemy
} TYPE_POS;

/* ================= 위치와 방향(2) =================== */
// 편의성을 위한 함수들. KEY, POSITION, DIRECTION 구조체들을 유기적으로 변환

// 편의성 함수
inline POSITION padd(POSITION p1, POSITION p2) {
	POSITION p = { p1.row + p2.row, p1.column + p2.column };
	return p;
}

// p1 - p2
inline POSITION psub(POSITION p1, POSITION p2) {
	POSITION p = { p1.row - p2.row, p1.column - p2.column };
	return p;
}


// 방향키인지 확인하는 함수
#define is_arrow_key(k)		(k_up <= (k) && (k) <= k_down)
// 더블클릭을 확인하기 위한 함수
#define is_arrow_key2(n)		(k_up <= (n) && (n) <= k_down)

// 화살표 '키'(KEY)를 '방향'(DIRECTION)으로 변환. 정수 값은 똑같으니 타입만 바꿔주면 됨
#define ktod(k)		(DIRECTION)(k)

// DIRECTION을 POSITION 벡터로 변환하는 함수
inline POSITION dtop(DIRECTION d) {
	static POSITION direction_vector[] = { {0, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 0} };
	return direction_vector[d];
}

// p를 d 방향으로 이동시킨 POSITION
#define pmove(p, d)		(padd((p), dtop(d)))

/* ================= game data =================== */
typedef struct {
	int spice;		// 현재 보유한 스파이스
	int spice_max;  // 스파이스 최대 저장량
	int population; // 현재 인구 수
	int population_max;  // 수용 가능한 인구 수
} RESOURCE;


// 대강 만들어 봤음. 기능 추가하면서 각자 수정할 것
typedef struct {
	POSITION pos;		// 현재 위치(position)
	POSITION dest;		// 목적지(destination)
	char repr;			// 화면에 표시할 문자(representation)
	int move_period;	// '몇 ms마다 한 칸 움직이는지'를 뜻함
	int next_move_time;	// 다음에 움직일 시간
} OBJECT_SAMPLE;

// 건물 관련
typedef struct {
	POSITION A,B,C,D; //건물 칸수 2 X 2
	char repr; //화면에 표시할 문자 - 건물 이름
	int layer; //어느 레이어인지 표시
	int formation; //아군, 적군중 어디인지 확인하기 위함 / 1 = 아군, 2 = 적군
}OBJECT_BUILDING;

//샌드웜 관련
typedef struct {
	POSITION pos;		// 현재 위치(position)
	POSITION dest;		// 목적지(destination)
	char sandworm_repr;	// 화면에 표시할 문자(representation)
	int adjacent_to_unit; // 유닛과 인접한 상태인지 여부 (0: 아님, 1: 맞음)
	int move_period;	// '몇 ms마다 한 칸 움직이는지'를 뜻함
	int next_move_time;	// 다음에 움직일 시간
}SANDWORM;

//하베스터 관련
typedef struct {
	POSITION pos;		// 현재 위치(position)
	POSITION dest;		// 목적지(destination)
	int adjacent_to_spice; // 스파이스과 인접한 상태인지 여부 (0: 아님, 1: 맞음)
	int harvester_harvest;  // 자원 채굴 상태 추적 (0: 대기, 1: 수확 중)
	char harvester_repr;
	int move_period;  // 이동 주기 (단위: 시간)
	int next_move_time;  // 하베스터가 다음으로 이동할 시간
}HARVESTER;

typedef struct {
	int on_click_H;
	int on_click_space;
	int on_click_b;
	int on_click_S;
}CLICK;

typedef struct {
	int plate;
	int dormitory;
	int garage;
	int barracks;
	int shelter;
}BUILDING;
#endif
