#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <pthread.h>

//He añadido estas variables a globales ya que se me ha hecho imposible pasarlas por los hilos
int servidor_fd;
char mensaje[2000], mensaje_servidor[2000];

// Esta funcion trabaja sobre un hilo para escribir los mensajes y mandarlos al servidor
void* escribir(void*){
    while(1){
    fgets(mensaje, sizeof(mensaje), stdin);
    write(servidor_fd, mensaje, strlen(mensaje) + 1);
    mensaje == "";
  }
}

// Esta funcion lee los mensajes que recibe del servidor
void* leer(void*){
    while(1){
      read(servidor_fd, mensaje_servidor, sizeof(mensaje_servidor));
      if(mensaje_servidor != ""){
      puts(mensaje_servidor);
      mensaje_servidor == "";
    }
  }
}

int main(int argc, char const *argv[]) {

  //Declaramos al socket cliente
  struct sockaddr_un s_filedata;
  const char * const socket_name = argv[1];

  //Configuramos el socket
  servidor_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
  s_filedata.sun_family = AF_LOCAL;
  strcpy(s_filedata.sun_path, socket_name);

  //Conectamos al servidor el socket
  connect(servidor_fd, (struct sockaddr *)&s_filedata, SUN_LEN(&s_filedata));

  //Declaramos los hilos
  pthread_t escribir_t;
  pthread_t leer_t;

  //Creamos los hilos
  pthread_create(&escribir_t, NULL, &escribir, NULL);
  pthread_create(&leer_t, NULL, &leer, NULL);

  //Entramos en ellos
  pthread_join(escribir_t, NULL);
  pthread_join(leer_t, NULL);

  //Cerramos el socket
  close(servidor_fd);
  return 0;
}
