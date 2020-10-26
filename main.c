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

#define LINM 11
#define COLM 27
#define xM 120
#define yM 25

void cursorState(int cursor);//Desativa o cursor se cursor=0 ou o ativa se cursor=1
void gotoxy(int x, int y);//Vai com o cursor para as coordenadas x e y.
void menuIn(int *tela);//Desenha o menu de inicialização e cria opções interativas.
void mapDraw(char map[][COLM],char mapC[][COLM], int mapSize,int pX,int pY);//Desenha na tela o char que tiver sido alterado em relação ao frame anterior
void leverSwitch(int pX, int pY,int dir, char mapC[][COLM]);//Função que troca o estado das portas se uma alavanca é acionada.
int ogreIa();
int points();
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
    mapSize=1, //Indica o tamanho do mapa em unidades: 1x1, 2x2, 3x3...
    pX, pY;//Guarda a posição do jogador.
    int entitiesPos[20][2], //Guarda a posição de cada inimigo.
    ogresN=0,//Guarda o número de ogros do mapa carregado.
    coins=0,//Guarda o número de moedas coletadas.
    points=0;//Guarda a quantidade de pontos adquiridos.
    char mapC[LINM][COLM]={0};//Cópia do mapa utilizada para só alterar na tela aquilo que mudar de um frame para o outro.
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

    for(i=0;i<LINM;i++)
    {
        for(z=0;z<COLM;z++)
        {
            if(map[i][z]=='J')//Guarda a posição do jogador.
            {
                pX=z;
                pY=i;
            }
            if(map[i][z]=='O')//Guarda a posição dos ogros(o sistema foi feito para permitir qualquer número de inimigos desejado).
            {
                entitiesPos[ogresN][0]=z;
                entitiesPos[ogresN][1]=i;
                ogresN++;
            }
        }
    }

    do
    {
        switch(tela)//Switch gerencia as telas á serem mostradas ao jogador.
        {
            case 0: system("cls");
                    cursorState(1);
                    menuIn(&tela);
                break;
            case 1: cursorState(0);
                    mapDraw(map ,mapC, mapSize, pX, pY);
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
                    system("cls");
                    gotoxy((xM-11)/2,0);
                    printf("Fim de jogo");
                break;
        }

        if(tela==1)//Condicionar que vale se o jogo tiver começado.
        {
            if(kbhit())
            {
                dir = getch();

                leverSwitch(pX, pY, dir, mapC);

                //Muda a posição do jogador se pressionar WASD.
                if((dir=='A'||dir=='a')&&mapC[pY][pX-1]!='#'&& mapC[pY][pX-1]!='D')
                {
                    map[pY][pX]= ' ';
                    map[pY][pX-1]= 'J';
                    pX-=1;
                }
                else if((dir=='D'||dir=='d')&&mapC[pY][pX+1]!='#'&& mapC[pY][pX+1]!='D')
                {
                    map[pY][pX]= ' ';
                    map[pY][pX+1]= 'J';
                    pX+=1;
                }
                else if((dir=='W'||dir=='w')&&mapC[pY-1][pX]!='#'&& mapC[pY-1][pX]!='D')
                {
                    map[pY][pX]= ' ';
                    map[pY-1][pX]= 'J';
                    pY-=1;
                }
                else if((dir=='S'||dir=='s')&&mapC[pY+1][pX]!='#'&& mapC[pY+1][pX]!='D')
                {

                    map[pY][pX]= ' ';
                    map[pY+1][pX]= 'J';
                    pY+=1;
                }
                if(mapC[pY][pX]=='M')//Contador de moedas coletadas
                {
                    coins++;
                    points+=10;
                    gotoxy(COLM*mapSize+3,mapSize);
                    printf("Moedas: %d", coins);
                }
            }
        }

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
void menuIn(int *tela)
{
    int i, n, option=1;
    char dir;
    char text[6][80]={{"A Espada Perdida de Midas"},{"Começar"},{"Carregar"},{"Níveis"},{"Opções"},{"Sair"}};

    for(i=0; i<6; i++)
    {
        n = strlen(text[i]);
        gotoxy((xM-n)/2,(yM/6*(i+1)));
        printf("%s", text[i]);
    }

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
}
void mapDraw(char map[][COLM],char mapC[][COLM], int mapSize, int pX, int pY)
{
    int i, z, countX, countY;

    for(i=0; i<LINM; i++)
    {
        for(z=0; z<COLM; z++)
        {
            gotoxy(z*mapSize,i*mapSize);
            if(map[i][z]!=mapC[i][z])
            {
                for(countY=0; countY<mapSize; countY++)
                {
                    for(countX=0; countX<mapSize; countX++)
                    {
                        if((z!=pX||i!=pY)&&(mapC[i][z]=='A'||mapC[i][z]=='D'||mapC[i][z]=='B'))
                        {
                            printf("%c", mapC[i][z]);
                            map[i][z]=mapC[i][z];
                        }
                        else printf("%c",map[i][z]);
                    }
                    gotoxy(z*mapSize,i*mapSize+countY+1);
                }
                if(mapC[i][z]!='A'&&(mapC[i][z]!='D'&&mapC[i][z]!='B')) mapC[i][z]=map[i][z];
            }
        }
    }
}
void leverSwitch(int pX, int pY, int dir, char mapC[][COLM])
{
    int i, z;
    if((dir=='B'||dir=='b')&&(mapC[pY][pX]=='B'||mapC[pY+1][pX]=='B'||mapC[pY-1][pX]=='B'||mapC[pY][pX+1]=='B'||mapC[pY][pX-1]=='B'||mapC[pY+1][pX+1]=='B'||mapC[pY+1][pX-1]=='B'||mapC[pY-1][pX+1]=='B'||mapC[pY-1][pX-1]=='B'))
    {
        for(i=0;i<LINM;i++)
        {
            for(z=0;z<COLM;z++)
            {
                if(mapC[i][z]=='A') mapC[i][z]='D';
                else if(mapC[i][z]=='D') mapC[i][z]='A';
            }
        }
    }
}
int ogreIa()
{

}
int points()
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
