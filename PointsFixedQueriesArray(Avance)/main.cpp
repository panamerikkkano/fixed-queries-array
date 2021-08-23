#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <string>
#include <algorithm>
#include "resource.h"
#include <cstdlib>
#include <math.h>
#include <windowsx.h>
#include "resource.h"

HINSTANCE hInst; // instancia
using namespace std;
int pxi,pyi,pxf,pyf,dd,color,Toler;
/*
    pxi, pyi, pxf, pfy = puntos que trazan el cuadro de entrada // input canvas
    dd = distandia dist || 20px
    (int) color === RGB(a,b,c)
*/
void SetPixel2(HDC hdc,int x,int y,COLORREF Color) // if pixel is inside the canvas.
{   if(x>pxi&&x<pxf&&y>pyi&&y<pyf)
        SetPixel(hdc, x, y, Color); // if x,y is inside the canvas. then set single pixel [.]  with color = color
}
void Draw8Points(HDC hdc,int xc,int yc, int a, int b,COLORREF Color)
{   SetPixel2(hdc, xc+a, yc+b, Color);
    SetPixel2(hdc, xc-a, yc+b, Color);
    SetPixel2(hdc, xc-a, yc-b, Color);
    SetPixel2(hdc, xc+a, yc-b, Color); // single pixel
    SetPixel2(hdc, xc+b, yc+a, Color); // circulo cuadrado 8 pixeles.
    SetPixel2(hdc, xc-b, yc+a, Color);
    SetPixel2(hdc, xc-b, yc-a, Color);
    SetPixel2(hdc, xc+b, yc-a, Color);
}
void CircleBresenham(HDC hdc,int xc,int yc, int R,int rrr,COLORREF color) // draw circle by angle a.
{   int x=0,y=R; // punto[x,y] =
    // R = radio
    int d=1-R;
    Draw8Points(hdc,xc,yc,x,y,color); //
    while(x<y)
    {   if(d<0)
            d+=2*x+2;
        else
        {   d+=2*(x-y)+5;
            y-=rrr;
        }
        x+=rrr;
        Draw8Points(hdc,xc,yc,x,y,color);
    }
}
class Punto
{   public:
    int x,y;
    Punto(int _x=0,int _y=0)
    {   x=_x;
        y=_y;
    }
};
class Nodo
{   public:
    Punto P;
    map <int,Nodo *> sig;
    Nodo(Punto PP=Punto(0,0))
    {   P=PP;
    }
};
class ArbolBK
{   public:
    Nodo *Raiz;
    map<int, Punto*> key_per_level; // llave por nivel <nivel, llave>;
    vector<vector<int>> table;
    ArbolBK()
    {   Raiz=NULL;
    }
    float distP(Punto P1,Punto P2)
    {   return (sqrt((P1.x-P2.x)*(P1.x-P2.x)+(P1.y-P2.y)*(P1.y-P2.y)))/dd;
    }
    void Update_table(string branch_path, string leaf){
        // string branch_path es un string que contiene las distancias: "2,1,4"
        // string leaf es la hoja a la que lleva la ruta branch_path (no lo usamos por el momento)
        // como el arbol es de altura fija entonces la altura debe ser igual a la cantidad de llaves + 1;
        cout <<"ruta completa | "<< branch_path <<" |\t"<<leaf<< endl;
        string dist;
        vector<int> temp;
        stringstream cins(branch_path);
        while(getline(cins,dist, ',')){
            temp.push_back(atoi(dist.c_str()));
        }
        table.push_back(temp);
    }
    void Insertar(Punto s)
    {   if(Raiz==NULL){
           Raiz=new Nodo(s);
           key_per_level[0] = new Punto(s);
           (Raiz->sig)[0] = new Nodo(s);
        }
        else
           Ins(Raiz,s, 0);
    }
    void Ins(Nodo *&R,Punto s, int level)
    {   int dist=distP(R->P,s);
        if(dist>0)
        {   if((R->sig).find(dist)==(R->sig).end()){
                if (key_per_level[level + 1] != NULL){ // verificamos si existe clave en ese nivel
                    key_per_level[level+1] = new Punto(R->sig[dist]->P);
                    R->sig[dist]->sig[0] = new Nodo(Punto(R->sig[dist]->P)); // si existe clave entonces copiamos la clave
                    Ins((R->sig)[dist],s,level + 1); // si reevaluamos desde el nodo clave.
                }else{
                    (R->sig)[dist]=new Nodo(s); // si no existe un nodo con la misma distancia entonces agregamos el nodo(palabra) como hijo del nodo actual;
                }
                (R->sig)[dist]=new Nodo(s);
            }else{
                if (key_per_level[level + 1] == NULL){// no keys in that level; key == NULL;
                    key_per_level[level+1] = new Punto(R->sig[dist]->P);
                    //Fixed_height_keys(Raiz, 0, level + 1);
                    R->sig[dist]->sig[0] = new Nodo(Punto(R->sig[dist]->P));
                    Ins((R->sig)[dist],s, level + 1);
                }else{ // en caso que ya exista un key en el nivel de la coincidencia;
                    Punto nodo_p = R->sig[dist]->P; // almacenando temporalmente el punto del nodo en conflicto.
                    R->sig[dist]->P = *key_per_level[level+1];
                    Ins(R->sig[dist],nodo_p,level + 1);
                    Ins(R->sig[dist],s,level + 1);
                }
            }
        }
    }
    void Fixed_height_keys(Nodo *&R, int it_level, int level){
        map <int,Nodo*>::iterator it;
        if (it_level + 1 == level){
            for(it=(R->sig).begin();it!=(R->sig).end();it++){
                int prevx = (*it).second->P.x;
                //(*it).second->P.x = key_per_level[level];
                Ins((*it).second,prevx,level);
            }
        }
        else{
            for(it=(R->sig).begin();it!=(R->sig).end();it++){
                Fixed_height_keys((*it).second,it_level + 1 , level);
            }
        }
    }
    void Mostrar(string t) // mostrar por CLI
    {   if(Raiz!=NULL){
            Mos(Raiz,t); // mostramos los nodos siempre que exista almenos uno;
            // cada vez que se redibuje el arbol redefinimos la tabla.
        }
        cout<<endl;
    }
    void Mos(Nodo *R,string t){ // T = distancia del nodo anterior con el nodo actual;
        // si el nodo no tiene hijos entonces estamos en el ultimo nivel;
        if (R->sig.size() == 0){
            //cout<<t<<" - "<<R->Palabra<<endl;
            t.resize(t.size() -1 );
            //Update_table(t, );  // imprimiendo la palabra del nodo actual
        }
        map <int,Nodo *>::iterator it;
        for(it=(R->sig).begin();it!=(R->sig).end();it++) // comprobamos que el nodo tiene nodos hijo;
            Mos((*it).second,t+ to_string((*it).first) + ","); // si el nodo tiene hijos entonces llamamos a la misma funcion recursivamente
    }
    void MostrarX(HDC hdc,int x,int y,int a, int level)
    {   if(Raiz!=NULL)
            MosX(hdc,Raiz,x,y,a,level);
    }
    void MosX(HDC hdc,Nodo *R,int x,int y,int a, int level)// arbol
    {   map <int,Nodo *>::iterator it;
        int ii,ancho,Tam=(R->sig).size();
        if (Tam>0)
        {   ancho=a/Tam;
            for(ii=0,it=(R->sig).begin();it!=(R->sig).end();it++,ii++)
            {   MoveToEx(hdc,x,y,NULL);
                LineTo(hdc,x+ii*ancho-a/2,y+50);
                MosX(hdc,(*it).second,x+ii*ancho-a/2,y+50,ancho-15,level + 1);
                char Cad2[10];
                itoa((*it).first,Cad2,10);
                TextOut(hdc,x+ii*ancho-a/2-10,y+50-5,Cad2,strlen(Cad2));
            }
        }
        char Cad[10];
        itoa((R->P).x,Cad,10);
        TextOut(hdc,x+1,y+1,Cad,strlen(Cad));
        itoa((R->P).y,Cad,10);
        TextOut(hdc,x+1,y+15,Cad,strlen(Cad));
        //{   if((R->sig).find(dist)==(R->sig).end())

        if (key_per_level[level] != NULL){ //verificando que exista una llave por nivel;
            if(R->P.x == key_per_level[level]->x && R->P.y == key_per_level[level]->y){
                cout << "nodes level \t" << level << "\t" << R->P.x  <<'\t'<< R->P.y << endl;
                cout << "keylevel \t" << level << "\t" << key_per_level[level]->x  <<'\t'<< key_per_level[level]->y << endl;
                char key_msg[] = "[key]";
                TextOut(hdc,x+1,y-15,key_msg,strlen(key_msg));
            }
        }
    }
    void MostrarR(HDC hdc,int xi,int yi,int xf,int yf)
    {   if (Raiz!=NULL)
            MosR(hdc,Raiz,xi,yi,xf,yf);
    }
    void MosR(HDC hdc,Nodo *R,int xi,int yi,int xf,int yf) // mostrar los puntos en el canvas.
    {   map <int,Nodo *>::iterator it;
        int ii,ancho,Tam=(R->sig).size();
        int xx=R->P.x;
        int yy=R->P.y;
        if (Tam>0)
        {   for(ii=0,it=(R->sig).begin();it!=(R->sig).end();it++,ii++)
            {   MosR(hdc,(*it).second,xi,yi,xf,yf);
            }
        }
        CircleBresenham(hdc,xx,yy,2,1,RGB(0,0,0));
    }
    void MostrarZ(HDC hdc) // lineas de puntos a otros puntos
    {   if (Raiz!=NULL)
            MosZ(hdc,Raiz);
    }
    void MosZ(HDC hdc,Nodo *R)
    {   map <int,Nodo *>::iterator it;
        int ii,ancho,Tam=(R->sig).size();
        int xx=R->P.x;
        int yy=R->P.y;
        if (Tam>0)
        {   for(ii=0,it=(R->sig).begin();it!=(R->sig).end();it++,ii++)
            {   MoveToEx(hdc,xx,yy,NULL);
                LineTo(hdc,((*it).second)->P.x,((*it).second)->P.y);
                MosZ(hdc,(*it).second);
            }
        }
        CircleBresenham(hdc,xx,yy,2,1,RGB(0,0,0));
    }
    string Get_leaf_from_path(vector<int> path){
        string leaf;
        Nodo *arrow = Raiz;
        for (int d = 0; d < path.size(); d++){
            arrow = arrow->sig[path[d]];
        }
        //return string arrow->P;
    }
    vector <Punto> similar(Nodo *R,Punto s,int Tol)
    {   int staRaiz;
        float dist;
        vector<vector<int>> fixed_array = table;
        vector<vector<int>> temp;
        vector <Punto> ret;
        cout << "\nWe are on the Similar function table size =\t" << fixed_array.size() << "\n\n";
        if(R!=NULL)
        {   dist=distP(R->P,s);
            if(dist<=Tol)
                ret.push_back(R->P);
            staRaiz = dist - Tol;
            if(staRaiz<0)
                staRaiz=1;
            vector <Punto> tmp;
            for(;staRaiz<dist+Tol;staRaiz++)
            {   if((R->sig).find(staRaiz)!=(R->sig).end())
                {   tmp=similar((R->sig)[staRaiz],s,Tol);
                    ret.insert(ret.end(),tmp.begin(),tmp.end());
                }
            }
        }
        map<int, Punto* >::iterator it;
        for (it = key_per_level.begin(); it != key_per_level.end(); it ++){
            if ((*it).second->x > 0){
                //dist = distP((*it).second, Punto s); // evaluando distancia (pivote, q);
                cout << "\nWe are on the level "  << (*it).first << ' '<< (*it).second  << " | , tol = "<<Tol<< " y con distancia a "<< "**" <<" = " <<dist<<  endl;
                printf("intervalo[ %d - %d , %d + %d ] \n",dist,Tol,dist,Tol);
                //cout <<(*it).first<<'\t'<< (*it).second.length() << endl;
                for (int i= 0; i < fixed_array.size();i++){
                    for (int j = 0; j < fixed_array[i].size(); j++){
                            cout << fixed_array[i][j] << " ";
                    }
                    if (fixed_array[i][(*it).first] >= dist - Tol && fixed_array[i][(*it).first] <= dist + Tol ){
                        // estamos dentro del rango;
                        cout << " YES";
                        temp.push_back(fixed_array[i]);
                    }else{
                        cout << " NO";
                    }
                    cout << endl;
                }
                fixed_array = temp;
                temp.clear();
            }
        }
        string leaf;
            cout << "Evaluando los candidatos: \n";
        for (int i = 0; i < fixed_array.size(); i++){
            // imprimiendo la ruta y el valor de la hoja;
            leaf = Get_leaf_from_path(fixed_array[i]);
            cout << leaf << " ";
        for (int j = 0; j < fixed_array[i].size(); j++){
                cout << fixed_array[i][j] << " ";
        }
        /*if (distP(leaf, """") <= Tol){
            cout<< " YES";
            ret.push_back(leaf);
        }
        else{
            cout << " NO";
        }
            cout << "\n";*/
        }
        return ret;
    }
};
ArbolBK A;
Punto PQ(1000,1000);
vector <Punto> VP;
vector <Punto> resultados;
Punto p_busqueda(0,0);
BOOL CALLBACK DlgMain(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{   switch(uMsg)
    {   case WM_PAINT:
        {   PAINTSTRUCT ps;
            HDC hdc;
            color=0;
            hdc=BeginPaint(hwndDlg,&ps);
            Rectangle(hdc,pxi,pyi,pxf,pyf);
          //  Dibuja lineas
            A.MostrarZ(hdc);
            A.MostrarX(hdc,800,100,550, 0);
          //  A.MostrarR(hdc,pxi,pyi,pxf,pyf);
          //  Dibuja circulos concentricos
            if (p_busqueda.x + p_busqueda.y > 0){ // siempre que el punto a buscar exista o sea diferente de 0;
                for(int cc=0;cc<20;cc++)
                    CircleBresenham(hdc,p_busqueda.x,p_busqueda.y,cc*dd,1,RGB(color,0,0));
                CircleBresenham(hdc,p_busqueda.x,p_busqueda.y,2,1,RGB(100,100,100)); // punto de busqueda
            }
            color+=40;
            for(int cc=0;cc<resultados.size();cc++){
                CircleBresenham(hdc,resultados[cc].x,resultados[cc].y,6,1,RGB(0,250,0));
            }
            for(int cc=0;cc<VP.size();cc++)
                CircleBresenham(hdc,VP[cc].x,VP[cc].y,6,2,RGB(255,0,0));
            CircleBresenham(hdc,PQ.x,PQ.y,(Toler-1)*dd,1,RGB(0,0,255));
            CircleBresenham(hdc,PQ.x,PQ.y,Toler*dd,1,RGB(255,0,0));
            //Ellipse(hdc,PQ.x-(Toler-1)*dd/2,PQ.y-(Toler-1)*dd/2,PQ.x+(Toler-1)*dd/2,PQ.y+(Toler-1)*dd/2);
            CircleBresenham(hdc,PQ.x,PQ.y,(Toler+1)*dd,1,RGB(0,0,255));
            EndPaint(hwndDlg,&ps);
        }
        return TRUE;
        case WM_LBUTTONDOWN:
        {
            long xPos = GET_X_LPARAM(lParam);
            long yPos = GET_Y_LPARAM(lParam);
            if(xPos<pxf&&xPos>pxi&&yPos<pyf&&yPos>pyi)
                A.Insertar(Punto((int)xPos,(int)yPos));

            InvalidateRect(hwndDlg,NULL,true);
        }
        return TRUE;
        case WM_INITDIALOG:
        {   pxi=10,pyi=50,pxf=pxi+450,pyf=pyi+450,dd=20,Toler=3;
            /*
            A.Insertar(Punto(347,233));
            A.Insertar(Punto(335,279));
            A.Insertar(Punto(217,216));
            A.Insertar(Punto(249,258));
            */
        }
        return TRUE;
        case WM_MOUSEMOVE:
        {   long xPos = GET_X_LPARAM(lParam);
            long yPos = GET_Y_LPARAM(lParam);
            char Cad[20];
            sprintf(Cad,"%d",xPos);
            SetDlgItemText(hwndDlg,EDITX,Cad);
            sprintf(Cad,"%d",yPos);
            SetDlgItemText(hwndDlg,EDITY,Cad);
            if(xPos<pxf&&xPos>pxi&&yPos<pyf&&yPos>pyi)
            {   VP=A.similar(A.Raiz,Punto(int(xPos),int(yPos)),Toler);
                PQ.x=xPos;
                PQ.y=yPos;
                InvalidateRect(hwndDlg,NULL,true);
            }
        }
        return TRUE;
        case WM_CLOSE:
        {   EndDialog(hwndDlg, 0);
        }
        return TRUE;
        case WM_COMMAND:
        {   switch(LOWORD(wParam))
            {
                case BUSCAR: // en caso que pulsemos el boton de [buscar];
                {
                    int coor_x=GetDlgItemInt(hwndDlg,BUSQ_X,NULL,NULL); // procesamos el texto del cuadro | buacar x
                    int coor_y=GetDlgItemInt(hwndDlg,BUSQ_Y,NULL,NULL); // procesamos el texto del cuadro | buscar y
                    int tolerancia=GetDlgItemInt(hwndDlg,BUSQ_TOL,NULL,NULL); // procesamos el texto del cuadro | tolerancia
                    cout << coor_x << coor_y << tolerancia << endl;
                    p_busqueda =  Punto(coor_x,coor_y);
                    resultados = A.similar(A.Raiz,Punto(coor_x,coor_y),tolerancia);
                    stringstream ss; // ss funcionara como una entrada;
                    for(int i=0;i<resultados.size();i++){
                         // iteramos por los resultados
                        cout<< resultados[i].x <<" , "<<resultados[i].y<<" "; // imprimimos las palabras;
                        //CircleBresenham(hdc,R[i].x,R[i].y,2,1,RGB(123,68,68));

                        ss<< " (" <<resultados[i].x << " , " << resultados[i].y <<") "<<"\t";
                    }
                    SetDlgItemTextA(hwndDlg,RESUL,(ss.str()).c_str()); // tratamos la entrada ss como un array char*

                    // para finalmente imprimirlo en la ventana;
                }
                return TRUE;
            }
        }
        return TRUE;
    }
    return FALSE;
}
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    hInst=hInstance;
    InitCommonControls();

    return DialogBox(hInst, MAKEINTRESOURCE(DLG_MAIN), NULL, (DLGPROC)DlgMain);
}
