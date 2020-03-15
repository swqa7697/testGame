/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

typedef std::uniform_real_distribution<double> doubleDistr;
typedef std::default_random_engine defEng;

class HelloWorld : public cocos2d::Scene
{
private:
	cocos2d::Sprite* backgroundIMG;
	cocos2d::Sprite* wheel;
	cocos2d::Sprite* wheelBorder;
	cocos2d::Sprite* arrow;
	cocos2d::MenuItemImage* startBtn;
	cocos2d::MenuItemImage* resetBtn;

	// array of prizes
	std::string prizes[8] = { "Life 30 min","Brush 3X","Gems 35","Hammer 3X","Coins 750","Brush 1X","Gems 75","Hammer 1X" };
	std::string imgs[8] = { "heart1.png","brush1.png","gem1.png","hammer1.png","coin1.png","brush2.png","gem2.png","hammer2.png" };

	// members for random
	defEng* engine;
	doubleDistr* distribution;
	double myRand(doubleDistr& distribution, defEng& engine);

	// set position for the items if no error
	void setPosition(cocos2d::Sprite* obj, std::string filename, float x, float y);
	void setPosition(cocos2d::MenuItemImage* obj, std::string filename, float x, float y);

	// determine the prize
	int selectPrize(doubleDistr& distribution, defEng& engine);

	// emulate specified times of spins and logging results into a txt file
	void testSpins(int n, const char* filename, doubleDistr& distribution, defEng& engine);
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
};

#endif // __HELLOWORLD_SCENE_H__
