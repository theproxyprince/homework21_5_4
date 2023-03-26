#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>

struct character {
    std::string name = "noname";
    std::string fraction;
    int armor = 0;
    int health = 0;
    int damage = 0;
    int x = 0;
    int y = 0;
};

struct field {
    char npc[40];
};

void battle(character& p, character& e, field f[]) {
    e.armor -= p.damage;
    if (e.armor < 0) {
        e.health += e.armor;
        e.armor = 0;
    }
    std::cout << e.name << " take " << p.damage << "DMG from " << p.name << "." << std::endl;
    std::cout << e.name << " HP: " << e.health << " AP: " << e.armor << std::endl;
    if (e.health <= 0) {
        e.health = 0;
        e.fraction = "dead";
        std::cout << e.name << " DEAD" << std::endl;
        f[e.y].npc[e.x] = '.';
    }
}

void view(field f[]) {
    for (int i = 0; i < 40; ++i) {
        for (int j = 0; j < 40; ++j) {
            std::cout << f[i].npc[j] << " ";
        }
        std::cout << std::endl;
    }
}

void playermove(character &p, std::string direction, field f[], character e[]) {
    if (direction == "w") {
        if (p.y - 1 >= 0 && f[p.y - 1].npc[p.x] == 'E') {
            int i = 0;
            while (e[i].x != p.x && e[i].y != p.y - 1) {
                ++i;
            }
            battle(p, e[i], f);
        } else if (p.y - 1 >= 0 ) {
            p.y -= 1;
        } else {
            std::cout << "You are set against the world border." << std::endl;
        }
    } else if (direction == "s") {
        if (p.y + 1 < 40 && f[p.y + 1].npc[p.x] == 'E') {
            int i = 0;
            while (e[i].x != p.x && e[i].y != p.y + 1) {
                ++i;
            }
            battle(p, e[i], f);
        } else if (p.y + 1 < 40) {
            p.y += 1;
        } else {
            std::cout << "You are set against the world border." << std::endl;
        }
    } else if (direction == "a") {
        if (p.x - 1 >= 0 && f[p.y].npc[p.x - 1] == 'E') {
            int i = 0;
            while (e[i].x != p.x - 1 && e[i].y != p.y) {
                ++i;
            }
            battle(p, e[i], f);
        } else if (p.x - 1 >= 0) {
            p.x -= 1;
        } else {
            std::cout << "You are set against the world border." << std::endl;
        }
    } else if (direction == "d") {
        if (p.x + 1 < 40 && f[p.y].npc[p.x + 1] == 'E') {
            int i = 0;
            while (e[i].x != p.x + 1 && e[i].y != p.y) {
                ++i;
            }
            battle(p, e[i], f);
        } else if (p.x + 1 < 40) {
            p.x += 1;
        } else {
            std::cout << "You are set against the world border." << std::endl;
        }
    } else if (direction == "skip") {
        std::cout << "You are skip your turn." << std::endl;
    } else {
        std::cout << "Unknown command. You are skip your turn." << std::endl;
    }
}

void move(character& e, field f[], character& p) {
    f[e.y].npc[e.x] = '.';
    int i = e.x;
    e.x += 1 - rand() % 2;
    if ((e.x >= 40 || e.x < 0) || (f[e.y].npc[e.x] != '.')) {
        if (f[e.y].npc[e.x] == 'P') {
            battle(e, p, f);
            e.x = i;
        }
        e.x = i - 1;
    }
    if (i - e.x == 0) {
        int cury = e.y;
        e.y += 1 - rand() % 2;
        if ((e.y >= 40 || e.y < 0) || (f[e.y].npc[e.x] != '.')) {
            if (f[e.y].npc[e.x] == 'P') {
                battle(e, p, f);
                e.y = cury;
            }
            e.y = cury - 1;
        }
    }
    if (e.fraction == "dead") {
        f[e.y].npc[e.x] = '.';
    }
    else {
        f[e.y].npc[e.x] = 'E';
    }
}

void save(std::ofstream& file, character& p, int& enemies) {
    int len = p.name.length();
    file.write((char*)&len, sizeof(len));
    file.write(p.name.c_str(), len);
    len = p.fraction.length();
    file.write((char*)&len, sizeof(len));
    file.write(p.fraction.c_str(), len);
    file.write((char*)&p.health, sizeof(p.health));
    file.write((char*)&p.armor, sizeof(p.armor));
    file.write((char*)&p.damage, sizeof(p.damage));
    file.write((char*)&p.x, sizeof(p.x));
    file.write((char*)&p.y, sizeof(p.y));
    file.write((char*)&enemies, sizeof(enemies));
}

void load(std::ifstream& file, character& p, int& enemies) {
    int len;
    file.read((char*)&len, sizeof(len));
    p.name.resize(len);
    file.read((char*)p.name.c_str(), len);
    file.read((char*)&len, sizeof(len));
    p.fraction.resize(len);
    file.read((char*)p.fraction.c_str(), len);
    file.read((char*)&p.health, sizeof(p.health));
    file.read((char*)&p.armor, sizeof(p.armor));
    file.read((char*)&p.damage, sizeof(p.damage));
    file.read((char*)&p.x, sizeof(p.x));
    file.read((char*)&p.y, sizeof(p.y));
    file.read((char*)&enemies, sizeof(enemies));
}

int main() {
    std::string command;
    field f[40];
    character p;
    character e[5];
    int enemies;

    std::cout << "Input command (start/load/exit): ";
    std::cin >> command;
    if (command == "load") {
        for (int i = 0; i < 40; ++i) {
            for (int j = 0; j < 40; ++j) {
                f[i].npc[j] = '.';
            }
        }
        std::ifstream file("../save.bin", std::ios::binary);
        load(file, p, enemies);
        f[p.y].npc[p.x] = 'P';
        for (int i = 0; i < 5; ++i) {
            load(file, e[i], enemies);
        }
        std::cout << p.name << " HP: " << p.health << " AP: " << p.armor << " DMG: " << p.damage << " Status: " << p.fraction << std::endl;
        for (int i = 0; i < 5; ++i) {
            std::cout << e[i].name << " HP: " << e[i].health << " AP: " << e[i].armor << " DMG: " << e[i].damage << " Status: " << e[i].fraction << std::endl;
            if (e[i].fraction != "dead") {
                f[e[i].y].npc[e[i].x] = 'E';
            }
        }
        view(f);
    } else if (command == "start") {
        std::cout << "Enter name of your character: ";
        p.fraction = "player";
        std::cin >> p.name;

        std::cout << "Welcome to HEROES, " << p.name << "!" << std::endl;
        std::cout << "Enter characteristics of your character: " << std::endl;
        std::cout << "Health: ";
        std::cin >> p.health;
        std::cout << "Armor: ";
        std::cin >> p.armor;
        std::cout << "Damage: ";
        std::cin >> p.damage;
        std::cout << "Great, " << p.name << "! You are have " << p.health << "HP, " << p.armor << "AP and " << p.damage << "DMG." << std::endl;
        std::cout << "Now generate your enemies." << std::endl;

        for (int i = 0; i < 5; ++i) {
            e[i].fraction = "enemy";
            e[i].name = "Enemy #" + std::to_string(i + 1);
            e[i].health = 150 - rand() % 100;
            e[i].armor = rand() % 51;
            e[i].damage = 30 - rand() % 15;
        }

        std::cout << "Ready. Your enemies: " << std::endl;
        for (int i = 0; i < 5; ++i) {
            std::cout << e[i].name << ": " << e[i].health << "HP, " << e[i].armor << "AR, " << e[i].damage << "DMG" << std::endl;
        }

        std::cout << "Lets play!" << std::endl;

        for (int i = 0; i < 40; ++i) {
            for (int j = 0; j < 40; ++j) {
                f[i].npc[j] = '.';
            }
        }
        p.x = rand() % 39;
        p.y = rand() % 39;
        f[p.y].npc[p.x] = 'P';

        for (int i = 0; i < 5; ++i) {
            e[i].x = rand() % 39;
            e[i].y = rand() % 39;
            f[e[i].y].npc[e[i].x] = 'E';
        }

        std::cout << std::endl;

        view(f);

        enemies = 5;
    } else if (command == "exit") {
        return 0;
    } else {
        std::cout << "Unknown command." << std::endl;
        return main();
    }

    do {
        for (int i = 0; i < 5; ++i) {
            std::cout << e[i].name << " " << e[i].fraction << " " << e[i].armor << " " << e[i].health << " " << e[i].damage << " " << e[i].x << " " << e[i].y << std::endl;
        }
        std::cout << "Enemies left: " << enemies << std::endl;
        std::cout << "Enter command(w/s/a/d or save/load/exit): ";
        std::cin >> command;
        if (command == "save") {
            std::ofstream file("../save.bin", std::ios::binary);
            save(file, p, enemies);
            for (int i = 0; i < 5; ++i) {
                save(file, e[i], enemies);
            }
        } else if (command == "load") {
            for (int i = 0; i < 40; ++i) {
                for (int j = 0; j < 40; ++j) {
                    f[i].npc[j] = '.';
                }
            }
            std::ifstream file("../save.bin", std::ios::binary);
            load(file, p, enemies);
            f[p.y].npc[p.x] = 'P';
            for (int i = 0; i < 5; ++i) {
                load(file, e[i], enemies);
            }
            std::cout << p.name << " HP: " << p.health << " AP: " << p.armor << " DMG: " << p.damage << " Status: " << p.fraction << std::endl;
            for (int i = 0; i < 5; ++i) {
                std::cout << e[i].name << " HP: " << e[i].health << " AP: " << e[i].armor << " DMG: " << e[i].damage << " Status: " << e[i].fraction << std::endl;
                if (e[i].fraction != "dead") {
                    f[e[i].y].npc[e[i].x] = 'E';
                }
            }
            view(f);
        } else if (command == "exit") {
            return 0;
        } else {
            enemies = 0;
            f[p.y].npc[p.x] = '.';
            playermove(p, command, f, e);
            if (p.fraction == "dead") {
                f[p.y].npc[p.x] = 'X';
            } else {
                f[p.y].npc[p.x] = 'P';
            }

            for (int i = 0; i < 5; ++i) {
                if (e[i].fraction != "dead") {
                    ++enemies;
                    move(e[i], f, p);
                }
            }

            view(f);
        }
    } while (enemies > 0 && p.fraction != "dead");

    if (enemies == 0 && p.fraction != "dead") {
        std::cout << p.name << " WIN!" << std::endl;
    }
    else {
        std::cout << "ENEMIES WIN!" << std::endl;
    }
    return 0;
}