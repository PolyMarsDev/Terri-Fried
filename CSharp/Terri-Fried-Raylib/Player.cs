using System.Numerics;

namespace Terri_Fried
{
	class Player
	{
		const int gravity = 1;
		double x;
		double y;
		readonly int width;
		readonly int height;
		bool onPlatform;
		Vector2 velocity;

		public Player(double x, double y, int width, int height)
		{
			this.x = x;
			this.y = y;
			this.width = width;
			this.height = height;
			onPlatform = false;
		}

		public double GetX()
		{
			return x;
		}

		public double GetY()
		{
			return y;
		}

		public void SetX(int x)
		{
			this.x = x;
		}

		public void SetY(int y)
		{
			this.y = y;
		}

		public int GetWidth()
		{
			return width;
		}

		public int GetHeight()
		{
			return height;
		}

		public bool IsOnGround()
		{
			return onPlatform;
		}
		public bool IsOnPlatform()
		{
			return onPlatform;
		}

		public void SetOnPlatform(bool result)
		{
			onPlatform = result;
		}

		public void SetVelocity(double x, double y)
		{
			velocity = new Vector2( (float)x, (float)y);
		}

		public Vector2 GetVelocity()
		{
			return velocity;
		}

		public void UpdatePosition()
		{
			x += velocity.X;
			y += velocity.Y;
			if (!IsOnGround())
			{
				velocity.Y += gravity;
			}
			else
			{
				velocity = new Vector2( 0, 0);
			}
			if (x < 0)
			{
				velocity.X *= -1;
			}
			if (x + width > 800)
			{
				velocity.X *= -1;
			}
		}
	}
}
