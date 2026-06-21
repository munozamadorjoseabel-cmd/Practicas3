#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <queue>
#include <climits>
#include <algorithm>
#include <sstream>

using namespace std;

struct Arista {
    int destino;
    int costo;
    int tiempo;
};

class Digrafo {
private:
    int vertices;
    vector<vector<Arista>> adj;

public:
    // Constructor por defecto o con vertices iniciales
    Digrafo(int v = 0) {
        inicializar(v);
    }

    void inicializar(int v) {
        vertices = v;
        adj.assign(v + 1, vector<Arista>());
    }

    int obtenerVertices() const { return vertices; }

    // Asegura que el vector tenga espacio si el usuario añade un nodo con ID mayor
    void asegurarCapacidad(int v) {
        if (v > vertices) {
            vertices = v;
            adj.resize(vertices + 1);
        }
    }

    // En un digrafo, la arista es UNIDIRECCIONAL (solo de u hacia v)
    void agregarArista(int u, int v, int costo, int tiempo) {
        asegurarCapacidad(max(u, v));
        adj[u].push_back({v, costo, tiempo});
    }

    void mostrarEstructura() {
        cout << "\n=== ESTRUCTURA DEL DIGRAFO ACTUAL ===" << endl;
        cout << "Total de vertices registrados: " << vertices << endl;
        bool tieneAristas = false;
        for (int i = 1; i <= vertices; i++) {
            for (auto& a : adj[i]) {
                // Se muestran todas las aristas dirigidas sin filtrar duplicados
                cout << "  Origen: " << i << " --> Destino: " << a.destino 
                     << " [Costo: " << a.costo << ", Tiempo: " << a.tiempo << "]" << endl;
                tieneAristas = true;
            }
        }
        if (!tieneAristas) cout << "  (El digrafo no tiene aristas aun)" << endl;
        cout << "======================================\n" << endl;
    }

    // Algoritmo de Dijkstra para Digrafos
    void dijkstra(int inicio, int fin, bool porTiempo) {
        if (inicio > vertices || fin > vertices || inicio <= 0 || fin <= 0) {
            cout << "Nodos de inicio o fin invalidos." << endl;
            return;
        }

        vector<int> dist(vertices + 1, INT_MAX);
        vector<int> padre(vertices + 1, -1);
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

        dist[inicio] = 0;
        pq.push({0, inicio});

        while (!pq.empty()) {
            int d = pq.top().first;
            int u = pq.top().second;
            pq.pop();

            if (u == fin) break;
            if (d > dist[u]) continue;

            for (auto& arista : adj[u]) {
                int peso = porTiempo ? arista.tiempo : arista.costo;
                if (dist[u] + peso < dist[arista.destino]) {
                    dist[arista.destino] = dist[u] + peso;
                    padre[arista.destino] = u;
                    pq.push({dist[arista.destino], arista.destino});
                }
            }
        }

        cout << (porTiempo ? "\nMejor ruta por TIEMPO: " : "\nMejor ruta por COSTO: ");
        if (dist[fin] == INT_MAX) { cout << "No hay ruta de " << inicio << " a " << fin << "." << endl; return; }
        
        vector<int> ruta;
        for (int at = fin; at != -1; at = padre[at]) ruta.push_back(at);
        reverse(ruta.begin(), ruta.end());
        for (size_t i = 0; i < ruta.size(); i++) cout << ruta[i] << (i == ruta.size() - 1 ? "" : " -> ");
        cout << " (Total: " << dist[fin] << ")" << endl;
    }

    // Guardar en formato JSON (Guarda cada direccion de forma independiente)
    void guardarJSON(string nombreArchivo) {
        ofstream f(nombreArchivo);
        if (!f.is_open()) {
            cout << "Error al abrir el archivo para guardar." << endl;
            return;
        }
        f << "{\n  \"grafo\": [\n";
        bool primero = true;
        for (int i = 1; i <= vertices; i++) {
            for (auto& a : adj[i]) {
                if (!primero) f << ",\n";
                f << "    {\"origen\": " << i << ", \"destino\": " << a.destino 
                  << ", \"costo\": " << a.costo << ", \"tiempo\": " << a.tiempo << "}";
                primero = false;
            }
        }
        f << "\n  ]\n}";
        f.close();
        cout << "Digrafo guardado exitosamente en '" << nombreArchivo << "'" << endl;
    }

    // Cargar desde archivo JSON
    void cargarJSON(string nombreArchivo) {
        ifstream f(nombreArchivo);
        if (!f.is_open()) {
            cout << "AVISO: No se encontro '" << nombreArchivo << "'. Se iniciara con un digrafo vacio." << endl;
            return;
        }

        string linea;
        int max_v = 0;
        struct DatosArista { int u, v, c, t; };
        vector<DatosArista> aristasLeidas;

        while (getline(f, linea)) {
            if (linea.find("\"origen\":") != string::npos) {
                size_t p_orig = linea.find("\"origen\":") + 9;
                size_t p_dest = linea.find("\"destino\":") + 10;
                size_t p_cost = linea.find("\"costo\":") + 8;
                size_t p_tiem = linea.find("\"tiempo\":") + 9;

                int u = stoi(linea.substr(p_orig, linea.find(",", p_orig) - p_orig));
                int v = stoi(linea.substr(p_dest, linea.find(",", p_dest) - p_dest));
                int c = stoi(linea.substr(p_cost, linea.find(",", p_cost) - p_cost));
                int t = stoi(linea.substr(p_tiem, linea.find("}", p_tiem) - p_tiem));

                aristasLeidas.push_back({u, v, c, t});
                if (u > max_v) max_v = u;
                if (v > max_v) max_v = v;
            }
        }
        f.close();

        if (max_v > 0) {
            inicializar(max_v);
            for (auto& a : aristasLeidas) {
                agregarArista(a.u, a.v, a.c, a.t);
            }
            cout << "Digrafo cargado desde archivo con " << max_v << " vertices." << endl;
        }
    }
};

// Funcion para pedir los datos de las aristas dirigidas
void pedirYAgregarAristas(Digrafo &g) {
    int origen, destino, costo, tiempo;
    cout << "\n--- Ingresa las aristas dirigidas (Origen, Destino, Costo, Tiempo) ---" << endl;
    cout << "(Para terminar, ingresa -1 en el Nodo Origen)" << endl;
    
    while (true) {
        cout << "\nNodo Origen: ";
        cin >> origen;
        if (origen == -1) break;
        
        cout << "Nodo Destino: ";
        cin >> destino;
        cout << "Costo de la ruta: ";
        cin >> costo;
        cout << "Tiempo de la ruta: ";
        cin >> tiempo;

        if (origen <= 0 || destino <= 0) {
            cout << "Error: Los IDs de los nodos deben ser mayores a 0. Intenta de nuevo." << endl;
        } else {
            g.agregarArista(origen, destino, costo, tiempo);
            cout << "Arista dirigida [" << origen << " -> " << destino << "] agregada temporalmente." << endl;
            cout << "Nota: Si deseas un camino de regreso, tendras que agregarlo de forma inversa como otra arista." << endl;
        }
    }
}

int main() {
    string archivo = "datos_grafo.json";
    Digrafo g;

    // Lee los datos previos del JSON al iniciar
    g.cargarJSON(archivo);

    int opcion;
    do {
        cout << "\n================ MENU DE DIGRAFOS ================" << endl;
        cout << "1. Mostrar Digrafo Actual" << endl;
        cout << "2. Buscar Mejor Ruta (Dijkstra)" << endl;
        cout << "3. Modificar Digrafo (Agregar nuevas conexiones dirigidas)" << endl;
        cout << "4. Crear un Digrafo Nuevo desde cero" << endl;
        cout << "5. Guardar Cambios y Salir" << endl;
        cout << "==================================================" << endl;
        cout << "Selecciona una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1:
                g.mostrarEstructura();
                break;

            case 2: {
                if (g.obtenerVertices() == 0) {
                    cout << "Aviso: El digrafo esta vacio. Agrega datos primero." << endl;
                    break;
                }
                int inicio, fin;
                cout << "Nodo de partida: "; cin >> inicio;
                cout << "Nodo de destino: "; cin >> fin;
                
                g.dijkstra(inicio, fin, false); // Por costo
                g.dijkstra(inicio, fin, true);  // Por tiempo
                break;
            }

            case 3:
                cout << "\n[Modificando Digrafo] Recuerda que las aristas tienen direccion unica." << endl;
                pedirYAgregarAristas(g);
                break;

            case 4: {
                int v;
                cout << "\n[Creando Nuevo Digrafo] ¿Cual es el numero estimado de vertices iniciales?: ";
                cin >> v;
                g.inicializar(v);
                pedirYAgregarAristas(g);
                break;
            }

            case 5:
                g.guardarJSON(archivo);
                cout << "Saliendo del programa..." << endl;
                break;

            default:
                cout << "Opcion invalida. Intenta de nuevo." << endl;
        }
    } while (opcion != 5);

    return 0;
}
