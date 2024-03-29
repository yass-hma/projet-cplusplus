#ifndef FUNC_HPP
#define FUNC_HPP
#define PORT 6789

#include <cairo/cairo.h>
#include <gtk/gtk.h>
#include <librsvg/rsvg.h>
#include <gio/gfile.h>
#include <tinyxml2.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <thread>
#include <netdb.h>
#include <cbor.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <stdio.h>
#include <stack>
#include <vector>
#include <cmath>
#include <sstream>
#include <chrono>
#include <string.h>

extern bool test;
extern bool error;
extern bool att;



extern tinyxml2::XMLElement* svg;
extern tinyxml2::XMLDocument svg_data;
extern GtkWidget *window;
extern GtkWidget *darea;

extern char *err;
extern char *file;
extern cbor_item_t *cbor_root;
extern RsvgHandle *svg_handle;
extern RsvgRectangle viewport;

template<typename T> T evaluer_comp_fon(const std::string& expression, cbor_item_t* map);
template<typename T> T evaluate_expression_cbor(const std::string& expression, cbor_item_t* map);
template<typename T> T evaluer_geo_fon(const std::string& expression, cbor_item_t* map);

size_t ft_serach_cle(cbor_item_t *,std::string );

std::string evaluer_style(std::string , std::string );

void* update_element_verification(tinyxml2::XMLElement* ,const char *,const char *, const char *, cbor_item_t* );
void getdriven(tinyxml2::XMLElement* , cbor_item_t* );

bool checkAttributeValue(std::string , std::string );
bool checkAttributein(std::string );
bool isinstyle(std::string , std::string );
bool isgeofunction(std::string );
bool iscompfunction(std::string );
bool isarethfunction(std::string );
bool islengthfunction(std::string );

void animate_element_attribute_change(tinyxml2::XMLElement* , const char* , float , float , int ); 
int evaluer_longueur(const std::string& , cbor_item_t* );
void reload_svg();
void *udp_listener(void* );

#endif
