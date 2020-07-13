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

void myCSS(void);
void seguirJugando();
int mode = UNDEFINED;

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
            shooting_state(&gamestate, newSocket);
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
    //console = GTK_WIDGET(gtk_builder_get_object(builder,"console"));
    box_insercion = GTK_WIDGET(gtk_builder_get_object(builder,"box_insercion"));
    btn_automatico = GTK_WIDGET(gtk_builder_get_object(builder,"btn_automatico"));
    btn_manual = GTK_WIDGET(gtk_builder_get_object(builder,"btn_manual"));
    
    
    //buffer = gtk_text_buffer_new (NULL);
    //gtk_text_buffer_set_text (buffer,
    //                      "Bienvenido a la batalla naval!\n",
    //                      31);
    //gtk_text_view_set_buffer(GTK_TEXT_VIEW(console),buffer);
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

void positionButtonCallback(GtkWidget *button,datos* data)
{
    printf("Apretaste un boton de posicion en %d-%d.\n",data->x,data->y);
    gtk_widget_set_name(button, "myButton_red");

}

void attackButtonCallback(GtkWidget *button,datos* data)
{
    printf("Apretaste un boton de ataque en %d-%d.\n",data->x,data->y);

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
        //Termino esta funcion
    }
}


//aqui se realizan las acciones correspondientes a un jugador que tiene que disparar
int shooting_state(Gamestate* gamestate, int socket)
{
    //obtenemos las coordenadas a donde disparar
    int coords[2];
    read_coords(coords);
    //me fijo si ya habia disparado en ese lugar
    while(gamestate->hisBoard[coords[0]][coords[1]] != UNKNOWN)
    {
        printf("Error: ya disparaste en las coordenadas [%d,%d]\n",coords[0],coords[1]);
        read_coords(coords);
    }
    int x = coords[0];
    int y = coords[1];

    //envio el disparo
    printf("Disparando...\n");
    send_shot(socket,x,y);

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

int play_game(int socket, Gamestate* gamestate)
{
    
    //gameloop
    //se juega hasta que ganes o pierdas
    //pid_t pid = fork();
    pthread_t hiloJuego;
    pthread_create(&hiloJuego,NULL,funcionJuego,(void*)1);//Creo un hilo auxiliar que maneje los semaforos
    while((gamestate->myState != WON) && (gamestate->myState != LOST))
    {
        //imprimo el estado del juego
        //if(!pid)
            //print_gamestate(gamestate);
        pintarBarcos(); //El problema con esto es que se traba esperando...
        //else{
        //while (gtk_events_pending ())
        //    gtk_main_iteration ();
        //me toca disparar

    //}
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
