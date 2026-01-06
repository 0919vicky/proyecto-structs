#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <algorithm> 

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif
using namespace std;

// ESTRUCTURAS DEL SISTEMA

// ENUMERACIONES
enum Bando { POLICIA_HONESTO, LADRON, CORRUPTO };
enum Estado { ACTIVO, CAPTURADO, INHABILITADO };
enum TipoPersonaje {
    INSPECTOR_JEFE, PERITO_FORENSE, OFICIAL_ASALTO,
    ANALISTA_DATOS, NEGOCIADOR, POLICIA_CORRUPTO,
    CEREBRO, FANTASMA, MULO, SABOTEADOR, HACKER, LOBERO
};

enum TipoItem {
    SCANNER_METALES, KIT_HUELLAS, ORDEN_ALLOGRAMIENTO,
    CHALECO_ANTIBALAS, GRABADORA_ENCUBIERTA, GANAS_MANOS,
    SENUELO_COMUNICACIONES, LLAVES_MANIPULADAS, FONDO_NEGRO,
    OTRO10, OTRO11, OTRO12, OTRO13, OTRO14, OTRO15,
    OTRO16, OTRO17, OTRO18, OTRO19, OTRO20
};

// ESTRUCTURAS PARA EL MAPA (GRAFO)
struct ConexionMapa {
    int idDestino;
    int distancia;
    ConexionMapa* siguiente;
};

struct Estacion {
    int id;
    string nombre;
    int oro;            // 0-3 lingotes
    bool tieneCivil;
    bool revelada;
    bool investigada;
    ConexionMapa* conexiones;
    Estacion* siguiente;
};

// ESTRUCTURAS ORIGINALES
struct Habilidad {
    string nombre;
    string descripcion;
    int cooldown;
    int cooldownActual;
    bool disponible;
};

struct Item {
    int id;
    string nombre;
    string descripcion;
    TipoItem tipo;
    int durabilidadMaxima;
    int usosRestantes;
    string efecto;
    bool disponible;
};

struct NodoItemMochila {
    Item* item;
    int usosActuales;
    NodoItemMochila* siguiente;
};

struct Personaje {
    int id;
    string nombre;
    string descripcion;
    TipoPersonaje tipo;
    Bando bando;
    Estado estado;
    int puntosVidaMax;
    int puntosVida;
    int capacidadOro;
    int cargaOro;
    int posicion;
    bool reveladoComoCorrupto;

    Habilidad habilidades[5];
    int numHabilidades;
    NodoItemMochila* inventario;
    Personaje* siguiente_global;
    Personaje* siguiente_equipo;
};

struct Equipo {
    string nombre;
    Personaje* cabeza;
    int cantidadMiembros;
};

struct ListaPersonajes {
    Personaje* cabeza;
    int contadorId;
};

struct NodoItemGlobal {
    Item* item;
    NodoItemGlobal* siguiente;
};

struct ListaItems {
    NodoItemGlobal* cabeza;
    int contadorId;
};

struct GrafoMapa {
    Estacion* estaciones;
    int totalEstaciones;
    int totalOro;
};

struct Juego {
    Equipo equipos[3];
    int oroTotal;
    int oroRecuperado;
    int ladronesCapturados;
    int corruptosDescubiertos;
    int rondaActual;
    bool juegoTerminado;
    GrafoMapa mapa;
    string logBitacora;
};

// PROTOTIPOS DE FUNCIONES
int leerEntero(int minVal, int maxVal);
void inicializarHabilidad(Habilidad* hab, const char* n, const char* d, int cd);
void inicializarHabilidadesPersonaje(Personaje* personaje);
void agregarItem(ListaItems* lista, Item* item);
void agregarPersonaje(ListaPersonajes* lista, Personaje* p);
void inicializarItemsBase(ListaItems* listaItems);
Personaje* crearPersonaje(const char* n, const char* d, TipoPersonaje t, Bando b, int cap);
void crearPersonajesBase(ListaPersonajes* listaPersonajes);
Personaje* buscarPorId(ListaPersonajes* lista, int id);
Personaje* buscarPorNombre(ListaPersonajes* lista, string nombre);
void mostrarHabilidadesPersonaje(Personaje* personaje);
void mostrarListaPersonajes(ListaPersonajes* lista);
void limpiarMochilaCompletamente(Personaje* personaje);
void eliminarPersonaje(ListaPersonajes* lista, int id);
Item* buscarItemPorId(ListaItems* lista, int id);
void mostrarItemsPorNombre(ListaItems* lista, string nombre);
bool eliminarItem(ListaItems* lista, int id);
void mostrarListaItems(ListaItems* lista);
void inicializarEquipos(Juego* juego);
void agregarPersonajeAEquipo(Juego* juego, Personaje* personaje);
void reorganizarEquipos(Juego* juego, ListaPersonajes* listaPersonajes);
void mostrarEquipos(Juego* juego);
void mostrarMochilaPersonaje(Personaje* personaje);
void agregarItemAMochila(Personaje* personaje, Item* item, ListaPersonajes* listaPersonajes);
void usarItemEnMochila(Personaje* personaje, int idItem);
NodoItemMochila* buscarItemEnMochilaPorId(Personaje* personaje, int idItem);
void eliminarItemDeMochila(Personaje* personaje, int idItem);
bool itemEstaEnAlgunaMochila(ListaPersonajes* listaPersonajes, int idItem);
void menuGestionPersonajes(ListaPersonajes* listaPersonajes);
void menuGestionItems(ListaItems* listaItems);
void menuGestionEquipos(Juego* juego, ListaPersonajes* listaPersonajes);
void menuGestionMochilas(ListaPersonajes* listaPersonajes, ListaItems* listaItems);
void mostrarMenuPrincipal();
void bubbleSortPersonajes(ListaPersonajes* lista, bool porIniciativa);

// NUEVAS FUNCIONES PARA MAPA Y JUEGO
bool archivoExiste(const string& nombre);
string obtenerRutaBase();
void crearArchivosPorDefecto();
void cargarMapaDesdeArchivo(GrafoMapa* mapa, const string& archivo);
Estacion* buscarEstacionPorId(Estacion* lista, int id);
void mostrarMapa(GrafoMapa* mapa);
void cargarAccesoriosDesdeArchivo(ListaItems* listaItems, const string& archivo);
void cargarPersonajesDesdeArchivo(ListaPersonajes* listaPersonajes, ListaItems* listaItems, const string& archivo);
void inicializarPartida(Juego* juego, ListaPersonajes* listaPersonajes);

void verificarVictoria(Juego* juego);
void guardarBitacora(Juego* juego, const string& accion);
void mostrarEstadoJuego(Juego* juego);
void menuJugarPartida(Juego* juego, ListaPersonajes* listaPersonajes);

//FUNCIONES AUXILIARES 

bool esMovimientoValido(Estacion* estacionActual, int idDestino) {
    if (!estacionActual || !estacionActual->conexiones) return false;
    
    ConexionMapa* temp = estacionActual->conexiones;
    while (temp != nullptr) {
        if (temp->idDestino == idDestino) {
            return true; 
        }
        temp = temp->siguiente;
    }
    return false;
}

string limpiarString(string s) {
    if (!s.empty() && s[s.length()-1] == '\r') {
        s.erase(s.length()-1);
    }
    return s;
}

int leerEntero(int minVal, int maxVal) {
    int val;
    while (true) {
        if (!(cin >> val)) {
            cout << "Entrada inválida. Intenta de nuevo: ";
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        if (val < minVal || val > maxVal) {
            cout << "Valor fuera de rango (" << minVal << "-" << maxVal << "). Intenta de nuevo: ";
            continue;
        }
        cin.ignore(10000, '\n');
        return val;
    }
}

bool archivoExiste(const string& nombre) {
    ifstream archivo(nombre);
    bool existe = archivo.is_open();
    archivo.close();
    return existe;
}

string obtenerRutaBase() {
#ifdef _WIN32
    char buffer[256];
    GetCurrentDirectoryA(256, buffer);
    return string(buffer) + "\\";
#else
    char buffer[256];
    getcwd(buffer, 256);
    return string(buffer) + "/";
#endif
}

void crearArchivosPorDefecto() {
    string ruta = obtenerRutaBase();
    cout << "Directorio actual: " << ruta << endl;
    
    // Crear accesorios.tren si no existe
    if (!archivoExiste("accesorios.tren")) {
        cout << "Creando archivo accesorios.tren por defecto..." << endl;
        ofstream archivo("accesorios.tren");
        if (archivo.is_open()) {
            archivo << "12\n---\n1\nLadron\nBomba de Humo\nFuncion: Movilidad/Evasion\nUso: 1\nAlcance: 0\n---\n";
            archivo << "2\nLadron\nJamelgo de Distraccion\nFuncion: Movilidad/Evasion\nUso: 1\nAlcance: 2\n---\n";
            archivo << "3\nPolicia\nPatines Electricos\nFuncion: Movilidad/Evasion\nUso: 3\nAlcance: 1\n---\n";
            archivo << "4\nLadron\nLlave Maestra\nFuncion: Interaccion/Control\nUso: 3\nAlcance: 0\n---\n";
            archivo << "5\nLadron\nCable de Extraccion\nFuncion: Interaccion/Control\nUso: 2\nAlcance: 1\n---\n";
            archivo << "6\nPolicia\nInmovilizador Sonico\nFuncion: Interaccion/Control\nUso: 1\nAlcance: 1\n---\n";
            archivo << "7\nLadron\nSensor de Calor\nFuncion: Busqueda/Vigilancia\nUso: 2\nAlcance: 3\n---\n";
            archivo << "8\nPolicia\nDrone de Rastreo\nFuncion: Busqueda/Vigilancia\nUso: 1\nAlcance: 6\n---\n";
            archivo << "9\nPolicia\nLinterna UV Forense\nFuncion: Busqueda/Vigilancia\nUso: 3\nAlcance: 0\n---\n";
            archivo << "10\nLadron\nChaqueta Reforzada\nFuncion: Asalto/Defensa\nUso: 2\nAlcance: 0\n---\n";
            archivo << "11\nPolicia\nEscudo Tactico\nFuncion: Asalto/Defensa\nUso: 2\nAlcance: 0\n---\n";
            archivo << "12\nPolicia\nEstimulante de Combate\nFuncion: Asalto/Defensa\nUso: 1\nAlcance: 0\n---\n";
            archivo.close();
            cout << "✓ accesorios.tren creado" << endl;
        }
    }
    
    // Crear personajes.tren si no existe
    if (!archivoExiste("personajes.tren")) {
        cout << "Creando archivo personajes.tren por defecto..." << endl;
        ofstream archivo("personajes.tren");
        if (archivo.is_open()) {
            archivo << "12\n---\n1\nPolicia\nDiana \"Sombra\" Vega\n3|6\n---\n";
            archivo << "2\nPolicia\nComisaria \"Centinela\" Rostova\n11|12|8\n---\n";
            archivo << "3\nPolicia\nCabo \"Fiasco\" Rico\n-\n---\n";
            archivo << "4\nPolicia\nDetective \"Lince\" Navarro\n9|8|6|3\n---\n";
            archivo << "5\nPolicia\nOficial \"Roca\" Tytus\n11\n---\n";
            archivo << "6\nPolicia\nAnalista \"Oracle\" Chen\n8|9\n---\n";
            archivo << "7\nLadron\nSaboteadora \"La Musa\"\n1|4\n---\n";
            archivo << "8\nLadron\nHacker \"Fantasma\" Zero\n7|5\n---\n";
            archivo << "9\nLadron\nMulo \"El Peso\" Hulk\n10\n---\n";
            archivo << "10\nLadron\nCerebro \"Arquitecta\" Volkov\n4|7|1|5\n---\n";
            archivo << "11\nLadron\nDistractor \"El Duende\" Loki\n2|1\n---\n";
            archivo << "12\nLadron\nAmo del Disfraz \"Camaleon\" Jax\n-\n---\n";
            archivo.close();
            cout << "✓ personajes.tren creado" << endl;
        }
    }
    
    // CREAR EL MAPA GRANDE DE 30 ESTACIONES SI NO EXISTE
    if (!archivoExiste("Mapa.tren")) {
        cout << "AVISO: No se encontró 'Mapa.tren'. Generando mapa completo de 30 estaciones..." << endl;
        ofstream archivo("Mapa.tren");
        if (archivo.is_open()) {
            archivo << "30\n";
            
            archivo << "---\n1\nIturbide\n-\n5:3|12:6|29:1|18:4\n";
            archivo << "---\n2\nDel Sol\n-\n1:5|16:2|27:4|30:1|8:3\n";
            archivo << "---\n3\nPlaza de Armas\n-\n6:6|10:1|15:3|21:5\n";
            archivo << "---\n4\nO'Higgins\n-\n5:2|13:5|18:3|21:6|22:1\n";
            archivo << "---\n5\nArtigas\n-\n3:1|7:4|19:6|25:2\n";
            archivo << "---\n6\nSan Martin\n-\n1:3|11:5|22:1|14:4\n";
            archivo << "---\n7\nBolivar\n-\n2:6|14:1|28:5|26:2\n";
            archivo << "---\n8\nRoraima\n-\n5:3|9:6|17:1|11:4\n";
            archivo << "---\n9\nTorres del Paine\n-\n10:4|24:2|26:6\n";
            archivo << "---\n10\nAngostura\n-\n12:5|16:3|20:2|3:1\n";
            archivo << "---\n11\nFormosa\n-\n7:4|15:6|23:2|2:5\n";
            archivo << "---\n12\nToledo\n-\n8:1|14:3|24:5|17:6\n";
            archivo << "---\n13\nKomsomolskaya\n-\n4:2|11:6|20:3|29:5\n";
            archivo << "---\n14\nAbrantes\n-\n3:5|10:4|18:1|28:6\n";
            archivo << "---\n15\nGrand Central Terminal\n-\n6:2|17:5|25:3|23:4\n";
            archivo << "---\n16\nArts et Métiers\n-\n1:4|13:1|22:6|7:3\n";
            archivo << "---\n17\nKungsträdgården\n-\n2:3|19:4|27:5|10:2\n";
            archivo << "---\n18\nMünchner Freiheit\n-\n3:6|15:2|26:1|30:4|4:5\n";
            archivo << "---\n19\nWestfriedhof\n-\n4:5|8:2|16:6|21:1\n";
            archivo << "---\n20\nBilbao\n-\n7:3|12:4|23:1|29:5\n";
            archivo << "---\n21\nShanghai\n-\n1:6|9:5|14:2|28:4\n";
            archivo << "---\n22\nT-Centralen\n-\n5:1|17:3|20:5|25:2\n";
            archivo << "---\n23\nAtocha\n-\n6:4|18:6|29:3|15:1\n";
            archivo << "---\n24\nLisboa\n-\n10:5|13:1|22:3|30:6\n";
            archivo << "---\n25\nSingapur\n-\n8:4|11:2|21:6|27:3\n";
            archivo << "---\n26\nPlaza Venezuela\n-\n7:2|19:3|28:5|12:1\n";
            archivo << "---\n27\nSolna Centrum\n-\n12:6|15:1|24:4|9:5\n";
            archivo << "---\n28\nWestminster\n-\n9:3|16:5|23:2|1:4\n";
            archivo << "---\n29\nAlameda\n-\n2:6|4:1|20:4|27:3|5:5\n";
            archivo << "---\n30\nAvtovo\n-\n5:4|13:2|19:1|26:5|24:3\n";
            
            archivo << "---\n"; // Fin del archivo
            archivo.close();
            cout << "✓ Mapa.tren creado con 30 estaciones." << endl;
        }
    }
}

//  INICIALIZACIÓN DE HABILIDADES 
void inicializarHabilidad(Habilidad* hab, const char* n, const char* d, int cd) {
    hab->nombre = n;
    hab->descripcion = d;
    hab->cooldown = cd;
    hab->cooldownActual = 0;
    hab->disponible = true;
}

void inicializarHabilidadesPersonaje(Personaje* personaje) {
    personaje->numHabilidades = 0;
    
    switch (personaje->tipo) {
        case INSPECTOR_JEFE:
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Investigacion Avanzada", "Revela 2 pistas en lugar de 1", 2);
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Intuicion Incorruptible", "Detecta si un policia en la misma estacion es corrupto", 3);
            break;
            
        case PERITO_FORENSE:
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Analisis Forense", "Revela informacion adicional de las pistas", 2);
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Recoleccion Evidencias", "Recoge huellas y pruebas digitales cruciales", 2);
            break;
            
        case OFICIAL_ASALTO:
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Captura Eficaz", "Mayor probabilidad de éxito en capturas", 1);
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Operativo Coordinado", "Coordina multiple capturas en misma zona", 3);
            break;
            
        default:
            break;
    }
}

// INICIALIZACIÓN DE ITEMS BASE 
void inicializarItemsBase(ListaItems* listaItems) {
    if (listaItems->cabeza == nullptr) listaItems->contadorId = 1;

    agregarItem(listaItems, new Item{0, "Scanner de Metales", 
        "Permite rastrear oro en un radio de 2 estaciones",
        SCANNER_METALES, 6, 6, "Rastrear oro en área amplia", true});
        
    agregarItem(listaItems, new Item{0, "Kit de Huellas Dactilares",
        "Mejora la acción Investigar, garantizando encontrar pista crucial",
        KIT_HUELLAS, 6, 6, "Investigación garantizada", true});
        
    agregarItem(listaItems, new Item{0, "Orden de Allanamiento",
        "Permite Investigar o Capturar en una estación adyacente sin moverse",
        ORDEN_ALLOGRAMIENTO, 6, 6, "Acción a distancia", true});
        
    agregarItem(listaItems, new Item{0, "Chaleco Antibalas",
        "Segunda oportunidad al fallar desafío de captura/escape",
        CHALECO_ANTIBALAS, 6, 6, "Protección en combate", true});
        
    agregarItem(listaItems, new Item{0, "Grabadora Encubierta",
        "Si graba a un corrupto usando habilidad ilegal, lo captura al instante",
        GRABADORA_ENCUBIERTA, 6, 6, "Contra corrupción", true});
    
    agregarItem(listaItems, new Item{0, "Ganas de Manos",
        "Aumenta temporalmente la capacidad de carga de oro",
        GANAS_MANOS, 6, 6, "Más capacidad de oro", true});
        
    agregarItem(listaItems, new Item{0, "Señuelo de Comunicaciones",
        "Crea falsa alerta de ladrón detectado en mapa de policías",
        SENUELO_COMUNICACIONES, 6, 6, "Distracción", true});
        
    agregarItem(listaItems, new Item{0, "Llaves Manipuladas",
        "Abrir pasaje secreto, teletransportándose a cualquier estación conectada",
        LLAVES_MANIPULADAS, 6, 6, "Teletransporte", true});
    
    agregarItem(listaItems, new Item{0, "Fondo Negro",
        "Soborno potente que permite Sabotear de forma remota",
        FONDO_NEGRO, 6, 6, "Soborno avanzado", true});
}

// CREACIÓN DE PERSONAJES
Personaje* crearPersonaje(const char* n, const char* d, TipoPersonaje t, Bando b, int cap) {
    Personaje* nuevo = new Personaje;
    nuevo->id = 0;
    nuevo->nombre = n;
    nuevo->descripcion = d;
    nuevo->tipo = t;
    nuevo->bando = b;
    nuevo->estado = ACTIVO;
    nuevo->puntosVidaMax = 5;
    nuevo->puntosVida = 5;
    nuevo->capacidadOro = cap;
    nuevo->cargaOro = 0;
    nuevo->posicion = 0;
    nuevo->reveladoComoCorrupto = false;
    nuevo->inventario = nullptr;
    nuevo->siguiente_global = nullptr;
    nuevo->siguiente_equipo = nullptr;
    nuevo->numHabilidades = 0;
    
    switch (t) {
        case MULO: 
            nuevo->puntosVidaMax = 15;
            break;
        case CEREBRO: 
            nuevo->puntosVidaMax = 8;
            break;
        case OFICIAL_ASALTO:
            nuevo->puntosVidaMax = 12;
            break;
        default:
            nuevo->puntosVidaMax = 5;
    }
    nuevo->puntosVida = nuevo->puntosVidaMax;
    
    inicializarHabilidadesPersonaje(nuevo);
    return nuevo;
}

void crearPersonajesBase(ListaPersonajes* listaPersonajes) {
    agregarPersonaje(listaPersonajes, crearPersonaje("Inspector Kaine",
        "La ley no es una guia flexible, es el faro que nos impide perdernos en la oscuridad",
        INSPECTOR_JEFE, POLICIA_HONESTO, 5));
    
    agregarPersonaje(listaPersonajes, crearPersonaje("Perito Rodriguez",
        "Especialista en analisis forense y recoleccion de evidencias",
        PERITO_FORENSE, POLICIA_HONESTO, 3));

    agregarPersonaje(listaPersonajes, crearPersonaje("Oficial Martinez",
        "Experto en operativos de captura y enfrentamientos",
        OFICIAL_ASALTO, POLICIA_HONESTO, 3));

    agregarPersonaje(listaPersonajes, crearPersonaje("Analista Costa",
        "Especialista en patrones de comportamiento y datos",
        ANALISTA_DATOS, POLICIA_HONESTO, 2));

    agregarPersonaje(listaPersonajes, crearPersonaje("Negociador Reyes",
        "Experto en interrogatorios y extraccion de informacion",
        NEGOCIADOR, POLICIA_HONESTO, 2));

    agregarPersonaje(listaPersonajes, crearPersonaje("Silas Vance",
        "El oro no es el botin, es la llave para comprar la libertad",
        CEREBRO, LADRON, 8));

    agregarPersonaje(listaPersonajes, crearPersonaje("El Fantasma",
        "Se desvanece en las sombras, dejando solo recuerdos",
        FANTASMA, LADRON, 4));

    agregarPersonaje(listaPersonajes, crearPersonaje("El Mulo",
        "Carga con el peso del botin, silencioso y eficiente",
        MULO, LADRON, 15));

    agregarPersonaje(listaPersonajes, crearPersonaje("Saboteador Nero",
        "Donde pasa, solo quedan problemas para los demas",
        SABOTEADOR, LADRON, 5));

    agregarPersonaje(listaPersonajes, crearPersonaje("Hacker Cipher",
        "Las paredes digitales son solo sugerencias",
        HACKER, LADRON, 4));

    agregarPersonaje(listaPersonajes, crearPersonaje("Lobero Grimm",
        "Confunde, distrae y siempre sale ganando",
        LOBERO, LADRON, 5));

    agregarPersonaje(listaPersonajes, crearPersonaje("Agente Smith",
        "Siempre en el lugar correcto en el momento correcto... demasiado correcto?",
        POLICIA_CORRUPTO, CORRUPTO, 4));
}

//  GESTIÓN DE LISTA DE PERSONAJES 
void agregarPersonaje(ListaPersonajes* lista, Personaje* p) {
    if(p->id == 0) {
        p->id = lista->contadorId++;
    } else {
        if(p->id >= lista->contadorId) {
            lista->contadorId = p->id + 1;
        }
    }
    p->siguiente_global = lista->cabeza;
    lista->cabeza = p;
}

Personaje* buscarPorId(ListaPersonajes* lista, int id) {
    Personaje* actual = lista->cabeza;
    while (actual != nullptr) {
        if (actual->id == id) return actual;
        actual = actual->siguiente_global;
    }
    return nullptr;
}

Personaje* buscarPorNombre(ListaPersonajes* lista, string nombre) {
    Personaje* actual = lista->cabeza;
    while (actual != nullptr) {
        if (actual->nombre == nombre) return actual;
        if (actual->nombre.find(nombre) != string::npos) return actual;
        actual = actual->siguiente_global;
    }
    return nullptr;
}

void mostrarHabilidadesPersonaje(Personaje* personaje) {
    cout << "  Habilidades:" << endl;
    for (int i = 0; i < personaje->numHabilidades; i++) {
        cout << "    - " << personaje->habilidades[i].nombre;
        cout << " (CD: " << personaje->habilidades[i].cooldown << ")";
        cout << ": " << personaje->habilidades[i].descripcion << endl;
    }
}

void mostrarListaPersonajes(ListaPersonajes* lista) {
    Personaje* actual = lista->cabeza;
    int contador = 1;
    cout << "==============================================================" << endl;
    cout << "                       LISTA DE PERSONAJES                      " << endl;
    cout << "==============================================================" << endl;

    while (actual != nullptr) {
        cout << "[" << contador << "] " << actual->nombre << " (ID: " << actual->id << ")" << endl;
        cout << "  Tipo: ";
        switch (actual->tipo) {
            case INSPECTOR_JEFE:  cout << "Inspector Jefe"; break;
            case PERITO_FORENSE:  cout << "Perito Forense"; break;
            case OFICIAL_ASALTO:  cout << "Oficial de Asalto"; break;
            case ANALISTA_DATOS:  cout << "Analista de Datos"; break;
            case NEGOCIADOR:  cout << "Negociador"; break;
            case POLICIA_CORRUPTO:  cout << "Policia Corrupto"; break;
            case CEREBRO:  cout << "Cerebro"; break;
            case FANTASMA:  cout << "Fantasma"; break;
            case MULO:  cout << "Mulo"; break;
            case SABOTEADOR:  cout << "Saboteador"; break;
            case HACKER:  cout << "Hacker"; break;
            case LOBERO:  cout << "Lobero"; break;
        }
        cout << " | Bando: ";
        switch (actual->bando) {
            case POLICIA_HONESTO:  cout << "Policia Honesto"; break;
            case LADRON:  cout << "Ladron"; break;
            case CORRUPTO:  cout << "Corrupto"; break;
        }
        cout << " | Estado: ";
        switch (actual->estado) {
            case ACTIVO: cout << "Activo"; break;
            case CAPTURADO: cout << "Capturado"; break;
            case INHABILITADO: cout << "Inhabilitado"; break;
        }
        cout << " | PV: " << actual->puntosVida << "/" << actual->puntosVidaMax;
        cout << " | Oro: " << actual->cargaOro << "/" << actual->capacidadOro << endl;
        cout << "  " << actual->descripcion << endl;
        
        mostrarHabilidadesPersonaje(actual);
        
        int itemsMochila = 0;
        NodoItemMochila* temp = actual->inventario;
        while (temp != nullptr) {
            itemsMochila++;
            temp = temp->siguiente;
        }
        cout << "  Items en mochila: " << itemsMochila << endl;
        
        cout << "  --------------------------------------------------------------" << endl;
        actual = actual->siguiente_global;
        contador++;
    }

    if (contador == 1) {
        cout << "  No hay personajes creados." << endl;
    }
}

void limpiarMochilaCompletamente(Personaje* personaje) {
    NodoItemMochila* actual = personaje->inventario;
    
    while (actual != nullptr) {
        NodoItemMochila* siguiente = actual->siguiente;
        delete actual;
        actual = siguiente;
    }
    
    personaje->inventario = nullptr;
}

void eliminarPersonaje(ListaPersonajes* lista, int id) {
    if (lista->cabeza == nullptr) return;

    Personaje* actual = lista->cabeza;
    Personaje* anterior = nullptr;
    
    while (actual != nullptr && actual->id != id) {
        anterior = actual;
        actual = actual->siguiente_global;
    }
    
    if (actual == nullptr) return;
    
    limpiarMochilaCompletamente(actual);
    
    if (anterior == nullptr) {
        lista->cabeza = actual->siguiente_global;
    } else {
        anterior->siguiente_global = actual->siguiente_global;
    }
    
    delete actual;
    cout << "Personaje eliminado completamente." << endl;
}

// GESTIÓN DE LISTA DE ITEMS
void agregarItem(ListaItems* lista, Item* item) {
    if (item->id == 0) {
        item->id = lista->contadorId++;
    } else {
        if (item->id >= lista->contadorId) {
            lista->contadorId = item->id + 1;
        }
    }
    item->disponible = true;
    NodoItemGlobal* nuevo = new NodoItemGlobal;
    nuevo->item = item;
    nuevo->siguiente = lista->cabeza;
    lista->cabeza = nuevo;
}

Item* buscarItemPorId(ListaItems* lista, int id) {
    NodoItemGlobal* actual = lista->cabeza;
    while (actual != nullptr) {
        if (actual->item->id == id) return actual->item;
        actual = actual->siguiente;
    }
    return nullptr;
}

void mostrarItemsPorNombre(ListaItems* lista, string nombre) {
    NodoItemGlobal* actual = lista->cabeza;
    nombre = limpiarString(nombre);
    
    bool encontrado = false;
    int contador = 1;

    cout << "\n--- RESULTADOS DE BÚSQUEDA: '" << nombre << "' ---" << endl;

    while (actual != nullptr) {
        string nombreItem = actual->item->nombre;
        
        // Verifica si el nombre coincide (parcial o total) y si está disponible
        if ((nombreItem == nombre || nombreItem.find(nombre) != string::npos) && actual->item->disponible) {
            
            cout << contador << ". ID: " << actual->item->id << " | " << actual->item->nombre << endl;
            cout << "   Descripcion: " << actual->item->descripcion << endl;
            cout << "   Usos: " << actual->item->usosRestantes << "/" << actual->item->durabilidadMaxima << endl;
            cout << "   Disponible: " << (actual->item->disponible ? "Si" : "No") << endl;
            cout << "   -----------------------------------------" << endl;
            
            encontrado = true;
            contador++;
        }
        actual = actual->siguiente;
    }

    if (!encontrado) {
        cout << "No se encontraron items con ese nombre." << endl;
    }
}

bool eliminarItem(ListaItems* lista, int id) {
    if (lista->cabeza == nullptr) return false;

    if (lista->cabeza->item->id == id) {
        NodoItemGlobal* temp = lista->cabeza;
        lista->cabeza = lista->cabeza->siguiente;
        delete temp->item;
        delete temp;
        return true;
    }

    NodoItemGlobal* actual = lista->cabeza;
    while (actual->siguiente != nullptr && actual->siguiente->item->id != id) {
        actual = actual->siguiente;
    }

    if (actual->siguiente != nullptr) {
        NodoItemGlobal* temp = actual->siguiente;
        actual->siguiente = temp->siguiente;
        delete temp->item;
        delete temp;
        return true;
    }
    return false;
}

void mostrarListaItems(ListaItems* lista) {
    NodoItemGlobal* actual = lista->cabeza;
    int contador = 1;
    cout << "==============================================================" << endl;
    cout << "                          LISTA DE ITEMS                      " << endl;
    cout << "==============================================================" << endl;

    while (actual != nullptr) {
        cout << "[" << contador << "] " << actual->item->nombre << " (ID: " << actual->item->id << ")" << endl;
        cout << "  Tipo: " << actual->item->descripcion << endl;
        cout << "  Usos máximos: " << actual->item->durabilidadMaxima << endl;
        cout << "  Usos restantes: " << actual->item->usosRestantes << endl;
        cout << "  Efecto: " << actual->item->efecto << endl;
        cout << "  Disponible: " << (actual->item->disponible ? "Sí" : "No") << endl;
        cout << "  --------------------------------------------------------------" << endl;
        actual = actual->siguiente;
        contador++;
    }

    if (contador == 1) {
        cout << "  No hay items creados." << endl;
    }
}

bool itemEstaEnAlgunaMochila(ListaPersonajes* listaPersonajes, int idItem) {
    Personaje* actualPersonaje = listaPersonajes->cabeza;
    
    while (actualPersonaje != nullptr) {
        NodoItemMochila* actualItem = actualPersonaje->inventario;
        
        while (actualItem != nullptr) {
            if (actualItem->item != nullptr && actualItem->item->id == idItem) {
                return true;
            }
            actualItem = actualItem->siguiente;
        }
        
        actualPersonaje = actualPersonaje->siguiente_global;
    }
    
    return false;
}

//  GESTIÓN DE EQUIPOS 
void inicializarEquipos(Juego* juego) {
    juego->equipos[0].nombre = "Policias Honestos";
    juego->equipos[0].cabeza = nullptr;
    juego->equipos[0].cantidadMiembros = 0;
    
    juego->equipos[1].nombre = "Ladrones";
    juego->equipos[1].cabeza = nullptr;
    juego->equipos[1].cantidadMiembros = 0;
    
    juego->equipos[2].nombre = "Policias Corruptos";
    juego->equipos[2].cabeza = nullptr;
    juego->equipos[2].cantidadMiembros = 0;
}

void agregarPersonajeAEquipo(Juego* juego, Personaje* personaje) {
    int indiceEquipo = -1;
    
    switch (personaje->bando) {
        case POLICIA_HONESTO: indiceEquipo = 0; break;
        case LADRON: indiceEquipo = 1; break;
        case CORRUPTO: indiceEquipo = 2; break;
    }
    
    if (indiceEquipo != -1) {
        personaje->siguiente_equipo = juego->equipos[indiceEquipo].cabeza;
        juego->equipos[indiceEquipo].cabeza = personaje;
        juego->equipos[indiceEquipo].cantidadMiembros++;
    }
}

void reorganizarEquipos(Juego* juego, ListaPersonajes* listaPersonajes) {
    inicializarEquipos(juego);
    
    Personaje* actual = listaPersonajes->cabeza;
    while (actual != nullptr) {
        if (actual->estado == ACTIVO) {
            agregarPersonajeAEquipo(juego, actual);
        }
        actual = actual->siguiente_global;
    }
}

void mostrarEquipos(Juego* juego) {
    cout << "\n==============================================================" << endl;
    cout << "                       GESTION DE EQUIPOS                      " << endl;
    cout << "==============================================================" << endl;
    
    for (int i = 0; i < 3; i++) {
        cout << "\n--- " << juego->equipos[i].nombre << " ---" << endl;
        
        Personaje* actual = juego->equipos[i].cabeza;
        int contador = 1;
        int miembrosActivos = 0;
        
        while (actual != nullptr) {
            if (actual->estado == ACTIVO) {
                miembrosActivos++;
                cout << contador << ". " << actual->nombre << " (ID: " << actual->id << ")" << endl;
                cout << "   Tipo: ";
                switch (actual->tipo) {
                    case INSPECTOR_JEFE:  cout << "Inspector Jefe"; break;
                    case PERITO_FORENSE:  cout << "Perito Forense"; break;
                    case OFICIAL_ASALTO:  cout << "Oficial de Asalto"; break;
                    case ANALISTA_DATOS:  cout << "Analista de Datos"; break;
                    case NEGOCIADOR:  cout << "Negociador"; break;
                    case POLICIA_CORRUPTO:  cout << "Policia Corrupto"; break;
                    case CEREBRO:  cout << "Cerebro"; break;
                    case FANTASMA:  cout << "Fantasma"; break;
                    case MULO:  cout << "Mulo"; break;
                    case SABOTEADOR:  cout << "Saboteador"; break;
                    case HACKER:  cout << "Hacker"; break;
                    case LOBERO:  cout << "Lobero"; break;
                }
                cout << " | PV: " << actual->puntosVida << "/" << actual->puntosVidaMax;
                cout << " | Oro: " << actual->cargaOro << endl;
                contador++;
            }
            actual = actual->siguiente_equipo;
        }
        
        cout << "Miembros activos: " << miembrosActivos << endl;
        
        if (contador == 1) {
            cout << "  No hay miembros activos en este equipo." << endl;
        }
    }
}

//  GESTIÓN DE MOCHILAS 
void mostrarMochilaPersonaje(Personaje* personaje) {
    cout << "\n--- MOCHILA DE " << personaje->nombre << " ---" << endl;
    
    if (personaje->inventario == nullptr) {
        cout << "  (Mochila vacia)" << endl;
        return;
    }
    
    NodoItemMochila* actual = personaje->inventario;
    int contador = 1;
    
    while (actual != nullptr) {
        if (actual->item != nullptr) {
            cout << contador << ". " << actual->item->nombre << " (ID: " << actual->item->id << ")" << endl;
            cout << "   Descripcion: " << actual->item->descripcion << endl;
            cout << "   Usos en mochila: " << actual->usosActuales << "/6" << endl;
            cout << "   Usos restantes del item: " << actual->item->usosRestantes << "/" 
                 << actual->item->durabilidadMaxima << endl;
            cout << "   Efecto: " << actual->item->efecto << endl;
            cout << "   -----------------------------------------" << endl;
        } else {
            cout << contador << ". [ITEM CORRUPTO - ERROR]" << endl;
        }
        actual = actual->siguiente;
        contador++;
    }
}

void agregarItemAMochila(Personaje* personaje, Item* item, ListaPersonajes* listaPersonajes) {
    if (item == nullptr) {
        cout << "Error: El item no existe." << endl;
        return;
    }
    
    if (personaje == nullptr) {
        cout << "Error: Personaje no válido." << endl;
        return;
    }
    
    if (itemEstaEnAlgunaMochila(listaPersonajes, item->id)) {
        cout << "Error: El item '" << item->nombre << "' ya está en la mochila de otro personaje." << endl;
        return;
    }
    
    if (!item->disponible) {
        cout << "Error: El item '" << item->nombre << "' ya no está disponible." << endl;
        return;
    }
    
    NodoItemMochila* nuevo = new NodoItemMochila;
    nuevo->item = item;
    nuevo->usosActuales = 0;
    nuevo->siguiente = personaje->inventario;
    
    personaje->inventario = nuevo;
    
    item->disponible = false;
    
    cout << "Item '" << item->nombre << "' (ID: " << item->id << ") agregado a la mochila de " << personaje->nombre << "!" << endl;
    cout << "Usa el mismo ID (" << item->id << ") para referenciar este item." << endl;
}

NodoItemMochila* buscarItemEnMochilaPorId(Personaje* personaje, int idItem) {
    if (personaje == nullptr || personaje->inventario == nullptr) {
        return nullptr;
    }
    
    NodoItemMochila* actual = personaje->inventario;
    
    while (actual != nullptr) {
        if (actual->item != nullptr && actual->item->id == idItem) {
            return actual;
        }
        actual = actual->siguiente;
    }
    
    return nullptr;
}

void usarItemEnMochila(Personaje* personaje, int idItem) {
    NodoItemMochila* nodo = buscarItemEnMochilaPorId(personaje, idItem);
    
    if (nodo == nullptr) {
        cout << "Error: Item no encontrado en la mochila." << endl;
        return;
    }
    
    if (nodo->item == nullptr) {
        cout << "Error: Item corrupto en la mochila." << endl;
        return;
    }
    
    nodo->usosActuales++;
    nodo->item->usosRestantes--;
    
    cout << "Item " << nodo->item->nombre << " (ID: " << nodo->item->id << ") usado." << endl;
    cout << "Usos actuales en mochila: " << nodo->usosActuales << endl;
    cout << "Usos restantes del item: " << nodo->item->usosRestantes << "/" 
         << nodo->item->durabilidadMaxima << endl;
    
    if (nodo->usosActuales >= 6 || nodo->item->usosRestantes <= 0) {
        cout << "¡El item " << nodo->item->nombre << " ha alcanzado su limite de usos!" << endl;
        
        NodoItemMochila* actual = personaje->inventario;
        NodoItemMochila* anterior = nullptr;
        
        while (actual != nullptr && actual != nodo) {
            anterior = actual;
            actual = actual->siguiente;
        }
        
        if (actual != nullptr) {
            if (anterior == nullptr) {
                personaje->inventario = actual->siguiente;
            } else {
                anterior->siguiente = actual->siguiente;
            }
            
            delete actual;
            
            cout << "Item eliminado de la mochila." << endl;
        }
    }
}

void eliminarItemDeMochila(Personaje* personaje, int idItem) {
    if (personaje == nullptr) {
        cout << "Error: Personaje no válido." << endl;
        return;
    }
    
    if (personaje->inventario == nullptr) {
        cout << "La mochila esta vacia." << endl;
        return;
    }
    
    NodoItemMochila* actual = personaje->inventario;
    NodoItemMochila* anterior = nullptr;
    
    while (actual != nullptr) {
        if (actual->item != nullptr && actual->item->id == idItem) {
            break;
        }
        anterior = actual;
        actual = actual->siguiente;
    }
    
    if (actual == nullptr) {
        cout << "Error: Item no encontrado en la mochila." << endl;
        return;
    }
    
    if (anterior == nullptr) {
        personaje->inventario = actual->siguiente;
    } else {
        anterior->siguiente = actual->siguiente;
    }
    
    delete actual;
    
    cout << "Item eliminado de la mochila correctamente." << endl;
}

//  CARGA DE ARCHIVOS .TREN 
void cargarMapaDesdeArchivo(GrafoMapa* mapa, const string& archivo) {
    ifstream file(archivo);

    if (!file.is_open()) {
        cout << "Aviso: No se pudo abrir " << archivo << ". Generando mapa interno de 30 estaciones..." << endl;
        
        mapa->estaciones = nullptr;
        mapa->totalEstaciones = 30;
        mapa->totalOro = 0;
        
        // Lista de nombres para el modo automático
        string nombresEstaciones[30] = {
            "Iturbide", "Del Sol", "Plaza de Armas", "O'Higgins", "Artigas",
            "San Martin", "Bolivar", "Roraima", "Torres del Paine", "Angostura",
            "Formosa", "Toledo", "Komsomolskaya", "Abrantes", "Grand Central",
            "Arts et Métiers", "Kungsträdgården", "Münchner Freiheit", "Westfriedhof", "Bilbao",
            "Shanghai", "T-Centralen", "Atocha", "Lisboa", "Singapur",
            "Plaza Venezuela", "Solna Centrum", "Westminster", "Alameda", "Avtovo"
        };

        // Crear estaciones
        for (int i = 1; i <= 30; i++) {
            Estacion* nueva = new Estacion();
            nueva->id = i;
            
            // Usar nombre del arreglo si está dentro del rango
            if (i <= 30) {
                nueva->nombre = nombresEstaciones[i - 1];
            } else {
                nueva->nombre = "Estacion " + to_string(i);
            }

            nueva->oro = rand() % 4;
            nueva->tieneCivil = (rand() % 100) < 30;
            nueva->revelada = false;
            nueva->investigada = false;
            nueva->conexiones = nullptr;
            
            nueva->siguiente = mapa->estaciones;
            mapa->estaciones = nueva;
            mapa->totalOro += nueva->oro;
        }

        // Crear conexiones automáticas (circular) para que se puedan mover
        for (int i = 1; i <= 30; i++) {
            Estacion* origen = buscarEstacionPorId(mapa->estaciones, i);
            int idDestino = (i % 30) + 1; 
            Estacion* destino = buscarEstacionPorId(mapa->estaciones, idDestino);

            if (origen != nullptr && destino != nullptr) {
                ConexionMapa* conIda = new ConexionMapa();
                conIda->idDestino = idDestino;
                conIda->distancia = (rand() % 5) + 1;
                conIda->siguiente = origen->conexiones;
                origen->conexiones = conIda;

                ConexionMapa* conVuelta = new ConexionMapa();
                conVuelta->idDestino = i;
                conVuelta->distancia = conIda->distancia;
                conVuelta->siguiente = destino->conexiones;
                destino->conexiones = conVuelta;
            }
        }
        
        cout << "Mapa por defecto generado (30 estaciones)." << endl;
        return; 
    }
    
    // 2. si el archivo abre, s lee
    string linea;
    getline(file, linea);
    try {
        mapa->totalEstaciones = stoi(limpiarString(linea));
    } catch (...) {
        cout << "Error: Formato inválido en primera línea." << endl;
        mapa->totalEstaciones = 0;
        file.close();
        return;
    }
    
    getline(file, linea);
    
    mapa->estaciones = nullptr;
    mapa->totalOro = 0;
    

    for (int i = 0; i < mapa->totalEstaciones; i++) {
        if (i > 0) getline(file, linea);
        
        Estacion* nueva = new Estacion();
        
        // ID
        getline(file, linea);
        try { nueva->id = stoi(limpiarString(linea)); } 
        catch (...) { delete nueva; break; }
        
        // Nombre
        getline(file, nueva->nombre);
        nueva->nombre = limpiarString(nueva->nombre);
        
        // Descripción (se lee pero no se guarda en struct Estacion actual)
        getline(file, linea); 
        
        // Datos aleatorios
        nueva->oro = rand() % 4;
        mapa->totalOro += nueva->oro;
        nueva->tieneCivil = (rand() % 100) < 30;
        nueva->revelada = false;
        nueva->investigada = false;
        nueva->conexiones = nullptr;
        
        // Conexiones
        getline(file, linea);
        linea = limpiarString(linea);
        stringstream ss(linea);
        string token;
        
        while (getline(ss, token, '|')) {
            size_t sep = token.find(':');
            if (sep != string::npos) {
                try {
                    int dest = stoi(token.substr(0, sep));
                    int dist = stoi(token.substr(sep + 1));
                    
                    ConexionMapa* con = new ConexionMapa();
                    con->idDestino = dest;
                    con->distancia = dist;
                    con->siguiente = nueva->conexiones;
                    nueva->conexiones = con;
                } catch (...) {}
            }
        }
        
        nueva->siguiente = mapa->estaciones;
        mapa->estaciones = nueva;
    }
    
    file.close();
    cout << "Mapa cargado correctamente desde " << archivo << "." << endl;
}

Estacion* buscarEstacionPorId(Estacion* lista, int id) {
    Estacion* actual = lista;
    while (actual != nullptr) {
        if (actual->id == id) return actual;
        actual = actual->siguiente;
    }
    return nullptr;
}

void mostrarMapa(GrafoMapa* mapa) {
    cout << "\n==============================================================" << endl;
    cout << "                          MAPA DEL METRO                      " << endl;
    cout << "==============================================================" << endl;
    
    Estacion* actual = mapa->estaciones;
    while (actual != nullptr) {
        cout << "Estacion #" << actual->id << ": " << actual->nombre << endl;
        cout << "  Oro: " << actual->oro << " lingotes | Civil: " 
             << (actual->tieneCivil ? "Si" : "No") << endl;
        cout << "  Conexiones: ";
        
        ConexionMapa* conexion = actual->conexiones;
        while (conexion != nullptr) {
            cout << conexion->idDestino << "(" << conexion->distancia << ")" 
                 << (conexion->siguiente != nullptr ? ", " : "");
            conexion = conexion->siguiente;
        }
        cout << endl << "  -----------------------------------------" << endl;
        
        actual = actual->siguiente;
    }
}

void cargarAccesoriosDesdeArchivo(ListaItems* listaItems, const string& archivo) {
    ifstream file(archivo);
    if (!file.is_open()) {
        cout << "Error: No se pudo abrir " << archivo << ". Usando accesorios base." << endl;
        inicializarItemsBase(listaItems);
        return;
    }
    
    string linea;
    getline(file, linea);
    int totalAccesorios = 0;
    try { totalAccesorios = stoi(limpiarString(linea)); } catch(...) {}
    
    getline(file, linea);
    
    cout << "Cargando " << totalAccesorios << " accesorios desde " << archivo << "..." << endl;
    
    for (int i = 0; i < totalAccesorios; i++) {
        string idStr, bando, nombre, funcion, usoLinea, alcanceLinea;
        
        getline(file, idStr);
        int id = 0;
        try { id = stoi(limpiarString(idStr)); } catch(...) {}
        
        getline(file, bando);
        getline(file, nombre);
        nombre = limpiarString(nombre);
        
        getline(file, funcion);
        getline(file, usoLinea);
        getline(file, alcanceLinea);
        getline(file, linea); // "---"
        
        int usos = 1;
        size_t pos = usoLinea.find(":");
        if (pos != string::npos) {
            try { usos = stoi(limpiarString(usoLinea.substr(pos + 2))); } catch(...) { usos = 1; }
        }
        
        int alcance = 0;
        pos = alcanceLinea.find(":");
        if (pos != string::npos) {
            try { alcance = stoi(limpiarString(alcanceLinea.substr(pos + 2))); } catch(...) { alcance = 0; }
        }
        

        TipoItem tipoItem = OTRO10; 

        // ITEMS DE MOVILIDAD
        if (nombre.find("Patines") != string::npos)       tipoItem = LLAVES_MANIPULADAS; // Moverse lejos/rápido
        else if (nombre.find("Bomba") != string::npos)    tipoItem = SENUELO_COMUNICACIONES; // Huida segura (éxito)
        else if (nombre.find("Jamelgo") != string::npos)  tipoItem = SENUELO_COMUNICACIONES; // Distracción

        //ITEMS DE INTERACCIÓN / CONTROL
        else if (nombre.find("Llave") != string::npos)    tipoItem = LLAVES_MANIPULADAS; // Abrir puertas
        else if (nombre.find("Cable") != string::npos)    tipoItem = ORDEN_ALLOGRAMIENTO; // Robar a distancia
        else if (nombre.find("Inmovilizador") != string::npos) tipoItem = GRABADORA_ENCUBIERTA; // Capturar (Inmovilizar)

        // ITEMS DE BÚSQUEDA / VIGILANCIA
        else if (nombre.find("Sensor") != string::npos)   tipoItem = SCANNER_METALES; // Detectar oro
        else if (nombre.find("Drone") != string::npos)    tipoItem = SCANNER_METALES; // Detectar oro
        else if (nombre.find("Linterna") != string::npos) tipoItem = KIT_HUELLAS; // Investigar mejor

        //ITEMS DE ASALTO / DEFENSA
        else if (nombre.find("Chaqueta") != string::npos) tipoItem = CHALECO_ANTIBALAS; // Armadura
        else if (nombre.find("Escudo") != string::npos)   tipoItem = CHALECO_ANTIBALAS; // Armadura
        else if (nombre.find("Estimulante") != string::npos) tipoItem = GANAS_MANOS; // Boost físico (mas carga)


        string descripcion = limpiarString(funcion);
        Item* nuevoItem = new Item{id, nombre, descripcion, tipoItem, usos, usos, "Efecto activo segun tipo", true};
        agregarItem(listaItems, nuevoItem);
    }
    
    file.close();
    cout << "Accesorios cargados y mapeados correctamente." << endl;
}

void cargarPersonajesDesdeArchivo(ListaPersonajes* listaPersonajes, ListaItems* listaItems, const string& archivo) {
    ifstream file(archivo);
    if (!file.is_open()) {
        cout << "Error: No se pudo abrir " << archivo << ". Usando personajes base." << endl;
        crearPersonajesBase(listaPersonajes);
        return;
    }
    
    string linea;
    getline(file, linea);
    int totalPersonajes = 0;
    try { totalPersonajes = stoi(limpiarString(linea)); } catch(...) {}
    getline(file, linea);
    
    cout << "Cargando " << totalPersonajes << " personajes desde " << archivo << "..." << endl;
    
    for (int i = 0; i < totalPersonajes; i++) {
        string idStr, bandoStr, nombre, accesoriosLinea;
        
        getline(file, idStr);
        int idArchivo = stoi(limpiarString(idStr));
        
        getline(file, bandoStr);
        bandoStr = limpiarString(bandoStr);
        
        getline(file, nombre);
        nombre = limpiarString(nombre);
        
        getline(file, accesoriosLinea);
        accesoriosLinea = limpiarString(accesoriosLinea);
        
        getline(file, linea);
        
        Bando bando;
        TipoPersonaje tipo;
        
        if (bandoStr == "Policia") {
            bando = POLICIA_HONESTO;
            tipo = OFICIAL_ASALTO; 
        } else {
            bando = LADRON;
            tipo = CEREBRO; 
        }
        
        Personaje* nuevo = crearPersonaje(nombre.c_str(), "Cargado desde archivo", tipo, bando, 5);
        nuevo->id = idArchivo;
        
        if (accesoriosLinea != "-" && accesoriosLinea.length() > 0) {
            stringstream ss(accesoriosLinea);
            string token;
            while (getline(ss, token, '|')) {
                try {
                    int idAccesorio = stoi(token);
                    Item* itemEncontrado = buscarItemPorId(listaItems, idAccesorio);
                    
                    if (itemEncontrado != nullptr && itemEncontrado->disponible) {
                        agregarItemAMochila(nuevo, itemEncontrado, listaPersonajes);
                    }
                } catch(...) {
                    cout << "Error leyendo ID de accesorio: " << token << endl;
                }
            }
        }

        agregarPersonaje(listaPersonajes, nuevo);
    }
    
    file.close();
    cout << "Personajes cargados correctamente." << endl;
}

// FUNCIONES DEL JUEGO 
void inicializarPartida(Juego* juego, ListaPersonajes* listaPersonajes) {
    cout << "\n=== INICIALIZANDO PARTIDA ===" << endl;
    
    cargarMapaDesdeArchivo(&juego->mapa, "Mapa.tren");
    
    Personaje* actual = listaPersonajes->cabeza;
    while (actual != nullptr) {
        // Asignar posición aleatoria en el mapa
        actual->posicion = 1 + (rand() % juego->mapa.totalEstaciones);
        
        // Resetear estados de la partida anterior
        actual->estado = ACTIVO; 
        actual->cargaOro = 0;
        // Restaurar vida al máximo
        actual->puntosVida = actual->puntosVidaMax;
        
        actual = actual->siguiente_global;
    }
    
    reorganizarEquipos(juego, listaPersonajes);
    
    // resetear contadores del juego
    juego->oroTotal = juego->mapa.totalOro;
    juego->oroRecuperado = 0;
    juego->ladronesCapturados = 0;
    juego->corruptosDescubiertos = 0;
    juego->rondaActual = 1;
    juego->juegoTerminado = false;
    juego->logBitacora = "=== BITACORA DE PARTIDA ===\n";
    
    cout << "Partida inicializada." << endl;
    cout << "Los equipos han sido cargados segun su configuracion actual." << endl;
}


void bubbleSortPersonajes(ListaPersonajes* lista, bool porIniciativa) {
    // Si la lista está vacía o tiene un solo elemento, no hacemos nada
    if (lista->cabeza == nullptr || lista->cabeza->siguiente_global == nullptr) {
        return;
    }

    bool huboIntercambio;
    
    // Repetimos el ciclo mientras sigan habiendo cambios
    do {
        huboIntercambio = false;
        Personaje* actual = lista->cabeza;
        Personaje* anterior = nullptr;

        while (actual->siguiente_global != nullptr) {
            Personaje* siguiente = actual->siguiente_global;
            bool debeCambiar = false;


            if (porIniciativa) {
                // Ordenar por VIDA (Descendente: Mayor a Menor)
                if (actual->puntosVida < siguiente->puntosVida) {
                    debeCambiar = true;
                }
            } else {
                // Ordenar por ID (Ascendente: Menor a Mayor)
                if (actual->id > siguiente->id) {
                    debeCambiar = true;
                }
            }

            if (debeCambiar) {
                if (anterior != nullptr) {
                    anterior->siguiente_global = siguiente;
                } else {
                    lista->cabeza = siguiente; 
                }

                actual->siguiente_global = siguiente->siguiente_global;
                siguiente->siguiente_global = actual;

                huboIntercambio = true;
                anterior = siguiente; 
            } else {
                // Si no hubo cambio, avanzamos los punteros normalmente
                anterior = actual;
                actual = actual->siguiente_global;
            }
        }
    } while (huboIntercambio);
}




void verificarVictoria(Juego* juego) {
    if (juego->oroTotal == 0) return;
    
    double porcentajeOroEscapado = 100.0 * (juego->oroTotal - juego->oroRecuperado) / juego->oroTotal;
    
    if (porcentajeOroEscapado > 70.0) {
        cout << "\n=======================================" << endl;
        cout << "|      LOS LADRONES HAN GANADO!        |" << endl;
        cout << "|    Escaparon con mas del 70% del oro |" << endl;
        cout << "=======================================" << endl;
        juego->juegoTerminado = true;
        juego->logBitacora += "Victoria de Ladrones\n";
        return;
    }
    
    if (juego->ladronesCapturados >= 4 && juego->oroRecuperado >= 0.9 * juego->oroTotal) {
        cout << "\n=======================================" << endl;
        cout << "|    POLICIAS HONESTOS HAN GANADO!      |" << endl;
        cout << "|   Recuperaron el oro y capturaron a   |" << endl;
        cout << "|           los ladrones                |" << endl;
        cout << "=======================================" << endl;
        juego->juegoTerminado = true;
        juego->logBitacora += "Victoria de Policias Honestos\n";
        return;
    }
    
    if (porcentajeOroEscapado >= 40.0 && porcentajeOroEscapado <= 60.0) {
        cout << "\n========================================" << endl;
        cout << "|    POLICIAS CORRUPTOS HAN GANADO!     |" << endl;
        cout << "|   Escapo entre 40% y 60% del oro      |" << endl;
        cout << "========================================" << endl;
        juego->juegoTerminado = true;
        juego->logBitacora += "Victoria de Policias Corruptos\n";
        return;
    }
}

void guardarBitacora(Juego* juego, const string& accion) {
    juego->logBitacora += "Ronda " + to_string(juego->rondaActual) + ": " + accion + "\n";
}

void mostrarEstadoJuego(Juego* juego) {
    cout << "\n=== ESTADO DE LA PARTIDA (Ronda " << juego->rondaActual << ") ===" << endl;
    cout << "Oro total: " << juego->oroTotal << " lingotes" << endl;
    cout << "Oro recuperado: " << juego->oroRecuperado << " lingotes" << endl;
    cout << "Oro escapado: " << (juego->oroTotal - juego->oroRecuperado) << " lingotes" << endl;
    cout << "Ladrones capturados: " << juego->ladronesCapturados << endl;
    cout << "Corruptos descubiertos: " << juego->corruptosDescubiertos << endl;
    
    if (juego->oroTotal > 0) {
        cout << "Porcentaje oro recuperado: " << (juego->oroRecuperado * 100 / juego->oroTotal) << "%" << endl;
        cout << "Porcentaje oro escapado: " << ((juego->oroTotal - juego->oroRecuperado) * 100 / juego->oroTotal) << "%" << endl;
    }
    
    cout << "----------------------------------------" << endl;
}

void menuJugarPartida(Juego* juego, ListaPersonajes* listaPersonajes) {
    cout << "\n==============================================================" << endl;
    cout << "             INICIAR PARTIDA - MODO INTERACTIVO               " << endl;
    cout << "==============================================================" << endl;
    
    // Ordenar por iniciativa
    bubbleSortPersonajes(listaPersonajes, true);
    
    inicializarPartida(juego, listaPersonajes);
    
    while (!juego->juegoTerminado && juego->rondaActual <= 10) {
        cout << "\n\n";
        cout << "##############################################################" << endl;
        cout << "                    INICIO DE LA RONDA " << juego->rondaActual << endl;
        cout << "##############################################################" << endl;
        
        bubbleSortPersonajes(listaPersonajes, true);
        Personaje* actual = listaPersonajes->cabeza;
        
        while (actual != nullptr && !juego->juegoTerminado) {
            if (actual->estado == ACTIVO) {
            
                bool bonoExito = false;          // Para Kit de Huellas / Señuelo
                bool modoTeletransporte = false; // Para Llaves Manipuladas
                bool turnoTerminadoPorItem = false; // Para acciones remotas (Allanamiento)
                
                cout << "\n--------------------------------------------------------------" << endl;
                cout << " TURNO DE: " << actual->nombre << " (ID: " << actual->id << ")" << endl;
                cout << " Bando: " << (actual->bando == POLICIA_HONESTO ? "Policia" : (actual->bando == LADRON ? "Ladron" : "Corrupto"));
                cout << " | Vida: " << actual->puntosVida << "/" << actual->puntosVidaMax;
                cout << " | Oro: " << actual->cargaOro << "/" << actual->capacidadOro << endl;
                
                Estacion* estacionActual = buscarEstacionPorId(juego->mapa.estaciones, actual->posicion);
                
                if (estacionActual) {
                    cout << " Ubicacion Actual: Estacion " << estacionActual->id << " - " << estacionActual->nombre << endl;

                    if (actual->inventario != nullptr) {
                        cout << " [!] Tienes items en tu mochila." << endl;
                        char resp;
                        cout << " Deseas usar un item antes de moverte? (s/n): ";
                        cin >> resp;
                        
                        if (resp == 's' || resp == 'S') {
                            mostrarMochilaPersonaje(actual);
                            cout << " Ingrese el ID del item a usar: ";
                            int idItem;
                            if (cin >> idItem) {
                                NodoItemMochila* nodoMochila = buscarItemEnMochilaPorId(actual, idItem);
                                
                                if (nodoMochila != nullptr) {
                                    cout << " \n>>> USANDO " << nodoMochila->item->nombre << " <<<" << endl;

                                    switch(nodoMochila->item->tipo) {
                                            
                                            //  SCANNER (Ver oro alrededor)
                                            case SCANNER_METALES: {
                                                cout << " [EFECTO] Escaneando sensores de oro..." << endl;
                                                ConexionMapa* c = estacionActual->conexiones;
                                                bool hallado = false;
                                                while(c) {
                                                    Estacion* vecina = buscarEstacionPorId(juego->mapa.estaciones, c->idDestino);
                                                    if (vecina && vecina->oro > 0) {
                                                        cout << "  -> Detectado " << vecina->oro << " lingotes en Estacion " << vecina->id << "!" << endl;
                                                        hallado = true;
                                                    }
                                                    c = c->siguiente;
                                                }
                                                if(!hallado) cout << "  -> No se detecto oro en las cercanias." << endl;
                                                break;
                                            }

                                            // KIT DE HUELLAS / SEÑUELO (Éxito garantizado)
                                            case KIT_HUELLAS:
                                            case SENUELO_COMUNICACIONES:
                                                cout << " [EFECTO] Tu proxima accion tendra 100% de probabilidad de exito." << endl;
                                                bonoExito = true;
                                                break;

                                            // ORDEN DE ALLANAMIENTO / FONDO NEGRO (Acción Remota)
                                            case ORDEN_ALLOGRAMIENTO:
                                            case FONDO_NEGRO: {
                                                cout << " [EFECTO] Selecciona una estacion adyacente para actuar sin moverte." << endl;
                                                cout << " Conexiones: ";
                                                ConexionMapa* c = estacionActual->conexiones;
                                                while(c) { cout << c->idDestino << " "; c = c->siguiente; }
                                                cout << endl;
                                                
                                                int idObj;
                                                cout << " ID objetivo: "; cin >> idObj;
                                                if (esMovimientoValido(estacionActual, idObj)) {
                                                    Estacion* remota = buscarEstacionPorId(juego->mapa.estaciones, idObj);
                                                    if (remota && remota->oro > 0) {
                                                        remota->oro--;
                                                        if(actual->bando == POLICIA_HONESTO) {
                                                            juego->oroRecuperado++;
                                                            cout << "  -> ¡Oro recuperado remotamente!" << endl;
                                                        } else {
                                                            cout << "  -> ¡Oro saboteado/robado remotamente!" << endl;
                                                            if(actual->bando == LADRON && actual->cargaOro < actual->capacidadOro) actual->cargaOro++;
                                                        }
                                                    } else {
                                                        cout << "  -> La estación objetivo estaba vacia." << endl;
                                                    }
                                                    turnoTerminadoPorItem = true; // Gastó su turno en esto
                                                } else {
                                                    cout << "  -> Estacion no válida. Item gastado sin efecto." << endl;
                                                }
                                                break;
                                            }

                                            //CHALECO ANTIBALAS (Recuperar vida)
                                            case CHALECO_ANTIBALAS:
                                                cout << " [EFECTO] Reforzando proteccion. Recuperas 2 Puntos de Vida." << endl;
                                                actual->puntosVida += 2;
                                                if(actual->puntosVida > actual->puntosVidaMax) actual->puntosVida = actual->puntosVidaMax;
                                                cout << "  -> Vida actual: " << actual->puntosVida << endl;
                                                break;

                                            // GRABADORA (Captura instantánea)
                                            case GRABADORA_ENCUBIERTA: {
                                                cout << " [EFECTO] Buscando evidencia incriminatoria en la estacion actual..." << endl;
                                                Personaje* p = listaPersonajes->cabeza;
                                                bool capturado = false;
                                                while(p) {
                                                    if(p->id != actual->id && p->posicion == actual->posicion && p->estado == ACTIVO) {
                                                        if (p->bando == CORRUPTO || p->bando == LADRON) {
                                                            cout << "  -> Has grabado a " << p->nombre << " cometiendo un delito!" << endl;
                                                            cout << "  -> " << p->nombre << " ha sido CAPTURADO." << endl;
                                                            p->estado = CAPTURADO;
                                                            if(p->bando == LADRON) juego->ladronesCapturados++;
                                                            if(p->bando == CORRUPTO) juego->corruptosDescubiertos++;
                                                            capturado = true;
                                                        }
                                                    }
                                                    p = p->siguiente_global;
                                                }
                                                if(!capturado) cout << "  -> No se encontraron criminales activos aqui." << endl;
                                                break;
                                            }

                                            // GANAS DE MANOS (Más capacidad)
                                            case GANAS_MANOS:
                                                cout << " [EFECTO] Capacidad de carga aumentada en +3 lingotes permanentemente." << endl;
                                                actual->capacidadOro += 3;
                                                break;

                                            // LLAVES MANIPULADAS (Teletransporte)
                                            case LLAVES_MANIPULADAS:
                                                cout << " [EFECTO] Pasadizos secretos abiertos. Puedes moverte a CUALQUIER estación." << endl;
                                                modoTeletransporte = true;
                                                break;

                                            default:
                                                cout << " [EFECTO] Item utilizado. (Efecto generico o pasivo)" << endl;
                                                break;
                                        }

                                    usarItemEnMochila(actual, idItem);
                                    
                                } else {
                                    cout << "Error: No tienes ese item." << endl;
                                }
                            } else {
                                cin.clear(); cin.ignore(10000, '\n');
                            }
                        }
                    } else {
                        cout << " (Mochila vacia)" << endl;
                    }

                    if (!turnoTerminadoPorItem) {
                        
                        cout << " -> Conexiones disponibles: ";
                        ConexionMapa* con = estacionActual->conexiones;
                        if (!con) cout << "Ninguna";
                        while (con) {
                            cout << "[" << con->idDestino << "] ";
                            con = con->siguiente;
                        }
                        if (modoTeletransporte) cout << " [TELETRANSPORTE ACTIVO: CUALQUIER ID ES VALIDO]";
                        cout << endl;

                        int idDestino;
                        bool movimientoValido = false;
                        
                        do {
                            cout << " >> Ingrese ID de la estacion a mover: ";
                            if (!(cin >> idDestino)) {
                                cin.clear(); cin.ignore(10000, '\n');
                                cout << "Entrada inválida." << endl;
                                continue;
                            }
                            
                            // Si tiene teletransporte, ignoramos la validación de aristas
                            if (modoTeletransporte) {
                                if (idDestino >= 1 && idDestino <= juego->mapa.totalEstaciones) movimientoValido = true;
                                else cout << "ID de estacion no existe." << endl;
                            } else {
                                if (esMovimientoValido(estacionActual, idDestino)) movimientoValido = true;
                                else cout << "Movimiento invalido! No hay conexion directa." << endl;
                            }
                            
                        } while (!movimientoValido);
                        
                        // Ejecutar movimiento
                        actual->posicion = idDestino;
                        cout << " -> " << actual->nombre << " se movio a estacion " << idDestino << "." << endl;
                        
                        
                        Estacion* nuevaEstacion = buscarEstacionPorId(juego->mapa.estaciones, actual->posicion);
                        int probabilidadBase = 50; 
                        if (bonoExito) probabilidadBase = 100; // Si usó Kit de Huellas, es 100%
                        
                        switch (actual->bando) {
                            case POLICIA_HONESTO:
                                cout << " [ACCION] Investigando..." << endl;
                                if (nuevaEstacion->oro > 0) {
                                    cout << "  ¡Hay " << nuevaEstacion->oro << " lingotes aqui!" << endl;
                                    
                                    if (rand() % 100 < probabilidadBase) {
                                        juego->oroRecuperado++;
                                        nuevaEstacion->oro--;
                                        cout << "  EXITO! Recuperado 1 lingote." << endl;
                                    } else {
                                        cout << "  Fallo la recuperación (Intenta usar un Kit de Huellas)." << endl;
                                    }
                                } else cout << "  Zona limpia." << endl;
                                break;
                                
                            case LADRON:
                                cout << " [ACCION] Buscando botin..." << endl;
                                if (nuevaEstacion->oro > 0) {
                                    if (actual->cargaOro < actual->capacidadOro) {
                                        if (rand() % 100 < probabilidadBase) {
                                            actual->cargaOro++;
                                            nuevaEstacion->oro--;
                                            cout << "  EXITO! Robado 1 lingote. (Mochila: " << actual->cargaOro << "/" << actual->capacidadOro << ")" << endl;
                                        } else {
                                             cout << "  No pudiste robar el oro sin ser visto." << endl;
                                        }
                                    } else cout << "  Mochila llena." << endl;
                                } else cout << "  No hay oro." << endl;
                                break;
                                
                            case CORRUPTO:
                                cout << " [ACCION] Saboteando..." << endl;
                                if (nuevaEstacion->oro > 0) cout << "  Has ocultado evidencias." << endl;
                                break;
                        }
                        
                        guardarBitacora(juego, actual->nombre + " movido a " + to_string(actual->posicion));
                    }
                }
            }
            actual = actual->siguiente_global;
        }
        
        verificarVictoria(juego);
        if (!juego->juegoTerminado) {
            juego->rondaActual++;
            cout << "\n--- FIN DE LA RONDA. Presione Enter ---";
            cin.ignore(); cin.get();
        }
    }
    
    if (!juego->juegoTerminado) {
        cout << "\n=== PARTIDA TERMINADA POR TIEMPO ===" << endl;
        juego->logBitacora += "Fin por rondas\n";
    }
    
    ofstream archivoBitacora("bitacora.txt");
    if (archivoBitacora.is_open()) {
        archivoBitacora << juego->logBitacora;
        archivoBitacora.close();
    }
    mostrarEstadoJuego(juego);
}

// menu gesiton
void menuGestionPersonajes(ListaPersonajes* listaPersonajes) {
    int opcion;
    do {
        cout << "\n==============================================================" << endl;
        cout << "                       GESTION DE PERSONAJES                    " << endl;
        cout << "==============================================================" << endl;
        cout << "1. Crear Personaje" << endl;
        cout << "2. Modificar Personaje" << endl;
        cout << "3. Eliminar Personaje" << endl;
        cout << "4. Mostrar Personajes" << endl;
        cout << "5. Buscar Personaje" << endl;
        cout << "6. Volver al Menu Principal" << endl;
        cout << "Seleccione una opcion: ";
        opcion = leerEntero(1, 6);

        switch (opcion) {
            case 1: {
                string nombre, descripcion;
                int tipo, bando, capacidad;
                
                cout << "\n--- CREAR NUEVO PERSONAJE ---" << endl;
                cout << "Nombre: ";
                getline(cin, nombre);
                cout << "Descripcion: ";
                getline(cin, descripcion);
                
                cout << "Tipo (1-12): " << endl;
                cout << "1. Inspector Jefe  2. Perito Forense  3. Oficial Asalto" << endl;
                cout << "4. Analista Datos  5. Negociador      6. Policia Corrupto" << endl;
                cout << "7. Cerebro         8. Fantasma        9. Mulo" << endl;
                cout << "10. Saboteador     11. Hacker         12. Lobero" << endl;
                tipo = leerEntero(1, 12);
                
                cout << "Bando (1-3): " << endl;
                cout << "1. Policia Honesto  2. Ladron  3. Corrupto" << endl;
                bando = leerEntero(1, 3);
                
                cout << "Capacidad de oro: ";
                capacidad = leerEntero(1, 1000);
                
                TipoPersonaje tipoEnum = static_cast<TipoPersonaje>(tipo - 1);
                Bando bandoEnum = static_cast<Bando>(bando - 1);
                
                Personaje* nuevo = crearPersonaje(nombre.c_str(), descripcion.c_str(), tipoEnum, bandoEnum, capacidad);
                agregarPersonaje(listaPersonajes, nuevo);
                cout << "Personaje creado exitosamente con ID: " << nuevo->id << "!" << endl;
                break;
            }
            
            case 2: {
                cout << "\n--- MODIFICAR PERSONAJE ---" << endl;
                mostrarListaPersonajes(listaPersonajes);
                int id;
                cout << "ID del personaje a modificar: ";
                id = leerEntero(1, 10000);
                
                Personaje* personaje = buscarPorId(listaPersonajes, id);
                if (personaje) {
                    string nuevoNombre, nuevaDesc;
                    cout << "Nuevo nombre (actual: " << personaje->nombre << "): ";
                    getline(cin, nuevoNombre);
                    cout << "Nueva descripcion: ";
                    getline(cin, nuevaDesc);
                    
                    if (!nuevoNombre.empty()) personaje->nombre = nuevoNombre;
                    if (!nuevaDesc.empty()) personaje->descripcion = nuevaDesc;
                    
                    cout << "Personaje modificado exitosamente!" << endl;
                } else {
                    cout << "Personaje no encontrado." << endl;
                }
                break;
            }
            
            case 3: {
                cout << "\n--- ELIMINAR PERSONAJE ---" << endl;
                mostrarListaPersonajes(listaPersonajes);
                int id;
                cout << "ID del personaje a eliminar: ";
                id = leerEntero(1, 10000);
                
                Personaje* personaje = buscarPorId(listaPersonajes, id);
                if (personaje) {
                    eliminarPersonaje(listaPersonajes, id);
                    cout << "Personaje eliminado exitosamente!" << endl;
                } else {
                    cout << "Personaje no encontrado." << endl;
                }
                break;
            }
            
            case 4:
                mostrarListaPersonajes(listaPersonajes);
                break;
                
            case 5: {
                string nombre;
                cout << "Nombre del personaje a buscar: ";
                getline(cin, nombre);
                
                Personaje* personaje = buscarPorNombre(listaPersonajes, nombre);
                if (personaje) {
                    cout << "Personaje encontrado:" << endl;
                    cout << "ID: " << personaje->id << " | Nombre: " << personaje->nombre << endl;
                    cout << "Descripcion: " << personaje->descripcion << endl;
                    mostrarHabilidadesPersonaje(personaje);
                } else {
                    cout << "Personaje no encontrado." << endl;
                }
                break;
            }
            
            case 6:
                cout << "Volviendo al menú principal..." << endl;
                break;
                
            default:
                cout << "Opcion no valida." << endl;
        }
    } while (opcion != 6);
}

void menuGestionItems(ListaItems* listaItems) {
    int opcion;
    do {
        cout << "\n==============================================================" << endl;
        cout << "                       GESTION DE ITEMS                       " << endl;
        cout << "==============================================================" << endl;
        cout << "1. Crear Item" << endl;
        cout << "2. Modificar Item" << endl;
        cout << "3. Eliminar Item" << endl;
        cout << "4. Listar Items" << endl;
        cout << "5. Buscar Item por nombre" << endl;
        cout << "6. Volver al Menu Principal" << endl;
        cout << "Seleccione una opcion: ";
        opcion = leerEntero(1, 6);

        switch (opcion) {
            case 1: {
                string nombre, descripcion, efecto;
                int tipo, durabilidad;
                cout << "\n--- CREAR NUEVO ITEM ---" << endl;
                cout << "Nombre: ";
                getline(cin, nombre);
                cout << "Descripcion: ";
                getline(cin, descripcion);
                cout << "Efecto: ";
                getline(cin, efecto);
                cout << "Durabilidad máxima (usos): ";
                durabilidad = leerEntero(1, 1000);
                cout << "Tipo (1-20): ";
                tipo = leerEntero(1, 20);
                TipoItem tipoEnum = static_cast<TipoItem>(tipo - 1);
                
                Item* nuevoItem = new Item{0, nombre, descripcion, tipoEnum, durabilidad, durabilidad, efecto, true};
                agregarItem(listaItems, nuevoItem);
                cout << "Item creado exitosamente con ID: " << nuevoItem->id << "!" << endl;
                cout << "Puedes crear otro item con el mismo nombre si lo deseas." << endl;
                break;
            }
            
            case 2: {
                cout << "\n--- MODIFICAR ITEM ---" << endl;
                mostrarListaItems(listaItems);
                cout << "ID del item a modificar: ";
                int id = leerEntero(1, 10000);
                Item* it = buscarItemPorId(listaItems, id);
                if (!it) { 
                    cout << "Item no encontrado." << endl; 
                    break; 
                }
                string nombre, descripcion, efecto;
                int durabilidad;
                cout << "Nuevo nombre (actual: " << it->nombre << "): ";
                getline(cin, nombre);
                cout << "Nueva descripcion: ";
                getline(cin, descripcion);
                cout << "Nuevo efecto: ";
                getline(cin, efecto);
                cout << "Nueva durabilidad máxima: ";
                durabilidad = leerEntero(1, 1000);
                if (!nombre.empty()) it->nombre = nombre;
                if (!descripcion.empty()) it->descripcion = descripcion;
                if (!efecto.empty()) it->efecto = efecto;
                it->durabilidadMaxima = durabilidad;
                it->usosRestantes = durabilidad;
                cout << "Item modificado exitosamente." << endl;
                break;
            }
            
            case 3: {
                cout << "\n--- ELIMINAR ITEM ---" << endl;
                mostrarListaItems(listaItems);
                cout << "ID del item a eliminar: ";
                int id = leerEntero(1, 10000);
                if (eliminarItem(listaItems, id)) {
                    cout << "Item eliminado." << endl;
                } else {
                    cout << "Item no encontrado." << endl;
                }
                break;
            }
            
            case 4:
                mostrarListaItems(listaItems);
                break;
                
            case 5: {
                string nombre;
                cout << "Nombre del item a buscar: ";
                getline(cin, nombre);
                
                mostrarItemsPorNombre(listaItems, nombre);
                break;
            }
            
            case 6:
                cout << "Volviendo al menu principal..." << endl;
                break;
                
            default:
                cout << "Opcion no valida." << endl;
        }
    } while (opcion != 6);
}

void menuGestionEquipos(Juego* juego, ListaPersonajes* listaPersonajes) {
    int opcion;
    do {
        cout << "\n==============================================================" << endl;
        cout << "                       GESTION DE EQUIPOS                      " << endl;
        cout << "==============================================================" << endl;
        cout << "1. Mostrar equipos organizados" << endl;
        cout << "2. Asignar personaje a equipo" << endl;
        cout << "3. Volver al Menu Principal" << endl;
        cout << "Seleccione una opcion: ";
        opcion = leerEntero(1, 3);

        switch (opcion) {
            case 1:
                mostrarEquipos(juego);
                break;
                
            case 2: {
                cout << "\n--- ASIGNAR PERSONAJE A EQUIPO ---" << endl;
                mostrarListaPersonajes(listaPersonajes);
                
                int idPersonaje;
                cout << "ID del personaje a asignar: ";
                idPersonaje = leerEntero(1, 10000);
                
                Personaje* personaje = buscarPorId(listaPersonajes, idPersonaje);
                if (!personaje) {
                    cout << "Personaje no encontrado." << endl;
                    break;
                }
                
                cout << "Bando actual del personaje: ";
                switch (personaje->bando) {
                    case POLICIA_HONESTO:  cout << "Policia Honesto"; break;
                    case LADRON:  cout << "Ladron"; break;
                    case CORRUPTO:  cout << "Corrupto"; break;
                }
                cout << endl;
                
                cout << "Nuevo bando (1-3): " << endl;
                cout << "1. Policia Honesto  2. Ladron  3. Corrupto" << endl;
                int nuevoBando = leerEntero(1, 3);
                Bando bandoEnum = static_cast<Bando>(nuevoBando - 1);
                
                personaje->bando = bandoEnum;
                reorganizarEquipos(juego, listaPersonajes);
                
                cout << "Personaje asignado al nuevo equipo exitosamente!" << endl;
                break;
            }
            
            case 3:
                cout << "Volviendo al menu principal..." << endl;
                break;
                
            default:
                cout << "Opcion no valida." << endl;
        }
    } while (opcion != 3);
}

void menuGestionMochilas(ListaPersonajes* listaPersonajes, ListaItems* listaItems) {
    int opcion;
    do {
        cout << "\n==============================================================" << endl;
        cout << "                       GESTION DE MOCHILAS                     " << endl;
        cout << "==============================================================" << endl;
        cout << "1. Ver mochila de personaje" << endl;
        cout << "2. Agregar item a mochila" << endl;
        cout << "3. Eliminar item de mochila" << endl;
        cout << "4. Usar item de mochila" << endl;
        cout << "5. Vaciar mochila completamente" << endl;
        cout << "6. Volver al Menu Principal" << endl;
        cout << "Seleccione una opcion: ";
        opcion = leerEntero(1, 6);

        switch (opcion) {
            case 1: {
                cout << "\n--- VER MOCHILA DE PERSONAJE ---" << endl;
                mostrarListaPersonajes(listaPersonajes);
                
                int idPersonaje;
                cout << "ID del personaje: ";
                idPersonaje = leerEntero(1, 10000);
                
                Personaje* personaje = buscarPorId(listaPersonajes, idPersonaje);
                if (!personaje) {
                    cout << "Personaje no encontrado." << endl;
                    break;
                }
                
                mostrarMochilaPersonaje(personaje);
                break;
            }
            
            case 2: {
                cout << "\n--- AGREGAR ITEM A MOCHILA ---" << endl;
                mostrarListaPersonajes(listaPersonajes);
                
                int idPersonaje;
                cout << "ID del personaje: ";
                idPersonaje = leerEntero(1, 10000);
                
                Personaje* personaje = buscarPorId(listaPersonajes, idPersonaje);
                if (!personaje) {
                    cout << "Personaje no encontrado." << endl;
                    break;
                }
                
                if (personaje->estado != ACTIVO) {
                    cout << "Error: El personaje no está activo." << endl;
                    break;
                }
                
                cout << "==============================================================" << endl;
                cout << "                   ITEMS DISPONIBLES                          " << endl;
                cout << "==============================================================" << endl;
                
                NodoItemGlobal* actual = listaItems->cabeza;
                // --- NUEVA LÓGICA SIN VECTOR ---
                // Simplemente listamos los items y pedimos el ID
                while (actual != nullptr) {
                    if (actual->item->disponible) {
                        cout << "ID " << actual->item->id << ": " << actual->item->nombre 
                             << " (" << actual->item->descripcion << ")" << endl;
                    }
                    actual = actual->siguiente;
                }
                
                cout << "Ingrese el ID del item que desea agregar: ";
                int idItemSel = leerEntero(1, 10000);
                Item* itemSeleccionado = buscarItemPorId(listaItems, idItemSel);
                
                if (itemSeleccionado != nullptr) {
                    agregarItemAMochila(personaje, itemSeleccionado, listaPersonajes);
                } else {
                    cout << "Error: Item no encontrado o ID incorrecto." << endl;
                }
                break;
            }
            
            case 3: {
                cout << "\n--- ELIMINAR ITEM DE MOCHILA ---" << endl;
                mostrarListaPersonajes(listaPersonajes);
                
                int idPersonaje;
                cout << "ID del personaje: ";
                idPersonaje = leerEntero(1, 10000);
                
                Personaje* personaje = buscarPorId(listaPersonajes, idPersonaje);
                if (!personaje) {
                    cout << "Personaje no encontrado." << endl;
                    break;
                }
                
                if (personaje->estado != ACTIVO) {
                    cout << "Error: El personaje no está activo." << endl;
                    break;
                }
                
                mostrarMochilaPersonaje(personaje);
                
                if (personaje->inventario == nullptr) {
                    cout << "La mochila está vacía." << endl;
                    break;
                }
                
                int idItem;
                cout << "ID del item a eliminar (ver en la lista arriba): ";
                idItem = leerEntero(1, 10000);
                
                eliminarItemDeMochila(personaje, idItem);
                break;
            }
            
            case 4: {
                cout << "\n--- USAR ITEM DE MOCHILA ---" << endl;
                mostrarListaPersonajes(listaPersonajes);
                
                int idPersonaje;
                cout << "ID del personaje: ";
                idPersonaje = leerEntero(1, 10000);
                
                Personaje* personaje = buscarPorId(listaPersonajes, idPersonaje);
                if (!personaje) {
                    cout << "Personaje no encontrado." << endl;
                    break;
                }
                
                if (personaje->estado != ACTIVO) {
                    cout << "Error: El personaje no está activo." << endl;
                    break;
                }
                
                mostrarMochilaPersonaje(personaje);
                
                if (personaje->inventario == nullptr) {
                    cout << "La mochila esta vacia." << endl;
                    break;
                }
                
                int idItem;
                cout << "ID del item a usar (ver en la lista arriba): ";
                idItem = leerEntero(1, 10000);
                
                usarItemEnMochila(personaje, idItem);
                break;
            }
            
            case 5: {
                cout << "\n--- VACIAR MOCHILA COMPLETAMENTE ---" << endl;
                mostrarListaPersonajes(listaPersonajes);
                
                int idPersonaje;
                cout << "ID del personaje: ";
                idPersonaje = leerEntero(1, 10000);
                
                Personaje* personaje = buscarPorId(listaPersonajes, idPersonaje);
                if (!personaje) {
                    cout << "Personaje no encontrado." << endl;
                    break;
                }
                
                cout << "¿Está seguro de vaciar completamente la mochila de " << personaje->nombre << "? (1 = Sí, 0 = No): ";
                int confirmar = leerEntero(0, 1);
                
                if (confirmar == 1) {
                    limpiarMochilaCompletamente(personaje);
                    cout << "Mochila vaciada completamente. Ahora puede agregar nuevos items." << endl;
                } else {
                    cout << "Operación cancelada." << endl;
                }
                break;
            }
            
            case 6:
                cout << "Volviendo al menu principal..." << endl;
                break;
                
            default:
                cout << "Opcion no valida." << endl;
        }
    } while (opcion != 6);
}

// menu princp.
void mostrarMenuPrincipal() {
    cout << "\n";
    cout << "==============================================================" << endl;
    cout << "            EL ROBO AL TREN DEL ORO - MENU PRINCIPAL          " << endl;
    cout << "==============================================================" << endl;
    cout << "\n";
    cout << " 1. Gestion de Personajes       6. Gestion de Equipos" << endl;
    cout << " 2. Gestion de Items            7. Gestion de Mochilas" << endl;
    cout << " 3. Mostrar Personajes          8. Mostrar Mapa" << endl;
    cout << " 4. Mostrar Items               9. JUGAR PARTIDA" << endl;
    cout << " 5. Mostrar Estado de Juego     0. Salir del Sistema" << endl;
    cout << "\n";
    cout << "Seleccione una opcion: ";
}


int main() {
    srand(time(0)); 
    
    cout << "==============================================================" << endl;
    cout << "            EL ROBO AL TREN DEL ORO - SISTEMA INICIADO        " << endl;
    cout << "==============================================================" << endl;
    
    // Verificas y crear archivos si no existen
    crearArchivosPorDefecto();
    
    // Inicializar estructuras
    ListaPersonajes listaPersonajes;
    listaPersonajes.cabeza = nullptr;
    listaPersonajes.contadorId = 1;
    
    ListaItems listaItems;
    listaItems.cabeza = nullptr;
    listaItems.contadorId = 1;
    
    Juego juego;
    inicializarEquipos(&juego);
    
    // Cargar datos desde archivos .tren
    cout << "\n=== CARGANDO DATOS ===" << endl;
    cargarAccesoriosDesdeArchivo(&listaItems, "accesorios.tren");
    cargarPersonajesDesdeArchivo(&listaPersonajes, &listaItems, "personajes.tren");
    
    // Cargar mapa (pero no mostrarlo todavía)
    cargarMapaDesdeArchivo(&juego.mapa, "Mapa.tren");
    
    reorganizarEquipos(&juego, &listaPersonajes);
    
    cout << "\n¡Todos los datos han sido cargados exitosamente!" << endl;
    cout << "==============================================================" << endl;
    
    int opcion;
    do {
        mostrarMenuPrincipal();
        opcion = leerEntero(0, 9);

        switch (opcion) {
            case 1: 
                menuGestionPersonajes(&listaPersonajes); 
                break;
            case 2: 
                menuGestionItems(&listaItems); 
                break;
            case 3: 
                mostrarListaPersonajes(&listaPersonajes);
                break;
            case 4: 
                mostrarListaItems(&listaItems);
                break;
            case 5: 
                mostrarEstadoJuego(&juego);
                break;
            case 6: 
                menuGestionEquipos(&juego, &listaPersonajes);
                break;
            case 7: 
                menuGestionMochilas(&listaPersonajes, &listaItems);
                break;
            case 8: 
                mostrarMapa(&juego.mapa);
                break;
            case 9: 
                menuJugarPartida(&juego, &listaPersonajes);
                break;
            case 0: 
                cout << "Saliendo del sistema..." << endl;
                break;
            default:
                cout << "Opcion no valida." << endl;
        }

    } while (opcion != 0);

    // limpiar mem
    cout << "\nLimpiando toda la memoria del juego..." << endl;
    
    Personaje* actualPersonaje = listaPersonajes.cabeza;
    while (actualPersonaje != nullptr) {
        Personaje* temp = actualPersonaje;
        actualPersonaje = actualPersonaje->siguiente_global;
        
        limpiarMochilaCompletamente(temp);
        delete temp;
    }
    
    NodoItemGlobal* actualItemGlobal = listaItems.cabeza;
    while (actualItemGlobal != nullptr) {
        NodoItemGlobal* temp = actualItemGlobal;
        actualItemGlobal = actualItemGlobal->siguiente;
        
        if (temp->item != nullptr) {
            delete temp->item;
        }
        delete temp;
    }
    
    // Liberar mem delmapa
    Estacion* actualEstacion = juego.mapa.estaciones;
    while (actualEstacion != nullptr) {
        Estacion* temp = actualEstacion;
        actualEstacion = actualEstacion->siguiente;
        
        // Liberar conexiones
        ConexionMapa* actualConexion = temp->conexiones;
        while (actualConexion != nullptr) {
            ConexionMapa* tempConexion = actualConexion;
            actualConexion = actualConexion->siguiente;
            delete tempConexion;
        }
        
        delete temp;
    }
    
    cout << "Memoria liberada correctamente. ¡Hasta la próxima!" << endl;

    return 0;
}
