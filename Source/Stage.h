#pragma once
#ifndef STAGE_H
#define STAGE_H

namespace game_framework
{
	class Stage
	{
		friend class GameArea;
	public:
		Stage(int);
		~Stage();
		void LoadStage();							//Load Stage from file
		double GetScoreOne();						//return score of one star
		double GetScoreTwo();						//return score of two star
		double GetScoreThree();						//return score of three star
		double GetLastScoreHistory();				//Get last highest score
		bool IsUnlock();
		bool IsFail();
		void SetPassOrFail(int);
		void RemoveLine();							//re-create the cnt_stg.txt without last 2 lines(last score and isunlock)
		void WriteBack();							//Write game data to file
		void SetUnlock();
		int GetCurrentScore();
		int GetCurrentStage();
		void SetCurrentScore(int score);
	private:
		int map[13][20];
		double scoreOne, scoreTwo, scoreThree;		//Target score for three star
		int vertical, horizontal, pack, chocolate;	//total special candy spawn onInit
		int candyType;								//Max candy type in this game
		double lastHighScore;						//History highest score
		int maxStep;
		int mode;
		bool isUnlock;
		bool isFail;
		bool initcandy;
		string stageTxt;
		int currentStage;
		int currentScore;
		bool hasPortal;								//is portal exist/s
		vector<pair<CPoint, CPoint>> portalList;	//list of all teleporter (start point, end point)
	};
}
#endif