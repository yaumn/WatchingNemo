#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "aquarium.h"
#include "client_listener.h"
#include "fish.h"
#include "log.h"
#include "parser.h"
#include "queue.h"
#include "view.h"


#define MAX_BUFFER_SIZE 256


int terminate_program;
int sock;
struct controller_config c;


struct client
{
  int socket;
  char name[256];
  time_t time_of_last_action;
  int has_continuous_updates;
  pthread_t thread;
  int connected;
  char *ip;

  LIST_ENTRY(client) queue_entries;
};

LIST_HEAD(client_queue, client);

struct client_queue *clients;

void *client_thread(void *a);


void add_new_client(int socket, char *ip)
{
  struct client *client = malloc(sizeof(struct client));
  client->socket = socket;
  client->name[0] = '\0';
  client->time_of_last_action = time(NULL);
  client->has_continuous_updates = 0;
  client->connected = 1;
  client->ip = ip;

  LIST_INSERT_HEAD(clients, client, queue_entries);

  pthread_create(&client->thread, NULL, client_thread, client);
}


void disconnect_client(struct client *client)
{
  if (client->name[0] != '\0') {
    view_set_available(client->name, 1);
  }
  close(client->socket);
  client->connected = 0;
  printf("\nClient %d disconnected\n$ ", client->socket);
  fflush(stdout);
  log_write(1, "Client %d disconnected (%s) and socket %d closed", client->socket, client->ip, client->socket);
}


void destroy_listener()
{
  struct client *client = NULL;
  while (!LIST_EMPTY(clients)) {
    client = LIST_FIRST(clients);
    LIST_REMOVE(client, queue_entries);
    pthread_join(client->thread, NULL);
    if (client->connected) {
      disconnect_client(client);
    }
    free(client);
  }
  free(clients);
}


void get_fishes(struct client *client)
{
  char *info = fishes_get_info(view_find(client->name));
  const int length = strlen(info);
  send(client->socket, info, length, 0);
  info[length - 1] = '\0';
  log_write(2, "Sent message \"%s\" to client %d (%s)", info, client->socket, client->ip);
  free(info);
}


void handle_hello(char *message, struct client *client)
{
  if (strncmp(message, "hello in as ", 12) == 0) {
    if (view_set_available(message + 12, 0)) {
      strcpy(client->name, message + 12);
    } else {
      const char *name = view_find_available();
      if (name != NULL) {
	strcpy(client->name, name);
      }
    }
  } else if (strcmp(message, "hello") == 0) {
    const char *name = view_find_available();
    if (name != NULL) {
      strcpy(client->name, name);
    }
  }

  if (client->name[0] == '\0') {
    send(client->socket, "no greeting\n", 12, 0);
    log_write(2, "Sent message \"no greeting\" to client %d (%s)", client->socket, client->ip);
  } else {
    char response[MAX_BUFFER_SIZE];
    const int response_length = snprintf(response, MAX_BUFFER_SIZE - 1,
					 "greeting %s\n", client->name);
    send(client->socket, response, response_length, 0);
  response[response_length - 1] = '\0';
    log_write(2, "Sent message \"%s\" to client %d (%s)", response, client->socket, client->ip);
  }
}


void handle_add_fish(char *message, struct client *client)
{
  char name[MAX_BUFFER_SIZE], moving_algorithm[MAX_BUFFER_SIZE];
  int x, y, width, height;

  if (sscanf(message + 8, "%s at %dx%d, %dx%d, %s", name, &x, &y,
	     &width, &height, moving_algorithm) == 6) {
    struct view *view = view_find(client->name);

    if (fish_add(name, view->start.x + view->size.width * x / 100,
		 view->start.y + view->size.height * y / 100,
		 view->size.width * width / 100,
		 view->size.height * height / 100,
		 moving_algorithm)) {
      send(client->socket, "OK\n", 3, 0);
      log_write(2, "Sent message \"OK\" to client %d (%s)", client->socket, client->ip);
    } else {
      send(client->socket, "NOK\n", 4, 0);
      log_write(2, "Sent message \"NOK\" to client %d (%s)", client->socket, client->ip);
    }
  }
}


void parse_client_message(char *message, struct client *client)
{
  if (strcmp(message, "log out") == 0) {
    send(client->socket, "bye\n", 4, 0);
    log_write(2, "Sent message \"bye\" to client %d (%s)", client->socket, client->ip);
    disconnect_client(client);
    pthread_exit(NULL);
  } else if (strncmp(message, "ping ", 5) == 0) {
    char response[MAX_BUFFER_SIZE];
    const int response_length = snprintf(response, MAX_BUFFER_SIZE - 1,
					 "pong %s\n", message + 5);
    send(client->socket, response, response_length, 0);
    log_write(2, "Sent message \"%s\" to client %d (%s)", response, client->socket, client->ip);
    client->time_of_last_action = time(NULL);
  } else {
    if (!aquarium_is_loaded()) {
      send(client->socket, "Please wait until the controller has initialized the aquarium\n", 62, 0);
      log_write(2, "Sent message \"Please wait until the controller has initialized the aquarium\" to client %d (%s)",
		client->socket, client->ip);
      return;
    }


    if (strncmp(message, "hello", 5) == 0) {
      handle_hello(message, client);
    } else if (strcmp(message, "getFishes") == 0) {
      get_fishes(client);
    } else if (strcmp(message, "getFishesContinuously") == 0
	       || strcmp(message, "ls") == 0) {
      get_fishes(client);
      client->has_continuous_updates = 1;
    } else if (strncmp(message, "addFish ", 8) == 0) {
      handle_add_fish(message, client);
    } else if (strncmp(message, "delFish ", 8) == 0) {
      char name[MAX_BUFFER_SIZE];
      if (sscanf(message + 8, "%s", name) == 1) {
	if (fish_remove(name)) {
	  send(client->socket, "OK\n", 3, 0);
	  log_write(2, "Sent message \"OK\" to client %d (%s)", client->socket, client->ip);
	} else {
	  send(client->socket, "NOK\n", 4, 0);
	  log_write(2, "Sent message \"NOK\" to client %d (%s)", client->socket, client->ip);
	}
      }
    } else if (strncmp(message, "startFish ", 10) == 0) {
      char name[MAX_BUFFER_SIZE];
      if (sscanf(message + 10, "%s", name) == 1) {
	if (fish_start(name)) {
	  send(client->socket, "OK\n", 3, 0);
log_write(2, "Sent message \"OK\" to client %d (%s)", client->socket, client->ip);
	} else {
	  send(client->socket, "NOK\n", 4, 0);
log_write(2, "Sent message \"NOK\" to client %d (%s)", client->socket, client->ip);
	}
      }
    }
    if (client->name[0] == '\0') {
      send(client->socket, "You must choose a view before!\n", 31, 0);
      log_write(2, "Sent message \"You must choose a view before!\" to client %d (%s)",
      client->socket, client->ip);
      return;
    }
  }
}


void *client_thread(void *a)
{
  struct client *client = a;
  char buffer[MAX_BUFFER_SIZE];
  ssize_t nb_bytes;

  while (!terminate_program) {
    if (client->has_continuous_updates) {
      get_fishes(client);
      msleep(c.fish_update_interval);
    }    

    if (time(NULL) - client->time_of_last_action > c.display_timeout_value) {
      disconnect_client(client);
      break;
    }

    if ((nb_bytes = recv(client->socket, buffer, MAX_BUFFER_SIZE, 0)) == -1) {
      if (errno != EAGAIN) {
	perror("recv");
	if (errno == ECONNRESET) {
	  disconnect_client(client);
	  break;
	}
      }
    } else {
      if (nb_bytes == 0) {
	disconnect_client(client);
	break;
      } else {
	buffer[nb_bytes - 1] = '\0';
	log_write(2, "Received message \"%s\" from client %d (%s)", buffer, client->socket, client->ip);
	parse_client_message(buffer, client);
      }
      client->time_of_last_action = time(NULL);
    }
  }

  pthread_exit(NULL);
}


void *create_client_listener(void *p)
{
  (void)p;
  parse_config_file("controller.cfg", &c);

  const int port = c.port;
  int opt = 1;
  struct sockaddr_in sin;
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
		 &opt, sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  if (listen(sock, 5) == -1) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  log_write(1, "Opened listening socket on port %d", port);

  clients = malloc(sizeof(struct client_queue));
  LIST_INIT(clients);

  while (!terminate_program) {
    struct sockaddr_in client_sin;
    socklen_t len = sizeof(client_sin);
    const int client_sock = accept(sock, (struct sockaddr *)&client_sin, &len);
    if (client_sock == -1) {
      if (!terminate_program) {
	perror("accept");
      }
    } else {
      fcntl(client_sock, F_SETFL, O_NONBLOCK);
      char *ip = inet_ntoa(client_sin.sin_addr);
      printf("\nConnection established with client %s, id = %d\n$ ", ip, client_sock);
      fflush(stdout);
      log_write(1, "Connected to client %s with socket %d", ip, client_sock);
      add_new_client(client_sock, ip);
    }
  }

  destroy_listener();
  close(sock);

  pthread_exit(NULL);
}
