#include <Windows.h>
#include "resource.h"
#include <iostream>
#include <stdlib.h>
#include <Commctrl.h>
#include <string>
#include <Commdlg.h>
#include <fstream>
#include <time.h>
#include <cstdlib>


using namespace std;
HINSTANCE instGlobal;
WNDPROC lpEditWndProc;
HWND hWnd;
HBITMAP bmp;
HBITMAP bmp2;

//Variables inicio
int cuantashay = 0;
char nomuser[15];
char nompass[15];
int nousuariosregistrados;
int usuarionoencontrado;
int contrasenaincorrecta;
int usuariorepetido;
//Variables end

//Struct inicio
struct fecha
{
	char dia[3];
	char mes[3];
	char año[5];
};
struct user
{
	char usuario[15];
	char contra[15];
	char appaterno[15];
	char apmaterno[15];
	char nombres[15];
	user* siguiente;
	user* anterior;
}; user* iniciox, * auxx, * finx = nullptr;
struct RegistroPersonas
{
	fecha fechaCita;
	char paterno[15];
	char materno[15];
	char nombres[30];
	char curp[19];
	char rfc[14];
	char calle[15];
	char colonia[15];
	char ciudad[15];
	char estado[15];
	char estadocivil[15];
	int pretelefono;
	int telefono;
	char sexo[10];
	char grupoocupa[15];
	char perfilriesgo[7];
	char foto[250]; //id ya sea ine, pasaporte, etc....
	int carnetid;
	RegistroPersonas* siguiente;
	RegistroPersonas* anterior;
};  RegistroPersonas* inicio, * aux, * fin = nullptr;
struct RegistroVacunas
{
	char tipo[15];
	char marca[15];
	int nodosis;
	char clavevacuna[15];
	char descripcion[30];
	int precio;
	RegistroVacunas* siguiente;
	RegistroVacunas* anterior;
}; RegistroVacunas* inicioxx, * auxxx, * finxx = nullptr;
//Struct Final

//Funciones inicio
void AgregarUsuario(user* nuevo);
void AgregarVacuna(RegistroVacunas* nuevo);
void AgregarPersona(RegistroPersonas* nuevo);
void escribirUsuarios();	void leerUsuarios();
void escribirPersonas();	void leerPersonas();
void escribirVacunas();	void leerVacunas();
void BuscarUsuario(char usuario[50]);	void BuscarContrasena(char usuario[50]); 
void BuscarUsuarioRepetido(char usuario[50]);
//Funciones final

//Funciones WinAPi inicio
BOOL CALLBACK LOGIN(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK REGISTRARSE(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK MyEditCallBckProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK REGISTROPERSONAS(HWND, UINT, WPARAM, LPARAM);

//Funciones WinAPi final

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, PSTR cmdLine, int cShow) {

	leerUsuarios();
	leerPersonas();
	leerVacunas();


	HWND hVentana1 = CreateDialog(hInst, MAKEINTRESOURCE(IDD_INICIARSESION), NULL, LOGIN);

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	ShowWindow(hVentana1, cShow);

	while (GetMessage(&msg, NULL, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

BOOL CALLBACK LOGIN(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg)
	{
	case WM_CLOSE:
	{

		escribirUsuarios();
		escribirPersonas();
		escribirVacunas();

		DestroyWindow(hwnd);
	}break;
	case WM_DESTROY:
	{
		PostQuitMessage(117);
	}break;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case BTN_INIC_REGISTRARSE:
		{
			EndDialog(hwnd, 0);
			DialogBox(instGlobal, MAKEINTRESOURCE(IDD_REGISTRARSE), hwnd, REGISTRARSE);
		}break;
		case BTN_INIC_INICIARSESION:
		{
			GetDlgItemText(hwnd, INICIARSESION_USUARIO, nomuser, 15);
			GetDlgItemText(hwnd, INICIARSESION_CONTR, nompass, 15);
			BuscarUsuario(nomuser);
			BuscarContrasena(nompass);

			if (nousuariosregistrados == 0)
			{
				MessageBox(hwnd, "No hay usuarios registrados", "AVISO", MB_OK | MB_ICONERROR);
			}
			else if (usuarionoencontrado == 0)
			{
				MessageBox(hwnd, "Usuario incorrecto", "AVISO", MB_OK | MB_ICONERROR);
			}
			else if (contrasenaincorrecta == 0)
			{
				MessageBox(hwnd, "Contrasena incorrecta", "AVISO", MB_OK | MB_ICONERROR);
			}
			else
			{
				EndDialog(hwnd, 0);
				DialogBox(instGlobal, MAKEINTRESOURCE(IDD_REGISTROPERSONAS), hwnd, REGISTROPERSONAS);
			}
		}break;
		}
	}
	}
	return FALSE;
}

BOOL CALLBACK REGISTRARSE(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	user* nodous = new user;
	switch (msg) {
	case WM_INITDIALOG: {
		lpEditWndProc = (WNDPROC)SetWindowLong(GetDlgItem(hwnd, BTN_REGISTRO_USUARIO), GWL_WNDPROC, (LONG)MyEditCallBckProcedure);

	}break;
	case WM_CLOSE: {
		escribirUsuarios();
		escribirPersonas();
		escribirVacunas();

		DestroyWindow(hwnd);
	}break;

	case WM_DESTROY: {
		PostQuitMessage(117);
	}break;

	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case BTN_REGISTRARSE_REGISTRO: {
			GetDlgItemText(hwnd, BTN_REGISTRO_USUARIO, nodous->usuario, 15);
			GetDlgItemText(hwnd, BTN_REGISTRO_CONTRA, nodous->contra, 15);
			GetDlgItemText(hwnd, BTN_REGISTRO_NOMBRES, nodous->nombres, 15);
			GetDlgItemText(hwnd, BTN_REGISTRO_APMATERNO, nodous->apmaterno, 15);
			GetDlgItemText(hwnd, BTN_REGISTRO_APPATERNO, nodous->appaterno, 15);
			BuscarUsuarioRepetido(nodous->usuario);
			if (usuariorepetido == 0) {
				AgregarUsuario(nodous);
				MessageBox(hwnd, "Usuario registrado.", "AVISO", MB_OK | MB_ICONINFORMATION);
				EndDialog(hwnd, 0);
				DialogBox(instGlobal, MAKEINTRESOURCE(IDD_INICIARSESION), hwnd, LOGIN);
			}
			else {
				MessageBox(hwnd, "Usuario repetido.", "AVISO", MB_OK | MB_ICONERROR);
			}
		}break;
		}
	}

	}
	return FALSE;
}

LRESULT CALLBACK MyEditCallBckProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (wParam == 32)
	{
		MessageBox(hWnd, "No se aceptan espacios.", "AVISO", MB_OK | MB_ICONSTOP);

		return 0;
	}


	return CallWindowProc(lpEditWndProc, hWnd, uMsg, wParam, lParam);
}

BOOL CALLBACK REGISTROPERSONAS(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_INITDIALOG: {
		lpEditWndProc = (WNDPROC)SetWindowLong(GetDlgItem(hwnd, IDC_REGISTROPERSONAS_CURP), GWL_WNDPROC, (LONG)MyEditCallBckProcedure);
		lpEditWndProc = (WNDPROC)SetWindowLong(GetDlgItem(hwnd, IDC_REGISTROPERSONAS_RFC), GWL_WNDPROC, (LONG)MyEditCallBckProcedure);

	}break;
	case WM_CLOSE: {
		escribirUsuarios();
		escribirPersonas();
		escribirVacunas();

		DestroyWindow(hwnd);
	}break;

	case WM_DESTROY: {
		PostQuitMessage(117);
	}break;
	case WM_COMMAND: {
		menu(wParam, hwnd);
		switch (LOWORD(wParam)) {
		case IDC_REGISTROPERSONAS_REGISTRARBTN: {
			RegistroPersonas* nodouse = new RegistroPersonas;
			SYSTEMTIME fechaCita;
			ZeroMemory(&fechaCita, sizeof(fechaCita));
			SendDlgItemMessage(hwnd, IDC_REGISTROPERSONAS_DATEPICK, DTM_GETSYSTEMTIME, 0, (LPARAM)&fechaCita);
			sprintf_s(nodouse->fechaCita.dia, "%d", fechaCita.wDay);
			sprintf_s(nodouse->fechaCita.mes, "%d", fechaCita.wMonth);
			sprintf_s(nodouse->fechaCita.anio, "%d", fechaCita.wYear);

			GetDlgItemText(hwnd, IDC_REGISTROPERSONAS_PATERNO, nodouse->paterno, 15);
			GetDlgItemText(hwnd, IDC_REGISTROPERSONAS_MATERNO, nodouse->materno, 15);
			GetDlgItemText(hwnd, IDC_REGISTROPERSONAS_NOMBRES, nodouse->nombres, 30);
			GetDlgItemText(hwnd, IDC_REGISTROPERSONAS_CURP, nodouse->curp, 19);
			GetDlgItemText(hwnd, IDC_REGISTROPERSONAS_RFC, nodouse->rfc, 14);
			GetDlgItemText(hwnd, IDC_REGISTROPERSONAS_CALLE, nodouse->calle, 15);
			GetDlgItemText(hwnd, IDC_REGISTROPERSONAS_COLONIA, nodouse->colonia, 15);
			GetDlgItemText(hwnd, IDC_REGISTROPERSONAS_CIUDAD, nodouse->ciudad, 15);
			GetDlgItemText(hwnd, IDC_REGISTROPERSONAS_ESTADO, nodouse->estado, 15);
			GetDlgItemText(hwnd, IDC_REGISTROPERSONAS_ESTADOCIVIL, nodouse->estadocivil, 15);
			strcpy_s(nodouse->foto, copiarfoto);
			GetDlgItemText(hwnd, IDC_REGISTROPERSONAS_SEXO, nodouse->sexo, 10);
			GetDlgItemText(hwnd, IDC_REGISTROPERSONAS_GRUPOOCUPA, nodouse->grupoocupa, 15);
			GetDlgItemText(hwnd, IDC_REGISTROPERSONAS_PERFILRIESGO, nodouse->perfilriesgo, 7);
			nodouse->telefono = GetDlgItemInt(hwnd, IDC_REGISTROPERSONAS_TELEFONO, NULL, FALSE);
			nodouse->pretelefono = GetDlgItemInt(hwnd, IDC_REGISTROPERSONAS_PRETELEFONO, NULL, FALSE);
			azar = rand() % 1000;
			aux = inicio;
			while (aux != nullptr) {
				if (aux->carnetid == azar) {
					azar = rand() % 1000;
				}
				aux = aux->siguiente;
			}
			nodouse->carnetid = azar;
			BuscarCurpRepetido(nodouse->curp);
			BuscarRfcRepedito(nodouse->rfc);
			if (curprepetido == 0 && rfcrepetido == 0) {
				AgregarPersona(nodouse);
				MessageBox(hwnd, "Persona registrada.", "AVISO", MB_OK | MB_ICONINFORMATION);
			}
			else {
				MessageBox(hwnd, "Curp o Rfc repetido.", "AVISO", MB_OK | MB_ICONERROR);
			}
			cuantashay++;
		}break;
		case IDC_REGISTROPERSONAS_CARGARFOTOBTN: {
			OPENFILENAME ofn;

			char zFile[250]; //260 caracteres

			ZeroMemory(&ofn, sizeof(ofn));

			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hwnd;
			ofn.lpstrFilter = "ALL\0 * .*\0Bitmaps\0 * .bmp\0";
			ofn.lpstrFile = zFile;
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(zFile);
			ofn.lpstrFileTitle = copiarfoto;
			ofn.nMaxFileTitle = sizeof(copiarfoto);
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			ofn.nFilterIndex = 2;
			ofn.lpstrFileTitle = copiarfoto;

			if (GetOpenFileName(&ofn) == TRUE) {
				bmp = (HBITMAP)LoadImage(NULL, zFile, IMAGE_BITMAP, 150, 100, LR_LOADFROMFILE | LR_DEFAULTSIZE);
				SendDlgItemMessage(hwnd, IDC_REGISTROPERSONAS_FOTO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmp);
			}
			else {
				MessageBox(hwnd, "No seleccionó una imagen", "AVISO", MB_OK | MB_ICONERROR);
			}
		}break;
		}
	}
	}
	return FALSE;
}


void AgregarPersona(RegistroPersonas* nuevo)
{
	if (inicio == nullptr)
	{
		inicio = new RegistroPersonas;
		aux = inicio;
		aux->anterior = nullptr;
		aux->siguiente = nullptr;
		strcpy_s(aux->paterno, nuevo->paterno);
		strcpy_s(aux->materno, nuevo->materno);
		strcpy_s(aux->nombres, nuevo->nombres);
		strcpy_s(aux->curp, nuevo->curp);
		strcpy_s(aux->rfc, nuevo->rfc);
		strcpy_s(aux->calle, nuevo->calle);
		strcpy_s(aux->colonia, nuevo->colonia);
		strcpy_s(aux->ciudad, nuevo->ciudad);
		strcpy_s(aux->estado, nuevo->estado);
		strcpy_s(aux->estadocivil, nuevo->estadocivil);
		strcpy_s(aux->foto, nuevo->foto);
		strcpy_s(aux->sexo, nuevo->sexo);
		strcpy_s(aux->grupoocupa, nuevo->grupoocupa);
		strcpy_s(aux->perfilriesgo, nuevo->perfilriesgo);

		strcpy_s(aux->fechaCita.dia, nuevo->fechaCita.dia);
		strcpy_s(aux->fechaCita.mes, nuevo->fechaCita.mes);
		strcpy_s(aux->fechaCita.año, nuevo->fechaCita.año);

		aux->pretelefono = nuevo->pretelefono;
		aux->telefono = nuevo->telefono;
		aux->carnetid = nuevo->carnetid;
	}
	else
	{
		aux = inicio;
		while (aux->siguiente != nullptr)
		{
			aux = aux->siguiente;
		}
		aux->siguiente = new RegistroPersonas;
		aux->siguiente->siguiente = nullptr;
		aux->siguiente->anterior = aux;
		aux = aux->siguiente;
		fin = aux;
		strcpy_s(aux->paterno, nuevo->paterno);
		strcpy_s(aux->materno, nuevo->materno);
		strcpy_s(aux->nombres, nuevo->nombres);
		strcpy_s(aux->curp, nuevo->curp);
		strcpy_s(aux->rfc, nuevo->rfc);
		strcpy_s(aux->calle, nuevo->calle);
		strcpy_s(aux->colonia, nuevo->colonia);
		strcpy_s(aux->ciudad, nuevo->ciudad);
		strcpy_s(aux->estado, nuevo->estado);
		strcpy_s(aux->estadocivil, nuevo->estadocivil);
		strcpy_s(aux->foto, nuevo->foto);
		strcpy_s(aux->sexo, nuevo->sexo);
		strcpy_s(aux->grupoocupa, nuevo->grupoocupa);
		strcpy_s(aux->perfilriesgo, nuevo->perfilriesgo);

		strcpy_s(aux->fechaCita.dia, nuevo->fechaCita.dia);
		strcpy_s(aux->fechaCita.mes, nuevo->fechaCita.mes);
		strcpy_s(aux->fechaCita.año, nuevo->fechaCita.año);

		aux->pretelefono = nuevo->pretelefono;
		aux->telefono = nuevo->telefono;
		aux->carnetid = nuevo->carnetid;
	}
}

void AgregarUsuario(user* nuevo)
{
	if (iniciox == nullptr)
	{
		iniciox = new user;
		auxx = iniciox;
		auxx->anterior = nullptr;
		auxx->siguiente = nullptr;
		strcpy_s(auxx->usuario, nuevo->usuario);
		strcpy_s(auxx->contra, nuevo->contra);
		strcpy_s(auxx->nombres, nuevo->nombres);
		strcpy_s(auxx->apmaterno, nuevo->apmaterno);
		strcpy_s(auxx->appaterno, nuevo->appaterno);
	}
	else
	{
		auxx = iniciox;
		while (auxx->siguiente != nullptr)
		{
			auxx = auxx->siguiente;
		}
		auxx->siguiente = new user;
		auxx->siguiente->siguiente = nullptr;
		auxx->siguiente->anterior = auxx;
		auxx = auxx->siguiente;
		finx = auxx;
		strcpy_s(auxx->usuario, nuevo->usuario);
		strcpy_s(auxx->contra, nuevo->contra);
		strcpy_s(auxx->nombres, nuevo->nombres);
		strcpy_s(auxx->apmaterno, nuevo->apmaterno);
		strcpy_s(auxx->appaterno, nuevo->appaterno);

	}
}

void AgregarVacuna(RegistroVacunas* nuevo)
{
	if (inicioxx == nullptr)
	{
		inicioxx = new RegistroVacunas;
		auxxx = inicioxx;
		auxxx->anterior = nullptr;
		auxxx->siguiente = nullptr;
		strcpy_s(auxxx->tipo, nuevo->tipo);
		strcpy_s(auxxx->marca, nuevo->marca);
		strcpy_s(auxxx->clavevacuna, nuevo->clavevacuna);
		strcpy_s(auxxx->descripcion, nuevo->descripcion);
		auxxx->nodosis = nuevo->nodosis;
		auxxx->precio = nuevo->precio;

	}
	else
	{
		auxxx = inicioxx;
		while (auxxx->siguiente != nullptr)
		{
			auxxx = auxxx->siguiente;
		}
		auxxx->siguiente = new RegistroVacunas;
		auxxx->siguiente->siguiente = nullptr;
		auxxx->siguiente->anterior = auxxx;
		auxxx = auxxx->siguiente;
		finxx = auxxx;
		strcpy_s(auxxx->tipo, nuevo->tipo);
		strcpy_s(auxxx->marca, nuevo->marca);
		strcpy_s(auxxx->clavevacuna, nuevo->clavevacuna);
		strcpy_s(auxxx->descripcion, nuevo->descripcion);
		auxxx->nodosis = nuevo->nodosis;
		auxxx->precio = nuevo->precio;
	}
}

void escribirUsuarios()
{
	auxx = iniciox;
	ofstream escribirUsuario;
	escribirUsuario.open("Usuarios.bin", ios::out | ios::binary | ios::trunc);
	if (escribirUsuario.is_open())
	{
		while (auxx != nullptr)
		{
			escribirUsuario.write((char*)auxx, sizeof(user));
			auxx = auxx->siguiente;
		}
		escribirUsuario.close();
		MessageBox(hWnd, "Usuarios guardado", "AVISO", MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		MessageBox(hWnd, "No se pudo abrir el archivo", "AVISO", MB_OK | MB_ICONERROR);
	}
}

void leerUsuarios()
{
	ifstream leerUsuarios;
	leerUsuarios.open("Usuarios.bin", ios::in | ios::binary);

	auxx = iniciox;
	if (leerUsuarios.is_open())
	{
		user* userleido = new user;

		while (!leerUsuarios.read((char*)userleido, sizeof(user)).eof())
		{

			while (auxx != nullptr && auxx->siguiente != nullptr)
			{
				auxx = auxx->siguiente;
			}
			if (auxx == nullptr)
			{
				iniciox = userleido;
				iniciox->siguiente = nullptr;
				iniciox->anterior = nullptr;
				auxx = iniciox;
			}
			else
			{
				auxx->siguiente = userleido;
				auxx->siguiente->anterior = auxx;
				auxx = auxx->siguiente;
				auxx->siguiente = nullptr;
			}

			userleido = new user;

		}
		leerUsuarios.close();
	}
}

void escribirPersonas()
{
	aux = inicio;
	ofstream escribirPersona;
	escribirPersona.open("Personas.bin", ios::out | ios::binary | ios::trunc);
	if (escribirPersona.is_open())
	{
		while (aux != nullptr)
		{
			escribirPersona.write((char*)aux, sizeof(RegistroPersonas));
			aux = aux->siguiente;
		}
		escribirPersona.close();
		MessageBox(hWnd, "Personas guardado", "AVISO", MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		MessageBox(hWnd, "No se pudo abrir el archivo", "AVISO", MB_OK | MB_ICONERROR);
	}
}

void leerPersonas()
{
	ifstream leerPersonas;
	leerPersonas.open("Personas.bin", ios::in | ios::binary);

	aux = inicio;
	if (leerPersonas.is_open())
	{
		RegistroPersonas* personaleido = new RegistroPersonas;

		while (!leerPersonas.read((char*)personaleido, sizeof(RegistroPersonas)).eof())
		{

			while (aux != nullptr && aux->siguiente != nullptr)
			{
				aux = aux->siguiente;
			}
			if (aux == nullptr)
			{
				inicio = personaleido;
				inicio->siguiente = nullptr;
				inicio->anterior = nullptr;
				aux = inicio;
			}
			else
			{
				aux->siguiente = personaleido;
				aux->siguiente->anterior = aux;
				aux = aux->siguiente;
				aux->siguiente = nullptr;
			}

			personaleido = new RegistroPersonas;
			cuantashay++;
		}
		leerPersonas.close();
	}
}

void escribirVacunas() {
	auxxx = inicioxx;
	ofstream escribirVacuna;
	escribirVacuna.open("Vacunas.bin", ios::out | ios::binary | ios::trunc);
	if (escribirVacuna.is_open()) {
		while (auxxx != nullptr) {
			escribirVacuna.write((char*)auxxx, sizeof(RegistroVacunas));
			auxxx = auxxx->siguiente;
		}
		escribirVacuna.close();
		MessageBox(hWnd, "Vacunas guardado", "AVISO", MB_OK | MB_ICONINFORMATION);
	}
	else {
		MessageBox(hWnd, "No se pudo abrir el archivo", "AVISO", MB_OK | MB_ICONERROR);
	}
}

void leerVacunas()
{
	ifstream leerVacunas;
	leerVacunas.open("Vacunas.bin", ios::in | ios::binary);

	auxxx = inicioxx;
	if (leerVacunas.is_open()) {
		RegistroVacunas* vacunaleido = new RegistroVacunas;

		while (!leerVacunas.read((char*)vacunaleido, sizeof(RegistroVacunas)).eof()) {

			while (auxxx != nullptr && auxxx->siguiente != nullptr) {
				auxxx = auxxx->siguiente;
			}
			if (auxxx == nullptr) {
				inicioxx = vacunaleido;
				inicioxx->siguiente = nullptr;
				inicioxx->anterior = nullptr;
				auxxx = inicioxx;
			}
			else {
				auxxx->siguiente = vacunaleido;
				auxxx->siguiente->anterior = auxxx;
				auxxx = auxxx->siguiente;
				auxxx->siguiente = nullptr;
			}

			vacunaleido = new RegistroVacunas;

		}
		leerVacunas.close();
	}
}

void BuscarUsuario(char usuario[50]) {
	nousuariosregistrados = 1;
	usuarionoencontrado = 1;
	auxx = iniciox;
	if (auxx == nullptr) {
		nousuariosregistrados = 0;
	}
	else {
		while (auxx != nullptr && strcmp(auxx->usuario, usuario) != 0) {
			auxx = auxx->siguiente;
		}
		if (auxx == nullptr) {
			usuarionoencontrado = 0;
		}
	}
}

void BuscarContrasena(char usuario[50]) {
	contrasenaincorrecta = 1;
	auxx = iniciox;
	if (auxx == nullptr) {
		nousuariosregistrados = 0;
	}
	else {
		while (auxx != nullptr && strcmp(auxx->contra, usuario) != 0) {
			auxx = auxx->siguiente;
		}
		if (auxx == nullptr) {
			contrasenaincorrecta = 0;
		}
	}
}

void BuscarUsuarioRepetido(char usuario[50]) {
	usuariorepetido = 0;
	auxx = iniciox;
	if (iniciox == nullptr) {
		usuariorepetido = 0;
	}
	while (auxx != nullptr && strcmp(auxx->usuario, usuario) != 0) {
		auxx = auxx->siguiente;
	}
	if (auxx == nullptr) {
		usuariorepetido = 0;
	}
	else {
		usuariorepetido = 1;
	}
}




