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
    printf("Ya recibi.\n");
    int i = 0;
    int j = 0;
    for(i = 0; i<10; i++)
    {
        for(j = 0; j<10; j++)
        {
            if(arr[i][j] == 1)
            {
                gtk_widget_set_name(positionButtons[i][j], "myButton_red");
            }
        }
    }
    
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
        //a jugar!
        //int res = jugar(newSocket, &gamestate);
        /*if(res == EXIT_FAILURE)
        {
            setConsoleText("Oops, hubo un problema...\n\n");
        }
        if(gamestate.myState == WON)
        {
            setConsoleText("Ganaste!\n\n");
        }
        if(gamestate.myState == LOST)
        {
            setConsoleText("Perdiste!\n\n");
        }*/
        //close(newSocket);
    }
}

void seguirJugando()
{
    //avisamos al oponente que ya estamos listos
    char ready = 'r';
    send(newSocket, &ready, 1, 0);
    //esperamos a que el oponente este listo
    read(newSocket, &ready, 1);
    printf("El oponente esta listo, asi que arranca el juego.\n");
    
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
            //setConsoleText("Partida iniciada\n");
            startGameAux(0);
            break;
        case JOIN_GAME:   
            hostname = (char *)gtk_entry_get_text(GTK_ENTRY(inputIp));
            portString = (char *)gtk_entry_get_text(GTK_ENTRY(inputPort));
            newSocket = join_game(hostname,atoi(portString));
            //setConsoleText("Partida iniciada.\n");
            startGameAux(1);
            break;
        case UNDEFINED:   
            //setConsoleText("Ocurrió un error.\n");
            exit(-1);
    }
}



// called when window is closed
void on_window_main_destroy()
{
    gtk_main_quit();
}
