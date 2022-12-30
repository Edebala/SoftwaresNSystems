//
// Created by Apor Biro on 23.11.2022.
//
#include "Game.h"

Creature::Creature(){}

Creature::Creature(const string& name, int hp, int maxHp, int strength,vector<Effect*>effect){
    this->name = name;
    this->hp = hp;
    this->maxHp = maxHp;
    this->strength = strength;
		effects = effect;
}

void Creature::setName(const string& name){
    this->name = name;
}

void Creature::setHp(int hp){
		this->hp = hp<0?0:(hp>maxHp?maxHp:hp);
}

void Creature::setMaxHp(int maxHp){
    this->maxHp = maxHp;
}

void Creature::setStrength(int strength){
    this->strength = strength;
}

int Creature::affect(FightLog* log){
	for(long unsigned int i=0;i<effects.size();i++){
		log->addLine(name + " has been affected by" + effects[i]->getName());
		if(!effects[i]->affect(this)) return 0;
		if(!effects[i]->getDuration()){
			delete effects[i];
			effects.erase(effects.begin()+i--);
		}
	}
	return 1;
}

void Creature::addEffect(Effect *effect){
	effects.push_back(effect);
}	

void Humanoid::setInventory(Inventory* inventory) {
    this->inventory = inventory;
}

string Creature::getName(){
    return this->name;
}

int Creature::getHp() const{
    return this->hp;
}

int Creature::getMaxHp() const{
    return this->maxHp;
}

int Creature::getStrength() const {
    return this->strength;
}

Inventory* Humanoid::getInventory() const {
    return this->inventory;
}

// Do Creatures have an Inventory? - Ede

bool Creature::attack(Creature* enemy) const{
		return enemy->damage(strength);
}

int Creature::damage(int dmg){
    setHp(hp-dmg);
	return getHp();
}


bool Humanoid::addElementToInventory(Slot* element) {
    return this->inventory->addElement(element);
}

int Humanoid::useElementFromInventoryByIndex(int index, Humanoid * player, Creature * enemy) {
    return this->inventory->useElementByIndex(index, player, enemy);
}

// Humanoid functions implementation

Humanoid::Humanoid(const string& name, int hp, int maxHp, int strength, vector<Effect*> effect, Armor *armor, Weapon *weapon):Creature(name,hp,maxHp,strength,effect){
    this->armor = armor;
    this->weapon = weapon;
		inventory = new Inventory();
}

Humanoid::Humanoid(string fileName){
	ifstream file(fileName.c_str());
	if(!file.is_open()){exit(-1);}
	file>>hp>>maxHp>>strength;
	string armorName, weaponName;
	file>>armorName;
	if(armorName != "NONE"){
		int armorDefense;
		file >>armorDefense;
		setArmor(new Armor(armorName,armorDefense));
	}
	file>>weaponName;
	if(weaponName != "NONE"){
		int weaponDamage;
		file >>weaponDamage;
		setWeapon(new Weapon(weaponName,weaponDamage));
	}
	string slotName;
	while(file>>slotName){
		string slotType;
		int data1,data2;
		file>> slotType;
		if(slotType == "Armor"){
			file>>data1;
			addElementToInventory(new Armor(slotName,data1));
		}
		if(slotType == "Weapon"){
			file>>data1;
			addElementToInventory(new Weapon(slotName,data1));
		}
		if(slotType == "HealingPotion"){
			file>>data1>>data2;
			addElementToInventory(new HealingPotion(slotName,data1,data2));
		}
		if(slotType == "Fireball"){
			addElementToInventory(new Fireball());
		}
		if(slotType == "PoisonGas"){
			file>>data1;
			addElementToInventory(new PoisonGas(data1));
		}
		if(slotType == "Mend"){
			file>>data1;
			addElementToInventory(new Mend(data1));
		}
	}
}


void Humanoid::setArmor(Armor* armor){
    this->armor = armor;
}

void Humanoid::setWeapon(Weapon* weapon){
    this->weapon = weapon;
}

Weapon* Humanoid::getWeapon() const{
    return this->weapon;
}

Armor* Humanoid::getArmor() const{
    return this->armor;
}

bool Humanoid::attack(Creature *enemy) const{
    return enemy->damage((getWeapon()==nullptr)?strength:strength*getWeapon()->getDamage());
}

int Humanoid::damage(int dmg){
	setHp((getArmor()==nullptr)?hp-dmg:hp-max(dmg-armor->getDefense(),0));
    return getHp();
}

int Creature::turn(Creature* enemy,Camera* camera,FightLog* log){
		log->addLine(name +" has attacked");
    return attack(enemy);
}


int getMoveChoice(Camera* camera,FightLog* log){
	SDL_Renderer* renderer = camera->getRenderer();
	SDL_Window* window = camera->getWindow();
	SDL_Event event;
	
	SDL_Surface *buffer = IMG_Load("Assets/Brick.png");
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer,buffer);

	UIBox box(300,300,600,600,10);
	box.setTexture(texture);
	box.subDivide(0.5,.3);

	buffer = IMG_Load("Assets/Attack.png");
	texture = SDL_CreateTextureFromSurface(renderer,buffer);
	box.getSubDivisions()[0]->setTexture(texture);
	
	buffer = IMG_Load("Assets/Use.png");
	texture = SDL_CreateTextureFromSurface(renderer,buffer);
	box.getSubDivisions()[1]->setTexture(texture);

	buffer = IMG_Load("Assets/TextBackground.png");
	texture = SDL_CreateTextureFromSurface(renderer,buffer);
	box.getSubDivisions()[2]->setTexture(texture);
	
	UIBox *logBox = box.getSubDivisions()[2];
	logBox->createGrid(1,log->getMaxSize());
  
	TTF_Init();
	TTF_Font *font;
	if(!(font = TTF_OpenFont("Assets/ARCADECLASSIC.TTF",32)))std::cout<<TTF_GetError()<<"\n";
	SDL_Surface *surf;
	SDL_Texture *textTexture;

	SDL_Color White = {255,255,255};

	for(int i=0;i<log->getLines(64).size();i++){
		surf = TTF_RenderText_Solid(font,log->getLines(64)[i].c_str(),White);
		textTexture = SDL_CreateTextureFromSurface(renderer,surf);
		logBox->getSubDivisions()[i]->setTexture(textTexture);
	}

	int mx,my;
	while(event.type != SDL_QUIT){
		SDL_PollEvent(&event);
		SDL_GetMouseState(&mx,&my);
		if(box.getSubDivisions()[0]->isPressed(&event,mx,my))
			return 1;
		if(box.getSubDivisions()[1]->isPressed(&event,mx,my))
			return 2;
		//SDL_RenderClear(renderer);
		box.draw(renderer);
		SDL_RenderPresent(renderer);
		usleep(6000);
	}
};

int getInventoryChoice(Humanoid* player,Camera* camera){
	SDL_Renderer* renderer = camera->getRenderer();
	SDL_Window* window = camera->getWindow();
	SDL_Event event;
	
	SDL_Surface *buffer = IMG_Load("Assets/Brick.png");
	SDL_Texture *bricktxt = SDL_CreateTextureFromSurface(renderer,buffer);

	buffer = IMG_Load("Assets/QuestionMark.png");
	SDL_Texture *questiontxt = SDL_CreateTextureFromSurface(renderer,buffer);

	buffer = IMG_Load("Assets/Sword.png");
	SDL_Texture *swordtxt = SDL_CreateTextureFromSurface(renderer,buffer);

	buffer = IMG_Load("Assets/Armor.png");
	SDL_Texture *armortxt = SDL_CreateTextureFromSurface(renderer,buffer);

	buffer = IMG_Load("Assets/HPPotion.png");
	SDL_Texture *hppottxt = SDL_CreateTextureFromSurface(renderer,buffer);

	buffer = IMG_Load("Assets/FireBall.png");
	SDL_Texture *fireballtxt = SDL_CreateTextureFromSurface(renderer,buffer);

	buffer = IMG_Load("Assets/PoisonGas.png");
	SDL_Texture *poisongastxt = SDL_CreateTextureFromSurface(renderer,buffer);

	buffer = IMG_Load("Assets/Mend.png");
	SDL_Texture *mendtxt = SDL_CreateTextureFromSurface(renderer,buffer);

	UIBox box(300,300,600,600,10);
	box.setTexture(bricktxt);
	box.createGrid(5,4);

	int mx,my;

	for(int i=0;i<player->getInventory()->getElements().size();i++){
		if(typeid(*(player->getInventory()->getElements()[i]))== typeid(Armor))
			box.getSubDivisions()[i]->setTexture(armortxt);
		else if(typeid(*(player->getInventory()->getElements()[i]))== typeid(Weapon))
			box.getSubDivisions()[i]->setTexture(swordtxt);
		else if(typeid(*(player->getInventory()->getElements()[i]))== typeid(HealingPotion))
			box.getSubDivisions()[i]->setTexture(hppottxt);
		else if(typeid(*(player->getInventory()->getElements()[i]))== typeid(Fireball))
			box.getSubDivisions()[i]->setTexture(fireballtxt);
		else if(typeid(*(player->getInventory()->getElements()[i]))== typeid(PoisonGas))
			box.getSubDivisions()[i]->setTexture(poisongastxt);
		else if(typeid(*(player->getInventory()->getElements()[i]))== typeid(Mend))
			box.getSubDivisions()[i]->setTexture(mendtxt);
		else
			box.getSubDivisions()[i]->setTexture(questiontxt);

	}
	while(event.type != SDL_QUIT){
		SDL_PollEvent(&event);
		SDL_GetMouseState(&mx,&my);
		for(int i=0;i<player->getInventory()->getElements().size();i++)
			if(box.getSubDivisions()[i]->isPressed(&event,mx,my))
				return i;
		//SDL_RenderClear(renderer);
		box.draw(renderer);
		SDL_RenderPresent(renderer);
		usleep(6000);
	}
};

int Humanoid::turn(Creature* enemy,Camera* camera,FightLog* log){
  int move = getMoveChoice(camera,log);
  switch (move) {
  case 1: 
		log->addLine(name +" has attacked");
		return attack(enemy);
    break;
  case 2:
    int chosen = getInventoryChoice(this,camera); 
		log->addLine(name +" has used "+ getInventory()->getElements()[chosen]->getName());
		return useElementFromInventoryByIndex(chosen,this,enemy);
  }
	return 1;
}
