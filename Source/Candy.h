#ifndef CANDY_H
#define CANDY_H

namespace game_framework
{
	class Candy
	{
	public:
		Candy(int, int, int);
		Candy(int, int);
		Candy();
		~Candy();
		Candy* Click();						//Change state of onClick if being clicked
		int GetCurrentX();					//Get current x
		int GetCurrentY();					//Get current y
		int GetPower();						//return current power
		int GetStyle();						//get current style
		int GetTopLeftX();					//Get destination x
		int GetTopLeftY();					//Get destination y
		int GetTopLeft(char c);				//Get destination(c) 'x'/'y'
		void InitClick();					//Unclick self
		bool IsMoving();					//return current coordinate != destination coordinate
		bool IsClicked();					//return onClick
		void Kill();						//simple remove without release power
		static void LoadBitmap();			//Load candy's bitmap according to style
		void OnMove(bool mute = false);		//move if current coordinate != destination coordinate
		void OnShow();						//display candy
		void Push(int, int);				//Push candies around self
		void Relive();						//reset candy's style
		void SetDestination(int, int);		//set destination x,y
		void SetDestination(int);			//set destination y
		void SetStyle(int);					//change current style
		void SetPower(int);					//Set current power
	private:
		void GetCurrentShow(CMovingBitmap**, CMovingBitmap**);

		static CMovingBitmap normal[6], normalClick[6];
		static CMovingBitmap horizon[6], horizonClick[6];
		static CMovingBitmap vertical[6], verticalClick[6];
		static CMovingBitmap pack[6], packClick[6];
		static CMovingBitmap super[6], superClick[6];
		bool *sound;
		int style, rawStyle;		//0 = non-exist
		int power;
		int x, y, dx, dy, pushX, pushY;			//current || destination coordinate
		bool onClick;
		int fallingSpeed;
	};
}

#endif
