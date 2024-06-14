#include "stdafx.h"
#include "Resource.h"
#include <mmsystem.h>
#include <ddraw.h>
#include "audio.h"
#include "gamelib.h"
#include "ScoreBoard.h"

namespace game_framework
{
	ScoreBoard::ScoreBoard()
		: score(0), moves(0), oneStar(0), twoStar(0), threeStar(0), lastHighScore(0), mode(0)
	{}

	void ScoreBoard::OnShow()
	{
		ShowScoreBar();
		scoreBoard.SetTopLeft(0, 0);
		scoreBoard.ShowBitmap();
		ShowMoves();
		ShowScore();
		ShowStars();
		ShowTarget();
	}

	void ScoreBoard::LoadBitmap()
	{
		score.LoadBitmap();
		scoreBoard.LoadBitmap("Bitmaps\\score_board.bmp", RGB(0, 0, 0));
		scoreBar.LoadBitmap("Bitmaps\\ScoreBar.bmp");
		blackBar.LoadBitmap(IDB_BLACK_BAR);
		yellowStar.LoadBitmap("Bitmaps\\YellowStar1.bmp", RGB(0, 43, 255));
		greenStar.LoadBitmap("Bitmaps\\GreenStar1.bmp", RGB(0, 43, 255));
		redStar.LoadBitmap("Bitmaps\\RedStar1.bmp", RGB(0, 43, 255));
		emptyStar1.LoadBitmap("Bitmaps\\EmptyStar1.bmp", RGB(0, 43, 255));
		emptyStar2.LoadBitmap("Bitmaps\\EmptyStar2.bmp", RGB(0, 43, 255));
		target1.LoadBitmap("Bitmaps\\Target1.bmp", RGB(251, 200, 201));
		target2.LoadBitmap("Bitmaps\\Target2.bmp", RGB(251, 200, 201));
	}

	bool ScoreBoard::IsReachedTarget()
	{
		switch (mode)
		{
		case 1:	//Target reached when score higher than target
			if (score >= target) return true;
		case 2:	//Target reached when all jelly cleared
			if (!target.GetInteger()) return true;
		default:
			return false;
		}
	}

	void ScoreBoard::ShowStars()
	{
		//Show empty star if score is lower than each relative score
		CMovingBitmap *thirdStar = score >= threeStar ? &yellowStar : &emptyStar2;
		CMovingBitmap *secondStar = score >= twoStar ? &greenStar : &emptyStar1;
		CMovingBitmap *firstStar = score >= oneStar ? &redStar : &emptyStar1;

		int star_X = scoreBoard.Left() + 159;

		thirdStar->SetTopLeft(star_X, scoreBoard.Top() + (281 - yellowStar.Height() / 2));
		thirdStar->ShowBitmap();
		secondStar->SetTopLeft(star_X, (int)(scoreBoard.Top() + 281 + (((threeStar - twoStar) / threeStar) * 254 - greenStar.Height() / 2)));
		secondStar->ShowBitmap();
		firstStar->SetTopLeft(star_X, (int)(scoreBoard.Top() + 281 + (((threeStar - oneStar) / threeStar) * 254 - redStar.Height() / 2)));
		firstStar->ShowBitmap();
	}

	void ScoreBoard::ShowMoves()
	{
		int CurrentMoves = moves.GetInteger();
		int size = 1;
		while (CurrentMoves > 9)
		{
			CurrentMoves /= 10;
			size++;
		}
		if (size <= 7)
		{
			moves.SetDigit(size);
			moves.SetTopLeft(scoreBoard.Left() + 119 - 9 * size, scoreBoard.Top() + 240);
			moves.ShowBitmap();
		}
	}

	void ScoreBoard::ShowScoreBar()
	{
		int X_point = scoreBoard.Left() + 159, Y_point = scoreBoard.Top() + 539;
		double currentLevel = (score.GetInteger() / threeStar) * 129;
		currentLevel = currentLevel > 129 ? 129 : currentLevel;
		for (int i = 0; i < 129; i++)
		{
			if (i < currentLevel)
			{
				scoreBar.SetTopLeft(X_point, Y_point);
				scoreBar.ShowBitmap();
			}
			else
			{
				blackBar.SetTopLeft(X_point, Y_point);
				blackBar.ShowBitmap();
			}
			Y_point -= 2;
		}
	}

	void ScoreBoard::ShowScore()
	{
		int CurrentScore = score.GetInteger();
		int size = 1;
		while (CurrentScore > 9)
		{
			CurrentScore /= 10;
			size++;
		}
		if (size <= 7)
		{
			score.SetDigit(size);
			score.SetTopLeft((scoreBoard.Left() + 144 - (18 * size)), scoreBoard.Top() + 325);
			score.ShowBitmap();
		}
	}

	void ScoreBoard::ShowTarget()
	{
		switch (mode)
		{
		case 1:
			target1.SetTopLeft(10, 90);
			target1.ShowBitmap();
			break;
		case 2:
			target2.SetTopLeft(10, 90);
			target2.ShowBitmap();
			break;
		}

		target.SetTopLeft(110, 95);
		target.ShowBitmap();
	}
}