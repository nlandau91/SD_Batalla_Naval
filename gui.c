#include <gtk/gtk.h>
#include <ctype.h>
#include <string.h>
#include <X11/Xlib.h>
#include "main.c"

GtkBuilder      *builder;
GtkWidget       *window;
GtkWidget       *inputIp,*inputPort;
GtkWidget       *btnAccept;
GtkWidget       *gamePanel,*startPanel;
GtkWidget       *positionButtons[10][10], *attackButtons[10][10];
GtkWidget       *positionGrid,*attackGrid;
GtkWidget       *console;
GtkTextBuffer   *buffer;
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


int mode = UNDEFINED;

int main(int argc, char *argv[])
{
    XInitThreads();
    gtk_init(&argc, &argv);

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
    console = GTK_WIDGET(gtk_builder_get_object(builder,"console"));
    buffer = gtk_text_buffer_new (NULL);
    gtk_text_buffer_set_text (buffer,
                          "Bienvenido a la batalla naval!\n",
                          31);
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(console),buffer);
    gtk_builder_connect_signals(builder, NULL);

    g_object_unref(builder);

    gtk_widget_show(window);
    gtk_main();

    return 0;
}

void positionButtonCallback(GtkWidget *button,datos* data)
{
    printf("Apretaste un boton de posicion en %d-%d.\n",data->x,data->y);

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


int auxCreateGame()
{
    int length = gtk_entry_get_text_length(GTK_ENTRY(inputPort)) ;
    if(length > 0)
    {
     char* portString = (char *)gtk_entry_get_text(GTK_ENTRY(inputPort));
     if(checkPort(portString,length))
     {
         int port = atoi(portString);
         printf("Creando juego.\n");
         printf("Puerto: %d.\n",port);
         char text[256] = "";
         char aux[256] ="";
         sprintf(aux, "Puerto: %d.\n",port);
         strcpy(text,"Partida creada.\n");
         strcat(text,aux);
         strcat(text,"Buscando oponente...");
         buffer = gtk_text_buffer_new (NULL);
         gtk_text_buffer_set_text (buffer,text,strlen(text));
         printf("EL MENSAJE QUE VOY A MOSTRAR ES: %s",text);
         gtk_text_view_set_buffer(GTK_TEXT_VIEW(console),buffer);
         pid_t pid = fork();
         if(!pid)
         {
             create_game(14550);
             printf("YA TERMINE");
         }

         return 1;
         //Deberia chequear que el puerto sea correcto.
         //Deberia esperar a que alguien se conecte
     }
    }
    return 0;
}

int auxJoinGame()
{
    int portLength = gtk_entry_get_text_length(GTK_ENTRY(inputPort));
    if(portLength > 0 && gtk_entry_get_text_length(GTK_ENTRY(inputIp)) > 0)
    {
        char* hostname = (char *)gtk_entry_get_text(GTK_ENTRY(inputIp));
        char* portString = (char *)gtk_entry_get_text(GTK_ENTRY(inputPort));
        int port = atoi(portString);
        if(checkPort(portString,portLength))
        {
            char text[256] = "";
            char aux[256] ="";
            sprintf(aux, "Puerto: %d.\n",port);
            strcpy(text,"Ingresando a partida...\n");
            strcat(text,aux);

            buffer = gtk_text_buffer_new (NULL);
            gtk_text_buffer_set_text (buffer,text,strlen(text));
            printf("EL MENSAJE QUE VOY A MOSTRAR ES: %s",text);
            gtk_text_view_set_buffer(GTK_TEXT_VIEW(console),buffer);
            //Deberia chequear que el puerto sea correcto.
            //Deberia esperar a que se conecte al host
             pid_t pid = fork();
             if(!pid)
             {
                 join_game("127.0.0.1",14550);
                 printf("YA TERMINE");
             }
            return 1;

        }
    }
    return 0;
}



void setConsoleText(char* text)
{
/*gtk_text_buffer_get_start_iter (buffer, &start);
gtk_text_buffer_get_end_iter (buffer, &end);*/
    GtkTextIter start,end;
    gtk_text_buffer_get_start_iter (buffer, &start);
    gtk_text_buffer_get_end_iter (buffer, &end);
    char* textoAnterior = gtk_text_buffer_get_text(buffer,&start,&end,0);
    buffer = gtk_text_buffer_new (NULL);
    strcat(textoAnterior,text);
    gtk_text_buffer_set_text (buffer,textoAnterior,strlen(textoAnterior));
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(console),buffer);

}


int actualizarTodo(gpointer data)
{
    printf("ENTRE A ACTUALIZAR A VER QUE MIERDA PASA.\n");
    setConsoleText("YA ACTUALICE LA CONCHA DE TU HERMANA.\n");
    return 0;
}


void startGame()
{
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
            gtk_grid_attach(GTK_GRID(positionGrid),positionButtons[i][j],i,j,1,1);
            gtk_grid_attach(GTK_GRID(attackGrid),attackButtons[i][j],i,j,1,1);
            gtk_widget_show(positionButtons[i][j]);
            gtk_widget_show(attackButtons[i][j]);
        }
    }
    gtk_widget_show(gamePanel);
    gtk_widget_hide(startPanel);
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
                          //setConsoleText("Esperando oponente...\n");
                          //pid = fork();
                          //if(!pid){
                            create_game(atoi(portString));
                            setConsoleText("Partida iniciada\n");
                            printf("DALE QUE VA.\n");

                            startGame();



                            //exit(0);
                           // }

                          break;
        case JOIN_GAME:   //setConsoleText("Uniendose a partida...\n");
                          printf("UNIENDOME.\n");
                          //hostname = (char *)gtk_entry_get_text(GTK_ENTRY(inputIp));
                          //portString = (char *)gtk_entry_get_text(GTK_ENTRY(inputPort));
                          join_game("127.0.0.1",atoi("14550"));
                          printf("DALE QUE VA.\n");
                          startGame();
                          //setConsoleText("Ya estas unido.\n");
                          start = 1;
                          break;
        case UNDEFINED:   printf("Ocurrió un error.\n");
                          exit(-1);
    }

    if(start)
    {

    }


}



// called when window is closed
void on_window_main_destroy()
{
    gtk_main_quit();
}
