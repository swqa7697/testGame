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

#define DEBUG_ON 0
#define ANG_VELOCITY 260
#define BASE_CYCLE 2

#include "HelloWorldScene.h"
#include <fstream>
#include <random>

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(std::string filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

	// get coordinates of visible area
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	/////////////////////////////
	// 2. setup random generator
	std::random_device device;
	engine = new defEng{ device() };
	distribution = new doubleDistr{ 0.0, 1.0 };

    /////////////////////////////
    // 3. add items (sprites) into the Scene

	// add a image as background
	backgroundIMG = Sprite::create("Assets/bg.jpg");
	setPosition(backgroundIMG, "'bg.jpg'", origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2);
	this->addChild(backgroundIMG, -1);

	// add the wheel
	wheel = Sprite::create("Assets/wheel.png");
	setPosition(wheel, "'wheel.png'", origin.x + visibleSize.width / 2, origin.y + visibleSize.height * 0.56f);
	wheel->setScale(0.68f * visibleSize.height / wheel->getContentSize().height);
	this->addChild(wheel, 0);

    // add the wheel border
	wheelBorder = Sprite::create("Assets/wheel_border.png");
	setPosition(wheelBorder, "'wheel_border.png'", wheel->getPosition().x, wheel->getPosition().y);
	wheelBorder->setScale(0.68f * visibleSize.height / wheelBorder->getContentSize().height);
	this->addChild(wheelBorder, 1);

	// add the arrow
	arrow = Sprite::create("Assets/wheel_arrow.png");
	setPosition(arrow, "'wheel_arrow.png'", wheel->getPosition().x, origin.y + visibleSize.height * 0.88f);
	arrow->setScale(0.15f * visibleSize.height / arrow->getContentSize().height);
	this->addChild(arrow, 2);
	
	// add the menu button for start to play
	startBtn = MenuItemImage::create("Assets/spin_button.png", "Assets/spin_button_selected.png", [&](Ref* sender) {
		// determine the prize at first
		int prize = selectPrize(*distribution, *engine);

		// calculate the angle and time based on designed velocity
		float finalAngle = 360.0f - (45.0f*prize + 22.5f);
		float totalAngle = BASE_CYCLE * 360.0f + finalAngle + 15.0f;
		float t = totalAngle / ANG_VELOCITY;

		// wheel actions
		auto beforeStart = EaseQuadraticActionOut::create(RotateBy::create(0.2f, -15.0f));
		auto spin = EaseQuarticActionOut::create(RotateBy::create(t, totalAngle));
		auto delay = DelayTime::create(0.4f);
		auto wheelFadeOut = FadeOut::create(0.2f);
		auto spinSeq = Sequence::create(beforeStart, spin, delay, wheelFadeOut, nullptr);
		wheel->runAction(spinSeq);

		// hide start button
		startBtn->setEnabled(false);
		startBtn->runAction(FadeOut::create(0.5f));

		// hide other parts of the wheel
		auto longDelay = DelayTime::create(t + 0.6f);
		wheelBorder->runAction(Sequence::create(longDelay, FadeOut::create(0.2f), nullptr));
		arrow->runAction(Sequence::create(longDelay, FadeOut::create(0.2f), nullptr));

		// show the image of prize
		auto prizeImg = Sprite::create("Assets/" + imgs[prize]);
		setPosition(prizeImg, "'" + imgs[prize] + "'", wheelBorder->getContentSize().width / 2, wheelBorder->getContentSize().height * 0.82f);
		prizeImg->setOpacity(0);
		wheelBorder->addChild(prizeImg);

		auto fadeIn = FadeIn::create(0.5f);
		auto zoomIn = ScaleBy::create(0.5f, 2.5f);
		auto moveDown = MoveTo::create(0.5f, Vec2(wheelBorder->getContentSize().width / 2, wheelBorder->getContentSize().height / 2));
		auto priImgSp = Spawn::create(fadeIn, zoomIn, moveDown, nullptr);
		prizeImg->runAction(Sequence::create(longDelay, priImgSp, nullptr));

		// display reset button
		auto showBtn = Show::create();
		resetBtn->runAction(Sequence::create(longDelay, showBtn, nullptr));
	});
	setPosition(startBtn, "'spin_button.png' and 'spin_button_selected.png'",
				wheel->getPosition().x, origin.y + visibleSize.height * 0.12f);
	startBtn->setScale(0.25f * visibleSize.width / startBtn->getContentSize().width);

	auto playLabel = Label::createWithSystemFont("Play On", "Arial", 96);
	playLabel->setPosition(startBtn->getContentSize().width / 2, startBtn->getContentSize().height / 1.8f);
	startBtn->addChild(playLabel);

	// add the menu button for reset
	resetBtn = MenuItemImage::create("Assets/spin_button.png", "Assets/spin_button_selected.png", [&](Ref* sender) {
		// remove the prize image
		wheelBorder->removeAllChildren();
		
		// reset the wheel
		wheel->setRotation(0.0f);
		wheel->setOpacity(255);
		wheelBorder->setOpacity(255);
		arrow->setOpacity(255);

		// set buttons as default
		resetBtn->setVisible(false);
		startBtn->setOpacity(255);
		startBtn->setEnabled(true);
	});
	setPosition(resetBtn, "'spin_button.png' and 'spin_button_selected.png'",
		startBtn->getPosition().x, startBtn->getPosition().y);
	resetBtn->setScale(0.25f * visibleSize.width / resetBtn->getContentSize().width);

	auto resetLabel = Label::createWithSystemFont("Claim", "Arial", 96);
	resetLabel->setPosition(resetBtn->getContentSize().width / 2, resetBtn->getContentSize().height / 1.8f);
	resetBtn->addChild(resetLabel);

	resetBtn->setVisible(false);

	auto menu = Menu::create(startBtn, resetBtn, NULL);
	menu->setPosition(Vec2::ZERO);

	this->addChild(menu, 4);

    /////////////////////////////
    // 4. Unit-Testing if Debug is on
	if (DEBUG_ON == 1)
	{
		testSpins(1000, "E:\\Documents\\workspace\\Outputs\\testLogs.txt", *distribution, *engine);
	}
    
    return true;
}

void HelloWorld::setPosition(Sprite* obj, std::string filename, float x, float y)
{
	if (obj == nullptr ||
		obj->getContentSize().width <= 0 ||
		obj->getContentSize().height <= 0)
	{
		problemLoading(filename);
	}
	else
	{
		obj->setPosition(Vec2(x, y));
	}
}

void HelloWorld::setPosition(MenuItemImage* obj, std::string filename, float x, float y)
{
	if (obj == nullptr ||
		obj->getContentSize().width <= 0 ||
		obj->getContentSize().height <= 0)
	{
		problemLoading(filename);
	}
	else
	{
		obj->setPosition(Vec2(x, y));
	}
}

double HelloWorld::myRand(doubleDistr& distribution, defEng& engine)
{
	return distribution(engine);
}

int HelloWorld::selectPrize(doubleDistr& distribution, defEng& engine)
{
	double selector = myRand(distribution, engine);
	if (selector < 0.2)
		return 0;
	else if (selector < 0.3)
		return 1;
	else if (selector < 0.4)
		return 2;
	else if (selector < 0.5)
		return 3;
	else if (selector < 0.55)
		return 4;
	else if (selector < 0.75)
		return 5;
	else if (selector < 0.8)
		return 6;
	else
		return 7;
}

void HelloWorld::testSpins(int n, const char* filename, doubleDistr& distribution, defEng& engine)
{
	std::ofstream file(filename);
	int results[] = { 0,0,0,0,0,0,0,0 };

	file << "Here is the " << n << " records of each spin. Summary at the bottom. " << std::endl << std::endl;

	for (int i = 0; i < n; i++)
	{
		int prize = selectPrize(distribution, engine);
		results[prize]++;
		file << "Prize: " << prizes[prize] << "    Turn: " << i + 1 << std::endl;
	}

	file << std::endl << "Summary: " << std::endl;

	for (int i = 0; i < 8; i++)
	{
		file << i + 1 << ". " << prizes[i] << ": " << results[i] << "  (" << results[i] * 100.0f / n << "%)" << std::endl;
	}

	file.close();
}
