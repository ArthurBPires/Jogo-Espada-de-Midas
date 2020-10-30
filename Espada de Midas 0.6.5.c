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

#define LINM 11
#define COLM 27
#define xM 120
#define yM 25

void cursorState(int cursor);//Desativa o cursor se cursor=0 ou o ativa se cursor=1
void gotoxy(int x, int y);//Vai com o cursor para as coordenadas x e y.
void menuIn(int *tela, int *hpN);//Desenha o menu de inicialização e cria opções interativas.
void mapDraw(char map[][COLM],char mapC[][COLM], int mapSize, int entitiesPos[][2], int ogresN);//Desenha na tela o char que tiver sido alterado em relação ao frame anterior
void leverSwitch(int pX, int pY,int dir, char mapC[][COLM], char map[][COLM]);//Função que troca o estado das portas se uma alavanca é acionada.
void playerMov(int pX, int pY, int entitiesPos[][2], char dir, char mapC[][COLM], char map[][COLM]);
void ogreIa(char map[][COLM], int entitiesPos[][2], int ogresN,int timer, int ogreD[20], int enemiesDead[]);
void statsDisplay(int hpN, int coinsN, int pointsN, int mapSize, int imortal);
void coins(int pX, int pY, int *coinsN, int *coinsCounter , int *pointsN, char mapC[][COLM], char map[][COLM]);
void invulnerable(int timer, int *coinsCounter,int *imortal, int *timer2, int coinsN, int totalMapC);
void ingameMenu(char dir, int *tela);
void death(int pXIn, int pYIn, int entitiesPos[][2], int ogresN, char map[][COLM], char mapC[][COLM], int imortal, int *hpN, int *pointsN, int *tela, int enemiesDead[]);
void gameOver(int *tela);
int pointsScore();
int save();
int load();
int levels();
void options(int *tela, int *mapSize);//Abre a tela de opções interativas.

int main()
{
    int i, z, //Contadores.
    dir, //Guarda a tecla pressionada pelo jogador.
    tela=0, //Nessa variável é guardado qual a tela atual do jogo.
    sair=0, //Flag indicando que o jogo deve terminar.
    mapSize=1; //Indica o tamanho do mapa em unidades: 1x1, 2x2, 3x3...
    int entitiesPos[20][2], //Guarda a posição de cada inimigo.
    ogresN=0,//Guarda o número de ogros do mapa carregado.
    coinsN=0,//Guarda o número de moedas coletadas.
    coinsCounter=0,
    pointsN=0,//Guarda a quantidade de pontos adquiridos.
    totalMapC=0,
    totalMapK=0,
    hpN,
    timer=0,
    timer2=0,
    ogreD[20]={0},
    imortal=0,
    posIn[2],
    enemiesDead[20]={0};
    char mapC[LINM][COLM]={0};//Mapa e cópia do mapa utilizada para só alterar na tela aquilo que mudar de um frame para o outro.
    char map[LINM][COLM]={'#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','J','M','M',
    '#','#','#','#','M','M','M','M','M','M','M','M','B','M','M','#','#','#','#','M','M','C','#','#','M','#','M','M','M','M','M','M','#','#','#','#',
    'D','#','#','#','#','M','M','M','M','M','M','#','M','#','#','M','#','#','#','#','#','M','#','#','#','B','M','M','M','M','#','#','#','#','#','M',
    '#','#','#','M','#','#','M','M','M','O','M','M','M','M','M','D','M','#','C','#','M','D','M','M','M','M','M','M','M','M','O','#','#','#','#','#',
    '#','#','#','D','#','#','#','M','#','#','#','M','#','M','#','#','#','#','A','#','#','#','#','#','M','M','O','M','M','M','M','M','M','D','M','#',
    'C','#','M','D','M','M','M','M','M','M','O','M','M','#','#','A','#','#','#','#','M','M','#','M','#','M','M','M','M','M','#','M','#','D','#','#',
    '#','#','#','D','#','#','M','M','M','M','M','#','M','#','M','#','#','#','#','#','#','#','M','#','M','M','M','M','M','M','M','#','#','#','C','#',
    '#','M','#','M','B','M','M','M','M','D','M','M','M','M','#','M','#','M','#','#','C','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#',
    '#','#','#','#','#','#','#','#','#','#','#','#','#','#'};

    setlocale(LC_ALL,"Portuguese");

    srand(time(NULL));

    for(i=0;i<LINM;i++)
    {
        for(z=0;z<COLM;z++)
        {
            if(map[i][z]=='M') totalMapC++;
            if(map[i][z]=='C') totalMapK++;
            mapC[i][z]=map[i][z];
            if(map[i][z]=='J')//Guarda a posição do jogador.
            {
                entitiesPos[0][0]=z;
                entitiesPos[0][1]=i;
                posIn[0]=z;
                posIn[1]=i;
            }
            if(map[i][z]=='O')//Guarda a posição dos ogros(o sistema foi feito para permitir qualquer número de inimigos desejado).
            {
                ogresN++;
                entitiesPos[ogresN][0]=z;
                entitiesPos[ogresN][1]=i;
            }
        }
    }

    do
    {
        switch(tela)//Switch gerencia as telas á serem mostradas ao jogador.
        {
            case 0: system("cls");
                    cursorState(1);
                    menuIn(&tela, &hpN);
                break;
            case 1: cursorState(0);
                    mapDraw(map ,mapC, mapSize, entitiesPos, ogresN);
                break;
            case 2: system("cls");
                    load();
                break;
            case 3: system("cls");
                    levels();
                break;
            case 4: system("cls");
                    cursorState(1);
                    options(&tela,&mapSize);
                break;
            case 5: sair=1;
                break;
            case 6: system("cls");
                    gameOver(&tela);
                break;
            case 7:
                break;
        }

        if(tela==1)//Condicionar que vale se o jogo tiver começado.
        {
            if(kbhit())
            {
                dir = getch();

                leverSwitch(entitiesPos[0][0], entitiesPos[0][1], dir, mapC, map);

                //Muda a posição do jogador se pressionar WASD.
                playerMov(entitiesPos[0][0], entitiesPos[0][1], entitiesPos, dir, mapC, map);

                coins(entitiesPos[0][0], entitiesPos[0][1], &coinsN, &coinsCounter, &pointsN, mapC, map);

                ingameMenu(dir, &tela);
            }

            invulnerable(timer, &coinsCounter, &imortal, &timer2, coinsN, totalMapC);

            death(posIn[0], posIn[1], entitiesPos, ogresN, map, mapC, imortal, &hpN, &pointsN, &tela, enemiesDead);

            ogreIa(map, entitiesPos, ogresN, timer, ogreD, enemiesDead);

            statsDisplay(hpN, coinsN, pointsN, mapSize, imortal);
        }

        timer++;

        Sleep(100);//Taxa de atualização da tela em ms.

    }while(sair==0);

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
void menuIn(int *tela, int *hpN)
{
    int i, n, option=1;
    char dir;
    char text[7][80]={{"A Espada Perdida de Midas"},{"Começar"},{"Carregar"},{"Níveis"},{"Opções"},{"Sair"},("Versão: 0.6.5")};

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

    system("cls");

    if(*tela==1)
    {
        *hpN=3;
    }
}
void mapDraw(char map[][COLM],char mapC[][COLM], int mapSize, int entitiesPos[][2], int ogresN)
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
                        for(j=0; j<=ogresN; j++)    if(entitiesPos[j][0]==z&&entitiesPos[j][1]==i)  f=1;

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
void leverSwitch(int pX, int pY, int dir, char mapC[][COLM], char map[][COLM])
{
    int i, z;
    if((dir=='B'||dir=='b')&&(mapC[pY][pX]=='B'||mapC[pY+1][pX]=='B'||mapC[pY-1][pX]=='B'||mapC[pY][pX+1]=='B'||mapC[pY][pX-1]=='B'||mapC[pY+1][pX+1]=='B'||mapC[pY+1][pX-1]=='B'||mapC[pY-1][pX+1]=='B'||mapC[pY-1][pX-1]=='B'))
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
void playerMov(int pX, int pY, int entitiesPos[][2], char dir, char mapC[][COLM], char map[][COLM])
{
    if((dir=='A'||dir=='a')&&map[pY][pX-1]!='#'&& map[pY][pX-1]!='D')
    {
        map[pY][pX-1]= 'J';
        entitiesPos[0][0]-=1;
    }
    else if((dir=='D'||dir=='d')&&map[pY][pX+1]!='#'&& map[pY][pX+1]!='D')
    {
        map[pY][pX+1]= 'J';
        entitiesPos[0][0]+=1;
    }
    else if((dir=='W'||dir=='w')&&map[pY-1][pX]!='#'&& map[pY-1][pX]!='D')
    {
        map[pY-1][pX]= 'J';
        entitiesPos[0][1]-=1;
    }
    else if((dir=='S'||dir=='s')&&map[pY+1][pX]!='#'&& map[pY+1][pX]!='D')
    {
        map[pY+1][pX]= 'J';
        entitiesPos[0][1]+=1;
    }
}
void ogreIa(char map[][COLM], int entitiesPos[][2], int ogresN,int timer, int ogreD[20], int enemiesDead[])
{
    int i, z=0, oX, oY;

    for(i=1; i<=ogresN; i++)
    {
        oX=entitiesPos[i][0];
        oY=entitiesPos[i][1];

        while((ogreD[i]==0)||(ogreD[i]==z||((map[oY-1][oX]=='#'||map[oY-1][oX]=='D')&&ogreD[i]==1)||((map[oY][oX+1]=='#'||map[oY][oX+1]=='D')&&ogreD[i]==2)||((map[oY+1][oX]=='#'||map[oY+1][oX]=='D')&&ogreD[i]==3)||((map[oY][oX-1]=='#'||map[oY][oX-1]=='D')&&ogreD[i]==4)))
        {
            z=ogreD[i];
            ogreD[i]=(rand()%4)+1;
        }
        if(enemiesDead[i]==0&&((ogreD[i]==1&&timer%6==0)&&(map[oY-1][oX]!='#'&&map[oY-1][oX]!='D')))
        {
            map[oY-1][oX]= 'O';

            entitiesPos[i][1]-=1;
        }
        else if(enemiesDead[i]==0&&((ogreD[i]==2&&timer%6==0)&&(map[oY][oX+1]!='#'&&map[oY][oX+1]!='D')))
        {
            map[oY][oX+1]= 'O';

            entitiesPos[i][0]+=1;
        }
        else if(enemiesDead[i]==0&&((ogreD[i]==3&&timer%6==0)&&(map[oY+1][oX]!='#'&&map[oY+1][oX]!='D')))
        {
            map[oY+1][oX]= 'O';

            entitiesPos[i][1]+=1;
        }
        else if(enemiesDead[i]==0&&((ogreD[i]==4&&timer%6==0)&&(map[oY][oX-1]!='#'&&map[oY][oX-1]!='D')))
        {
            map[oY][oX-1]= 'O';

            entitiesPos[i][0]-=1;
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
void coins(int pX, int pY, int *coinsN, int *coinsCounter, int *pointsN, char mapC[][COLM], char map[][COLM])
{
    if(mapC[pY][pX]=='M')//Contador de moedas coletadas
    {
        mapC[pY][pX]=' ';
        *coinsN+=1;
        *coinsCounter+=1;
        *pointsN+=10;
    }
}
void invulnerable(int timer, int *coinsCounter, int *imortal, int *timer2, int coinsN, int totalMapC)
{
    if(*coinsCounter%10==0&&*coinsCounter!=0)
    {
        *timer2 = timer;
        *imortal=1;
        *coinsCounter-=10;
    }
    if((timer-*timer2)==25)
    {
        *imortal=0;
    }
    if(coinsN==totalMapC) *imortal=1;
}
void ingameMenu(char dir, int *tela)
{
    int i, n, option=1, selected=0;
    char text[10][20]={"Continuar","Salvar","Carregar","Opções","Sair"};
    char keypress;

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
                else if(keypress=='\r') selected=option;
            }
        }while(selected==0);

        if(selected==1) *tela=1;
        else if(selected==2) *tela=7;
        else if(selected==3) *tela=2;
        else if(selected==4) *tela=4;
        else if(selected==5) *tela=0;
    }
}
void death(int pXIn, int pYIn, int entitiesPos[][2], int ogresN, char map[][COLM], char mapC[][COLM],int imortal, int *hpN, int *pointsN, int *tela, int enemiesDead[])
{
    int i, enemyX, enemyY;

    for(i=1; i<=ogresN; i++)
    {
        if((entitiesPos[0][0]==entitiesPos[i][0])&&(entitiesPos[0][1]==entitiesPos[i][1]))
        {
            enemyX=entitiesPos[i][0];
            enemyY=entitiesPos[i][1];
            if(imortal==0)
            {
                *hpN-=1;
                map[pYIn][pXIn]='J';
                map[enemyY][enemyX]='O';
                entitiesPos[0][0]=pXIn;
                entitiesPos[0][1]=pYIn;
            }
            else
            {
                map[enemyY][enemyX]='J';
                entitiesPos[i][0]=0;
                entitiesPos[i][1]=0;
                enemiesDead[i]=1;
                *pointsN+=100;
            }
            if(*hpN==0) *tela=6;
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
int save()
{

}
int load()
{

}
int levels()
{

}
void options(int *tela, int *mapSize)
{
    int i, n, option=1;
    char dir;
    char text[5][80]={{"Opções"},{"Tamanho do mapa: << Pequeno >>"},{"Voltar"},{"Tamanho do mapa: << Médio >>"},{"Tamanho do mapa: << Grande >>"}};

    for(i=0; i<3; i++)
    {
        n = strlen(text[i]);
        gotoxy((xM-n)/2,(yM/3*(i+1)));
        printf("%s", text[i]);
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
                    printf("%s  ", text[4]);

                    *mapSize=3;
                }
                else if(*mapSize==2)
                {
                    gotoxy((xM-strlen(text[3])-1)/2,(yM/3*2));
                    printf("%s", text[1]);

                    *mapSize=1;
                }
                else if(*mapSize==3)
                {
                    gotoxy((xM-strlen(text[4]))/2,(yM/3*2));
                    printf("%s", text[3]);

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
                    printf("%s", text[4]);

                    *mapSize=3;
                }
                else if(*mapSize==3)
                {
                    gotoxy((xM-strlen(text[4]))/2,(yM/3*2));
                    printf("%s", text[1]);

                    *mapSize=1;
                }
            }
            else if(dir=='\r'&&option==2) *tela = 0;
        }

    }while(*tela==4);
}
