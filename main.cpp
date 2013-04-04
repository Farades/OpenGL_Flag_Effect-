#include<windows.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <GL/glu.h>
#include <GL/gl.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#include <iostream>
#include <math.h>

float points[ 45 ][ 45 ][3]; // массив точек сетки нашей "волны"
int wiggle_count = 0;        // счетчик для контроля быстроты развевания флага
GLfloat hold;                // временно содержит число с плавающей запятой

#define ANGLE_ROTATION 0.2
#define CALL_TIMER     30
#define TEX_IMAGE_NAME "D:/workspace/OpenGL_Flag_Effect-/test.jpg"

float WinWidth       = 1200.0;
float WinHeight      = 900.0;

GLuint	texture[1];		// Место для одной текстуры

GLfloat	x_rotation = 50.;
GLfloat	y_rotation = 50.;
GLfloat	z_rotation = 50.;

//-----------------------------------------------------------------------------
//Таймер вращения
static void timer_rotation(int value = 0)
{
    x_rotation += ANGLE_ROTATION;
    y_rotation += ANGLE_ROTATION;
    z_rotation += ANGLE_ROTATION;

  /* send redisplay event */
  glutPostRedisplay();

  /* call this function again in 30 milliseconds */
  glutTimerFunc(CALL_TIMER, timer_rotation, 0);
}

//-----------------------------------------------------------------------------
//Функция загрузки изображения текстуры
void Load_Tex_Image()
{
    int width, height, bpp;

    ilLoad(IL_JPG, reinterpret_cast<const ILstring>(TEX_IMAGE_NAME));
    int err = ilGetError();                          // Считывание кода ошибки
    if (err != IL_NO_ERROR)
    {
        const char* strError = iluErrorString(err);  // Считываем строку ошибки
        std::cout << "Error load texture image: " << strError << std::endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "Load texture image completed!" << std::endl;
        width  = ilGetInteger(IL_IMAGE_WIDTH);
        height = ilGetInteger(IL_IMAGE_HEIGHT);
        bpp    = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
        std::cout << "width:  "<< width << std::endl << "height: "
                  << height << std::endl << "bpp:    " << bpp << std::endl;
    }

    unsigned char* data = ilGetData();
    unsigned int type;

    switch (bpp) {
    case 1:
      type  = GL_RGB8;
      break;
    case 3:
      type = GL_RGB;
      break;
    case 4:
      type = GL_RGBA;
      break;
    }
    glGenTextures(1, &texture[0]);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0,
    GL_RGB, GL_UNSIGNED_BYTE, data);
}

//-----------------------------------------------------------------------------
//Функция инициализации
void init()
{
    //Инициализация DevIL
    ilInit();
    iluInit();
    ilutInit();

    Load_Tex_Image();

    glEnable(GL_TEXTURE_2D);
    glViewport( 0, 0, WinWidth, WinHeight );
    glClearColor(0.9f, 0.9f, 0.9f, 0.0f);
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( 45.0f, (GLfloat)WinWidth/(GLfloat)WinHeight, 0.1f, 100.0f );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

//    glPolygonMode( GL_BACK, GL_FILL );       // нижняя (задняя) сторона заполнена
//    glPolygonMode( GL_FRONT, GL_LINE );      // верхняя (передняя) сторона прорисована линиями

    for(int x=0; x<45; x++)
    {
        for(int y=0; y<45; y++)
        {
            // применим волну к нашей сетке
            points[x][y][0]=float((x/5.0f)-4.5f);
            points[x][y][1]=float((y/5.0f)-4.5f);
            points[x][y][2]=float(sin((((x/5.0f)*40.0f)/360.0f)*3.141592654*2.0f));
        }
    }
}


//-----------------------------------------------------------------------------
//Функция рисования сцены
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0f,0.0f,-12.0f);

    glRotatef(x_rotation,1.0f,0.0f,0.0f);                     // вращение по оси X
    glRotatef(y_rotation,0.0f,1.0f,0.0f);                     // вращение по оси Y
    glRotatef(z_rotation,0.0f,0.0f,1.0f);                     // вращение по оси Z

    glBindTexture(GL_TEXTURE_2D, texture[0]);

    int x, y;
    float float_x, float_y, float_xb, float_yb;

    glBegin(GL_QUADS);              // начинаем рисовать квадраты
        for( x = 0; x < 44; x++ ) // пройдемся по оси X 0-44 (45 точек)
        {
            for( y = 0; y < 44; y++ ) // пройдемся по оси Y 0-44 (45 точек)
            {
            float_x = float(x)/44.0f;         // создать значение X как float
            float_y = float(y)/44.0f;         // создать значение Y как float
            float_xb = float(x+1)/44.0f;
            float_yb = float(y+1)/44.0f;
            // первая координата текстуры (нижняя левая)
            glTexCoord2f( float_x, float_y);
            glVertex3f( points[x][y][0], points[x][y][1], points[x][y][2] );

            // вторая координата текстуры (верхняя левая)
            glTexCoord2f( float_x, float_yb );
            glVertex3f( points[x][y+1][0], points[x][y+1][1], points[x][y+1][2]);

            // третья координата текстуры (верхняя правая)
            glTexCoord2f( float_xb, float_yb );
            glVertex3f( points[x+1][y+1][0], points[x+1][y+1][1], points[x+1][y+1][2]);

            // четвертая координата текстуры (нижняя правая)
            glTexCoord2f( float_xb, float_y );
            glVertex3f( points[x+1][y][0], points[x+1][y][1], points[x+1][y][2]);
            }
        }
    glEnd();

        for( y = 0; y < 45; y++ )
        {
        // сохраним текущее значение одной точки левой стороны волны
        hold=points[0][y][2];
        for( x = 0; x < 44; x++)
        {
            // текущее значение волны равно значению справа
            points[x][y][2] = points[x+1][y][2];
        }
        // последнее значение берется из дальнего левого сохраненного значения
        points[44][y][2]=hold;
        }


    glFlush();
}

int main(int argc, char ** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WinWidth, WinHeight);
    glutInitWindowPosition(100, 70);
    glutCreateWindow("Texture_test");
    glutDisplayFunc(display);
    init();
    timer_rotation();
    glutMainLoop();
}
