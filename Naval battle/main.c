#include <stdio.h>
#include <stdlib.h>                                                                                                                                            //Made by rodrigo soria
#include <time.h>
#include <windows.h>

///CONSTANTES

const int DIM = 10;

///ESTRUCTURAS

typedef struct
{
    char nombre[30];
    int vidas;
    int aciertos;
    int fallidos;
    int puntuacion;
    int turns;
}estadisticas;

///FUNCIONES

void Battle_Main_menu(void);
int BattleNavMain(estadisticas *Jugador);

void Print_Menu(void);
void inicMatriz(char Table1[][DIM], char Table2[][DIM], char Table3[][DIM]);
void imprimir_tablero(char Table1[][DIM], char Table2[][DIM]);
void mostrar_stats(estadisticas *PJ, estadisticas PNJ, int TurnoActual);
void Score_Board(char nombre[]);

void posicionarCursor(short int x, short int y);
int crear_Barcos(char Table[][DIM]);
void verificar_espacio(short int direccion, short int tamanio, COORD Posicion, int *tipo_barco, char Table[][DIM]);

int AtaqueHumano(char TableroMachine[][DIM], char TableroENEMIGO[][DIM], COORD Objetive);
void Seleccionar_Coordenadas(COORD *Position);

int AI_Attack(char TableroJug[][DIM], COORD *Originales, COORD *ActualPos, short int *direccion, short int Disponibles[], int validos, short int *encontrado);
short int Verificar_direciones(short int Dispo[], int valids);
void Mover_posicionXdir(short int *dir, char Tabl_Jug[][DIM], COORD *ActualPosition, short int disponible_Dirs[]);

void Calcular_punt(estadisticas *Usuario, int Vida_enemiga);
void AgregarRegistro(estadisticas PJ, char nombre[]);
void OrdenarRegistros(char nombre[]);
void ordenarPorPuntuacion(estadisticas Arreglo[], int maximo);
void AcomodarElemento(estadisticas[], int, estadisticas);




///CUERPO

int main()
{

    Battle_Main_menu();

    return 0;
}


void Battle_Main_menu(void)
{
    short int opcion = 0;
    estadisticas Player;
    //Datos para las puntuaciones.
    char Nombre_archivo[30] = "Puntuaciones.bin";
    int SaveStats = 0;


    do
    {
        system("cls");
        Print_Menu();
        posicionarCursor(30, 9);
        scanf("%hd", &opcion);
        switch(opcion)
        {
        case 1:
                system("cls");

                SaveStats = BattleNavMain(&Player);
                if(SaveStats == 1)
                {
                    AgregarRegistro(Player, Nombre_archivo);
                }

                break;
        case 2:
                system("cls");
                Score_Board(Nombre_archivo);

                break;
        case 3:
                break;
        default:
                posicionarCursor(27, 13);
                printf("Ingrese una opcion valida");
        }
    }while(opcion != 3);


}

void Print_Menu(void)
{
    printf("          Batalla naval en C, disfrutalo lince.\n");
    printf("                    Elija una opcion           \n");
    printf("                                               \n");
    printf("                  |-------------------|        \n");
    printf("                  |                   |        \n");
    printf("                  | 1 - Jugar         |        \n");
    printf("                  | 2 - Puntuaciones  |        \n");
    printf("                  | 3 - Salir         |        \n");
    printf("                  |                   |        \n");
    printf("                  | Opcion:           |        \n");
    printf("                  |                   |        \n");
    printf("                  |-------------------|        \n");
}


///Funcion donde se desarrolla el juego.
int BattleNavMain(estadisticas *Jugador)
{

    ///Inicializacion de Tablero
                 //DIM = tamaño usable + 2 que cuentan como borde (tanto para Y como para X).
    char table_Jug1[DIM][DIM];
    char table_Machine[DIM][DIM];
    char table_EnemyZone[DIM][DIM];
    inicMatriz(table_Jug1, table_Machine, table_EnemyZone);

    imprimir_tablero(table_Jug1, table_EnemyZone);

    ///POSICIONAMIENTO DE BARCOS.

    srand(time(NULL));
    int espacios_Jug1 = 0;
    int espacios_Machine = 0;
    espacios_Jug1 = crear_Barcos(table_Jug1);
    espacios_Machine = crear_Barcos(table_Machine);

    ///VARIABLES DE CONTROL DE JUEGO

    //estadisticas *Jugador;
        (*Jugador).vidas = espacios_Jug1;
        (*Jugador).aciertos = 0;
        (*Jugador).fallidos = 0;
    estadisticas Maquina;
        Maquina.vidas = espacios_Machine;
        Maquina.aciertos = 0;
        Maquina.fallidos = 0;
    int turno = 0;
    int Hit = 0;
    COORD Objetivo;

    ///VARIABLES DE CONTROL DE AI.

    short int Direction = 0;
        /*  Direcciones:
             - 0: Ninguna direccion.
             - 1: arriba.
             - 2: abajo.
             - 3: izquierda.
             - 4: derecha.
                amen (?.
        */
    short int Directions_Used[5] = {0}; //Determinara que direcciones se usan y cuales han sido usadas.
    int Direct_Validos = 5;
    short int Barco_encontrado = 0;     //Variable que se irá sumando a medida que encuentre barcos.

    COORD Original_Boat_Found; //Se guardará con la anteultima posicion de barco encontrada, como si fuera la "sombra" de Newer_Boat
          Original_Boat_Found.X = 0;
          Original_Boat_Found.Y = 0;

    COORD Newer_Boat;          //Se verá modificada con los movimientos realizados en la Original.
          Newer_Boat.X = 0;
          Newer_Boat.Y = 0;


    ///BUCLE JUEGO

    while((*Jugador).vidas > 0 && Maquina.vidas > 0)
    {
        turno++;
        system("cls");
        ///MUESTRA DONDE ATACÓ LA IA DURANTE SU TURNO.
        if(turno > 1)
        {
            posicionarCursor(0, DIM + 8);
            printf("Maquina ataca a X: %hd Y: %hd", Newer_Boat.X, Newer_Boat.Y);
        }
        imprimir_tablero(table_Jug1, table_EnemyZone);                                                                                                                                                                   //Made by Rodrigo Soria
        mostrar_stats(Jugador, Maquina, turno);

        ///ATAQUE DEL JUGADOR.

        Seleccionar_Coordenadas(&Objetivo);
        Hit = AtaqueHumano(table_Machine, table_EnemyZone, Objetivo);
        if (Hit == 1)
        {
            (*Jugador).aciertos++;
            Maquina.vidas--;
        }else
        {
            (*Jugador).fallidos++;
        }

        imprimir_tablero(table_Jug1, table_EnemyZone);                                                                                                                                                                   //Made by Rodrigo Soria
        mostrar_stats(Jugador, Maquina, turno);

        if(Maquina.vidas > 0)
        {
            ///DESARROLLO Y ATAQUE DE AI.

            Hit = AI_Attack(table_Jug1, &Original_Boat_Found, &Newer_Boat, &Direction, Directions_Used, Direct_Validos, &Barco_encontrado);

            system("cls");
            imprimir_tablero(table_Jug1, table_EnemyZone);                                                                                                                                                                   //Made by Rodrigo Soria
            mostrar_stats(Jugador, Maquina, turno);

            if (Hit == 1)
            {
                Maquina.aciertos++;
                (*Jugador).vidas--;
            }else
            {
                Maquina.fallidos++;
            }
        }

    } //Main while loop end.

    ///Ultima pasada de mostrar pantalla.
    system("cls");
    imprimir_tablero(table_Jug1, table_EnemyZone);                                                                                                                                                                   //Made by Rodrigo Soria
    mostrar_stats(Jugador, Maquina, turno);
    posicionarCursor(0, DIM + 8);
    printf("Maquina ataca a X: %hd Y: %hd", Newer_Boat.X, Newer_Boat.Y);

    ///MUESTRA VICTORIA O DERROTA
    posicionarCursor(24, DIM + 9);
    if((*Jugador).vidas > 0)
    {
        printf("HAS VENCIDO SOBRE TU ENEMIGO!");
    }else
    {
        printf("TU FLOTA HA SIDO DESTRUIDA");
    }
    posicionarCursor(24, DIM + 10);
    system("PAUSE");

    system("cls");
    printf("Ingrese nombre del jugador: ");
    fflush(stdin);
    gets((*Jugador).nombre);
    ///Calcula la puntuacion del jugador.
    (*Jugador).turns = turno;
    Calcular_punt(Jugador, Maquina.vidas);

    short int OpciondeGuardar = 2;
    printf("Desea guardar el registro?\n 1- Si\n0- No\nOpcion: ");
    while(OpciondeGuardar < 0 || OpciondeGuardar > 1)
    {
        scanf("%hd", &OpciondeGuardar);
    }

    return OpciondeGuardar;
}

///Coloca bordes a los tableros y crea los espacios libres:
void inicMatriz(char Table1[][DIM], char Table2[][DIM], char Table3[][DIM])
{
    int F, C;
    for( F = 0; F < DIM; F++)
    {
        for( C = 0; C < DIM; C++)
        {
            if(F == 0 || F == DIM-1)      //Top and bottom bar
            {
                Table1[F][C] = '-';
                Table2[F][C] = '-';
                Table3[F][C] = '-';
            }else{
                if(C == 0 || C == DIM-1)  //Left and right bar
                {
                    Table1[F][C] = '|';
                    Table2[F][C] = '|';
                    Table3[F][C] = '|';
                }else{
                    Table1[F][C] = '~';   //Valid zones
                    Table2[F][C] = '~';
                    Table3[F][C] = '~';
                 }

             }

        } /// "C" loop end

    } /// "F" loop end

}

void imprimir_tablero(char Table1[][DIM], char Table2[][DIM])
{
    int F, C;
    for (F = 0; F < DIM; F++)
    {
        for ( C = 0; C < DIM; C++)
        {
            posicionarCursor(C+30, F+5);           ///MODIFICAR Parametros EN CASO DE imprimir los tableros en otro sitio
            printf("%c", Table1[F][C]);
            posicionarCursor(C+30+DIM, F+5);
            printf("%c", Table2[F][C]);
        }
        printf("\n");
    }
}

///Funcion que determina la cantidad de barcos a colocar y su forma.
int crear_Barcos(char Table[][DIM])
{

    ///POSICION INICIAL Y DIRECCION A VERIFICAR PARA COLOCAR BARCOS
    COORD POS_Inicial;
    short int direccion = 0;
    /*  Direcciones:
             - 1: derecha.
             - 2: abajo.
    */

    /// variables que determinan la cantidad de barcos.
    int S = 3;  // Small boats (2x1)
    int M = 2;  // Medium boats (3x1)
    int L = 1;  // Large boats (4x1)
    int EspaciosTomados = 0;
    EspaciosTomados = (S * 2) + (M * 3) + (L * 4);


    ///BUCLE QUE COLOCA LOS BARCOS.
    while( S > 0)
    {
        //Obtiene coordenadas aleatorias entre 1 y 8. y una direccion para colocar el barco.
        POS_Inicial.X = (rand()%(DIM-2))+1;
        POS_Inicial.Y = (rand()%(DIM-2))+1;
        direccion = (rand()%2)+1;
        ///Los barcos se colocan en orden del mas grande al mas chico.
        if(L > 0){
               verificar_espacio(direccion, 4, POS_Inicial, &L, Table);
        }else{
            if(M > 0){
                   verificar_espacio(direccion, 3, POS_Inicial, &M, Table);

            }else{
                if(S > 0){
                        verificar_espacio(direccion, 2, POS_Inicial, &S, Table);
                }
            }
        }

    }  //While loop end

return EspaciosTomados;
}

///Llamada por "Crear_Barcos", verifica si hay espacio suficiente y luego coloca los barcos en la direccion deseada.
void verificar_espacio(short int direccion, short int tamanio, COORD Posicion, int *tipo_barco, char Table[][DIM])
{
    short int recorrido = 0; //Se usara para recorrer los espacios adyacentes y asi verificar si es posible poner el barco.
    short int flag = 1; //Determina si hay o no hay espacio suficiente.

    ///EN EL CASO DE QUE LA DIRECCION SEA A LA DERECHA:
    if (direccion == 1)
    {
        while (recorrido < tamanio && flag != 0)         ///se recorren las posiciones necesarias para verificar si hay espacio.
        {
            if (Table[Posicion.Y][Posicion.X + recorrido] != '~')
            {
                flag = 0;
            }
            recorrido++;
        }

                ///COLOCACION DE BARCO.
        recorrido = 0; //Recorrido vuelve a su valor original.
        if ( flag == 1)
        {
            ///Se recorre nuevamente las posiciones, esta vez sabiendo que hay espacio, y se coloca el barco.
            while (recorrido < tamanio)
            {
                Table[Posicion.Y][Posicion.X + recorrido] = 'O';
                recorrido++;
            }
            ///Se resta 1 a la cantidad de barcos que hay de éste tipo en la funcion llamadora.
            (*tipo_barco)--;
        }

    }

    ///EN EL CASO DE QUE LA DIRECCION SEA HACIA ABAJO:

    if (direccion == 2)
    {
        ///se recorren las posiciones necesarias para verificar si hay espacio.
        while (recorrido < tamanio && flag != 0)
        {
            if (Table[Posicion.Y + recorrido][Posicion.X] != '~')
            {
                flag = 0;
            }
            recorrido++;
        }
        recorrido = 0; //Recorrido vuelve a su valor original.
        if ( flag == 1)
        {
            ///Se recorre nuevamente las posiciones, esta vez sabiendo que hay espacio, y se coloca el barco.
            while (recorrido < tamanio)
            {
                Table[Posicion.Y + recorrido][Posicion.X] = 'O';
                recorrido++;
            }
            ///Se resta 1 a la cantidad de barcos que hay de éste tipo en la funcion llamadora.
            (*tipo_barco)--;
        }

    }

}

///Estadisticas del juego alrededor del tablero principal.
void mostrar_stats(estadisticas *PJ, estadisticas PNJ, int TurnoActual)
{

        ///LADO SUPERIOR CON RESPECTO AL TABLERO
        posicionarCursor(0, 0);
        printf("\n          Batalla naval en C, disfrutalo lince.\n");
        posicionarCursor(36, 4);
        printf("Turno %d", TurnoActual);

        ///LADO IZQUIERDO CON RESPECTO AL TABLERO
        posicionarCursor(13, 5);
        printf("Jugador");
        posicionarCursor(10, 6);
        printf("Vida: %d", (*PJ).vidas);
        posicionarCursor(10, 7);
        printf("Aciertos: %d", (*PJ).aciertos);
        posicionarCursor(10,8);
        printf("Fallidos: %d", (*PJ).fallidos);

        ///LADO DERECHO CON RESPECTO AL TABLERO
        posicionarCursor(20+(DIM*2)+20, 5);
        printf("Maquina");
        posicionarCursor(20+(DIM*2)+15, 6);
        printf("Vida: %d", PNJ.vidas);
        posicionarCursor(20+(DIM*2)+15, 7);
        printf("Aciertos: %d", PNJ.aciertos);
        posicionarCursor(20+(DIM*2)+15,8);
        printf("Fallidos: %d", PNJ.fallidos);

        ///LADO INFERIOR CON RESPECTO AL TABLERO.
        posicionarCursor(22, DIM+5);
        printf("Ingrese coordenadas para el ataque");
        posicionarCursor(35, DIM+6);
        printf("X:     Y: ");
        posicionarCursor(59, 24);
        printf("Made by Rodrigo Soria");
}

///ATAQUE DE USUARIO.

void Seleccionar_Coordenadas(COORD *Position)
{
    posicionarCursor(38,DIM+6);
    scanf("%hd", &(*Position).X);
    posicionarCursor(45,DIM+6);
    scanf("%hd", &(*Position).Y);
    while((*Position).X >= DIM-1 || (*Position).X <= 0 || (*Position).Y >= DIM-1 || (*Position).Y <= 0)
    {
        posicionarCursor(27,DIM+7);
        printf("Ingrese una posicion valida. ");
        posicionarCursor(38,DIM+6);
        scanf("%hd", &(*Position).X);
        posicionarCursor(45,DIM+6);
        scanf("%hd", &(*Position).Y);
    } //Control while loop end.
}

int AtaqueHumano(char TableroMachine[][DIM], char TableroENEMIGO[][DIM], COORD Objetive)
{
    int HitAttack = 0;
    if (TableroENEMIGO[Objetive.Y][Objetive.X] != 'Ø')
    {
        if (TableroMachine[Objetive.Y][Objetive.X] == 'O')
        {
            TableroENEMIGO[Objetive.Y][Objetive.X] = 'Ø';
            TableroMachine[Objetive.Y][Objetive.X] = 'Ø';
            HitAttack = 1;
        }else
        {
            TableroENEMIGO[Objetive.Y][Objetive.X] = 'X';
        }
    }
    return HitAttack;
}

///A.I. ATTACK

int AI_Attack(char TableroJug[][DIM], COORD *Originales, COORD *ActualPos, short int *direccion, short int Disponibles[], int validos, short int *encontrado)
{

    int HitAttack = 0;
    int i;
    short int existeDireccion = 0;

    ///SE FIJA SI AÚN SE PUEDEN HACER MOVIMIENTOS HACIA ALGUN LADO.

    existeDireccion = Verificar_direciones(Disponibles, validos);

    ///SI YA NO QUEDAN MOVIMIENTOS QUIERE DECIR QUE TERMINO DE RECORRER EL BARCO O REVISO TODAS LAS DIRECCIONES Y DEBE REINICIAR TODO.

    if(existeDireccion == 0)
    {
        *direccion = 0;
        (*Originales).X = 0;
        (*Originales).Y = 0;
        for(i = 0; i < validos; i++)
        {
            Disponibles[i] = 0;
        }
        printf("SE QUEDO SIN MOVIMIENTOS\n");
        *encontrado = 0;
    }

    ///DA POSICIONES ALEATORIAS PARA LA PRIMERA VEZ DESPUES DE CADA BARCO COMPLETO, Reiniciando las direcciones.

    if(*encontrado == 0)
    {
        (*Originales).X = (rand()%(DIM-2))+1;
        (*Originales).Y = (rand()%(DIM-2))+1;
        (*ActualPos).X = (*Originales).X;
        (*ActualPos).Y = (*Originales).Y;
        for(i = 0; i < validos; i++)
        {
        Disponibles[i] = 0;
        }
        *direccion = 0;
        printf("TIRO AL AZAR PARA ENCONTRAR ORIGINAL\n");
    }

    ///CUANDO LA ACTUAL POSICION SE MUEVA Y NO ENCUENTRE UN OBJETIVO, DEBERA VOLVER A DONDE ESTABA. (Se utiliza Direccion para saber si el ataque fue fallido. No es necesario si ya son iguales)
    if(((*ActualPos).X != (*Originales).X || (*ActualPos).Y != (*Originales).Y) && *direccion == 0)
    {
        (*ActualPos).X = (*Originales).X;
        (*ActualPos).Y = (*Originales).Y;
    }


    ///INTENTARA CON DIRECCIONES ALEATORIAS CUANDO ENCUENTRE UN BARCO Y SIEMPRE QUE HAYA DISPONIBLES.

    if(*encontrado >= 1 && *direccion == 0 && existeDireccion == 1)
    {
        while(Disponibles[*direccion] == 1)
        {
            *direccion = (rand()%4)+1;
        }
    }

    ///MUEVE LA POSICION DE ATAQUE DE ACUERDO A LA DIRECCION

    Mover_posicionXdir(direccion, TableroJug, ActualPos, Disponibles);

    ///ATAQUE
    ///Si su golpe será en una posicion ya usada elegirá otra o cambiará de dirección de acuerdo a si se puede o no.
    while(TableroJug[(*ActualPos).Y][(*ActualPos).X] == 'X' || TableroJug[(*ActualPos).Y][(*ActualPos).X] == 'Ø')
    {
           ///SI NO HAY ALTERNATIVAS TIRARÁ AL AZAR.
        if(existeDireccion == 0)
        {
            ///TIRARA UNA NUEVA POSICION AL AZAR Y REINICIA LA DIRECCION.
            (*ActualPos).X = (rand()%(DIM-2))+1;
            (*ActualPos).Y = (rand()%(DIM-2))+1;
            *direccion = 0;
            ///AL HABER SELECCIONADO UNA NUEVA POSICION, REINICIA LA VARIABLE QUE CONTROLA LOS BARCOS ENCONTRADOS
            *encontrado = 0;

            printf("TIRO AL AZAR PORQUE YA GOLPEO AHÍ\n");
        }else
        {
            ///BLOQUEA ESA DIRECCION
            Disponibles[*direccion] = 1;
            ///Se fija si existen otras alternativas.
            existeDireccion = Verificar_direciones(Disponibles, validos);

            //Da una nueva direccion disponible si es que existen otras direcciones.
            while(Disponibles[*direccion] == 1 && existeDireccion == 1)
            {
                *direccion = (rand()%4)+1;
            }
            ///VUELVE LA ACTUAL POSICION A SU VALOR ANTERIOR(ORIGINAL)
            (*ActualPos).X = (*Originales).X;
            (*ActualPos).Y = (*Originales).Y;
            if(existeDireccion == 1) ///SI ELIJIÓ OTRA DIRECCION DE ENTRE LAS DISPONIBLES LO MOVERÁ SEGÚN CORRESPONDA.
            {
                Mover_posicionXdir(direccion, TableroJug, ActualPos, Disponibles);
            }

        }
    }//While loop end

    if(TableroJug[(*ActualPos).Y][(*ActualPos).X] != 'O')
    {
        ///Bloquea esa direccion y reinicia la actual para que en el proximo turno se vuelva a elegir una nueva..
        Disponibles[*direccion] = 1;
        *direccion = 0;
        if(TableroJug[(*ActualPos).Y][(*ActualPos).X] == '~')
        {
           TableroJug[(*ActualPos).Y][(*ActualPos).X] = 'X';
        }
        ///SI YA HABIA ACERTADO A UN BARCO EN MAS DE DOS POSICIONES SEGUIDAS, QUIERE DECIR QUE LLEGO AL FINAL Y REINICIA ENCONTRADOS.
        if(*encontrado > 1)
            {
                *encontrado = 0;
            }

    }else
    {
        ///MUEVE LAS POSICIONES ORIGINALES A LA ENCONTRADA RECIENTEMENTE.
        (*Originales).X = (*ActualPos).X;
        (*Originales).Y = (*ActualPos).Y;
        TableroJug[(*ActualPos).Y][(*ActualPos).X] = 'Ø';
        HitAttack = 1;

         ///AUMENTA ENCONTRADO POR CADA BARCO CONSECUTIVO QUE ENCUENTRE
        (*encontrado)++;
    }



return HitAttack;
}

///Llamada por la IA para ver si hay direcciones sin usar.
short int Verificar_direciones(short int Dispo[], int valids){
    int j;
    short int existDir = 0;
    for(j = 0; j < valids; j++)
    {
        if(Dispo[j] == 0)
        {
            existDir = 1;
        }
    }
    return existDir;
}

///Llamada por la IA para mover la posicion actual de acuerdo a una direccion.
void Mover_posicionXdir(short int *dir, char Tabl_Jug[][DIM], COORD *ActualPosition, short int disponible_Dirs[]){

    switch(*dir)
    {
    case 0:
        break;
    case 1:
        if(Tabl_Jug[(*ActualPosition).Y-1][(*ActualPosition).X] == 'O' || Tabl_Jug[(*ActualPosition).Y-1][(*ActualPosition).X] == '~')
        {
            (*ActualPosition).Y--;
        }else
        {
            disponible_Dirs[*dir] = 1;
        }
        break;
    case 2:
        if(Tabl_Jug[(*ActualPosition).Y+1][(*ActualPosition).X] == 'O' || Tabl_Jug[(*ActualPosition).Y+1][(*ActualPosition).X] == '~')
        {
            (*ActualPosition).Y++;
        }else
        {
            disponible_Dirs[*dir] = 1;
        }
        break;
    case 3:
        if(Tabl_Jug[(*ActualPosition).Y][(*ActualPosition).X-1] == 'O' || Tabl_Jug[(*ActualPosition).Y][(*ActualPosition).X-1] == '~')
        {
            (*ActualPosition).X--;
        }else
        {
            disponible_Dirs[*dir] = 1;
        }
        break;
    case 4:
        if(Tabl_Jug[(*ActualPosition).Y][(*ActualPosition).X+1] == 'O' || Tabl_Jug[(*ActualPosition).Y][(*ActualPosition).X+1] == '~')
        {
            (*ActualPosition).X++;
        }else
        {
            disponible_Dirs[*dir] = 1;
        }
        break;
    }
}



///FUNCIONES DE LAS PUNTUACIONES

///Calcula puntaje
void Calcular_punt(estadisticas *Usuario, int Vida_enemiga)
{
    int atk_hits = 0;
    int atk_miss = 0;
    int turnitos = 0;
    int Vida = 0;
    Vida = (*Usuario).vidas;
    atk_hits = (*Usuario).aciertos * 35;
    atk_miss = (*Usuario).fallidos * 3;
    turnitos = (*Usuario).turns * 2;
    Vida_enemiga = Vida_enemiga * 2;


    //Calcula
    (*Usuario).puntuacion = (atk_hits + Vida)- (atk_miss + turnitos + Vida_enemiga);

}

///GUARDA EL NUEVO SCORE
void AgregarRegistro(estadisticas PJ, char nombre[])
{
    FILE *Puntos_DataInfo = NULL;

    ///ABRE EL ARCHIVO PARA AGREGAR AL NUEVO USUARIO.
    Puntos_DataInfo = fopen(nombre, "ab");
    if(Puntos_DataInfo == NULL)
    {
        printf("Error al cargar \"%s\" \nNo se ha podido agregar al jugador: \"%s\" ", nombre, PJ.nombre);
    }else
    {
        fwrite(&PJ, sizeof(estadisticas), 1, Puntos_DataInfo);

    }

    fclose(Puntos_DataInfo);
}

///PASA A UN ARREGLO TODO EL ARCHIVO Y LO ORDENA
void OrdenarRegistros(char nombre[])
{
    FILE *archi = NULL;
    archi = fopen(nombre, "r+b");
    int validos = 0;

    if(archi == NULL)
    {
        printf("Error al abrir el archivo %s", nombre);
    }else
    {
        ///Cuenta la cantidad de registros que hay
        fseek(archi, 0, SEEK_END);
        validos = ftell(archi) / sizeof(estadisticas);
        fseek(archi, 0, SEEK_SET);

        ///Crea un arreglo con esa cantidad de registros
        estadisticas Stats_AllPlayers[validos];
        fread(Stats_AllPlayers, sizeof(estadisticas), validos, archi);

        ///Lo ordena.
        ordenarPorPuntuacion(Stats_AllPlayers, validos);

        ///Vuelve al inicio y vuelca toda la informacion al archivo.
        fseek(archi, 0, SEEK_SET);
        fwrite(Stats_AllPlayers, sizeof(estadisticas), validos, archi);
    }


    fclose(archi);
}

void ordenarPorPuntuacion(estadisticas Arreglo[], int maximo)
{
    int u = 0;
    while ( u < (maximo-1))
    {
        AcomodarElemento(Arreglo, u, Arreglo[u+1]);
        u++;
    }

}

///Llamada por "OrdenarPorPuntuacion" (ordena por insercion)
void AcomodarElemento(estadisticas arreglo[], int U, estadisticas elemento)
{
    while ( U >= 0 && elemento.puntuacion > arreglo[U].puntuacion)
    {
        arreglo[U+1] = arreglo[U];
        U--;
    }
    arreglo[U+1] = elemento;
}

///MUESTRA LA TABLA DE PUNTUACIONES

void Score_Board(char Nombre[])
{
    OrdenarRegistros(Nombre);

    system("cls");
    FILE *archi = NULL;
    archi = fopen(Nombre, "rb");
    estadisticas ActualReg;
    int Fila = 0;

    if(archi == NULL)
    {
        printf("Error al abrir el archivo %s \n no se han podido cargar las puntuaciones.\n", Nombre);
    }else
    {
        printf("--------------------------------|-------|----------|----------|--------|--------");
        printf("Nombre                          | Vidas | Aciertos | Fallidos | turnos | Puntaje");
        printf("--------------------------------|-------|----------|----------|--------|--------");
        while(fread(&ActualReg, sizeof(estadisticas), 1, archi) > 0)
        {
            posicionarCursor(0,Fila+3);
            printf("%s", ActualReg.nombre);
            posicionarCursor(32,Fila+3);
            printf("|  %d",ActualReg.vidas);
            posicionarCursor(40,Fila+3);
            printf("|  %d", ActualReg.aciertos);
            posicionarCursor(51,Fila+3);
            printf("|  %d", ActualReg.fallidos);
            posicionarCursor(62,Fila+3);
            printf("|  %d", ActualReg.turns);
            posicionarCursor(71,Fila+3);
            printf("| %d", ActualReg.puntuacion);
            Fila++;
        }

        printf("\n--------------------------------------------------------------------------------");
    }

    fclose(archi);
    posicionarCursor(24,Fila+6);
    system("PAUSE");

}



/// https://www.youtube.com/watch?v=otCpCn0l4Wo
void posicionarCursor(short int x, short int y)
{
    COORD cursorization;
    cursorization.X = x;
    cursorization.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),cursorization);
}

