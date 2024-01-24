#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"
#include "raylib.h"
#include "raymath.h"
#include <iostream>
#include <cmath>
#include <string>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++11-narrowing"
using namespace std;

struct Coord{
	int x;
	int y;
};

const int MAX_TRACK = 100;

struct Ant{
	int x;
	int y;
	float rotate = 0;
	Coord tracks[MAX_TRACK];
	int track = -1;
};

void addPath(Coord paths[], Coord path, int &n) {
	if (sqrt(pow(path.x - paths[0].x, 2) + pow(path.y - paths[0].y, 2)) < 10) {
		return;
	}
	if (n < MAX_TRACK) n++;
	for (int i = n - 1; i >= 1; i--) {
		paths[i].x = paths[i - 1].x;
		paths[i].y = paths[i - 1].y;
	}
	paths[0].x = path.x;
	paths[0].y = path.y;
}

void printPaths(Coord paths[], int n) {
	for (int i = 0; i < n; i++) {
		cout << "x: " << paths[i].x << "\n" << "y: " << paths[i].y << "\n";
	}
}

int main() {
	const int screenWidth = 1000;
	const int screenHeight = 500;
	InitWindow(screenWidth, screenHeight, "Ant");
	Image ant_img = LoadImage(R"(D:\clion\Ant-Draw\source\ant.png)");
	ImageResize(&ant_img, ant_img.width / 8, ant_img.height / 8);

	Texture2D texture = LoadTextureFromImage(ant_img);
	UnloadImage(ant_img);
	SetTargetFPS(60);

	const int MAX = 1;
	Ant ants[MAX];
	ants[0] = {
		screenWidth / 2,
		screenHeight / 2,
		0.0
	};

	Coord dest{
		screenWidth / 2,
		screenHeight / 2,
	};
	bool foodMode = true;
	bool debugMode = false;
	bool trackMode = false;

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawFPS(screenWidth - 100, 0);

		DrawRectangle(screenWidth / 2, 0, 1, screenHeight, BLUE);
		DrawRectangle(0, screenHeight / 2, screenWidth, 1, BLUE);
		DrawCircle(GetRenderWidth() / 2, GetScreenHeight() / 2, 3, BLACK);

		if (debugMode) {
			string text = "x: " + to_string(ants[0].x) + " | " + to_string(screenWidth) + "\n";
			text += "y: " + to_string(ants[0].y)  + " | " + to_string(screenHeight) + "\n";
			DrawText(text.c_str(), 0, 0, 10, BLACK);
			string s = to_string(GetRenderWidth() / 2) + " : " + to_string(GetScreenHeight() / 2);
			DrawText(s.c_str(), GetRenderWidth() / 2 - MeasureText(s.c_str(), 10) / 2, GetScreenHeight() / 2 - 13, 10, BLACK);
		}

		for (int i = 0; i < 100; i++) {
			DrawCircle(screenWidth / 2, screenHeight / 2, 3, BLACK);
		}

		float x = GetMouseX();
		float y = GetMouseY();

		if (!foodMode || IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			dest.x = x;
			dest.y = y;
		}
		if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
			foodMode = !foodMode;
		}
		if (IsKeyPressed(KEY_D)) {
			debugMode = !debugMode;
		}
		if (IsKeyPressed(KEY_T)) {
			if (trackMode) {
				for (auto & ant : ants) {
					ant.track = -1;
				}
			}
			trackMode = !trackMode;
		}

		if (IsKeyPressed(KEY_R)) {
			for (auto & ant : ants) {
				ant.x = rand() % screenWidth;
				ant.y = rand() % screenHeight;
				ant.track = -1;
			}
			EndDrawing();
			continue;
		}

		float px{0.0f}, py{0.0f};

		DrawLine(dest.x, dest.y, ants[0].x, ants[0].y, YELLOW);
		DrawCircle(dest.x, dest.y, 2, BLUE);
		if (debugMode) DrawText(("dX: " + to_string(dest.x) + "\ndY: " + to_string(dest.y)).c_str(), 0, 80, 10, BLACK);

		for (int i = 0; i < MAX; i++) {
			px = dest.x - ants[i].x;
			py = dest.y - ants[i].y;

			float r = sqrt(px * px + py * py);
			if (debugMode && i == 0) DrawText(("r: " + to_string(r) + "\npX: " + to_string(px) + "\npY: " + to_string(py)).c_str(), 0, 40, 10, BLACK);

			if (r > texture.height * sqrt(2) / 2) {
				if (trackMode) addPath(ants[i].tracks, Coord{ants[i].x, ants[i].y}, ants[i].track);
				if (debugMode) DrawText("MOVE", screenWidth / 2 - MeasureText("MOVE", 10) / 2, screenHeight / 2, 10, BLACK);
				if (abs(px) > texture.width / 2.0) {
					ants[i].x += px / texture.width + px / (abs(px) + 1);
//					ants[i].x += px / 10.0f;
				}
				if (abs(py) > texture.height / 2.0) {
//					ants[i].y += py / 10.0f;
					ants[i].y += py / texture.height + py / (abs(py) + 1);
				}
			}
			if (trackMode) for (int j = 0, jE = ants[i].track - 1; j < jE; j++) {
				DrawCircle(ants[i].tracks[j].x, ants[i].tracks[j].y, 2, BLUE);
			}

			if (debugMode) {
				for (int j = 0, jE = ants[i].track - 1; j < jE; j++) {
					DrawLine(ants[i].tracks[j].x, ants[i].tracks[j].y, ants[i].tracks[j + 1].x, ants[i].tracks[j + 1].y, BLACK);
				}
			}

			ants[i].rotate = atan2(px, py) * -57.29578f;

			if (debugMode) {
				DrawRectangle(ants[i].x - texture.width * 0.75, ants[i].y, texture.width * 1.5, 1, BLACK);
				DrawRectangle(ants[i].x, ants[i].y - texture.height * 0.75,1, texture.height * 1.5, BLACK);

				DrawRectanglePro(Rectangle{ants[i].x, ants[i].y, texture.width * 2, 1}, {texture.width, 0}, ants[i].rotate, BLUE);
				DrawRectanglePro(Rectangle{ants[i].x, ants[i].y, texture.width * 2, 1}, {texture.width, 0}, ants[i].rotate + 90, RED);
			}

			//DrawPolyLines({ants[i].x, ants[i].y}, 4, texture.width, 45, BLACK);
			//DrawRectanglePro(Rectangle{ants[i].x, ants[i].y, texture.width, texture.height}, {texture.width / 2, texture.height / 2}, ants[i].rotate + 180, RED);

			DrawTexturePro(texture,Rectangle{0, 0, (float)texture.width, (float)texture.height},
						   Rectangle{ants[i].x, ants[i].y, texture.width, texture.height},
						   {texture.width / 2, texture.height / 2},
						   ants[i].rotate + 180,
						   BLACK);
		}
		EndDrawing();
	}
	UnloadTexture(texture);
	CloseWindow();
	return 0;
}
#pragma clang diagnostic pop
#pragma clang diagnostic pop