#include <gtk/gtk.h>
    GtkBuilder      *builder; 
    GtkWidget       *window;
    GtkWidget       *inputIp,*inputPort;
    GtkWidget       *btnAccept;
    GtkWidget       *gamePanel,*startPanel;
    GtkWidget       *positionButtons[10][10], *attackButtons[10][10];
    GtkWidget       *positionGrid,*attackGrid;
    
    typedef struct datos
    {
        int x;
        int y;
    } datos;
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
    gtk_widget_show(inputPort);
    gtk_widget_hide(inputIp);
    gtk_widget_show(btnAccept);
    
}

void joinGame()
{
    gtk_widget_show(inputPort);
    gtk_widget_show(inputIp);
    gtk_widget_show(btnAccept);
}

void acceptGame()
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
            printf("toSend %d-%d.\n",toSend->x,toSend->y);
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



// called when window is closed
void on_window_main_destroy()
{
    gtk_main_quit();
}
