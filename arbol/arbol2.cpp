#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

class Nodo {
public:
    int id;
    vector<Nodo*> hijos;

    Nodo(int valor) {
        id = valor;
    }
};

class Arbol {
private:
    Nodo* raiz;
    vector<Nodo*> todos;


    void mostrarRecursivo(Nodo* nodo, string espacio, bool ultimo) {
        cout << espacio;
        if(nodo != raiz) cout << "+-- ";
        cout << nodo->id << endl;

        for(int i = 0; i < nodo->hijos.size(); i++) {
            mostrarRecursivo(nodo->hijos[i], espacio + (ultimo ? "    " : "¦   "), i == nodo->hijos.size() - 1);
        }
    }

    int contarNodosRec(Nodo* nodo) {
        if (nodo == NULL) return 0;
        int count = 1;
        for (Nodo* hijo : nodo->hijos) {
            count += contarNodosRec(hijo);
        }
        return count;
    }

    void preordenRec(Nodo* nodo) {
        if (nodo == NULL) return;
        cout << nodo->id << " ";
        for (Nodo* hijo : nodo->hijos) {
            preordenRec(hijo);
        }
    }

    void posordenRec(Nodo* nodo) {
        if (nodo == NULL) return;
        for (Nodo* hijo : nodo->hijos) {
            posordenRec(hijo);
        }
        cout << nodo->id << " ";
    }


    void guardarTXTRec(Nodo* nodo, ofstream &archivo) {
        archivo << "Nodo " << nodo->id << " -> ";
        for(int i = 0; i < nodo->hijos.size(); i++) {
            archivo << nodo->hijos[i]->id << (i < nodo->hijos.size() - 1 ? "," : "");
        }
        archivo << endl;
        for(auto hijo : nodo->hijos) guardarTXTRec(hijo, archivo);
    }

    void guardarXMLRec(Nodo* nodo, ofstream &archivo, int nivel) {
        string tab(nivel * 4, ' ');
        archivo << tab << "<nodo id=\"" << nodo->id << "\">" << endl;
        for(auto hijo : nodo->hijos) guardarXMLRec(hijo, archivo, nivel + 1);
        archivo << tab << "</nodo>" << endl;
    }

public:
    Arbol() : raiz(NULL) {}

    void generar(int cantidadNodos, int ramasMax) {
        if(cantidadNodos <= 0) return;
        todos.clear();
        for(int i = 1; i <= cantidadNodos; i++) todos.push_back(new Nodo(i));
        raiz = todos[0];
        int siguiente = 1;
        for(int i = 0; i < cantidadNodos && siguiente < cantidadNodos; i++) {
            for(int j = 0; j < ramasMax && siguiente < cantidadNodos; j++) {
                todos[i]->hijos.push_back(todos[siguiente++]);
            }
        }
    }


    void mostrarReportes() {
        cout << "\n--- ESTADISTICAS DEL ARBOL ---" << endl;
        cout << "Total de nodos: " << contarNodosRec(raiz) << endl;
        
        cout << "Recorrido Preorden: ";
        preordenRec(raiz);
        cout << endl;

        cout << "Recorrido Posorden: ";
        posordenRec(raiz);
        cout << endl;
    }

    void mostrar() {
        cout << "\nARBOL GENERADO\n\n";
        mostrarRecursivo(raiz, "", true);
    }

    void guardarTXT() {
        ofstream archivo("arbol.txt");
        guardarTXTRec(raiz, archivo);
        archivo.close();
    }

    void guardarJSON() {
        ofstream archivo("arbol.json");
        archivo << "{\n";
        for(int i = 0; i < todos.size(); i++) {
            archivo << "  \"" << todos[i]->id << "\":[";
            for(int j = 0; j < todos[i]->hijos.size(); j++) {
                archivo << todos[i]->hijos[j]->id << (j < todos[i]->hijos.size() - 1 ? "," : "");
            }
            archivo << "]" << (i < todos.size() - 1 ? "," : "") << endl;
        }
        archivo << "}";
        archivo.close();
    }

    void guardarXML() {
        ofstream archivo("arbol.xml");
        archivo << "<?xml version=\"1.0\"?>\n<arbol>\n";
        guardarXMLRec(raiz, archivo, 1);
        archivo << "</arbol>";
        archivo.close();
    }

    ~Arbol() {
        for(auto n : todos) delete n;
    }
};

int main() {
    int nodos, ramas;
    cout << "Cantidad de nodos: "; cin >> nodos;
    cout << "Ramas maximas por nodo: "; cin >> ramas;

    Arbol arbol;
    arbol.generar(nodos, ramas);
    arbol.mostrar();
    

    arbol.mostrarReportes();

    arbol.guardarTXT();
    arbol.guardarJSON();
    arbol.guardarXML();

    cout << "\nArchivos creados: arbol.txt, arbol.json, arbol.xml" << endl;
    return 0;
}
