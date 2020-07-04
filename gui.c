#include <gtk/gtk.h>
#include <ctype.h>
#include "guiCommunication.c"
#include <string.h>


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
                          "Bienvenido a la batalla naval!",
                          30);
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
     char* portString = gtk_entry_get_text(GTK_ENTRY(inputPort));
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
            GUI_create_game(port);
         //Deberia chequear que el puerto sea correcto.
         //Deberia esperar a que alguien se conecte
         return 1;
     }
    }
    return 0;
}


void acceptGame()
{
    int start = 0;
    switch(mode)
    {
        case CREATE_GAME: if(auxCreateGame())
                            start = 1;
                          break;
        case JOIN_GAME:   if(gtk_entry_get_text_length(GTK_ENTRY(inputPort)) > 0 && gtk_entry_get_text_length(GTK_ENTRY(inputIp)) > 0)
                          {
                             char* hostname = gtk_entry_get_text(GTK_ENTRY(inputIp));
                             int port = atoi(gtk_entry_get_text(GTK_ENTRY(inputPort)));
                             char text[256] = "";
                             sprintf(text,"Uniendome a juego.\n");
                             sprintf(text,"Direccion IP: %s.\n",hostname);
                             sprintf(text,"Puerto: %d.\n",port);
                             gtk_text_buffer_set_text (buffer,text,256);
                             printf("Uniendome a juego.\n");
                             printf("Direccion IP: %s.\n",hostname);
                             //Deberia chequear que la ip sea correcta.
                             printf("Puerto: %d.\n",port);
                             //Deberia chequear que el puerto sea correcto.
                             //Deberia esperar a que se conecte al host
                             pid_t pid = fork();
                             if(!pid){
                                printf("Estoy en proceso hijo.\n");
                                GUI_join_game(hostname,port);
                                start = 1;
                             }
                             printf("Estoy en proceso padre.\n");

                          }
                          break;
        case UNDEFINED:   printf("Ocurrió un error.\n");
                          exit(-1);
    }

    if(start)
    {
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


}



// called when window is closed
void on_window_main_destroy()
{
    gtk_main_quit();
}
