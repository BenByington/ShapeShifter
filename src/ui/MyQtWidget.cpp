/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   MyQtWidget.cpp
 * Author: bbyington
 *
 * Created on July 24, 2016, 8:59 AM
 */

#include "ui/MyQtWidget.h"
#include "opengl/Shader.h"

#include <QDebug>
#include <QOpenGLContext>

MyQtWidget::MyQtWidget() {
  widget.setupUi(this);

}

MyQtWidget::~MyQtWidget() {
}

void MyQtWidget::initializeGL() {

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

std::cerr << "valid: " << this->context()->isValid() << std::endl;
QString versionString(QLatin1String(reinterpret_cast<const char*>(glGetString(GL_VERSION))));
qDebug() << "Driver Version String:" << versionString;
qDebug() << "Current Context:" << this->format();

  float tri_vert[] = {
     0.0f, .5f, 0.0f,
    .5f,-.5f, 0.0f,
     -.5f,-.5f, 0.0f};
  float tri_col[] = {
     1.0f, 1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,
     1.0f, 1.0f, 1.0f};

  GLuint points_vbo = 0;
  glGenBuffers (1, &points_vbo);
  glBindBuffer (GL_ARRAY_BUFFER, points_vbo);
  glBufferData (GL_ARRAY_BUFFER, 9 * sizeof (float), tri_vert, GL_STATIC_DRAW);

	GLuint colours_vbo = 0;
  glGenBuffers (1, &colours_vbo);
  glBindBuffer (GL_ARRAY_BUFFER, colours_vbo);
  glBufferData (GL_ARRAY_BUFFER, 9 * sizeof (float), tri_col, GL_STATIC_DRAW);

  glGenVertexArrays (1, &vao);
  glBindVertexArray (vao);
  glBindBuffer (GL_ARRAY_BUFFER, points_vbo);
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glBindBuffer (GL_ARRAY_BUFFER, colours_vbo);
  glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	
	glEnableVertexAttribArray (0);
  glEnableVertexAttribArray (1);
  // float quad_vert[] = {
  //      -1.0f, 1.0f, 0.0f,
  //       1.0f, 1.0f, 0.0f,
  //       1.0f,-1.0f, 0.0f,
  //      -1.0f,-1.0f, 0.0f};
	
	ShapeShifter::Opengl::VertexShader("/Users/bbyington/ShapeShifter/shaders/vertex/BasicVertexShader.vert");
	ShapeShifter::Opengl::FragmentShader("/Users/bbyington/ShapeShifter/shaders/vertex/BasicFragmetShader.frag");
}

void MyQtWidget::gluPerspective(double fovy,double aspect, double zNear, double zFar) {
    // Start in projection mode.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double xmin, xmax, ymin, ymax;
    ymax = zNear * tan(fovy * M_PI / 360.0);
    ymin = -ymax;
    xmin = ymin * aspect;
    xmax = ymax * aspect;
    glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}

void MyQtWidget::resizeGL(int width, int height) {
    height = height?height:1;

    glViewport( 0, 0, (GLint)width, (GLint)height );

    gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void MyQtWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(-1.5f,0.0f,-6.0f);
      
		//glBindVertexArray (vao);
    //glDrawArrays (GL_TRIANGLES, 0, 3);

    glBegin(GL_TRIANGLES);
        glVertex3f( 0.0f, 1.0f, 0.0f);
        glVertex3f(-1.0f,-1.0f, 0.0f);
        glVertex3f( 1.0f,-1.0f, 0.0f);
    glEnd(); 

    glTranslatef(3.0f,0.0f,0.0f);

    glBegin(GL_QUADS);
        glVertex3f(-1.0f, 1.0f, 0.0f);
        glVertex3f( 1.0f, 1.0f, 0.0f);
        glVertex3f( 1.0f,-1.0f, 0.0f);
        glVertex3f(-1.0f,-1.0f, 0.0f);
    glEnd(); 
}
