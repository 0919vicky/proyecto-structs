#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <cstdlib>
using namespace std;

//  enumeracion
enum Bando { POLICIA_HONESTO, LADRON, CORRUPTO };
enum Estado { ACTIVO, CAPTURADO, INHABILITADO };
enum TipoPersonaje {
    INSPECTOR_JEFE, PERITO_FORENSE, OFICIAL_ASALTO,
    ANALISTA_DATOS, NEGOCIADOR, POLICIA_CORRUPTO,
    CEREBRO, FANTASMA, MULO, SABOTEADOR, HACKER, LOBERO
};

// enum de la funcion 'inicializaritemsBase'
enum TipoItem {
    SCANNER_METALES, KIT_HUELLAS, ORDEN_ALLOGRAMIENTO,
    CHALECO_ANTIBALAS, GRABADORA_ENCUBIERTA, GANAS_MANOS,
    SENUELO_COMUNICACIONES, LLAVES_MANIPULADAS, FONDO_NEGRO,
    OTRO10, OTRO11, OTRO12, OTRO13, OTRO14, OTRO15,
    OTRO16, OTRO17, OTRO18, OTRO19, OTRO20
};

// estructura para habilidades
struct Habilidad {
    string nombre;
    string descripcion;
    int cooldown;
    int cooldownActual;
    bool disponible;
};

// estructura para 'item'
struct Item {
    int id;
    string nombre;
    string descripcion;
    TipoItem tipo;
    int durabilidad; 
    int usosRestantes;
    string efecto;
};

// nodo par la lista de items
struct NodoItem {
    Item* item;
    NodoItem* siguiente;
};

// estructurq para 'personaje'
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

    Habilidad habilidades[5]; // arreglo estatico para hqbilidades
    int numHabilidades;
    NodoItem* inventario;
    Personaje* siguiente_global; // Puntero para la ListaPersonajes
    Personaje* siguiente_equipo; // Puntero para la lista del Equipo
};

// estructura para equipo
struct Equipo {
     string nombre;
    Personaje* cabeza;
    int cantidadMiembros;
};

// estructura para el juego
struct Juego {
    Equipo equipos[3]; // 0: Honestos, 1: Ladrones, 2: Corruptos
    int oroTotal;
    int oroRecuperado;
    int ladronesCapturados;
    int corruptosDescubiertos;
    int rondaActual;
    bool juegoTerminado;
};

// estructura para lista de personajes
struct ListaPersonajes {
    Personaje* cabeza;
    int contadorId;
};

// estructura para lista de items
struct ListaItems {
    NodoItem* cabeza;
    int contadorId;
};

// prototipos para funciones usadas antes de su definicion
void agregarItem(ListaItems* lista, Item* item);
void agregarPersonaje(ListaPersonajes* lista, Personaje* p);

// funciones auxiliares

// ayuda a la lectura segura de enteros con rango
int leerEntero(int minVal, int maxVal) {
    int val;
    while (true) {
        if (!( cin >> val)) {
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

// funciones de inicializacio

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
            
        case ANALISTA_DATOS:
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Rastreo de Patrones", "Analiza movimientos de ladrones en el metro", 2);
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Vigilancia Digital", "Accede a camaras de seguridad del metro", 2);
            break;
            
        case NEGOCIADOR:
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Interrogatorio Eficaz", "Obtiene informacion adicional de civiles y ladrones capturados", 2);
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Testimonio Clave", "Extrae informacion vital de testigos", 2);
            break;
            
        case POLICIA_CORRUPTO:
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Sabotaje", "Borra evidencias criticas permanentemente", 2);
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Soborno", "Bloquea el movimiento de un policia honesto", 3);
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Rastreo Ilegal", "Rastrea oro para beneficio personal", 2);
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Fingir Investigacion", "Mantiene cobertura mientras sabotea", 1);
            break;
            
        case CEREBRO:
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Mover Fichas", "Da una acción adicional a un ladrón en el mismo radio", 2);
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Sembrar Duda", "Crea falsas pistas que señalan policías honestos como corruptos", 3);
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Plan Maestro", "Coordina multiples acciones de ladrones simultaneamente", 4);
            break;
            
        case FANTASMA:
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Sigilo Total", "Invisible para habilidades de rastreo por 2 turnos", 3);
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Escape Silencioso", "Escapa de capturas con mayor facilidad", 2);
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Movimiento Fantasma", "Se mueve sin dejar rastro en el mapa", 2);
            break;
            
        case MULO:
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Transporte Pesado", "Puede cargar el triple de oro por 3 turnos", 4);
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Carga Rapida", "Mueve oro mas rapido entre estaciones", 2);
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Resistencia", "Resiste mas dano cuando carga oro", 2);
            break;
            
        case SABOTEADOR:
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Pistas Falsas", "Coloca pistas que desvían a los policías", 2);
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Trampas", "Retrasa el avance de policías en una estación", 2);
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Sabotaje Sistemas", "Desactiva sistemas de seguridad temporalmente", 3);
            break;
            
        case HACKER:
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Interrumpir Comunicaciones", "Bloquea habilidades especiales de policías por 1 turno", 3);
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Scanner Ilegal", "Rastrea oro con mayor precision que policias", 2);
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Sobrecarga Sistemas", "Desactiva camaras y sensores en area amplia", 3);
            break;
            
        case LOBERO:
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Distraccion", "Mueve un policía contra su voluntad a una estación adyacente", 2);
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Senuelo", "Crea senuelos que atraen a policias a ubicaciones falsas", 2);
            inicializarHabilidad(&personaje->habilidades[personaje->numHabilidades++], 
                "Confusion Tactica", "Desordena formaciones policiales", 2);
            break;
    }
}

void inicializarItemsBase(ListaItems* listaItems) {
    //items para policias honestos
    agregarItem(listaItems, new Item{0, "Scanner de Metales", 
        "Permite rastrear oro en un radio de 2 estaciones",
        SCANNER_METALES, 3, 3, "Rastrear oro en área amplia"});
        
    agregarItem(listaItems, new Item{0, "Kit de Huellas Dactilares",
        "Mejora la acción Investigar, garantizando encontrar pista crucial",
        KIT_HUELLAS, 2, 2, "Investigación garantizada"});
        
    agregarItem(listaItems, new Item{0, "Orden de Allanamiento",
        "Permite Investigar o Capturar en una estación adyacente sin moverse",
        ORDEN_ALLOGRAMIENTO, 2, 2, "Acción a distancia"});
        
    agregarItem(listaItems, new Item{0, "Chaleco Antibalas",
        "Segunda oportunidad al fallar desafío de captura/escape",
        CHALECO_ANTIBALAS, 1, 1, "Protección en combate"});
        
    agregarItem(listaItems, new Item{0, "Grabadora Encubierta",
        "Si graba a un corrupto usando habilidad ilegal, lo captura al instante",
        GRABADORA_ENCUBIERTA, 1, 1, "Contra corrupción"});
    
    // items para ladrones
    agregarItem(listaItems, new Item{0, "Ganas de Manos",
        "Aumenta temporalmente la capacidad de carga de oro",
        GANAS_MANOS, 2, 2, "Más capacidad de oro"});
        
    agregarItem(listaItems, new Item{0, "Señuelo de Comunicaciones",
        "Crea falsa alerta de ladrón detectado en mapa de policías",
        SENUELO_COMUNICACIONES, 2, 2, "Distracción"});
        
    agregarItem(listaItems, new Item{0, "Llaves Manipuladas",
        "Abrir pasaje secreto, teletransportándose a cualquier estación conectada",
        LLAVES_MANIPULADAS, 1, 1, "Teletransporte"});
    
    // items para policias corruptos
    agregarItem(listaItems, new Item{0, "Fondo Negro",
        "Soborno potente que permite Sabotear de forma remota",
        FONDO_NEGRO, 1, 1, "Soborno avanzado"});
}

Personaje* crearPersonaje(const char* n, const char* d, TipoPersonaje t, Bando b, int cap) {
    Personaje* nuevo = new Personaje;
    nuevo->id = 0; // se asigna un id unico al ingresr a la lista
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
    
    // configurar puntos de vida segun tipo
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
    // policias honestos
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

    // ladrones
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

    // policias corruptos
    agregarPersonaje(listaPersonajes, crearPersonaje("Agente Smith",
        "Siempre en el lugar correcto en el momento correcto... demasiado correcto?",
        POLICIA_CORRUPTO, CORRUPTO, 4));
}

// lista de personajes

void agregarPersonaje(ListaPersonajes* lista, Personaje* p) {
    p->id = lista->contadorId++;
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

Personaje* buscarPorNombre(ListaPersonajes* lista,  string nombre) {
    Personaje* actual = lista->cabeza;
    while (actual != nullptr) {
        if (actual->nombre == nombre) return actual;
        actual = actual->siguiente_global;
    }
    return nullptr;
}

void eliminarPersonaje(ListaPersonajes* lista, int id) {
    if (lista->cabeza == nullptr) return;

    if (lista->cabeza->id == id) {
        Personaje* temp = lista->cabeza;
        lista->cabeza = lista->cabeza->siguiente_global;
        delete temp;
        return;
    }

    Personaje* actual = lista->cabeza;
    while (actual->siguiente_global != nullptr && actual->siguiente_global->id != id) {
        actual = actual->siguiente_global;
    }

    if (actual->siguiente_global != nullptr) {
        Personaje* temp = actual->siguiente_global;
        actual->siguiente_global = temp->siguiente_global;
        delete temp;
    }
}

void mostrarHabilidadesPersonaje(Personaje* personaje) {
     cout << "  Habilidades:" <<  endl;
    for (int i = 0; i < personaje->numHabilidades; i++) {
         cout << "    - " << personaje->habilidades[i].nombre;
         cout << " (CD: " << personaje->habilidades[i].cooldown << ")";
         cout << ": " << personaje->habilidades[i].descripcion <<  endl;
    }
}

void mostrarListaPersonajes(ListaPersonajes* lista) {
    Personaje* actual = lista->cabeza;
    int contador = 1;
     cout << "==============================================================" <<  endl;
     cout << "                       LISTA DE PERSONAJES                      " <<  endl;
     cout << "==============================================================" <<  endl;

    while (actual != nullptr) {
         cout << "[" << contador << "] " << actual->nombre << " (ID: " << actual->id << ")" <<  endl;
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
         cout << " | PV: " << actual->puntosVida << "/" << actual->puntosVidaMax;
         cout << " | Oro: " << actual->cargaOro << "/" << actual->capacidadOro <<  endl;
         cout << "  " << actual->descripcion <<  endl;
        
        mostrarHabilidadesPersonaje(actual);
        
         cout << "  --------------------------------------------------------------" <<  endl;
        actual = actual->siguiente_global;
        contador++;
    }

    if (contador == 1) {
         cout << "  No hay personajes creados." <<  endl;
    }
}

// lista de items

void agregarItem(ListaItems* lista, Item* item) {
    //asignar id a cada item
    if (item->id == 0) {
        item->id = lista->contadorId++;
    }
    NodoItem* nuevo = new NodoItem;
    nuevo->item = item;
    nuevo->siguiente = lista->cabeza;
    lista->cabeza = nuevo;
}

Item* buscarItemPorId(ListaItems* lista, int id) {
    NodoItem* actual = lista->cabeza;
    while (actual != nullptr) {
        if (actual->item->id == id) return actual->item;
        actual = actual->siguiente;
    }
    return nullptr;
}

Item* buscarItemPorNombre(ListaItems* lista,  string nombre) {
    NodoItem* actual = lista->cabeza;
    while (actual != nullptr) {
        if (actual->item->nombre == nombre) return actual->item;
        actual = actual->siguiente;
    }
    return nullptr;
}

bool eliminarItem(ListaItems* lista, int id) {
    if (lista->cabeza == nullptr) return false;

    if (lista->cabeza->item->id == id) {
        NodoItem* temp = lista->cabeza;
        lista->cabeza = lista->cabeza->siguiente;
        delete temp->item;
        delete temp;
        return true;
    }

    NodoItem* actual = lista->cabeza;
    while (actual->siguiente != nullptr && actual->siguiente->item->id != id) {
        actual = actual->siguiente;
    }

    if (actual->siguiente != nullptr) {
        NodoItem* temp = actual->siguiente;
        actual->siguiente = temp->siguiente;
        delete temp->item;
        delete temp;
        return true;
    }
    return false;
}

void mostrarListaItems(ListaItems* lista) {
    NodoItem* actual = lista->cabeza;
    int contador = 1;
     cout << "==============================================================" <<  endl;
     cout << "                         LISTA DE ITEMS                         " <<  endl;
     cout << "==============================================================" <<  endl;

    while (actual != nullptr) {
         cout << "[" << contador << "] " << actual->item->nombre << " (ID: " << actual->item->id << ")" <<  endl;
         cout << "  Tipo: " << actual->item->descripcion <<  endl;
         cout << "  Usos: " << actual->item->usosRestantes << "/" << actual->item->durabilidad <<  endl;
         cout << "  Efecto: " << actual->item->efecto <<  endl;
         cout << "  --------------------------------------------------------------" <<  endl;
        actual = actual->siguiente;
        contador++;
    }

    if (contador == 1) {
         cout << "  No hay items creados." <<  endl;
    }
}

// gestion de equipos

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
        // agrega al inicio de la lista del equipo
        personaje->siguiente_equipo = juego->equipos[indiceEquipo].cabeza;
        juego->equipos[indiceEquipo].cabeza = personaje;
        juego->equipos[indiceEquipo].cantidadMiembros++;
    }
}

void reorganizarEquipos(Juego* juego, ListaPersonajes* listaPersonajes) {
    // se reinician todos los equipos
    inicializarEquipos(juego);
    
    // se reasignan todos los personajes a su equipo correspondiente
    Personaje* actual = listaPersonajes->cabeza;
    while (actual != nullptr) {
        agregarPersonajeAEquipo(juego, actual);
        actual = actual->siguiente_global;
    }
}

void mostrarEquipos(Juego* juego) {
     cout << "\n==============================================================" <<  endl;
     cout << "                       GESTION DE EQUIPOS                      " <<  endl;
     cout << "==============================================================" <<  endl;
    
    for (int i = 0; i < 3; i++) {
         cout << "\n--- " << juego->equipos[i].nombre << " ---" <<  endl;
         cout << "Cantidad de miembros: " << juego->equipos[i].cantidadMiembros <<  endl;
        
        Personaje* actual = juego->equipos[i].cabeza;
        int contador = 1;
        
        while (actual != nullptr) {
             cout << contador << ". " << actual->nombre << " (ID: " << actual->id << ")" <<  endl;
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
             cout << " | PV: " << actual->puntosVida << "/" << actual->puntosVidaMax <<  endl;
            actual = actual->siguiente_equipo;
            contador++;
        }
        
        if (contador == 1) {
             cout << "  No hay miembros en este equipo." <<  endl;
        }
    }
}

void mostrarPersonajesDisponiblesPorBando(ListaPersonajes* listaPersonajes, Bando bando) {
    Personaje* actual = listaPersonajes->cabeza;
    int contador = 1;
     cout << "\n--- PERSONAJES DISPONIBLES PARA ";

    switch (bando) {
        case POLICIA_HONESTO:  cout << "POLICIAS HONESTOS"; break;
        case LADRON:  cout << "LADRONES"; break;
        case CORRUPTO:  cout << "POLICIAS CORRUPTOS"; break;
    }
     cout << " ---" <<  endl;

    while (actual != nullptr) {
        
        bool esApropiado = false;
        
        switch (bando) {
            case POLICIA_HONESTO:
                esApropiado = (actual->tipo == INSPECTOR_JEFE || actual->tipo == PERITO_FORENSE ||
                              actual->tipo == OFICIAL_ASALTO || actual->tipo == ANALISTA_DATOS ||
                              actual->tipo == NEGOCIADOR);
                break;
            case LADRON:
                esApropiado = (actual->tipo == CEREBRO || actual->tipo == FANTASMA ||
                              actual->tipo == MULO || actual->tipo == SABOTEADOR ||
                              actual->tipo == HACKER || actual->tipo == LOBERO);
                break;
            case CORRUPTO:
                esApropiado = (actual->tipo == POLICIA_CORRUPTO);
                break;
        }

        if (esApropiado) {
             cout << "[" << contador << "] " << actual->nombre << " (ID: " << actual->id << ")" <<endl;
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
             cout << " | Bando actual: ";
            switch (actual->bando) {
                case POLICIA_HONESTO:  cout << "Policia Honesto"; break;
                case LADRON:  cout << "Ladron"; break;
                case CORRUPTO:  cout << "Corrupto"; break;
            }
             cout <<  endl;
            contador++;
        }
        actual = actual->siguiente_global;
    }

    if (contador == 1) {
         cout << "  No hay personajes disponibles para este bando." <<  endl;
    }
}

void menuGestionEquipos(Juego* juego, ListaPersonajes* listaPersonajes) {
    int opcion;
    do {
         cout << "\n==============================================================" << endl;
         cout << "                       GESTION DE EQUIPOS                      " <<endl;
         cout << "==============================================================" <<endl;
         cout << "1. Mostrar equipos organizados" <<endl;
         cout << "2. Asignar personaje a equipo" << endl;
         cout << "3. Ver personajes disponibles por equipo" << endl;
         cout << "4. Volver al Menu Principal" <<  endl;
         cout << "Seleccione una opcion: ";
        opcion = leerEntero(1, 4);

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
                 cout <<  endl;
                
                 cout << "Nuevo bando (1-3): " <<  endl;
                 cout << "1. Policia Honesto  2. Ladron  3. Corrupto" << endl;
                int nuevoBando = leerEntero(1, 3);
                Bando bandoEnum = static_cast<Bando>(nuevoBando - 1);
                
                // cambiar el bando del personaj
                personaje->bando = bandoEnum;
                
                
                reorganizarEquipos(juego, listaPersonajes);
                
                 cout << "Personaje asignado al nuevo equipo exitosamente!" << endl;
                break;
            }
            
            case 3: {
                 cout << "\n--- VER PERSONAJES DISPONIBLES POR EQUIPO ---" << endl;
                 cout << "Seleccione el equipo para ver personajes disponibles:" << endl;
                 cout << "1. Policia Honesto" <<  endl;
                 cout << "2. Ladron" <<  endl;
                 cout << "3. Corrupto" <<  endl;
                int opcionBando = leerEntero(1, 3);
                Bando bandoSeleccionado = static_cast<Bando>(opcionBando - 1);
                
                mostrarPersonajesDisponiblesPorBando(listaPersonajes, bandoSeleccionado);
                break;
            }
            
            case 4:
                 cout << "Volviendo al menu principal..." <<  endl;
                break;
                
            default:
                 cout << "Opcion no valida." <<  endl;
        }
    } while (opcion != 4);
}

// gestion de mochilas

void mostrarMochilaPersonaje(Personaje* personaje) {
     cout << "\n--- MOCHILA DE " << personaje->nombre << " ---" <<  endl;
    
    if (personaje->inventario == nullptr) {
         cout << "  (Mochila vacia)" <<  endl;
        return;
    }
    
    NodoItem* actual = personaje->inventario;
    int contador = 1;
    
    while (actual != nullptr) {
         cout << contador << ". " << actual->item->nombre << " (ID: " << actual->item->id << ")" <<  endl;
         cout << "   Descripcion: " << actual->item->descripcion <<  endl;
         cout << "   Usos: " << actual->item->usosRestantes << "/" << actual->item->durabilidad <<  endl;
         cout << "   Efecto: " << actual->item->efecto <<  endl;
         cout << "   -----------------------------------------" <<  endl;
        actual = actual->siguiente;
        contador++;
    }
}

void agregarItemAMochila(Personaje* personaje, Item* item) {
    NodoItem* nuevo = new NodoItem;
    nuevo->item = item;
    nuevo->siguiente = personaje->inventario;
    personaje->inventario = nuevo;
}

void eliminarItemDeMochila(Personaje* personaje, int idItem) {
    if (personaje->inventario == nullptr) {
         cout << "La mochila esta vacia." <<  endl;
        return;
    }

    
    if (personaje->inventario->item->id == idItem) {
        NodoItem* temp = personaje->inventario;
        personaje->inventario = personaje->inventario->siguiente;
        delete temp;
         cout << "Item eliminado de la mochila." <<  endl;
        return;
    }

    // buscar el item en la lista
    NodoItem* actual = personaje->inventario;
    while (actual->siguiente != nullptr && actual->siguiente->item->id != idItem) {
        actual = actual->siguiente;
    }

    if (actual->siguiente != nullptr) {
        NodoItem* temp = actual->siguiente;
        actual->siguiente = temp->siguiente;
        delete temp;
         cout << "Item eliminado de la mochila." <<  endl;
    } else {
         cout << "Item no encontrado en la mochila." <<  endl;
    }
}

void menuGestionMochilas(ListaPersonajes* listaPersonajes, ListaItems* listaItems) {
    int opcion;
    do {
         cout << "\n==============================================================" <<  endl;
         cout << "                       GESTION DE MOCHILAS                     " <<  endl;
         cout << "==============================================================" <<  endl;
         cout << "1. Ver mochila de personaje" <<  endl;
         cout << "2. Agregar item a mochila" <<  endl;
         cout << "3. Eliminar item de mochila" <<  endl;
         cout << "4. Volver al Menu Principal" <<  endl;
         cout << "Seleccione una opcion: ";
        opcion = leerEntero(1, 4);

        switch (opcion) {
            case 1: {
                 cout << "\n--- VER MOCHILA DE PERSONAJE ---" <<  endl;
                mostrarListaPersonajes(listaPersonajes);
                
                int idPersonaje;
                 cout << "ID del personaje: ";
                idPersonaje = leerEntero(1, 10000);
                
                Personaje* personaje = buscarPorId(listaPersonajes, idPersonaje);
                if (!personaje) {
                     cout << "Personaje no encontrado." <<  endl;
                    break;
                }
                
                mostrarMochilaPersonaje(personaje);
                break;
            }
            
            case 2: {
                 cout << "\n--- AGREGAR ITEM A MOCHILA ---" <<  endl;
                mostrarListaPersonajes(listaPersonajes);
                
                int idPersonaje;
                 cout << "ID del personaje: ";
                idPersonaje = leerEntero(1, 10000);
                
                Personaje* personaje = buscarPorId(listaPersonajes, idPersonaje);
                if (!personaje) {
                     cout << "Personaje no encontrado." <<  endl;
                    break;
                }
                
                mostrarListaItems(listaItems);
                
                int idItem;
                 cout << "ID del item a agregar: ";
                idItem = leerEntero(1, 10000);
                
                Item* item = buscarItemPorId(listaItems, idItem);
                if (!item) {
                     cout << "Item no encontrado." <<  endl;
                    break;
                }
                
                agregarItemAMochila(personaje, item);
                 cout << "Item agregado a la mochila de " << personaje->nombre << "!" <<  endl;
                break;
            }
            
            case 3: {
                 cout << "\n--- ELIMINAR ITEM DE MOCHILA ---" <<  endl;
                mostrarListaPersonajes(listaPersonajes);
                
                int idPersonaje;
                 cout << "ID del personaje: ";
                idPersonaje = leerEntero(1, 10000);
                
                Personaje* personaje = buscarPorId(listaPersonajes, idPersonaje);
                if (!personaje) {
                     cout << "Personaje no encontrado." <<  endl;
                    break;
                }
                
                mostrarMochilaPersonaje(personaje);
                
                int idItem;
                 cout << "ID del item a eliminar: ";
                idItem = leerEntero(1, 10000);
                
                eliminarItemDeMochila(personaje, idItem);
                break;
            }
            
            case 4:
                 cout << "Volviendo al menu principal..." <<  endl;
                break;
                
            default:
                 cout << "Opcion no valida." <<  endl;
        }
    } while (opcion != 4);
}

//gestion de personajes

void menuGestionPersonajes(ListaPersonajes* listaPersonajes) {
    int opcion;
    do {
         cout << "\n==============================================================" <<  endl;
         cout << "                       GESTION DE PERSONAJES                         " <<  endl;
         cout << "==============================================================" <<  endl;
         cout << "1. Crear Personaje" <<  endl;
         cout << "2. Modificar Personaje" <<  endl;
         cout << "3. Eliminar Personaje" <<  endl;
         cout << "4. Mostrar Personajes" <<  endl;
         cout << "5. Buscar Personaje" <<  endl;
         cout << "6. Volver al Menu Principal" <<  endl;
         cout << "Seleccione una opcion: ";
        opcion = leerEntero(1, 6);

        switch (opcion) {
            case 1: {
                 string nombre, descripcion;
                int tipo, bando, capacidad;
                
                 cout << "\n--- CREAR NUEVO PERSONAJE ---" <<  endl;
                 cout << "Nombre: ";
                getline( cin, nombre);
                 cout << "Descripcion: ";
                getline( cin, descripcion);
                
                 cout << "Tipo (1-12): " <<  endl;
                 cout << "1. Inspector Jefe  2. Perito Forense  3. Oficial Asalto" <<  endl;
                 cout << "4. Analista Datos  5. Negociador      6. Policia Corrupto" <<  endl;
                 cout << "7. Cerebro         8. Fantasma        9. Mulo" <<  endl;
                 cout << "10. Saboteador     11. Hacker         12. Lobero" <<  endl;
                tipo = leerEntero(1, 12);
                
                 cout << "Bando (1-3): " <<  endl;
                 cout << "1. Policia Honesto  2. Ladron  3. Corrupto" <<  endl;
                bando = leerEntero(1, 3);
                
                 cout << "Capacidad de oro: ";
                capacidad = leerEntero(1, 1000);
                
                TipoPersonaje tipoEnum = static_cast<TipoPersonaje>(tipo - 1);
                Bando bandoEnum = static_cast<Bando>(bando - 1);
                
                
                Personaje* nuevo = crearPersonaje(nombre.c_str(), descripcion.c_str(), tipoEnum, bandoEnum, capacidad);
                agregarPersonaje(listaPersonajes, nuevo);
                
                 cout << "Personaje creado exitosamente con ID: " << nuevo->id << "!" <<  endl;
                break;
            }
            
            case 2: {
                 cout << "\n--- MODIFICAR PERSONAJE ---" <<  endl;
                mostrarListaPersonajes(listaPersonajes);
                int id;
                 cout << "ID del personaje a modificar: ";
                id = leerEntero(1, 10000);
                
                Personaje* personaje = buscarPorId(listaPersonajes, id);
                if (personaje) {
                     string nuevoNombre, nuevaDesc;
                     cout << "Nuevo nombre (actual: " << personaje->nombre << "): ";
                    getline( cin, nuevoNombre);
                     cout << "Nueva descripcion: ";
                    getline( cin, nuevaDesc);
                    
                    if (!nuevoNombre.empty()) personaje->nombre = nuevoNombre;
                    if (!nuevaDesc.empty()) personaje->descripcion = nuevaDesc;
                    
                     cout << "Personaje modificado exitosamente!" <<  endl;
                } else {
                     cout << "Personaje no encontrado." <<  endl;
                }
                break;
            }
            
            case 3: {
                 cout << "\n--- ELIMINAR PERSONAJE ---" <<  endl;
                mostrarListaPersonajes(listaPersonajes);
                int id;
                 cout << "ID del personaje a eliminar: ";
                id = leerEntero(1, 10000);
                
                Personaje* personaje = buscarPorId(listaPersonajes, id);
                if (personaje) {
                    eliminarPersonaje(listaPersonajes, id);
                     cout << "Personaje eliminado exitosamente!" <<  endl;
                } else {
                     cout << " Personaje no encontrado." <<  endl;
                }
                break;
            }
            
            case 4:
                mostrarListaPersonajes(listaPersonajes);
                break;
                
            case 5: {
                 string nombre;
                 cout << "Nombre del personaje a buscar: ";
                getline( cin, nombre);
                
                Personaje* personaje = buscarPorNombre(listaPersonajes, nombre);
                if (personaje) {
                     cout << "Personaje encontrado:" <<  endl;
                     cout << "ID: " << personaje->id << " | Nombre: " << personaje->nombre <<  endl;
                     cout << "Descripcion: " << personaje->descripcion <<  endl;
                    mostrarHabilidadesPersonaje(personaje);
                } else {
                     cout << "Personaje no encontrado." <<  endl;
                }
                break;
            }
            
            case 6:
                 cout << "Volviendo al menú principal..." <<  endl;
                break;
                
            default:
                 cout << "Opcion no valida." <<  endl;
        }
    } while (opcion != 6);
}

void menuGestionItems(ListaItems* listaItems) {
    int opcion;
    do {
         cout << "\n==============================================================" <<  endl;
         cout << "                       GESTION DE ITEMS                         " <<  endl;
         cout << "==============================================================" <<  endl;
         cout << "1. Crear Item" <<  endl;
         cout << "2. Modificar Item" <<  endl;
         cout << "3. Eliminar Item" <<  endl;
         cout << "4. Listar Items" <<  endl;
         cout << "5. Buscar Item" <<  endl;
         cout << "6. Volver al Menu Principal" <<  endl;
         cout << "Seleccione una opcion: ";
        opcion = leerEntero(1, 6);

        switch (opcion) {
            case 1: {
                 string nombre, descripcion, efecto;
                int tipo, durabilidad;
                 cout << "\n--- CREAR NUEVO ITEM ---" <<  endl;
                 cout << "Nombre: ";
                getline( cin, nombre);
                 cout << "Descripcion: ";
                getline( cin, descripcion);
                 cout << "Efecto: ";
                getline( cin, efecto);
                 cout << "Durabilidad: ";
                durabilidad = leerEntero(1, 1000);
                 cout << "Tipo (1-20): ";
                tipo = leerEntero(1, 20);
                TipoItem tipoEnum = static_cast<TipoItem>(tipo - 1);
                
                Item* nuevoItem = new Item{0, nombre, descripcion, tipoEnum, durabilidad, durabilidad, efecto};
                agregarItem(listaItems, nuevoItem);
                 cout << "Item creado exitosamente con ID: " << nuevoItem->id << "!" <<  endl;
                break;
            }
            
            case 2: {
                 cout << "\n--- MODIFICAR ITEM ---" <<  endl;
                mostrarListaItems(listaItems);
                 cout << "ID del item a modificar: ";
                int id = leerEntero(1, 10000);
                Item* it = buscarItemPorId(listaItems, id);
                if (!it) { 
                     cout << "Item no encontrado." <<  endl; 
                    break; 
                }
                 string nombre, descripcion, efecto;
                int durabilidad;
                 cout << "Nuevo nombre (actual: " << it->nombre << "): ";
                getline( cin, nombre);
                 cout << "Nueva descripcion: ";
                getline( cin, descripcion);
                 cout << "Nuevo efecto: ";
                getline( cin, efecto);
                 cout << "Nueva durabilidad: ";
                durabilidad = leerEntero(1, 1000);
                if (!nombre.empty()) it->nombre = nombre;
                if (!descripcion.empty()) it->descripcion = descripcion;
                if (!efecto.empty()) it->efecto = efecto;
                it->durabilidad = durabilidad;
                it->usosRestantes = durabilidad;
                 cout << "Item modificado exitosamente." <<  endl;
                break;
            }
            
            case 3: {
                 cout << "\n--- ELIMINAR ITEM ---" <<  endl;
                mostrarListaItems(listaItems);
                 cout << "ID del item a eliminar: ";
                int id = leerEntero(1, 10000);
                if (eliminarItem(listaItems, id)) {
                     cout << "Item eliminado." <<  endl;
                } else {
                     cout << "Item no encontrado." <<  endl;
                }
                break;
            }
            
            case 4:
                mostrarListaItems(listaItems);
                break;
                
            case 5: {
                 string nombre;
                 cout << "Nombre del item a buscar: ";
                getline( cin, nombre);
                
                Item* item = buscarItemPorNombre(listaItems, nombre);
                if (item) {
                     cout << "Item encontrado:" <<  endl;
                     cout << "ID: " << item->id << " | Nombre: " << item->nombre <<  endl;
                     cout << "Descripcion: " << item->descripcion <<  endl;
                     cout << "Efecto: " << item->efecto <<  endl;
                     cout << "Usos: " << item->usosRestantes << "/" << item->durabilidad <<  endl;
                } else {
                     cout << "Item no encontrado." <<  endl;
                }
                break;
            }
            
            case 6:
                 cout << "Volviendo al menu principal..." <<  endl;
                break;
                
            default:
                 cout << "Opcion no valida." <<  endl;
        }
    } while (opcion != 6);
}

void mostrarMenuPrincipal() {
     cout << "\n";
     cout << "==============================================================" << endl;
     cout << "           EL ROBO AL TREN DEL ORO - MENU           " <<  endl;
     cout << "==============================================================" <<endl;
     cout << "\n";
     cout << "Bienvenido al sistema de gestion del robo al tren del oro" <<endl;
     cout << "\n";
     cout << " 1. Gestion de Personajes       4. Gestion de Mochilas" <<endl;
     cout << " 2. Gestion de Implementos      5. Gestion de Mapas   " <<endl;
     cout << " 3. Gestion de Equipos          6. Consultar Equipo   " <<endl;
     cout << "\n";
     cout << " 7. Simular Juego Completo      8. Ver Bitacora       " <<endl;
     cout << " 9. Salir del Sistema" <<endl;
     cout << "\n";
     cout << "Seleccione una opcion: ";
}


int main() {
    srand(time(0)); 
    
    
    ListaPersonajes listaPersonajes;
    listaPersonajes.cabeza = nullptr;
    listaPersonajes.contadorId = 1;
    
    ListaItems listaItems;
    listaItems.cabeza = nullptr;
    listaItems.contadorId = 1;
    
    Juego juego;
    inicializarEquipos(&juego);
    
    
    inicializarItemsBase(&listaItems);
    crearPersonajesBase(&listaPersonajes);
    
    // se asignan personajes a equipos
    reorganizarEquipos(&juego, &listaPersonajes);
    
     cout << "==============================================================" <<endl;
     cout << "           EL ROBO AL TREN DEL ORO v1.0           " <<endl;
     cout << "   Un juego de estrategia, traicion y toma de decisiones   " << endl;
     cout << "==============================================================" <<endl;

    int opcion;
    do {
        mostrarMenuPrincipal();
        opcion = leerEntero(1, 9);

        switch (opcion) {
            case 1: 
                menuGestionPersonajes(&listaPersonajes); 
                break;
            case 2: 
                menuGestionItems(&listaItems); 
                break;
            case 3: 
                menuGestionEquipos(&juego, &listaPersonajes);
                break;
            case 4: 
                menuGestionMochilas(&listaPersonajes, &listaItems);
                break;
            case 5: 
                 cout << "Gestion de Mapas - Proximamente..." <<endl;
                break;
            case 6: 
                 cout << "Consultar Equipo - Proximamente..." <<endl;
                break;
            case 7: 
                 cout << "Simular Juego - Proximamente..." <<endl;
                break;
            case 8: 
                 cout << "Ver Bitacora - Proximamente..." <<endl;
                break;
            case 9: 
                 cout << "Gracias por jugar a El Robo al Tren del Oro!" <<endl;
                break;
            default:
                 cout << " Opcion no valida. Por favor, seleccione 1-9." <<endl;
        }

    } while (opcion != 9);

    // sirve para limpiar memoria
    Personaje* actualPersonaje = listaPersonajes.cabeza;
    while (actualPersonaje != nullptr) {
        Personaje* temp = actualPersonaje;
        actualPersonaje = actualPersonaje->siguiente_global;
        
        // se limpia el inventario del personaje
        NodoItem* actualItem = temp->inventario;
        while (actualItem != nullptr) {
            NodoItem* tempItem = actualItem;
            actualItem = actualItem->siguiente;
            delete tempItem;
        }
        
        delete temp;
    }
    
    NodoItem* actualItemGlobal = listaItems.cabeza;
    while (actualItemGlobal != nullptr) {
        NodoItem* temp = actualItemGlobal;
        actualItemGlobal = actualItemGlobal->siguiente;
        delete temp->item;
        delete temp;
    }

    return 0;
}
