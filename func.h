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
size_t ft_serach_cle(cbor_item_t *cbor_root,std::string by);
std::string evaluer_style(std::string style, std::string param);
void* update_element_verification(tinyxml2::XMLElement* parent,const char *target,const char *targetType, const char *by, cbor_item_t* cbor_root);
void getdriven(tinyxml2::XMLElement* parent, cbor_item_t* cbor_root);
bool checkAttributeValue(std::string attribute, std::string value);
bool checkAttributein(std::string attribute);
bool isinstyle(std::string stl, std::string att);
bool isgeofunction(std::string func);
bool iscompfunction(std::string func);
bool isarethfunction(std::string func);
bool islengthfunction(std::string func);
void animate_element_attribute_change(tinyxml2::XMLElement* parent, const char* target, float start_value, float end_value, int duration_ms); 
int evaluer_longueur(const std::string& expression, cbor_item_t* map);
void reload_svg();
void *udp_listener(void* );

#endif
