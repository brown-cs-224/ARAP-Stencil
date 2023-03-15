#include "glwidget.h"

#include <QApplication>
#include <QKeyEvent>
#include <iostream>

#define SPEED 1.5
#define ROTATE_SPEED 0.0025

using namespace std;

GLWidget::GLWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    m_deltaTimeProvider(),
    m_intervalTimer(),
    m_arap(),
    m_camera(),
    m_defaultShader(),
    m_pointShader(),
    m_forward(),
    m_sideways(),
    m_vertical(),
    m_lastX(),
    m_lastY(),
    m_capture(false)
{
    // GLWidget needs all mouse move events, not just mouse drag events
    setMouseTracking(true);

    // Hide the cursor since this is a fullscreen app
    QApplication::setOverrideCursor(Qt::ArrowCursor);

    // GLWidget needs keyboard focus
    setFocusPolicy(Qt::StrongFocus);

    // Function tick() will be called once per interva
    connect(&m_intervalTimer, SIGNAL(timeout()), this, SLOT(tick()));
}

GLWidget::~GLWidget()
{
    if (m_defaultShader != nullptr) delete m_defaultShader;
    if (m_pointShader   != nullptr) delete m_pointShader;
}

// ================== Basic OpenGL Overrides

void GLWidget::initializeGL()
{
    // Initialize GL extension wrangler
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) fprintf(stderr, "Error while initializing GLEW: %s\n", glewGetErrorString(err));
    fprintf(stdout, "Successfully initialized GLEW %s\n", glewGetString(GLEW_VERSION));

    // Set clear color to white
    glClearColor(1, 1, 1, 1);

    // Enable depth-testing and backface culling
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Initialize the shader and simulation
    m_defaultShader = new Shader(":resources/shaders/shader.vert",      ":resources/shaders/shader.frag");
    m_pointShader   = new Shader(":resources/shaders/anchorPoint.vert", ":resources/shaders/anchorPoint.geom", ":resources/shaders/anchorPoint.frag");


    Eigen::Vector3f min, max, center, range, position;
    m_arap.init(min,max);
    center = (min + max) / 2.;
    range = max - min;

    m_vSize = range.maxCoeff() * 7.5f;

    float yLength = std::max(range[0] / static_cast<float>(width()) * height(),range[1]);
    float fovY = 120.;
    float epsilon = 0.1;
    float zLength = yLength / 2. / tanf(fovY / 2.) * (1. + epsilon);
    Eigen::Vector3f near = center - range;
    Eigen::Vector3f far = center + range;
    position = center - Eigen::Vector3f::UnitZ() * (zLength + range[2]);

    m_move = range * 0.8;

    // Initialize camera with a reasonable transform
    Eigen::Vector3f eye    = {0, 2, -5};
    Eigen::Vector3f target = {0, 1,  0};
    m_camera.lookAt(eye, target);
    m_camera.setOrbitPoint(target);
    m_camera.setPerspective(120, width() / static_cast<float>(height()), 0.1, 50);

    m_deltaTimeProvider.start();
    m_intervalTimer.start(1000 / 60);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_defaultShader->bind();
    m_defaultShader->setUniform("proj", m_camera.getProjection());
    m_defaultShader->setUniform("view", m_camera.getView());
    m_arap.draw(m_defaultShader, GL_TRIANGLES);
    m_defaultShader->unbind();

    m_pointShader->bind();
    m_pointShader->setUniform("proj",   m_camera.getProjection());
    m_pointShader->setUniform("view",   m_camera.getView());
    m_pointShader->setUniform("vSize",  m_vSize);
    m_pointShader->setUniform("width",  width());
    m_pointShader->setUniform("height", height());
    m_arap.draw(m_pointShader, GL_POINTS);
    m_pointShader->unbind();
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    m_camera.setAspect(static_cast<float>(w) / h);
}

// ================== Event Listeners

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    float x = event->x();
    float y = event->y();

    Eigen::Vector3f ray = transformToWorldRay(x, y);

    int closest_vertex = m_arap.getClosestVertex(m_camera.getPosition(), ray);

    if (closest_vertex != -1) {
        if (event->button() == Qt::MouseButton::RightButton){
            //right click
            m_arap.select(m_pointShader, closest_vertex);
            m_lastSelected = -1;
        } else {
            if(m_lastSelected == closest_vertex) {
                //left click on the same point
                     m_lastSelected = -1;
            } else {
                //left click on new point
                    m_lastSelected = closest_vertex;
           }
        }
    }

    m_capture = true;
    m_lastX = event->position().x();
    m_lastY = event->position().y();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_lastSelected != -1){
        Eigen::Vector3f ray = transformToWorldRay(event->x(), event->y());

        Eigen::Vector3f pos;

        if (m_arap.getAnchorPos(m_lastSelected, pos, ray, m_camera.getPosition())) {
            m_arap.move(m_lastSelected, pos);
        } else{
            int deltaX = event->x() - m_lastX;
            int deltaY = event->y() - m_lastY;

            if(m_capture) {
                if(deltaX != 0 || deltaY != 0) {
                    m_camera.rotate(-deltaX * 0.01f, deltaY * 0.01f);
                }
            }

            m_lastX = event->x();
            m_lastY = event->y();
        }
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_capture = false;
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    float zoom = 1 - event->pixelDelta().y() * 0.1f / 120.f;
    m_camera.zoom(zoom);
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;

    switch (event->key())
    {
    case Qt::Key_W: m_forward  += SPEED; break;
    case Qt::Key_S: m_forward  -= SPEED; break;
    case Qt::Key_A: m_sideways -= SPEED; break;
    case Qt::Key_D: m_sideways += SPEED; break;
    case Qt::Key_F: m_vertical -= SPEED; break;
    case Qt::Key_R: m_vertical += SPEED; break;
    case Qt::Key_C: m_camera.toggleIsOrbiting(); break;
    case Qt::Key_T: m_arap.toggleWire(); break;
    case Qt::Key_Escape: QApplication::quit();
    }
}

void GLWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;

    switch (event->key())
    {
    case Qt::Key_W: m_forward  -= SPEED; break;
    case Qt::Key_S: m_forward  += SPEED; break;
    case Qt::Key_A: m_sideways += SPEED; break;
    case Qt::Key_D: m_sideways -= SPEED; break;
    case Qt::Key_F: m_vertical += SPEED; break;
    case Qt::Key_R: m_vertical -= SPEED; break;
    }
}

// ================== Physics Tick

void GLWidget::tick()
{
    float deltaSeconds = m_deltaTimeProvider.restart() / 1000.f;

    // Move camera
    auto look = m_camera.getLook();
    look.y() = 0;
    look.normalize();
    Eigen::Vector3f perp(-look.z(), 0, look.x());
    Eigen::Vector3f moveVec = m_forward * look.normalized() + m_sideways * perp.normalized() + m_vertical * Eigen::Vector3f::UnitY();
    moveVec *= deltaSeconds;
    m_camera.move(moveVec);

    // Flag this view for repainting (Qt will call paintGL() soon after)
    update();
}

// ================== Helper

Eigen::Vector3f GLWidget::transformToWorldRay(int x, int y)
{
    Eigen::Vector4f clipCoords = Eigen::Vector4f(
        (float(x) / width()) * 2.f - 1.f,
        1.f - (float(y) / height()) * 2.f, -1.f, 1.f
    );

    Eigen::Vector4f transformed_coords = m_camera.getProjection().inverse() * clipCoords;
    transformed_coords = Eigen::Vector4f(transformed_coords.x(), transformed_coords.y(), -1.f, 0.f);
    transformed_coords = m_camera.getView().inverse() * transformed_coords;

    return Eigen::Vector3f(transformed_coords.x(), transformed_coords.y(), transformed_coords.z()).normalized();
}
