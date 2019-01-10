#include <GL/gl.h>
#include <GL/glut.h>
#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * ~ Rubik's cube ~
 * Select rows with keys 1-3 on the keyboard
 * Select columns with keys 8-0 on the keyboard
 * Press j to move the selected body left or down
 * Cube starts as solved
 * For exercise 4 - toggle comment on line 428
 * For exercise 5 - toggle comment on line 509
*/

using glm::vec3;
using std::cout;
using std::endl;

const int cubeSides = 6;
const int cubePieces = 9;

char view = 'w';
int selected = -1;
GLuint tex;

//The cube starts out solved
int matrix[cubeSides][cubePieces] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0}, /*  0 = red */
    {1, 1, 1, 1, 1, 1, 1, 1, 1}, /*  1 = yellow */
    {2, 2, 2, 2, 2, 2, 2, 2, 2}, /*  2 = blue */
    {3, 3, 3, 3, 3, 3, 3, 3, 3}, /*  3 = white */
    {4, 4, 4, 4, 4, 4, 4, 4, 4}, /*  4 = green */
    {5, 5, 5, 5, 5, 5, 5, 5, 5}  /*  5 = orange */
};

void view0() {
  gluLookAt(5.0, 5.0, 10.0, //Camera position in World Space
            0.0, 0.0, 0.0,  //Camera looks towards this position
            0.0, 1.0, 0.0); //Which direction is up
}

void view1() {
  glRotatef(-90, 1.0f, 0.0f, 0.0f);
}

void view2() {
  glRotatef(90, 1.0f, 0.0f, 0.0f);
}

void view3() {
  glRotatef(90, 0.0f, 1.0f, 0.0f);
}

void view4() {
  glRotatef(-90, 0.0f, 1.0f, 0.0f);
}

vec3 getColor(int side) {
  vec3 currCol;
  switch (side) {
  case 0:
    currCol = vec3(1.0f, 0.0f, 0.0f); //red
    break;
  case 1:
    currCol = vec3(1.0f, 1.0f, 0.0f); //yellow
    break;
  case 2:
    currCol = vec3(0.0f, 0.0f, 1.0f); //blue
    break;
  case 3:
    currCol = vec3(1.0f, 1.0f, 1.0f); //white
    break;
  case 4:
    currCol = vec3(0.0f, 1.0f, 0.0f); //green
    break;
  case 5:
    currCol = vec3(1.0f, 0.5f, 0.0f); //orange
    break;
  default:
    currCol = vec3(0.0f, 0.0f, 0.0f); //black to indicate error
  }
  return currCol;
}

//rotate selected row to the left or column down
void moveLeftDown() {
  //make a copy of the current matrix
  int mCopy[cubeSides][cubePieces];
  std::copy(&matrix[0][0], &matrix[0][0] + cubeSides * cubePieces, &mCopy[0][0]);

  if (selected == 1) { //top row rotation
    for (int c = 0; c < 5; c++) {
      if (c == 0) { //blue -> red
        mCopy[0][0] = matrix[2][2];
        mCopy[0][1] = matrix[2][5];
        mCopy[0][2] = matrix[2][8];
      } else if (c == 1) { //orange -> blue
        mCopy[2][2] = matrix[5][0];
        mCopy[2][5] = matrix[5][3];
        mCopy[2][8] = matrix[5][6];
      } else if (c == 2) { //white -> white
        mCopy[3][0] = matrix[3][6];
        mCopy[3][1] = matrix[3][3];
        mCopy[3][2] = matrix[3][0];
        mCopy[3][3] = matrix[3][7];
        mCopy[3][5] = matrix[3][1];
        mCopy[3][6] = matrix[3][8];
        mCopy[3][7] = matrix[3][5];
        mCopy[3][8] = matrix[3][2];
      } else if (c == 3) { //red -> green
        mCopy[4][0] = matrix[0][0];
        mCopy[4][3] = matrix[0][1];
        mCopy[4][6] = matrix[0][2];
      } else { //green -> orange
        mCopy[5][0] = matrix[4][0];
        mCopy[5][3] = matrix[4][3];
        mCopy[5][6] = matrix[4][6];
      }
    }
  } else if (selected == 2) { //middle row rotation
    for (int c = 0; c < 4; c++) {
      if (c == 0) { //blue -> red
        mCopy[0][3] = matrix[2][1];
        mCopy[0][4] = matrix[2][4];
        mCopy[0][5] = matrix[2][7];
      } else if (c == 1) { //orange -> blue
        mCopy[2][1] = matrix[5][7];
        mCopy[2][4] = matrix[5][4];
        mCopy[2][7] = matrix[5][1];
      } else if (c == 3) { //red -> green
        mCopy[4][1] = matrix[0][5];
        mCopy[4][4] = matrix[0][4];
        mCopy[4][7] = matrix[0][3];
      } else { //green -> orange
        mCopy[5][7] = matrix[4][7];
        mCopy[5][4] = matrix[4][4];
        mCopy[5][1] = matrix[4][1];
      }
    }
  } else if (selected == 3) { //bottom row rotation
    for (int c = 0; c < 6; c++) {
      if (c == 0) { //blue -> red
        mCopy[0][6] = matrix[2][0];
        mCopy[0][7] = matrix[2][3];
        mCopy[0][8] = matrix[2][6];
      } else if (c == 2) { //orange ->blue
        mCopy[2][0] = matrix[5][8];
        mCopy[2][3] = matrix[5][5];
        mCopy[2][6] = matrix[5][2];
      } else if (c == 3) { //yellow -> yellow
        mCopy[1][0] = matrix[1][2];
        mCopy[1][1] = matrix[1][5];
        mCopy[1][2] = matrix[1][8];
        mCopy[1][3] = matrix[1][1];
        mCopy[1][5] = matrix[1][7];
        mCopy[1][6] = matrix[1][0];
        mCopy[1][7] = matrix[1][3];
        mCopy[1][8] = matrix[1][6];

      } else if (c == 4) { //red -> green
        mCopy[4][8] = matrix[0][6];
        mCopy[4][5] = matrix[0][7];
        mCopy[4][2] = matrix[0][8];
      } else { //green -> orange
        mCopy[5][8] = matrix[4][8];
        mCopy[5][5] = matrix[4][5];
        mCopy[5][2] = matrix[4][2];
      }
    }
  } else if (selected == 8) { //left column rotation
    for (int c = 0; c < 5; c++) {
      if (c == 0) { //white -> red
        mCopy[0][0] = matrix[3][8];
        mCopy[0][3] = matrix[3][5];
        mCopy[0][6] = matrix[3][2];
      } else if (c == 1) { //orange -> white
        mCopy[3][2] = matrix[5][0];
        mCopy[3][5] = matrix[5][1];
        mCopy[3][8] = matrix[5][2];
      } else if (c == 2) { //yellow -> orange
        mCopy[5][0] = matrix[1][6];
        mCopy[5][1] = matrix[1][3];
        mCopy[5][2] = matrix[1][0];
      } else if (c == 3) { //red -> yellow
        mCopy[1][0] = matrix[0][0];
        mCopy[1][3] = matrix[0][3];
        mCopy[1][6] = matrix[0][6];
      } else { //green -> green
        mCopy[4][0] = matrix[4][6];
        mCopy[4][1] = matrix[4][3];
        mCopy[4][2] = matrix[4][0];
        mCopy[4][3] = matrix[4][7];
        mCopy[4][5] = matrix[4][1];
        mCopy[4][6] = matrix[4][8];
        mCopy[4][7] = matrix[4][5];
        mCopy[4][8] = matrix[4][2];
      }
    }
  } else if (selected == 9) { //middle column rotation
    for (int c = 0; c < 4; c++) {
      if (c == 0) { //white -> red
        mCopy[0][1] = matrix[3][7];
        mCopy[0][4] = matrix[3][4];
        mCopy[0][7] = matrix[3][1];
      } else if (c == 1) { //orange -> white
        mCopy[3][1] = matrix[5][3];
        mCopy[3][4] = matrix[5][4];
        mCopy[3][7] = matrix[5][5];
      } else if (c == 3) { //yellow -> orange
        mCopy[5][5] = matrix[1][1];
        mCopy[5][4] = matrix[1][4];
        mCopy[5][3] = matrix[1][7];
      } else { //red -> yellow
        mCopy[1][7] = matrix[0][7];
        mCopy[1][4] = matrix[0][4];
        mCopy[1][1] = matrix[0][1];
      }
    }
  } else if (selected == 0) { //right column rotation
    for (int c = 0; c < 6; c++) {
      if (c == 0) { //white -> red
        mCopy[0][2] = matrix[3][6];
        mCopy[0][5] = matrix[3][3];
        mCopy[0][8] = matrix[3][0];
      } else if (c == 2) { //orange -> white
        mCopy[3][6] = matrix[5][8];
        mCopy[3][3] = matrix[5][7];
        mCopy[3][0] = matrix[5][6];
      } else if (c == 3) { //yellow -> orange
        mCopy[5][8] = matrix[1][2];
        mCopy[5][7] = matrix[1][5];
        mCopy[5][6] = matrix[1][8];
      } else if (c == 4) { //red -> yellow
        mCopy[1][2] = matrix[0][2];
        mCopy[1][5] = matrix[0][5];
        mCopy[1][8] = matrix[0][8];
      } else { //blue -> blue
        mCopy[2][0] = matrix[2][2];
        mCopy[2][1] = matrix[2][5];
        mCopy[2][2] = matrix[2][8];
        mCopy[2][3] = matrix[2][1];
        mCopy[2][5] = matrix[2][7];
        mCopy[2][6] = matrix[2][0];
        mCopy[2][7] = matrix[2][3];
        mCopy[2][8] = matrix[2][6];
      }
    }
  }

  //copy back to original matrix
  std::copy(&mCopy[0][0], &mCopy[0][0] + cubeSides * cubePieces, &matrix[0][0]);
}

//TODO: rotate selected row to the right or up
void moveRightUp() {
  //opposite of moveLeftDown()
}

void keyPressed(unsigned char key, int x, int y) {
  switch (key) {
  case 'w':
    glClear(GL_COLOR_BUFFER_BIT);
    view1();
    view = 'w';
    break;
  case 'a':
    glClear(GL_COLOR_BUFFER_BIT);
    view4();
    view = 'a';
    break;
  case 's':
    glClear(GL_COLOR_BUFFER_BIT);
    view2();
    view = 's';
    break;
  case 'd':
    glClear(GL_COLOR_BUFFER_BIT);
    view3();
    view = 'd';
    break;
  case 27:
    exit(1);
    break;
  case '1':
    selected = 1;
    cout << "Selected top row" << endl;
    break;
  case '2':
    selected = 2;
    cout << "Selected middle row" << endl;
    break;
  case '3':
    selected = 3;
    cout << "Selected bottom row" << endl;
    break;
  case '8':
    selected = 8;
    cout << "Selected left column" << endl;
    break;
  case '9':
    selected = 9;
    cout << "Selected middle column" << endl;
    break;
  case '0':
    selected = 0;
    cout << "Selected right column" << endl;
    break;
  case 'j':
    if (selected > 0 && selected < 4) {
      cout << "Moving row left" << endl;
      moveLeftDown();
    } else if (selected > 7 || selected == 0) {
      cout << "Moving column down" << endl;
      moveLeftDown();
    } else {
      cout << "Nothing selected. Select a row (1-3) or column (8-0)" << endl;
    }
    break;
  case 'l':
    if (selected != -1 && selected < 4) {
      cout << "Moving row right" << endl;
      moveRightUp();
    } else if (selected > 7 && selected == 0) {
      cout << "Moving column up" << endl;
      moveLeftDown();
    } else {
      cout << "Nothing selected. Select a row (1-3) or column (8-0)" << endl;
    }
    break;
  default:
    break;
  }

  glutPostRedisplay();
}

void drawSquare() {
  glPolygonMode(GL_FRONT, GL_FILL);
  glBegin(GL_POLYGON);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(1.0f, 0.0f, 0.0f);
  glVertex3f(1.0f, -1.0f, 0.0f);
  glVertex3f(0.0f, -1.0f, 0.0f);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glEnd();

  glFlush();
}

//Like the drawSquare() function but with texture enabled
void drawTexture() {
  glPolygonMode(GL_FRONT, GL_FILL);
  glBegin(GL_POLYGON);
  glTexCoord2f(0.0, 0.0);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glTexCoord2f(0.0, 1.0);

  glVertex3f(1.0f, 0.0f, 0.0f);
  glTexCoord2f(1.0, 1.0);
  glVertex3f(1.0f, -1.0f, 0.0f);
  glTexCoord2f(1.0, 0.0);

  glVertex3f(0.0f, -1.0f, 0.0f);
  glTexCoord2f(0.0, 0.0);
  glVertex3f(0.0f, 0.0f, 0.0f);

  glEnd();
  glFlush();
}

void initLight() {
  //material
  GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat mat_shininess[] = {50.0};

  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

  //light
  GLfloat light_color[] = {0.5, 0.0, 0.5, 0.0}; //Adds purple light
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_color);

  glShadeModel(GL_SMOOTH);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);
}

void initTexture() {
  //Configure texture parameters
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                  GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_NEAREST);
  //Import image
  int width, height;
  unsigned char *image =
      SOIL_load_image("magma.jpg", &width, &height, 0, SOIL_LOAD_RGB);

  if (image) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, image);
  } else {
    cout << "Failed to load texture" << endl;
  }
  SOIL_free_image_data(image); //Clean up the image
}

void init(void) {
  glClearColor(0.5, 0.5, 0.5, 0.0);
  glShadeModel(GL_FLAT);
  //glLoadIdentity();
  gluPerspective(60.0, 4.0 / 3.0, 0.1, 100.0); //Belongs in view, not here

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);

  //initLight(); //Exercise 4 - toggle to see
  initTexture(); //Exercise 5 - optional when viewing other exercises
}

void displaySides() {
  for (int c = 0; c < cubeSides; c++) {
    glPushMatrix();
    glTranslatef(-2.5f, 1.5f, 1.5f); //top left from centre
    for (int d = 0; d < cubePieces; d++) {
      vec3 col = getColor(matrix[c][d]);
      glColor3f(col.x, col.y, col.z);
      if (d != 0 && d % 3 == 0) {
        glTranslatef(-2.0f, -1.0f, 0.0f); //row end
        drawSquare();
      } else {
        glTranslatef(1.0f, 0.0f, 0.0f);
        drawSquare();
      }
    }
    glPopMatrix();

    //Rotate
    switch (c) {
    case 0:
      glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
      break;
    case 4:
      glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
      break;
    default:
      glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
      break;
    }
  }
}

void displaySidesWTex() {
  //Using the texture
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glBindTexture(GL_TEXTURE_2D, tex);

  for (int c = 0; c < cubeSides; c++) {
    glPushMatrix();
    glTranslatef(-2.5f, 1.5f, 1.5f); //top left from centre
    for (int d = 0; d < cubePieces; d++) {
      vec3 col = getColor(matrix[c][d]);
      glColor3f(col.x, col.y, col.z);
      if (d != 0 && d % 3 == 0) {
        glTranslatef(-2.0f, -1.0f, 0.0f); //row end
        drawTexture();
      } else {
        glTranslatef(1.0f, 0.0f, 0.0f);
        drawTexture();
      }
    }
    glPopMatrix();

    //Rotate
    switch (c) {
    case 0:
      glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
      break;
    case 4:
      glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
      break;
    default:
      glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
      break;
    }
  }
}

void displaySolution(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix();

  //Draw here
  displaySides(); //Toggle this off for exercise 5
  //displaySidesWTex(); //Exercise 5 - toggle on
  glPopMatrix();

  glFlush();
  glutSwapBuffers();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 30.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -10);
}

int main(int argc, char **argv) {
  std::cout << "Use numbers 1-3 and 8-0 to select rows or columns" << std::endl;
  std::cout << "Use wasd to rotate the cube and 'j' to move selected rows and columns" << std::endl;

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(800, 600);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Exercise 3 - Rubik's cube");
  init();
  glutDisplayFunc(displaySolution);
  view0();
  glutKeyboardFunc(keyPressed);
  glutMainLoop();
  return 0;
}
