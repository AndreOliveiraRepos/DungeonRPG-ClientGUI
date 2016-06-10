// =============================================================================
// PaintWithInvalidate.C											     
// ============================================================================= 
// Cria uma janela de nome "Janela Principal", pinta fundo
// de branco
// Recebe caracteres do teclado e mostra-os na janela 
// Trata WM_PAINT para refrescar a janela ap�s minimiza��o, etc.
// Trata WM_MOVE para detectar que a janela se moveu
// Em WM_CHAR guarda os caracters recebidos num buffer sem os mostrar com 
// "TextOut()". 

// WM_MOVE e WM_CHAR chamam InvalidateRect() para "dizer" ao Windows que 
// o estado actual de uma zona da janela (ou toda a janela) n�o est� actualizado. 
// Como resposta, o Windows gera WM_PAINT onde � realizado o TextOut() 
// para update da janela

// TESTES: Digitar caracteres e verificar o aumento do contador de entradas
//		   em WM_PAINT
//		   Redimensionar ou arrastar a janela: Mant�m o conte�do e o contador
//		   de entradas sobe

// ============================================================================
// In�cio do programa
// ============================================================================
#include <windows.h>
#include <tchar.h>		// Para chamada � fun��o "sprintf" 

#define TAM 255
// Pr�-declara��o da fun��o de processamento da janela, neste caso "WndProc()"
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Nome da janela (ou do programa, se ele tem uma s� janela)
TCHAR szProgName[] = TEXT("MostrarTexto");

// ============================================================================
// WinMain()
// ============================================================================
// Par�metros:
//   hInst:		handle da inst�ncia deste programa 
//   hPrevInst: Sempre NULL para o NT (era usado no Windows 3.1)
//   lpCmdLine: Ponteiro para uma string ASCIIZ de par�metros para o programa
//   nCmdShow:  Modo de exibi��o da janela 

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd;			// handler da janela (a gerar por CreateWindow())
	MSG lpMsg;			// Estrutura das mensagens
	WNDCLASSEX wcApp;	// Estrutura que define a classe da janela

						// ============================================================================
						// Caracter�sticas da janela "wcApp" 
						// ============================================================================

	wcApp.cbSize = sizeof(WNDCLASSEX);	// Tamanho da estrutura WNDCLASSEX
	wcApp.hInstance = hInst;			// Inst�ncia da janela actualmente exibida 
										// ("hInst" � par�metro de WinMain)
	wcApp.lpszClassName = szProgName;	// Nome da janela 
	wcApp.lpfnWndProc = WndProc;		// Endere�o da fun��o de processamento da janela 
	wcApp.style = CS_HREDRAW | CS_VREDRAW;			// Fazer o redraw hor. e vert. se mudar
	wcApp.hIcon = LoadIcon(NULL, IDI_APPLICATION);	// �con normal=Aplica��o do Windows
	wcApp.hIconSm = LoadIcon(NULL, IDI_WINLOGO);	// �con pequeno=�con WinLogo
	wcApp.hCursor = LoadCursor(NULL, IDC_ARROW);	// rato = "seta"
	wcApp.lpszMenuName = NULL;						// Janela sem menus
	wcApp.cbClsExtra = 0;							// Livre, para uso particular
	wcApp.cbWndExtra = 0;							// Livre, para uso particular
	wcApp.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// Fundo branco

																// ============================================================================
																// Registar a classe "wcApp" 
																// ============================================================================
	if (!RegisterClassEx(&wcApp))
		return(0);

	// ============================================================================
	// Criar a janela
	// ============================================================================
	hWnd = CreateWindow(
		szProgName,				// Nome da janela e/ou programa
		TEXT("Mostrar Texto"),		// T�tulo da janela
		WS_OVERLAPPEDWINDOW,	// Estilo da janela 
		CW_USEDEFAULT,			// Posi��o x 
		CW_USEDEFAULT,			// Posi��o y 
		CW_USEDEFAULT,			// Largura 
		CW_USEDEFAULT,			// Altura 
		(HWND)HWND_DESKTOP,	// handle da janela pai (HWND_DESKTOP para 1�)
		(HMENU)NULL,			// handle do menu (se tiver menu)
		(HINSTANCE)hInst,			// handle da inst�ncia actual (vem de WinMain())
		(LPSTR)NULL);			// N�o h� par�metros adicionais 

								// ============================================================================
								// Mostrar a janela
								// ============================================================================
	ShowWindow(hWnd, nCmdShow);	// "hWnd"= handler da janela
								// "nCmdShow"= modo, par�metro de WinMain()
	UpdateWindow(hWnd);			// Refrescar a janela 

								// ============================================================================
								// Loop de Mensagens
								// ============================================================================
								// Par�metros de "getMessage":
								//  1)	"&lpMsg"=Endere�o de uma estrutura do tipo MSG ("MSG lpMsg" ja foi 
								//		declarada no in�cio de WinMain()):

	while (GetMessage(&lpMsg, NULL, 0, 0)) {
		TranslateMessage(&lpMsg);			// Pr�-processamento da mensagem
		DispatchMessage(&lpMsg);			// Enviar a mensagem traduzida de volta
											// ao Windows
	}

	// ============================================================================
	// Fim do programa
	// ============================================================================
	return((int)lpMsg.wParam);	// Status = Par�metro "wParam" da estrutura "lpMsg"
}

// ============================================================================
// FUN��O DE PROCESSAMENTO DA JANELA
// Neste exemplo vamos processar as mensagens:
//		WM_CHAR				Guardar o caracter digitado nu buffer (sem mostrar)
//							Chamar InvalidateRect() para originar WM_PAINT
//		WM_PAINT			Mostrar o conte�do do buffer com TextOut()
// ============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;						// Declara��o de um handler para um Device Context
	PAINTSTRUCT PtStc;				// Estrutura com dados para o refrescamento da janela
	RECT lpRect;
	static TCHAR str[TAM] = TEXT("\0");		// Buffer para string completa
											// Notar o uso de "static" para manter o valor
											// entre entradas sucessivas em WndProc()
	TCHAR temp[2] = TEXT("\0");				// Buffer tempor�rio para um s� caracter

	static int contador = 1;			// Contador de entradas em WM_PAINT
	TCHAR buffer[TAM] = TEXT("\0");			// Outro buffer para n� entradas em WM_PAINT

											// Processamento das mensagens
	switch (messg) {
		//==============================================================================
		// Recolher caracteres digitados num buffer
		//==============================================================================
		// Mensagem WM_CHAR: Caracter (normal) foi digitado no teclado
	case WM_CHAR:
		_stprintf_s(temp, sizeof(temp) / sizeof(TCHAR), TEXT("%c"), wParam);		// Guardar caracter no buf. temp.
		_stprintf_s(str, sizeof(str) / sizeof(TCHAR), TEXT("%s%s"), str, temp);		// Concatenar a string em "temp"
																					// com a string em "str"

																					// Chamar InvalidateRect(window_handler, pont_struc_RECT, erase)
																					// Informa o Windows que o rect�ngulo definido pelas coordenadas contidas na
																					// estrutura RECT (ver abaixo) apontada por "pont_rect" n�o est� actualizado.
																					// Isto origina que o Windows gere WM_PAINT. Com pont_struct=NULL toda janela
																					// � considerada n�o actualizada
																					// O �ltimo par�metro "erase" � booleano: Se T, o background � repintado
		InvalidateRect(hWnd, NULL, 1);

		break;

		//==============================================================================
		// Detectar movimento da janela
		//==============================================================================
	case WM_MOVE:						// Detectar que a janela se moveu
		InvalidateRect(hWnd, NULL, 1);	// Gerar WM_PAINT para actualizar 
										// as coordenadas vis�veis na janela
		break;

		//==============================================================================
		// Mostrar texto recolhido actual e refrescar a janela depois de minimizada, 
		// maximizada ou dimens�es alteradas
		//==============================================================================
	case WM_PAINT:
		// GetDeviceContext: BeginPaint(handler_janela, Ponteiro_para_Paint_Structure)
		/* ...  |
		>	typedef struct tagRECT {
		LONG left, top			Coordenadas do ponto superior esquerdo
		LONG right, bottom		Coordenadas do ponto inferior direito
		} RECT;
		*/
		hdc = BeginPaint(hWnd, &PtStc);
		// Mostrar o n�mero de entrada em WM_PAINT (e incrementar contador)
		// Para testar: Escrever caracteres, redimensionar a janela ou min/mazimizar
		_stprintf_s(buffer, sizeof(buffer) / sizeof(TCHAR), TEXT("Entradas em WM_PAINT: %d"), contador);
		TextOut(hdc, 0, 50, buffer, _tcslen(buffer));
		contador++;
		// Mostrar o buffer que cont�m a string completa, "str"
		TextOut(hdc, 0, 0, str, _tcslen(str));
		GetWindowRect(hWnd, &lpRect);	// Coordenadas (em pixels) dos limites
										// da janela. O resultado fica numa estrutura
										// do tipo RECT (ver coment�rio anterior)
		_stprintf_s(buffer, sizeof(buffer) / sizeof(TCHAR), TEXT("Coordenadas da janela: %d %d %d %d"),
			lpRect.left, lpRect.top, lpRect.right, lpRect.bottom);
		TextOut(hdc, 0, 100, buffer, _tcslen(buffer));
		// Libertar o device contexto relativo a uma opera��o de refrescamento
		EndPaint(hWnd, &PtStc);
		break;
		//==============================================================================
		// Terminar e Processamentos default
		//==============================================================================
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
		// Restantes mensagens t�m processamento default
	default:
		return(DefWindowProc(hWnd, messg, wParam, lParam));
		break;
	}
	return(0);
}


