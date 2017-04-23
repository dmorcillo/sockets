#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <pthread.h>

static int id = 10;

// Tabla que guarda el numero de clientes conectados
typedef struct {
  int conexion;
  int id;
} client_t;

// Asigna el numero maximo de clientes
client_t *clientes[100];

// Añade un usuario a la tabla de clientes
void push(client_t *cl){
  int i;
  for(i=0;i<10;i++){
    if(!clientes[i]){
      clientes[i] = cl;
      return;
    }
  }
}

// Elimina un usuario de la tabla de clientes
void pop(int id){
  int i;
  for(i=0;i<10;i++){
    if(clientes[i]){
      if(clientes[i]->id == id){
        clientes[i] = NULL;
        return;
      }
    }
  }
}

// Esta funcion envia el mensaje recibido a todos los clientes de la tabla menos el que lo envia
void enviar_mensaje(char *s, int id){
  int i;
  for(i=0;i<10;i++){
    if(clientes[i]){
      if(clientes[i]->id != id){
        write(clientes[i]->conexion, s, strlen(s) + 1);
      }
    }
  }
}

// Esta funcion maneja las conexiones lee, y reenvia los mensajes
void* escribir(void *arg){
  int tamano;
  char mensaje[2000];

  client_t *cli = (client_t *)arg;

  // Lee el mensaje y llama a la funcion que envia los mensajes
  while ((tamano = read(cli->conexion, mensaje, sizeof(mensaje))) > 0){
    enviar_mensaje(mensaje, cli->id);
    mensaje == "";
  }

  //Cierra las conexiones
  close(cli->conexion);

  //Elimina al usuario de la tabla y libera la reserva
  pop(cli->id);
  free(cli);
  pthread_detach(pthread_self());
}

int main(int argc, char const *argv[]) {

  //Declaramos el socket, nombre del socket y los clientes
  const char * const nombre_socket = argv[1];
  int servidor_fd = 0, conexion = 0;
  struct sockaddr_un servidor;
  struct sockaddr_un cliente;
  pthread_t thread;

  //Añadimos las configuraciones
  servidor_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
  servidor.sun_family = AF_LOCAL;
  strcpy(servidor.sun_path, nombre_socket);

  //Unimos el servidor con la configuracion
  bind(servidor_fd, (struct sockaddr*)&servidor, sizeof(servidor));

  //Abrimos el socket para que acepte conexiones
  listen(servidor_fd, 10);

  //Este bucle se encarga de crear hilos para poder aceptar las conexiones entrantes de los clientes
  while(1){
    socklen_t tamanocliente = sizeof(cliente);
    conexion = accept(servidor_fd, (struct sockaddr*)&cliente, &tamanocliente);

    // configuraciones del cliente
    client_t *cli = (client_t *)malloc(sizeof(client_t));
    cli->conexion = conexion;
    cli->id = id++;

    // Añade un cliente a la tabla y crea un hilo
    push(cli);
    pthread_create(&thread, NULL, &escribir, (void*)cli);
  }

  return 0;
}
