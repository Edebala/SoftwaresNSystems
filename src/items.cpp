//
// Created by Apor Biro on 23.11.2022.
//

#include "../include/items.h"

// Item class implementations

Item::Item(const string& name){
    this->name = name;
}

void Item::setName(const string& name) {
    this->name = name;
}

string Item::getName(){
    return this->name;
}

// Weapon class implementations

Weapon::Weapon(const string& name, int damage) : Item(name) {
    this->damage = damage;
}

Weapon::Weapon(const string &name, float magicAdjust, int damage) : Item(name) {
    this->damage = damage;
    this->magicAdjust = magicAdjust;
}

int Weapon::getDamage() const {
    return this->damage;
}

float Weapon::getMagicAdjust() const {
    return this->magicAdjust;
}

// Armor class implementations

Armor::Armor(const string& name, int defense) : Item(name) {
    this->defense = defense;
}

int Armor::getDefense() const{
    return this->defense;
}