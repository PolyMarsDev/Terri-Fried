using System;

namespace Terri_Fried
{
	class Platform
	{
		const int screenHeight = 450;

		double x;
		double y;
		readonly int width;
		readonly int height;
		bool hasCoin;
		int coinX;
		int coinY;

		public Platform(int index)
		{
			width = 100;
			height = 32;
			x = new Random().Next() % 660 + 20;
			y = 0 - height - (index* 100);
			int coinInt = new Random().Next() % 4;
			if (coinInt == 0 || index == 0)
			{
				hasCoin = false;
			} else {
				hasCoin = true;
			}
			coinX = (int)x + width / 2 - 24 / 2;
			coinY = (int)y - 24 - 5;
    
		}

		public double GetX()
		{
			return x;
		}

		public double GetY()
		{
			return y;
		}

		public int GetWidth()
		{
			return width;
		}

		public int GetHeight()
		{
			return height;
		}

		public bool GetHasCoin()
		{
			return hasCoin;
		}
		public void SetHasCoin(bool value)
		{
			hasCoin = value;
		}
		public int GetCoinX()
		{
			return coinX;
		}
		public int GetCoinY()
		{
			return coinY;
		}

		public void UpdatePosition()
		{
			y += 1;
			coinX = (int)x + width / 2 - 24 / 2;
			coinY = (int)y - 24 - 5;
			if (y > screenHeight)
			{
				x = new Random().Next() % 660 + 20;
				y = 0 - height;
				int coinInt = new Random().Next() % 4;
				if (coinInt == 0)
				{
					hasCoin = false;
				}
				else
				{
					hasCoin = true;
				}
			}
		}
	}
}
