#include <stdio.h>
#include "cos3d.h"
#include "math.h"
#include "stdlib.h"
#include "GLCD/GLCD.h"
#include "car_lp.h"
#include "sprite.h"
#include <stdio.h>

void setProjectionMatrix(float angleOfView, float near, float far, matrix44f M) {
	float scale = 1 / tan(angleOfView * 0.5 * 3.14519 / 180); 
    M[0][0] = scale; 
    M[1][1] = scale; 
    M[2][2] = -far / (far - near); 
    M[3][2] = -far * near / (far - near); 
    M[2][3] = -1; 
    M[3][3] = 0; 
}
void multPointMatrix(vector3f *in, vector3f *out, matrix44f M) 
{ 
	float w;
    out->x   = in->x * M[0][0] + in->y * M[1][0] + in->z * M[2][0] + /* in->z = 1 */ M[3][0]; 
    out->y   = in->x * M[0][1] + in->y * M[1][1] + in->z * M[2][1] + /* in->z = 1 */ M[3][1]; 
    out->z   = in->x * M[0][2] + in->y * M[1][2] + in->z * M[2][2] + /* in->z = 1 */ M[3][2]; 
	w = in->x * M[0][3] + in->y * M[1][3] + in->z * M[2][3] + /* in->z = 1 */ M[3][3]; 
 
	if (w != 1) { 
        out->x /= w; 
        out->y /= w; 
        out->z /= w; 
    } 
} 


int cmin(int a, int b) {
	if (a<b) return a; else return b;
}
int cmax(int a, int b) {
	if (a>b) return a; else return b;
}

vector2f car_data_out[car_vertexes_n];
matrix44f Mproj; 
matrix44f worldToCamera; 
matrix44f rotMatrix, translationMatrix;
	
void draw_car(vector3f rotation, vector3f translation, int color) {
	uint32_t imageWidth = 240, imageHeight = 240; 
	float angleOfView = 90; 
    float near = 0.1; 
    float far = 10; 
	int i, colore, colmin = 65000, colmax = 0;
	
	init_matrix(worldToCamera);
	init_matrix(Mproj);
	init_matrix(translationMatrix);
	
    worldToCamera[3][1] = -0.5; 
    worldToCamera[3][2] = -5; 
	
	init_matrix(rotMatrix);
	rotateY_matrix(rotMatrix, rotation.y);
	//rotateZ_matrix(rotMatrix, rotation.z);
	translate_matrix(translationMatrix, translation.x, translation.y, translation.z);
	
	setProjectionMatrix(angleOfView, near, far, Mproj); 
	
	if (color != White)
		for (i = 0; i < car_vertexes_n; i++) {
			vector3f applied_translation, applied_rotation, vertCamera, projectedVert;
			
			multPointMatrix(&(car_vertexes[i]), &applied_rotation, rotMatrix);
			multPointMatrix(&applied_rotation, &applied_translation, translationMatrix);
			multPointMatrix(&applied_translation, &vertCamera, worldToCamera);
			multPointMatrix(&vertCamera, &projectedVert, Mproj);
			if (projectedVert.x < -2 || projectedVert.x > 2 || projectedVert.y < -2 || projectedVert.y > 2) continue;
			car_data_out[i].x = cmin(imageWidth - 1, (uint32_t)((projectedVert.x + 1) * 0.5 * imageWidth));
			car_data_out[i].y = cmin(imageHeight - 1, (uint32_t)((1 - (projectedVert.y + 1) * 0.5) * imageHeight));
		}
	
	for (i=0; i < car_edges_n - 4*(fari<1); i++) {
		LCD_DrawLine(car_data_out[car_edges[i].a].x, car_data_out[car_edges[i].a].y, car_data_out[car_edges[i].b].x, car_data_out[car_edges[i].b].y, color);
	}
}

const vector2int finestrini[4] = {
					{10, 11},
					{3, 1},
					{7, 4},
					{53, 54}};
vector3f create_vec3f(float x, float y, float z) {
	vector3f v;
	v.x = x;
	v.y = y;
	v.z = z;
	return v;
}

void anima_finestrini() {
	static int f[4] = {1, 1, 1, 1};
	int i;
	float vel = 0.05f;
	for (i=0; i<4; i++)  {
			f[i] += (f[i]<target_finestrini[i])?vel:-vel;
			muovi_finestrini(i, f[i]);
		}
}
void set_finestrini(int index, float target) {
	int i;
	if (index != -1)
		target_finestrini[index] = target;
	else 
		for (i=0; i<4; i++)  
			target_finestrini[i] = target;
}
	

void muovi_finestrini(int index, float lvl) {	//ToDo: invece che muoverli verso il basso e poi verso l'alto di amount, calcolare differenza tra edge sup ed edge inf e usare amount come % della differenza da applicare.
	int i;
	float max_delta = 0.40f;
	static float livello_attuale = 1;
	float amount = -(livello_attuale-lvl)*max_delta;
	if (index == -1)
		for (i=0; i<4; i++) {
			car_vertexes[finestrini[i].a].y += amount;
			car_vertexes[finestrini[i].b].y += amount;
			
			if (i%2) {
				car_vertexes[finestrini[i].a].x += amount/2.0f;
				car_vertexes[finestrini[i].b].x += amount/2.0f;
			} else {
				car_vertexes[finestrini[i].a].x -= amount/2.0f;
				car_vertexes[finestrini[i].b].x -= amount/2.0f;
			}
		}
	else {
		car_vertexes[finestrini[index].a].y += amount;
		car_vertexes[finestrini[index].b].y += amount;
		
		if (index%2) {
				car_vertexes[finestrini[index].a].x += amount/2.0f;
				car_vertexes[finestrini[index].b].x += amount/2.0f;
			} else {
				car_vertexes[finestrini[index].a].x -= amount/2.0f;
				car_vertexes[finestrini[index].b].x -= amount/2.0f;
			};
	}
	livello_attuale = lvl;

}



void matrix_mult(matrix44f *m1, matrix44f *m2, matrix44f *result) {
	int i, j, k;
	for (i = 0; i < 4; i++) 
		for (j = 0; j < 4; j++) {
			*result[i][j] = 0;
			for (k = 0; k < 4; k++) 
				*result[i][j] += *m1[i][k] * *m2[k][j];
		}
}

void rotateX(vector3f *in, float angle) {
	in->y = in->y*cosf(angle) - in->z*sinf(angle);
	in->z = in->y*sinf(angle) + in->z*cosf(angle);
}
void rotateY(vector3f *in, float angle) {
	in->x = in->x*cosf(angle) + in->z*sinf(angle);
	in->z = -in->x*sinf(angle) + in->z*cosf(angle);
}
void rotateZ(vector3f *in, float angle) {
	in->x = in->x*cosf(angle) - in->y*sinf(angle);
	in->y = in->x*sinf(angle) + in->y*cosf(angle);
}


void init_matrix(matrix44f m) {
	int i,j;
	for (i = 0; i<4; i++)
		for (j=0; j<4; j++)
			m[i][j] = 0;
	
	m[0][0] = 1;
	m[1][1] = 1;
	m[2][2] = 1;
	m[3][3] = 1;
}
	
void rotateY_matrix(matrix44f m, float angle) {
	m[0][0] = cosf(angle);
	m[0][2] = sinf(angle);
	m[2][0] = -sinf(angle);
	m[2][2] = cosf(angle);
}
void rotateZ_matrix(matrix44f m, float angle) {	//ToDo: controllare segni
	m[0][0] = cosf(angle);
	m[0][1] = -sinf(angle);
	m[1][0] = sinf(angle);
	m[1][1] = cosf(angle);
	
}

void translate_matrix(matrix44f m, float x, float y, float z) {
	m[3][0] = x;
	m[3][1] = y;
	m[3][2] = z;
}

void DrawSprite(int offsetX, int offsetY, const char* pixel_data, int width, int height) {
	int i, j, high, low, color;
	for (i=0; i<height; i++) 
		for (j=0; j<width; j++) {
			low  = pixel_data[i*width*2+j*2];
			high = pixel_data[i*width*2+j*2+1];
			color = (high<<8) | (0x00FF & low);
			if (color != White)	// uso il bianco come "trasparenza" quando disegno le immagini importate
				LCD_SetPoint(j+offsetX, i+offsetY, color);
		}
}
void EraseSprite(int offsetX, int offsetY, const char* pixel_data, int width, int height) {
	int i, j, high, low, color;
	for (i=0; i<height; i++) 
		for (j=0; j<width; j++) {
			low  = pixel_data[i*width*2+j*2];
			high = pixel_data[i*width*2+j*2+1];
			color = (high<<8) | (0x00FF & low);
			if (color != White)	// uso il bianco come "trasparenza" quando disegno le immagini importate
				LCD_SetPoint(j+offsetX, i+offsetY, White);
		}
}

void DrawSpeed(int angle, int color, int bkColor) {
	int offset_X = 40, offset_y=240+55, width = 2, inner_length = 3, outer_length = 25;
	static int tailX=-1, tailY, arrowX, arrowY;
	int i;
	char buffer[10];
	
	if (tailX != -1) {	// se non è il primo giro, entro nella routine di erase della vecchia freccia.
		for (i=-width; i<width; i++)
			LCD_DrawLine(offset_X+i,offset_y, arrowX, arrowY, bkColor);
	}
	
	tailX = offset_X + inner_length*cosf((180-angle)*3.14/180);
	tailY = offset_y - inner_length*sinf(angle*3.14/180);
	arrowX = offset_X + outer_length*cosf((180-angle)*3.14/180);
	arrowY = offset_y - outer_length*sinf(angle*3.14/180);
	
	for (i=-width; i<width; i++)
		LCD_DrawLine(offset_X+i,offset_y, arrowX, arrowY, color);
	
	if (angle < 100)
		sprintf(buffer," %dkm/h ", angle);
	else 
		sprintf(buffer,"%dkm/h ", angle);
	GUI_Text(offset_X-30,offset_y+10,buffer,Black,White);
	
}


void justDraw(void) {
	static int frame = 0, direzione_finestrini = 1;
	static char just_once = 1;
	static float livello_finestrini = 1.0f;
	vector3f rotation, translation;
	
	if (just_once) {
		LCD_DrawLine(0,240,240,240,Black);
		LCD_DrawLine(80,240,80,320,Black);
		DrawSprite(0,240,speed_meter,80,80);
		just_once = 0;
	}
	
	rotation.y = -frame/50.0;
	translation.y = -1.5;
	translation.z = 0.5;
	draw_car(rotation, translation, White);
	

	
// 	Demo mode
	//muovi_finestrini(-1, livello_finestrini);
	/*
	if ((livello_finestrini >= 1) || (livello_finestrini <= 0))
		direzione_finestrini = -direzione_finestrini;
	livello_finestrini += direzione_finestrini/50.0f;
	
	fari = (frame/30)%4;
	freccia = (2+frame/100)%4;
	freno_parcheggio = (3+frame/30)%2; 
	velocita = frame%140;*/
// 	Fine demo mode

//	Esempio d'uso dei setter con enum associati.
//	Set_fari(ABBAGLIANTI);
//	Set_freccia(FRECCIA_SX);
//	Set_freno(INSERITO);
//	Set_acceleratore(50);
	

	frame++;
	anima_finestrini();
	draw_car(rotation, translation, Blue);
	
	drawSegnaletica();

	if (frame%10)
		DrawSpeed(velocita, Black, White);
}

void Set_fari(enum e_fari v) {
	fari = v;
}
void Set_freccia(enum e_freccia v) {
	freccia = v;
}
void Set_freno(enum e_freno v) {
	freno_parcheggio = v;
}
void Set_acceleratore(int v) {
	velocita = v;
}
int getAcceleratore(){
	return velocita;
}

void drawSegnaletica(void) {
	int posizioniX[] = {85, 85+35, 85+35*2, 85+35*3};
	int posizioneY = 290;
	int periodo_freccia = 10;
	static int stato_fari=4, stato_freccia=4, blink_freccia = 1, stato_freno_parcheggio=4;
	const char *tipi_di_fari[3] = {anabbaglianti, abbaglianti, fendinebbia};
	const char *tipi_di_freccia[3] = {freccia_sx, freccia_dx, hazard_lights};
	char buffer[10];
	if (stato_fari != fari) {
		EraseSprite(posizioniX[0],posizioneY,tipi_di_fari[stato_fari-1],30,30);
		if (fari)
			DrawSprite(posizioniX[0],posizioneY,tipi_di_fari[fari-1],30,30);
		
		stato_fari = fari;
	}
	if (freccia) {
		if (!blink_freccia || stato_freccia != freccia)
			EraseSprite(posizioniX[1],posizioneY,tipi_di_freccia[stato_freccia-1],30,30);
		if (freccia && (blink_freccia==periodo_freccia/2))
			DrawSprite(posizioniX[1],posizioneY,tipi_di_freccia[freccia-1],30,30);
		
		blink_freccia = (blink_freccia+1)%periodo_freccia;
		stato_freccia = freccia;
	}
		else{
			EraseSprite(posizioniX[1],posizioneY,tipi_di_freccia[stato_freccia-1],30,30);
		}
	
	sprintf(buffer,"fa=%d,fr=%d,pa=%.2f", fari, freccia, target_finestrini[0]);//freno_parcheggio);
	GUI_Text(posizioniX[1]-20,posizioneY-30,buffer,Black,White);
	
	if (stato_freno_parcheggio != freno_parcheggio) {
		EraseSprite(posizioniX[2],posizioneY,frenoamano,30,30);
		if (freno_parcheggio)
			DrawSprite(posizioniX[2],posizioneY,frenoamano,30,30);
		
		stato_freno_parcheggio = freno_parcheggio;
	}
}

void encodeCANdata(enum periferica p, int8_t val1, float val2, char *out) {
	out[0] = p;
	out[1] = val1;
	*((float *) out +2 ) = val2;
}
void decodeCANdata(enum periferica *p, int8_t *val1, float *val2, char *in) {
	*p = in[0];
	*val1 = in[1];
	*val2 = *((float *) in+2);
}
void decodeAndExecuteCANdata(char *in){
	enum periferica p;
	int8_t val1;
	float val2;
	decodeCANdata(&p,&val1, &val2,in);
	switch (p) {
		case FARI:
			Set_fari(val1);
			break;
		case FRECCIA:
			Set_freccia(val1);
			break;
		case ACCELERATORE:
			Set_acceleratore(val1);
			break;
		case FRENO:
			//ToDo
			break;
		case FINESTRINO:
			set_finestrini(val1,val2);
			break;
		case STERZO:
			//ToDo
			break;
	}
			
}




















