#ifndef PARSER_H
#define PARSER_H


int parse_config_file(const char *filename, struct controller_config *config);
int parse_aquarium_file(const char *filename);


#endif // PARSER_H
