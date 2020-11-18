//Trabalho feito por Arthur Brackmann Pires e Leonardo Rezende Alles
#ifdef _WIN32
#include <windows.h>
#else
#error "Sitema não suportado."
#endif
#include <stdio.h>
#include <math.h>
#include <locale.h>
#include <conio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <conio2.h>

#define LINM 11
#define COLM 27
#define xM 120
#define yM 25

typedef struct entities
{
    int x, y, xIn, yIn, dir, hp, imortal;
}ENTITY;
typedef struct level
{
    int keys, coins;
    int ogreN, playerN, entitiesN;
}LEVEL;


void cursorState(int cursor);//Desativa o cursor se cursor=0 ou o ativa se cursor=1
void gotoxy(int x, int y);//Vai com o cursor para as coordenadas x e y.
void menuIn(int *tela, int *selected);//Desenha o menu de inicialização e cria opções interativas.
void mapDraw(ENTITY entities[], LEVEL level,char map[][COLM],char mapC[][COLM], int mapSize);//Desenha na tela o char de cima(map).
void leverSwitch(ENTITY player, char mapC[][COLM], char map[][COLM]);//Função que troca o estado das portas se uma alavanca é acionada.
void playerMov(ENTITY *player, char mapC[][COLM], char map[][COLM]);//Realiza o movimento de player
void ogreIa(ENTITY *entities, LEVEL level, char map[][COLM], int timer);//Lida com a IA dos ogros.
void statsDisplay(int hpN, long int coinsN, long int pointsN, int mapSize, int imortal, int levelN);//Desenha na tela algumas informações(pontos,moedas,level).
void coins(int pX, int pY, long int coinsN[], long int *pointsN, char mapC[][COLM]);//Coleta as moedas quando o player passa por cima.
void keys(int pX, int pY, int *keysN, char mapC[][COLM]);//Coleta as chaves quando o player passa por cima.
void invulnerable(ENTITY *player, int timer[], long int coinsN[], int totalMapC);//Controla a invulnerabilidade do player
void ingameMenu(char dir, int *tela, int *selected);//Controla o menu do jogo.
void ingameCommands(ENTITY entities[], LEVEL *level, char dir, long int coinsN[], long int *pointsN, int *levelN, int *keysN, int mapSize, char map[][COLM], char mapC[][COLM], int *tela);//Função que lida com os comandos executaveis dentro do jogo.
void death(ENTITY entities[], LEVEL level, char map[][COLM], char mapC[][COLM], long int *pointsN, int *tela);//Controla a morte de inimigos e players.
void gameOver(int *tela);//Tela de Game Over.
int pointsRank(long int pointsN, char nome[]);//Responsável por ranquear a pontuação do jogador no final do jogo.
void save_or_load(ENTITY *entities, LEVEL *level, char map[][COLM], char mapC[][COLM], long int *pointsN, int *levelN, long int coinsN[], int *keysN, int timer[], int *tela, int selected, long int saveData[][9]);//Salva ou carrega o jogo de um slot.
int loadLevel(ENTITY *entities, LEVEL *level, int levelN, char map[][COLM], char mapC[][COLM], int *tela);//Carrega o mapa do level
void nextLevel(ENTITY *entities, LEVEL *level, int *keysN, int *levelN, char map[][COLM], char mapC[][COLM], int *tela, long int coinsN[], long int pointsN);//Faz o avanço para o próximo nível.
void options(int *tela, int *mapSize, int selected);//Abre a tela de opções interativas.

int main()
{
    ENTITY entities[LINM*COLM];
    LEVEL level;
    FILE *save_options;//Arquivo que guarda as opções selecionadas e o saveData.
    int tela=0, //Nessa variável é guardado qual a tela atual do jogo.
    sair=0, //Flag indicando que o jogo deve terminar.
    mapSize=1; //Indica o tamanho do mapa em unidades: 1x1, 2x2, 3x3...
    int keysN=0,//Guarda quantidade de chaves coletadas
    selected=0,//Indica qual opção do menu dentro do jogo foi selecionado
    levelN=1,//Indica o level atual
    timer[2]={0};//Indica tempo decorrido
    long int saveData[4][9]={0},//Informação relativas ao momento que um save foi feito(data,hora,ponto,moedas...)
    pointsN=0, //Guarda a quantidade de pontos adquiridos.
    coinsN[3]={0};//Guarda o número de moedas coletadas.

    char mapC[LINM][COLM];//Mapa e cópia do mapa
    char map[LINM][COLM];

    setlocale(LC_ALL,"Portuguese");

    srand(time(NULL));

    if(!(save_options = fopen("options.bin", "rb")))
    {
        //printf("Arquivo options.bin não encontrado.\n");
        //Arquivo não encontrado ou é o primeiro Load.
    }
    else
    {
        if(fread(saveData, sizeof(long int), 9*4, save_options)==9*4)
        {
            if(fread(&mapSize, sizeof(long int), 1, save_options)==1);
            else
            {
                printf("Não foi possível achar mapSize em options.bin\n");
                Sleep(2000);
            }
        }
        else
        {
           printf("Não foi possível achar saveData em options.bin\n");
           Sleep(2000);
        }
    }
    fclose(save_options);

    do
    {
        switch(tela)//Switch gerencia as telas á serem mostradas ao jogador.
        {
            case 0: system("cls");
                    cursorState(1);
                    menuIn(&tela, &selected);
                    if(tela==1) loadLevel(entities, &level, levelN, map, mapC, &tela);
                break;
            case 1: cursorState(0);
                    mapDraw(entities, level, map, mapC, mapSize);
                break;
            case 2: system("cls");
                    save_or_load(entities, &level, map, mapC, &pointsN, &levelN, coinsN, &keysN, timer, &tela, selected, saveData);
                break;
            case 3: system("cls");
                break;
            case 4: system("cls");
                    cursorState(1);
                    options(&tela,&mapSize,selected);
                break;
            case 5: sair=1;
                break;
            case 6: system("cls");
                    gameOver(&tela);
                break;
        }

        if(tela==1)//Condicionar que vale se o jogo tiver começado.
        {
            if(kbhit())
            {
                entities[0].dir = getch();

                leverSwitch(entities[0], mapC, map);

                //Muda a posição do jogador se pressionar WASD.
                playerMov(&entities[0], mapC, map);

                coins(entities[0].x, entities[0].y, coinsN, &pointsN, mapC);

                keys(entities[0].x, entities[0].y, &keysN, mapC);

                ingameMenu(entities[0].dir, &tela, &selected);

                ingameCommands(entities, &level, entities[0].dir, coinsN, &pointsN, &levelN, &keysN, mapSize, map, mapC, &tela);

                nextLevel(entities, &level, &keysN, &levelN, map, mapC, &tela, coinsN, pointsN);
            }

            invulnerable(&entities[0], timer, coinsN, level.coins);

            death(entities, level, map, mapC, &pointsN, &tela);

            ogreIa(entities, level, map, timer[0]);

            statsDisplay(entities[0].hp, coinsN[2], pointsN, mapSize, entities[0].imortal, levelN);
        }

        timer[0]++;

        Sleep(100);//Taxa de atualização da tela em ms.

    }while(sair==0);

    if(!(save_options = fopen("options.bin", "wb")))
    {
        printf("Não foi possível criar arquivo options.bin\n");
        Sleep(2000);
    }
    else
    {
        if(fwrite(saveData, sizeof(long int), 9*4, save_options)==9*4)
        {
            if(fwrite(&mapSize, sizeof(long int), 1, save_options)==1);
            else
            {
                printf("Erro em salvar mapSize em options.bin\n");
                Sleep(2000);
            }

        }
        else
        {
            printf("Erro em salvar saveData em options.bin\n");
            Sleep(2000);
        }
    }
    fclose(save_options);

    return 0;
}
void cursorState(int cursor)
{
   HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
   CONSOLE_CURSOR_INFO info;
   info.dwSize = 1;
   info.bVisible = cursor;
   SetConsoleCursorInfo(consoleHandle, &info);
}
void gotoxy(int x, int y)
{
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),(COORD){x,y});
}
void menuIn(int *tela, int *selected)
{
    int i, n, option=1;
    char dir;
    char text[7][80]={{"A Espada Perdida de Midas"},{"Começar"},{"Carregar"},{"Níveis"},{"Opções"},{"Sair"},("Versão: 1.0.0")};

    for(i=0; i<6; i++)
    {
        n = strlen(text[i]);
        gotoxy((xM-n)/2,(yM/6*(i+1)));
        printf("%s", text[i]);
    }
    gotoxy(xM-15, yM+3);
    printf("%s", text[6]);

    gotoxy((xM+strlen(text[1]))/2,(yM/6*2));
    do
    {
        if(kbhit())
        {
            dir = getch();

            if(dir=='W'||dir=='w')
            {
                if(option!=1)
                {
                    gotoxy((xM+strlen(text[option-1]))/2,(yM/6*option));
                    option-=1;
                }
                else
                {
                    gotoxy((xM+strlen(text[5]))/2,(yM/6*6));
                    option=5;
                }
            }
            else if(dir=='S'||dir=='s')
            {
                if(option!=5)
                {
                    gotoxy((xM+strlen(text[option+1]))/2,(yM/6*(option+2)));
                    option+=1;
                }
                else
                {
                    gotoxy((xM+strlen(text[1]))/2,(yM/6*2));
                    option=1;
                }
            }
            else if(dir=='\r') *tela = option;
        }
    }while(*tela==0);

    if(*tela==3) *selected=0;

    system("cls");
}
void mapDraw(ENTITY entities[], LEVEL level, char map[][COLM], char mapC[][COLM], int mapSize)
{
    int i, z, j, f=0,countX, countY;

    for(i=0; i<LINM; i++)
    {
        for(z=0; z<COLM; z++)
        {
            gotoxy(z*mapSize,i*mapSize);
            for(countY=0; countY<mapSize; countY++)
            {
                for(countX=0; countX<mapSize; countX++)
                {
                    if(map[i][z]=='J'||map[i][z]=='O')
                    {
                        f=0;
                        for(j=0; j<=level.entitiesN; j++) if(entities[j].x==z&&entities[j].y==i)  f=1;

                        if((f==0)&&(mapC[i][z]!='D'&&mapC[i][z]!='A'&&mapC[i][z]!='M'&&mapC[i][z]!='B'&&mapC[i][z]!='C')) map[i][z]=' ';
                        else if(f==0) map[i][z] = mapC[i][z];
                    }
                    printf("%c", map[i][z]);
                }
                gotoxy(z*mapSize,i*mapSize+countY+1);
            }
        }
    }
}
void leverSwitch(ENTITY player, char mapC[][COLM], char map[][COLM])
{
    int i, z;
    if((player.dir=='B'||player.dir=='b')&&(mapC[player.y][player.x]=='B'||mapC[player.y+1][player.x]=='B'||mapC[player.y-1][player.x]=='B'||mapC[player.y][player.x+1]=='B'||mapC[player.y][player.x-1]=='B'||mapC[player.y+1][player.x+1]=='B'||mapC[player.y+1][player.x-1]=='B'||mapC[player.y-1][player.x+1]=='B'||mapC[player.y-1][player.x-1]=='B'))
    {
        for(i=0;i<LINM;i++)
        {
            for(z=0;z<COLM;z++)
            {
                if(mapC[i][z]=='A')
                {
                    mapC[i][z]='D';
                    map[i][z]='D';
                }
                else if(mapC[i][z]=='D')
                {
                    mapC[i][z]='A';
                    map[i][z]='A';
                }
            }
        }
    }
}
void playerMov(ENTITY *player, char mapC[][COLM], char map[][COLM])
{
    if((player->dir=='A'||player->dir=='a')&&map[player->y][player->x-1]!='#'&& map[player->y][player->x-1]!='D')
    {
        map[player->y][player->x-1]= 'J';
        player->x-=1;
    }
    else if((player->dir=='D'||player->dir=='d')&&map[player->y][player->x+1]!='#'&& map[player->y][player->x+1]!='D')
    {
        map[player->y][player->x+1]= 'J';
        player->x+=1;
    }
    else if((player->dir=='W'||player->dir=='w')&&map[player->y-1][player->x]!='#'&& map[player->y-1][player->x]!='D')
    {
        map[player->y-1][player->x]= 'J';
        player->y-=1;
    }
    else if((player->dir=='S'||player->dir=='s')&&map[player->y+1][player->x]!='#'&& map[player->y+1][player->x]!='D')
    {
        map[player->y+1][player->x]= 'J';
        player->y+=1;
    }
}
void ogreIa(ENTITY *entities, LEVEL level,char map[][COLM],int timer)
{
    int i, z=0, oX, oY;

    for(i=level.playerN; i<level.ogreN+level.playerN; i++)
    {
        oX=(entities+i)->x;
        oY=(entities+i)->y;

        while(((entities+i)->dir==0)||((entities+i)->dir==z||((map[oY-1][oX]=='#'||map[oY-1][oX]=='D')&&(entities+i)->dir==1)||((map[oY][oX+1]=='#'||map[oY][oX+1]=='D')&&(entities+i)->dir==2)||((map[oY+1][oX]=='#'||map[oY+1][oX]=='D')&&(entities+i)->dir==3)||((map[oY][oX-1]=='#'||map[oY][oX-1]=='D')&&(entities+i)->dir==4)))
        {
            z=(entities+i)->dir;
            (entities+i)->dir=(rand()%4)+1;
        }
        if((entities+i)->hp!=0&&(((entities+i)->dir==1&&timer%6==0)&&(map[oY-1][oX]!='#'&&map[oY-1][oX]!='D')))
        {
            map[oY-1][oX]= 'O';

            (entities+i)->y-=1;
        }
        else if((entities+i)->hp!=0&&(((entities+i)->dir==2&&timer%6==0)&&(map[oY][oX+1]!='#'&&map[oY][oX+1]!='D')))
        {
            map[oY][oX+1]= 'O';

            (entities+i)->x+=1;
        }
        else if((entities+i)->hp!=0&&(((entities+i)->dir==3&&timer%6==0)&&(map[oY+1][oX]!='#'&&map[oY+1][oX]!='D')))
        {
            map[oY+1][oX]= 'O';

            (entities+i)->y+=1;
        }
        else if((entities+i)->hp!=0&&(((entities+i)->dir==4&&timer%6==0)&&(map[oY][oX-1]!='#'&&map[oY][oX-1]!='D')))
        {
            map[oY][oX-1]= 'O';

            (entities+i)->x-=1;
        }
    }
}
void statsDisplay(int hpN, long int coinsN, long int pointsN, int mapSize, int imortal, int levelN)
{
    gotoxy(COLM*mapSize+3,mapSize);
     printf("Level: %d   ", levelN);
    gotoxy(COLM*mapSize+3,mapSize+2);
    printf("Vidas: %d   ", hpN);
    gotoxy(COLM*mapSize+3,mapSize+4);
    printf("Moedas: %ld  ", coinsN);
    gotoxy(COLM*mapSize+3,mapSize+6);
    printf("Pontos: %ld  ", pointsN);
    if(imortal>=1)
    {
        gotoxy(COLM*mapSize+3,mapSize+8);
        printf("*Você está imortal*");
    }
    else
    {
        gotoxy(COLM*mapSize+3,mapSize+10);
        printf("                      ");
    }
}
void coins(int pX, int pY, long int coinsN[], long int *pointsN, char mapC[][COLM])
{
    if(mapC[pY][pX]=='M')//Contador de moedas coletadas
    {
        mapC[pY][pX]=' ';
        coinsN[0]+=1;
        coinsN[1]+=1;
        coinsN[2]+=1;
        *pointsN+=10;
    }
}
void keys(int pX, int pY, int *keysN, char mapC[][COLM])
{
    if(mapC[pY][pX]=='C')//Contador de moedas coletadas
    {
        mapC[pY][pX]=' ';
        *keysN+=1;
    }
}
void invulnerable(ENTITY *player, int timer[], long int coinsN[], int totalMapC)
{
    if(coinsN[1]==10&&player->imortal!=2)
    {
        timer[1] = timer[0];
        player->imortal=1;
        coinsN[1]-=10;
    }
    if((timer[0]-timer[1])==25&&player->imortal!=2)
    {
        player->imortal=0;
    }
    if(coinsN[0]==totalMapC&&player->imortal!=2) player->imortal=1;
}
void ingameMenu(char dir, int *tela, int *selected)
{
    int i, n, option=1;
    char text[10][20]={"Continuar","Salvar","Carregar","Opções","Sair"};
    char keypress;

    *selected=0;
    if(dir=='\t')
    {
        for(i=0; i<3; i++)
        {
            gotoxy(0,i);
            printf("\33[2K");
            Sleep(400);
        }
        for(i=0; i<5; i++)
        {
            n=strlen(text[i]);
            gotoxy(((xM-n)/6)*(i+1),1);
            printf("%s", text[i]);
        }
        cursorState(1);
        gotoxy((((xM-strlen(text[0]))/6)*1)+strlen(text[0]),1);
        do
        {
            if(kbhit())
            {
                keypress = getch();

                if(keypress=='A'||keypress=='a')
                {
                    if(option!=1)
                    {
                        option--;
                        gotoxy((((xM-strlen(text[option-1]))/6)*option)+strlen(text[option-1]),1);
                    }
                    else
                    {
                        option=5;
                        gotoxy((((xM-strlen(text[option-1]))/6)*option)+strlen(text[option-1]),1);
                    }
                }
                else if(keypress=='D'||keypress=='d')
                {
                    if(option!=5)
                    {
                        option++;
                        gotoxy((((xM-strlen(text[option-1]))/6)*option)+strlen(text[option-1]),1);
                    }
                    else
                    {
                        option=1;
                        gotoxy((((xM-strlen(text[option-1]))/6)*option)+strlen(text[option-1]),1);
                    }
                }
                else if(keypress=='\r')
                {
                    *selected=option;
                    gotoxy(0,1);
                    printf("\33[2K");
                }
            }
        }while(*selected==0);

        if(*selected==1) *tela=1;
        else if(*selected==2) *tela=2;
        else if(*selected==3) *tela=2;
        else if(*selected==4) *tela=4;
        else if(*selected==5) *tela=0;
    }
}
void ingameCommands(ENTITY entities[], LEVEL *level, char dir, long int coinsN[], long int *pointsN, int *levelN, int *keysN, int mapSize, char map[][COLM], char mapC[][COLM], int *tela)
{
    char command[20], type, coord1, coord2;
    int x, y, n, countEntity;
    long int value;
    int i, z, j, countX, countY;
    int g, f;//flags
    char coord[COLM]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q'};

    if(dir==27)
    {
        system("cls");
        do
        {
            gotoxy(1*mapSize,0);
            for(i=0; i<COLM; i++)
            {
                printf("%c", coord[i]);
                for(z=1; z<mapSize;z++) printf(" ");
            }
            printf("\n");
            for(i=0; i<LINM; i++)
            {
                gotoxy(0,(i+1)*mapSize);
                printf("%c", coord[i]);
                for(z=0; z<COLM; z++)
                {
                    gotoxy((z+1)*mapSize,(i+1)*mapSize);
                    for(countY=0; countY<mapSize; countY++)
                    {
                        for(countX=0; countX<mapSize; countX++)
                        {
                            g=0;
                            for(countEntity=0; countEntity<level->entitiesN; countEntity++)
                            {
                                if(z==entities[countEntity].x&&i==entities[countEntity].y)
                                {
                                    if(countEntity<level->playerN) printf("%d", countEntity+1);
                                    else printf("%d", countEntity+1-level->playerN);
                                    g=1;
                                }
                            }
                            if(map[i][z]=='J'||map[i][z]=='O')
                            {
                                f=0;
                                for(j=0; j<=level->entitiesN; j++)
                                if(entities[j].x==z&&entities[j].y==i)
                                {
                                    f=1;

                                }

                                if((f==0)&&(mapC[i][z]!='D'&&mapC[i][z]!='A'&&mapC[i][z]!='M'&&mapC[i][z]!='B'&&mapC[i][z]!='C')) map[i][z]=' ';
                                else if(f==0) map[i][z] = mapC[i][z];
                            }
                            if(g==0) printf("%c", map[i][z]);
                        }
                        gotoxy((z+1)*mapSize,(i+1)*mapSize+countY+1);
                    }
                }

            }
            cursorState(1);
            gotoxy(2,((LINM+2)*mapSize));
            printf("Console: ");
            scanf("%s", command);

            if(!strcmp(command, "ogro.hp"))
            {
                scanf("%d%d", &n, &value);
                entities[level->playerN+n-1].hp=value;
            }
            else if(!strcmp(command, "ogro.pos"))
            {
                scanf("%d %c %c", &n, &coord1, &coord2);
                for(i=0; i<COLM; i++)
                {
                    if(coord1==coord[i]) x=i;
                    if(coord2==coord[i]) y=i;
                }
                entities[level->playerN+n-1].x=x;
                entities[level->playerN+n-1].y=y;
                map[y][x]='O';
            }
            else if(!strcmp(command, "ogro.imortal"))
            {
                scanf("%d%d", &n, &value);
                entities[level->playerN+n-1].imortal=value;
            }
            else if(!strcmp(command, "player.hp"))
            {
                scanf("%d%d", &n, &value);
                entities[n-1].hp=value;
            }
            else if(!strcmp(command, "player.pos"))
            {
                scanf("%d %c %c", &n, &coord1, &coord2);
                for(i=0; i<COLM; i++)
                {
                    if(coord1==coord[i]) x=i;
                    if(coord2==coord[i]) y=i;
                }
                entities[n-1].x=x;
                entities[n-1].y=y;
                map[y][x]='J';
            }
            else if(!strcmp(command, "player.imortal"))
            {
                scanf("%d%d", &n, &value);
                entities[n-1].imortal=value;
            }
            else if(!strcmp(command, "sou"))
            {
                scanf("%s", command);
                if(!strcmp(command, "covarde")) entities[0].imortal=2;
            }
            else if(!strcmp(command, "coins"))
            {
                scanf("%ld", &value);
                coinsN[2]=value;
            }
            else if(!strcmp(command, "points"))
            {
                scanf("%ld", &value);
                *pointsN=value;
            }
            else if(!strcmp(command, "level"))
            {
                scanf("%d", &value);
                *levelN=value;
                loadLevel(entities, level, *levelN, map, mapC, tela);
            }
            else if(!strcmp(command, "keys"))
            {
                scanf("%d", &value);
                *keysN=value;
            }
            else if(!strcmp(command, "create"))
            {
                scanf(" %c %c %c", &type, &coord1, &coord2);
                for(i=0; i<COLM; i++)
                {
                    if(coord1==coord[i]) x=i;
                    if(coord2==coord[i]) y=i;
                }
                if(type!='O'&&type!='J')
                {
                    if(mapC[y][x]=='M') level->coins--;
                    else if(mapC[y][x]=='C') level->keys--;
                    if(mapC[y][x]!='M'&&type=='M') level->coins++;
                    else if(mapC[y][x]!='C'&&type=='C') level->keys++;
                    mapC[y][x]=type;
                }
                if(type=='O')
                {
                    entities[level->playerN+level->ogreN].x=x;
                    entities[level->playerN+level->ogreN].y=y;
                    entities[level->playerN+level->ogreN].xIn=x;
                    entities[level->playerN+level->ogreN].yIn=y;
                    entities[level->playerN+level->ogreN].hp=100;
                    level->ogreN++;
                    level->entitiesN++;
                }
                map[y][x]=type;
            }
            else if(!strcmp(command, "destroy"))
            {
                scanf(" %c %c", &coord1, &coord2);
                for(i=0; i<COLM; i++)
                {
                    if(coord1==coord[i]) x=i;
                    if(coord2==coord[i]) y=i;
                }
                if(map[y][x]=='O'||map[y][x]=='J')
                {
                    for(i=0;i<level->entitiesN;i++)
                    {
                        if(entities[i].x==x&&entities[i].y==y)
                        {
                            entities[i].hp=0;
                            entities[i].x=-1;
                            entities[i].y=-1;
                        }
                    }
                }
                else
                {
                    if(mapC[y][x]=='M') level->coins--;
                    else if(mapC[y][x]=='C') level->keys--;
                    mapC[y][x]=' ';
                    map[y][x]=' ';
                }
            }
            cursorState(0);
            system("cls");

        }while(strcmp(command, "exit"));
    }
}
void death(ENTITY entities[], LEVEL level, char map[][COLM], char mapC[][COLM], long int *pointsN, int *tela)
{
    int i, z,enemyX, enemyY;

    for(i=1; i<=level.entitiesN; i++)
    {
        if((entities[0].x==entities[i].x)&&(entities[0].y==entities[i].y))
        {
            enemyX=entities[i].x;
            enemyY=entities[i].y;
            if(entities[0].imortal==0)
            {
                entities[0].hp-=1;
                for(z=0; z<level.entitiesN; z++)
                {
                    if(z<level.playerN) map[entities[z].yIn][entities[z].xIn]='J';
                    else if(z<level.playerN+level.ogreN) map[entities[z].yIn][entities[z].xIn]='O';
                    if(entities[z].hp!=0)
                    {
                        entities[z].x=entities[z].xIn;
                        entities[z].y=entities[z].yIn;
                    }
                }
            }
            else
            {
                map[enemyY][enemyX]='J';
                entities[i].x=-1;
                entities[i].y=-1;
                entities[i].hp=0;
                *pointsN+=100;
            }
            if(entities[0].hp<=0) *tela=6;
        }
    }
}
void gameOver(int *tela)
{
    int n;
    char text[50]={"Você Perdeu!"};

    n = strlen(text);
    gotoxy((xM-n)/2,yM/2);
    printf("%s", text);

    Sleep(1200);

    *tela=0;
}
int pointsRank(long int pointsN, char nome[])
{
    char nomef[10][100]={0};
    int pointsf[10]={0}, i, z=-1, j=0;
    FILE *rank_arq;

    if(!(rank_arq = fopen("ranking.txt", "r+")))
    {
        rank_arq = fopen("ranking.txt", "w+");
    }
    while(!feof(rank_arq))
    {
        fscanf(rank_arq, "Rank %dº: %s \nPontos:%ld \n", &i, &nomef[j][0] , &pointsf[j]);
        j++;
    }
    for(i=0;i<10&&z==-1;i++)
    {
        if(pointsf[i]<pointsN)
        {
            for(z=9;z>i;z--)
            {
                pointsf[z]=pointsf[z-1];
                strcpy(nomef[z],nomef[z-1]);
            }
            pointsf[i]=pointsN;
            strcpy(nomef[i],nome);
        }
    }
    if(z!=-1)
    {
        rewind(rank_arq);
        for(z=0;z<10;z++)
        {
            if(pointsf[z]!=0) fprintf(rank_arq, "Rank %dº: %s \nPontos:%ld \n", z+1, nomef[z], pointsf[z]);
        }
    }
    fclose(rank_arq);

    return i;
}
void save_or_load(ENTITY *entities, LEVEL *level, char map[][COLM], char mapC[][COLM], long int *pointsN, int *levelN, long int coinsN[], int *keysN, int timer[], int *tela, int selected, long int saveData[][9])
{
    int i, z, n, option=1;
    char dir;
    char text[6][80]={{"Saves"},{"Slot 1"},{"Slot 2"},{"Slot 3"},{"Slot 4"},{"Voltar"}};
    char saveText[16]={"save"};
    char saveN[9];
    FILE *save_file;

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    for(i=0; i<6; i++)
    {
        n=strlen(text[i]);
        if(i==0) gotoxy((xM-n)/2, 1);
        else if(i==5) gotoxy((xM-n)/2, yM+2);
        else gotoxy(10, (i*5));
        if(i==0&&selected!=2) printf("Load");
        else printf("%s", text[i]);
    }
    for(i=1; i<=4; i++)
    {
        if(saveData[i-1][0]!=0)
        {
            gotoxy(12+strlen(text[i]), (i*5));
            printf("Data: %02ld-%02ld-%ld %02ld:%02ld:%02ld  ", saveData[i-1][0], saveData[i-1][1], saveData[i-1][2], saveData[i-1][3], saveData[i-1][4], saveData[i-1][5]);
            gotoxy(12+strlen(text[i]), (i*5)+1);
            printf("Level: %ld       ", saveData[i-1][6]);
            gotoxy(12+strlen(text[i]), (i*5)+2);
            printf("Moedas: %ld       ", saveData[i-1][7]);
            gotoxy(12+strlen(text[i]), (i*5)+3);
            printf("Pontos: %ld       ", saveData[i-1][8]);
        }
    }
    gotoxy(10+strlen(text[option]), (option*5));
    do
    {
        if(kbhit())
        {
            dir = getch();

            if(dir=='W'||dir=='w')
            {
                if(option!=1)
                {
                    option-=1;
                    gotoxy(10+strlen(text[option]), (option*5));
                }
                else
                {
                    option=5;
                    gotoxy((xM-(strlen(text[option])))/2, yM+2);
                }
            }
            else if(dir=='S'||dir=='s')
            {
                if(option!=5&&option!=4)
                {
                    option+=1;
                    gotoxy(10+strlen(text[option]), (option*5));
                }
                else if(option==5)
                {
                    option=1;
                    gotoxy(10+strlen(text[option]), (option*5));
                }
                else
                {
                    option+=1;
                    gotoxy((xM-(strlen(text[option])))/2, yM+2);
                }
            }
            else if(dir=='\r')
            {
                if(option==5&&selected!=0) *tela=1;
                else if(option==5) *tela=0;
                else if(selected==2)
                {
                    saveData[option-1][0]=tm.tm_mday;
                    saveData[option-1][1]=tm.tm_mon + 1;
                    saveData[option-1][2]=tm.tm_year + 1900;
                    saveData[option-1][3]=tm.tm_hour;
                    saveData[option-1][4]=tm.tm_min;
                    saveData[option-1][5]=tm.tm_sec;
                    saveData[option-1][6]=*levelN;
                    saveData[option-1][7]=coinsN[2];
                    saveData[option-1][8]=*pointsN;

                    gotoxy(12+strlen(text[option]), (option*5));
                    printf("Data: %02ld-%02ld-%ld %02ld:%02ld:%02ld  ", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
                    gotoxy(12+strlen(text[option]), (option*5)+1);
                    printf("Level: %ld      ", *levelN);
                    gotoxy(12+strlen(text[option]), (option*5)+2);
                    printf("Moedas: %ld      ", coinsN[2]);
                    gotoxy(12+strlen(text[option]), (option*5)+3);
                    printf("Pontos: %ld      ", *pointsN);

                    sprintf(saveN, "0%d", option);
                    strcat(saveText, saveN);
                    strcat(saveText, ".txt");
                    if(!(save_file = fopen(saveText,"w")))
                    {
                        gotoxy(25+strlen(text[option]), (option*5));
                        printf("Ocorreu um erro em salvar\n");
                    }
                    else
                    {
                        fprintf(save_file, "Level=%d\n", *levelN);
                        fprintf(save_file, "Pontos=%ld\n", *pointsN);
                        fprintf(save_file, "Chaves=%d\n", *keysN);
                        fprintf(save_file, "Chaves Totais=%d\n", level->keys);
                        fprintf(save_file, "Moedas Totais=%ld\n", coinsN[2]);
                        fprintf(save_file, "Moedas desse level=%ld\n", coinsN[0]);
                        fprintf(save_file, "Moedas até invul.=%ld\n", coinsN[1]);
                        fprintf(save_file, "Moedas Totais=%d\n", level->coins);
                        fprintf(save_file, "Timer=%d\n", timer[0]);
                        fprintf(save_file, "Timer invul.=%d\n", timer[1]);
                        fprintf(save_file, "Entidades Totais=%d\n", level->entitiesN);
                        fprintf(save_file, "Número de Players=%d\n", level->playerN);
                        fprintf(save_file, "Número de Ogros=%d\n", level->ogreN);

                        for(i=0; i<level->playerN; i++)
                        {
                            fprintf(save_file, "Player %d\n", i+1);
                            fprintf(save_file, "Vidas=%d\n", entities[i].hp);
                            fprintf(save_file, "Imortal=%d\n",  entities[i].imortal);
                            fprintf(save_file, "X=%d\n",  entities[i].x);
                            fprintf(save_file, "Y=%d\n",  entities[i].y);
                            fprintf(save_file, "X inicial=%d\n",  entities[i].xIn);
                            fprintf(save_file, "Y inicial=%d\n",  entities[i].yIn);
                        }
                        for(i=level->playerN; i<((level->ogreN)+(level->playerN)); i++)
                        {
                            fprintf(save_file, "Ogro %d\n", i);
                            fprintf(save_file, "Vidas=%d\n", entities[i].hp);
                            fprintf(save_file, "Imortal=%d\n", entities[i].imortal);
                            fprintf(save_file, "X=%d\n", entities[i].x);
                            fprintf(save_file, "Y=%d\n", entities[i].y);
                            fprintf(save_file, "X inicial=%d\n", entities[i].xIn);
                            fprintf(save_file, "Y inicial=%d\n", entities[i].yIn);
                        }
                        fprintf(save_file,"Mapa\n");
                        for(i=0; i<LINM; i++)
                        {
                            for(z=0; z<COLM; z++)
                            {
                                fprintf(save_file,"%c", mapC[i][z]);
                            }
                            putc('\n', save_file);
                        }
                    }
                    fclose(save_file);
                    strcpy(saveN,"");
                    strcpy(saveText,"save");
                }
                else
                {
                    sprintf(saveN, "0%d", option);
                    strcat(saveText, saveN);
                    strcat(saveText, ".txt");
                    if(!(save_file = fopen(saveText,"r")))
                    {
                        gotoxy(25+strlen(text[option]), (option*5));
                        printf("Não foi possível detectar nenhum arquivo salvo nesse slot.\n");
                    }
                    else
                    {
                        fscanf(save_file, "Level=%d\n", levelN);
                        fscanf(save_file, "Pontos=%ld\n", pointsN);
                        fscanf(save_file, "Chaves=%d\n", keysN);
                        fscanf(save_file, "Chaves Totais=%d\n", &level->keys);
                        fscanf(save_file, "Moedas Totais=%ld\n", &coinsN[2]);
                        fscanf(save_file, "Moedas desse level=%ld\n", &coinsN[0]);
                        fscanf(save_file, "Moedas até invul.=%ld\n", &coinsN[1]);
                        fscanf(save_file, "Moedas Totais=%d\n", &level->coins);
                        fscanf(save_file, "Timer=%d\n", &timer[0]);
                        fscanf(save_file, "Timer invul.=%d\n", &timer[1]);
                        fscanf(save_file, "Entidades Totais=%d\n", &level->entitiesN);
                        fscanf(save_file, "Número de Players=%d\n", &level->playerN);
                        fscanf(save_file, "Número de Ogros=%d\n", &level->ogreN);

                        for(i=0; i<level->playerN; i++)
                        {
                            fscanf(save_file, "Player %d\n", &z);
                            fscanf(save_file, "Vidas=%d\n", &entities[i].hp);
                            fscanf(save_file, "Imortal=%d\n", &entities[i].imortal);
                            fscanf(save_file, "X=%d\n", &entities[i].x);
                            fscanf(save_file, "Y=%d\n", &entities[i].y);
                            fscanf(save_file, "X inicial=%d\n", &entities[i].xIn);
                            fscanf(save_file, "Y inicial=%d\n", &entities[i].yIn);
                        }
                        for(i=level->playerN; i<((level->ogreN)+(level->playerN)); i++)
                        {
                            fscanf(save_file, "Ogro %d\n", &z);
                            fscanf(save_file, "Vidas=%d\n", &entities[i].hp);
                            fscanf(save_file, "Imortal=%d\n", &entities[i].imortal);
                            fscanf(save_file, "X=%d\n", &entities[i].x);
                            fscanf(save_file, "Y=%d\n", &entities[i].y);
                            fscanf(save_file, "X inicial=%d\n", &entities[i].xIn);
                            fscanf(save_file, "Y inicial=%d\n", &entities[i].yIn);
                        }
                        fscanf(save_file,"Mapa\n");
                        for(i=0; i<LINM; i++)
                        {
                            for(z=0; z<COLM; z++)
                            {
                                fscanf(save_file,"%c", &mapC[i][z]);
                                map[i][z]=mapC[i][z];
                            }
                            fscanf(save_file,"\n");
                        }

                        for(i=0; i<(level->entitiesN); i++)
                        {
                            if((i<level->playerN)&&(entities[i].hp!=0)) map[entities[i].y][entities[i].x]='J';
                            else if(entities[i].hp!=0) map[entities[i].y][entities[i].x]='O';
                        }
                        *tela=1;
                    }
                    fclose(save_file);
                    strcpy(saveN,"");
                    strcpy(saveText,"save");
                }
            }
        }
    }while(*tela==2);

    system("cls");

}
int loadLevel(ENTITY *entities, LEVEL *level, int levelN, char map[][COLM], char mapC[][COLM], int *tela)
{
    int x, i, z;
    int success;
    char mapa[14]={"mapa"}, levelnumber[8];
    FILE *mapa_arq;

    if(levelN<10) sprintf(levelnumber, "0%d", levelN);
    else sprintf(levelnumber, "%d", levelN);

    strcat(mapa, levelnumber);
    strcat(mapa, ".txt");

    if(!(mapa_arq = fopen(mapa, "r")))
    {
        if(levelN<=3)
        {
            gotoxy(xM/2,yM/2);
            printf("Erro em carregar mapa.\n");
            Sleep(4000);
            *tela=0;
        }
        success=0;
    }
    else
    {
        level->coins=0;
        level->keys=0;
        level->playerN=0;
        level->ogreN=0;
        level->entitiesN=0;
        for(x=0; x<2; x++)
        {
            for(i=0; i<LINM; i++)
            {
                for(z=0; z<COLM; z++)
                {
                    if(x==0)
                    {
                        map[i][z]=getc(mapa_arq);
                        mapC[i][z]=map[i][z];
                        if(map[i][z]=='M') level->coins++;
                        if(map[i][z]=='C') level->keys++;
                        if(map[i][z]=='J')//Guarda a posição do jogador.
                        {
                            entities[level->playerN].x=z;
                            entities[level->playerN].y=i;
                            entities[level->playerN].xIn=z;
                            entities[level->playerN].yIn=i;
                            entities[level->playerN].hp=3;
                            if(entities[level->playerN].imortal!=2)entities[level->playerN].imortal=0;
                            level->playerN++;
                        }
                    }
                    else if(map[i][z]=='O'&&x==1)//Guarda a posição dos ogros(o sistema foi feito para permitir qualquer número de inimigos desejado).
                    {
                        entities[level->playerN + level->ogreN].x=z;
                        entities[level->playerN + level->ogreN].y=i;
                        entities[level->playerN + level->ogreN].xIn=z;
                        entities[level->playerN + level->ogreN].yIn=i;
                        entities[level->playerN + level->ogreN].dir=0;
                        entities[level->playerN + level->ogreN].hp=100;
                        level->ogreN++;
                    }
                }
                getc(mapa_arq);
            }
        }
        level->entitiesN=level->playerN+level->ogreN;
        fclose(mapa_arq);

        success=1;
    }

    return success;
}
void nextLevel(ENTITY *entities, LEVEL *level, int *keysN, int *levelN, char map[][COLM], char mapC[][COLM], int *tela, long int coinsN[], long int pointsN)
{
    char nome[100];
    int rank;

    if(*keysN>=level->keys)
    {
        *levelN=*levelN+1;
        system("cls");
        gotoxy((xM/2)-10, yM/2);
        if(loadLevel(entities, level, *levelN, map, mapC, tela))
        {
            printf("Você avançou para o nível %d\n", *levelN);

            Sleep(4000);

            system("cls");

            *keysN=0;
            coinsN[0]=0;
            coinsN[1]=0;
        }
        else if(*levelN>3)
        {
            printf("Você ganhou o jogo!");
            Sleep(4000);
            gotoxy((xM/2)-25, yM/2);
            printf("Informe o nome com o qual deseja salvar o score: ");
            cursorState(1);
            scanf("%s", nome);
            cursorState(0);
            gotoxy((xM/2)-25, yM/2);
            rank = pointsRank(pointsN, nome);
            if(rank<=10)
            {
                printf("Você atingiu %dº lugar no ranking de pontuação total              \n", rank);
                Sleep(4000);
            }
            *tela=0;
        }
    }
}
void options(int *tela, int *mapSize, int selected)
{
    int i, n, option=1;
    char dir;
    char text[5][80]={{"Opções"},{"Tamanho do mapa: << Pequeno >>"},{"Voltar"},{"Tamanho do mapa: << Médio >>"},{"Tamanho do mapa: << Grande >>"}};

    for(i=0; i<3; i++)
    {
        n = strlen(text[i]);
        gotoxy((xM-n)/2,(yM/3*(i+1)));
        printf("%s ", text[i]);
    }
    if(*mapSize!=1)
    {
        gotoxy((xM-strlen(text[1]))/2,(yM/3*2));
        printf("%s  ", text[*mapSize+1]);
    }
    gotoxy((xM+strlen(text[1]))/2,(yM/3*2));
    do
    {
        if(kbhit())
        {
            dir = getch();

            if(dir=='W'||dir=='w')
            {
                if(option!=1)
                {
                    gotoxy((xM+strlen(text[option-1]))/2,(yM/3*option));
                    option-=1;
                }
                else
                {
                    gotoxy((xM+strlen(text[2]))/2,(yM/3*3));
                    option=2;
                }
            }
            else if(dir=='S'||dir=='s')
            {
                if(option!=2)
                {
                    gotoxy((xM+strlen(text[option+1]))/2,(yM/3*(option+2)));
                    option+=1;
                }
                else
                {
                    gotoxy((xM+strlen(text[1]))/2,(yM/3*2));
                    option=1;
                }
            }
            else if(dir=='A'||dir=='a')
            {
                if(*mapSize==1)
                {
                    gotoxy((xM-strlen(text[1]))/2,(yM/3*2));
                    printf("%s ", text[4]);

                    *mapSize=3;
                }
                else if(*mapSize==2)
                {
                    gotoxy((xM-strlen(text[3])-1)/2,(yM/3*2));
                    printf("%s ", text[1]);

                    *mapSize=1;
                }
                else if(*mapSize==3)
                {
                    gotoxy((xM-strlen(text[4]))/2,(yM/3*2));
                    printf("%s ", text[3]);

                    *mapSize=2;
                }
            }
            else if(dir=='D'||dir=='d')
            {
                if(*mapSize==1)
                {
                    gotoxy((xM-strlen(text[1]))/2,(yM/3*2));
                    printf("%s  ", text[3]);

                    *mapSize=2;
                }
                else if(*mapSize==2)
                {
                    gotoxy((xM-strlen(text[3])-1)/2,(yM/3*2));
                    printf("%s ", text[4]);

                    *mapSize=3;
                }
                else if(*mapSize==3)
                {
                    gotoxy((xM-strlen(text[4]))/2,(yM/3*2));
                    printf("%s ", text[1]);

                    *mapSize=1;
                }
            }
            else if(dir=='\r'&&option==2) *tela = 0;
        }

    }while(*tela==4);

    if(selected==4)
    {
        *tela=1;
        system("cls");
    }
}
