# include <Siv3D.hpp> // Siv3D v0.6.14
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
		constexpr double SPEED = 480.0;
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

	Ball() : velocity({ 0, -constants::ball::SPEED }), ball({ 400, 400, 8 }) {}

	/// @brief 更新
	void Update() {
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
			}.setLength(constants::ball::SPEED);
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
void Main()
{
	//// 背景の色を設定する | Set the background color
	//Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	//// 画像ファイルからテクスチャを作成する | Create a texture from an image file
	//const Texture texture{ U"example/windmill.png" };

	//// 絵文字からテクスチャを作成する | Create a texture from an emoji
	//const Texture emoji{ U"🦖"_emoji };

	//// 太文字のフォントを作成する | Create a bold font with MSDF method
	//const Font font{ FontMethod::MSDF, 48, Typeface::Bold };

	//// テキストに含まれる絵文字のためのフォントを作成し、font に追加する | Create a font for emojis in text and add it to font as a fallback
	//const Font emojiFont{ 48, Typeface::ColorEmoji };

	//// ボタンを押した回数 | Number of button presses
	//font.addFallback(emojiFont);
	//int32 count = 0;

	//// チェックボックスの状態 | Checkbox state
	//bool checked = false;

	//// プレイヤーの移動スピード | Player's movement speed
	//double speed = 200.0;

	//// プレイヤーの X 座標 | Player's X position
	//double playerPosX = 400;

	//// プレイヤーが右を向いているか | Whether player is facing right
	//bool isPlayerFacingRight = true;

	//while (System::Update())
	//{
	//	// テクスチャを描く | Draw the texture
	//	texture.draw(20, 20);

	//	// テキストを描く | Draw text
	//	font(U"Hello, Siv3D!🎮").draw(64, Vec2{ 20, 340 }, ColorF{ 0.2, 0.4, 0.8 });

	//	// 指定した範囲内にテキストを描く | Draw text within a specified area
	//	font(U"Siv3D (シブスリーディー) は、ゲームやアプリを楽しく簡単な C++ コードで開発できるフレームワークです。")
	//		.draw(18, Rect{ 20, 430, 480, 200 }, Palette::Black);

	//	// 長方形を描く | Draw a rectangle
	//	Rect{ 540, 20, 80, 80 }.draw();

	//	// 角丸長方形を描く | Draw a rounded rectangle
	//	RoundRect{ 680, 20, 80, 200, 20 }.draw(ColorF{ 0.0, 0.4, 0.6 });

	//	// 円を描く | Draw a circle
	//	Circle{ 580, 180, 40 }.draw(Palette::Seagreen);

	//	// 矢印を描く | Draw an arrow
	//	Line{ 540, 330, 760, 260 }.drawArrow(8, SizeF{ 20, 20 }, ColorF{ 0.4 });

	//	// 半透明の円を描く | Draw a semi-transparent circle
	//	Circle{ Cursor::Pos(), 40 }.draw(ColorF{ 1.0, 0.0, 0.0, 0.5 });

	//	// ボタン | Button
	//	if (SimpleGUI::Button(U"count: {}"_fmt(count), Vec2{ 520, 370 }, 120, (checked == false)))
	//	{
	//		// カウントを増やす | Increase the count
	//		++count;
	//	}

	//	// チェックボックス | Checkbox
	//	SimpleGUI::CheckBox(checked, U"Lock \U000F033E", Vec2{ 660, 370 }, 120);

	//	// スライダー | Slider
	//	SimpleGUI::Slider(U"speed: {:.1f}"_fmt(speed), speed, 100, 400, Vec2{ 520, 420 }, 140, 120);

	//	// 左キーが押されていたら | If left key is pressed
	//	if (KeyLeft.pressed())
	//	{
	//		// プレイヤーが左に移動する | Player moves left
	//		playerPosX = Max((playerPosX - speed * Scene::DeltaTime()), 60.0);
	//		isPlayerFacingRight = false;
	//	}

	//	// 右キーが押されていたら | If right key is pressed
	//	if (KeyRight.pressed())
	//	{
	//		// プレイヤーが右に移動する | Player moves right
	//		playerPosX = Min((playerPosX + speed * Scene::DeltaTime()), 740.0);
	//		isPlayerFacingRight = true;
	//	}

	//	// プレイヤーを描く | Draw the player
	//	emoji.scaled(0.75).mirrored(isPlayerFacingRight).drawAt(playerPosX, 540);

	Bricks bricks;
	Ball ball;
	Paddle paddle;

	while (System::Update())
	{
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
	}
}
//
// - Debug ビルド: プログラムの最適化を減らす代わりに、エラーやクラッシュ時に詳細な情報を得られます。
//
// - Release ビルド: 最大限の最適化でビルドします。
//
// - [デバッグ] メニュー → [デバッグの開始] でプログラムを実行すると、[出力] ウィンドウに詳細なログが表示され、エラーの原因を探せます。
//
// - Visual Studio を更新した直後は、プログラムのリビルド（[ビルド]メニュー → [ソリューションのリビルド]）が必要な場合があります。
//
