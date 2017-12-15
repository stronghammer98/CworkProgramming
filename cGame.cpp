/*
==================================================================================
cGame.cpp
==================================================================================
*/
#include "cGame.h"

cGame* cGame::pInstance = NULL;
static cTextureMgr* theTextureMgr = cTextureMgr::getInstance(); // used to manage textures
static cFontMgr* theFontMgr = cFontMgr::getInstance(); // used to manage fonts
static cSoundMgr* theSoundMgr = cSoundMgr::getInstance(); //used to manage sounds
static cButtonMgr* thebuttonMgr = cButtonMgr::getInstance(); // used to manage buttons
int score = 0;


/*
=================================================================================
Constructor
=================================================================================
*/
cGame::cGame()
{

}
/*
=================================================================================
Singleton Design Pattern
=================================================================================
*/
cGame* cGame::getInstance()
{
	if (pInstance == NULL)
	{
		pInstance = new cGame();
	}
	return cGame::pInstance;
}


void cGame::initialise(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	// seed random number gen
	srand(time(NULL));
	// Get width and height of render context
	SDL_GetRendererOutputSize(theRenderer, &renderWidth, &renderHeight);
	this->m_lastTime = high_resolution_clock::now();
	// Clear the buffer with a black background
	SDL_SetRenderDrawColor(theRenderer, 0, 0, 0, 255);
	SDL_RenderPresent(theRenderer);

	theTextureMgr->setRenderer(theRenderer);
	theFontMgr->initFontLib();
	theSoundMgr->initMixer();

	// Store the textures of objects
	textureName = { "ufo1", "ufo2", "bullet","theRocket","theBackground", "instructions" };
	texturesToUse = { "Images\\ufo1.png", "Images\\ufo2.png", "Images\\bullet.png", "Images\\rocketSprite.png", "Images\\cartoonsky.png", "Images\\Instruct.png" };
	
	for (int tCount = 0; tCount < textureName.size(); tCount++)
	{
		theTextureMgr->addTexture(textureName[tCount], texturesToUse[tCount]);
	}
	// Create textures for Game Dialogue (text)
	fontList = { "digital", "spaceAge" };
	fontsToUse = { "Fonts/digital-7.ttf", "Fonts/space age.ttf" };
	
	for (int fonts = 0; fonts < fontList.size(); fonts++)
	{
		theFontMgr->addFont(fontList[fonts], fontsToUse[fonts], 36);
	}
	gameTextList = { "Fighter X", "Score", "Game Over" };

	theTextureMgr->addTexture("Title", theFontMgr->getFont("spaceAge")->createTextTexture(theRenderer, gameTextList[0], SOLID, { 255, 255, 255, 255 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("Score", theFontMgr->getFont("digital")->createTextTexture(theRenderer, gameTextList[1], SOLID, { 255, 255, 255, 255 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("Game Over", theFontMgr->getFont("digital")->createTextTexture(theRenderer, gameTextList[2], SOLID, { 255,255,255,255 }, { 0,0,0,0 }));

	
	// Store the  button textures
	btnNameList = { "exit_btn", "instructions_btn", "load_btn", "menu_btn", "play_btn", "save_btn", "settings_btn" };
	btnTexturesToUse = { "Images/button_exit.png", "Images/button_instructions.png", "Images/button_load.png", "Images/button_menu.png", "Images/button_play.png", "Images/button_save.png", "Images/button_settings.png" };
	btnPos = { { 400, 375 },{ 400, 300 },{ 400, 300 },{ 500, 500 },{ 400, 300 },{ 740, 500 },{ 400, 300 } };
	
	
	for (int bCount = 0; bCount < btnNameList.size(); bCount++)
	{
		theTextureMgr->addTexture(btnNameList[bCount], btnTexturesToUse[bCount]);
	}


	for (int bCount = 0; bCount < btnNameList.size(); bCount++)
	{
		cButton * newBtn = new cButton();
		newBtn->setTexture(theTextureMgr->getTexture(btnNameList[bCount]));
		newBtn->setSpritePos(btnPos[bCount]);
		newBtn->setSpriteDimensions(theTextureMgr->getTexture(btnNameList[bCount])->getTWidth(), theTextureMgr->getTexture(btnNameList[bCount])->getTHeight());
		thebuttonMgr->add(btnNameList[bCount], newBtn);
	}


	//setting the game state for the game beginning
	theGameState = MENU;
	theBtnType = EXIT;



	// Load game sounds
	soundList = { "theme", "shot", "explosion" };
	soundTypes = { MUSIC, SFX, SFX };
	soundsToUse = { "Audio/who10Edit.wav", "Audio/shot007.wav", "Audio/explosion2.wav" };
	for (int sounds = 0; sounds < soundList.size(); sounds++)
	{
		theSoundMgr->add(soundList[sounds], soundsToUse[sounds], soundTypes[sounds]);
	}

	theSoundMgr->getSnd("theme")->play(-1);


	//setting positions of backgrounds and sprites as the game starts
	spriteBkgd.setSpritePos({ 0, 0 });
	spriteBkgd.setTexture(theTextureMgr->getTexture("theBackground"));
	spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("theBackground")->getTWidth(), theTextureMgr->getTexture("theBackground")->getTHeight());


	theRocket.setSpritePos({ 25, 350 });
	theRocket.setTexture(theTextureMgr->getTexture("theRocket"));
	theRocket.setSpriteDimensions(theTextureMgr->getTexture("theRocket")->getTWidth(), theTextureMgr->getTexture("theRocket")->getTHeight());
	theRocket.setRocketVelocity({ 0, 0 });

	// Create vector array of enemies
	//this will pick randomly between the two ufo designs and spawn them on random areas on the y axis

	
	for (int astro = 0; astro < 5; astro++)

	{
		theAsteroids.push_back(new cAsteroid);
		theAsteroids[astro]->setSpritePos({ 650, 75 * (rand() % 8 + 1) });
		theAsteroids[astro]->setSpriteTranslation({ -5,rand() % 3 + 1 });
		int randAsteroid = rand() % 2;
		theAsteroids[astro]->setTexture(theTextureMgr->getTexture(textureName[randAsteroid]));
		theAsteroids[astro]->setSpriteDimensions(theTextureMgr->getTexture(textureName[randAsteroid])->getTWidth(), theTextureMgr->getTexture(textureName[randAsteroid])->getTHeight());
		FPoint newXY = { -0.1,-0.1 };
		theAsteroids[astro]->setSpriteScale(newXY);
		theAsteroids[astro]->scaleSprite();
		theAsteroids[astro]->setActive(true);

	}

}





void cGame::run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	//this loop will be used for when the game is active or not
	bool loop = true;

	while (loop)
	{
		//We get the time that passed since the last frame
		double elapsedTime = this->getElapsedSeconds();

		loop = this->getInput(loop);
		this->update(elapsedTime);
		this->render(theSDLWND, theRenderer);
	}
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	
	//this clears the renderer 
	SDL_RenderClear(theRenderer);
	spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());

	
	SDL_RenderClear(theRenderer);
	
	
//This switch and case statements dictate what is rendered during each game state

	switch (theGameState)
	{
	case MENU:
	{
		//in the menu game state the background, title, instructions image, and 2 buttons are rendered
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());

		// Render the Title

		cTexture* tempTextTexture = theTextureMgr->getTexture("Title");
		SDL_Rect pos = { 400, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		FPoint scale = { 3, 2 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		
		//render buttons

		thebuttonMgr->getBtn("play_btn")->render(theRenderer, &thebuttonMgr->getBtn("play_btn")->getSpriteDimensions(), &thebuttonMgr->getBtn("play_btn")->getSpritePos(), thebuttonMgr->getBtn("play_btn")->getSpriteScale());
		thebuttonMgr->getBtn("play_btn")->setSpritePos({ 450, 300 });
		thebuttonMgr->getBtn("exit_btn")->setSpritePos({ 450, 375 });
		thebuttonMgr->getBtn("exit_btn")->render(theRenderer, &thebuttonMgr->getBtn("exit_btn")->getSpriteDimensions(), &thebuttonMgr->getBtn("exit_btn")->getSpritePos(), thebuttonMgr->getBtn("exit_btn")->getSpriteScale());
	

		//render instructions image
		cTexture*tempTexture = theTextureMgr->getTexture("instructions");
		pos = { 400,425, tempTexture->getTextureRect().w, tempTexture->getTextureRect().h };
		scale = {0.5, 0.5};
		tempTexture->renderTexture(theRenderer, tempTexture->getTexture(), &tempTexture->getTextureRect(), &pos, scale);
	
	}
	break;
	case PLAYING:
	{
		//in the playing gamestate, the background, score, enemies and player are rendered.

		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());

	//render the buttons

			thebuttonMgr->getBtn("menu_btn")->setSpritePos({ 900, 650 });
			thebuttonMgr->getBtn("menu_btn")->render(theRenderer, &thebuttonMgr->getBtn("menu_btn")->getSpriteDimensions(), &thebuttonMgr->getBtn("menu_btn")->getSpritePos(), thebuttonMgr->getBtn("menu_btn")->getSpriteScale());
			thebuttonMgr->getBtn("load_btn")->setSpritePos({ 900, 500 });
			thebuttonMgr->getBtn("load_btn")->render(theRenderer, &thebuttonMgr->getBtn("load_btn")->getSpriteDimensions(), &thebuttonMgr->getBtn("load_btn")->getSpritePos(), thebuttonMgr->getBtn("load_btn")->getSpriteScale());
			thebuttonMgr->getBtn("save_btn")->setSpritePos({ 900, 575 });
			thebuttonMgr->getBtn("save_btn")->render(theRenderer, &thebuttonMgr->getBtn("save_btn")->getSpriteDimensions(), &thebuttonMgr->getBtn("save_btn")->getSpritePos(), thebuttonMgr->getBtn("save_btn")->getSpriteScale());


		// Render each asteroid in the vector array
		for (int draw = 0; draw < theAsteroids.size(); draw++)
		{
			theAsteroids[draw]->render(theRenderer, &theAsteroids[draw]->getSpriteDimensions(), &theAsteroids[draw]->getSpritePos(), theAsteroids[draw]->getSpriteRotAngle(), &theAsteroids[draw]->getSpriteCentre(), theAsteroids[draw]->getSpriteScale());
		}
		// Render each bullet in the vector array
		for (int draw = 0; draw < theBullets.size(); draw++)
		{
			theBullets[draw]->render(theRenderer, &theBullets[draw]->getSpriteDimensions(), &theBullets[draw]->getSpritePos(), theBullets[draw]->getSpriteRotAngle(), &theBullets[draw]->getSpriteCentre(), theBullets[draw]->getSpriteScale());
		}
		// render the rocket
		theRocket.render(theRenderer, &theRocket.getSpriteDimensions(), &theRocket.getSpritePos(), theRocket.getSpriteRotAngle(), &theRocket.getSpriteCentre(), theRocket.getSpriteScale());
		
		//rendering the changing score feature
		if (scoreChanged) {

			gameTextList[2] = scoreAsString.c_str();
			theTextureMgr->addTexture("Score", theFontMgr->getFont("digital")->createTextTexture(theRenderer, gameTextList[2], SOLID, { 255, 255, 255, 255 }, { 0, 0, 0, 0 }));


			scoreChanged = false;
		}

		tempTextTexture = theTextureMgr->getTexture("Score");
		pos = { 10, 50, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		SDL_RenderPresent(theRenderer);
	}
	break;
	case INSTRUCTIONS:
	{
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());

	}
	case END:
		//in the end gamestate, a game over text, score, and 2 butons are displayed
	{
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());

		cTexture* tempTextTexture = theTextureMgr->getTexture("Game Over");
		SDL_Rect pos = { 400, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		FPoint scale = { 3, 2 };

		//this renders the buttons
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		thebuttonMgr->getBtn("exit_btn")->setSpritePos({ 500, 575 });
		thebuttonMgr->getBtn("exit_btn")->render(theRenderer, &thebuttonMgr->getBtn("exit_btn")->getSpriteDimensions(), &thebuttonMgr->getBtn("exit_btn")->getSpritePos(), thebuttonMgr->getBtn("exit_btn")->getSpriteScale());
		thebuttonMgr->getBtn("menu_btn")->setSpritePos({ 500, 500 });
		thebuttonMgr->getBtn("menu_btn")->render(theRenderer, &thebuttonMgr->getBtn("menu_btn")->getSpriteDimensions(), &thebuttonMgr->getBtn("menu_btn")->getSpritePos(), thebuttonMgr->getBtn("menu_btn")->getSpriteScale());

		//this renders the updated score
		if (scoreChanged) {

			gameTextList[2] = scoreAsString.c_str();
			theTextureMgr->addTexture("Score", theFontMgr->getFont("digital")->createTextTexture(theRenderer, gameTextList[2], SOLID, { 255, 255, 255, 255 }, { 0, 0, 0, 0 }));


			scoreChanged = false;
		}

		tempTextTexture = theTextureMgr->getTexture("Score");
		pos = { 10, 50, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		SDL_RenderPresent(theRenderer);

	}
	break;
	case QUIT:
	{

	}
	break;
	default:
		break;
	}
	SDL_RenderPresent(theRenderer);

}
void cGame::update(double deltaTime)
{
	//Check the button clicked and state change

	if (theGameState == MENU || theGameState == END)
	{
		//this dictates what the buttons do during these states
		theGameState = thebuttonMgr->getBtn("exit_btn")->update(theGameState, QUIT, theAreaClicked);
		theGameState = thebuttonMgr->getBtn("play_btn")->update(theGameState, PLAYING, theAreaClicked);

	}
	else
	{
		// Update the visibility and position of each asteriod
		vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin();
		while (asteroidIterator != theAsteroids.end())
		{
			if ((*asteroidIterator)->isActive() == false)
			{
				asteroidIterator = theAsteroids.erase(asteroidIterator);
			}
			else
			{
				(*asteroidIterator)->update(deltaTime);
				++asteroidIterator;
			}
		}
		// Update the visibility and position of each bullet
		vector<cBullet*>::iterator bulletIterartor = theBullets.begin();
		while (bulletIterartor != theBullets.end())
		{
			if ((*bulletIterartor)->isActive() == false)
			{
				bulletIterartor = theBullets.erase(bulletIterartor);
			}
			else
			{
				(*bulletIterartor)->update(deltaTime);
				++bulletIterartor;
			}
		}
		/*
		==============================================================
		| Check for collisions
		==============================================================
		*/
		for (vector<cBullet*>::iterator bulletIterartor = theBullets.begin(); bulletIterartor != theBullets.end(); ++bulletIterartor)
		{
			//(*bulletIterartor)->update(deltaTime);
			for (vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin(); asteroidIterator != theAsteroids.end(); ++asteroidIterator)
			{
				if ((*asteroidIterator)->collidedWith(&(*asteroidIterator)->getBoundingRect(), &(*bulletIterartor)->getBoundingRect()))
				{
					// if a collision set the bullet and asteroid to false
					theSoundMgr->getSnd("explosion")->play(0);
					(*asteroidIterator)->setActive(false);
					(*bulletIterartor)->setActive(false);

					//increase score
					score++;

					//keeping the score updated
					if (theTextureMgr->getTexture("Score") != NULL) {
						theTextureMgr->deleteTexture("Score");
					}
					string theScore = to_string(score);
					scoreAsString = "score: " + theScore;
					scoreChanged = true;


				}

			}
		}

		//ending the game based on instructions
		if (score == 5)
		{
			theGameState = INSTRUCTIONS;
		}
		// Update the Rockets position
		theRocket.update(deltaTime);
	}
	//dictating what the buttons do
	theGameState = thebuttonMgr->getBtn("exit_btn")->update(theGameState, QUIT, theAreaClicked);


	theGameState = thebuttonMgr->getBtn("menu_btn")->update(theGameState, MENU, theAreaClicked);

}


bool cGame::getInput(bool theLoop)
//this section dictates what the key presses do
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			theLoop = false;
		}

		switch (event.type)
		{
		case SDL_MOUSEBUTTONDOWN:
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
			{
				theAreaClicked = { event.motion.x, event.motion.y };
			}
			break;
			case SDL_BUTTON_RIGHT:
				break;
			default:
				break;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
			{
			}
			break;
			case SDL_BUTTON_RIGHT:
				break;
			default:
				break;
			}
			break;
		case SDL_MOUSEMOTION:
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				theLoop = false;
				break;
			case SDLK_DOWN:
			{
				if (theRocket.getSpritePos().x < (renderWidth - theRocket.getSpritePos().w))
				{
					theRocket.setSpriteTranslation({ -5, -5 });
				}
			}
			break;

			case SDLK_UP:
			{
				if (theRocket.getSpritePos().x > 0)
				{
					theRocket.setSpriteTranslation({ 5, 5 });
				}
			}
			break;

			case SDLK_SPACE:
			{
				//this fires the bullets and dictates their speed, size
				theBullets.push_back(new cBullet);
				int numBullets = theBullets.size() - 1;
				theBullets[numBullets]->setSpritePos({ theRocket.getBoundingRect().x + theRocket.getSpriteCentre().x, theRocket.getBoundingRect().y + theRocket.getSpriteCentre().y });
				theBullets[numBullets]->setSpriteTranslation({ 2, 2 });
				theBullets[numBullets]->setTexture(theTextureMgr->getTexture("bullet"));
				theBullets[numBullets]->setSpriteDimensions(theTextureMgr->getTexture("bullet")->getTWidth(), theTextureMgr->getTexture("bullet")->getTHeight());
				theBullets[numBullets]->setBulletVelocity({ 2, 2 });
				theBullets[numBullets]->setSpriteRotAngle(theRocket.getSpriteRotAngle());
				theBullets[numBullets]->setActive(true);
				cout << "Bullet added to Vector at position - x: " << theRocket.getBoundingRect().x << " y: " << theRocket.getBoundingRect().y << endl;
			}
			//this plays the shot sound 
			theSoundMgr->getSnd("shot")->play(0);
			break;
			default:
				break;

			}
		default:
			break;
		}
		if (theGameState == QUIT)
		{
			//this ends the game
			theLoop = false;
		}
	}
	return theLoop;
}

double cGame::getElapsedSeconds()
{
	this->m_CurrentTime = high_resolution_clock::now();
	this->deltaTime = (this->m_CurrentTime - this->m_lastTime);
	this->m_lastTime = this->m_CurrentTime;
	return deltaTime.count();
}

void cGame::cleanUp(SDL_Window* theSDLWND)
{
	// Delete our OpengL context
	SDL_GL_DeleteContext(theSDLWND);

	// Destroy the window
	SDL_DestroyWindow(theSDLWND);

	// Quit IMG system
	IMG_Quit();

	// Shutdown SDL 2
	SDL_Quit();
}

