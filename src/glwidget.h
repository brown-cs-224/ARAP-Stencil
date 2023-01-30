#pragma once

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include "arap.h"
#include "graphics/camera.h"
#include "graphics/shader.h"

#include <QOpenGLWidget>
#include <QElapsedTimer>
#include <QTimer>
#include <memory>

class GLWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = nullptr);
    ~GLWidget();

private:
    static const int FRAMES_TO_AVERAGE = 30;

private:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyRepeatEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    Eigen::Vector3f transformToWorldRay(int x, int y);

private:
    QWidget *m_window;
    QElapsedTimer m_time;
    QTimer m_timer;

    GLuint m_surfaceFbo;
    GLuint depthBuffer;
    GLuint frameTexture;

    ARAP m_arap;
    Camera     m_camera;
    Shader *m_defaultShader;
    Shader *m_pointShader;

    int m_forward, m_sideways, m_vertical;
    Eigen::Vector3f m_move;
    float m_vSize;

    int m_lastX, m_lastY;

    bool m_capture;

    int m_lastSelected = -1;

private slots:

    // Physics Tick
    void tick();
};
