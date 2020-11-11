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
void mapDraw(ENTITY entities[], LEVEL level,char map[][COLM],char mapC[][COLM], int mapSize);//Desenha na tela o char que tiver sido alterado em relação ao frame anterior
void leverSwitch(ENTITY player, char mapC[][COLM], char map[][COLM]);//Função que troca o estado das portas se uma alavanca é acionada.
void playerMov(ENTITY *player, char mapC[][COLM], char map[][COLM]);
void ogreIa(ENTITY *entities, LEVEL level, char map[][COLM], int timer);
void statsDisplay(int hpN, int coinsN, int pointsN, int mapSize, int imortal);
void coins(int pX, int pY, int coinsN[], int *pointsN, char mapC[][COLM]);
void keys(int pX, int pY, int *keysN, char mapC[][COLM]);
void invulnerable(int timer[], int *imortal, int coinsN[], int totalMapC);
void ingameMenu(char dir, int *tela, int *selected);
void death(ENTITY *entities, LEVEL level, char map[][COLM], char mapC[][COLM], int *pointsN, int *tela);
void gameOver(int *tela);
int pointsScore();
void save(ENTITY *entities, LEVEL *level, char map[][COLM], char mapC[][COLM], int *pointsN, int *levelN, int coinsN[], int *keysN, int timer[], int *tela, int selected, int saveData[][9]);
void loadLevel(ENTITY *entities, LEVEL *level, int levelN, char map[][COLM], char mapC[][COLM], int *tela);
int nextLevel(ENTITY *entities, LEVEL *level, int *keysN, int *levelN, char map[][COLM], char mapC[][COLM], int *tela);
void options(int *tela, int *mapSize, int selected);//Abre a tela de opções interativas.

int main()
{
    ENTITY entities[LINM*COLM];
    LEVEL level;
    int i, z, x, //Contadores.
    tela=0, //Nessa variável é guardado qual a tela atual do jogo.
    sair=0, //Flag indicando que o jogo deve terminar.
    mapSize=1; //Indica o tamanho do mapa em unidades: 1x1, 2x2, 3x3...
    int coinsN[2]={0},//Guarda o número de moedas coletadas.
    keysN=0,
    pointsN=0,//Guarda a quantidade de pontos adquiridos.
    saveData[3][9]={0},
    selected=0,
    levelN=1,
    timer[2]={0};

    char mapC[LINM][COLM];//Mapa e cópia do mapa utilizada para só alterar na tela aquilo que mudar de um frame para o outro.
    char map[LINM][COLM];
    FILE *save_options;

    setlocale(LC_ALL,"Portuguese");

    srand(time(NULL));

    if(!(save_options = fopen("options.bin", "rb")))
    {
        //printf("Arquivo options.bin não encontrado.\n");
        //Arquivo não encontrado ou é o primeiro Load.
    }
    else
    {
        if(fread(saveData, sizeof(int), 9*3, save_options)==9*3)
        {
            if(fread(&mapSize, sizeof(int), 1, save_options)==1);
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
                    save(entities, &level, map, mapC, &pointsN, &levelN, coinsN, &keysN, timer, &tela, selected, saveData);
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
            case 7: system("cls");
                    //save(entities, &level, map, mapC, &pointsN, &levelN, coinsN, &keysN, timer, &tela, selected, saveData);
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

                nextLevel(entities, &level, &keysN, &levelN, map, mapC, &tela);

                ingameMenu(entities[0].dir, &tela, &selected);
            }

            invulnerable(timer, &entities[0].imortal, coinsN, level.coins);

            death(entities, level, map, mapC, &pointsN, &tela);

            ogreIa(entities, level, map, timer[0]);

            statsDisplay(entities[0].hp, coinsN[0], pointsN, mapSize, entities[0].imortal);
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
        if(fwrite(saveData, sizeof(int), 9*3, save_options)==9*3)
        {
            if(fwrite(&mapSize, sizeof(int), 1, save_options)==1);
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
    char text[7][80]={{"A Espada Perdida de Midas"},{"Começar"},{"Carregar"},{"Níveis"},{"Opções"},{"Sair"},("Versão: 0.9.0")};

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
void statsDisplay(int hpN, int coinsN, int pointsN, int mapSize, int imortal)
{
    gotoxy(COLM*mapSize+3,mapSize);
    printf("Vidas: %d   ", hpN);
    gotoxy(COLM*mapSize+3,mapSize+2);
    printf("Moedas: %d  ", coinsN);
    gotoxy(COLM*mapSize+3,mapSize+4);
    printf("Pontos: %d  ", pointsN);
    if(imortal==1)
    {
        gotoxy(COLM*mapSize+3,mapSize+6);
        printf("*Você está imortal*");
    }
    else
    {
        gotoxy(COLM*mapSize+3,mapSize+6);
        printf("                      ");
    }
}
void coins(int pX, int pY, int coinsN[], int *pointsN, char mapC[][COLM])
{
    if(mapC[pY][pX]=='M')//Contador de moedas coletadas
    {
        mapC[pY][pX]=' ';
        coinsN[0]+=1;
        coinsN[1]+=1;
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
void invulnerable(int timer[], int *imortal, int coinsN[], int totalMapC)
{
    if(coinsN[1]==10)
    {
        timer[1] = timer[0];
        *imortal=1;
        coinsN[1]-=10;
    }
    if((timer[0]-timer[1])==25)
    {
        *imortal=0;
    }
    if(coinsN[0]==totalMapC) *imortal=1;
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
void death(ENTITY *entities, LEVEL level, char map[][COLM], char mapC[][COLM], int *pointsN, int *tela)
{
    int i, z,enemyX, enemyY;

    for(i=1; i<=level.entitiesN; i++)
    {
        if(((entities+0)->x==(entities+i)->x)&&((entities+0)->y==(entities+i)->y))
        {
            enemyX=(entities+i)->x;
            enemyY=(entities+i)->y;
            if((entities+0)->imortal==0)
            {
                (entities+0)->hp-=1;
                for(z=0; z<level.entitiesN; z++)
                {
                    if(z<level.playerN) map[(entities+z)->yIn][(entities+z)->xIn]='J';
                    else if(z<level.playerN+level.ogreN) map[(entities+z)->yIn][(entities+z)->xIn]='O';
                    if((entities+z)->hp!=0)
                    {
                        (entities+z)->x=(entities+z)->xIn;
                        (entities+z)->y=(entities+z)->yIn;
                    }
                }
            }
            else
            {
                map[enemyY][enemyX]='J';
                (entities+i)->x=0;
                (entities+i)->y=0;
                (entities+i)->hp=0;
                *pointsN+=100;
            }
            if((entities+0)->hp==0) *tela=6;
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
int pointsScore()
{

}
void save(ENTITY *entities, LEVEL *level, char map[][COLM], char mapC[][COLM], int *pointsN, int *levelN, int coinsN[], int *keysN, int timer[], int *tela, int selected, int saveData[][9])
{
    int i, z, n, option=1;
    char dir;
    char text[6][80]={{"Saves"},{"Slot 1"},{"Slot 2"},{"Slot 3"},{"Slot 4"},{"Voltar"}};
    char saveText[12]={"save"};
    char saveN[6];
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
    for(i=1; i<=3; i++)
    {
        if(saveData[i-1][0]!=0)
        {
            gotoxy(12+strlen(text[i]), (i*5));
            printf("Data: %02d-%02d-%d %02d:%02d:%02d  ", saveData[i-1][0], saveData[i-1][1], saveData[i-1][2], saveData[i-1][3], saveData[i-1][4], saveData[i-1][5]);
            gotoxy(12+strlen(text[i]), (i*5)+1);
            printf("Level: %d       ", saveData[i-1][6]);
            gotoxy(12+strlen(text[i]), (i*5)+2);
            printf("Moedas: %d       ", saveData[i-1][7]);
            gotoxy(12+strlen(text[i]), (i*5)+3);
            printf("Pontos: %d       ", saveData[i-1][8]);
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
                    saveData[option-1][7]=coinsN[0];
                    saveData[option-1][8]=*pointsN;

                    gotoxy(12+strlen(text[option]), (option*5));
                    printf("Data: %02d-%02d-%d %02d:%02d:%02d  ", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
                    gotoxy(12+strlen(text[option]), (option*5)+1);
                    printf("Level: %d      ", *levelN);
                    gotoxy(12+strlen(text[option]), (option*5)+2);
                    printf("Moedas: %d      ", coinsN[0]);
                    gotoxy(12+strlen(text[option]), (option*5)+3);
                    printf("Pontos: %d      ", *pointsN);

                    sprintf(saveN, "0%d", option);
                    strcat(saveText, saveN);
                    strcat(saveText, ".txt");
                    if(!(save_file = fopen(saveText,"w")))
                    {
                        gotoxy(60+strlen(text[option]), (option*5));
                        printf("Ocorreu um erro em salvar\n");
                    }
                    else
                    {
                        fprintf(save_file, "Level=%d\n", *levelN);
                        fprintf(save_file, "Pontos=%d\n", *pointsN);
                        fprintf(save_file, "Chaves=%d\n", *keysN);
                        fprintf(save_file, "Chaves Totais=%d\n", level->keys);
                        fprintf(save_file, "Moedas=%d\n", coinsN[0]);
                        fprintf(save_file, "Moedas até invul.=%d\n", coinsN[1]);
                        fprintf(save_file, "Moedas Totais=%d\n", level->coins);
                        fprintf(save_file, "Timer=%d\n", timer[0]);
                        fprintf(save_file, "Timer invul.=%d\n", timer[1]);
                        fprintf(save_file, "Entidades Totais=%d\n", level->entitiesN);
                        fprintf(save_file, "Entidades Totais=%d\n", level->playerN);
                        fprintf(save_file, "Entidades Totais=%d\n", level->ogreN);

                        for(i=0; i<level->playerN; i++)
                        {
                            fprintf(save_file, "Player %d\n", i+1);
                            fprintf(save_file, "Vidas=%d\n", (entities+i)->hp);
                            fprintf(save_file, "Imortal=%d\n", (entities+i)->imortal);
                            fprintf(save_file, "X=%d\n", (entities+i)->x);
                            fprintf(save_file, "Y=%d\n", (entities+i)->y);
                            fprintf(save_file, "X inicial=%d\n", (entities+i)->xIn);
                            fprintf(save_file, "Y inicial=%d\n", (entities+i)->yIn);
                        }
                        for(i=level->playerN; i<level->ogreN+level->playerN; i++)
                        {
                            fprintf(save_file, "Ogro %d\n", i+1-level->playerN);
                            fprintf(save_file, "Vidas=%d\n", (entities+i)->hp);
                            fprintf(save_file, "Imortal=%d\n", (entities+i)->imortal);
                            fprintf(save_file, "X=%d\n", (entities+i)->x);
                            fprintf(save_file, "Y=%d\n", (entities+i)->y);
                            fprintf(save_file, "X inicial=%d\n", (entities+i)->xIn);
                            fprintf(save_file, "Y inicial=%d\n", (entities+i)->yIn);
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
                        fclose(save_file);
                    }
                }
                else
                {
                    sprintf(saveN, "0%d", option);
                    strcat(saveText, saveN);
                    strcat(saveText, ".txt");
                    if(!(save_file = fopen(saveText,"r")))
                    {
                        gotoxy(30+strlen(text[option]), (option*5));
                        printf("Não foi possível detectar nenhum arquivo salvo nesse slot.\n");
                    }
                    else
                    {
                        fscanf(save_file, "Level=%d\n", levelN);
                        fscanf(save_file, "Pontos=%d\n", pointsN);
                        fscanf(save_file, "Chaves=%d\n", keysN);
                        fscanf(save_file, "Chaves Totais=%d\n", &level->keys);
                        fscanf(save_file, "Moedas=%d\n", &coinsN[0]);
                        fscanf(save_file, "Moedas até invul.=%d\n", &coinsN[1]);
                        fscanf(save_file, "Moedas Totais=%d\n", &level->coins);
                        fscanf(save_file, "Timer=%d\n", &timer[0]);
                        fscanf(save_file, "Timer invul.=%d\n", &timer[1]);
                        fscanf(save_file, "Entidades Totais=%d\n", &level->entitiesN);
                        fscanf(save_file, "Entidades Totais=%d\n", &level->playerN);
                        fscanf(save_file, "Entidades Totais=%d\n", &level->ogreN);

                        for(i=0; i<level->playerN; i++)
                        {
                            fscanf(save_file, "Player %d\n", &z);
                            fscanf(save_file, "Vidas=%d\n", &(entities+i)->hp);
                            fscanf(save_file, "Imortal=%d\n", &(entities+i)->imortal);
                            fscanf(save_file, "X=%d\n", &(entities+i)->x);
                            fscanf(save_file, "Y=%d\n", &(entities+i)->y);
                            fscanf(save_file, "X inicial=%d\n", &(entities+i)->xIn);
                            fscanf(save_file, "Y inicial=%d\n", &(entities+i)->yIn);
                        }
                        for(i=level->playerN; i<level->ogreN+level->playerN; i++)
                        {
                            fscanf(save_file, "Ogro %d\n", &z);
                            fscanf(save_file, "Vidas=%d\n", &(entities+i)->hp);
                            fscanf(save_file, "Imortal=%d\n", &(entities+i)->imortal);
                            fscanf(save_file, "X=%d\n", &(entities+i)->x);
                            fscanf(save_file, "Y=%d\n", &(entities+i)->y);
                            fscanf(save_file, "X inicial=%d\n", &(entities+i)->xIn);
                            fscanf(save_file, "Y inicial=%d\n", &(entities+i)->yIn);
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
                        fclose(save_file);

                        for(i=0; i<level->entitiesN; i++)
                        {
                            if((i<level->playerN)&&((entities+i)->hp!=0)) map[(entities+i)->y][(entities+i)->x]='J';
                            else if((entities+i)->hp!=0) map[(entities+i)->y][(entities+i)->x]='O';
                        }

                        *tela=1;
                    }
                }
            }
        }
    }while(*tela==2);

    system("cls");

}
void loadLevel(ENTITY *entities, LEVEL *level, int levelN, char map[][COLM], char mapC[][COLM], int *tela)
{
    int x, i, z;
    char mapa[12]={"mapa"}, levelnumber[4];
    FILE *mapa_arq;

    if(levelN<10) sprintf(levelnumber, "0%d", levelN);
    else sprintf(levelnumber, "%d", levelN);

    strcat(mapa, levelnumber);
    strcat(mapa, ".txt");

    if(!(mapa_arq = fopen(mapa, "r")))
    {
        gotoxy(xM/2,yM/2);
        printf("Erro em carregar mapa.\n");
        Sleep(4000);
        *tela=0;
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
                        if(map[i][z]=='M') level->coins++;
                        if(map[i][z]=='C') level->keys++;
                        mapC[i][z]=map[i][z];
                        if(map[i][z]=='J')//Guarda a posição do jogador.
                        {
                            (entities+(level->playerN))->x=z;
                            (entities+(level->playerN))->y=i;
                            (entities+(level->playerN))->xIn=z;
                            (entities+(level->playerN))->yIn=i;
                            (entities+(level->playerN))->hp=3;
                            level->playerN++;
                        }
                    }
                    else if(map[i][z]=='O'&&x==1)//Guarda a posição dos ogros(o sistema foi feito para permitir qualquer número de inimigos desejado).
                    {
                        (entities+(level->playerN + level->ogreN))->x=z;
                        (entities+(level->playerN + level->ogreN))->y=i;
                        (entities+(level->playerN + level->ogreN))->xIn=z;
                        (entities+(level->playerN + level->ogreN))->yIn=i;
                        (entities+(level->playerN + level->ogreN))->dir=0;
                        (entities+(level->playerN + level->ogreN))->hp=100;
                        level->ogreN++;
                    }
                }
                getc(mapa_arq);
            }
        }
        level->entitiesN=level->playerN+level->ogreN;
        fclose(mapa_arq);
    }
}
int nextLevel(ENTITY *entities, LEVEL *level, int *keysN, int *levelN, char map[][COLM], char mapC[][COLM], int *tela)
{
    if(*keysN==level->keys)
    {
        *levelN=*levelN+1;
        system("cls");
        gotoxy((xM/2)-10, yM/2);
        printf("Você avançou para o nível %d\n", *levelN);

        Sleep(4000);

        system("cls");

        loadLevel(entities, level, *levelN, map, mapC, &tela);

        *keysN=0;
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
