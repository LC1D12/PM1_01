#include <Novice.h>
#include <string.h>

const char kWindowTitle[] = "Simple Hip Drop - One Button Game";

const int kWindowWidth = 1280;
const int kWindowHeight = 720;
const float kFloorY = 550.0f;

// プレイヤー情報
struct Player {
	float x, y;
	float speedY;
	float radius;
	bool isHipDropping; // 急降下中かどうか
};

// 障害物情報
struct Obstacle {
	float x, y;
	float width, height;
};

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	// 初期化
	Player player = { 200.0f, kFloorY, 0.0f, 20.0f, false };

	const int kMaxObstacles = 2;
	Obstacle obstacles[kMaxObstacles] = {
		{800.0f, kFloorY - 60.0f, 50.0f, 60.0f}, // 地上の罠
		{1400.0f, kFloorY - 180.0f, 60.0f, 80.0f} // 空中の罠（下をくぐる）
	};

	float scrollSpeed = 5.0f;
	bool isGameOver = false;

	while (Novice::ProcessMessage() == 0) {
		Novice::BeginFrame();

		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		/// ↓更新処理ここから
		if (!isGameOver) {

			// 1. スペースを押したら「急降下」スタート！
			if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
				player.speedY = 25.0f; // 下向きの爆速
				player.isHipDropping = true;
			}

			// 2. プレイヤーの移動処理
			player.y += player.speedY;

			// 急降下中でなければ、勝手にポヨンポヨン跳ねる
			if (!player.isHipDropping) {
				player.speedY += 0.8f; // 通常重力
			}

			// 地面に着いた時の処理
			if (player.y >= kFloorY) {
				player.y = kFloorY;
				player.speedY = -16.0f; // 勝手に次ジャンプ！
				player.isHipDropping = false;
			}

			// 3. 障害物の移動と当たり判定
			for (int i = 0; i < kMaxObstacles; i++) {
				obstacles[i].x -= scrollSpeed;

				// 画面外に出たら右から戻す
				if (obstacles[i].x < -obstacles[i].width) {
					obstacles[i].x = 1300.0f;
				}

				// 簡単な当たり判定（矩形と円）
				float closestX = player.x;
				if (closestX < obstacles[i].x) closestX = obstacles[i].x;
				if (closestX > obstacles[i].x + obstacles[i].width) closestX = obstacles[i].x + obstacles[i].width;

				float closestY = player.y;
				if (closestY < obstacles[i].y) closestY = obstacles[i].y;
				if (closestY > obstacles[i].y + obstacles[i].height) closestY = obstacles[i].y + obstacles[i].height;

				float distX = player.x - closestX;
				float distY = player.y - closestY;

				if ((distX * distX + distY * distY) < (player.radius * player.radius)) {
					isGameOver = true;
				}
			}

		}
		else {
			// リスタート処理
			if (keys[DIK_R] && !preKeys[DIK_R]) {
				player.y = kFloorY;
				player.speedY = 0.0f;
				player.isHipDropping = false;
				obstacles[0].x = 800.0f;
				obstacles[1].x = 1400.0f;
				isGameOver = false;
			}
		}
		/// ↑更新処理ここまで

		/// ↓描画処理ここから
		// 地面
		Novice::DrawLine(0, (int)kFloorY + (int)player.radius, kWindowWidth, (int)kFloorY + (int)player.radius, 0x888888FF);

		if (!isGameOver) {
			// 障害物（赤色）
			for (int i = 0; i < kMaxObstacles; i++) {
				Novice::DrawBox((int)obstacles[i].x, (int)obstacles[i].y, (int)obstacles[i].width, (int)obstacles[i].height, 0.0f, 0xFF4444FF, kFillModeSolid);
			}

			// 急降下中の演出（赤っぽくなる）
			unsigned int playerColor = player.isHipDropping ? 0xFF8800FF : 0x00FF88FF;

			// プレイヤー描画
			Novice::DrawEllipse((int)player.x, (int)player.y, (int)player.radius, (int)player.radius, 0.0f, playerColor, kFillModeSolid);

			// UI
			Novice::ScreenPrintf(10, 10, "SPACE : Hip Drop!");

		}
		else {
			Novice::ScreenPrintf(580, 340, "GAME OVER");
			Novice::ScreenPrintf(540, 380, "Press 'R' to Restart");
		}
		/// ↑描画処理ここまで

		Novice::EndFrame();

		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	Novice::Finalize();
	return 0;
}