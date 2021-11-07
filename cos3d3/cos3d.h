#ifndef _COSD_H
#define _COSD_H

typedef struct {
	float x,y,z;
} vector3f;
typedef struct {
	float x,y;
} vector2f;

typedef struct {
	int a, b;
} vector2int;

typedef float matrix44f[4][4];

enum e_freccia { NESSUNA=0, FRECCIA_SX=1, FRECCIA_DX=2, HAZARD=3 };
enum e_fari {SPENTI=0, ANABBAGLIANTI=1, ABBAGLIANTI=2, FENDINEBBIA=3 };
enum e_freno {NON_INSERITO=0, INSERITO=1 };
enum periferica {FARI=0, FRECCIA, ACCELERATORE, FRENO, FINESTRINO, STERZO};

static char freccia = 0; // 0 = spenta; 1 = sx; 2 = dx; 3=hazard;
static int velocita = 0;
static char fari = 0; // 0 = spenti; 1 = anabbaglianti; 2 = abbaglianti; 3 = fendinebbia;
static char freno_parcheggio = 0; // 0 = non inserito; 1 = inserito
static float target_finestrini[4] = {1.,1.,1.,1.};

void init_matrix(matrix44f m);
void rotateY_matrix(matrix44f m, float angle);
void rotateZ_matrix(matrix44f m, float angle);
void translate_matrix(matrix44f m, float x, float y, float z);
void draw_car(vector3f rotation, vector3f translation, int color);

void accendi_fari(int v);
vector3f create_vec3f(float x, float y, float z);
void DrawSprite(int offsetX, int offsetY, const char* pixel_data, int width, int height);
void DrawSpeed(int angle, int color, int bkColor);
void drawSegnaletica(void);
void justDraw(void);

void Set_fari(enum e_fari v);
void Set_freccia(enum e_freccia v);
void Set_freno(enum e_freno v);
void Set_acceleratore(int v);
int getAcceleratore();
void muovi_finestrini(int index, float lvl);
void set_finestrini(int index, float target);


#endif

