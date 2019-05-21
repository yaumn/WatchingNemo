#ifndef CLIENT_LISTENER_H
#define CLIENT_LISTENER_H

//init
void *create_client_listener(void *p);

//Commands
void hello();
void get_fishes();
void get_fishes_continuously();
void ls();
void ping();
void add_fish();
void del_fish();
void start_fish();

//Communication
//void send();

#endif
