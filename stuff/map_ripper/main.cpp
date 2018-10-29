#include <SFML/Graphics.hpp>
#include <math.h>

#define TILE_SIZE 16

long getTileVal(sf::Image img, unsigned int tileX, unsigned int tileY){
    long tileVal = 0;
    int pixelIdx = 0;

    for(unsigned int x = tileX; x < tileX + TILE_SIZE; ++x){
        for(unsigned int y = tileY; y < tileY + TILE_SIZE; ++y){
            sf::Color col = img.getPixel(x, y);
            int currentCol = col.r + col.g + col.b;
            tileVal += currentCol * pixelIdx;

            pixelIdx ++;
        }
    }

    return tileVal;
}


char getTileFrom(sf::Image img, unsigned int tileX, unsigned int tileY){
    switch(getTileVal(img, tileX, tileY)){
            case 500160: return '.';  //grass
            case 2946240: return '&'; //forest
            case 1397760: return '~'; //water
            case 3323520: return '^';  //mountains
            case 4406400: return ';';  //town
            case 4398720: return '(';  //castle
            case 8907840: return '|';  //portal
            case 2325120: return ':';  //village
            case 5139840: return '?';  //signpost
            case 3886080: return '*';  //dungeon
            case 5444160: return '!';  //tower
            case 1140480: return '`';  //swamp
            case 15667200: return '#';  //wall
            case 2030080: return ',';  //road
            case 6517760: return '=';  //forceField
        // case 3916800: return '+';  //door  // originally, letter "I" is used as doors
            case 2008800: return '[';  //deskLeft
            case 1958400: return '_';  //desk
            case 3621600: return ']';  //deskRight
            case 5592480: return 'A';  //A
            case 5180160: return 'B';  //B
            case 4620960: return 'C';  //C
            case 4987200: return 'D';  //D
            case 4867680: return 'E';  //E
            case 4430400: return 'F';  //F
            case 4883520: return 'G';  //G
            case 5163840: return 'H';  //H
            case 3916800: return 'I';  //I
            case 4600320: return 'J';  //J
            case 5628960: return 'K';  //K
            case 4542720: return 'L';  //L
            case 6983040: return 'M';  //M
            case 5224800: return 'N';  //N
            case 5140800: return 'O';  //O
            case 4776480: return 'P';  //P
        case 1145: return 'Q';  //Q
            case 5067360: return 'R';  //R
            case 5027520: return 'S';  //S
            case 4498560: return 'T';  //T
            case 5292480: return 'U';  //U
            case 5331360: return 'V';  //V
            case 6853440: return 'W';  //W
        case 1152: return 'X';  //X
            case 5424960: return 'Y';  //Y
            case 5332320: return 'Z';  //Z
        // case 1155: return '<';  //ladderUp
        // case 1156: return '>';  //ladderDown

            case 4563840: return 'q';  //horse
            case 5165280: return 'w';  //ship
            case 4770240: return 'e';  //plane
            case 2713440: return 'r';  //shuttle
            case 878400: return 't';  //sword
            case 3990240: return 'y';  //shield
        // case 1163: return 'u';  //chest
            case 3175200: return 'i';  //thief
            case 4249440: return 'o';  //wizard
            case 3503040: return 'p';  //cleric
            case 2979360: return 'a';  //warrior
            case 3327360: return 's';  //jester
            case 3096480: return 'd';  //merchant
            case 4911360: return 'f';  //guard
            case 5865600: return 'g';  //guard2
        // case 1172: return 'h';  //british
            case 4912320: return 'i';  //orc
            case 3292800: return 'j';  //daemon
            case 4039840: return 'k';  //devil
            case 7608960: return 'l';  //balron
        // case 1177: return 'z';  //serpent
        // case 1178: return 'x';  //beholder
        // case 1179: return 'c';  //ghost
        // case 1180: return 'v';  //viper
        // case 1181: return 'b';  //gremlin
        // case 1182: return 'n';  //daemonBig
        // case 1183: return 'm';  //balrog
    }

    return ' ';
}

int main(int argc, char *argv[]){

    if(argc < 2){
        printf("Gimme something to work with.\nI mean, provide an image file path or sth?");
        return 0;
    }

    sf::Texture texture;
    if (!texture.loadFromFile(argv[1])){
        return EXIT_FAILURE;
    }

    sf::Image img = texture.copyToImage();

    int mapSizeX = img.getSize().x / TILE_SIZE;
    int mapSizeY = img.getSize().y / TILE_SIZE;
    char* map = new char[mapSizeX * (mapSizeY+4)];
    int idx = 0;
    unsigned int imgSizeX = img.getSize().x;
    unsigned int imgSizeY = img.getSize().y;

    if(true){
        for(unsigned int y = 0; y < imgSizeY; y+=TILE_SIZE){
            map[idx] = '\"'; idx ++;
            for(unsigned int x = 0; x < imgSizeX; x+=TILE_SIZE){
                map[idx] = getTileFrom(img, x, y);
                idx ++;
            }
            map[idx] = '\"'; idx ++;
            map[idx] = ','; idx ++;
            map[idx] = '\n'; idx++;
        }

        map[idx] = '\0';

        puts(map);
    }

    return 0; // rest of the code was used to determine tile "codes". no need to execute it now

    sf::RenderWindow app(sf::VideoMode(1024, 1024), "Bestima ][ Map Ripper");

    sf::Sprite sprite(texture);

    while (app.isOpen()){
        sf::Event event;
        while (app.pollEvent(event)){
            if (event.type == sf::Event::Closed){
                app.close();
            }

            if(event.type == sf::Event::MouseButtonPressed){
                int tileX = event.mouseButton.x / TILE_SIZE;
                int tileY = event.mouseButton.y / TILE_SIZE;

                printf("Tile: %dx%d: %ld\n", tileX, tileY, getTileVal(img, tileX*TILE_SIZE, tileY*TILE_SIZE));
            }
        }

        app.clear();
        app.draw(sprite);
        app.display();
    }

    return EXIT_SUCCESS;
}









