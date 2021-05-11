using System;
using Raylib_cs;
using static Raylib_cs.Raylib;
using static Raylib_cs.Color;
using static Raylib_cs.MouseButton;
using System.Runtime.InteropServices;
using System.Numerics;

namespace Terri_Fried
{
	public class Program
	{
		static readonly Platform[] platforms = new Platform[4]{ new Platform(0), new Platform(1), new Platform(2), new Platform(3) };
		static readonly Player player = new Player(platforms[0].GetX() + platforms[0].GetWidth()/2 - 26/2, platforms[0].GetY() - 32, 26, 32);

		[DllImport(nativeLibName, CallingConvention = CallingConvention.Cdecl)]
		[return: MarshalAs(UnmanagedType.I1)]
		public static extern bool SaveStorageValue(uint position, int value);

		[DllImport(nativeLibName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int LoadStorageValue(uint position);

		static int scoreInt = 0;
		static int highscoreInt = (0);
		static string score;
		static string highscore;

		static bool titleScreen = true;
		static bool playCoinFX = false;

		static void AddScore(int amount)
		{
			scoreInt += amount;
			if (scoreInt < 10)
			{
				score = scoreInt.ToString();
			}
			else if (scoreInt < 100)
			{
				score = scoreInt.ToString();
			}
			else
			{
				score = scoreInt.ToString();
			}
			if (scoreInt > highscoreInt)
			{
				highscoreInt = scoreInt;
				highscore = "BEST: " + highscoreInt.ToString();
			}
		}
		static void ResetScore()
		{
			scoreInt = 0;
			score = scoreInt.ToString();
			SaveStorageValue(0, highscoreInt);
		}
		static void ResetGame()
		{
			ResetScore();
			for (int i = 0; i < 4; i++)
			{
				platforms[i] = new Platform(i);
			}
			player.SetVelocity(0, 0);
			player.SetX((int)platforms[0].GetX() + platforms[0].GetWidth() / 2 - 26 / 2);
			player.SetY((int)platforms[0].GetY() - player.GetHeight());
		}


		static void CheckPlayerCollision()
		{
			bool onPlatform = false;
			for (int i = 0; i < 4; i++)
			{
				if (platforms[i].GetHasCoin() && player.GetX() + player.GetWidth() - 3 > platforms[i].GetCoinX() && player.GetX() + 3 < platforms[i].GetCoinX() + 24 && player.GetY() + player.GetHeight() - 3 > platforms[i].GetCoinY() && player.GetY() + 3 < platforms[i].GetCoinY() + 24)
				{
					AddScore(1);
					platforms[i].SetHasCoin(false);
					playCoinFX = true;
				}
				if (player.GetX() + 1 < platforms[i].GetX() + platforms[i].GetWidth() && player.GetX() + player.GetWidth() > platforms[i].GetX() && player.GetY() + player.GetHeight() >= platforms[i].GetY() && player.GetY() < platforms[i].GetY() + platforms[i].GetHeight())
				{
					if (player.GetY() > platforms[i].GetY() + platforms[i].GetHeight() / 2)
					{
						player.SetVelocity(player.GetVelocity().X, 5);
					}
					else if (player.GetY() + player.GetHeight() < platforms[i].GetY() + platforms[i].GetHeight())
					{
						onPlatform = true;
						player.SetY((int)platforms[i].GetY() - player.GetHeight());
						player.SetY((int)player.GetY() + 1);
					}
				}

			}
			player.SetOnPlatform(onPlatform);
		}


		public static void Main()
		{
			ResetScore();
			highscore = "BEST: " + highscoreInt.ToString();
			const int screenWidth = 800;
			const int screenHeight = 450;

			int mouseDownX = 0;
			int mouseDownY = 0;
			double timer = 0;
			double splashTimer = 0;
			bool firstTime = true;
			bool playedSplash = false;
			bool playedSelect = false;
			Image egg = LoadImage("resources/egg.png");
			InitWindow(screenWidth, screenHeight, "Terri-Fried");
			SetWindowIcon(egg);
			InitAudioDevice();
			SetMasterVolume(0.3f);

			Texture2D playerSprite = LoadTexture("resources/egg.png");
			Texture2D lavaSprite = LoadTexture("resources/lava.png");
			Texture2D platformSprite = LoadTexture("resources/platform.png");
			Texture2D coinSprite = LoadTexture("resources/coin.png");
			Texture2D scoreBoxSprite = LoadTexture("resources/scorebox.png");
			Texture2D logo = LoadTexture("resources/logo.png");
			Texture2D splashEggSprite = LoadTexture("resources/splash_egg.png");

			Sound fxLaunch = LoadSound("resources/launch.wav");
			Sound fxClick = LoadSound("resources/click.wav");
			Sound fxDeath = LoadSound("resources/die.wav");
			Sound fxCoin = LoadSound("resources/coin.wav");
			Sound fxSplash = LoadSound("resources/splash.wav");
			Sound fxSelect = LoadSound("resources/select.wav");
			Font font = LoadFontEx("resources/font.otf", 64, null, 210);
			SetTargetFPS(60);
			while (!WindowShouldClose())
			{
				if (titleScreen)
				{
					if (splashTimer > 120)
					{
						if (!playedSelect)
						{
							PlaySound(fxSelect);
							playedSelect = true;
						}
						BeginDrawing();
						ClearBackground(ColorFromNormalized(new Vector4( 0.933f, 0.894f, 0.882f, 1.0f)));
						DrawTexture(logo, screenWidth / 2 - 200, screenHeight / 2 - 45 - 30, WHITE);
						DrawTextEx(font, highscore, new Vector2( screenWidth / 2 - 37, screenHeight / 2 + 10), 32, 0, BLACK);
						DrawTextEx(font, "CLICK ANYWHERE TO BEGIN", new Vector2( screenWidth / 2 - 134, screenHeight / 2 + 50), 32, 0, ColorFromNormalized(new Vector4( .698f, .588f, .49f, 0.4f)));
						EndDrawing();
						if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
						{
							PlaySound(fxSelect);
							titleScreen = false;
							mouseDownX = GetMouseX();
							mouseDownY = GetMouseY();
						}
					}
					else
					{
						if (!playedSplash)
						{
							PlaySound(fxSplash);
							playedSplash = true;
						}
						BeginDrawing();
						ClearBackground(ColorFromNormalized(new Vector4( 0.933f, 0.894f, 0.882f, 1.0f)));
						DrawTextEx(font, "POLYMARS", new Vector2( screenWidth / 2 - 54, screenHeight / 2 + 3), 32, 0, ColorFromNormalized(new Vector4( .835f, .502f, .353f, 1.0f)));
						DrawTexture(splashEggSprite, screenWidth / 2 - 16, screenHeight / 2 - 16 - 23, WHITE);
						EndDrawing();
						splashTimer += 1;
					}
				}
				else
				{


					if (playCoinFX)
					{
						PlaySound(fxCoin);
						playCoinFX = false;
					}
					if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && player.IsOnGround())
					{
						PlaySound(fxClick);
						mouseDownX = GetMouseX();
						mouseDownY = GetMouseY();
					}
					if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && player.IsOnGround())
					{
						if (firstTime)
						{
							firstTime = false;
						}
						else
						{
							PlaySound(fxLaunch);
							if (player.IsOnPlatform())
							{
								player.SetY((int)(player.GetY() - 1f));
							}
							int velocityX = GetMouseX() - mouseDownX;

							int velocityY = GetMouseY() - mouseDownY;

							player.SetVelocity((double)velocityX * .08, (double)velocityY * .08);
						}
					}
					CheckPlayerCollision();
					player.UpdatePosition();
					if (player.GetY() > screenHeight)
					{
						PlaySound(fxDeath);
						ResetGame();
					}
					for (int i = 0; i < 4; i++)
					{
						platforms[i].UpdatePosition();
					}

					double lavaY = screenHeight - 43 - Math.Sin(timer) * 5;
					timer += 0.05;
					BeginDrawing();

					ClearBackground(ColorFromNormalized(new Vector4( 0.933f, 0.894f, 0.882f, 1.0f)));
					if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && player.IsOnGround())
					{
						DrawLineEx(new Vector2((float) (mouseDownX + (player.GetX() - mouseDownX + (player.GetWidth() / 2))), (float)(mouseDownY + (player.GetY() - mouseDownY) + (player.GetHeight() / 2))), new Vector2( (float)(GetMouseX() + (player.GetX() - mouseDownX) + (player.GetWidth() / 2)), (float)(GetMouseY() + (player.GetY() - mouseDownY) + (player.GetHeight() / 2))), 3.0f, ColorFromNormalized(new Vector4( .906f, .847f, .788f, 1.0f)));
					}
					//DrawRectangle(player.getX(), player.getY(), player.getWidth(), player.getHeight(), WHITE);  
					for (int i = 0; i < 4; i++)
					{
						DrawTexture(platformSprite, (int)platforms[i].GetX(), (int)platforms[i].GetY(), ColorFromNormalized(new Vector4( .698f, .588f, .49f, 1.0f)));
					if (platforms[i].GetHasCoin())
					{
						DrawTexture(coinSprite, platforms[i].GetCoinX(), platforms[i].GetCoinY(), WHITE);
					}
				}
				DrawTexture(playerSprite, (int)player.GetX(), (int)player.GetY(), WHITE);
				DrawTexture(lavaSprite, 0, (int)lavaY, WHITE);
				DrawTexture(scoreBoxSprite, 17, 17, WHITE);
				DrawTextEx(font, score, new Vector2( 28, 20),64, 0, BLACK);
				DrawTextEx(font, highscore, new Vector2( 17, 90), 32, 0, BLACK);

				EndDrawing();
			}

		}

		UnloadTexture(playerSprite);
		UnloadTexture(lavaSprite);
		UnloadTexture(platformSprite);
		UnloadTexture(coinSprite);
		UnloadTexture(scoreBoxSprite);
		UnloadTexture(logo);
		UnloadTexture(splashEggSprite);
		UnloadSound(fxClick);
		UnloadSound(fxLaunch);
		UnloadSound(fxDeath);
		UnloadSound(fxCoin);
		UnloadSound(fxSplash);
		UnloadSound(fxSelect);
		UnloadFont(font);
		CloseAudioDevice();
		CloseWindow();        
   
    return;
}
}
}
