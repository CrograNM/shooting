// ----------------------------------------------------------------------
// 탄막 회피 게임 (텍스트 콘솔)
// 
// - 구조체
// - 콘솔 그래픽
// - 키보드 입력 처리
// - 충돌 감지
// - 동적 객체 관리
// ----------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>

const int WIDTH{ 60 };
const int HEIGHT{ 30 };

// 탄환 구조체
typedef struct bullet {
	int x;
	int y;

	int prevX;
	int prevY;
	
	int direction; // 8방향 (0~7), 0: 상, 1: 우상, 2: 우, 3: 우하, 4: 하, 5: 좌하, 6: 좌, 7: 좌상

} Bullet;

// 탄환 정보
const int MAX_BULLETS { 50 };
Bullet bulletInfo[MAX_BULLETS];
int bulletNum;

// 플레이어 위치
int X_plain;
int Y_plain;

void CharacterPos(int x, int y) {
	COORD pos { x, y };
	SetConsoleCursorPosition(
		GetStdHandle(STD_OUTPUT_HANDLE), 
		pos
	);
}

void MoveCharacter() {
	CharacterPos(X_plain, Y_plain);
	printf("  "); // 이전 위치 지우기

	// 키 입력 처리
	if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
		X_plain -= 2; 
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		X_plain += 2;
	}
	if (GetAsyncKeyState(VK_UP) & 0x8000) {
		Y_plain--;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
		Y_plain++;
	}

	// 경계 검사
	// 맵도 그리기 때문에 특수문자의 크기까지 고려해서 더 작게 잡아야 함
	if (X_plain < 1) {
		X_plain = 1;
	}
	if (Y_plain < 1) {
		Y_plain = 1;
	}
	if (X_plain > WIDTH - 3) { // 특수문자 크기 고려 (특수문자의 폭은 2칸, 높이는 1칸)
		X_plain = WIDTH - 3;
	}
	if (Y_plain > HEIGHT - 2) { 
		Y_plain = HEIGHT - 2;
	}

	CharacterPos(X_plain, Y_plain);
	printf("◆"); // 해당 특수문자의 폭은 2칸임
}

void CreateNewBullet() {
	int startingPos;			// 상하좌우 시작 위치
	startingPos = rand() % 4;	// 0: 상, 1: 우, 2: 하, 3: 좌

	switch (startingPos) {
		case 0: // 상
			bulletInfo[bulletNum].x = 1 + rand() % (WIDTH - 2);
			bulletInfo[bulletNum].y = 1;
			bulletInfo[bulletNum].direction = 3 + rand() % 3; // 3: 우하, 4: 하, 5: 좌하
			break;
		case 1: // 우
			bulletInfo[bulletNum].x = WIDTH - 2;
			bulletInfo[bulletNum].y = 1 + rand() % (HEIGHT - 2);
			bulletInfo[bulletNum].direction = 5 + rand() % 3; // 5: 좌하, 6: 좌, 7: 좌상
			break;
		case 2: // 하
			bulletInfo[bulletNum].x = 1 + rand() % (WIDTH - 2);
			bulletInfo[bulletNum].y = HEIGHT - 2;
			bulletInfo[bulletNum].direction = (7 + rand() % 3) % 8; // 7: 좌상, 0: 상, 1: 우상
			break;
		case 3: // 좌
			bulletInfo[bulletNum].x = 1;
			bulletInfo[bulletNum].y = 1 + rand() % (HEIGHT - 2);
			bulletInfo[bulletNum].direction = 1 + rand() % 3; // 1: 우상, 2: 우, 3: 우하
			break;
	}
	bulletInfo[bulletNum].prevX = bulletInfo[bulletNum].x;
	bulletInfo[bulletNum].prevY = bulletInfo[bulletNum].y;

	bulletNum++;
}

void MoveBullet(int* px, int* py, int dir) {

	switch (dir) {
		case 0: // 상
			(*py) -= 1;
			break;
		case 1: // 우상
			(*px) += 1;
			(*py) -= 1;
			break;
		case 2: // 우
			(*px) += 2;
			break;
		case 3: // 우하
			(*px) += 1;
			(*py) += 1;
			break;
		case 4: // 하
			(*py) += 1;
			break;
		case 5: // 좌하
			(*px) -= 1;
			(*py) += 1;
			break;
		case 6: // 좌
			(*px) -= 2;
			break;
		case 7: // 좌상
			(*px) -= 1;
			(*py) -= 1;
			break;
	}
}

void CopyBullet(int a, int b) {
	bulletInfo[a] = bulletInfo[b];
}

void KillBullet(int index) {
	int i;
	for (i = index; i < bulletNum - 1; i++) {
		CopyBullet(i, i + 1);
	}
	bulletNum--;
}

int PrintBullet() {
	int i;
	for (i = 0; i < bulletNum; i++) {
		
		CharacterPos(bulletInfo[i].prevX, bulletInfo[i].prevY);
		printf(" "); // 이전 위치 지우기

		MoveBullet(&bulletInfo[i].x, &bulletInfo[i].y, bulletInfo[i].direction);

		// 화면 밖으로 나갔는지 검사
		if (bulletInfo[i].x < 1 || bulletInfo[i].x > WIDTH - 2 ||
			bulletInfo[i].y < 1 || bulletInfo[i].y > HEIGHT - 2) {
		
			KillBullet(i);
			CreateNewBullet();

			i--;
			continue;
		}

		// 플레이어와 충돌 검사
		if ((bulletInfo[i].x == X_plain || bulletInfo[i].x == X_plain + 1) && bulletInfo[i].y == Y_plain) {			
			return 1;
		}
		
		bulletInfo[i].prevX = bulletInfo[i].x;
		bulletInfo[i].prevY = bulletInfo[i].y;

		CharacterPos(bulletInfo[i].x, bulletInfo[i].y);
		printf("o");

	}
	return 0;
}

void drawMap() {
	int x, y;

	CharacterPos(0, 0);
	for (x = 0; x < WIDTH; x++) {
		printf("□");
	}

	CharacterPos(0, HEIGHT - 1);
	for (x = 0; x < WIDTH; x++) {
		printf("□");
	}

	for (y = 1; y < HEIGHT - 1; y++) {
		CharacterPos(0, y);
		printf("□");
		CharacterPos(WIDTH - 1, y);
		printf("□");
	}
}

void SetCursorInvisible() {
	CONSOLE_CURSOR_INFO cursorInfo;
	cursorInfo.dwSize = 1;
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(
		GetStdHandle(STD_OUTPUT_HANDLE), 
		&cursorInfo
	);
}

int main() {
	// 콘솔 커서 숨기기
	SetCursorInvisible();

	// 초기화
	srand(time(NULL));

	X_plain = WIDTH / 2;
	Y_plain = HEIGHT / 2;
	MoveCharacter();

	int i;
	int gameover = 0;
	for (i = 0; i < 5; i++)
		CreateNewBullet();

	drawMap();
	while (true) {
		MoveCharacter();

		gameover = PrintBullet();
		if (gameover == 1) {
			system("cls");
			printf("\n\n     Game Over!    \n\n\n");
			system("pause");
			break;
		}
		Sleep(70);
	}

	return 0;
}

