#pragma once
#ifndef BLAST_H
#define BLAST_H

namespace game_framework
{
	class Blast
	{
	public:
		virtual ~Blast() {};
		virtual void LoadBitmap() {};
		virtual void OnShow() {};
		virtual void OnMove() {};
		virtual void SetTopLeft(int x, int y);
		virtual bool IsLast() = 0;
	protected:
		int x, y;
		int style;
		bool *sound;
	};

	class NormalBlast :public Blast
	{
	public:
		NormalBlast();
		NormalBlast(int style, int x, int y);
		static void LoadBitmap();
		void OnMove();
		void OnShow();
		bool IsLast();
	private:
		static CMovingBitmap normalBlast[6][10];
		static CMovingBitmap shatter[6][15];
		int curShow;
		int shift[3][3];				//move direction/ moving speed/ spin direction
		int shatPosition[3][2];			//every shatter current position
		int shatShow[3];				//shatter current frame
		const int totalShow;			//total shatter spawned
		double size;					//current size
	};

	class LineBlast :public Blast
	{
	public:
		LineBlast(int style, int x, int y, int power);
		static void LoadBitmap();
		void OnMove();
		void OnShow();
		bool IsLast();
	private:
		static CMovingBitmap horizontal[6][30];
		static CMovingBitmap vertical[6][30];
		int curShow;
		int powStyle;
	};

	class MagicBlast :public Blast
	{
	public:
		MagicBlast(int x, int y);
		MagicBlast(CPoint);
		bool IsLast();
		static void LoadBitmap();
		void OnMove();
		void OnShow();
		bool operator==(const MagicBlast& rhs);
	private:
		static CMovingBitmap bmp[4];
		int curShow;
		int delay;
		int x, y;
	};

	class SuperBlast :public Blast
	{
	public:
		SuperBlast(int x, int y, int delay = 0, bool showAll = false);
		~SuperBlast();
		void AddPoint(int x, int y);
		bool IsLast();
		static void LoadBitmap();
		void OnMove();
		void OnShow();
	private:
		void ShowLightning(bool showAll = false);
		void DrawLine(CDC*, const CPoint&, const CPoint&);
		list<CPoint>* GetRoutePoints(CPoint, CPoint);

		static CAnimation chocalate;
		vector<CPoint> target;
		list<MagicBlast> magicBlasts;
		int curShow;
		int lightningDelay;		//delay time of lightning exist
		bool showAll;			//Show all lightning in one-time
	};
}

#endif
