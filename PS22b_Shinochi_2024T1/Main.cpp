# include <Siv3D.hpp>

/*
	よりC++ライクな書き方
	・構造体ベース
	・ポインタは使わない
*/

//==============================
// 定数
//==============================
namespace constants {
	namespace brick {
		/// @brief ブロックのサイズ
		constexpr Size SIZE{ 40, 20 };

		/// @brief ブロックの数　縦
		constexpr int Y_COUNT = 5;

		/// @brief ブロックの数　横
		constexpr int X_COUNT = 20;

		/// @brief 合計ブロック数
		constexpr int MAX = Y_COUNT * X_COUNT;
	}

	namespace ball {
		/// @brief ボールの速さ
		constexpr double SPEED = 400.0;
		/// @brief ボールの初速度
		constexpr double INITIAL_SPEED = 400.0;
		/// @brief ボールの速度増加量（毎秒）
		constexpr double SPEED_INCREMENT = 10.0;
		/// @brief ボールの最大速度
		constexpr double MAX_SPEED = 600.0;
	}

	namespace paddle {
		/// @brief パドルのサイズ
		constexpr Size SIZE{ 60, 10 };
	}
}

/// @brief ボール
class Ball {
public:
	/// @brief 速度
	Vec2 velocity;

	/// @brief ボール
	Circle ball;

	//Ball() : velocity({ 0, -constants::ball::SPEED }), ball({ 400, 400, 8 }) {}

	/// @brief 経過時間
	double elapsedTime = 0.0;

	Ball() : velocity({ 0, -constants::ball::INITIAL_SPEED }), ball({ 400, 400, 8 }) {}

	/// @brief 更新
	void Update() {
		// 経過時間を更新
		elapsedTime += Scene::DeltaTime();
		// 速度を増加させる（ただし最大速度を超えないようにする）
		double newSpeed = Min(constants::ball::INITIAL_SPEED + elapsedTime * constants::ball::SPEED_INCREMENT, constants::ball::MAX_SPEED);
		velocity.setLength(newSpeed);
		// ボールを動かす
		ball.moveBy(velocity * Scene::DeltaTime());
	}

	/// @brief 描画
	void Draw() {
		ball.draw();
	}
};

/// @brief ブロック
class Bricks {
public:
	/// @brief ブロックリスト
	Rect brickTable[constants::brick::MAX];

	Bricks() {
		using namespace constants::brick;

		for (int y = 0; y < Y_COUNT; ++y) {
			for (int x = 0; x < X_COUNT; ++x) {
				int index = y * X_COUNT + x;
				brickTable[index] = Rect{
					x * SIZE.x,
					60 + y * SIZE.y,
					SIZE
				};
			}
		}
	}

	/// @brief 衝突検知
	void Intersects(Ball& ball) {
		using namespace constants::brick;

		for (int i = 0; i < MAX; ++i) {
			// 参照で保持
			Rect& refBrick = brickTable[i];

			// 衝突を検知
			if (refBrick.intersects(ball.ball))
			{
				// ブロックの上辺、または底辺と交差
				if (refBrick.bottom().intersects(ball.ball)
					|| refBrick.top().intersects(ball.ball))
				{
					ball.velocity.y *= -1;
				}
				else // ブロックの左辺または右辺と交差
				{
					ball.velocity.x *= -1;
				}

				// あたったブロックは画面外に出す
				refBrick.y -= 600;

				// 同一フレームでは複数のブロック衝突を検知しない
				break;
			}
		}
	}

	/// @brief 描画
	void Draw() {
		using namespace constants::brick;

		for (int i = 0; i < MAX; ++i) {
			brickTable[i].stretched(-1).draw(HSV{ brickTable[i].y - 40 });
		}
	}
};

/// @brief パドル
class Paddle {
public:
	Rect paddle;

	Paddle() : paddle(Rect(Arg::center(Cursor::Pos().x, 500), constants::paddle::SIZE)) {}

	/// @brief 衝突検知
	void Intersects(Ball& ball) {
		if ((0 < ball.velocity.y) && paddle.intersects(ball.ball))
		{
			ball.velocity = Vec2{
				(ball.ball.x - paddle.center().x) * 10,
				-ball.velocity.y
			}.setLength(constants::ball::INITIAL_SPEED);
		}
	}

	/// @brief 更新
	void Update() {
		paddle.x = Cursor::Pos().x - (constants::paddle::SIZE.x / 2);
	}

	/// @brief 描画
	void Draw() {
		paddle.rounded(3).draw();
	}
};

/// @brief 壁
struct Wall {
	/// @brief 衝突検知
	static void Intersects(Ball& ball) {
		// 天井との衝突を検知
		if ((ball.ball.y < 0) && (ball.velocity.y < 0))
		{
			ball.velocity.y *= -1;
		}

		// 壁との衝突を検知
		if (((ball.ball.x < 0) && (ball.velocity.x < 0))
			|| ((Scene::Width() < ball.ball.x) && (0 < ball.velocity.x)))
		{
			ball.velocity.x *= -1;
		}
	}
};
// アセット名を定義
const String FontAssetName = U"ExampleFont";
void Main()
{
	Bricks bricks;
	Ball ball;
	Paddle paddle;

	// フォントアセットを登録
	FontAsset::Register(FontAssetName, 30);

	while (System::Update())
	{
		// ゲームオーバーフラグ
		bool gameOver = false;

		while (System::Update())
		{
			// ゲームオーバーでない場合のみ更新
			if (!gameOver)
			{
				// ... (更新とコリジョンの処理は変更なし) ...

				// ボールが画面下端を超えたらゲームオーバー
				if (ball.ball.y > Scene::Height())
				{
					gameOver = true;
				}
			}
			//==============================
			// 更新
			//==============================
			paddle.Update();
			ball.Update();

			//==============================
			// コリジョン
			//==============================
			bricks.Intersects(ball);
			Wall::Intersects(ball);
			paddle.Intersects(ball);

			//==============================
			// 描画
			//==============================
			bricks.Draw();
			ball.Draw();
			paddle.Draw();

			// ゲームオーバー時の処理
			if (gameOver)
			{
				// フォントアセットを使用してテキストを描画
				if (FontAsset::IsRegistered(FontAssetName))
				{
					FontAsset(FontAssetName)(U"GameOver").draw(Arg::center(Scene::Center()));
				}

				// Rキーを押すとリスタート
				if (KeyR.down())
				{
					gameOver = false;
					// ゲームの状態をリセット
					bricks = Bricks();
					ball = Ball();
					paddle = Paddle();
				}
			}
		}
	}
}
