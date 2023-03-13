#include <cairo/cairo.h>
#include <gtk/gtk.h>
#include <librsvg/rsvg.h>
#include <gio/gfile.h>
#include <tinyxml2.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>
#include <cbor.h>
#include <iostream>
#include <stdio.h>

#define PORT 6789

using namespace tinyxml2;

static void do_drawing(cairo_t *);
static void do_drawing_svg(cairo_t *);

RsvgHandle *svg_handle;
RsvgRectangle viewport;

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr,
    gpointer user_data)
{
  do_drawing_svg(cr);

  return FALSE;
}

static void do_drawing_svg(cairo_t * cr)
{
  rsvg_handle_render_document (svg_handle, cr, &viewport, NULL);
}

void *udp_listener(void *)
{
  int sockfd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_len;
  char buffer[1024];

  // Create socket
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);

  // ------------------------------- Bind socket to address -----------------------------------//
  
  bzero(&server_addr, sizeof(server_addr)); // supprimer les n byte commmence de le pointeur 
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr); // Use localhost
  bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  
  //-------------------------------------------------------------------------------------------//

  // Receive data in a loop
  while (1) {
    bzero(buffer, sizeof(buffer));
    client_len = sizeof(client_addr);
    ssize_t recev;
    ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_len);
    cbor_load_result result;
    cbor_item_t *cbor_root = cbor_load(cbor_data(buffer),n,&result);
        if (cbor_isa_map(cbor_root)) {
            size_t map_size = cbor_map_size(cbor_root);
            std::cout << "Received a CBOR map with " << map_size << " elements:\n";
            for (size_t i = 0; i < map_size; i++) {
                cbor_pair pair = cbor_map_handle(cbor_root)[i];
                if (cbor_isa_string(pair.key)) {
                    const uint8_t *key_str = cbor_string_handle(pair.key);
                    std::cout << "  " << key_str << ": ";
                } else {
                    std::cout << "  Invalid key type\n";
                }

                if (cbor_is_float(pair.value)) {
                    double val = cbor_float_get_float(pair.value);
                    std::cout << val << std::endl;
                } else {
                    std::cout << "Invalid value type\n";
                }
            }
        }
        cbor_decref(&cbor_root);
    }
        

  // Close socket
  close(sockfd);
}

// Fonction récursive pour chercher l'élément driven et récupérer ses attributs
void getdriven(XMLElement* parent){
  if (!parent) return;  // Vérification de sécurité pour s'assurer que le parent existe

  XMLElement* child = parent->FirstChildElement();
  while (child) {
    if (std::string(child->Name()) == "driven") {  // Vérification si l'élément est "driven"
      const char* target = child->Attribute("target");
      const char* by = child->Attribute("by");
      std::cout << "Found driven element: target=" << target << ", by=" << by << std::endl;
    }
    getdriven(child);  // Recherche récursive dans les enfants
    child = child->NextSiblingElement();
  }

}

int main(int argc, char *argv[])
{
  GtkWidget *window;
  GtkWidget *darea;

  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  darea = gtk_drawing_area_new();
  gtk_container_add(GTK_CONTAINER(window), darea);

  XMLDocument svg_data;
  svg_data.LoadFile("maison.svg");
  XMLElement* svg = svg_data.FirstChildElement("svg");
  getdriven(svg);
  
  XMLElement *rectangle1 = svg_data.FirstChildElement("svg")->FirstChildElement("g")->FirstChildElement("rect");
  rectangle1->SetAttribute("style", "fill:#000000");
  XMLPrinter printer;
  svg_data.Print(&printer);
  svg_handle = rsvg_handle_new_from_data ((const unsigned char*) printer.CStr(), printer.CStrSize(), NULL);
  rsvg_handle_get_geometry_for_element  (svg_handle, NULL, &viewport, NULL, NULL);

  // Create thread for network listener
  pthread_t listener_thread;
  pthread_create(&listener_thread, NULL, udp_listener, NULL);

  g_signal_connect(G_OBJECT(darea), "draw",
      G_CALLBACK(on_draw_event), NULL);
  g_signal_connect(window, "destroy",
      G_CALLBACK(gtk_main_quit), NULL);

  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), viewport.width, viewport.height);
  gtk_window_set_title(GTK_WINDOW(window), "GTK window");

  gtk_widget_show_all(window);
  gtk_main();
  

  return 0;
}
