#include "view.h"

#include "viewformat.h"

#include <QApplication>
#include <QKeyEvent>
#include <Eigen/StdVector>
#include <iostream>

using namespace std;

View::View(QWidget *parent) : QGLWidget(ViewFormat(), parent),
    m_window(parent->parentWidget()),
    m_time(), m_timer(),
    m_forward(), m_sideways(), m_vertical(),
    m_lastX(), m_lastY(),
    m_capture(false)
{
    // View needs all mouse move events, not just mouse drag events
    setMouseTracking(true);

    // Hide the cursor since this is a fullscreen app
    QApplication::setOverrideCursor(Qt::ArrowCursor);

    // View needs keyboard focus
    setFocusPolicy(Qt::StrongFocus);

    // The game loop is implemented using a timer
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));
}

View::~View()
{
    delete m_defaultShader;
    delete m_pointShader;

}

void View::initializeGL()
{
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK) {
        std::cerr << "glew initialization failed" << std::endl;
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // alice blue
    glClearColor(240.0f/255.0f, 248.0f/255.0f, 255.0f/255.0f, 1);

    m_defaultShader = new Shader(":/shaders/shader.vert", ":/shaders/shader.frag");
    m_pointShader = new Shader(":/shaders/anchorPoint.vert", ":/shaders/anchorPoint.geom", ":/shaders/anchorPoint.frag");

    m_sim.init();

    m_camera.setPosition(Eigen::Vector3f(0, 0, 10));
    m_camera.lookAt(Eigen::Vector3f(0, 5, -15), Eigen::Vector3f(0, 5, 0), Eigen::Vector3f(0, 1, 0));
    m_camera.setTarget(Eigen::Vector3f(0, 50, 0));
    m_camera.setPerspective(120, width() / static_cast<float>(height()), 0.1, 50);

    m_time.start();
    m_timer.start(1000 / 60);
}

void View::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f p = m_camera.getProjection();
    Eigen::Matrix4f v =  m_camera.getView();
    Eigen::Matrix4f mvp = p * v;

    m_defaultShader->bind();
    m_defaultShader->setUniform("m", model);
    m_defaultShader->setUniform("vp", mvp);
    m_sim.draw(m_defaultShader, GL_TRIANGLES);
    m_defaultShader->unbind();

    m_pointShader->bind();
    m_pointShader->setUniform("m", model);
    m_pointShader->setUniform("vp", mvp);
    m_pointShader->setUniform("width", width());
    m_pointShader->setUniform("height", height());
    m_sim.draw(m_pointShader, GL_POINTS);
    m_pointShader->unbind();
}

void View::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    m_camera.setAspect(static_cast<float>(w) / h);
}


Eigen::Vector3f View::transformToWorldRay(int x, int y) {
    Eigen::Vector4f clipCoords = Eigen::Vector4f(
        (float(x) / m_window->width()) * 2.f - 1.f,
        1.f - (float(y) / m_window->height()) * 2.f, -1.f, 1.f
    );

    Eigen::Vector4f transformed_coords = m_camera.getProjection().inverse() * clipCoords;
    transformed_coords = Eigen::Vector4f(transformed_coords.x(), transformed_coords.y(), -1.f, 0.f);
    transformed_coords = m_camera.getView().inverse() * transformed_coords;

    return Eigen::Vector3f(transformed_coords.x(), transformed_coords.y(), transformed_coords.z()).normalized();
}

void View::mousePressEvent(QMouseEvent *event)
{
    float x = event->x();
    float y = event->y();

    Eigen::Vector3f ray = transformToWorldRay(x, y);

    m_sim.select(m_pointShader, m_camera.getPosition(), ray, event->button() == Qt::MouseButton::RightButton);

    m_capture = true;
    m_lastX = event->x();
    m_lastY = event->y();
}

void View::mouseMoveEvent(QMouseEvent *event)
{
    Eigen::Vector3f ray = transformToWorldRay(event->x(), event->y());
    if(!m_sim.move(ray,  m_camera.getPosition())) {

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

void View::mouseReleaseEvent(QMouseEvent *)
{
    m_capture = false;
}

void View::wheelEvent(QWheelEvent *event)
{
    float zoom = 1 - event->delta() * 0.1f / 120;
    m_camera.zoom(zoom);
}

void View::keyPressEvent(QKeyEvent *event)
{
    // Don't remove this -- helper code for key repeat events
    if(event->isAutoRepeat()) {
        keyRepeatEvent(event);
        return;
    }

    // Feel free to remove this
    if (event->key() == Qt::Key_Escape) QApplication::quit();

    if(event->key() == Qt::Key_C) {
        m_camera.toggleOrbit();
    }
    else if(event->key() == Qt::Key_W) {
        m_forward += 1;
    }
    else if(event->key() == Qt::Key_S) {
        m_forward -= 1;
    }
    else if(event->key() == Qt::Key_A) {
        m_sideways -= 1;
    }
    else if(event->key() == Qt::Key_D) {
        m_sideways += 1;
    }
    else if(event->key() == Qt::Key_Q) {
        m_vertical -= 1;
    }
    else if(event->key() == Qt::Key_E) {
        m_vertical += 1;
    } else if(event->key() == Qt::Key_T) {
        m_sim.toggleWire();
    }
}

void View::keyRepeatEvent(QKeyEvent *)
{
}

void View::keyReleaseEvent(QKeyEvent *event)
{
    // Don't remove this -- helper code for key repeat events
    if(event->isAutoRepeat()) {
        return;
    }

    if(event->key() == Qt::Key_W) {
        m_forward -= 1;
    }
    else if(event->key() == Qt::Key_S) {
        m_forward += 1;
    }
    else if(event->key() == Qt::Key_A) {
        m_sideways += 1;
    }
    else if(event->key() == Qt::Key_D) {
        m_sideways -= 1;
    }
    else if(event->key() == Qt::Key_Q) {
        m_vertical += 1;
    }
    else if(event->key() == Qt::Key_E) {
        m_vertical -= 1;
    }
}

void View::tick()
{
    float seconds = m_time.restart() * 0.001;
    m_sim.update(0.01);

    auto look = m_camera.getLook();
    look.y() = 0;
    look.normalize();
    Eigen::Vector3f perp(-look.z(), 0, look.x());
    Eigen::Vector3f moveVec = m_forward * look + m_sideways * perp + m_vertical * Eigen::Vector3f::UnitY();
    moveVec *= seconds;
    m_camera.move(moveVec);

    // Flag this view for repainting (Qt will call paintGL() soon after)
    update();
}
