#include <gtk/gtk.h>
#include <ctype.h>
#include <string.h>
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <time.h>
#include "battleship.h"
#include "connection.h"
#include "init.h"
#include "utils.h"
#include <pthread.h>
#include <semaphore.h>
 
GtkBuilder      *builder;
GtkWidget       *window;
GtkWidget       *inputIp,*inputPort;
GtkWidget       *btnAccept;
GtkWidget       *gamePanel,*startPanel;
GtkWidget       *positionButtons[10][10], *attackButtons[10][10];
GtkWidget       *positionGrid,*attackGrid;
GtkWidget       *console;
GtkTextBuffer   *buffer;
GtkWidget       *shipsContainer;
GtkWidget       *box_insercion;
GtkWidget       *btn_automatico,*btn_manual;
GtkWidget       *btnFrigate,*btnDestroyer,*btnBattleship,*btnCarrier;
GtkWidget       *btnOrientation;
GtkTextMark     *text_mark_end;

int frigateRestantes = 2;
int destroyerRestantes = 3;
int battleshipRestantes = 3;
int carrierRestantes = 1;
//0 frigate, 1 destroyer, 2 battleship, 3 carrier
int barcoActual = -1; 
//0 Horizontal, 1 Vertical
int orientacion = 0; 

sem_t mutex;
Gamestate gamestate;
int newSocket = -1;

typedef struct datos
{
    int x;
    int y;
} datos;

typedef enum gameModes
{
    CREATE_GAME,
    JOIN_GAME,
    UNDEFINED
} gameModes;

int x = 0;
int y = 0;
void myCSS(void);
void seguirJugando();
int mode = UNDEFINED;
int activarBotones = 0;
int ataqueEnemigo[3]={0,0,0};

void pintarBarcos();


int shooting_state(Gamestate* gamestate, int socket);
int waiting_state(Gamestate* gamestate, int socket);
void* funcionJuego(void* arg)
{
    while(((&gamestate)->myState != WON) && ((&gamestate)->myState != LOST))
    {
        //imprimo el estado del juego
        //if(!pid)
            //print_gamestate(gamestate);
        //pintarBarcos(); //El problema con esto es que se traba esperando...
        //else{
        //while (gtk_events_pending ())
        //    gtk_main_iteration ();
        //me toca disparar
        if((&gamestate)->myState == SHOOTING)
        {
            sem_wait(&mutex);
            activarBotones=1;
            sem_post(&mutex);
            shooting_state(&gamestate, newSocket);
            sem_wait(&mutex);
            activarBotones=0;
            sem_post(&mutex);
        }
        //me toca recibir un disparo
        else
        {
            waiting_state(&gamestate, newSocket);
        }
    //}
    }
}

int main(int argc, char *argv[])
{
    XInitThreads();
    gtk_init(&argc, &argv);
    myCSS();
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "gui.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    inputIp = GTK_WIDGET(gtk_builder_get_object(builder,"entryIp"));
    inputPort = GTK_WIDGET(gtk_builder_get_object(builder,"entryPort"));
    btnAccept = GTK_WIDGET(gtk_builder_get_object(builder,"buttonAccept"));
    gamePanel = GTK_WIDGET(gtk_builder_get_object(builder,"gamePanel"));
    startPanel = GTK_WIDGET(gtk_builder_get_object(builder,"startPanel"));
    positionGrid = GTK_WIDGET(gtk_builder_get_object(builder,"positionGrid"));
    attackGrid = GTK_WIDGET(gtk_builder_get_object(builder,"attackGrid"));
    shipsContainer = GTK_WIDGET(gtk_builder_get_object(builder,"shipsContainer"));
    console = GTK_WIDGET(gtk_builder_get_object(builder,"console"));
    box_insercion = GTK_WIDGET(gtk_builder_get_object(builder,"box_insercion"));
    btn_automatico = GTK_WIDGET(gtk_builder_get_object(builder,"btn_automatico"));
    btn_manual = GTK_WIDGET(gtk_builder_get_object(builder,"btn_manual"));
    btnFrigate = GTK_WIDGET(gtk_builder_get_object(builder,"btnFrigate"));
    btnDestroyer = GTK_WIDGET(gtk_builder_get_object(builder,"btnDestroyer"));
    btnBattleship = GTK_WIDGET(gtk_builder_get_object(builder,"btnBattleship"));
    btnCarrier = GTK_WIDGET(gtk_builder_get_object(builder,"btnCarrier"));
    btnOrientation = GTK_WIDGET(gtk_builder_get_object(builder,"btnOrientation"));
    
    sem_init(&mutex,0,1);
    
    buffer = gtk_text_buffer_new (NULL);
    gtk_text_buffer_set_text (buffer,
                          "Bienvenido a la batalla naval!\n",
                          31);
    GtkTextIter text_iter_end;
    gtk_text_buffer_get_end_iter (buffer, &text_iter_end);
    text_mark_end = gtk_text_buffer_create_mark (buffer,
                                               NULL,
                                               &text_iter_end,
                                               FALSE);
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(console),buffer);
    
    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);
    gtk_widget_show(window);
    
    gtk_main();

    return 0;
}

void myCSS(void){
    GtkCssProvider *provider;
    GdkDisplay *display;
    GdkScreen *screen;

    provider = gtk_css_provider_new ();
    display = gdk_display_get_default ();
    screen = gdk_display_get_default_screen (display);
    gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    const gchar *myCssFile = "style.css";
    GError *error = 0;

    gtk_css_provider_load_from_file(provider, g_file_new_for_path(myCssFile), &error);
    g_object_unref (provider);
}

void append_text(const gchar *text) {
  if (text) {
    /* get the text buffer */
    GtkTextBuffer *text_buffer;
    text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (console));

    /* get an end iter */
    GtkTextIter text_iter_end;
    gtk_text_buffer_get_end_iter (text_buffer, &text_iter_end);

    /* append text */
    gtk_text_buffer_insert (text_buffer, &text_iter_end, text, strlen(text));

    /* now scroll to the end using marker */
    gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW (console),
                                  text_mark_end,
                                  0., FALSE, 0., 0.);
  }
}


void frigateCallback()
{
    barcoActual = 0;    
}

void destroyerCallback()
{
    barcoActual = 1;
}

void battleshipCallback()
{
    barcoActual = 2;
}

void carrierCallback()
{
    barcoActual = 3;
}



void positionButtonCallback(GtkWidget *button,datos* data)
{
    printf("Apretaste un boton de posicion en %d-%d.\n",data->x,data->y);
    //gtk_widget_set_name(button, "myButton_red");
    //Veo la orientacion y el barco
    printf("Barco Actual: %d.\n",barcoActual);
    switch(barcoActual)
    {
        case 0: 
            //Voy a insertar una fragata
            printf("Fragatas restantes: %d.\n",frigateRestantes);
            if(!manualAddShipGUI(&gamestate,9-frigateRestantes,data->x,data->y,orientacion)){
                printf("pudo añadirse.\n");
                frigateRestantes--;
                if(frigateRestantes == 0)
                {
                    //Desactivo el boton
                    gtk_widget_set_sensitive (btnFrigate, FALSE);
                }
            }
            break;
        case 1:
            //Voy a insertar un destroyer
            printf("Destructores restantes: %d.\n",destroyerRestantes);
            if(!manualAddShipGUI(&gamestate,7-destroyerRestantes,data->x,data->y,orientacion)){
                printf("pudo añadirse.\n");
                destroyerRestantes--;
                if(destroyerRestantes == 0)
                {
                    //Desactivo el boton
                    gtk_widget_set_sensitive (btnDestroyer, FALSE);
                }
            }
            break;
        case 2:
            //Voy a ingresar un battleship
            printf("Battleship restantes: %d.\n",battleshipRestantes);
            if(!manualAddShipGUI(&gamestate,4-battleshipRestantes,data->x,data->y,orientacion))
            {
                printf("pudo añadirse.\n");
                battleshipRestantes--;
                if(battleshipRestantes == 0)
                {
                    //Desactivo el boton
                    gtk_widget_set_sensitive (btnBattleship, FALSE);
                } 
            }
            break;
        case 3:
            //Voy a ingresar un carrier
            printf("PortaAviones restantes: %d.\n",carrierRestantes);
            if(!manualAddShipGUI(&gamestate,0,data->x,data->y,orientacion))
            {
                printf("pudo añadirse.\n");
                carrierRestantes--;
                if(carrierRestantes == 0)
                {
                    //Desactivo el boton
                    gtk_widget_set_sensitive (btnCarrier, FALSE);
                } 
            }
            break;
    }
    barcoActual = -1;
    pintarBarcos();
    if(frigateRestantes == 0 && destroyerRestantes == 0 &&
        battleshipRestantes == 0 && carrierRestantes == 0)
        {
            //Debo empezar la partida
            gtk_widget_hide(shipsContainer);
            /*Debo deshabilitar todos los botones de posicion*/
            for(int i = 0; i < 10; i++)
            {
                for(int j = 0; j < 10; j++)
                {
                    gtk_widget_set_sensitive (positionButtons[i][j], FALSE);
                }
            }
            //Ahora que ya agregue naves sigo el flujo del programa.
            seguirJugando();
        }
}

void attackButtonCallback(GtkWidget *button,datos* data)
{
    printf("Apretaste un boton de ataque en %d-%d.\n",data->x,data->y);
        //obtenemos las coordenadas a donde disparar
    int coords[2];
    coords[0] = data->x;
    coords[1] = data->y;
        
    //me fijo si ya habia disparado en ese lugar
    while((&gamestate)->hisBoard[coords[0]][coords[1]] != UNKNOWN)
    {
        printf("Error: ya disparaste en las coordenadas [%d,%d]\n",coords[0],coords[1]);
        read_coords(coords);
    }
    x = coords[0];
    y = coords[1];

    //envio el disparo
    printf("Disparando...\n");
    send_shot(newSocket,x,y);

}

// called when window is closed
void createGame()
{
    mode = CREATE_GAME;
    gtk_widget_show(inputPort);
    gtk_widget_hide(inputIp);
    gtk_widget_show(btnAccept);

}

void joinGame()
{
    mode = JOIN_GAME;
    gtk_widget_show(inputPort);
    gtk_widget_show(inputIp);
    gtk_widget_show(btnAccept);
}

int checkPort(char* port,int length)
{
    char c;
    int i;
    for(i = 0; i < length; i++)
    {
        c = port[i];
        if(!isdigit(c))
            return 0;
    }
    return 1;
}

void pintarBarcos()
{
    int **arr= getGUIShips(&gamestate);
    int i = 0;
    int j = 0;
    for(i = 0; i<10; i++)
    {
        for(j = 0; j<10; j++)
        {
            if(arr[i][j] == 1)
            {
                gtk_widget_set_name(positionButtons[i][j], "myButton_white");
                gtk_button_set_label (GTK_BUTTON(positionButtons[i][j]), "S");
            }
            if(arr[i][j] == 2)
            {
                gtk_widget_set_name(positionButtons[i][j], "myButton_red");
                gtk_button_set_label (GTK_BUTTON(positionButtons[i][j]), "D");
            }
            if(arr[i][j] == 3)
            {
                gtk_widget_set_name(positionButtons[i][j], "myButton_blue");
                gtk_button_set_label (GTK_BUTTON(positionButtons[i][j]), "W");
            }
        }
    }
    while (gtk_events_pending ())
            gtk_main_iteration ();
    
}

void pintarAttackButtons()
{
    int **arr= getGUIAttacks(&gamestate);
    int i = 0;
    int j = 0;
    for(i = 0; i<10; i++)
    {
        for(j = 0; j<10; j++)
        {
            if(arr[i][j] == 1)
            {
                //gtk_widget_set_name(positionButtons[i][j], "myButton_white");
                gtk_button_set_label (GTK_BUTTON(attackButtons[i][j]), "?");
            }
            if(arr[i][j] == 2)
            {
                gtk_widget_set_name(attackButtons[i][j], "myButton_blue");
                gtk_button_set_label (GTK_BUTTON(attackButtons[i][j]), "W");
                gtk_widget_set_sensitive (attackButtons[i][j], FALSE);
            }
            if(arr[i][j] == 3)
            {
                gtk_widget_set_name(attackButtons[i][j], "myButton_white");
                gtk_button_set_label (GTK_BUTTON(attackButtons[i][j]), "S");
                gtk_widget_set_sensitive (attackButtons[i][j], FALSE);
            }
            if(arr[i][j] == 4)
            {
                gtk_widget_set_name(attackButtons[i][j], "myButton_red");
                gtk_button_set_label (GTK_BUTTON(attackButtons[i][j]), "D");
                gtk_widget_set_sensitive (attackButtons[i][j], FALSE);
            }
        }
    }
    while (gtk_events_pending ())
            gtk_main_iteration ();
}


void insertAutomatic()
{
    printf("Hice click en automatico.\n");
    autoaddships(&gamestate);
    //Tengo que pintar los botones correspondientes
    //Para indicar donde estan los barcos.
    pintarBarcos();
    
    //Tengo que quitar de la vista los botones estos.
    gtk_widget_hide(box_insercion);
    
    //Ahora que ya agregue naves sigo el flujo del programa.
    seguirJugando();
    
}

void insertManual()
{
    printf("Hice click en manual.\n");
    gtk_widget_hide(box_insercion);
    gtk_widget_show(shipsContainer);
    /*Debo habilitar todos los botones de posicion*/
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            gtk_widget_set_sensitive (positionButtons[i][j], TRUE);
        }
    }
}

void changeOrientation()
{
    orientacion = !orientacion;
    if(orientacion)
        gtk_button_set_label (GTK_BUTTON(btnOrientation), "Vertical");
    else
        gtk_button_set_label (GTK_BUTTON(btnOrientation), "Horizontal");
}


//void setConsoleText(char* text)
//{
/*gtk_text_buffer_get_start_iter (buffer, &start);
gtk_text_buffer_get_end_iter (buffer, &end);*/
//    GtkTextIter start,end;
//    gtk_text_buffer_get_start_iter (buffer, &start);
//    gtk_text_buffer_get_end_iter (buffer, &end);
//    char* textoAnterior = gtk_text_buffer_get_text(buffer,&start,&end,0);
//    buffer = gtk_text_buffer_new (NULL);
//    strcat(textoAnterior,text);
//    gtk_text_buffer_set_text (buffer,textoAnterior,strlen(textoAnterior));
//    gtk_text_view_set_buffer(GTK_TEXT_VIEW(console),buffer);
//
//}


int jugar(int socket, Gamestate* gamestate)
{
    return EXIT_SUCCESS;
}

int startGameAux(int mode)
{
    gtk_widget_hide(startPanel);
    printf("ESTOY CREANDO EL PANEL.\n");
    int i,j;
    for(i = 0; i<10; i++)
    {
        for(j = 0; j < 10; j++)
        {
            positionButtons[i][j] = gtk_button_new();
            attackButtons[i][j] = gtk_button_new();
            datos* toSend = malloc(sizeof(datos));
            toSend->x = i;
            toSend->y = j;
            g_signal_connect(positionButtons[i][j],"clicked",G_CALLBACK(positionButtonCallback),toSend);
            g_signal_connect(attackButtons[i][j],"clicked",G_CALLBACK(attackButtonCallback),toSend);
            gtk_widget_set_sensitive (positionButtons[i][j], FALSE);
            gtk_widget_set_sensitive (attackButtons[i][j], FALSE);
            gtk_button_set_label (GTK_BUTTON(positionButtons[i][j]), "?");
            gtk_button_set_label (GTK_BUTTON(attackButtons[i][j]), "?");
            gtk_grid_attach(GTK_GRID(positionGrid),positionButtons[i][j],i,j,1,1);
            gtk_grid_attach(GTK_GRID(attackGrid),attackButtons[i][j],i,j,1,1);
            gtk_widget_show(positionButtons[i][j]);
            gtk_widget_show(attackButtons[i][j]);
            
        }
    }
    //gtk_widget_show(shipsContainer);
    //
    printf("Ya termine de crear los botones.\n");
    gtk_widget_show(gamePanel);
    printf("Ahi puse el panel de juego\n");
    gtk_widget_show(box_insercion);
    printf("Ahi puse el panel de insercion.\n");
    
    
    if(mode == 0)
        gamestate.myState = SHOOTING;
    if(mode == 1)
        gamestate.myState = WAITING;
        
    if(newSocket < 0 )
    {
        printf("Error al crear o unirse a la partida...\n");
    }
    else
    {
        init_gamestate2(&gamestate);// Inicio el gamestate unicamente
        pintarBarcos();
        //Termino esta funcion
    }
}


//aqui se realizan las acciones correspondientes a un jugador que tiene que disparar
int shooting_state(Gamestate* gamestate, int socket)
{
    //espero la respuesta
    char receive_buffer[8] = {0};
    wait_shot_resp(socket,receive_buffer);

    //revisamos el resultado del disparo
    tile new_tile;
    if(receive_buffer[0] == MISS)
    {
        printf("AGUA!\n\n");
        gamestate->myState = WAITING;
        new_tile = WATER;
    }
    if(receive_buffer[0] == HIT)
    {
        printf("TOCADO!\n\n");
        new_tile = SHIP;
    }
    if(receive_buffer[0] == SUNK)
    {
        printf("HUNDIDO!\n\n");
        new_tile = DESTROYED;

        //informacion de la nave destruida
        //necesaria para actualizar el tablero
        int rcv_x = charToInt(receive_buffer[1]);
        int rcv_y = charToInt(receive_buffer[2]);
        int rcv_size = charToInt(receive_buffer[3]);
        orientation rcv_orientacion = charToInt(receive_buffer[4]);

        //destruimos la nave y actualizamos el tablero
        destroy_enemy_ship(gamestate, rcv_x, rcv_y, rcv_size, rcv_orientacion);

        //vemos si ganamos
        gamestate->myState = check_win(gamestate);

    }
    //actualizamos el tablero
    gamestate->hisBoard[x][y] = new_tile;
    return 0;
}

//aqui se realizan las acciones correspondientes a un jugador que esta esperando un disparo
int waiting_state(Gamestate* gamestate, int socket)
{
    //me toca recibir los disparos del oponente
    if(gamestate->myState == WAITING)
    {
        result res;
        char send_buffer[8] = {0};
        int argc = 0;

        printf("Esperando disparo del oponente...\n\n");
        char receive_buffer[8] = {0};
        receive_shot(socket,receive_buffer);
        int x = charToInt(receive_buffer[0]);
        int y = charToInt(receive_buffer[1]);
        printf("Recibido disparo en la posicion [%d,%d]\n\n",x,y);
        sem_wait(&mutex);
        ataqueEnemigo[0]=1;
        ataqueEnemigo[1]=x;
        ataqueEnemigo[2]=y;
        sem_post(&mutex);
        //evaluo el disparo
        res = check_hit(&gamestate->myBoard, x, y);
        if(res == MISS)
        {
            printf("AGUA!\n\n");
            gamestate->myState = SHOOTING;
            argc = 1;
        }
        if(res == HIT)
        {
            printf("TOCADO!\n\n");
            argc = 1;
            gamestate->myBoard[x][y]->hitsRemaining--;
            if(gamestate->myBoard[x][y]->hitsRemaining == 0)
            {
                res = SUNK;
            }
        }
        if(res == SUNK)
        {
            printf("HUNDIDO!\n\n");

            //vemos si perdimos
            gamestate->myState = check_loss(gamestate);

            //guardamos en el buffer informacion de la nave hundida
            send_buffer[1] = intToChar(gamestate->myBoard[x][y]->x);
            send_buffer[2] = intToChar(gamestate->myBoard[x][y]->y);
            send_buffer[3] = intToChar(gamestate->myBoard[x][y]->largo);
            send_buffer[4] = intToChar(gamestate->myBoard[x][y]->orientacion);
            argc = 5;
        }
        send_buffer[0] = res;
        //enviamos la respuesta del disparo al oponente
        respond_shot(socket, send_buffer, argc);
    }
    return 0;
}

void toggleButtons(int mode)
{
    printf("Voy a cambiar los botones a modo: %d.\n",mode);
    for(int i = 0; i<10; i++)
    {
        for(int j = 0; j<10; j++)
        {
            const char* cadena = gtk_button_get_label(GTK_BUTTON(attackButtons[i][j]));
            if(!strcmp(cadena, "?"))
                gtk_widget_set_sensitive (attackButtons[i][j], mode);
        }
    }
}


int play_game(int socket, Gamestate* gamestate)
{
    
    //gameloop
    //se juega hasta que ganes o pierdas
    //pid_t pid = fork();
    pthread_t hiloJuego;
    int ultimoModoBotones = 0;
    pthread_create(&hiloJuego,NULL,funcionJuego,(void*)1);//Creo un hilo auxiliar que maneje los semaforos
    while((gamestate->myState != WON) && (gamestate->myState != LOST))
    {
        //imprimo el estado del juego
        //if(!pid)
            //print_gamestate(gamestate);
        sem_wait(&mutex);
        if(ataqueEnemigo[0])
        {
            /*
            char str[3096]; //Le doy 3kb de tamaño, espero que alcance
            char str2[100];
            GtkTextIter start,end;
            gtk_text_buffer_get_start_iter (buffer, &start);
            gtk_text_buffer_get_end_iter (buffer, &end);
            
            //char* textoAnterior = gtk_text_buffer_get_text(buffer,&start,&end,0);
            sprintf(str2,"Ataque enemigo en (%d,%d).\n",ataqueEnemigo[1],ataqueEnemigo[2]);
            gtk_text_buffer_insert(buffer,&end,str2,strlen(str2));
            //strcpy(str,textoAnterior);
            //strcat(str,str2);
            /*gtk_text_buffer_set_text (buffer,
                          str,
                          strlen(str));
            gtk_text_view_set_buffer(GTK_TEXT_VIEW(console),buffer);*/
            char str2[100];
            sprintf(str2,"Ataque enemigo en (%d,%d).\n",ataqueEnemigo[1],ataqueEnemigo[2]);
            append_text(str2);
            ataqueEnemigo[0] = 0;
        }
        
        if(activarBotones != ultimoModoBotones)
        {
            ultimoModoBotones = activarBotones;
            toggleButtons(activarBotones);
        }
        sem_post(&mutex);
        pintarBarcos(); //El problema con esto es que se traba esperando...
        pintarAttackButtons();
    }
    return EXIT_SUCCESS;
}


void seguirJugando()
{
    //avisamos al oponente que ya estamos listos
    char ready = 'r';
    send(newSocket, &ready, 1, 0);
    //esperamos a que el oponente este listo
    read(newSocket, &ready, 1);
    printf("El oponente esta listo, asi que arranca el juego.\n");
    //a jugar!
    int res = play_game(newSocket, &gamestate);
    if(res == EXIT_FAILURE)
    {
        printf("Oops, hubo un problema...\n\n");
    }
    if(gamestate.myState == WON)
    {
        printf("Ganaste!\n\n");
    }
    if(gamestate.myState == LOST)
    {
        printf("Perdiste!\n\n");
    }
    close(newSocket);
}


char* portString,*hostname;
pid_t pid;
void acceptGame()
{
    int start = 0;
    switch(mode)
    {
        case CREATE_GAME:
            portString = (char *)gtk_entry_get_text(GTK_ENTRY(inputPort));
            newSocket = create_game(atoi(portString));
            gamestate.myState = SHOOTING;
            //setConsoleText("Partida iniciada\n");
            startGameAux(0);
            break;
        case JOIN_GAME:   
            hostname = (char *)gtk_entry_get_text(GTK_ENTRY(inputIp));
            portString = (char *)gtk_entry_get_text(GTK_ENTRY(inputPort));
            newSocket = join_game(hostname,atoi(portString));
            //setConsoleText("Partida iniciada.\n");
            gamestate.myState = WAITING;
            startGameAux(1);
            break;
        case UNDEFINED:   
            //setConsoleText("Ocurrió un error.\n");
            exit(-1);
    }
    //Termino la ejecucion del flujo.
    //Ahora continuara a partir del boton presionado para ubicar barcos
}



// called when window is closed
void on_window_main_destroy()
{
    gtk_main_quit();
}
