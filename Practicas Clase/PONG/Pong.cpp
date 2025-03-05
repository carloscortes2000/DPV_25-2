#include <GL/glut.h> //the glut file for windows operations
                     // it also includes gl.h and glu.h for the openGL library calls
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PI 3.1415926535898 

// Area de juego 
const int GAME_WIDTH = 320;
const int GAME_HEIGHT = 240;

double xpos, ypos;           // posición actual de la bola
double ballSpeedX, ballSpeedY;  // velocidades de la pelota en X y Y
GLfloat RadiusOfBall = 7.0;  // tamano de la pelota

GLfloat T1[16] = {1.,0.,0.,0.,\
                  0.,1.,0.,0.,\
                  0.,0.,1.,0.,\
                  0.,0.,0.,1.};
GLfloat S[16] = {1.,0.,0.,0.,\
                 0.,1.,0.,0.,\
                 0.,0.,1.,0.,\
                 0.,0.,0.,1.};
GLfloat T[16] = {1.,0.,0.,0.,\
                 0., 1., 0., 0.,\
                 0.,0.,1.,0.,\
                 0.,0.,0.,1.};

// Paletas
const double paddleWidth = 2.0;
const double paddleHeight = 20.0;
double leftPaddleX, leftPaddleY;
double rightPaddleX, rightPaddleY;

// Marcador de puntaje
int scoreLeft = 0, scoreRight = 0;

// Control de inicio de juego
bool gameStarted = false;

GLint circle_points = 100;
void MyCircle2f(GLfloat centerx, GLfloat centery, GLfloat radius) {
    GLint i;
    GLdouble angle;
    glBegin(GL_POLYGON);
    for (i = 0; i < circle_points; i++) {
        angle = 2 * PI * i / circle_points;
        glVertex2f(centerx + radius * cos(angle), centery + radius * sin(angle));
    }
    glEnd();
}

// Dibuja la bola, centrada en el origen
void draw_ball() {
    glColor3f(1.0, 1.0, 1.0);
    MyCircle2f(0., 0., RadiusOfBall);
}

// Dibuja una paleta en (x, y)
void drawPaddle(double x, double y) {
    glColor3f(1.0, 1.0, 1.0); // color blanco
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + paddleWidth, y);
        glVertex2f(x + paddleWidth, y + paddleHeight);
        glVertex2f(x, y + paddleHeight);
    glEnd();
}

// Dibuja un texto centrado 
void drawCenteredText(const char* text, float y) {
    int len = (int) strlen(text);
    int width = 0;
    for (int i = 0; i < len; i++) {
        width += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
    float x = (GAME_WIDTH - width) / 2.0f;
    glColor3f(1.0, 1.0, 1.0); // texto en blanco
    glRasterPos2f(x, y);
    for (int i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}

void Display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    // Si el juego no ha comenzado, mostrar mensaje de inicio
    if (!gameStarted) {
        glLoadIdentity();
        drawCenteredText("Presiona espacio para empezar", GAME_HEIGHT / 2.0f);
        glutSwapBuffers();
        glutPostRedisplay();
        return;
    }

    // Actualizamos la posición de la bola según su velocidad
    xpos += ballSpeedX;
    ypos += ballSpeedY;

    // Rebote en techo y piso
    if (ypos + RadiusOfBall >= GAME_HEIGHT || ypos - RadiusOfBall <= 0)
        ballSpeedY = -ballSpeedY;

    // Colisión con la paleta izquierda
    if (xpos - RadiusOfBall <= leftPaddleX + paddleWidth &&
        ypos >= leftPaddleY && ypos <= leftPaddleY + paddleHeight &&
        ballSpeedX < 0) {
        ballSpeedX = fabs(ballSpeedX); // rebota hacia la derecha
    }

    // Colisión con la paleta derecha
    if (xpos + RadiusOfBall >= rightPaddleX &&
        ypos >= rightPaddleY && ypos <= rightPaddleY + paddleHeight &&
        ballSpeedX > 0) {
        ballSpeedX = -fabs(ballSpeedX); // rebota hacia la izquierda
    }

    if (xpos - RadiusOfBall < 0) {
        scoreRight++;
        printf("Puntuacion: Izquierda %d - Derecha %d\n", scoreLeft, scoreRight);
        // Reiniciar la bola en el centro, moviéndose hacia la derecha
        xpos = GAME_WIDTH / 2.0;
        ypos = GAME_HEIGHT / 2.0;
        ballSpeedX = fabs(ballSpeedX);
    }
    
    if (xpos + RadiusOfBall > GAME_WIDTH) {
        scoreLeft++;
        printf("Puntuacion: Izquierda %d - Derecha %d\n", scoreLeft, scoreRight);
        // Reiniciar la bola en el centro, moviéndose hacia la izquierda
        xpos = GAME_WIDTH / 2.0;
        ypos = GAME_HEIGHT / 2.0;
        ballSpeedX = -fabs(ballSpeedX);
    }

    // Usar las matrices de transformación para posicionar la bola
    T[12] = xpos;
    T[13] = ypos;
    glLoadMatrixf(T);

    // Se mantiene el escalado en 1
    S[0] = 1.0;
    S[5] = 1.0;
    T1[13] = -RadiusOfBall;
    glMultMatrixf(T1);
    glMultMatrixf(S);
    T1[13] = RadiusOfBall;
    glMultMatrixf(T1);

    // Dibujar la bola
    draw_ball();

    // Reiniciar la matriz para dibujar las paletas sin transformaciones previas
    glLoadIdentity();
    drawPaddle(leftPaddleX, leftPaddleY);
    drawPaddle(rightPaddleX, rightPaddleY);

    glutSwapBuffers();
    glutPostRedisplay();
}


void reshape (int w, int h)
{
   // on reshape and on startup, keep the viewport to be the entire size of the window
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();

   // keep our logical coordinate system constant
   gluOrtho2D(0.0, 160.0, 0.0, 120.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity ();

}

// Función para mover la paleta izquierda (teclas 'W' y 'S')
void keyboard(unsigned char key, int x, int y) {
    const double step = 30.0;
    if (!gameStarted) {
        // Si aún no comenzó el juego, presionar espacio lo inicia
        if (key == ' ') {
            gameStarted = true;
            return;
        }
    }
    switch(key) {
        case 'w':
        case 'W':
            if (leftPaddleY + paddleHeight + step <= GAME_HEIGHT)
                leftPaddleY += step;
            break;
        case 's':
        case 'S':
            if (leftPaddleY - step >= 0)
                leftPaddleY -= step;
            break;
        case 27: // tecla Esc para salir
            exit(0);
            break;
    }
}

// Función para mover la paleta derecha (teclas de flecha arriba/abajo)
void specialKeys(int key, int x, int y) {
    const double step = 30.0;
    switch(key) {
        case GLUT_KEY_UP:
            if (rightPaddleY + paddleHeight + step <= GAME_HEIGHT)
                rightPaddleY += step;
            break;
        case GLUT_KEY_DOWN:
            if (rightPaddleY - step >= 0)
                rightPaddleY -= step;
            break;
    }
}


void init(void){
  //set the clear color to be white
  glClearColor(0.0,0.8,0.0,1.0);
  // initial position set to 0,0
  xpos = 60; ypos = RadiusOfBall; xdir = 1; ydir = 1;
  sx = 1.; sy = 1.; squash = 0.9;
  rot = 0; 

}


int main(int argc, char* argv[])
{

  glutInit( & argc, argv );
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize (320, 240);   
  glutCreateWindow("Pong");
  init();
  glutDisplayFunc(Display);
  glutReshapeFunc(reshape);
  glutMainLoop();

  return 1;
}